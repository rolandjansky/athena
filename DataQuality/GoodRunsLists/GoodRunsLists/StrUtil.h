/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <vector>

using namespace std;

namespace GRLStrUtil {
  void trim (string& input);
  void split (const string& input, string& first, string& second);
  vector<string> split(string input);
}

