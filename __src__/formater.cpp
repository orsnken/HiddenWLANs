#include <cstdlib>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string& s, char delim) {
  using namespace std;
  vector<string> elems;
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
    if (!item.empty()) {
      elems.push_back(item);
    }
  }
  return elems;
}

int run(const std::string& path, int dd) {
  using namespace std;

  const string prefix = path + "/D" + to_string(dd);
  const string filename_output_th = prefix + "_thrpughput.csv";
  const string filename_output_fi = prefix + "_fairness.csv";

  ofstream ofs_th(filename_output_th);
  ofstream ofs_fi(filename_output_fi);

  if (!ofs_th || !ofs_fi) {
    cerr << "[Error] could not open the output file <"
      << filename_output_th << " | " << filename_output_fi << ">." << endl;
    return 1;
  }

  for (int y = -100; y <= 100; y += 10) {
    for (int x = -100; x <= 100; x += 10) {
      // ----
      // open the input file. [FORMAT: Dxx_Xxx_Yxx.csv]
      const string filename_input = prefix + "_X" + to_string(x) + "_Y" + to_string(y) + ".csv";
      ifstream ifs(filename_input);
      // ----
      // the flow to input data
      // 
      // >> is the file available?
      // >>>> calculate average throughput and fairness
      // 
      if (ifs.fail()) {
        cerr << "[Error] could not open the file <" << filename_input << ">." << endl;
      } else {
        double th_ave = 0.0, fi_ave = 0.0;
        string str;
        int i = 0;
        while (getline(ifs, str)) {
          vector<string> items = split(str, ',');
            // [str FORMAT: SEED,RUN_NUMBER,DATA_1,DATA_2,...,DATA_X,AVERAGE_THROUGHPUT,FAIRNESS]
          const int index_th = items.size() - 2;
          const int index_fi = items.size() - 1;
          th_ave += stod(items[index_th]);
          fi_ave += stod(items[index_fi]);
          i++;
        }
        if (i >= 1) {
          th_ave /= i;
          fi_ave /= i;
        }
        ofs_th << th_ave << ",";
        ofs_fi << fi_ave << ",";
      }
    }
    ofs_th << endl;
    ofs_fi << endl;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  using namespace std;
  if (argc < 3) {
    cout << "[Error] a few arguments. -> program [path<maybe dir-name including source files>][distance-distrurbers<int>]" << endl;
    exit(-1);
  } else {
    int dd = atoi(argv[2]);
    if (dd <= 0) {
      cout << "[Error] the distance must be a positive integer value." << endl;
      exit(-1);
    }
    run(string(argv[1]), dd);
  }
  return 0;
}
