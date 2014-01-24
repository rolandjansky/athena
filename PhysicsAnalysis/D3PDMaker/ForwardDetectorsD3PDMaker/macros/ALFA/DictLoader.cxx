/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Constants.h"

// loader for dictionary generator
#include <vector>
#include <map>
//using namespace std;

#include "CAlfaEvent.h"

#ifdef __MAKECINT__
#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<int> >+;

#pragma link C++ class map<ULong64_t,DCSBLM>+;
#pragma link C++ class map<ULong64_t,DCSHVCHANNEL>+;
#pragma link C++ class map<ULong64_t,DCSLOCALMONITORING>+;
#pragma link C++ class map<ULong64_t,DCSMOVEMENT>+;
#pragma link C++ class map<ULong64_t,DCSRADMON>+;
#pragma link C++ class map<ULong64_t,DCSTRIGGERRATES>+;
#pragma link C++ class map<ULong64_t,DCSFECONFIGURATION>+;
#pragma link C++ class map<ULong64_t,DCSTRIGGERSETTINGS>+;
#endif
