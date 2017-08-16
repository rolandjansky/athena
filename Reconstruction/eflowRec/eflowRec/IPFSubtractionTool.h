/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPFSUBTRACTIONALGTOOL_H
#define IPFSUBTRACTIONALGTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

class eflowCaloObjectContainer;
class eflowRecTrackContainer;
class eflowRecClusterContainer;

class IPFSubtractionTool : virtual public IAlgTool {

 public:

  virtual StatusCode intialize() {return StatusCode::SUCCESS;}
  virtual void execute(eflowCaloObjectContainer*, eflowRecTrackContainer*, eflowRecClusterContainer*, xAOD::CaloClusterContainer&) = 0;
  virtual StatusCode finalize() {return StatusCode::SUCCESS;}

};

#endif
