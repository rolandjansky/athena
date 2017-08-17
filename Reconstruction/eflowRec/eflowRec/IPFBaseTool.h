/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPFBASETOOL_H
#define IPFBASETOOL_H

class eflowCaloObjectContainer;
#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class IPFBaseTool : virtual public IAlgTool {

 public:

  virtual StatusCode intialize() {return StatusCode::SUCCESS;}
  virtual void execute(eflowCaloObjectContainer*, xAOD::CaloClusterContainer&) = 0;
  virtual StatusCode finalize() {return StatusCode::SUCCESS;}

};
#endif
