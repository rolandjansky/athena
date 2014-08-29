// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

File: TrigPhotonContainer.h

Description: Container of TrigPhoton objects to be stored in POOL.
             This was a typedef of a DataVector 
	     Changes made so that it can be stored
             in StoreGate and persistified in POOL

Authour: R.Goncalo - Thu Sep 20 22:02:43 BST 2005

Modified:

*/

#ifndef TRIG_PHOTON_CONTAINER
#define TRIG_PHOTON_CONTAINER

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "TrigParticle/TrigPhoton.h"
#include "SGTools/BaseInfo.h"

//typedef DataVector<TrigPhoton> TrigPhotonContainer;
class TrigPhotonContainer : public DataVector<TrigPhoton> { }; 

CLASS_DEF( TrigPhotonContainer , 1296898266 , 1 )

SG_BASE(TrigPhotonContainer, DataVector<TrigPhoton>);

#endif // TRIG_PHOTON_CONTAINER
