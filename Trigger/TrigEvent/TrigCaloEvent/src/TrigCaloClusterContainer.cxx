/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/*******************************************************

NAME:           TrigCaloClusterContainer.cxx
PACKAGE:        Trigger/TrigEvent/TrigCaloEvent
AUTHORS:        Denis O. Damazio

PURPOSE:        Keep a container for the important
		output variables from the Calorimeter
		LVL2 Trigger.
                Variables here should be RAW variables.
DATE:           May 31st, 2011

******************************************************/

#include "TrigCaloEvent/TrigCaloClusterContainer.h"

using namespace std;

// Simple debug method
void
TrigCaloClusterContainer::print ( void ) const {
	std::cout << "TrigCaloClusterContainer with ";
	std::cout << size() << " TrigCaloClusters" << std::endl;
	for ( TrigCaloClusterContainer::const_iterator i = begin(); i!=end();++i)
		(*i)->print();
}

// Simple debug method using MsgStream
void
TrigCaloClusterContainer::print ( MsgStream& log ) const {
	log <<MSG::DEBUG<< "TrigCaloClusterContainer with ";
	log <<MSG::DEBUG<< size() << " TrigCaloClusters" << endmsg;
	for ( TrigCaloClusterContainer::const_iterator i = begin(); i!=end();++i)
		(*i)->print(log);
}

// End of file TrigCaloClusterContainer.cxx
