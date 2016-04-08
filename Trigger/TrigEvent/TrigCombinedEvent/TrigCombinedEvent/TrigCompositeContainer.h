/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOMPOSITECONTAINER_H
#define TRIGCOMPOSITECONTAINER_H


/*******************************************************

	NAME:           TrigCompositeContainer.h
	PACKAGE:        Trigger/TrigEvent/TrigCombinedEvent
        AUTHORS:        Camille B.-Champagne
	CREATION DATE:  November 28th, 2011
	
******************************************************/
// includes section
#include "TrigCombinedEvent/TrigComposite.h"
#include "DataModel/DataVector.h"
#include "SGTools/BaseInfo.h"

/** Container from TrigComposite type objects.
    See TrigComposite
    for more details */
class TrigCompositeContainer : public DataVector<TrigComposite> {
  
 public:
  /**  calls TrigComposite::print (to std::cout)
       for all available combos   */
  void print_size ( void ) const;

 private:
};

// CLIDSvc is already loaded in base class
//CLASS_DEF( TrigCompositeContainer , 1249274952 , 1 )
///CLASS_DEF( DataVector<TrigCompositeContainer> , 1328522819 , 1 )

CLASS_DEF( TrigCompositeContainer , 1190930394 , 1 )
CLASS_DEF( DataVector<TrigCompositeContainer> , 1178462341 , 1 )
  
SG_BASE(TrigCompositeContainer, DataVector<TrigComposite>);

#endif
