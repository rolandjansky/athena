/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPFBASETOOL_H
#define IPFBASETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class eflowCaloObjectContainer;

class IPFBaseTool : virtual public IAlgTool {

 public:

  /** Standard initialize method */
  virtual StatusCode intialize() {return StatusCode::SUCCESS;}
  /** Execute method to use eflowCaloObjectContainer and modify xAOD::CaloClusterContainer */
  virtual void execute(const eflowCaloObjectContainer&, xAOD::CaloClusterContainer&) = 0;
  /** Standard finalize method */
  virtual StatusCode finalize() {return StatusCode::SUCCESS;}

};
#endif
