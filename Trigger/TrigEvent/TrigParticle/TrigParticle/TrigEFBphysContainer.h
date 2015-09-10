// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "TrigParticle/TrigEFBphys.h"
#include "SGTools/BaseInfo.h"

class TrigEFBphys;

//typedef DataVector<TrigEFBphys> TrigEFBphysContainer;
class TrigEFBphysContainer : public DataVector<TrigEFBphys> {}; 


CLASS_DEF( TrigEFBphysContainer , 1308708496 , 1 )

SG_BASE(TrigEFBphysContainer, DataVector<TrigEFBphys>); 

#endif // TRIG_EFBPHYS_CONTAINER
