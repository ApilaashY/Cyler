import <iostream>;
import <fstream>;
import <string>;
#include <cstdlib> // Required header

import compile;

using namespace std;

string EXECUTECOMMAND =
    "/opt/homebrew/bin/g++-15 -std=c++20 -fmodules-ts -Wall -g";

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    // cout << compile(argv[i]) << endl;
    system((EXECUTECOMMAND + " " + compile(argv[i]) + " -o test").c_str());
    system("./test");
  }

  return 0;
}
