// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

File: TrigL2BphysContainer.h

Description: Container of TrigL2Bphys objects to be stored in POOL.
             This was a typedef of a DataVector 
	     Changes made so that it can be stored
             in StoreGate and persistified in POOL

Authour: J.Kirk - Wed May 10

Modified:

*/

#ifndef TRIG_BPHYS_CONTAINER
#define TRIG_BPHYS_CONTAINER

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "SGTools/BaseInfo.h"

#include "TrigParticle/TrigL2Bphys.h"
class TrigL2Bphys;

//typedef DataVector<TrigL2Bphys> TrigL2BphysContainer;
class TrigL2BphysContainer : public DataVector<TrigL2Bphys> { }; 

CLASS_DEF( TrigL2BphysContainer , 1305857775 , 1 )

SG_BASE(TrigL2BphysContainer, DataVector<TrigL2Bphys>);

#endif // TRIG_BPHYS_CONTAINER
