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

int run(const std::string& path, int dd, int sr_min, int sr_max, int dlt) {
  using namespace std;

  const string prefix = path + "/D" + to_string(dd);
  const string filename_output_th = prefix + "_throughput_idv.csv";
  const string filename_output_fi = prefix + "_fairness_idv.csv";

  ofstream ofs_th(filename_output_th);
  ofstream ofs_fi(filename_output_fi);

  if (!ofs_th || !ofs_fi) {
    cerr << "[Error] could not open the output file <"
      << filename_output_th << " | " << filename_output_fi << ">." << endl;
    return 1;
  }

  ofs_th << "distance,";
  ofs_fi << "distance,";
  for (int x = sr_min; x <= sr_max; x += dlt) {
    ofs_th << x << ",";
    ofs_fi << x << ",";
  }
  ofs_th << endl;
  ofs_fi << endl;

  for (int y = sr_min; y <= sr_max; y += dlt) {
    ofs_th << y << ",";
    ofs_fi << y << ",";
    for (int x = sr_min; x <= sr_max; x += dlt) {
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
          const int index_th = items.size() - 3; // get WLAN 4 (MOVABLE)
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
  if (argc < 6) {
    cout << "[Error] a few arguments. -> program [path<maybe dir-name including source files>][distance-distrurbers<int>]"
      << "[search-range-min<int>][search-range-max<int>][stride<int>]" << endl;
    exit(-1);
  } else {
    int dd = atoi(argv[2]);
    int sr_min = atoi(argv[3]);
    int sr_max = atoi(argv[4]);
    int dlt = atoi(argv[5]);
    if (dd <= 0) {
      cout << "[Error] the distance must be a positive integer value." << endl;
      exit(-1);
    }
    run(string(argv[1]), dd, sr_min, sr_max, dlt);
  }
  return 0;
}
