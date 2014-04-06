/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWEVENT_EFLOWCALOCLUSTER_H
#define EFLOWEVENT_EFLOWCALOCLUSTER_H

/********************************************************************

NAME:     eflowCaloCluster.h
PACKAGE:  offline/Reconstruction/eflowEvent

AUTHORS:  Dan Tovey
CREATED:  Feb. 2004

PURPOSE:  Replica cluster work class for eflowRec
          Inherits from CaloCluster but includes non-const access

********************************************************************/

// INCLUDE HEADER FILES:

// baseclass
#include "CaloEvent/CaloCluster.h" 
#include "CaloEvent/CaloCellContainer.h"
#include <iostream>

class CaloCell;

class eflowCaloCluster: public CaloCluster {
  
 public:

  eflowCaloCluster();
  eflowCaloCluster(const CaloCluster* clus);
  eflowCaloCluster(const CaloCluster* clus, bool copyMoments);

  ~eflowCaloCluster();

};

#endif 




