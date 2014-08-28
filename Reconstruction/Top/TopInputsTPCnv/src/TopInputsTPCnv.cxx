/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

#include "TopInputs/TopMuonMap.h"
#include "TopInputsTPCnv/TopMuonMapCnv_p1.h"
#include "TopInputsTPCnv/TopMuonMapCnv_p2.h"

#include "TopInputs/TopElectronMap.h"
#include "TopInputsTPCnv/TopElectronMapCnv_p1.h"
#include "TopInputsTPCnv/TopElectronMapCnv_p2.h"

#include "TopInputs/TopPhotonMap.h"
#include "TopInputsTPCnv/TopPhotonMapCnv_p1.h"

#include "TopInputs/TopJetMap.h"
#include "TopInputsTPCnv/TopJetMapCnv_p1.h"

#include "TopInputs/TopTauJetMap.h"
#include "TopInputsTPCnv/TopTauJetMapCnv_p1.h"

#include "TopInputs/TopTrackMap.h"
#include "TopInputsTPCnv/TopTrackMapCnv_p1.h"

#include "TopInputs/TopOverlap.h"
#include "TopInputsTPCnv/TopOverlapCnv_p1.h"

#include "TopInputs/TopOverlapTrigger.h"
#include "TopInputsTPCnv/TopOverlapTriggerCnv_p1.h"

#include "TopInputsTPCnv/TopInputsCnv_p1.h"
#include "TopInputsTPCnv/TopInputs_p1.h"
#include "TopInputsTPCnv/TopInputsCnv_p2.h"
#include "TopInputsTPCnv/TopInputs_p2.h"
#include "TopInputs/TopInputs.h"

#include "DataModelAthenaPool/DataLink_p1.h"

#include "MissingETEvent/MissingET.h"

// The plugin service will give us warnings if we define multiple
// different transient types corresponding to the same persistent type.
// The typedefs here are to work around that.  (At the cost of not
// being able to look up converters by persistent class name.)

typedef TopInputMap_p1 TopInputMap_ForTauJet_p1;
DECLARE_TPCNV_FACTORY(TopTauJetMapCnv_p1,
                      TopRec::TopInputMap<Analysis::TauJetContainer>,
                      TopInputMap_ForTauJet_p1,
                      Athena::TPCnvVers::Current)
                      
typedef TopInputMap_p1 TopInputMap_ForJet_p1;
DECLARE_TPCNV_FACTORY(TopJetMapCnv_p1,
                      TopRec::TopInputMap<JetCollection>,
                      TopInputMap_ForJet_p1,
                      Athena::TPCnvVers::Current)
                      
typedef TopInputMap_p1 TopInputMap_ForPhoton_p1;
DECLARE_TPCNV_FACTORY(TopPhotonMapCnv_p1,
                      TopRec::TopInputMap<PhotonContainer>,
                      TopInputMap_ForPhoton_p1,
                      Athena::TPCnvVers::Current)
                      
typedef TopInputMap_p1 TopInputMap_ForTrack_p1;
DECLARE_TPCNV_FACTORY(TopTrackMapCnv_p1,
                      TopRec::TopInputMap<Rec::TrackParticleContainer>,
                      TopInputMap_ForTrack_p1,
                      Athena::TPCnvVers::Current)
                      
typedef TopInputMap_p1 TopInputMap_ForMuon_p1;
DECLARE_TPCNV_FACTORY(TopMuonMapCnv_p1,
                      TopRec::TopInputMapTrigger<Analysis::MuonContainer>,
                      TopInputMap_ForMuon_p1,
                      Athena::TPCnvVers::Old)

typedef TopInputMapTrigger_p1 TopInputMapTrigger_ForMuon_p1;
DECLARE_TPCNV_FACTORY(TopMuonMapCnv_p2,
                      TopRec::TopInputMapTrigger<Analysis::MuonContainer>,
                      TopInputMapTrigger_ForMuon_p1,
                      Athena::TPCnvVers::Current)
                      
typedef TopInputMap_p1 TopInputMap_ForElectron_p1;
DECLARE_TPCNV_FACTORY(TopElectronMapCnv_p1,
                      TopRec::TopInputMapTrigger<ElectronContainer>,
                      TopInputMap_ForElectron_p1,
                      Athena::TPCnvVers::Old)
                      
typedef TopInputMapTrigger_p1 TopInputMapTrigger_ForElectron_p1;
DECLARE_TPCNV_FACTORY(TopElectronMapCnv_p2,
                      TopRec::TopInputMapTrigger<ElectronContainer>,
                      TopInputMapTrigger_ForElectron_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TopInputsCnv_p1,
                      TopRec::TopInputs,
                      TopInputs_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TopInputsCnv_p2,
                      TopRec::TopInputs,
                      TopInputs_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TopOverlapCnv_p1,
                      TopRec::TopOverlap,
                      TopOverlap_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TopOverlapTriggerCnv_p1,
                      TopRec::TopOverlapTrigger,
                      TopOverlapTrigger_p1,
                      Athena::TPCnvVers::Current)
