// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

File: TrigElectronContainer.h

Description: Container of TrigElectron objects to be stored in POOL.
             This was a typedef of a DataVector 
	     Changes made so that it can be stored
             in StoreGate and persistified in POOL

Authour: R.Goncalo - Thu Sep 20 22:02:43 BST 2005

Modified:

*/

#ifndef TRIG_ELECTRON_CONTAINER
#define TRIG_ELECTRON_CONTAINER

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "TrigParticle/TrigElectron.h"
#include "SGTools/BaseInfo.h"

//typedef DataVector<TrigElectron> TrigElectronContainer;
class TrigElectronContainer : public DataVector<TrigElectron> {}; 


CLASS_DEF( TrigElectronContainer , 127789076 , 1 )

SG_BASE(TrigElectronContainer, DataVector<TrigElectron>); 

#endif // TRIG_ELECTRON_CONTAINER
