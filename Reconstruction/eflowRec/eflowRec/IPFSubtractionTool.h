/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPFSUBTRACTIONTOOL_H
#define IPFSUBTRACTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

class eflowCaloObjectContainer;
class eflowRecTrackContainer;
class eflowRecClusterContainer;

class IPFSubtractionTool : virtual public IAlgTool {

 public:

  /** Execute method to modify calorimeter clusters, when performing charegd shower subtraction procedures in particle flow  */
  virtual void execute(eflowCaloObjectContainer*, eflowRecTrackContainer*, eflowRecClusterContainer*, xAOD::CaloClusterContainer&) = 0;

  DeclareInterfaceID(IPFSubtractionTool,1,0);
  
};

#endif
