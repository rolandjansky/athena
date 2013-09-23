/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector> 
#ifdef __CINT__
#pragma link C++ class vector<vector<int> >;
#else
template class std::vector<std::vector<int> >;
#endif
