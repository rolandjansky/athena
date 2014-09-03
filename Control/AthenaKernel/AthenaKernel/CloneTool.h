/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_CLONETOOL_H
#define ATHENAKERNEL_CLONETOOL_H
#include "GaudiKernel/StatusCode.h"
#include <string>
class InterfaceID;
class IAlgTool;
namespace CloneTool {
  ///implementation, experts only
  StatusCode typeless_clone(const IAlgTool& cloned, 
			    const std::string& cloneName,
			    const InterfaceID& cloneID, 
			    IAlgTool*& result);

  /** given a reference to a cloned AlgTool sets a reference to its clone */
  template <class CLONEDTOOL>
  StatusCode clone(const CLONEDTOOL& cloned, 
		   const std::string& cloneName,
		   CLONEDTOOL*& result) {
    return typeless_clone(cloned, cloneName, CLONEDTOOL::interfaceID(), 
			  (IAlgTool*&)result);
  }
}
#endif
