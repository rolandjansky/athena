/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUCLUSTERCONTAINER_H
#define TRIGTAUCLUSTERCONTAINER_H


/*******************************************************

NAME:		TrigTauClusterContainer.h
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHORS:	Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:	Keep a container for the important
		output variables from the Calorimeter
		LVL2 tau Trigger.
		Optionally seed ID Trigger and EF.
		Variables here should be corrected variables
		not raw (in mother class).
DATE:		October 17th, 2005

******************************************************/
// includes section
#include "TrigCaloEvent/TrigTauCluster.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/BaseInfo.h"

/** Container from TrigTauCluster type objects.
        See TrigTauCluster and T2CaloTaus
        for more details */
class TrigTauClusterContainer : public DataVector<TrigTauCluster> {

public:
	/**  calls TrigTauCluster::print (to std::cout)
                for all available clusters   */
        void print ( void ) const;
	/**  calls TrigTauCluster::print (to MsgStream)
                for all available clusters   */
        void print ( MsgStream& log ) const;
private:
};

// CLIDSvc is already loaded in base class
CLASS_DEF ( TrigTauClusterContainer, 1245276288, 1)
CLASS_DEF( DataVector<TrigTauClusterContainer> , 1294524303 , 1 )

SG_BASE(TrigTauClusterContainer, DataVector<TrigTauCluster>);

#endif
