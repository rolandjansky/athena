// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

Description: Needed to provide a persistent TrigElectron class in 
             TrigElectronContainer (which is derived from DataVector)
  
Authour: R.Goncalo - Thu Sep 20 18:01:20 BT 2005

*/

#ifndef TRIG_TOPOEVENT_DICT
#define TRIG_TOPOEVENT_DICT

#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"


//the function triggers loading dictionaries for typedefs
void dummyTriggerForTrigPartTypedefs(ElectronMuonTopoInfo a,
				     DataVector<ElectronMuonTopoInfo> b
				     ){

  //to avoid warnings about unused variables
  ElectronMuonTopoInfo aa = a;
  DataVector<ElectronMuonTopoInfo> bb = b;

  }

#endif // TRIG_PARTICLE_DICT
