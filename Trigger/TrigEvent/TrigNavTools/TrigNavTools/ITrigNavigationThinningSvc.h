// -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigNavTool_ITrigNavigationThinningSvc_h
#define TrigNavTool_ITrigNavigationThinningSvc_h
#include "TrigNavTools/TrigNavigationSlimmingTool.h"


class ITrigNavigationThinningSvc {
 public:
  virtual ~ITrigNavigationThinningSvc();

  virtual StatusCode doSlimming( std::vector<uint32_t>& slimmed_and_serialzied ) = 0;
  static const InterfaceID& interfaceID();  
};


#endif
