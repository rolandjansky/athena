// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**

File: TrigEFBphysContainer.h

Description: Container of TrigEFBphys objects to be stored in POOL.
             This was a typedef of a DataVector 
	     Changes made so that it can be stored
             in StoreGate and persistified in POOL

Authour: J.Kirk - Wed May 10

Modified:

*/

#ifndef TRIG_EFBPHYS_CONTAINER
#define TRIG_EFBPHYS_CONTAINER

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TrigParticle/TrigEFBphys.h"
#include "AthenaKernel/BaseInfo.h"

class TrigEFBphys;

//typedef DataVector<TrigEFBphys> TrigEFBphysContainer;
class TrigEFBphysContainer : public DataVector<TrigEFBphys> {
public:
  using DataVector::DataVector;  // inherit constructors
};


CLASS_DEF( TrigEFBphysContainer , 1308708496 , 1 )

SG_BASE(TrigEFBphysContainer, DataVector<TrigEFBphys>); 

#endif // TRIG_EFBPHYS_CONTAINER
