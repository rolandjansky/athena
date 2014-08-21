/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../test/tcls1.h"
#include "../test/tcls1.cxx"

struct RootConversionsTest1Dict
{
  std::pair<int, T1> x1;
};


#include "RVersion.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,23,0)
// only needed for old root version...
#undef __P
#undef __N
#endif
