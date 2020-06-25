/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/*******************************************************

NAME:		TrigTauClusterDetailsContainer.cxx
PACKAGE:	Trigger/TrigEvent/TrigCaloEvent
AUTHORS:	M.P. Casado

PURPOSE:	Keep a container for the detailed variables of
                the Calorimeter LVL2 tau Trigger.
DATE:		January 21st, 2009

******************************************************/

#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"

using namespace std;

// Simple debug method
void
TrigTauClusterDetailsContainer::print ( void ) const {
	std::cout << "TrigTauClusterDetailsContainer with ";
	std::cout << size() << " TrigTauClusterDetails" << std::endl;
	for ( TrigTauClusterDetailsContainer::const_iterator i = begin(); i!=end();++i)
		(*i)->print();
}

// Simple debug method using MsgStream
void
TrigTauClusterDetailsContainer::print ( MsgStream& log ) const {
	log <<MSG::DEBUG<< "TrigTauClusterDetailsContainer with ";
	log <<MSG::DEBUG<< size() << " TrigTauClusterDetails" << endmsg;
	for ( TrigTauClusterDetailsContainer::const_iterator i = begin(); i!=end();++i)
		(*i)->print(log);
}

// End of file TrigTauClusterDetailsContainer.cxx
