/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


/*******************************************************

NAME:           TrigEMClusterContainer.cxx
PACKAGE:        Trigger/TrigEvent/TrigCaloEvent
AUTHORS:        Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:        Keep a container for the important
		output variables from the Calorimeter
		LVL2 egamma Trigger.
                Optionally seed ID Trigger and EF.
                Variables here should be RAW variables
                not corrected.
DATE:           October 17th, 2005

******************************************************/

#include "TrigCaloEvent/TrigEMClusterContainer.h"

using namespace std;

// Simple debug method
void
TrigEMClusterContainer::print ( void ) const {
	std::cout << "TrigEMClusterContainer with ";
	std::cout << size() << " TrigEMClusters" << std::endl;
        for (const TrigEMCluster* c : *this)
		c->print();
}

// Simple debug method using MsgStream
void
TrigEMClusterContainer::print ( MsgStream& log ) const {
	log <<MSG::DEBUG<< "TrigEMClusterContainer with ";
	log <<MSG::DEBUG<< size() << " TrigEMClusters" << endmsg;
        for (const TrigEMCluster* c : *this)
		c->print(log);
}

// End of file TrigEMClusterContainer.cxx
