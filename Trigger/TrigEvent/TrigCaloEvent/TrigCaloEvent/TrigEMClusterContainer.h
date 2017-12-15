/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEMCLUSTERCONTAINER_H
#define TRIGEMCLUSTERCONTAINER_H


/*******************************************************

NAME:		TrigEMClusterContainer.h
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHORS:	Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:	Keep a container for the important
		output variables from the Calorimeter
		LVL2 egamma Trigger.
		Optionally seed ID Trigger and EF.
		Variables here should be corrected variables
		not raw (in mother class).
DATE:		October 17th, 2005

******************************************************/
// includes section
#include "TrigCaloEvent/TrigEMCluster.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/BaseInfo.h"

/** Container from TrigEMCluster type objects.
	See TrigEMCluster and T2CaloEgamma
	for more details */
class TrigEMClusterContainer : public DataVector<TrigEMCluster> {

public:
	 /**  calls TrigEMCluster::print (to std::cout)
		for all available clusters   */
        void print ( void ) const;
	 /**  calls TrigEMCluster::print (to MsgStream)
		for all available clusters   */
        void print ( MsgStream& log ) const;
private:
};

// CLIDSvc is already loaded in base class
CLASS_DEF ( TrigEMClusterContainer, 1212891485, 1)
CLASS_DEF( DataVector<TrigEMClusterContainer> , 1331587544 , 1 )

SG_BASE(TrigEMClusterContainer, DataVector<TrigEMCluster>);
#endif
