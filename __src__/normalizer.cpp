#include <cmath>
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

int calculate_normalized_v(std::ifstream& ifs, std::vector<double>& res) {
  using namespace std;
  // ####
  // FILE FORMAT -> csv :
  // >> dd x1  x2  x3  x4  ... xn
  // >> y1 v11 v12 v13 v14 ... v1n
  // >> y2 v21 v22 v23 v24 ... v2n
  // >> :                       :
  // >> yn vn1 vn2 vn3 vn4 ... vnn
  // >>
  // ####

  // ----
  // calculate normalized throughput and fairness.
  vector<double> nv;
  double ave = 0.0;
  double s = 0.0;
  int n = 0;
  string str;
  if (!getline(ifs, str)) {
    cerr << "[Warning] the input file is corrupted! maybe it is empty." << endl;
    return 1;
  } else {
    n = split(str, ',').size() - 1;
    if (n <= 0) {
      cerr << "[Warning] the input file is corrupted! maybe it has no-value." << endl;
      return 1;
    }
  }

  // read values and calculate average
  while (getline(ifs, str)) {
    vector<string> items = split(str, ',');
      // [str FORMAT: Yx,V1,V2,...,V_N]
    items.erase(items.begin());
      // Yx should be erased
    for (string& item: items) {
      double v = stod(item);
      ave += v;
      nv.push_back(v);
    }
  }
  ave /= static_cast<double>(nv.size());

  // calculate s
  double tmp = 0.0;
  for (double v: nv) {
    tmp += (v - ave) * (v - ave);
  }
  s = sqrt(tmp / static_cast<double>(nv.size()));

  // calculate normalized value
  for (double v: nv) {
    double w = (v - ave) / s;
    res.push_back(w);
  }

  return n;
}

int output(std::ofstream& ofs, const std::vector<double>& res, int sr_min, int sr_max, int dlt) {
  using namespace std;

  ofs << "distance,";
  for (int x = sr_min; x <= sr_max; x += dlt) {
    ofs << x << ",";
  }
  ofs << endl;
  
  auto itr = res.begin();
  for (int y = sr_min; y <= sr_max; y += dlt) {
    ofs << y << ",";
    for (int x = sr_min; x <= sr_max; x += dlt) {
      ofs << *(itr++) << ",";
    }
    ofs << endl;
  }
  return 0;
}

int run(const std::string& path, int dd, int sr_min, int sr_max, int dlt) {
  using namespace std;

  const string prefix = path + "/D" + to_string(dd);
  const string filename_output = prefix + "_normalized.csv";
  const string filename_output_th = prefix + "_throughput_normalized.csv";
  const string filename_output_fi = prefix + "_fairness_normalized.csv";

  ofstream ofs(filename_output);
  ofstream ofs_th(filename_output_th);
  ofstream ofs_fi(filename_output_fi);

  if (!ofs || !ofs_th || !ofs_fi) {
    cerr << "[Error] could not open the output file <"
      << filename_output << "," << filename_output_th << "," << filename_output_fi << ">." << endl;
    return 1;
  }

  // ----
  // input fairness result
  vector<double> res_fi;
  vector<double> res_th;
  vector<double> res;
  ifstream ifs_fi(prefix + "_fairness.csv");
  ifstream ifs_th(prefix + "_throughput.csv");
  if (ifs_fi.fail() || ifs_th.fail()) {
    cerr << "[Error] could not open the input-file <unknown input-file>." << endl;
    return 1;
  }

  calculate_normalized_v(ifs_th, res_th);
  calculate_normalized_v(ifs_fi, res_fi);

  for (int i = 0, size = res_fi.size(); i < size; i++) {
    res.push_back(res_fi[i] + res_th[i]);
  }

  output(ofs_th, res_th, sr_min, sr_max, dlt);
  output(ofs_fi, res_fi, sr_min, sr_max, dlt);
  output(ofs, res, sr_min, sr_max, dlt);

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
