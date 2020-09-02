/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPFSUBTRACTIONTOOL_H
#define IPFSUBTRACTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"

class eflowCaloObjectContainer;
class eflowRecTrackContainer;
class eflowRecClusterContainer;

/**
Algorithm base class which inherits from IAlgTool and defines pure virtual execute method which takes pointers to eflowCaloObjectContainer, eflowRecTrackContainer and eflowRecClusterContainer as input. Algtools which inherit from this are used to modify calorimeter clusters, when performing charegd shower subtraction procedures in particle flow. 
*/
class IPFSubtractionTool : virtual public IAlgTool {

 public:

  /** Execute method to modify calorimeter clusters, when performing charegd shower subtraction procedures in particle flow  */
  virtual void execute(eflowCaloObjectContainer*, eflowRecTrackContainer*, eflowRecClusterContainer*) const = 0;

  DeclareInterfaceID(IPFSubtractionTool,1,0);

};
#endif
