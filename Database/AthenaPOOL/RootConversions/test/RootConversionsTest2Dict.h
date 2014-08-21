/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../test/tcls2.h"
#include "../test/tcls2.cxx"

struct RootConversionsTest2Dict
{
  std::pair<int, T1> x1;
  std::vector<float> x2;
};

#include "RVersion.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,23,0)
// only needed for old root version...
#undef __P
#undef __N
#endif
