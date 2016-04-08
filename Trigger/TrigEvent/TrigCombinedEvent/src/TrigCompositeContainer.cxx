/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************

	NAME:           TrigCompositeContainer.cxx
	PACKAGE:        Trigger/TrigEvent/TrigCombinedEvent
	AUTHORS:        Camille B.-Champagne
	CREATION DATE:  November 28th, 2011
	
******************************************************/

#include "TrigCombinedEvent/TrigCompositeContainer.h"

using namespace std;

// Simple debug method
void
TrigCompositeContainer::print_size ( void ) const {
  std::cout << "TrigCompositeContainer with ";
  std::cout << size() << " TrigComposites" << std::endl;
  //for ( TrigCompositeContainer::const_iterator i = begin(); i!=end();i++)
  //(*i)->print();
}



// End of file TrigCompositeContainer.cxx
