/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUCLUSTERDETAILSCONTAINER_H
#define TRIGTAUCLUSTERDETAILSCONTAINER_H


/*******************************************************

NAME:		TrigTauClusterDetailsContainer.h
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHORS:	M.P. Casado

PURPOSE:	Keep a container for the detailed variables of
                the Calorimeter LVL2 tau Trigger.
DATE:		January 21st, 2009

******************************************************/
// includes section
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/BaseInfo.h"

/** Container from TrigTauClusterDetails type objects.
        See TrigTauClusterDetails and T2CaloTaus
        for more details */
class TrigTauClusterDetailsContainer : public DataVector<TrigTauClusterDetails> {

public:
	/**  calls TrigTauClusterDetails::print (to std::cout)
                for all available clusters   */
        void print ( void ) const;
	/**  calls TrigTauClusterDetails::print (to MsgStream)
                for all available clusters   */
        void print ( MsgStream& log ) const;
private:
};

// CLIDSvc is already loaded in base class
CLASS_DEF ( TrigTauClusterDetailsContainer, 1149551343, 1)
CLASS_DEF( DataVector<TrigTauClusterDetailsContainer> , 1112563054, 1 )

SG_BASE(TrigTauClusterDetailsContainer, DataVector<TrigTauClusterDetails>);

#endif
