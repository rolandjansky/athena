/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/IConversionSvc.h"
#include "TrigNavigation/NavigationCore.h"
#include "TrigNavigation/NavigationCore.icc"

#include "TrigNavigation/TriggerElement.h" 
#include "TrigNavigation/AccessProxy.h" 
//#include "TrigNavigation/Holder.h"

struct GCCXML_DUMMY_INSTANTIATION_TRIGNAVIGATION {
  std::vector<HLT::TriggerElement*> te_vec;
  std::vector<const HLT::TriggerElement*> te_const_vec;
  // HLT::NavigationCore nc;
};

