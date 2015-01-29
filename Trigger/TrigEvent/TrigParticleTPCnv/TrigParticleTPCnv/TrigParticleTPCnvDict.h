/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGPARTICLETPCNV_TRIGPARTICLETPCNVDICT_H
#define TRIGPARTICLETPCNV_TRIGPARTICLETPCNVDICT_H

/* TrigElectron T/P classes: persistent version 1*/
#include "TrigParticleTPCnv/TrigElectronContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p1.h"
#include "TrigParticleTPCnv/TrigElectron_p1.h"
/* TrigElectron T/P classes: persistent version 2*/
#include "TrigParticleTPCnv/TrigElectronContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p2.h"
#include "TrigParticleTPCnv/TrigElectron_p2.h"
/* TrigElectron T/P classes: persistent version 3*/
#include "TrigParticleTPCnv/TrigElectronContainer_p3.h"
#include "TrigParticleTPCnv/TrigElectron_p3.h"

//TrigPhoton p1
#include "TrigParticleTPCnv/TrigPhotonContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p1.h"
#include "TrigParticleTPCnv/TrigPhoton_p1.h"

//TrigPhoton p2
#include "TrigParticleTPCnv/TrigPhotonContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p2.h"
#include "TrigParticleTPCnv/TrigPhoton_p2.h"

//TrigPhoton p3
#include "TrigParticleTPCnv/TrigPhotonContainer_p3.h"
#include "TrigParticleTPCnv/TrigPhoton_p3.h"

//TrigTau and TrigTauContainer
#include "TrigParticleTPCnv/TrigTauContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigTauContainer_p1.h"
#include "TrigParticleTPCnv/TrigTau_tlp1.h"
#include "TrigParticleTPCnv/TrigTau_p1.h"
#include "TrigParticleTPCnv/TrigTau_tlp2.h"
#include "TrigParticleTPCnv/TrigTau_p2.h"

#include "TrigParticleTPCnv/TrigTauContainer_p3.h"
#include "TrigParticleTPCnv/TrigTau_p3.h"

// TrigL2Bphys
#include "TrigParticleTPCnv/TrigL2BphysContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigL2BphysContainer_p1.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p1.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p2.h"

#include "TrigParticleTPCnv/TrigL2BphysContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p3.h"

// TrigEFBphys
#include "TrigParticleTPCnv/TrigEFBphysContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigEFBphysContainer_p1.h"
#include "TrigParticleTPCnv/TrigEFBphys_p1.h"
#include "TrigParticleTPCnv/TrigEFBphys_p2.h"

#include "TrigParticleTPCnv/TrigEFBphysContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigEFBphys_p3.h"

// TrigL2Bjet
#include "TrigParticleTPCnv/TrigL2BjetContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigL2BjetContainer_p1.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p1.h" 

#include "TrigParticleTPCnv/TrigL2BjetContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigL2BjetContainer_p2.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p2.h"

#include "TrigParticleTPCnv/TrigL2BjetContainer_p3.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p3.h"

// TrigEFBjet
#include "TrigParticleTPCnv/TrigEFBjetContainer_tlp1.h"  
#include "TrigParticleTPCnv/TrigEFBjetContainer_p1.h"
#include "TrigParticleTPCnv/TrigEFBjet_p1.h"

#include "TrigParticleTPCnv/TrigEFBjetContainer_tlp2.h" 
#include "TrigParticleTPCnv/TrigEFBjetContainer_p2.h" 
#include "TrigParticleTPCnv/TrigEFBjet_p2.h" 

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigParticleTPCnvDict
{

  std::vector<TrigTau_p1>                     m_dummyTrigTau_p1;
  std::vector<TrigTau_p3>                     m_dummyTrigTau_p3;
  std::vector<TrigElectron_p1>                m_dummyTrigElectron_p1;
  std::vector<TrigElectron_p2>                m_dummyTrigElectron_p2;
  std::vector<TrigPhoton_p1> 		      m_dummyTrigPhoton_p1;
  std::vector<TrigPhoton_p2> 		      m_dummyTrigPhoton_p2;
  std::vector<TrigPhoton_p3> 		      m_dummyTrigPhoton_p3;
  std::vector<TrigEFBjet_p1>                  m_dummyEFBjet_p1;
  std::vector<TrigEFBjet_p2>                  m_dummyEFBjet_p2;
  std::vector<TrigEFBphys_p1>                 m_dummyEFBphys_p1;
  std::vector<TrigEFBphys_p2>                 m_dummyEFBphys_p2;
  std::vector<TrigEFBphys_p3>                 m_dummyEFBphys_p3;
  std::vector<TrigL2Bphys_p1>                 m_dummyL2Bphys_p1;
  std::vector<TrigL2Bphys_p2>                 m_dummyL2Bphys_p2;
  std::vector<TrigL2Bphys_p3>                 m_dummyL2Bphys_p3;
  std::vector<TrigL2Bjet_p1>                  m_dummyL2Bjet_p1;
  std::vector<TrigL2Bjet_p2>                  m_dummyL2Bjet_p2;
  std::vector<TrigL2Bjet_p3>                  m_dummyL2Bjet_p3;

};

#endif
