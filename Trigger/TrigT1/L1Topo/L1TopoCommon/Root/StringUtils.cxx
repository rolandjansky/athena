/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoCommon/StringUtils.h"

using namespace std;

string
TCS::join(const vector<string>& v, const char c) {
   string joined{""};
   for(const string & s : v) {
      if(joined!="") joined += c;
      joined += s;
   }
   return joined;
}
