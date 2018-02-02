/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOCLUSTERCONTAINER_H
#define TRIGCALOCLUSTERCONTAINER_H


/*******************************************************

NAME:		TrigCaloClusterContainer.h
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHORS:	Denis O. Damazio

PURPOSE:	Keep a container for the important
		output variables from the Calorimeter
		LVL2 Trigger.
		Variables here should be raw variables.
DATE:		May 31st, 2011

******************************************************/
// includes section
#include "TrigCaloEvent/TrigCaloCluster.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/BaseInfo.h"

/** Container from TrigCaloCluster type objects.
	See TrigCaloCluster for more details */
class TrigCaloClusterContainer : public DataVector<TrigCaloCluster> {


public:
	 /**  calls TrigCaloCluster::print (to std::cout)
		for all available clusters   */
        void print ( void ) const;
	 /**  calls TrigCaloCluster::print (to MsgStream)
		for all available clusters   */
        void print ( MsgStream& log ) const;
private:
};

// CLIDSvc is already loaded in base class
CLASS_DEF ( TrigCaloClusterContainer, 1306462450, 1)
CLASS_DEF( DataVector<TrigCaloClusterContainer> , 1108497977 , 1 )

SG_BASE(TrigCaloClusterContainer, DataVector<TrigCaloCluster>);
#endif
