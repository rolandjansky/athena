/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

/* TrigElectron T/P classes: persistent version 1*/
#include "TrigParticleTPCnv/TrigElectronContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigElectronContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p1.h"
#include "TrigParticleTPCnv/TrigElectron_p1.h"
/* TrigElectron T/P classes: persistent version 2*/
#include "TrigParticleTPCnv/TrigElectronContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigElectronContainerCnv_tlp2.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p2.h"
#include "TrigParticleTPCnv/TrigElectron_p2.h"
/* TrigElectron T/P classes: persistent version 3*/
#include "TrigParticleTPCnv/TrigElectronContainerCnv_p3.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p3.h"

//TrigPhoton p1
#include "TrigParticleTPCnv/TrigPhotonContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigPhotonContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p1.h"
#include "TrigParticleTPCnv/TrigPhoton_p1.h"

//TrigPhoton p2
#include "TrigParticleTPCnv/TrigPhotonContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigPhotonContainerCnv_tlp2.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p2.h"
#include "TrigParticleTPCnv/TrigPhoton_p2.h"

#include "TrigParticleTPCnv/TrigPhotonContainerCnv_p3.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p3.h"

//TrigTau and TrigTauContainer
#include "TrigParticleTPCnv/TrigTauContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigTauContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigTauContainer_p1.h"
#include "TrigParticleTPCnv/TrigTau_tlp1.h"
#include "TrigParticleTPCnv/TrigTauCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigTau_p1.h"
#include "TrigParticleTPCnv/TrigTau_tlp2.h"
#include "TrigParticleTPCnv/TrigTauCnv_tlp2.h"
#include "TrigParticleTPCnv/TrigTau_p2.h"

#include "TrigParticleTPCnv/TrigTau_p3.h"
#include "TrigParticleTPCnv/TrigTauCnv_p3.h"
#include "TrigParticleTPCnv/TrigTauContainer_p3.h"
#include "TrigParticleTPCnv/TrigTauContainerCnv_p3.h"

// TrigL2Bphys
#include "TrigParticleTPCnv/TrigL2BphysContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigL2BphysContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigL2BphysContainer_p1.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p1.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p2.h"

#include "TrigParticleTPCnv/TrigL2BphysContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigL2BphysContainerCnv_tlp2.h"
#include "TrigParticleTPCnv/TrigL2Bphys_p3.h"

// TrigEFBphys
#include "TrigParticleTPCnv/TrigEFBphysContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigEFBphysContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigEFBphysContainer_p1.h"
#include "TrigParticleTPCnv/TrigEFBphys_p1.h"
#include "TrigParticleTPCnv/TrigEFBphys_p2.h"

#include "TrigParticleTPCnv/TrigEFBphysContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigEFBphysContainerCnv_tlp2.h"
#include "TrigParticleTPCnv/TrigEFBphys_p3.h"

// TrigL2Bjet
#include "TrigParticleTPCnv/TrigL2BjetContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigL2BjetContainer_p1.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p1.h" 

#include "TrigParticleTPCnv/TrigL2BjetContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_tlp2.h"
#include "TrigParticleTPCnv/TrigL2BjetContainer_p2.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p2.h"

#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_p3.h"
#include "TrigParticleTPCnv/TrigL2BjetContainer_p3.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p3.h"

// TrigEFBjet
#include "TrigParticleTPCnv/TrigEFBjetContainer_tlp1.h"  
#include "TrigParticleTPCnv/TrigEFBjetContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigEFBjetContainer_p1.h"
#include "TrigParticleTPCnv/TrigEFBjet_p1.h"

#include "TrigParticleTPCnv/TrigEFBjetContainer_tlp2.h" 
#include "TrigParticleTPCnv/TrigEFBjetContainerCnv_tlp2.h" 
#include "TrigParticleTPCnv/TrigEFBjetContainer_p2.h" 
#include "TrigParticleTPCnv/TrigEFBjet_p2.h" 

DECLARE_TPCNV_FACTORY(TrigEFBjetContainerCnv_tlp1,
                      TrigEFBjetContainer,
                      TrigEFBjetContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigEFBjetContainerCnv_tlp2,
                      TrigEFBjetContainer,
                      TrigEFBjetContainer_tlp2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigEFBphysContainerCnv_tlp1,
                      TrigEFBphysContainer,
                      TrigEFBphysContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigEFBphysContainerCnv_tlp2,
                      TrigEFBphysContainer,
                      TrigEFBphysContainer_tlp2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigElectronContainerCnv_tlp1,
                      TrigElectronContainer,
                      TrigElectronContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigElectronContainerCnv_tlp2,
                      TrigElectronContainer,
                      TrigElectronContainer_tlp2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigElectronContainerCnv_p3,
                      TrigElectronContainer,
                      TrigElectronContainer_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigL2BjetContainerCnv_tlp1,
                      TrigL2BjetContainer,
                      TrigL2BjetContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigL2BjetContainerCnv_tlp2,
                      TrigL2BjetContainer,
                      TrigL2BjetContainer_tlp2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigL2BjetContainerCnv_p3,
                      TrigL2BjetContainer,
                      TrigL2BjetContainer_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigL2BphysContainerCnv_tlp1,
                      TrigL2BphysContainer,
                      TrigL2BphysContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigL2BphysContainerCnv_tlp2,
                      TrigL2BphysContainer,
                      TrigL2BphysContainer_tlp2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigPhotonContainerCnv_tlp1,
                      TrigPhotonContainer,
                      TrigPhotonContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigPhotonContainerCnv_tlp2,
                      TrigPhotonContainer,
                      TrigPhotonContainer_tlp2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigPhotonContainerCnv_p3,
                      TrigPhotonContainer,
                      TrigPhotonContainer_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigTauCnv_tlp1,
                      TrigTau,
                      TrigTau_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigTauCnv_tlp2,
                      TrigTau,
                      TrigTau_tlp2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigTauContainerCnv_tlp1,
                      TrigTauContainer,
                      TrigTauContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigTauContainerCnv_p3,
                      TrigTauContainer,
                      TrigTauContainer_p3,
                      Athena::TPCnvVers::Current)
