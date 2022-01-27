// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**

File: TrigTauContainer.h

Description: Container of TrigTau objects to be stored in POOL.
             This was a typedef of a DataVector 
	     Changes made so that it can be stored
             in StoreGate and persistified in POOL

Authour: M.P. Casado - Mon Mar 06 2006

Modified:

*/

#ifndef TRIG_TAU_CONTAINER
#define TRIG_TAU_CONTAINER

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TrigParticle/TrigTau.h"
#include "AthenaKernel/BaseInfo.h"

//typedef DataVector<TrigTau> TrigTauContainer;
class TrigTauContainer : public DataVector<TrigTau> {
public:
  using DataVector::DataVector;  // inherit constructors
};


CLASS_DEF( TrigTauContainer , 1330589297 , 1 )

SG_BASE(TrigTauContainer, DataVector<TrigTau>);

#endif // TRIG_TAU_CONTAINER
