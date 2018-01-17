/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENT_TRIGT2JETCONTAINER_H
#define TRIGCALOEVENT_TRIGT2JETCONTAINER_H
/********************************************************************

NAME:     TrigT2JetContainer.h
PACKAGE:  
 
AUTHORS:  Kyle Cranmer
CREATED:  Oct. 05

********************************************************************/

// INCLUDE HEADER FILES:

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "AthenaKernel/BaseInfo.h"

/** container of TrigT2Jet elements */
class TrigT2JetContainer : public DataVector<TrigT2Jet> 
{
 public:
  /*
  TrigT2JetContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS)
    : DataVector<TrigT2Jet>(own) 
    {  }
  */
  
  /** Destructor */
  virtual ~TrigT2JetContainer()  {  } ;
  
  /** Print all TrigT2Jet elements */
  // void print();

};

CLASS_DEF(TrigT2JetContainer, 1178384516, 1)
     //CLASS_DEF( DataVector<TrigT2JetContainer> , 1311929759 , 1 )
SG_BASE(TrigT2JetContainer, DataVector<TrigT2Jet>);
#endif
