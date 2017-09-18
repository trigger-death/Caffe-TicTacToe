#include "tictactoe.h"
#include "tictactoe_experiment.h"
#include <caffe/caffe.hpp>
#include <caffe/layers/memory_data_layer.hpp>

void testResults(TicTacToe& ttt, int iterations) {
	std::shared_ptr<caffe::Net<float>> testnet;
	testnet.reset(new caffe::Net<float>("./model.prototxt", caffe::TEST));
	std::string path = ttt.snapshotPath + "/TTT_iter_" + std::to_string(iterations) + ".caffemodel";
	testnet->CopyTrainedLayersFrom(path.c_str());
	
	caffe::MemoryDataLayer<float> *dataLayer_testnet =
			(caffe::MemoryDataLayer<float> *) (testnet->layer_by_name("test_inputdata").get());
			
	std::vector<float> label;
	for (int i = 0; i < ttt.label.size(); i++) {
		label.push_back((ttt.label[i] + 1.0f) / 2.0f);
	}
	
	dataLayer_testnet->Reset(&ttt.data[0], &label[0], ttt.label.size());
	
	testnet->Forward();
	
	boost::shared_ptr<caffe::Blob<float>> output_layer = testnet->blob_by_name("output");
	const float* begin = output_layer->cpu_data();
	const float* end = begin + ttt.label.size();
    
	std::vector<float> results(begin, end);
	
	for (int i = 0; i < ttt.label.size(); i++) {
		results[i] = ((results[i] * 2.0f) - 1.0f);
	}
	
    experiment(ttt, results);
}

int main() {
	// Change working directory to that of program
	char buffer[1024] = { 0 };
	int index = readlink("/proc/self/exe", buffer, 1024);
	buffer[index] = '\0';
	for (int i = index - 1; i >= 0; i--) {
		if (buffer[i] == '/') {
			buffer[i] = '\0';
			break;
		}
	}
	chdir(buffer);
	
	TicTacToe ttt = TicTacToe();
	
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif
	
	caffe::SolverParameter solver_param;
	caffe::ReadSolverParamsFromTextFileOrDie("./solver.prototxt", &solver_param);
	
	std::shared_ptr<caffe::Solver<float>>
			solver(caffe::SolverRegistry<float>::CreateSolver(solver_param));
	
	caffe::MemoryDataLayer<float> *dataLayer_trainnet =
			(caffe::MemoryDataLayer<float> *)(solver->net()->layer_by_name("inputdata").get());
	
	std::vector<float> label;
	for (int i = 0; i < ttt.label.size(); i++) {
		label.push_back((ttt.label[i] + 1.0f) / 2.0f);
	}
	
	dataLayer_trainnet->Reset(&ttt.data[0], &label[0], ttt.label.size());
	
	std::vector<std::pair<int, int>> increments;
	
	// Read settings
	std::ifstream fin("./settings.txt", std::ifstream::in);
	if (fin.is_open()) {
		std::string name;
		fin >> std::boolalpha;
		while (!fin.eof()) {
			fin >> name;
			if (fin.eof())
				break;
			else if (name == "increment") {
				int inc = 10000;
				int incThresh = 0;
				fin >> inc;
				fin >> incThresh;
				increments.push_back(std::pair<int, int>(inc, incThresh));
			}
				
		}
		fin.close();
	}
	
	/*if (increments.empty()) {
		increments.push_back(std::pair<int, int>(1,  100));
		increments.push_back(std::pair<int, int>(100, 1000));
		increments.push_back(std::pair<int, int>(1000, 10000));
		increments.push_back(std::pair<int, int>(10000, 100000));
		increments.push_back(std::pair<int, int>(100000, 5000000));
	}*/
	
	int incIndex = 0;
	int inc = increments[0].first;
	int thresh = increments[0].second;
	int total = increments[increments.size() - 1].second;
	
	for (int i = 0; i < total; i += inc) {
		if (i >= thresh) {
			incIndex++;
			inc = increments[incIndex].first;
			thresh = increments[incIndex].second;
		}
		solver->Step(inc);
		solver->Snapshot();
		testResults(ttt, i + inc);
		std::cout << "Iterations: " << i + inc << std::endl;
	}
	
	return 0;
}
