/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPFBASETOOL_H
#define IPFBASETOOL_H

#include "GaudiKernel/IAlgTool.h"

class eflowCaloObjectContainer;

class IPFBaseTool : virtual public IAlgTool {

 public:

  /** Execute method to use eflowCaloObjectContainer */
  virtual StatusCode execute(const eflowCaloObjectContainer&) = 0;

  DeclareInterfaceID(IPFBaseTool,1,0);

};
#endif
