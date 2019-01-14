/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** Adapted from code by A.Hamilton to check trigger EDM; R.Goncalo 21/11/07 */

#include "GaudiKernel/IToolSvc.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"

#include "xAODTrigger/TrigPassBitsContainer.h"
#include "xAODTrigger/TrigPassBits.h"
#include "xAODTrigger/TriggerMenuContainer.h"
#include "TrigNavStructure/TriggerElement.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "AthContainers/debug.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "TrigParticle/TrigEFBjetContainer.h"
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticle/TrigTauContainer.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"

#include "xAODEventInfo/EventInfo.h"

#include "xAODMuon/MuonContainer.h"
#include "MuonCombinedToolInterfaces/IMuonPrintingTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"


#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauJet.h"


#include "TrigValAlgs/TrigEDMChecker.h"

#include "xAODTrigMinBias/TrigSpacePointCountsContainer.h"
#include "xAODTrigMinBias/TrigSpacePointCounts.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigVertexCountsContainer.h"
#include "xAODTrigMinBias/TrigVertexCounts.h"
#include "xAODTrigMinBias/TrigTrackCountsContainer.h"
#include "xAODTrigMinBias/TrigTrackCounts.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigRoiConversion/RoiSerialise.h"
#include "xAODTrigger/RoiDescriptorStore.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"


#include <iostream>
#include <fstream>
#include <queue>

static  int trackWarningNum;
static  int vertexWarningNum;
static  int  maxRepWarnings;


TrigEDMChecker::TrigEDMChecker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAnalysisAlgorithm(name, pSvcLocator),
    m_muonPrinter("Rec::MuonPrintingTool/MuonPrintingTool"),
    m_clidSvc( "ClassIDSvc", name )
{
  /** switches to control the analysis through job options */

  declareProperty("doDumpAll", m_doDumpAll = true);
  declareProperty("doDumpTrigPassBits", m_doDumpTrigPassBits = false);
  declareProperty("doDumpLVL1_ROI", m_doDumpLVL1_ROI = false);
  declareProperty("doDumpTrigMissingET", m_doDumpTrigMissingET = false);
  declareProperty("doDumpxAODTrigMissingET", m_doDumpxAODTrigMissingET = false);
  declareProperty("doDumpMuonFeature", m_doDumpMuonFeature = false);
  declareProperty("doDumpCombinedMuonFeature", m_doDumpCombinedMuonFeature = false);
  declareProperty("doDumpTileMuFeature",      m_doDumpTileMuFeature = false);
  declareProperty("doDumpTileTrackMuFeature", m_doDumpTileTrackMuFeature = false);
  declareProperty("doDumpTrigPhotonContainer", m_doDumpTrigPhotonContainer = false);
  declareProperty("doDumpTrigL2BphysContainer", m_doDumpTrigL2BphysContainer = false);
  declareProperty("doDumpTrigEFBphysContainer", m_doDumpTrigEFBphysContainer = false);
  declareProperty("doDumpTrigEFBjetContainer", m_doDumpTrigEFBjetContainer = false);
  declareProperty("doDumpTrigL2BjetContainer", m_doDumpTrigL2BjetContainer = false);
  declareProperty("doDumpxAODJetContainer", m_doDumpxAODJetContainer = false);
  declareProperty("doDumpTrigMuonEFContainer", m_doDumpTrigMuonEFContainer = false);
  declareProperty("doDumpTrigMuonEFInfoContainer", m_doDumpTrigMuonEFInfoContainer = false);
  declareProperty("doDumpTrigMuonEFIsolationContainer", m_doDumpTrigMuonEFIsolationContainer = false);
  declareProperty("doDumpxAODMuonContainer", m_doDumpxAODMuonContainer = false);
  declareProperty("doDumpTrigElectronContainer", m_doDumpTrigElectronContainer = false);
  declareProperty("doDumpxAODTrigElectronContainer", m_doDumpxAODTrigElectronContainer = false);
  declareProperty("doDumpxAODTrigPhotonContainer", m_doDumpxAODTrigPhotonContainer = false);
  declareProperty("doDumpxAODElectronContainer", m_doDumpxAODElectronContainer = false);
  declareProperty("doDumpxAODPhotonContainer", m_doDumpxAODPhotonContainer = false);
  declareProperty("doDumpHLTResult", m_doDumpHLTResult = false);
  declareProperty("doDumpTrigTauContainer", m_doDumpTrigTauContainer = false);
  declareProperty("doDumpTrigTauTracksInfo", m_doDumpTrigTauTracksInfo = false);
  declareProperty("doDumpTrigInDetTrackCollection", m_doDumpTrigInDetTrackCollection = false);
  declareProperty("doDumpTrigVertexCollection", m_doDumpTrigVertexCollection = false);
  declareProperty("doDumpTrigEMCluster", m_doDumpTrigEMCluster = false);
  declareProperty("doDumpTrigEMClusterContainer", m_doDumpTrigEMClusterContainer = false);
  declareProperty("doDumpxAODTrigEMCluster", m_doDumpxAODTrigEMCluster = false);
  declareProperty("doDumpxAODTrigEMClusterContainer", m_doDumpxAODTrigEMClusterContainer = false);
  declareProperty("doDumpTrigTauClusterContainer", m_doDumpTrigTauClusterContainer = false);
  declareProperty("doDumpTrackParticleContainer", m_doDumpTrackParticleContainer = false);
  declareProperty("doDumpTauJetContainer", m_doDumpTauJetContainer = false);
  declareProperty("doDumpxAODTrackParticle", m_doDumpxAODTrackParticle = false);
  declareProperty("doDumpxAODVertex", m_doDumpxAODVertex = false);
  declareProperty("doDumpxAODTauJetContainer", m_doDumpxAODTauJetContainer = false);
  declareProperty("doDumpxAODTrigMinBias", m_doDumpxAODTrigMinBias = false);
  declareProperty("doDumpStoreGate", m_doDumpStoreGate = false );
  declareProperty("doDumpAllTrigComposite", m_doDumpAllTrigComposite = false );
  declareProperty("dumpTrigCompositeContainers", m_dumpTrigCompositeContainers, "List of TC to dump" );
  declareProperty("doDumpTrigCompsiteNavigation", m_doDumpTrigCompsiteNavigation = false );
  declareProperty("doDumpNavigation", m_doDumpNavigation = false );
  declareProperty("doTDTCheck", m_doTDTCheck = false );
  declareProperty( "ClassIDSvc", m_clidSvc, "Service providing CLID info" );
}



TrigEDMChecker::~TrigEDMChecker() {}

StatusCode TrigEDMChecker::initialize() {

  ATH_CHECK( m_navigationHandleKey.initialize() );
  ATH_CHECK( m_decisionsKey.initialize() );
  ATH_CHECK( m_navigationTool.retrieve() );

  ATH_MSG_DEBUG("Initializing TrigEDMChecker");

  ATH_MSG_INFO("REGTEST Initializing...");
  ATH_MSG_INFO("REGTEST m_doDumpAll                      = " << m_doDumpAll );
  ATH_MSG_INFO("REGTEST m_doDumpLVL1_ROI                 = " << m_doDumpLVL1_ROI);
  ATH_MSG_INFO("REGTEST m_doDumpTrigMissingET            = " << m_doDumpTrigMissingET );
  ATH_MSG_INFO("REGTEST m_doDumpxAODTrigMissingET        = " << m_doDumpxAODTrigMissingET );
  ATH_MSG_INFO("REGTEST m_doDumpMuonFeature              = " << m_doDumpMuonFeature );
  ATH_MSG_INFO("REGTEST m_doDumpCombinedMuonFeature      = " << m_doDumpCombinedMuonFeature );
  ATH_MSG_INFO("REGTEST m_doDumpTileMuFeature            = " << m_doDumpTileMuFeature);
  ATH_MSG_INFO("REGTEST m_doDumpTileTrackMuFeature       = " << m_doDumpTileTrackMuFeature);
  ATH_MSG_INFO("REGTEST m_doDumpTrigPhotonContainer      = " << m_doDumpTrigPhotonContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrigL2BphysContainer     = " << m_doDumpTrigL2BphysContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrigEFBphysContainer     = " << m_doDumpTrigEFBphysContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrigEFBjetContainer      = " << m_doDumpTrigEFBjetContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrigL2BjetContainer      = " << m_doDumpTrigL2BjetContainer );
  ATH_MSG_INFO("REGTEST m_doDumpxAODJetContainer         = " << m_doDumpxAODJetContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrigMuonEFContainer      = " << m_doDumpTrigMuonEFContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrigMuonEFInfoContainer  = " << m_doDumpTrigMuonEFInfoContainer );
  ATH_MSG_INFO("REGTEST m_doDumpxAODMuonContainer        = " << m_doDumpxAODMuonContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrigElectronContainer    = " << m_doDumpTrigElectronContainer );
  ATH_MSG_INFO("REGTEST m_doDumpxAODTrigElectronContainer= " << m_doDumpxAODTrigElectronContainer );
  ATH_MSG_INFO("REGTEST m_doDumpxAODTrigPhotonContainer  = " << m_doDumpxAODTrigPhotonContainer );
  ATH_MSG_INFO("REGTEST m_doDumpxAODElectronContainer    = " << m_doDumpxAODElectronContainer );
  ATH_MSG_INFO("REGTEST m_doDumpxAODPhotonContainer      = " << m_doDumpxAODPhotonContainer );
  ATH_MSG_INFO("REGTEST m_doDumpHLTResult                = " << m_doDumpHLTResult );
  ATH_MSG_INFO("REGTEST m_doDumpTrigTauContainer         = " << m_doDumpTrigTauContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrigTauTracksInfo        = " << m_doDumpTrigTauTracksInfo );
  ATH_MSG_INFO("REGTEST m_doDumpTrigInDetTrackCollection = " << m_doDumpTrigInDetTrackCollection );
  ATH_MSG_INFO("REGTEST m_doDumpTrigVertexCollection     = " << m_doDumpTrigVertexCollection );
  ATH_MSG_INFO("REGTEST m_doDumpTrigEMCluster            = " << m_doDumpTrigEMCluster );
  ATH_MSG_INFO("REGTEST m_doDumpTrigEMClusterContainer   = " << m_doDumpTrigEMClusterContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrigTauClusterContainer  = " << m_doDumpTrigTauClusterContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTrackParticleContainer   = " << m_doDumpTrackParticleContainer );
  ATH_MSG_INFO("REGTEST m_doDumpTauJetContainer          = " << m_doDumpTauJetContainer );
  ATH_MSG_INFO("REGTEST m_doDumpxAODTrackParticle        = " << m_doDumpxAODTrackParticle );
  ATH_MSG_INFO("REGTEST m_doDumpxAODVertex               = " << m_doDumpxAODVertex );
  ATH_MSG_INFO("REGTEST m_doDumpxAODTauJetContainer      = " << m_doDumpxAODTauJetContainer );
  ATH_MSG_INFO("REGTEST m_doDumpxAODTrigMinBias          = " << m_doDumpxAODTrigMinBias );
  ATH_MSG_INFO("REGTEST m_doDumpStoreGate                = " << m_doDumpStoreGate );
  ATH_MSG_INFO("REGTEST m_doDumpAllTrigComposite         = " << m_doDumpAllTrigComposite );
  ATH_MSG_INFO("REGTEST m_dumpTrigCompositeContainers    = " << m_dumpTrigCompositeContainers );
  ATH_MSG_INFO("REGTEST m_doDumpTrigCompsiteNavigation   = " << m_doDumpTrigCompsiteNavigation );
  ATH_MSG_INFO("REGTEST m_doTDTCheck                     = " << m_doTDTCheck );

	
//      puts this here for the moment
    maxRepWarnings = 5;
	ATH_MSG_INFO("maxRepWarning      = " <<  maxRepWarnings );

	vertexWarningNum = 0;
    trackWarningNum = 0;

	if(m_doDumpxAODMuonContainer || m_doDumpAll) {
	  StatusCode sc = m_muonPrinter.retrieve();
	  if(sc.isFailure()) {
	    ATH_MSG_ERROR("Could not retrieve MuonPrinter tool");
	    return sc;
	  }
	}

  if (m_doDumpTrigCompsiteNavigation) {
    ATH_CHECK( m_clidSvc.retrieve() );
  }

  if (m_doTDTCheck) {
    ATH_CHECK( m_trigDec.retrieve() );
    m_trigDec->ExperimentalAndExpertMethods()->enable();
  }

	return StatusCode::SUCCESS;
}


StatusCode TrigEDMChecker::execute() {

  ATH_MSG_INFO( " ==========START of event===========" );

	if(m_doDumpTrackParticleContainer){
		StatusCode sc = dumpTrackParticleContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrackParticleContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpLVL1_ROI ){
		StatusCode sc = dumpLVL1_ROI();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpLVL1_ROI() failed");
		}
	}

    /*
	if(m_doDumpAll || m_doDumpTrigMissingET){
		StatusCode sc = dumpTrigMissingET();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigMissingET() failed" << endmsg;

		}
	}
	*/

	if(m_doDumpAll || m_doDumpxAODTrigMissingET){
		StatusCode sc = dumpxAODTrigMissingET();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpxAODTrigMissingET() failed");
		}
    }

	if(m_doDumpAll || m_doDumpMuonFeature){
		StatusCode sc = dumpMuonFeature();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpMuonFeature() failed");
		}
	}

	if(m_doDumpAll || m_doDumpCombinedMuonFeature){
		StatusCode sc = dumpCombinedMuonFeature();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpCombinedMuonFeature() failed");
		}
		sc = dumpCombinedMuonFeatureContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpCombinedMuonFeatureContainer() failed");
          return StatusCode::SUCCESS;
		}
	}

	if(m_doDumpAll || m_doDumpTileMuFeature) {
		StatusCode sc = dumpTileMuFeatureContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTileMuFeatureContainer() failed");          
		}
	}

	if(m_doDumpAll || m_doDumpTileTrackMuFeature) {
		StatusCode sc = dumpTileTrackMuFeatureContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTileTrackMuFeatureContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpTrigEMCluster){
		StatusCode sc = dumpTrigEMCluster();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigEMCluster() failed");
		}
	}

	if(m_doDumpAll || m_doDumpTrigEMClusterContainer){
		StatusCode sc = dumpTrigEMClusterContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigEMClusterContainer() failed");          
		}
	}

    if(m_doDumpAll || m_doDumpxAODTrigEMCluster){
      StatusCode sc = dumpxAODTrigEMCluster();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpxAODTrigEMCluster() failed");
      }
    }

    if(m_doDumpAll || m_doDumpxAODTrigEMClusterContainer){
      StatusCode sc = dumpxAODTrigEMClusterContainer();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpxAODTrigEMClusterContainer() failed");
      }
    }

	if(m_doDumpTrigTauClusterContainer){
		StatusCode sc = dumpTrigTauClusterContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigTauClusterContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpTrigPhotonContainer){
		StatusCode sc = dumpTrigPhotonContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigPhotonContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpxAODJetContainer){
		StatusCode sc = dumpxAODJetContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpxAODJetContainer() failed");          
		}
	}

	if(m_doDumpAll || m_doDumpTrigL2BphysContainer){
		StatusCode sc = dumpTrigL2BphysContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigL2BphysContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpTrigEFBphysContainer){
		StatusCode sc = dumpTrigEFBphysContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigEFBphysContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpTrigEFBjetContainer){
		StatusCode sc = dumpTrigEFBjetContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigEFBjetContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpTrigL2BjetContainer){
		StatusCode sc = dumpTrigL2BjetContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigL2BjetContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpTrigMuonEFContainer){
		StatusCode sc = dumpTrigMuonEFContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigMuonEFContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpTrigMuonEFInfoContainer){
		StatusCode sc = dumpTrigMuonEFInfoContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigMuonEFInfoContainer() failed");          
		}
	}

	if(m_doDumpAll || m_doDumpTrigMuonEFIsolationContainer) {
	  StatusCode sc = dumpTrigMuonEFIsolationContainer();
	  if(sc.isFailure()) {
	    ATH_MSG_ERROR("The method dumpTrigMuonEFIsolationContainer() failed");
	  }
	}

	if(m_doDumpAll || m_doDumpxAODMuonContainer) {
	  StatusCode sc = dumpxAODMuonContainer();
	  if(sc.isFailure()) {
	    ATH_MSG_ERROR("The method dumpxAODMuonContainer() failed");
	  }
	}

	if(m_doDumpAll || m_doDumpTrigElectronContainer){
		StatusCode sc = dumpTrigElectronContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigElectronContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpxAODTrigElectronContainer){
		StatusCode sc = dumpxAODTrigElectronContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpxAODTrigElectronContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpxAODTrigPhotonContainer){
		StatusCode sc = dumpxAODTrigPhotonContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpxAODTrigElectronContainer() failed");
		}
	}

    if(m_doDumpAll || m_doDumpxAODElectronContainer){
      StatusCode sc = dumpxAODElectronContainer();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpxAODTrigElectronContainer() failed");
      }
	}

	if(m_doDumpAll || m_doDumpxAODPhotonContainer){
      StatusCode sc = dumpxAODPhotonContainer();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpxAODTrigElectronContainer() failed");        
      }
	}

    if(m_doDumpTrigTauContainer){
      StatusCode sc = dumpTrigTauContainer();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpTrigTauContainer() failed");
      }
	}

	if(m_doDumpTrigTauTracksInfo){
		StatusCode sc = dumpTrigTauTracksInfo();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigTauTracksInfo() failed");
		}
	}

	if(m_doDumpAll || m_doDumpHLTResult){
		StatusCode sc = dumpHLTResult();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpHLTResult() failed");
		}
	}

	if(m_doDumpTrigInDetTrackCollection){
		StatusCode sc = dumpTrigInDetTrackCollection();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTrigInDetTrackCollection() failed");
		}
	}

	if(m_doDumpAll || m_doDumpTrigVertexCollection){
      StatusCode sc = dumpTrigVertexCollection();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpTrigVertexCollection() failed");
      }
	}

	if(m_doDumpAll || m_doDumpxAODTauJetContainer){
      StatusCode sc = dumpxAODTauJetContainer();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpxAODTauJetContainer() failed");
      }
	}

	if(m_doDumpTauJetContainer){
		StatusCode sc = dumpTauJetContainer();
		if (sc.isFailure()) {
          ATH_MSG_ERROR("The method dumpTauJetContainer() failed");
		}
	}

	if(m_doDumpAll || m_doDumpxAODTrackParticle){
      StatusCode sc = dumpxAODTrackParticle();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpxAODTrackParticle() failed");      
      }
	}

	if(m_doDumpAll || m_doDumpxAODVertex){
      StatusCode sc = dumpxAODVertex();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpxAODVertex() failed");
      }
	}

	if (m_doDumpAll || m_doDumpxAODTrigMinBias){
      StatusCode sc = dumpxAODTrigMinBias();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpxAODTrigMinBias() failed");
      }
	}

	if (m_doDumpTrigPassBits){
      StatusCode sc = dumpTrigPassBits();
      if (sc.isFailure()) {
        ATH_MSG_ERROR("The method dumpTrigPassBits() failed");
      }      
	}
	
  if (m_doDumpAll || m_doDumpStoreGate) {
    ATH_MSG_DEBUG(evtStore()->dump());
  }

  if (m_doDumpAll || m_doDumpNavigation) {
    StatusCode sc = dumpNavigation();
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR("The method dumpNavigation() failed");
    }
  }

  if (m_doDumpAll || m_doTDTCheck) {
    ATH_CHECK(dumpTDT());
  }

  if (m_doDumpAll || m_doDumpAllTrigComposite || m_dumpTrigCompositeContainers.size() > 0) {
  	ATH_CHECK( dumpTrigComposite() );
  }

  if (m_doDumpAll || m_doDumpTrigCompsiteNavigation) {
    std::string trigCompositeSteering;
    ATH_CHECK(TrigCompositeNavigationToDot(trigCompositeSteering));
    const xAOD::EventInfo* evtInfo = nullptr;
    if (evtStore()->contains<xAOD::EventInfo>("EventInfo")) {
      ATH_CHECK( evtStore()->retrieve(evtInfo) );
    }
    static int eventStatic = 0; // Might not always have EventInfo (early testing of Run-3 software)
    const std::string evtNumber = (evtInfo == nullptr ? std::to_string(eventStatic++) : std::to_string(evtInfo->eventNumber()));
    std::ofstream ofile(std::string("NavigationGraph_" + evtNumber + ".dot").c_str());
    ofile << trigCompositeSteering;
  }



  ATH_MSG_INFO( " ==========END of event===========" );
	return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpTrigPassBits(){
    const std::string name="HLT_xAOD__TrigPassBitsContainer_passbits";
    const xAOD::TrigPassBitsContainer *xbitscont=nullptr;
    StatusCode sc = evtStore()->retrieve(xbitscont,name);
    if (sc.isFailure() ){
        ATH_MSG_INFO("Cannot retrieve TrigPassBits");
    }
    else {
        ATH_MSG_INFO("Size of PassBits container : " << xbitscont->size());
        for(const auto &bits:*xbitscont){
            if(bits==nullptr){
                ATH_MSG_INFO("TrigPassBits point nullptr ");
                continue;
            }
            ATH_MSG_DEBUG("Analyzing bits for " << bits->containerClid() << " of size " << bits->size() << " with bit size " << bits->passBits().size());
        }
        xAOD::TrigPassBitsContainer::const_iterator itr  = xbitscont->begin();
        xAOD::TrigPassBitsContainer::const_iterator itrE = xbitscont->end();

        for (int j=0; itr != itrE; ++itr, ++j ) {
            const xAOD::TrigPassBits * bits = (*itr);
            if(bits==nullptr){
                ATH_MSG_INFO("TrigPassBits point nullptr ");
                continue;
            }
            ATH_MSG_DEBUG("Analyzing bits for " << bits->containerClid() << " of size " << bits->size() << " with bit size " << bits->passBits().size());
        }
    }
    return StatusCode::SUCCESS;
}

void TrigEDMChecker::dumpTrigSpacePointCounts()
{
    ATH_MSG_INFO("MinBias in dumpTrigSpacePointCounts()");

	std::string METTag="HLT_xAOD__TrigSpacePointCountsContainer_spacepoints";

	const xAOD::TrigSpacePointCountsContainer* SpacePointCountsCont=0;
	StatusCode sc = evtStore()->retrieve(SpacePointCountsCont,METTag);

	if (sc.isFailure())
      ATH_MSG_INFO("failed to retrieve " << METTag);
    else {
      ATH_MSG_INFO("Accessing " << METTag << " with " << SpacePointCountsCont->size() << " elements");
      
      std::string s; char buff[128];
      std::vector<float> getVec;
      float sum;
      
      // Loop over container content
      for(uint i = 0; i < SpacePointCountsCont->size(); i++) {
        getVec = SpacePointCountsCont->at(i)->contentsPixelClusEndcapC();
        sum = 0.;
        for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
        snprintf(buff, sizeof(buff), "REGTEST %s SUM of contentsPixelClusEndcapC() =         %10.2f ", s.c_str(), sum );
        ATH_MSG_INFO(buff);
        
        getVec = SpacePointCountsCont->at(i)->contentsPixelClusBarrel();
        sum = 0.;
        for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
        snprintf(buff, sizeof(buff), "REGTEST %s SUM of contentsPixelClusBarrel() =         %10.2f ", s.c_str(), sum );
        ATH_MSG_INFO(buff);
        
        getVec = SpacePointCountsCont->at(i)->contentsPixelClusEndcapA();
        sum = 0.;
        for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
        snprintf(buff, sizeof(buff), "REGTEST %s SUM of contentsPixelClusEndcapA() =         %10.2f ", s.c_str(), sum );
        ATH_MSG_INFO(buff);
        
        snprintf(buff, sizeof(buff), "REGTEST %s pixelClusTotBins() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusTotBins() );
        ATH_MSG_INFO(buff);
        
        snprintf(buff, sizeof(buff), "REGTEST %s pixelClusTotMin() =        %10.2f ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusTotMin() );
        ATH_MSG_INFO(buff);
        
        snprintf(buff, sizeof(buff), "REGTEST %s pixelClusTotMax() =        %10.2f ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusTotMax() );
        ATH_MSG_INFO(buff);
        
        snprintf(buff, sizeof(buff), "REGTEST %s pixelClusSizeBins() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusSizeBins() );
        ATH_MSG_INFO(buff);
        
        snprintf(buff, sizeof(buff), "REGTEST %s pixelClusSizeMin()  =        %10.2f ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusSizeMin() );
        ATH_MSG_INFO(buff);
        
        snprintf(buff, sizeof(buff), "REGTEST %s pixelClusSizeMax() =        %10.2f ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusSizeMax() );
        ATH_MSG_INFO(buff);
        
        snprintf(buff, sizeof(buff), "REGTEST %s sctSpEndcapC() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->sctSpEndcapC() );
        ATH_MSG_INFO(buff);
        
        snprintf(buff, sizeof(buff), "REGTEST %s sctSpBarrel() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->sctSpBarrel() );
        ATH_MSG_INFO(buff);
        
        snprintf(buff, sizeof(buff), "REGTEST %s sctSpEndcapA() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->sctSpEndcapA() );
        ATH_MSG_INFO(buff);
      }
	}
}

void TrigEDMChecker::dumpTrigT2MBTSBits(){
  ATH_MSG_INFO("MinBias in dumpTrigT2MBTSBits()");

  std::string METTag="HLT_xAOD__TrigT2MbtsBitsContainer_T2Mbts";

  const xAOD::TrigT2MbtsBitsContainer* T2MbtsBitsCont=0;
  StatusCode sc = evtStore()->retrieve(T2MbtsBitsCont,METTag);

  if (sc.isFailure())
    ATH_MSG_INFO("failed to retrieve " << METTag);
  else {
    ATH_MSG_INFO("Accessing " << METTag << " with " << T2MbtsBitsCont->size() << " elements");

    std::string s; char buff[380];
    std::vector<float> getVec;
    float sum;

    // Loop over container content
    for(uint i = 0; i < T2MbtsBitsCont->size(); i++) {
      getVec = T2MbtsBitsCont->at(i)->triggerEnergies();
      sum = 0.;
      for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
      snprintf(buff, sizeof(buff), "REGTEST %s SUM of triggerEnergies() =         %10.2f ", s.c_str(), sum );
      ATH_MSG_INFO(buff);

      getVec = T2MbtsBitsCont->at(i)->triggerTimes();
      sum = 0.;
      for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
      snprintf(buff, sizeof(buff), "REGTEST %s SUM of triggerTimes() =         %10.2f ", s.c_str(), sum );
      ATH_MSG_INFO(buff);
    }
  }
}

void TrigEDMChecker::dumpTrigVertexCounts(){
  ATH_MSG_INFO("MinBias in dumpTrigVertexCounts()");

  std::string METTag="HLT_xAOD__TrigVertexCountsContainer_vertexcounts";

  const xAOD::TrigVertexCountsContainer* T2VertexCountsCont=0;
  StatusCode sc = evtStore()->retrieve(T2VertexCountsCont,METTag);

  if (sc.isFailure())
    ATH_MSG_INFO("failed to retrieve " << METTag);
  else {
    ATH_MSG_INFO("Accessing " << METTag << " with " << T2VertexCountsCont->size() << " elements");

    std::string s; char buff[380];
    std::vector<float> fgetVec;
    float fsum(0.);
    std::vector<unsigned int> ugetVec;
    unsigned int usum(0);

    // Loop over container content
    for(uint i = 0; i < T2VertexCountsCont->size(); i++) {
      ugetVec = T2VertexCountsCont->at(i)->vtxNtrks();
      for (uint j = 0; j < ugetVec.size(); ++j) usum += ugetVec[j];
      snprintf(buff, sizeof(buff), "REGTEST %s SUM of vtxNtrks() =         %u ", s.c_str(), usum );
      ATH_MSG_INFO(buff);

      fgetVec = T2VertexCountsCont->at(i)->vtxTrkPtSqSum();
      for (uint j = 0; j < fgetVec.size(); ++j) fsum += fgetVec[j];
      snprintf(buff, sizeof(buff), "REGTEST %s SUM of vtxTrkPtSqSum() =         %10.2f ", s.c_str(), fsum );
      ATH_MSG_INFO(buff);
    }
  }
}

void TrigEDMChecker::dumpTrigTrackCounts(){
  ATH_MSG_INFO("MinBias in dumpTrigTrackCounts()");

  std::string METTag="HLT_xAOD__TrigTrackCountsContainer_trackcounts";

  const xAOD::TrigTrackCountsContainer* T2TrackCountsCont=0;
  StatusCode sc = evtStore()->retrieve(T2TrackCountsCont,METTag);

  if (sc.isFailure())
    ATH_MSG_INFO("failed to retrieve " << METTag);
  else {
    ATH_MSG_INFO("Accessing " << METTag << " with " << T2TrackCountsCont->size() << " elements");

    std::string s; char buff[380];
    std::vector<float> getVec;
    float sum;

    // Loop over container content
    for(uint i = 0; i < T2TrackCountsCont->size(); i++) {
      getVec = T2TrackCountsCont->at(i)->z0_pt();
      sum = 0.;
      for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
      snprintf(buff, sizeof(buff), "REGTEST %s SUM of z0_pt =         %10.2f ", s.c_str(), sum );
      ATH_MSG_INFO(buff);

      getVec = T2TrackCountsCont->at(i)->eta_phi();
      sum = 0.;
      for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
      snprintf(buff, sizeof(buff), "REGTEST %s SUM of eta_phi() =         %10.2f ", s.c_str(), sum );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s z0Bins() =        %u ", s.c_str(), T2TrackCountsCont->at(i)->z0Bins() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s z0Min() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->z0Min() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s z0Max() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->z0Max() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s ptBins() =        %u ", s.c_str(), T2TrackCountsCont->at(i)->ptBins() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s ptMin() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->ptMin() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s ptMax() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->ptMax() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s etaBins() =        %u ", s.c_str(), T2TrackCountsCont->at(i)->etaBins() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s etaMin() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->etaMin() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s etaMax() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->etaMax() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s phiBins() =        %u ", s.c_str(), T2TrackCountsCont->at(i)->phiBins() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s phiMin() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->phiMin() );
      ATH_MSG_INFO(buff);

      snprintf(buff, sizeof(buff), "REGTEST %s phiMax() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->phiMax() );
      ATH_MSG_INFO(buff);
    }
  }
}

StatusCode TrigEDMChecker::dumpxAODTrigMinBias() {

	dumpTrigSpacePointCounts();
	dumpTrigT2MBTSBits();
	dumpTrigVertexCounts();
	dumpTrigTrackCounts();

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////



StatusCode TrigEDMChecker::dumpxAODTrigMissingET() {

  ATH_MSG_INFO("dumpxAODTrigMissingET()");

  int ntag=4;
  std::string METTags[]={"HLT_xAOD__TrigMissingETContainer_EFJetEtSum","HLT_xAOD__TrigMissingETContainer_TrigEFMissingET", "HLT_xAOD__TrigMissingETContainer_TrigL2MissingET_FEB","HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl"};

  for(int itag=0; itag <ntag; itag++) {

    const xAOD::TrigMissingETContainer* MissingETCont=0;
    StatusCode sc = evtStore()->retrieve(MissingETCont,METTags[itag]);
    if (sc.isFailure())
      ATH_MSG_INFO("failed to retrieve " << METTags[itag]);
    else {
      ATH_MSG_INFO("Accessing " << METTags[itag] << " with " << MissingETCont->size() << " elements");

      // Loop over container content
      for(uint i = 0; i < MissingETCont->size(); i++) {

        std::string s; char buff[3000];
             
        snprintf(buff, sizeof(buff), "REGTEST %s Ex =         %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->ex() );
        ATH_MSG_INFO(buff);
        snprintf(buff, sizeof(buff), "REGTEST %s Ey =         %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->ey() );
        ATH_MSG_INFO(buff);
        snprintf(buff, sizeof(buff), "REGTEST %s Ez =         %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->ez() );
        ATH_MSG_INFO(buff);
        snprintf(buff, sizeof(buff), "REGTEST %s SumET =         %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->sumEt() );
        ATH_MSG_INFO(buff);
        snprintf(buff, sizeof(buff), "REGTEST %s SumE =       %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->sumE() );
        ATH_MSG_INFO(buff);
        snprintf(buff, sizeof(buff), "REGTEST %s Flag =       %d", s.c_str(), MissingETCont->at(i)->flag() );
        ATH_MSG_INFO(buff);
        snprintf(buff, sizeof(buff), "REGTEST %s Flag =       %d", s.c_str(), MissingETCont->at(i)->roiWord() );
        ATH_MSG_INFO(buff);
             
        unsigned int Nc = MissingETCont->at(i)->getNumberOfComponents();
        if (Nc > 0) { 
          s="REGTEST __name____status_usedChannels__sumOfSigns__calib1_calib0";
          s+="/MeV__ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/CLHEP::MeV";
          ATH_MSG_INFO(s);
        }
             
        for(uint j = 0; j < Nc; j++) {
             
          std::string name =               MissingETCont->at(i)->nameOfComponent(j);
          const short status =             MissingETCont->at(i)->statusComponent(j);
          const unsigned short usedChan =  MissingETCont->at(i)->usedChannelsComponent(j);
          const short sumOfSigns =         MissingETCont->at(i)->sumOfSignsComponent(j);
          const float calib0 =             MissingETCont->at(i)->calib0Component(j);
          const float calib1 =             MissingETCont->at(i)->calib1Component(j);
          const float ex =                 MissingETCont->at(i)->exComponent(j);
          const float ey =                 MissingETCont->at(i)->eyComponent(j);
          const float ez =                 MissingETCont->at(i)->ezComponent(j);
          const float sumE =               MissingETCont->at(i)->sumEComponent(j);
          const float sumEt =              MissingETCont->at(i)->sumEtComponent(j);

          snprintf(buff, sizeof(buff),
                   "REGTEST   %s   %6d %12d %10d   %6.2f  %6.3f %10.2f %10.2f %10.2f %10.2f %10.2f",
                   name.c_str(), status, usedChan, sumOfSigns, calib1, calib0,
                   ex, ey, ez, sumE, sumEt);
          ATH_MSG_INFO(buff);		
        }              
      }
    }
  }

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////



StatusCode TrigEDMChecker::dumpTrigMissingET() {

  ATH_MSG_DEBUG("in dumpTrigMissingET()");

  int ntag=3;
  std::string METTags[]={"HLT_TrigMissingETContainer_TrigEFMissingET", "HLT_TrigMissingETContainer_TrigEFMissingET_FEB", "HLT_TrigMissingETContainer_TrigEFMissingET_topocl"};

  /// >= 14.2.10 /// ----------------------------
  for (int itag=0; itag < ntag; itag++) { // loop over L2, EF
    const TrigMissingETContainer* trigMETcont;
    StatusCode sc=evtStore()->retrieve(trigMETcont , METTags[itag]);
    if( sc.isFailure() ){
      ATH_MSG_INFO("Failed to retrieve TrigMissingETContainer with key " << METTags[itag]);
      continue;
    }

    ATH_MSG_INFO("Got TrigMissingETContainer with key \"" << METTags[itag]<< "\"");

    TrigMissingETContainer::const_iterator trigMETfirst  = trigMETcont->begin();
    TrigMissingETContainer::const_iterator trigMETlast = trigMETcont->end();

    for (int j=0; trigMETfirst !=  trigMETlast;  ++trigMETfirst++, ++j  ) {

      ATH_MSG_INFO("REGTEST ==========START of TrigMissingET DUMP===========");

      std::string s;
      char buff[128];

      snprintf(buff, sizeof(buff), "REGTEST %s Ex =         %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->ex() );
      ATH_MSG_INFO(buff);
      snprintf(buff, sizeof(buff), "REGTEST %s Ey =         %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->ey() );
      ATH_MSG_INFO(buff);
      snprintf(buff, sizeof(buff), "REGTEST %s Ez =         %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->ez() );
      ATH_MSG_INFO(buff);
      snprintf(buff, sizeof(buff), "REGTEST %s Et =         %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->et() );
      ATH_MSG_INFO(buff);
      snprintf(buff, sizeof(buff), "REGTEST %s SumEt =      %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->sumEt() );
      ATH_MSG_INFO(buff);
      snprintf(buff, sizeof(buff), "REGTEST %s SumE =       %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->sumE() );
      ATH_MSG_INFO(buff);
      snprintf(buff, sizeof(buff), "REGTEST %s E =          %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->e() );
      ATH_MSG_INFO(buff);
      snprintf(buff, sizeof(buff), "REGTEST %s flag =    %10d",       s.c_str(), (*trigMETfirst)->getFlag() );
      ATH_MSG_INFO(buff);
      snprintf(buff, sizeof(buff), "REGTEST %s RoIword = %10ld",      s.c_str(), (*trigMETfirst)->RoIword() );
      ATH_MSG_INFO(buff);

      unsigned int Nc = (*trigMETfirst)->getNumOfComponents();
      if (Nc > 0) {
        s="REGTEST __name____status_usedChannels__sumOfSigns__calib1_calib0";
        s+="/MeV__ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/CLHEP::MeV";
        ATH_MSG_INFO(s);

        for (unsigned int i=0; i<Nc; ++i) { // loop over components
          std::string name =              (*trigMETfirst)->getNameOfComponent(i);
          const short status =            (*trigMETfirst)->getStatus(i);
          const unsigned short usedChan = (*trigMETfirst)->getUsedChannels(i);
          const short sumOfSigns =        (*trigMETfirst)->getSumOfSigns(i);
          const float calib0 =            (*trigMETfirst)->getComponentCalib0(i);
          const float calib1 =            (*trigMETfirst)->getComponentCalib1(i);
          const float ex =                (*trigMETfirst)->getExComponent(i);
          const float ey =                (*trigMETfirst)->getEyComponent(i);
          const float ez =                (*trigMETfirst)->getEzComponent(i);
          const float sumE =              (*trigMETfirst)->getSumEComponent(i);
          const float sumEt =             (*trigMETfirst)->getSumEtComponent(i);

          snprintf(buff, sizeof(buff),
                   "REGTEST   %s   %6d %12d %10d   %6.2f  %6.3f %10.2f %10.2f %10.2f %10.2f %10.2f",
                   name.c_str(), status, usedChan, sumOfSigns, calib1, calib0,
                   ex, ey, ez, sumE, sumEt);
          ATH_MSG_INFO(buff);
        } // loop over components
      }
    } // loop over TrigMissingET objects
  } // loop over TrigMissingETContainers

	// if( sc.isSuccess() ) return sc; // Commented out by FB (12.07.14)

	/// up to 14.2.0 /// ----------------------------
  ATH_MSG_INFO("Trying to fetch TrigMissingET objects from older releases");

  const DataHandle<TrigMissingET> trigMETfirst ,trigMETlast;
  StatusCode sc=evtStore()->retrieve(trigMETfirst ,trigMETlast);
  if( sc.isFailure() ){
    ATH_MSG_INFO("Failed to retrieve TrigMissingET (rel. <= 14.2.0)");
  }

  for( ; trigMETfirst != trigMETlast ; ++trigMETfirst ){ // loop over TrigMissingET objects
    std::string name(trigMETfirst.key());
    ATH_MSG_INFO("Got TrigMissingET object with key \"" << name << "\"");

    std::string s;
    char buff[3000];

    if( name.find("TrigEFMissingET") != std::string::npos ) {
      s="REGTEST EF: ";
    } else if( name.find("T2MissingET") != std::string::npos ){
      s="REGTEST L2: ";
    } else {
      ATH_MSG_WARNING(" This is UNKNOWN! " << name);
      s="REGTEST ??? ";
    }

    snprintf(buff, sizeof(buff), "%s Ex =         %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->ex() );
    ATH_MSG_INFO(buff);
    snprintf(buff, sizeof(buff), "%s Ey =         %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->ey() );
    ATH_MSG_INFO(buff);
    snprintf(buff, sizeof(buff), "%s Ez =         %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->ez() );
    ATH_MSG_INFO(buff);
    snprintf(buff, sizeof(buff), "%s Et =         %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->et() );
    ATH_MSG_INFO(buff);
    snprintf(buff, sizeof(buff), "%s SumE =       %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->sumE() );
    ATH_MSG_INFO(buff);
    snprintf(buff, sizeof(buff), "%s SumEt =      %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->sumEt() );
    ATH_MSG_INFO(buff);
    snprintf(buff, sizeof(buff), "%s E =          %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->e() );
    ATH_MSG_INFO(buff);
    snprintf(buff, sizeof(buff), "%s flag =    %10d",       s.c_str(), trigMETfirst->getFlag() );
    ATH_MSG_INFO(buff);
    snprintf(buff, sizeof(buff), "%s RoIword = %10ld",      s.c_str(), trigMETfirst->RoIword() );
    ATH_MSG_INFO(buff);

    unsigned int Nc = trigMETfirst->getNumOfComponents();
    if (Nc > 0) {
      s="REGTEST __name____status_usedChannels__sumOfSigns__calib1_calib0";
      s+="/MeV__ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/CLHEP::MeV";
      ATH_MSG_INFO(s);

      for (unsigned int i=0; i<Nc; ++i) { // loop over components
        std::string name =              trigMETfirst->getNameOfComponent(i);
        const short status =            trigMETfirst->getStatus(i);
        const unsigned short usedChan = trigMETfirst->getUsedChannels(i);
        const short sumOfSigns =        trigMETfirst->getSumOfSigns(i);
        const float calib0 =            trigMETfirst->getComponentCalib0(i);
        const float calib1 =            trigMETfirst->getComponentCalib1(i);
        const float ex =                trigMETfirst->getExComponent(i);
        const float ey =                trigMETfirst->getEyComponent(i);
        const float ez =                trigMETfirst->getEzComponent(i);
        const float sumE =              trigMETfirst->getSumEComponent(i);
        const float sumEt =             trigMETfirst->getSumEtComponent(i);

        snprintf(buff, sizeof(buff),
                 "REGTEST   %s   %6d %12d %10d   %6.2f  %6.3f %10.2f %10.2f %10.2f %10.2f %10.2f",
                 name.c_str(), status, usedChan, sumOfSigns, calib1, calib0,
                 ex, ey, ez, sumE, sumEt);
        ATH_MSG_INFO(buff);
      } // loop over components
    }
  } // loop over TrigMissingET objects

  ATH_MSG_INFO("REGTEST ==========END of TrigMissingET DUMP===========");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrackParticleContainer() {

  ATH_MSG_DEBUG("in dumpTrackParticleContainer()");

  ATH_MSG_INFO("REGTEST ==========START of TrackParticleContainer DUMP===========");

	std::string trackPtags[]={"HLT_InDetTrigParticleCreation_Bjet_EFID",
			"HLT_InDetTrigParticleCreation_Bphysics_EFID",
			"HLT_InDetTrigParticleCreation_Electron_EFID",
			"HLT_InDetTrigParticleCreation_FullScan_EFID",
			"HLT_InDetTrigParticleCreation_Muon_EFID",
			"HLT_InDetTrigParticleCreation_Photon_EFID",
			"HLT_InDetTrigParticleCreation_Tau_EFID"};

	int ntag=7;

	StatusCode returnsc=StatusCode::SUCCESS;

	for (int itag=0; itag<ntag; itag++){
		const Rec::TrackParticleContainer*  pTrackParticleC = nullptr;
		StatusCode sc = evtStore()->retrieve(pTrackParticleC, trackPtags[itag]);
		if (sc.isFailure()) {
          ATH_MSG_INFO("REGTEST No TrackParticleContainer found with tag " << trackPtags[itag]);
			continue;
		}
		ATH_MSG_INFO("TrackParticleContainer found with tag " << trackPtags[itag]
                     << " and size " << pTrackParticleC->size());

		Rec::TrackParticleContainer::const_iterator trackItr  = pTrackParticleC->begin();
		Rec::TrackParticleContainer::const_iterator trackItrE = pTrackParticleC->end();
		for (int ind=1; trackItr != trackItrE; ++trackItr, ind++) {
			const Rec::TrackParticle * trackParticle = (*trackItr);
			ATH_MSG_INFO(" TrackParticle " << ind << " charge "
                         << trackParticle->charge() << " p "
                         << trackParticle->p()<< " eta " << trackParticle->eta()
                         << " phi " <<  trackParticle->phi());            

			/// track
			const Trk::Track * track = trackParticle->originalTrack();
			if ( track ) {
              ATH_MSG_INFO(" Got attached track");
				const Trk::TrackParameters* perigee = track->perigeeParameters();
				if (perigee) {
					const auto& parameterVector = perigee->parameters();
					ATH_MSG_INFO(" q/P " << parameterVector[Trk::qOverP] <<
                                 " theta " << parameterVector[Trk::theta] <<
                                 " phi   " <<parameterVector[Trk::phi]);
				} else {
                  ATH_MSG_INFO(" No perigee attached to track");
				}

			} else {
			  if(   trackWarningNum <= maxRepWarnings ) {
			    ATH_MSG_DEBUG(" No attached track");
			    if(  trackWarningNum == maxRepWarnings) {
			      ATH_MSG_WARNING(" Max attached track warning reached, no further warnings given");
			    }
			    trackWarningNum++;
			  }
			}

			/// track vertex position
			const Trk::VxCandidate * vertex = trackParticle->reconstructedVertex();
			if ( vertex ) {
				const Trk::RecVertex vtx = vertex->recVertex();
				const Amg::Vector3D position = vtx.position();
				ATH_MSG_INFO(" vertex position (" << position[0] << ", " <<
                             position[1] << ", " << position[2] << ") ");
			} else {
			  if(   vertexWarningNum <= maxRepWarnings ) {
			    ATH_MSG_DEBUG(" No attached vertex");
			    if(  vertexWarningNum == maxRepWarnings) {
			      ATH_MSG_WARNING(" Max attached vertex warning reached, no further warnings given");
			    }
			    vertexWarningNum++;
			  }
			}

			const Trk::Perigee* perigee = trackParticle->measuredPerigee();
			if (perigee) {
				const auto& parameters = perigee->parameters();
				ATH_MSG_INFO("Trk::Perigee parameters:");
				ATH_MSG_INFO(" * d_0   : "<< parameters[Trk::d0]      );
				ATH_MSG_INFO(" * z_0   : "<< parameters[Trk::z0]      );
				ATH_MSG_INFO(" * phi   : "<< parameters[Trk::phi]     );
				ATH_MSG_INFO(" * Theta : "<< parameters[Trk::theta]   );
				ATH_MSG_INFO(" * q/p   : "<< parameters[Trk::qOverP]  );
			} else {
              ATH_MSG_WARNING(" No attached perigee");
			}
			/// access to TrackSummary information
			const Trk::TrackSummary* summary = trackParticle->trackSummary();
			if (summary) {
              ATH_MSG_DEBUG("Track summary information:");
              ATH_MSG_DEBUG(" * Number of B layer hits : "<<summary->get(Trk::numberOfBLayerHits));
              ATH_MSG_DEBUG(" * Number of pixel hits : "<<summary->get(Trk::numberOfPixelHits));
              ATH_MSG_DEBUG(" * Number of SCT hits : "<<summary->get(Trk::numberOfSCTHits));
              ATH_MSG_DEBUG(" * Number of TRT hits : "<<summary->get(Trk::numberOfTRTHits));
			}
		}
	}
	return returnsc;
}


//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpLVL1_ROI() {

  ATH_MSG_DEBUG("in dumpLVL1_ROI()");

  ATH_MSG_INFO("REGTEST ==========START of LVL1_ROI DUMP===========");

  const LVL1_ROI * lvl1ROI;
  StatusCode sc = evtStore()->retrieve(lvl1ROI);
  if (sc.isFailure() ) {
    ATH_MSG_INFO("REGTEST No LVL1_ROI found");
    return  StatusCode::SUCCESS;
  }

  ATH_MSG_INFO("REGTEST LVL1_ROI retrieved");

  LVL1_ROI::emtaus_type::const_iterator itEMTau   =
    (lvl1ROI->getEmTauROIs()).begin();
  LVL1_ROI::emtaus_type::const_iterator itEMTau_e =
    (lvl1ROI->getEmTauROIs()).end();
  int j = 0;
  for( ; itEMTau != itEMTau_e; ++itEMTau, ++j) {
    ATH_MSG_INFO("REGTEST Looking at LVL1_ROI " << j);
    ATH_MSG_INFO("REGTEST LVL1 EmTauROI Eta     is " << itEMTau->getEta());
    ATH_MSG_INFO("REGTEST LVL1 EmTauROI Phi     is " << itEMTau->getPhi());
    ATH_MSG_INFO("REGTEST LVL1 EmTauROI EMClus  is " << itEMTau->getEMClus());
    ATH_MSG_INFO("REGTEST LVL1 EmTauROI TauClus is " << itEMTau->getTauClus());
    ATH_MSG_INFO("REGTEST LVL1 EmTauROI EMIsol  is " << itEMTau->getEMIsol());
    ATH_MSG_INFO("REGTEST LVL1 EmTauROI HadIsol is " << itEMTau->getHadIsol());
    ATH_MSG_INFO("REGTEST LVL1 EmTauROI Core    is " << itEMTau->getCore());
    ATH_MSG_INFO("REGTEST LVL1 EmTauROI HadCore is " << itEMTau->getHadCore());
    ATH_MSG_INFO("REGTEST LVL1 EmTauROI roiWord is " << itEMTau->getROIWord());
  }

  ATH_MSG_INFO("REGTEST ==========END of LVL1_ROI DUMP===========");
  ATH_MSG_DEBUG("dumpLVL1_ROI() succeeded");
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigPhotonContainer() {

  ATH_MSG_DEBUG("in dumpTrigPhotonContainer()");

  ATH_MSG_INFO("REGTEST ==========START of TrigPhotonContainer DUMP===========");

  const DataHandle< TrigPhotonContainer > trigPhoton;
  const DataHandle< TrigPhotonContainer > lastTrigPhoton;

  StatusCode sc = evtStore()->retrieve(trigPhoton,lastTrigPhoton);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigPhotonContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigPhotonContainers retrieved");


  for (int i=0; trigPhoton != lastTrigPhoton; ++trigPhoton, ++i) {

    ATH_MSG_INFO("REGTEST Looking at TrigPhotonContainer " << i);

    TrigPhotonContainer::const_iterator PhotonItr  = trigPhoton->begin();
    TrigPhotonContainer::const_iterator PhotonItrE = trigPhoton->end();

    for (int j=0; PhotonItr != PhotonItrE; ++PhotonItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigPhoton " << j);

      ATH_MSG_INFO("REGTEST TrigPhoton->dPhi() returns " << (*PhotonItr)->dPhi());
      ATH_MSG_INFO("REGTEST TrigPhoton->dEta() returns " << (*PhotonItr)->dEta());
      ATH_MSG_INFO("REGTEST TrigPhoton->rCore() returns " << (*PhotonItr)->rCore());
      ATH_MSG_INFO("REGTEST TrigPhoton->isValid() returns " << (*PhotonItr)->isValid());
      ATH_MSG_INFO("REGTEST TrigPhoton->Et() returns " << (*PhotonItr)->Et());

      if ( (*PhotonItr)->cluster() != NULL ) {
        ATH_MSG_INFO("REGTEST Cluster info: ");
        ATH_MSG_INFO("REGTEST cluster->e() returns " << (*PhotonItr)->cluster()->e());
        ATH_MSG_INFO("REGTEST cluster->e237() returns " << (*PhotonItr)->cluster()->e237());
        ATH_MSG_INFO("REGTEST cluster->e277() returns " << (*PhotonItr)->cluster()->e277());
        ATH_MSG_INFO("REGTEST cluster->fracs1() returns " << (*PhotonItr)->cluster()->fracs1());
        ATH_MSG_INFO("REGTEST cluster->weta2() returns " << (*PhotonItr)->cluster()->weta2());
        ATH_MSG_INFO("REGTEST cluster->ehad() returns " << (*PhotonItr)->cluster()->ehad1());
        ATH_MSG_INFO("REGTEST cluster->emaxs1() returns " << (*PhotonItr)->cluster()->emaxs1());

        ATH_MSG_INFO("REGTEST Looking at P4PtEtaPhiM " << j);

        ATH_MSG_INFO("REGTEST P4PtEtaPhiM->Pt() returns " << (*PhotonItr)->pt());
        ATH_MSG_INFO("REGTEST P4PtEtaPhiM->Eta() returns " << (*PhotonItr)->eta());
        ATH_MSG_INFO("REGTEST P4PtEtaPhiM->Phi() returns " << (*PhotonItr)->phi());
        ATH_MSG_INFO("REGTEST P4PtEtaPhiM->m() returns " << (*PhotonItr)->m());
      }

      // printout variables using the new << operator
      ATH_MSG_INFO("TrigPhoton printout:");
      ATH_MSG_INFO("REGTEST " << (**PhotonItr));

      // do second loop to compare TrigPhotons using comparison operators
      TrigPhotonContainer::const_iterator PhotonItr2  = PhotonItr;
      TrigPhotonContainer::const_iterator PhotonItr2E = trigPhoton->end();

      for (int k=0; PhotonItr2 != PhotonItr2E; ++PhotonItr2, ++k ) {
        // find if TrigPhotons are the same (i.e. have same cluster and track
        if ( (**PhotonItr) == (**PhotonItr2) ) {
          ATH_MSG_INFO("REGTEST TrigPhoton nr. " << j << " is the same as TrigPhoton nr. " << k);
        } else {
          // TrigPhotons are different, print out differences
          std::map<std::string, double> v_diff;
          diff(*(*PhotonItr),*(*PhotonItr2), v_diff);
          std::map<std::string, double>::iterator it=v_diff.begin();
          ATH_MSG_INFO("TrigPhoton " << k << " different form TrigPhoton " << j << " :");
          for (int m=0;it !=v_diff.end();++it, ++m) {
            ATH_MSG_INFO("TrigPhoton Delta_" << (*it).first << " = " << (*it).second);
          }
        }
      }
    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigPhotonContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigPhotonContainer() succeeded");

  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigMuonEFContainer() {

  ATH_MSG_DEBUG("in dumpTrigMuonEFContainer()");

  ATH_MSG_INFO("REGTEST ==========START of TrigMuonEFContainer DUMP===========");

  const DataHandle< TrigMuonEFContainer > trigMuon;
  const DataHandle< TrigMuonEFContainer > lastTrigMuon;

  StatusCode sc = evtStore()->retrieve(trigMuon,lastTrigMuon);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigMuonEFContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigMuonEFContainers retrieved");

  for (int i=0; trigMuon != lastTrigMuon; ++trigMuon, ++i) {

    ATH_MSG_INFO("REGTEST Looking at TrigMuonEFContainer " << i);

    TrigMuonEFContainer::const_iterator MuonItr  = trigMuon->begin();
    TrigMuonEFContainer::const_iterator MuonItrE = trigMuon->end();

    for (int j=0; MuonItr != MuonItrE; ++MuonItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigMuonEF " << j);

      ATH_MSG_INFO("REGTEST TrigMuonEF->muonCode() returns " << (*MuonItr)->MuonCode());
      ATH_MSG_INFO("REGTEST TrigMuonEF->roi() returns " << (*MuonItr)->RoINum());
      ATH_MSG_INFO("REGTEST TrigMuonEF->charge() returns " << (*MuonItr)->Charge());

      ATH_MSG_INFO("REGTEST Looking at P4IPtCotThPhiM " << j);

      ATH_MSG_INFO("REGTEST P4IPtCotThPhiM->iPt() returns " << (*MuonItr)->iPt());
      ATH_MSG_INFO("REGTEST P4IPtCotThPhiM->CotTh() returns " << (*MuonItr)->cotTh());
      ATH_MSG_INFO("REGTEST P4IPtCotThPhiM->Phi() returns " << (*MuonItr)->phi());
      ATH_MSG_INFO("REGTEST P4IPtCotThPhiM->m() returns " << (*MuonItr)->m());
    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigMuonEFContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigMuonEFContainer() succeeded");

  return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpxAODMuonContainer() {

  ATH_MSG_DEBUG("In dumpxAODMuonContainer");

  ATH_MSG_INFO( "REGTEST ==========START of xAOD::MuonContainer DUMP===========" );

  const xAOD::MuonContainer* muonCont=0;
  StatusCode sc = evtStore()->retrieve(muonCont,"HLT_xAOD__MuonContainer_MuonEFInfo");
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No muon container HLT_xAOD__MuonContainer_MuonEFInfo");
    return StatusCode::SUCCESS;
  }

  std::string output = m_muonPrinter->print( *muonCont );
  msg(MSG::INFO) << output << endmsg;

  ATH_MSG_INFO( "REGTEST ==========END of xAOD::MuonContainer DUMP===========" );

  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigMuonEFInfoContainer() {

  ATH_MSG_DEBUG("in dumpTrigMuonEFInfoContainer()");

  ATH_MSG_INFO("REGTEST ==========START of TrigMuonEFInfoContainer DUMP===========");

  const DataHandle< TrigMuonEFInfoContainer > trigMuon;
  const DataHandle< TrigMuonEFInfoContainer > lastTrigMuon;

  StatusCode sc = evtStore()->retrieve(trigMuon,lastTrigMuon);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigMuonEFInfoContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigMuonEFInfoContainers retrieved");


  for (int i=0; trigMuon != lastTrigMuon; ++trigMuon, ++i) {

    ATH_MSG_INFO("REGTEST -+-+-+-+ Looking at TrigMuonEFInfoContainer " << i);

    TrigMuonEFInfoContainer::const_iterator MuonItr  = trigMuon->begin();
    TrigMuonEFInfoContainer::const_iterator MuonItrE = trigMuon->end();

    for (int j=0; MuonItr != MuonItrE; ++MuonItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigMuonEFInfo " << j);

      const TrigMuonEFInfo* muonInfo = (*MuonItr);

      ATH_MSG_INFO("REGTEST Test self equality ");
      if (*muonInfo == *muonInfo) {
        ATH_MSG_INFO("REGTEST passed ");
      }
      else {
        ATH_MSG_INFO("REGTEST failed ");
      }


      if (muonInfo->hasTrack()) {
        ATH_MSG_INFO("REGTEST Test new version ");

        ATH_MSG_INFO("REGTEST hasTracks()=true ");
        const TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();
        ATH_MSG_INFO("REGTEST TrackContainer size: " <<  tc->size());

        for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = tc->begin() ; TrackItr!=tc->end(); ++TrackItr) {

          const TrigMuonEFInfoTrack* muonInfo = (*TrackItr);
          ATH_MSG_INFO("REGTEST MuonType(): ");

          TrigMuonEFTrack* muonTrack = muonInfo->SpectrometerTrack();
          if (muonTrack) {
            printMuonTrk(muonTrack);
          } else {
            ATH_MSG_INFO("REGTEST no SpectrometerTrack track found");
          }

          ATH_MSG_INFO("REGTEST Looking at TrigMuonEFTrack ExtrapolatedTrack()");
          muonTrack = muonInfo->ExtrapolatedTrack();
          if (muonTrack) {
            printMuonTrk(muonTrack);
          } else {
            ATH_MSG_INFO("REGTEST no ExtrapolatedTrack track found");
          }

          ATH_MSG_INFO("REGTEST Looking at TrigMuonEFTrack CombinedTrack()");
          TrigMuonEFCbTrack* muonCbTrack = muonInfo->CombinedTrack();
          if (muonCbTrack) {
            printMuonTrk(muonTrack);
          } else {
            ATH_MSG_INFO("REGTEST no CombinedTrack track found");
          }
        }

      }

      if (!muonInfo) {
        ATH_MSG_INFO("REGTEST no TrigMuonEFInfo found");
        return StatusCode::SUCCESS;
      }
      ATH_MSG_INFO("REGTEST TrigMuonEFInfo->RoINum() returns " << muonInfo->RoINum());
    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigMuonEFInfoContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigMuonEFInfoContainer() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigMuonEFIsolationContainer() {

  ATH_MSG_DEBUG("in dumpTrigMuonEFIsolationContainer()");

  ATH_MSG_INFO("REGTEST ==========START of TrigMuonEFIsolationContainer DUMP===========");

  const DataHandle< TrigMuonEFIsolationContainer > trigMuon;
  const DataHandle< TrigMuonEFIsolationContainer > lastTrigMuon;

  StatusCode sc = evtStore()->retrieve(trigMuon,lastTrigMuon);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigMuonEFIsolationContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigMuonEFIsolationContainers retrieved");


  for (int i=0; trigMuon != lastTrigMuon; ++trigMuon, ++i) {

    ATH_MSG_INFO("REGTEST -+-+-+-+ Looking at TrigMuonEFIsolationContainer " << i);

    TrigMuonEFIsolationContainer::const_iterator MuonItr  = trigMuon->begin();
    TrigMuonEFIsolationContainer::const_iterator MuonItrE = trigMuon->end();

    for (int j=0; MuonItr != MuonItrE; ++MuonItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigMuonEFIsolation " << j);

      const TrigMuonEFIsolation* muonIsolation = (*MuonItr);

      ATH_MSG_INFO("REGTEST Test self equality ");
      if (*muonIsolation == *muonIsolation) {
        ATH_MSG_INFO("REGTEST passed ");
      }
      else {
        ATH_MSG_INFO("REGTEST failed ");
      }

      ATH_MSG_INFO("REGTEST sumTrkPtCone02(): " << muonIsolation->sumTrkPtCone02());
      ATH_MSG_INFO("REGTEST sumTrkPtCone03(): " << muonIsolation->sumTrkPtCone03());
      ATH_MSG_INFO("REGTEST sumEtCone01()   : " << muonIsolation->sumEtCone01());
      ATH_MSG_INFO("REGTEST sumEtCone02()   : " << muonIsolation->sumEtCone02());
      ATH_MSG_INFO("REGTEST sumEtCone03()   : " << muonIsolation->sumEtCone03());
      ATH_MSG_INFO("REGTEST sumEtCone04()   : " << muonIsolation->sumEtCone04());
      ATH_MSG_INFO("REGTEST trackPosition() : " << muonIsolation->trackPosition() );
      // access MuonInfo* - only works for >=2012 data
      if(muonIsolation->getMuonInfo()) {
        ATH_MSG_INFO("REGTEST Link MuonEFInfo found: " << muonIsolation->getMuonInfo());
        if(!muonIsolation->getEFMuonInfoTrack()) {
          ATH_MSG_WARNING("REGTEST No InfoTrack attached to this EFIsolation object");
        }
        else {
          const TrigMuonEFInfoTrack* trk = muonIsolation->getEFMuonInfoTrack();
          ATH_MSG_INFO("REGTEST Linke EFInfoTrack has MuonType : " << trk->MuonType());
          if(trk->hasCombinedTrack()) ATH_MSG_INFO("REGTEST Linked EFInfoTrack combined pt : " << trk->CombinedTrack()->pt());
          if(trk->hasExtrapolatedTrack()) ATH_MSG_INFO("REGTEST Linked EFInfoTrack extrapolated pt : " << trk->ExtrapolatedTrack()->pt());
          if(trk->hasSpectrometerTrack()) ATH_MSG_INFO("REGTEST Linked EFInfoTrack MS pt : " << trk->SpectrometerTrack()->pt());
          if(trk->hasCombinedTrack()) {
            if(trk->CombinedTrack()->getIDTrackParticle()) 
              ATH_MSG_INFO("REGTEST Linked EFInfoTrack ID track pt : " << trk->CombinedTrack()->getIDTrackParticle()->pt());
          }

        }
      }else {
        ATH_MSG_INFO("REGTEST No Linked MuonEFInfo, expected for pre 2012 data");
      }

    }//TrigMuonEFIsolation loop
  }//TrigMuonEFIsolationContainer loop

  ATH_MSG_INFO("REGTEST ==========END of TrigMuonEFIsolationContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigMuonEFIsolationContainer() succeeded");

  return StatusCode::SUCCESS;

}//dumpTrigMuonEFIsolationContainer


void TrigEDMChecker::printMuonTrk(const TrigMuonEFTrack* muonTrack) {
  ATH_MSG_INFO("POINTER TrigMuonEFTrack: " << muonTrack);
	//if(muonTrack) mLog << MSG::INFO << "REGTEST TrigMuonEFTrack: " << *muonTrack << endmsg;

// 	mLog <<MSG::INFO << "REGTEST TrigMuonEFTrack->charge() returns " << muonTrack->Charge() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->iPt() returns " << muonTrack->iPt() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->CotTh() returns " << muonTrack->cotTh() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->Phi() returns " << muonTrack->phi() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->m() returns " << muonTrack->m() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST chi2() returns " << muonTrack->chi2() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST chi2prob() returns " << muonTrack->chi2prob() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST posX() returns " << muonTrack->posX() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST posY() returns " << muonTrack->posY() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST posZ() returns " << muonTrack->posZ() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST NCscHitsEta() returns " << muonTrack->NCscHitsEta() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST NCscHitsPhi() returns " << muonTrack->NCscHitsPhi() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST NTgcHitsEta() returns " << muonTrack->NTgcHitsEta() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST NTgcHitsPhi() returns " << muonTrack->NTgcHitsPhi() << endmsg;
//  	mLog <<MSG::INFO << "REGTEST NMdtHitsEta() returns " << muonTrack->NMdtHitsEta() << endmsg;
//  	mLog <<MSG::INFO << "REGTEST NMdtHitsPhi() returns " << muonTrack->NMdtHitsPhi() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST NRpcHitsEta() returns " << muonTrack->NRpcHitsEta() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST NRpcHitsPhi() returns " << muonTrack->NRpcHitsPhi() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST d0() returns " << muonTrack->d0() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST z0() returns " << muonTrack->z0() << endmsg;
}

void TrigEDMChecker::printMuonTrk(const TrigMuonEFCbTrack* muonTrack) {
  ATH_MSG_INFO(" POINTER TrigMuonEFCbTrack: " << muonTrack);
	//if(muonTrack) mLog << MSG::INFO << "REGTEST TrigMuonEFCbTrack: " << *muonTrack << endmsg;

// 	printMuonTrk(mLog,(TrigMuonEFTrack*)muonTrack);
//  	mLog <<MSG::INFO << "REGTEST TrigMuonEFCbTrack " << muonTrack << endmsg;
// 	mLog <<MSG::INFO << "REGTEST NIdPixelHits() returns " << muonTrack->NIdPixelHits() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST NIdSctHits() returns " << muonTrack->NIdSctHits() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST NTrtHits() returns " << muonTrack->NTrtHits() << endmsg;
// 	mLog <<MSG::INFO << "REGTEST matchChi2() returns " << muonTrack->matchChi2() << endmsg;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigElectronContainer() {

  ATH_MSG_DEBUG("in dumpTrigElectronContainer()");

  ATH_MSG_INFO("REGTEST ==========START of TrigElectronContainer DUMP===========");

	const DataHandle< TrigElectronContainer > trigElec;
	const DataHandle< TrigElectronContainer > lastTrigElec;

	StatusCode sc = evtStore()->retrieve(trigElec,lastTrigElec);
	if (sc.isFailure()) {
      ATH_MSG_INFO("REGTEST No TrigelectronContainer found");
		return  StatusCode::SUCCESS;
	}
	ATH_MSG_INFO("REGTEST TrigElectronContainers retrieved");

	// declare pointer to the last trigElectron to test comparison operators
	TrigElectronContainer::const_iterator firstTrigEl ;

	for (int i=0; trigElec != lastTrigElec; ++trigElec, ++i) {

	  ATH_MSG_INFO("REGTEST Looking at TrigElectronContainer " << i);

	  TrigElectronContainer::const_iterator elecItr  = trigElec->begin();
	  TrigElectronContainer::const_iterator elecItrE = trigElec->end();

	  for (int j=0; elecItr != elecItrE; ++elecItr, ++j ) {

	    ATH_MSG_INFO("REGTEST Looking at TrigElectron " << j);
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->isValid()      returns " << (*elecItr)->isValid() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->pt()           returns " << (*elecItr)->pt() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->eta()          returns " << (*elecItr)->eta() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->phi()          returns " << (*elecItr)->phi() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->Zvtx()         returns " << (*elecItr)->Zvtx() << endmsg;

	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->charge()       returns " << (*elecItr)->charge() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trackIndx()    returns " << (*elecItr)->trackIndx() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trkClusDphi()  returns " << (*elecItr)->trkClusDphi() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trkClusDeta()  returns " << (*elecItr)->trkClusDeta() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->Pt() (track)   returns " << (*elecItr)->Pt() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->err_Pt()       returns " << (*elecItr)->err_Pt() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->err_eta()      returns " << (*elecItr)->err_eta() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->err_phi()      returns " << (*elecItr)->err_phi() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->err_Zvtx()     returns " << (*elecItr)->err_Zvtx() << endmsg;

	    // 			mLog <<MSG::INFO << "REGTEST New methods: " << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->roiWord()      returns " << (*elecItr)->roiWord() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trkEtaAtCalo() returns " << (*elecItr)->trkEtaAtCalo() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trkPhiAtCalo() returns " << (*elecItr)->trkPhiAtCalo() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->caloEta()      returns " << (*elecItr)->caloEta() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->caloPhi()      returns " << (*elecItr)->caloPhi() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->Rcore()        returns " << (*elecItr)->Rcore() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->Eratio()       returns " << (*elecItr)->Eratio() << endmsg;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->EThad()        returns " << (*elecItr)->EThad() << endmsg;

	    // printout variables using the new << operator
	    ATH_MSG_INFO("TrigElectron printout:");
	    ATH_MSG_INFO("REGTEST " << (**elecItr));

	    // do second loop to compare TrigElectrons using comparison operators
	    TrigElectronContainer::const_iterator elecItr2  = elecItr; //trigElec->begin();
	    TrigElectronContainer::const_iterator elecItr2E = trigElec->end();

	    for (int k=0; elecItr2 != elecItr2E; ++elecItr2, ++k ) {
	      // find if TrigElectrons are the same (i.e. have same cluster and track
	      if ( (**elecItr) == (**elecItr2) ) {
            ATH_MSG_INFO("REGTEST TrigElectron nr. " << j << " is the same as TrigElectron nr. " << k);
	      } else {
            // TrigElectrons are different, print out differences
            std::map<std::string, double> v_diff;
            diff(*(*elecItr),*(*elecItr2), v_diff);
            std::map<std::string, double>::iterator it=v_diff.begin();
            ATH_MSG_INFO("TrigElectron " << k << " different form TrigElectron " << j << " :");
            for (int m=0;it !=v_diff.end();++it, ++m) {
              ATH_MSG_INFO("TrigElectron Delta_" << (*it).first << " = " << (*it).second);
            }
	      }
	    }
	  }
	}

	ATH_MSG_INFO("REGTEST ==========END of TrigElectronContainer DUMP===========");
    ATH_MSG_DEBUG("dumpTrigElectronContainer() succeeded");

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpxAODTrigElectronContainer() {

  ATH_MSG_DEBUG("In dumpxAODElectronContainer");

  ATH_MSG_INFO( "REGTEST ==========START of xAOD::TrigElectronContainer DUMP===========" );

  const xAOD::TrigElectronContainer* elCont=0;
  StatusCode sc = evtStore()->retrieve(elCont,"HLT_xAOD__TrigElectronContainer_L2ElectronFex");
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigElectron container HLT_xAOD__TrigElectronContainer_L2ElectronFex");
    return StatusCode::SUCCESS;
  }

  for (const auto& eg : *elCont){
      ATH_MSG_INFO("REGTEST TrigElectron->Phi() returns " << eg->phi());
      ATH_MSG_INFO("REGTEST TrigElectron->Eta() returns " << eg->eta());
      ATH_MSG_INFO("REGTEST TrigElectron->rEta returns " << eg->rcore());
      ATH_MSG_INFO("REGTEST TrigElectron->eratio() returns " << eg->eratio());
      ATH_MSG_INFO("REGTEST TrigElectron->pt() returns " << eg->pt());
      ATH_MSG_INFO("REGTEST TrigElectron->etHad() returns " << eg->etHad());
      ATH_MSG_INFO("REGTEST TrigElectron->f1() returns " << eg->f1());
      ATH_MSG_INFO("REGTEST TrigElectron caloEta = " << eg->caloEta());
      ATH_MSG_INFO("REGTEST TrigElectron dPhiCalo" << eg->trkClusDphi());
      ATH_MSG_INFO("REGTEST TrigElectron dEtaCalo" << eg->trkClusDeta());
      ATH_MSG_INFO("REGTEST TrigElectron pTcalo" << eg->pt());
      ATH_MSG_INFO("REGTEST TrigElectron eTOverPt" << eg->etOverPt());
      ATH_MSG_INFO("REGTEST TrigElectron nTRTHits" << eg->nTRTHits());
      ATH_MSG_INFO("REGTEST TrigElectron nStrawHits" << eg->nTRTHiThresholdHits());
      ATH_MSG_INFO("REGTEST TrigElectron Check EMCluster");
      if(eg->emCluster()){
          ATH_MSG_INFO("REGTEST TrigElectron EMCluster retrieved");
          ATH_MSG_INFO("REGTEST TrigElectron emCluster->energy() returns " << eg->emCluster()->energy());
          ATH_MSG_INFO("REGTEST TrigElectron emCluster->phi() returns " << eg->emCluster()->phi());
          ATH_MSG_INFO("REGTEST TrigElectron emCluster->eta() returns " << eg->emCluster()->eta());
          ATH_MSG_INFO("REGTEST TrigElectron emCluster check Element Link");
          ATH_MSG_INFO("REGTEST TrigElectron emCluster energy = " << eg->emCluster()->energy());
          ATH_MSG_INFO("REGTEST TrigElectron ElementLink emCluster energy = " << (*eg->emClusterLink())->energy());
      }
      else  ATH_MSG_INFO("REGTEST TrigElectron No EMCluster retrieved!");
      ATH_MSG_INFO("REGTEST TrigElectron Check TrackParticle");
      if(eg->trackParticle()){
          ATH_MSG_INFO("REGTEST TrigElectron TrackParticle retrieved");
          ATH_MSG_INFO("REGTEST TrigElectron trackParticle->pt() returns " << eg->trackParticle()->pt());
          ATH_MSG_INFO("REGTEST TrigElectron trackParticle->phi() returns " << eg->trackParticle()->phi());
          ATH_MSG_INFO("REGTEST TrigElectron trackParticle->eta() returns " << eg->trackParticle()->eta());
          ATH_MSG_INFO("REGTEST TrigElectron check TrackParticle Element Link");
          ATH_MSG_INFO("REGTEST TrigElectron TrackParticle pt = " << eg->trackParticle()->pt());
          ATH_MSG_INFO("REGTEST TrigElectron ElementLink TrackParticle pt = " << (*eg->trackParticleLink())->pt());
      }
      else  ATH_MSG_INFO("REGTEST TrigElectron No TrackParticle retrieved!");
  }
  ATH_MSG_INFO( "REGTEST ==========END of xAOD::TrigElectronContainer DUMP===========" );

  return StatusCode::SUCCESS;

}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpxAODTrigPhotonContainer() {

  ATH_MSG_DEBUG("In dumpxAODTrigPhotonContainer");

  ATH_MSG_INFO( "REGTEST ==========START of xAOD::TrigPhotonContainer DUMP===========" );

  const xAOD::TrigPhotonContainer* phCont=0;
  StatusCode sc = evtStore()->retrieve(phCont,"HLT_xAOD__TrigPhotonContainer_L2PhotonFex");
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigPhoton container HLT_xAOD__TrigPhotonContainer_L2PhotonFex");
    return StatusCode::SUCCESS;
  }

  for (const auto& eg : *phCont){

      ATH_MSG_INFO("REGTEST TrigPhoton->Phi() returns " << eg->phi());
      ATH_MSG_INFO("REGTEST TrigPhoton->Eta() returns " << eg->eta());
      ATH_MSG_INFO("REGTEST TrigPhoton->dPhi() returns " << eg->dPhi());
      ATH_MSG_INFO("REGTEST TrigPhoton->dEta() returns " << eg->dEta());
      ATH_MSG_INFO("REGTEST TrigPhoton->rEta returns " << eg->rcore());
      ATH_MSG_INFO("REGTEST TrigPhoton->eratio() returns " << eg->eratio());
      ATH_MSG_INFO("REGTEST TrigPhoton->pt() returns " << eg->pt());
      ATH_MSG_INFO("REGTEST TrigPhoton->etHad() returns " << eg->etHad());
      ATH_MSG_INFO("REGTEST TrigPhoton->f1() returns " << eg->f1());
      ATH_MSG_INFO("REGTEST TrigPhoton Check EMCluster");
      if(eg->emCluster()){
          ATH_MSG_INFO("REGTEST TrigPhoton EMCluster retrieved");
          ATH_MSG_INFO("REGTEST TrigPhoton emCluster->energy() returns " << eg->emCluster()->energy());
          ATH_MSG_INFO("REGTEST TrigPhoton emCluster->phi() returns " << eg->emCluster()->phi());
          ATH_MSG_INFO("REGTEST TrigPhoton emCluster->eta() returns " << eg->emCluster()->eta());
          ATH_MSG_INFO("REGTEST TrigPhoton emCluster check Element Link");
          ATH_MSG_INFO("REGTEST TrigPhoton emCluster energy = " << eg->emCluster()->energy());
          ATH_MSG_INFO("REGTEST TrigPhoton ElementLink emCluster energy = " << (*eg->emClusterLink())->energy());
      }
      else  ATH_MSG_INFO("REGTEST TrigPhoton No EMCluster retrieved!");
  }
  ATH_MSG_INFO( "REGTEST ==========END of xAOD::TrigPhotonContainer DUMP===========" );

  return StatusCode::SUCCESS;

}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpxAODElectronContainer() {

  ATH_MSG_DEBUG("In dumpxAODElectronContainer");

  ATH_MSG_INFO( "REGTEST ==========START of xAOD::ElectronContainer DUMP===========" );

  const xAOD::ElectronContainer* elCont=0;
  StatusCode sc = evtStore()->retrieve(elCont,"HLT_xAOD__ElectronContainer_egamma_Electrons");
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No Electron container HLT_xAOD__ElectronContainer_egamma_Electrons");
    return StatusCode::SUCCESS;
  }
  float val_float=-99;
  unsigned int isEMbit=0;
  bool pid=false;
  //DEBUG output for Egamma container
  ATH_MSG_INFO(" REGTEST: xAOD Reconstruction variables: ");
  //                //Cluster and ShowerShape info
  for (const auto& eg : *elCont){
      //REGTEST printout
      if (eg) {
          ATH_MSG_INFO(" REGTEST: egamma energy: " << eg->e() );
          ATH_MSG_INFO(" REGTEST: egamma eta: " << eg->eta() );
          ATH_MSG_INFO(" REGTEST: egamma phi: " << eg->phi() );
          if(eg->selectionisEM(isEMbit,"isEMVLoose"))
              ATH_MSG_INFO(" REGTEST: isEMVLoose " << std::hex << isEMbit << std::dec);
          else ATH_MSG_WARNING(" REGTEST: Missing Aux info");
          if(eg->selectionisEM(isEMbit,"isEMLoose"))
              ATH_MSG_INFO(" REGTEST: isEMLoose " << std::hex << isEMbit << std::dec);
          else ATH_MSG_WARNING(" REGTEST: Missing Aux info");
          if(eg->selectionisEM(isEMbit,"isEMMedium"))
              ATH_MSG_INFO(" REGTEST: isEMMedium " << std::hex << isEMbit << std::dec);
          else ATH_MSG_WARNING(" REGTEST: Missing Aux info");
          if(eg->selectionisEM(isEMbit,"isEMTight"))
              ATH_MSG_INFO(" REGTEST: isEMTight " << std::hex << isEMbit << std::dec);
          else ATH_MSG_WARNING(" REGTEST: Missing Aux info");
          if(eg->selectionisEM(isEMbit,"isEMLHVLoose"))
              ATH_MSG_INFO(" REGTEST: isEMLHVLoose " << std::hex << isEMbit << std::dec);
          else ATH_MSG_WARNING(" REGTEST: Missing Aux info");
          if(eg->selectionisEM(isEMbit,"isEMLHLoose"))
              ATH_MSG_INFO(" REGTEST: isEMLHLoose " << std::hex << isEMbit << std::dec);
          else ATH_MSG_WARNING(" REGTEST: Missing Aux info");
          if(eg->selectionisEM(isEMbit,"isEMLHMedium"))
              ATH_MSG_INFO(" REGTEST: isEMLHMedium " << std::hex << isEMbit << std::dec);
          else ATH_MSG_WARNING(" REGTEST: Missing Aux info");
          if(eg->selectionisEM(isEMbit,"isEMLHTight"))
              ATH_MSG_INFO(" REGTEST: isEMLHTight " << std::hex << isEMbit << std::dec);
          else ATH_MSG_WARNING(" REGTEST: Missing Aux info");
          if(eg->likelihoodValue(val_float,"LHValue"))
              ATH_MSG_INFO(" REGTEST: LHValue " << val_float);
          else
              ATH_MSG_WARNING(" REGTEST: Missing Aux info ");
          if(eg->likelihoodValue(val_float,"LHCaloValue"))
              ATH_MSG_INFO(" REGTEST: LHValue " << val_float);
          else
              ATH_MSG_WARNING(" REGTEST: Missing Aux info ");
          if(eg->passSelection(pid,"LHVLoose"))
              ATH_MSG_INFO(" REGTEST: LHVLoose " << pid); 
          else
              ATH_MSG_WARNING(" REGTEST: Missing Aux info ");
          if(eg->passSelection(pid,"LHLoose"))
              ATH_MSG_INFO(" REGTEST: LHLoose " << pid); 
          else
              ATH_MSG_WARNING(" REGTEST: Missing Aux info ");
          if(eg->passSelection(pid,"LHMedium"))
              ATH_MSG_INFO(" REGTEST: LHMedium " << pid); 
          else
              ATH_MSG_WARNING(" REGTEST: Missing Aux info ");
          if(eg->passSelection(pid,"LHTight"))
              ATH_MSG_INFO(" REGTEST: LHTight " << pid); 
          else
              ATH_MSG_WARNING(" REGTEST: Missing Aux info ");
      } else{
          ATH_MSG_INFO(" REGTEST: problems with egamma pointer" );
          return StatusCode::SUCCESS;
      }
      ATH_MSG_INFO(" REGTEST: caloCluster variables ");
      if (eg->caloCluster()) {
          ATH_MSG_INFO(" REGTEST: egamma cluster transverse energy: " << eg->caloCluster()->et() );
          ATH_MSG_INFO(" REGTEST: egamma cluster eta: " << eg->caloCluster()->eta() );
          ATH_MSG_INFO(" REGTEST: egamma cluster phi: " << eg->caloCluster()->phi() );
          double tmpeta = -999.;
          double tmpphi = -999.;
          eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
          eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi); 
          ATH_MSG_INFO(" REGTEST: egamma cluster calo-frame coords. etaCalo = " << tmpeta); 
          ATH_MSG_INFO(" REGTEST: egamma cluster calo-frame coords. phiCalo = " << tmpphi);
      } else{
          ATH_MSG_INFO(" REGTEST: problems with egamma cluster pointer" );
      }
      ATH_MSG_INFO("REGTEST: Check the original (uncalibrated)");
      static SG::AuxElement::Accessor<ElementLink<xAOD::CaloClusterContainer> > orig ("originalCaloCluster");
      if (!orig.isAvailable(*eg->caloCluster()) || !orig(*eg->caloCluster()).isValid()){
          ATH_MSG_INFO("Problem with original cluster link");
      }
      else {
          const xAOD::CaloCluster *origClus = *orig(*eg->caloCluster());
          ATH_MSG_INFO("REGTEST:: Compare new and old clusters");
          ATH_MSG_INFO("REGTEST:: Original Cluster e,eta,phi" << origClus->e() << " " <<  origClus->eta() << " " << origClus->phi());
          ATH_MSG_INFO("REGTEST:: MVA      Cluster e,eta,phi" << eg->caloCluster()->e() << " " <<  eg->caloCluster()->eta() << " " << eg->caloCluster()->phi());
      }
      ATH_MSG_INFO(" REGTEST: trackmatch variables ");
      if(eg->trackParticle()){
          ATH_MSG_INFO(" REGTEST: pt=  " << eg->trackParticle()->pt());
          ATH_MSG_INFO(" REGTEST: charge=  " << eg->trackParticle()->charge());
          ATH_MSG_INFO(" REGTEST: E/p=  " << eg->caloCluster()->et() / eg->trackParticle()->pt() );
          eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaEta1);
          ATH_MSG_INFO(" REGTEST: Delta eta 1st sampling=  " << val_float);
          eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaPhi2);
          ATH_MSG_INFO(" REGTEST: Delta phi 2nd sampling=  " << val_float);
      } else{
          ATH_MSG_INFO(" REGTEST: no electron eg->trackParticle() pointer");
      }

      //msg() << MSG::VERBOSE << " REGTEST: cluster variables " << endmsg;
      //clus = eg->caloCluster();
      ATH_MSG_INFO(" REGTEST: EMShower variables ");
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::ethad);
      ATH_MSG_INFO(" REGTEST: ethad    =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e011);
      ATH_MSG_INFO(" REGTEST: e011     =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e132);
      ATH_MSG_INFO(" REGTEST: e132     =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e237);
      ATH_MSG_INFO(" REGTEST: e237     =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e335);
      ATH_MSG_INFO(" REGTEST: e335     =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2ts1);
      ATH_MSG_INFO(" REGTEST: e2ts1    =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2tsts1);
      ATH_MSG_INFO(" REGTEST: e2tsts1  =  " << val_float);
      eg->isolationValue(val_float,xAOD::Iso::ptcone20);
      ATH_MSG_INFO(" REGTEST: ptcone20   =  " << val_float);
      eg->isolationValue(val_float,xAOD::Iso::ptcone30);
      ATH_MSG_INFO(" REGTEST: ptcone30   =  " << val_float);
      eg->isolationValue(val_float,xAOD::Iso::ptcone40);
      ATH_MSG_INFO(" REGTEST: ptcone40   =  " << val_float);
      eg->isolationValue(val_float,xAOD::Iso::etcone20);
      ATH_MSG_INFO(" REGTEST: etcone20   =  " << val_float);
      eg->isolationValue(val_float,xAOD::Iso::etcone30);
      ATH_MSG_INFO(" REGTEST: etcone30   =  " << val_float);
      eg->isolationValue(val_float,xAOD::Iso::etcone40);
      ATH_MSG_INFO(" REGTEST: etcone40   =  " << val_float);
      //DEBUG info for Electrons which by definition have a track match

  }
  ATH_MSG_INFO( "REGTEST ==========END of xAOD::ElectronContainer DUMP===========" );

  return StatusCode::SUCCESS;

}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpxAODPhotonContainer() {

  ATH_MSG_DEBUG("In dumpxAODPhotonContainer");

  ATH_MSG_INFO( "REGTEST ==========START of xAOD::PhotonContainer DUMP===========" );

  const xAOD::PhotonContainer* phCont=0;
  StatusCode sc = evtStore()->retrieve(phCont,"HLT_xAOD__PhotonContainer_egamma_Photons");
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No Photon container HLT_xAOD__PhotonContainer_egamma_Photons");
    return StatusCode::SUCCESS;
  }

  float val_float=-99;
  unsigned int isEMbit=0;
  //DEBUG output for xAOD::PhotonContainer
  ATH_MSG_INFO(" REGTEST: xAOD Reconstruction variables: ");
  //                //Cluster and ShowerShape info
  for (const auto& eg : *phCont){
      //REGTEST printout
      if (eg) {
          ATH_MSG_INFO(" REGTEST: egamma energy: " << eg->e() );
          ATH_MSG_INFO(" REGTEST: egamma eta: " << eg->eta() );
          ATH_MSG_INFO(" REGTEST: egamma phi: " << eg->phi() );
          ATH_MSG_INFO(" REGTEST: isEMLoose " << eg->selectionisEM(isEMbit,"isEMLoose"));
          ATH_MSG_INFO(" REGTEST: isEMLoose bit " << std::hex << isEMbit << std::dec);
          ATH_MSG_INFO(" REGTEST: isEMMedium " << eg->selectionisEM(isEMbit,"isEMMedium"));
          ATH_MSG_INFO(" REGTEST: isEMMedium bit " << std::hex << isEMbit << std::dec);
          ATH_MSG_INFO(" REGTEST: isEMTight " << eg->selectionisEM(isEMbit,"isEMTight"));
          ATH_MSG_INFO(" REGTEST: isEMTight bit " << std::hex << isEMbit << std::dec);
      } else{
          ATH_MSG_INFO(" REGTEST: problems with egamma pointer" );
          return StatusCode::SUCCESS;
      }
      ATH_MSG_INFO(" REGTEST: caloCluster variables ");
      if (eg->caloCluster()) {
          ATH_MSG_INFO(" REGTEST: egamma cluster transverse energy: " << eg->caloCluster()->et() );
          ATH_MSG_INFO(" REGTEST: egamma cluster eta: " << eg->caloCluster()->eta() );
          ATH_MSG_INFO(" REGTEST: egamma cluster phi: " << eg->caloCluster()->phi() );
          double tmpeta = -999.;
          double tmpphi = -999.;
          eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
          eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi); 
          ATH_MSG_INFO(" REGTEST: egamma cluster calo-frame coords. etaCalo = " << tmpeta); 
          ATH_MSG_INFO(" REGTEST: egamma cluster calo-frame coords. phiCalo = " << tmpphi);
      } else{
          ATH_MSG_INFO(" REGTEST: problems with egamma cluster pointer" );
      }
      ATH_MSG_INFO("REGTEST: Check the original (uncalibrated)");
      static SG::AuxElement::Accessor<ElementLink<xAOD::CaloClusterContainer> > orig ("originalCaloCluster");
      if (!orig.isAvailable(*eg->caloCluster()) || !orig(*eg->caloCluster()).isValid()){
          ATH_MSG_INFO("Problem with original cluster link");
      }
      else {
          const xAOD::CaloCluster *origClus = *orig(*eg->caloCluster());
          ATH_MSG_INFO("REGTEST:: Compare new and old clusters");
          ATH_MSG_INFO("REGTEST:: Original Cluster e,eta,phi" << origClus->e() << " " <<  origClus->eta() << " " << origClus->phi());
          ATH_MSG_INFO("REGTEST:: MVA      Cluster e,eta,phi" << eg->caloCluster()->e() << " " <<  eg->caloCluster()->eta() << " " << eg->caloCluster()->phi());
      }
      //msg() << MSG::VERBOSE << " REGTEST: cluster variables " << endmsg;
      //clus = eg->caloCluster();
      ATH_MSG_INFO(" REGTEST: EMShower variables ");
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::ethad);
      ATH_MSG_INFO(" REGTEST: ethad    =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e011);
      ATH_MSG_INFO(" REGTEST: e011     =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e132);
      ATH_MSG_INFO(" REGTEST: e132     =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e237);
      ATH_MSG_INFO(" REGTEST: e237     =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e335);
      ATH_MSG_INFO(" REGTEST: e335     =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2ts1);
      ATH_MSG_INFO(" REGTEST: e2ts1    =  " << val_float);
      eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2tsts1);
      ATH_MSG_INFO(" REGTEST: e2tsts1  =  " << val_float);
      eg->isolationValue(val_float,xAOD::Iso::etcone20);
      ATH_MSG_INFO(" REGTEST: etcone20   =  " << val_float);
      eg->isolationValue(val_float,xAOD::Iso::etcone30);
      ATH_MSG_INFO(" REGTEST: etcone30   =  " << val_float);
      eg->isolationValue(val_float,xAOD::Iso::etcone40);
      ATH_MSG_INFO(" REGTEST: etcone40   =  " << val_float);
      //DEBUG info for Electrons which by definition have a track match

  }
  ATH_MSG_INFO( "REGTEST ==========END of xAOD::PhotonContainer DUMP===========" );

  return StatusCode::SUCCESS;

}
//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigTauContainer() {

  ATH_MSG_DEBUG("in dumpTrigTauContainer()");
  ATH_MSG_INFO("REGTEST ==========START of TrigTauContainer DUMP===========");

  const DataHandle< TrigTauContainer > trigTau;
  const DataHandle< TrigTauContainer > lastTrigTau;

  StatusCode sc = evtStore()->retrieve(trigTau,lastTrigTau);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigTauContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigTauContainer retrieved");

  for (int i=0; trigTau != lastTrigTau; ++trigTau, ++i) {

    ATH_MSG_INFO("REGTEST Looking at TrigTauContainer " << i);

    TrigTauContainer::const_iterator TrigTauItr  = trigTau->begin();
    TrigTauContainer::const_iterator TrigTauItrE = trigTau->end();

    for (int j=0; TrigTauItr != TrigTauItrE; ++TrigTauItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigTau " << j);

      ATH_MSG_INFO("REGTEST TrigTau->pt() returns " << (*TrigTauItr)->pt());
      ATH_MSG_INFO("REGTEST TrigTau->eta() returns " << (*TrigTauItr)->eta());
      ATH_MSG_INFO("REGTEST TrigTau->phi() returns " << (*TrigTauItr)->phi());

      ATH_MSG_INFO("REGTEST TrigTau->nMatchedTracks() returns " << (*TrigTauItr)->nMatchedTracks());
      ATH_MSG_INFO("REGTEST TrigTau->simpleEtFlow() returns " << (*TrigTauItr)->simpleEtFlow());
      ATH_MSG_INFO("REGTEST TrigTau->m() returns " << (*TrigTauItr)->m());
    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigTau DUMP===========");
  ATH_MSG_DEBUG("dumpTrigTauContainer() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpTrigTauTracksInfo() {

  ATH_MSG_DEBUG("in dumpTrigTauTracksInfo()");
  ATH_MSG_INFO("REGTEST ==========START of TrigTauTracksInfo DUMP===========");

  const DataHandle< TrigTauTracksInfo > trigTau;
  const DataHandle< TrigTauTracksInfo > lastTrigTau;

  StatusCode sc = evtStore()->retrieve(trigTau,lastTrigTau);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigTauTracksInfo found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigTauTracksInfos retrieved");

  for (int i=0; trigTau != lastTrigTau; ++trigTau, ++i) {

    const TrigTauTracksInfo* thisTrigTau = &(*trigTau);

    ATH_MSG_INFO("REGTEST Looking at TrigTauTracksInfo " << i);

    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->pt() returns " << thisTrigTau->pt());
    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->eta() returns " << thisTrigTau->eta());
    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->phi() returns " << thisTrigTau->phi());

    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->nCoreTracks() returns " << thisTrigTau->nCoreTracks());
    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->nSlowTracks() returns " << thisTrigTau->nSlowTracks());
    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->nIsoTracks() returns " << thisTrigTau->nIsoTracks());

    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->charge() returns " << thisTrigTau->charge());
    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->leadingTrackPt() returns " <<
                 thisTrigTau->leadingTrackPt());
    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->scalarPtSumCore() returns " <<
                 thisTrigTau->scalarPtSumCore());
    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->scalarPtSumIso() returns " <<
                 thisTrigTau->scalarPtSumIso());
    ATH_MSG_INFO("REGTEST TrigTauTracksInfo->threeFastestTracks().pt() returns " <<
                 thisTrigTau->threeFastestTracks().pt());
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigTauTracksInfo DUMP===========");
  ATH_MSG_DEBUG("dumpTrigTauTracksInfo() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpHLTResult() {

  ATH_MSG_DEBUG("in dumpHLTResult()");

  ATH_MSG_INFO("REGTEST ==========START of HLTResult DUMP===========");

  const HLT::HLTResult* hltResult = 0;
  StatusCode sc=evtStore()->retrieve( hltResult, "HLTResult_L2");
  if( sc.isFailure()  ||  !hltResult ) {
    ATH_MSG_INFO("No HLTResult_L2 found in TDS");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST HLTResult_L2 Successfully Retrieved");

  ATH_MSG_INFO("REGTEST HLTResult_L2->isEmpty() returns " << hltResult->isEmpty());
  ATH_MSG_INFO("REGTEST HLTResult_L2->size() returns " << hltResult->size());
  ATH_MSG_INFO("REGTEST HLTResult_L2->isPassThrough() returns " << hltResult->isPassThrough());
  ATH_MSG_INFO("REGTEST HLTResult_L2->isAccepted() returns " << hltResult->isAccepted());

  ATH_MSG_INFO("REGTEST ==========END of HLTResult DUMP===========");

  ATH_MSG_DEBUG("dumpHLTResult() succeeded");

  return StatusCode::SUCCESS;
}




//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigInDetTrackCollection() {

  std::string TrigInDetTrackTags[]={ "HLT_TrigIDSCAN_Jet",
                                     "HLT_TrigIDSCAN_Tau",
                                     "HLT_TrigIDSCAN_eGamma",
                                     "HLT_TrigIDSCAN_Muon",
                                     "HLT_TrigIDSCAN_muonIso",
                                     "HLT_TrigIDSCAN_Bphysics",
                                     "HLT_TrigIDSCAN_FullScan",
                                     "HLT_TrigIDSCAN_Cosmics",
                                     "HLT_TrigIDSCAN_eGamma_Brem",
                                     "HLT_TrigIDSCAN_Tile",
                                     "HLT_TrigSiTrack_eGamma",
                                     "HLT_TrigSiTrack_Muon",
                                     "HLT_TrigSiTrack_muonIso",
                                     "HLT_TrigSiTrack_Tau",
                                     "HLT_TrigSiTrack_Jet",
                                     "HLT_TrigSiTrack_Bphysics",
                                     "HLT_TrigSiTrack_FullScan",
                                     "HLT_TrigSiTrack_Cosmics",
                                     "HLT_TRTSegmentFinder",
                                     "HLT_TRTxK"};
  int ntag=20;

  ATH_MSG_DEBUG("in dumpTrigInDetTrackCollection()");

  const TrigInDetTrackTruthMap* pTruthMap(nullptr);
  bool gotTruthMap = false;

  if (evtStore()->contains<TrigInDetTrackTruthMap>("TrigInDetTrackTruthMap")) {
    StatusCode sc=evtStore()->retrieve(pTruthMap,"TrigInDetTrackTruthMap");
    if (sc.isFailure())
      {
        ATH_MSG_WARNING(" could not retrieve TrackTruthMap with key TrigInDetTruthMap");
      } else {
      gotTruthMap=true;
      //      pTruthMap->print();
    }
  }
  else ATH_MSG_DEBUG(" didn't find any TrackTruthMap objects with key TrigInDetTruthMap");

  ATH_MSG_INFO("REGTEST ==========START of TrigInDetTrackCollection DUMP===========");

  for (int iTag=0; iTag < ntag; iTag++) {
    const TrigInDetTrackCollection* trigInDetTrackCollection = nullptr;
    StatusCode sc = evtStore()->retrieve(trigInDetTrackCollection,TrigInDetTrackTags[iTag] );
    if (sc.isFailure()) {
      ATH_MSG_DEBUG("REGTEST No TrigInDetTrackCollection found with key " << TrigInDetTrackTags[iTag]);
      continue;
    }
    ATH_MSG_INFO("REGTEST TrigInDetTrackCollections retrieved with key "
                 << TrigInDetTrackTags[iTag]);

    ATH_MSG_INFO("REGTEST TrigInDetTrackCollection->size() returns " << trigInDetTrackCollection->size());

    TrigInDetTrackCollection::const_iterator trkItr  = trigInDetTrackCollection->begin();
    TrigInDetTrackCollection::const_iterator trkItrE = trigInDetTrackCollection->end();

    for (int j=0; trkItr != trkItrE; ++trkItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigInDetTrack " << j);

      ATH_MSG_INFO("REGTEST TrigInDetTrack->algorithmId() returns " << (*trkItr)->algorithmId());
      ATH_MSG_INFO("REGTEST TrigInDetTrack->chi2() returns " << (*trkItr)->chi2());
      ATH_MSG_INFO("REGTEST TrigInDetTrack->NStrawHits() returns " << (*trkItr)->NStrawHits());

      ATH_MSG_INFO("REGTEST TrigInDetTrack->NStraw() returns " << (*trkItr)->NStraw());
      ATH_MSG_INFO("REGTEST TrigInDetTrack->NStrawTime() returns " << (*trkItr)->NStrawTime());
      ATH_MSG_INFO("REGTEST TrigInDetTrack->NTRHits() returns " << (*trkItr)->NTRHits());

      ATH_MSG_INFO("REGTEST Looking at TrigInDetFitTrack->param()");
      const TrigInDetTrackFitPar* my_param = (*trkItr)->param();

      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->a0() returns " << my_param->a0());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->z0() returns " << my_param->z0());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->phi0() returns " << my_param->phi0());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->eta() returns " << my_param->eta());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->pT() returns " << my_param->pT());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->ea0() returns " << my_param->ea0());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->ez0() returns " << my_param->ez0());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->ephi0() returns " << my_param->ephi0());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->eeta() returns " << my_param->eeta());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->epT() returns " << my_param->epT());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->surfaceType() returns " << my_param->surfaceType());
      ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->surfaceCoordinate() returns " << my_param->surfaceCoordinate());
      ATH_MSG_INFO("REGTEST Looking at covarience matrix: ");

      const std::vector<double>* const my_cov = my_param->cov();
      if (!my_cov) {
        ATH_MSG_INFO("REGTEST covarience matrix NULL (not stored)");
      } else {
        std::vector<double> tempcov = *my_cov;
        std::vector<double>::iterator iter;
        int k;
        for(iter = tempcov.begin(), k=0; iter != tempcov.end(); ++iter, ++k){
          ATH_MSG_INFO("REGTEST TrigInDetTrackFitPar->cov() element " << k << " is " << (*iter));
        }
      }
      if (gotTruthMap) {
        if (!(pTruthMap->hasTruth((*trkItr)))) {
          ATH_MSG_DEBUG(" REGTEST Track has no truth info  ");
        } else {
          ATH_MSG_DEBUG(" Track has truth info:  ");
          const TrigInDetTrackTruth* pTRTtruth = pTruthMap->truth((*trkItr));
          int nMatches = pTRTtruth->nrMatches() ;
          if (nMatches==0) {
            ATH_MSG_INFO(" REGTEST Track has no truth matches");
          } else {
            ATH_MSG_INFO("REGTEST number of matched truth hits: Si: " <<  pTruthMap->bestMatchSiHits((*trkItr))
                         << " TRT: " <<   pTruthMap->bestMatchTRTHits((*trkItr)));
            if (pTruthMap->bestMatchSiHits((*trkItr)) > 0) {

              const HepMcParticleLink* pSiTruth = pTruthMap->bestMatchSi((*trkItr));

              ATH_MSG_INFO("REGTEST Si match to kine ref " <<  pSiTruth->barcode());
              if (pSiTruth->cptr() != 0) {
                ATH_MSG_INFO("REGTEST Si PDG id " << pSiTruth->cptr()->pdg_id());
              }
            }

            if (pTruthMap->bestMatchTRTHits((*trkItr)) > 0) {
              const HepMcParticleLink* pTRTtruth = pTruthMap->bestMatchTRT((*trkItr));

              ATH_MSG_INFO("REGTEST TRT match to kine ref " <<  pTRTtruth->barcode());
              if (pTRTtruth->cptr() != 0) {
                ATH_MSG_INFO("REGTEST TRT PDG id " << pTRTtruth->cptr()->pdg_id());
              }
            }
          }
        }
      } // if (gotTruthMap)
    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigInDetTrackCollection DUMP===========");
  ATH_MSG_DEBUG("dumpTrigInDetTrackCollection() succeeded");

  return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigVertexCollection() {

  ATH_MSG_DEBUG("in dumpTrigVertexCollection()");
  ATH_MSG_INFO("REGTEST ==========START of TrigVertexCollection DUMP===========");

  const DataHandle< TrigVertexCollection > trigVertex;
  const DataHandle< TrigVertexCollection > lastTrigVertex;

  StatusCode sc = evtStore()->retrieve(trigVertex,lastTrigVertex);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigVertexCollection found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigVertexCollection retrieved");


  for (int i=0; trigVertex != lastTrigVertex; ++trigVertex, ++i) {

    ATH_MSG_INFO("REGTEST Looking at TrigVertexCollection " << i);

    TrigVertexCollection::const_iterator VertexItr  = trigVertex->begin();
    TrigVertexCollection::const_iterator VertexItrE = trigVertex->end();

    for (int j=0; VertexItr != VertexItrE; ++VertexItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigVertex " << j);

      ATH_MSG_INFO("REGTEST TrigVertex->x() returns " << (*VertexItr)->x());
      ATH_MSG_INFO("REGTEST TrigVertex->y() returns " << (*VertexItr)->y());
      ATH_MSG_INFO("REGTEST TrigVertex->z() returns " << (*VertexItr)->z());
      ATH_MSG_INFO("REGTEST TrigVertex->energyFraction() returns " << (*VertexItr)->energyFraction());
      ATH_MSG_INFO("REGTEST TrigVertex->ndof() returns " << (*VertexItr)->ndof());

      if(((*VertexItr)->tracks())!=NULL ){
        //	mLog <<MSG::INFO << "REGTEST *** ((*VertexItr)->tracks())!=NULL *** " << endmsg;
        TrackInVertexList::const_iterator trkItr  = (*VertexItr)->tracks()->begin();
        TrackInVertexList::const_iterator trkItrE = (*VertexItr)->tracks()->end();

        for (int p=0; trkItr != trkItrE; ++trkItr, ++p ) {

          ATH_MSG_INFO("REGTEST Looking at track " << p);

          ATH_MSG_INFO("REGTEST Tracks info: ");
          ATH_MSG_INFO("REGTEST tracks->NStrawHits() returns " << (*trkItr)->NStrawHits());
          ATH_MSG_INFO("REGTEST tracks->NStrawTime() returns " << (*trkItr)->NStrawTime());
          ATH_MSG_INFO("REGTEST tracks->NTRHits() returns " << (*trkItr)->NTRHits());
        }
      }
    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigVertexCollection DUMP===========");
  ATH_MSG_DEBUG("dumpTrigVertexCollection() succeeded");

  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////


StatusCode TrigEDMChecker::dumpTrigEFBphysContainer() {

  ATH_MSG_DEBUG("in dumpTrigEFBphysContainer()");

  ATH_MSG_INFO("REGTEST ==========START of TrigEFBphysContainer DUMP===========");

  std::string EFBphysTags[]={"HLT_xAOD__TrigBphysContainer_EFBMuMuFex",
                             "HLT_xAOD__TrigBphysContainer_EFBMuMuXFex",
                             "HLT_xAOD__TrigBphysContainer_EFDsPhiPiFex",
                             "HLT_xAOD__TrigBphysContainer_EFMuPairs",
                             "HLT_xAOD__TrigBphysContainer_EFMultiMuFex",
                             "HLT_xAOD__TrigBphysContainer_EFTrackMass"
  };

  int ntag= (int) sizeof(EFBphysTags) / sizeof(EFBphysTags[0]);


  for (int itag=0; itag<ntag; itag++){
    const xAOD::TrigBphysContainer*  trigEFBphys;
    StatusCode sc = evtStore()->retrieve(trigEFBphys, EFBphysTags[itag]);
    if (sc.isFailure()) {
      ATH_MSG_INFO("REGTEST No TrigEFBphysContainer found with tag " << EFBphysTags[itag]);
      continue;
    }

    ATH_MSG_INFO("REGTEST TrigEFBphysContainer found with tag " << EFBphysTags[itag]
                 << " and size " << trigEFBphys->size());

    //  for (int i=0; trigEFBphys != lastTrigEFBphys; ++trigEFBphys, ++i) {

    //mLog << MSG::INFO << "REGTEST Looking at TrigEFBphysContainer " << i << endmsg;

    xAOD::TrigBphysContainer::const_iterator EFBphysItr  = trigEFBphys->begin();
    xAOD::TrigBphysContainer::const_iterator EFBphysItrE = trigEFBphys->end();

    for (int j=0; EFBphysItr != EFBphysItrE; ++EFBphysItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigEFBphys " << j);

      ATH_MSG_INFO("REGTEST TrigEFBphys->eta() returns " << (*EFBphysItr)->eta());
      ATH_MSG_INFO("REGTEST TrigEFBphys->phi() returns " << (*EFBphysItr)->phi());
      ATH_MSG_INFO("REGTEST TrigEFBphys->mass() returns " << (*EFBphysItr)->mass());
      ATH_MSG_INFO("REGTEST TrigEFBphys->fitmass() returns " << (*EFBphysItr)->fitmass());
      // ATH_MSG_INFO("REGTEST TrigEFBphys->isValid() returns " << (*EFBphysItr)->isValid());
      ATH_MSG_INFO("REGTEST TrigEFBphys->roiId() returns " << (*EFBphysItr)->roiId());
      ATH_MSG_INFO("REGTEST TrigEFBphys->particleType() returns " << (*EFBphysItr)->particleType());

      if( (*EFBphysItr)->secondaryDecay() != NULL){
        const xAOD::TrigBphys * psecond =(*EFBphysItr)->secondaryDecay();
        ATH_MSG_INFO("REGTEST Secondary decay info: ");
        ATH_MSG_INFO("REGTEST pSecondDecay->eta() returns " << psecond->eta());
        ATH_MSG_INFO("REGTEST pSecondDecay->phi() returns " << psecond->phi());
        ATH_MSG_INFO("REGTEST pSecondDecay->mass() returns " << psecond->mass());
        ATH_MSG_INFO("REGTEST pSecondDecay->fitmass() returns " << psecond->fitmass());
        // ATH_MSG_INFO("REGTEST pSecondDecay->isValid() returns " << (*EFBphysItr)->secondaryDecayLink()->isValid());
        ATH_MSG_INFO("REGTEST pSecondDecay->roiId() returns " << psecond->roiId());
        ATH_MSG_INFO("REGTEST pSecondDecay->particleType() returns " << psecond->particleType());

      } // end if secondary exists



      const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*EFBphysItr)->trackParticleLinks();
      if (trackVector.size() != 0) {
        ATH_MSG_INFO(" REGTEST got track vector size: " << trackVector.size());
      } else {
        ATH_MSG_INFO(" REGTEST no track vector!!! " );
      }
      std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
      for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
        if (!(trkIt->isValid())) {
          ATH_MSG_WARNING("TrackParticleContainer::Invalid ElementLink to track ");
          continue;
        }
        //const Trk::Perigee* trackPerigee=(*(*trkIt))->measuredPerigee();
        const Trk::Perigee* trackPerigee=&((*(*trkIt))->perigeeParameters());

        //      msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endmsg;
        double phi = trackPerigee->parameters()[Trk::phi];
        double theta = trackPerigee->parameters()[Trk::theta];
        double px = trackPerigee->momentum()[Trk::px];
        double py = trackPerigee->momentum()[Trk::py];
        double pt = sqrt(px*px + py*py);
        double eta = -std::log(tan(theta/2));

        ATH_MSG_INFO("track " << itrk << " pt phi eta " << pt << " " <<
                     phi << " " << eta);
      }

    }
  }
  ATH_MSG_INFO("REGTEST ==========END of TrigEFBphysContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigEFBphysContainer() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpTrigL2BphysContainer() {

  ATH_MSG_DEBUG("in dumpTrigL2BphysContainer()");
  ATH_MSG_INFO("REGTEST ==========START of TrigL2BphysContainer DUMP===========");

  std::string L2BphysTags[]={"HLT_xAOD__TrigBphysContainer_L2BMuMuFex",
                             "HLT_xAOD__TrigBphysContainer_L2BMuMuXFex",
                             "HLT_xAOD__TrigBphysContainer_L2DsPhiPiFexDs",
                             "HLT_xAOD__TrigBphysContainer_L2DsPhiPiFexPhi",
                             "HLT_xAOD__TrigBphysContainer_L2JpsieeFex",
                             "HLT_xAOD__TrigBphysContainer_L2MultiMuFex",
                             "HLT_xAOD__TrigBphysContainer_L2TrackMass",
  };
  const int ntag = (int) sizeof(L2BphysTags) / sizeof(L2BphysTags[0]);


  for (int itag=0; itag<ntag; itag++){
    const xAOD::TrigBphysContainer*  trigL2Bphys;
    StatusCode sc = evtStore()->retrieve(trigL2Bphys, L2BphysTags[itag]);
    if (sc.isFailure()) {
      ATH_MSG_INFO("REGTEST No TrigL2BphysContainer found with tag " << L2BphysTags[itag]);
      continue;
    }

    ATH_MSG_INFO("REGTEST TrigL2BphysContainer found with tag " << L2BphysTags[itag]
                 << " and size " << trigL2Bphys->size());

    //  for (int i=0; trigL2Bphys != lastTrigL2Bphys; ++trigL2Bphys, ++i) {

    // mLog << MSG::INFO << "REGTEST Looking at TrigL2BphysContainer " << i << endmsg;

    xAOD::TrigBphysContainer::const_iterator L2BphysItr  = trigL2Bphys->begin();
    xAOD::TrigBphysContainer::const_iterator L2BphysItrE = trigL2Bphys->end();

    for (int j=0; L2BphysItr != L2BphysItrE; ++L2BphysItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigL2Bphys " << j);

      ATH_MSG_INFO("REGTEST TrigL2Bphys->eta() returns " << (*L2BphysItr)->eta());
      ATH_MSG_INFO("REGTEST TrigL2Bphys->phi() returns " << (*L2BphysItr)->phi());
      ATH_MSG_INFO("REGTEST TrigL2Bphys->mass() returns " << (*L2BphysItr)->mass());
      ATH_MSG_INFO("REGTEST TrigL2Bphys->fitmass() returns " << (*L2BphysItr)->fitmass());
      // ATH_MSG_INFO("REGTEST TrigL2Bphys->isValid() returns " << (*L2BphysItr)->isValid());
      ATH_MSG_INFO("REGTEST TrigL2Bphys->roiId() returns " << (*L2BphysItr)->roiId());
      ATH_MSG_INFO("REGTEST TrigL2Bphys->particleType() returns " << (*L2BphysItr)->particleType());

      if( (*L2BphysItr)->secondaryDecay() != NULL){
        const xAOD::TrigBphys * psecond =(*L2BphysItr)->secondaryDecay();
        ATH_MSG_INFO("REGTEST Secondary decay info: ");
        ATH_MSG_INFO("REGTEST pSecondDecay->eta() returns " << psecond->eta());
        ATH_MSG_INFO("REGTEST pSecondDecay->phi() returns " << psecond->phi());
        ATH_MSG_INFO("REGTEST pSecondDecay->mass() returns " << psecond->mass());
        ATH_MSG_INFO("REGTEST pSecondDecay->fitmass() returns " << psecond->fitmass());
        // ATH_MSG_INFO("REGTEST pSecondDecay->isValid() returns " << (*L2BphysItr)->secondaryDecayLink()->isValid());
        ATH_MSG_INFO("REGTEST pSecondDecay->roiId() returns " << psecond->roiId());
        ATH_MSG_INFO("REGTEST pSecondDecay->particleType() returns " << psecond->particleType());
      } // end if secondary exists

      const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*L2BphysItr)->trackParticleLinks();
      if (trackVector.size() != 0) {
        ATH_MSG_INFO(" REGTEST got track vector size: " << trackVector.size());
      } else {
        ATH_MSG_INFO(" REGTEST no track vector!!! " );
      }
      std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
      for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
        if (!(trkIt->isValid())) {
          ATH_MSG_WARNING("TrackParticleContainer::Invalid ElementLink to track ");
          continue;
        }
        //const Trk::Perigee* trackPerigee=(*(*trkIt))->measuredPerigee();
        const Trk::Perigee* trackPerigee=&((*(*trkIt))->perigeeParameters());

        //      msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endmsg;
        double phi = trackPerigee->parameters()[Trk::phi];
        double theta = trackPerigee->parameters()[Trk::theta];
        double px = trackPerigee->momentum()[Trk::px];
        double py = trackPerigee->momentum()[Trk::py];
        double pt = sqrt(px*px + py*py);
        double eta = -std::log(tan(theta/2));
        
        ATH_MSG_INFO("track " << itrk << " pt phi eta " << pt << " " <<
                     phi << " " << eta);
      }
    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigL2BphysContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigL2BphysContainer() succeeded");

  return StatusCode::SUCCESS;
}



//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpxAODJetContainer() {
    
    ATH_MSG_DEBUG("in dumpxAODJetContainer()");
    
    ATH_MSG_INFO("REGTEST ==========START of xAOD::JetContainer DUMP===========");
    
    std::string containerName[30] = {"HLT_xAOD__JetContainer_a4tcemjesFS",
                                     "HLT_xAOD__JetContainer_a4tcemsubFS",
                                     "HLT_xAOD__JetContainer_a4tcemsubjesFS",
                                     "HLT_xAOD__JetContainer_a4tcemnojcalibFS",
                                     "HLT_xAOD__JetContainer_a4tcemjesPS",
                                     "HLT_xAOD__JetContainer_a4tcemnojcalibPS",
                                     "HLT_xAOD__JetContainer_a4tclcwjesFS",
                                     "HLT_xAOD__JetContainer_a4tclcwsubFS",
                                     "HLT_xAOD__JetContainer_a4tclcwsubjesFS",
                                     "HLT_xAOD__JetContainer_a4tclcwnojcalibFS",
                                     "HLT_xAOD__JetContainer_a4tclcwjesPS",
                                     "HLT_xAOD__JetContainer_a4tclcwnojcalibPS",
                                     "HLT_xAOD__JetContainer_a4TTemnojcalibFS",
                                     "HLT_xAOD__JetContainer_a4TThadnojcalibFS",
                                     "HLT_xAOD__JetContainer_a10tcemjesFS",
                                     "HLT_xAOD__JetContainer_a10tcemsubFS",
                                     "HLT_xAOD__JetContainer_a10tcemsubjesFS",
                                     "HLT_xAOD__JetContainer_a10tcemnojcalibFS",
                                     "HLT_xAOD__JetContainer_a10tcemjesPS",
                                     "HLT_xAOD__JetContainer_a10tcemnojcalibPS",
                                     "HLT_xAOD__JetContainer_a10tclcwjesFS",
                                     "HLT_xAOD__JetContainer_a10tclcwsubFS",
                                     "HLT_xAOD__JetContainer_a10tclcwsubjesFS",
                                     "HLT_xAOD__JetContainer_a10tclcwnojcalibFS",
                                     "HLT_xAOD__JetContainer_a10tclcwjesPS",
                                     "HLT_xAOD__JetContainer_a10tclcwnojcalibPS",
                                     "HLT_xAOD__JetContainer_a10TTemnojcalibFS",
                                     "HLT_xAOD__JetContainer_a10TThadnojcalibFS",
                                     "HLT_xAOD__JetContainer_a10r_tcemsubjesFS",
                                     "HLT_xAOD__JetContainer_TrigHLTJetDSSelectorCollection"};
    float containerSizeParameter[30] = {0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.4};
    int containerInputType[30] = {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 2, 2,
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1};
    int containerSignalState[30] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0};
    bool onefilled = false;
    
    for (int icont=0; icont<30; ++icont) {
        //retrieve jet collection
        const xAOD::JetContainer* jetCont = 0;
        StatusCode sc = evtStore()->retrieve(jetCont, containerName[icont]) ;
        
        if (sc.isFailure()) {
            ATH_MSG_WARNING("REGTEST Cannot retrieve jet container");
            continue;
        }
        
        int jetContsize = jetCont->size();
        ATH_MSG_INFO("REGTEST Got jet container " << containerName[icont] << ", size: " << jetContsize);
        if (jetContsize != 0) {
            onefilled = true;
            int i = 0;
            for(const auto & thisjet : *jetCont) {
                ++i;
                ATH_MSG_INFO( "REGTEST Looking at jet " << i);
                if (thisjet) {
                    //checks jet variables
                    ATH_MSG_DEBUG("REGTEST    Checking jet variables");
                    ATH_MSG_INFO( "REGTEST    pt: " << thisjet->pt() );
                    ATH_MSG_INFO( "REGTEST    eta: " << thisjet->eta() );
                    ATH_MSG_INFO( "REGTEST    phi: " << thisjet->phi() );
                    ATH_MSG_INFO( "REGTEST    m: " << thisjet->m() );
                    ATH_MSG_INFO( "REGTEST    e: " << thisjet->e() );
                    ATH_MSG_INFO( "REGTEST    rapidity: " << thisjet->rapidity() );
                    ATH_MSG_INFO( "REGTEST    px: " << thisjet->px() );
                    ATH_MSG_INFO( "REGTEST    py: " << thisjet->py() );
                    ATH_MSG_INFO( "REGTEST    pz: " << thisjet->pz() );
                    ATH_MSG_INFO( "REGTEST    type: " << thisjet->type() );
                    ATH_MSG_INFO( "REGTEST    algorithm (kt: 0, cam: 1, antikt: 2, ...): " << thisjet->getAlgorithmType() << "; should be 2");
                    if(thisjet->getAlgorithmType() != 2) ATH_MSG_WARNING("Jet algorithm different from container");
                    ATH_MSG_INFO( "REGTEST    size parameter: " << thisjet->getSizeParameter() << "; should be " << containerSizeParameter[icont]);
                    if(thisjet->getSizeParameter() != containerSizeParameter[icont]) ATH_MSG_WARNING("Jet size different from container");
                    ATH_MSG_INFO( "REGTEST    input (LCTopo: 0, EMTopo: 1, TopoTower: 2, ...): " << thisjet->getInputType() << "; should be " << containerInputType[icont]);
                    if(thisjet->getInputType() != containerInputType[icont]) ATH_MSG_WARNING("Jet input different from container");
                    ATH_MSG_INFO( "REGTEST    constituents signal state (uncalibrated: 0, calibrated: 1): " << thisjet->getConstituentsSignalState() << "; should be " << containerSignalState[icont]);
                    if(thisjet->getConstituentsSignalState() != containerSignalState[icont]) ATH_MSG_WARNING("Jet constituents' signal state different from container");
                    ATH_MSG_INFO( "REGTEST    number of constituents: " << thisjet->numConstituents() );
                    
                    
                    //checks the constituents
                    ATH_MSG_DEBUG("REGTEST    Checking jet constituents");
                    
                    xAOD::JetConstituentVector constitCont = thisjet->getConstituents();
                    unsigned int constitContsize = constitCont.size();
                    ATH_MSG_INFO("REGTEST    Got constituent vector, size: " << constitContsize << "; should be " << thisjet->numConstituents());
                    if(constitContsize != thisjet->numConstituents()) ATH_MSG_WARNING("Constituents container size different from number of constituents");
                    
                    if (constitContsize != 0) {
//                        int j = 0;
                        //                        for (const auto thisconstit : constitCont) {
                        //                            ++j;
                        //                            ATH_MSG_INFO( "REGTEST    Looking at constituent " << j);
                        //                            if(thisconstit){
                        //                                ATH_MSG_INFO( "REGTEST        constituent pt: " << thisconstit->pt() );
                        //                                ATH_MSG_INFO( "REGTEST        constituent eta: " << thisconstit->eta() );
                        //                                ATH_MSG_INFO( "REGTEST        constituent phi: " << thisconstit->phi() );
                        //                                ATH_MSG_INFO( "REGTEST        constituent m: " << thisconstit->m() );
                        //                                ATH_MSG_INFO( "REGTEST        constituent e: " << thisconstit->e() );
                        //                                ATH_MSG_INFO( "REGTEST        constituent type (CaloCluster: 1, Jet: 2, ...): " << thisconstit->type() );
                        //                            }
                        //                            else{
                        //                                ATH_MSG_WARNING("REGTEST Problem with constituent pointer");
                        //                                return StatusCode::SUCCESS;
                        //                            }
                        //                        }
                        //                        ATH_MSG_INFO("REGTEST        size of constituent vector == number of displayed constituents: "<< (constitContsize == j) );
                        //                        if (constitContsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this attribute");
                    }
                    //                    else ATH_MSG_WARNING("REGTEST    This jet has 0 constituents!");
                    
                    //checks int attributes
                    ATH_MSG_DEBUG("REGTEST    Checking int attributes");
                    int valueint;
                    if( thisjet->getAttribute(xAOD::JetAttribute::GhostMuonSegmentCount, valueint))    ATH_MSG_INFO("REGTEST    GhostMuonSegmentCount: " << valueint);
                    if( thisjet->getAttribute(xAOD::JetAttribute::GhostTrackCount, valueint))          ATH_MSG_INFO("REGTEST    GhostTrackCount: " << valueint);
                    if( thisjet->getAttribute(xAOD::JetAttribute::GhostTruthParticleCount, valueint))  ATH_MSG_INFO("REGTEST    GhostTruthParticleCount: " << valueint);
                    if( thisjet->getAttribute(xAOD::JetAttribute::FracSamplingMaxIndex, valueint))   ATH_MSG_INFO( "REGTEST    FracSamplingMaxIndex: " << valueint);
                    
                    
                    //checks float attributes
                    ATH_MSG_DEBUG("REGTEST    Checking float attributes");
                    float value;
                    if( thisjet->getAttribute(xAOD::JetAttribute::ActiveArea, value))        ATH_MSG_INFO( "REGTEST    ActiveArea: " << value );
                    // Four-vector type                   if( thisjet->getAttribute(xAOD::JetAttribute::ActiveArea4vec, value))        ATH_MSG_INFO( "REGTEST    ActiveArea4vec: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::AverageLArQF, value))      ATH_MSG_INFO( "REGTEST    AverageLArQF: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::BchCorrCell, value))       ATH_MSG_INFO( "REGTEST    BchCorrCell: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::BchCorrDotx, value))       ATH_MSG_INFO( "REGTEST    BchCorrDotx: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::BchCorrJet, value))        ATH_MSG_INFO( "REGTEST    BchCorrJet: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::BchCorrJetForCell, value)) ATH_MSG_INFO( "REGTEST    BchCorrJetForCell: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::CentroidR, value))         ATH_MSG_INFO( "REGTEST    CentroidR: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::HECQuality, value))        ATH_MSG_INFO( "REGTEST    HECQuality: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::IsoKR20Par, value))        ATH_MSG_INFO( "REGTEST    IsoKR20Par: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::IsoKR20Perp, value))       ATH_MSG_INFO( "REGTEST    IsoKR20Perp: " << value );
                    // ElementLink<DataVector<xAOD::Vertex> > type                    if( thisjet->getAttribute(xAOD::JetAttribute::HighestJVFVtx, value))       ATH_MSG_INFO( "REGTEST    HighestJVFVtx: " << value );
                    // ??? type                    if( thisjet->getAttribute(xAOD::JetAttribute::JetLabel, value))       ATH_MSG_INFO( "REGTEST    JetLabel: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::KtDR, value))              ATH_MSG_INFO( "REGTEST    KtDR: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::LArBadHVEnergy, value))    ATH_MSG_INFO( "REGTEST    LArBadHVEnergy: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::LArBadHVRatio, value))     ATH_MSG_INFO( "REGTEST    LArBadHVRatio: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::LArQuality, value))        ATH_MSG_INFO( "REGTEST    LArQuality: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::NegativeE, value))         ATH_MSG_INFO( "REGTEST    NegativeE: " << value );
                    // no tools available yet                    if( thisjet->getAttribute(xAOD::JetAttribute::NumTowers, value))         ATH_MSG_INFO( "REGTEST    NumTowers: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::SamplingMax, value))       ATH_MSG_INFO( "REGTEST    SamplingMax: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Timing, value))            ATH_MSG_INFO( "REGTEST    Timing: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::VoronoiArea, value))       ATH_MSG_INFO( "REGTEST    VoronoiArea: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::VoronoiAreaE, value))      ATH_MSG_INFO( "REGTEST    VoronoiAreaE: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::VoronoiAreaPx, value))     ATH_MSG_INFO( "REGTEST    VoronoiAreaPx: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::VoronoiAreaPy, value))     ATH_MSG_INFO( "REGTEST    VoronoiAreaPy: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::VoronoiAreaPz, value))     ATH_MSG_INFO( "REGTEST    VoronoiAreaPz: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Width, value))             ATH_MSG_INFO( "REGTEST    WIDTH: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::FracSamplingMax, value))   ATH_MSG_INFO( "REGTEST    FracSamplingMax: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::EMFrac, value))            ATH_MSG_INFO( "REGTEST    EMFrac: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::HECFrac, value))           ATH_MSG_INFO( "REGTEST    HECFrac: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::isBadLoose, value))        ATH_MSG_INFO( "REGTEST    isBadLoose: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::isBadMedium, value))       ATH_MSG_INFO( "REGTEST    isBadMedium: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::isBadTight, value))        ATH_MSG_INFO( "REGTEST    isBadTight: " << value );
                    // unknown attribute                   if( thisjet->getAttribute(xAOD::JetAttribute::isUgly, value))            ATH_MSG_INFO( "REGTEST    isUgly: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::N90Constituents, value))   ATH_MSG_INFO( "REGTEST    N90Constituents: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::N90Cells, value))          ATH_MSG_INFO( "REGTEST    N90Cells: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::OotFracClusters10, value)) ATH_MSG_INFO( "REGTEST    OotFracClusters10: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::OotFracClusters5, value))  ATH_MSG_INFO( "REGTEST    OotFracClusters5: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::OotFracCells5, value))     ATH_MSG_INFO( "REGTEST    OotFracCells5: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::OotFracCells10, value))    ATH_MSG_INFO( "REGTEST    OotFracCells10: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::PtTruth, value))    ATH_MSG_INFO( "REGTEST    PtTruth: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Tau1, value))              ATH_MSG_INFO( "REGTEST    Tau1: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Tau2, value))              ATH_MSG_INFO( "REGTEST    Tau2: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Tau3, value))              ATH_MSG_INFO( "REGTEST    Tau3: " << value );
                    // unknown attribute                  if( thisjet->getAttribute(xAOD::JetAttribute::Split12, value))           ATH_MSG_INFO( "REGTEST    Split12: " << value );
                    // unknown attribute                   if( thisjet->getAttribute(xAOD::JetAttribute::Split23, value))           ATH_MSG_INFO( "REGTEST    Split23: " << value );
                    // unknown attribute                   if( thisjet->getAttribute(xAOD::JetAttribute::Split34, value))           ATH_MSG_INFO( "REGTEST    Split34: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Dip12, value))             ATH_MSG_INFO( "REGTEST    Dip12: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Dip13, value))             ATH_MSG_INFO( "REGTEST    Dip13: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Dip23, value))             ATH_MSG_INFO( "REGTEST    Dip23: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::DipExcl12, value))         ATH_MSG_INFO( "REGTEST    DipExcl12: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::ThrustMin, value))         ATH_MSG_INFO( "REGTEST    ThrustMin: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::ThrustMaj, value))         ATH_MSG_INFO( "REGTEST    ThrustMaj: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::FoxWolfram0, value))       ATH_MSG_INFO( "REGTEST    FoxWolfram0: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::FoxWolfram1, value))       ATH_MSG_INFO( "REGTEST    FoxWolfram1: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::FoxWolfram2, value))       ATH_MSG_INFO( "REGTEST    FoxWolfram2: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::FoxWolfram3, value))       ATH_MSG_INFO( "REGTEST    FoxWolfram3: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::FoxWolfram4, value))       ATH_MSG_INFO( "REGTEST    FoxWolfram4: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Sphericity, value))        ATH_MSG_INFO( "REGTEST    Sphericity: " << value );
                    if( thisjet->getAttribute(xAOD::JetAttribute::Aplanarity, value))        ATH_MSG_INFO( "REGTEST    Aplanarity: " << value );
                    
                    //checks vector<int> attributes
                    ATH_MSG_DEBUG("REGTEST    Checking vector<int> attributes");
                    std::vector<int> vecvalueint;
                    if (thisjet->getAttribute(xAOD::JetAttribute::NumTrkPt1000, vecvalueint)) {
                        int vecsize = vecvalueint.size();
                        ATH_MSG_INFO("REGTEST    Got NumTrkPt1000 vector, size: " << vecsize);
                        if (vecsize != 0) {
                            int j = 0;
                            for(const auto & thisvalue : vecvalueint){
                                ++j;
                                ATH_MSG_INFO("REGTEST        NumTrkPt1000 #" << j << ": " << thisvalue);
                            }
                            ATH_MSG_INFO("REGTEST        size of attribute vector == number of displayed attributes: " << (vecsize == j) );
                            if (vecsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this attribute");
                        }
                    }
                    if (thisjet->getAttribute(xAOD::JetAttribute::NumTrkPt500, vecvalueint)) {
                        int vecsize = vecvalueint.size();
                        ATH_MSG_INFO("REGTEST    Got Got NumTrkPt500 vector, size: " << vecsize);
                        if (vecsize != 0) {
                            int j = 0;
                            for(const auto & thisvalue : vecvalueint){
                                ++j;
                                ATH_MSG_INFO("REGTEST        NumTrkPt500 #" << j << ": " << thisvalue);
                            }
                            ATH_MSG_INFO("REGTEST        size of attribute vector == number of displayed attributes: " << (vecsize == j) );
                            if (vecsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this attribute");
                        }
                    }
                    
                    //checks vector<float> attributes
                    ATH_MSG_DEBUG("REGTEST    Checking vector<float> attributes");
                    std::vector<float> vecvalue;
                    if (thisjet->getAttribute(xAOD::JetAttribute::JVF, vecvalue)) {
                        int vecsize = vecvalue.size();
                        ATH_MSG_INFO("REGTEST    Got JVF vector, size: " << vecsize);
                        if (vecsize != 0) {
                            int j = 0;
                            for(const auto & thisvalue : vecvalue){
                                ++j;
                                ATH_MSG_INFO("REGTEST        JVF #" << j << ": " << thisvalue);
                            }
                            ATH_MSG_INFO("REGTEST        size of attribute vector == number of displayed attributes: " << (vecsize == j) );
                            if (vecsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this attribute");
                        }
                    }
                    if (thisjet->getAttribute(xAOD::JetAttribute::SumPtTrkPt1000, vecvalue)) {
                        int vecsize = vecvalue.size();
                        ATH_MSG_INFO("REGTEST    Got SumPtTrkPt1000 vector, size: " << vecsize);
                        if (vecsize != 0) {
                            int j = 0;
                            for(const auto & thisvalue : vecvalue){
                                ++j;
                                ATH_MSG_INFO("REGTEST        SumPtTrkPt1000 #" << j << ": " << thisvalue);
                            }
                            ATH_MSG_INFO("REGTEST        size of attribute vector == number of displayed attributes: " << (vecsize == j) );
                            if (vecsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this attribute");
                        }
                    }
                    if (thisjet->getAttribute(xAOD::JetAttribute::SumPtTrkPt500, vecvalue)) {
                        int vecsize = vecvalue.size();
                        ATH_MSG_INFO("REGTEST    Got SumPtTrkPt500 vector, size: " << vecsize);
                        if (vecsize != 0) {
                            int j = 0;
                            for(const auto & thisvalue : vecvalue){
                                ++j;
                                ATH_MSG_INFO("REGTEST        SumPtTrkPt500 #" << j << ": " << thisvalue);
                            }
                            ATH_MSG_INFO("REGTEST        size of attribute vector == number of displayed attributes: " << (vecsize == j) );
                            if (vecsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this attribute");
                        }
                    }
                    if (thisjet->getAttribute(xAOD::JetAttribute::TrackWidthPt1000, vecvalue)) {
                        int vecsize = vecvalue.size();
                        ATH_MSG_INFO("REGTEST    Got TrackWidthPt1000 vector, size: " << vecsize);
                        if (vecsize != 0) {
                            int j = 0;
                            for(const auto & thisvalue : vecvalue){
                                ++j;
                                ATH_MSG_INFO("REGTEST        TrackWidthPt1000 #" << j << ": " << thisvalue);
                            }
                            ATH_MSG_INFO("REGTEST        size of attribute vector == number of displayed attributes: " << (vecsize == j) );
                            if (vecsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this attribute");
                        }
                    }
                    if (thisjet->getAttribute(xAOD::JetAttribute::TrackWidthPt500, vecvalue)) {
                        int vecsize = vecvalue.size();
                        ATH_MSG_INFO("REGTEST    Got TrackWidthPt500 vector, size: " << vecsize);
                        if (vecsize != 0) {
                            int j = 0;
                            for(const auto & thisvalue : vecvalue){
                                ++j;
                                ATH_MSG_INFO("REGTEST        TrackWidthPt500 #" << j << ": " << thisvalue);
                            }
                            ATH_MSG_INFO("REGTEST        size of attribute vector == number of displayed attributes: " << (vecsize == j) );
                            if (vecsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this attribute");
                        }
                    }
                    if (thisjet->getAttribute(xAOD::JetAttribute::EnergyPerSampling, vecvalue)) {
                        int vecsize = vecvalue.size();
                        ATH_MSG_INFO("REGTEST    Got EnergyPerSampling vector, size: " << vecsize);
                        if (vecsize != 0) {
                            int j = 0;
                            for(const auto & thisvalue : vecvalue){
                                ++j;
                                ATH_MSG_INFO("REGTEST        EnergyPerSampling #" << j << ": " << thisvalue);
                            }
                            ATH_MSG_INFO("REGTEST        size of attribute vector == number of displayed attributes: " << (vecsize == j) );
                            if (vecsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this attribute");
                        }
                    }
                    
                    
                    //checks associated objects
                    //                    ATH_MSG_DEBUG("REGTEST    Checking associated objects");
                    //                                std::vector<const xAOD::TrackParticle*> track;
                    //                                if( thisjet->getAssociatedObjects(xAOD::JetAttribute::GhostTrack, track) ){
                    //                                    int vecsize = track.size();
                    //                                    ATH_MSG_INFO("REGTEST    Got GhostTrack vector, size: " << vecsize);
                    //                                    if (vecsize != 0) {
                    //                                        int j = 0;
                    //                                        for(const auto & thistrack : track){
                    //                                            ++j;
                    //                                            //checks only one associated variable, just making sure getting the object worked
                    //                                            if (thistrack) ATH_MSG_INFO("REGTEST        z0 for GhostTrack #" << j << ": " << thistrack->z0());
                    //                                            else{
                    //                                                ATH_MSG_WARNING("REGTEST Problem with attribute pointer");
                    //                                                return StatusCode::SUCCESS;
                    //                                            }
                    //                                        }
                    //                                        ATH_MSG_INFO("REGTEST        size of associated object vector == number of displayed attributes: " << (vecsize == j) );
                    //                                        if (vecsize != j) ATH_MSG_WARNING("REGTEST Problem with displaying this associated object");
                    //                                    }
                    //                                }
                }
                else{
                    ATH_MSG_WARNING("REGTEST Problem with jet pointer");
                    return StatusCode::SUCCESS;
                }
            }
            
            if (jetContsize == i) ATH_MSG_INFO("REGTEST size of jet container == number of displayed jets: " << (jetContsize == i) );
            else ATH_MSG_WARNING("REGTEST Problem with displaying jets");
        }
    }
    
    if (!onefilled) ATH_MSG_DEBUG("There was no filled jet containers");
    
    ATH_MSG_INFO("REGTEST ==========END of xAOD::JetContainer DUMP===========");
    
    ATH_MSG_DEBUG("leaving dumpxAODJetContainer()");
    
    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigEFBjetContainer() {

  ATH_MSG_DEBUG("in dumpTrigEFBjetContainer()");
  ATH_MSG_INFO("REGTEST ==========START of TrigEFBjetContainer DUMP===========");

  const DataHandle< TrigEFBjetContainer > trigEFBjet;
  const DataHandle< TrigEFBjetContainer > lastTrigEFBjet;

  StatusCode sc = evtStore()->retrieve(trigEFBjet,lastTrigEFBjet);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigEFBjetContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigEFBjetContainers retrieved");


  for (int i=0; trigEFBjet != lastTrigEFBjet; ++trigEFBjet, ++i) {

    ATH_MSG_INFO("REGTEST Looking at TrigEFBjetContainer " << i);

    TrigEFBjetContainer::const_iterator EFBjetItr  = trigEFBjet->begin();
    TrigEFBjetContainer::const_iterator EFBjetItrE = trigEFBjet->end();

    for (int j=0; EFBjetItr != EFBjetItrE; ++EFBjetItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigEFBjet " << j);

      ATH_MSG_INFO("REGTEST TrigEFBjet->prmVtx() returns " << (*EFBjetItr)->prmVtx());
      ATH_MSG_INFO("REGTEST TrigEFBjet->xComb() returns " << (*EFBjetItr)->xComb());
      //      mLog <<MSG::INFO << "REGTEST TrigEFBjet->x2D() returns " << (*EFBjetItr)->x2D() << endmsg;
      ATH_MSG_INFO("REGTEST TrigEFBjet->xIP1D() returns " << (*EFBjetItr)->xIP1D());
      ATH_MSG_INFO("REGTEST TrigEFBjet->isValid() returns " << (*EFBjetItr)->isValid());
      ATH_MSG_INFO("REGTEST TrigEFBjet->roiId() returns " << (*EFBjetItr)->roiId());
      //      mLog <<MSG::INFO << "REGTEST TrigEFBjet->xD0() returns " << (*EFBjetItr)->xD0() << endmsg;
      ATH_MSG_INFO("REGTEST TrigEFBjet->xIP2D() returns " << (*EFBjetItr)->xIP2D());
      //      mLog <<MSG::INFO << "REGTEST TrigEFBjet->xZ0() returns " << (*EFBjetItr)->xZ0() << endmsg;
      ATH_MSG_INFO("REGTEST TrigEFBjet->xIP3D() returns " << (*EFBjetItr)->xIP3D());
      // new one:
      ATH_MSG_INFO("REGTEST TrigEFBjet->xCHI2() returns " << (*EFBjetItr)->xCHI2());
      //      mLog <<MSG::INFO << "REGTEST TrigEFBjet->x3D() returns " << (*EFBjetItr)->x3D() << endmsg; // replaced by :
      ATH_MSG_INFO("REGTEST TrigEFBjet->xSV() returns " << (*EFBjetItr)->xSV());
      //
      ATH_MSG_INFO("REGTEST TrigEFBjet->xMVtx() returns " << (*EFBjetItr)->xMVtx());
      ATH_MSG_INFO("REGTEST TrigEFBjet->xEVtx() returns " << (*EFBjetItr)->xEVtx());
      ATH_MSG_INFO("REGTEST TrigEFBjet-> xNVtx() returns " << (*EFBjetItr)-> xNVtx());
    }
  }
  ATH_MSG_INFO("REGTEST ==========END of TrigEFBjetContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigEFBjetContainer() succeeded");

  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigL2BjetContainer() {

  ATH_MSG_DEBUG("in dumpTrigL2BjetContainer()");
  ATH_MSG_INFO("REGTEST ==========START of TrigL2BjetContainer DUMP===========");

  const DataHandle< TrigL2BjetContainer > trigL2Bjet;
  const DataHandle< TrigL2BjetContainer > lastTrigL2Bjet;

  StatusCode sc = evtStore()->retrieve(trigL2Bjet,lastTrigL2Bjet);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigL2BjetContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigL2BjetContainers retrieved");


  for (int i=0; trigL2Bjet != lastTrigL2Bjet; ++trigL2Bjet, ++i) {

    ATH_MSG_INFO("REGTEST Looking at TrigL2BjetContainer " << i);

    TrigL2BjetContainer::const_iterator L2BjetItr  = trigL2Bjet->begin();
    TrigL2BjetContainer::const_iterator L2BjetItrE = trigL2Bjet->end();

    for (int j=0; L2BjetItr != L2BjetItrE; ++L2BjetItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigL2Bjet " << j);
      ATH_MSG_INFO("REGTEST TrigL2Bjet->prmVtx() returns " << (*L2BjetItr)->prmVtx());
      ATH_MSG_INFO("REGTEST TrigL2Bjet->xComb() returns " << (*L2BjetItr)->xComb());
      // ATH_MSG_INFO("REGTEST TrigL2Bjet->x2D() returns " << (*L2BjetItr)->x2D());
      ATH_MSG_INFO("REGTEST TrigL2Bjet->xIP1D() returns " << (*L2BjetItr)->xIP1D());
      ATH_MSG_INFO("REGTEST TrigL2Bjet->isValid() returns " << (*L2BjetItr)->isValid());
      ATH_MSG_INFO("REGTEST TrigL2Bjet->roiId() returns " << (*L2BjetItr)->roiId());
      // ATH_MSG_INFO("REGTEST TrigL2Bjet->xD0() returns " << (*L2BjetItr)->xD0());
      ATH_MSG_INFO("REGTEST TrigL2Bjet->xIP2D() returns " << (*L2BjetItr)->xIP2D());
      // ATH_MSG_INFO("REGTEST TrigL2Bjet->xZ0() returns " << (*L2BjetItr)->xZ0());
      ATH_MSG_INFO("REGTEST TrigL2Bjet->xIP3D() returns " << (*L2BjetItr)->xIP3D());
      // new one:
      ATH_MSG_INFO("REGTEST TrigL2Bjet->xCHI2() returns " << (*L2BjetItr)->xCHI2());
      // ATH_MSG_INFO("REGTEST TrigL2Bjet->x3D() returns " << (*L2BjetItr)->x3D());
      ATH_MSG_INFO("REGTEST TrigL2Bjet->xSV() returns " << (*L2BjetItr)->xSV());
      ATH_MSG_INFO("REGTEST TrigL2Bjet->xMVtx() returns " << (*L2BjetItr)->xMVtx());
      ATH_MSG_INFO("REGTEST TrigL2Bjet->xEVtx() returns " << (*L2BjetItr)->xEVtx());
      ATH_MSG_INFO("REGTEST TrigL2Bjet-> xNVtx() returns " << (*L2BjetItr)-> xNVtx());
    }
  }
  ATH_MSG_INFO("REGTEST ==========END of TrigL2BjetContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigL2BjetContainer() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpMuonFeature() {

  ATH_MSG_DEBUG("in dumpMuonFeature()");

  ATH_MSG_INFO("REGTEST ==========START of MuonFeature DUMP===========");

  const DataHandle< MuonFeature > MuFeature;
  const DataHandle< MuonFeature > lastMuFeature;

  StatusCode sc = evtStore()->retrieve(MuFeature,lastMuFeature);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No MuonFeature found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST MuonFeature retrieved");


  for (int i=0; MuFeature != lastMuFeature; ++MuFeature, ++i) {

    const MuonFeature* thisMuFeature = &(*MuFeature);

    ATH_MSG_INFO("REGTEST Looking at MuonFeature " << i);
    ATH_MSG_INFO("REGTEST MuonFeature->roiId() returns " << thisMuFeature->roiId());
    ATH_MSG_INFO("REGTEST MuonFeature->eta() returns " << thisMuFeature->eta());
    ATH_MSG_INFO("REGTEST MuonFeature->phi() returns " << thisMuFeature->phi());
    ATH_MSG_INFO("REGTEST MuonFeature->saddress() returns " << thisMuFeature->saddress());
    ATH_MSG_INFO("REGTEST MuonFeature->pt() returns " << thisMuFeature->pt());
    ATH_MSG_INFO("REGTEST MuonFeature->radius() returns " << thisMuFeature->radius());
    ATH_MSG_INFO("REGTEST MuonFeature->dir_phi() returns " << thisMuFeature->dir_phi());
    ATH_MSG_INFO("REGTEST MuonFeature->zeta() returns " << thisMuFeature->zeta());
    ATH_MSG_INFO("REGTEST MuonFeature->dir_zeta() returns " << thisMuFeature->dir_zeta());
    ATH_MSG_INFO("REGTEST MuonFeature->beta() returns " << thisMuFeature->beta());
  }

  ATH_MSG_INFO("REGTEST ==========END of MuonFeature DUMP===========");
  ATH_MSG_DEBUG("dumpMuonFeature() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpCombinedMuonFeature() {

  ATH_MSG_DEBUG("in dumpCombinedMuonFeature()");

  ATH_MSG_INFO("REGTEST ==========START of CombinedMuonFeature DUMP===========");

  const DataHandle< CombinedMuonFeature > CombMuon;
  const DataHandle< CombinedMuonFeature > lastCombMuon;

  StatusCode sc = evtStore()->retrieve(CombMuon,lastCombMuon);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No CombinedMuonFeature found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST CombinedMuonFeatures retrieved");

  for (int i=0; CombMuon != lastCombMuon; ++CombMuon, ++i) {

    const CombinedMuonFeature* thisCombMuFeature = &(*CombMuon);

    ATH_MSG_INFO("REGTEST Looking at CombinedMuonFeature " << i);
    ATH_MSG_INFO("REGTEST TrigPhoton->pt() returns " << (thisCombMuFeature)->pt());
    ATH_MSG_INFO("REGTEST TrigPhoton->sigma_pt() returns " << (thisCombMuFeature)->sigma_pt());
    ATH_MSG_INFO("REGTEST MuonFeature info: ");
    //      if ((thisCombMuFeature)->muFastTrack()) {
    if ((thisCombMuFeature)->muFastTrackLink().isValid() ) {
      ATH_MSG_INFO("REGTEST muFastTrack->eta() returns " << (thisCombMuFeature)->muFastTrack()->eta());
      ATH_MSG_INFO("REGTEST muFastTrack->phi() returns " << (thisCombMuFeature)->muFastTrack()->phi());
      ATH_MSG_INFO("REGTEST muFastTrack->radius() returns " << (thisCombMuFeature)->muFastTrack()->radius());
      ATH_MSG_INFO("REGTEST muFastTrack->zeta() returns " << (thisCombMuFeature)->muFastTrack()->zeta());
    } else {
      ATH_MSG_INFO("CombinedMuonFeature has no muFastTrack" );
    }

    ATH_MSG_INFO("REGTEST TrigInDetTrack info: ");
    //      if ((thisCombMuFeature)->IDTrack()) {
    if ((thisCombMuFeature)->IDTrackLink().isValid() ) {
      ATH_MSG_INFO("REGTEST IDTrack->algorithmId() returns " <<(thisCombMuFeature)->IDTrack()->algorithmId());
      ATH_MSG_INFO("REGTEST IDTrack->chi2() returns " << (thisCombMuFeature)->IDTrack()->chi2());
      ATH_MSG_INFO("REGTEST IDTrack->NStrawHits() returns " <<(thisCombMuFeature)->IDTrack()->NStrawHits());
      ATH_MSG_INFO("REGTEST IDTrack->NStraw() returns " << (thisCombMuFeature)->IDTrack()->NStraw());
      ATH_MSG_INFO("REGTEST IDTrack->NStrawTime() returns " <<(thisCombMuFeature)->IDTrack()->NStrawTime());
      ATH_MSG_INFO("REGTEST IDTrack->NTRHits() returns " <<(thisCombMuFeature)->IDTrack()->NTRHits());
    } else {
      ATH_MSG_INFO("CombinedMuonFeature has no IDTrack" );
    }

  }

  ATH_MSG_INFO("REGTEST ==========END of CombinedMuonFeature DUMP===========");
  ATH_MSG_DEBUG("dumpCombinedMuonFeature() succeeded");

  return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpCombinedMuonFeatureContainer() {

  ATH_MSG_DEBUG("in dumpCombinedMuonFeatureContainer()");
  ATH_MSG_INFO("REGTEST ==========START of CombinedMuonFeatureContainer DUMP===========");

  const DataHandle< CombinedMuonFeatureContainer > CombMuon;
  const DataHandle< CombinedMuonFeatureContainer > lastCombMuon;

  StatusCode sc = evtStore()->retrieve(CombMuon,lastCombMuon);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No CombinedMuonFeatureContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST CombinedMuonFeaturesContainer retrieved");

  for (int j=0; CombMuon != lastCombMuon; ++CombMuon, ++j) {
    ATH_MSG_INFO("REGTEST Looking at CombinedMuonFeatureContainer " << j);

    const CombinedMuonFeatureContainer* container = &(*CombMuon);
    CombinedMuonFeatureContainer::const_iterator muon;

    int i = 0;
    for ( muon = container->begin() ; muon != container->end(); ++i, ++muon ) {
      const CombinedMuonFeature* thisCombMuFeature = *muon;
      ATH_MSG_INFO("REGTEST Looking at CombinedMuonFeature " << i);

      ATH_MSG_INFO("REGTEST CombinedMuonFeature->pt() returns " << (thisCombMuFeature)->pt());
      ATH_MSG_INFO("REGTEST CombinedMuonFeature->sigma_pt() returns " << (thisCombMuFeature)->sigma_pt());

      ATH_MSG_INFO("REGTEST MuonFeature info: ");
      if ( thisCombMuFeature->muFastTrackLink().isValid() ) {
        ATH_MSG_INFO("REGTEST muFastTrack->eta() returns " << (thisCombMuFeature)->muFastTrack()->eta());
        ATH_MSG_INFO("REGTEST muFastTrack->phi() returns " << (thisCombMuFeature)->muFastTrack()->phi());
        ATH_MSG_INFO("REGTEST muFastTrack->radius() returns " << (thisCombMuFeature)->muFastTrack()->radius());
        ATH_MSG_INFO("REGTEST muFastTrack->zeta() returns " << (thisCombMuFeature)->muFastTrack()->zeta());
      } else {
        ATH_MSG_WARNING("MuonFeature has no muFastTrack!" );
      }

      ATH_MSG_INFO("REGTEST TrigInDetTrack info: ");
      if ( thisCombMuFeature->IDTrackLink().isValid() ) {
        ATH_MSG_INFO("REGTEST IDTrack->algorithmId() returns " <<(thisCombMuFeature)->IDTrack()->algorithmId());
        ATH_MSG_INFO("REGTEST IDTrack->chi2() returns " << (thisCombMuFeature)->IDTrack()->chi2());
        ATH_MSG_INFO("REGTEST IDTrack->NStrawHits() returns " <<(thisCombMuFeature)->IDTrack()->NStrawHits());

        ATH_MSG_INFO("REGTEST IDTrack->NStraw() returns " << (thisCombMuFeature)->IDTrack()->NStraw());
        ATH_MSG_INFO("REGTEST IDTrack->NStrawTime() returns " <<(thisCombMuFeature)->IDTrack()->NStrawTime());
        ATH_MSG_INFO("REGTEST IDTrack->NTRHits() returns " <<(thisCombMuFeature)->IDTrack()->NTRHits());
      } else {
        ATH_MSG_WARNING("MuonFeature has no IDTrack!" );
      }

    }
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigEMCluster() {

  ATH_MSG_DEBUG("in dumpTrigEMCluster()");
  ATH_MSG_INFO("REGTEST ==========START of TrigEMCluster DUMP===========");

  const DataHandle< TrigEMCluster > EMCluster;
  const DataHandle< TrigEMCluster > lastEMCluster;

  StatusCode sc = evtStore()->retrieve(EMCluster,lastEMCluster);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigEMCluster found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigEMCluster retrieved");

  for (int i=0; EMCluster != lastEMCluster; ++EMCluster, ++i) {

    const TrigEMCluster* thisEMCluster = &(*EMCluster);

    ATH_MSG_INFO("REGTEST Looking at TrigEMCluster " << i);

    ATH_MSG_INFO("REGTEST TrigEMCluster->energy() returns " << thisEMCluster->energy());
    ATH_MSG_INFO("REGTEST TrigEMCluster->e() returns " << thisEMCluster->e());
    ATH_MSG_INFO("REGTEST TrigEMCluster->phi() returns " << thisEMCluster->phi());

    ATH_MSG_INFO("REGTEST TrigEMCluster->eta() returns " << thisEMCluster->eta());
    ATH_MSG_INFO("REGTEST TrigEMCluster->e237() returns " << thisEMCluster->e237());
    ATH_MSG_INFO("REGTEST TrigEMCluster->e277() returns " << thisEMCluster->e277());
    ATH_MSG_INFO("REGTEST TrigEMCluster->fracs1() returns " << thisEMCluster->fracs1());
    ATH_MSG_INFO("REGTEST TrigEMCluster->weta2() returns " << thisEMCluster->weta2());
    ATH_MSG_INFO("REGTEST TrigEMCluster->ehad1() returns " << thisEMCluster->ehad1());
    ATH_MSG_INFO("REGTEST TrigEMCluster->Eta1() returns " << thisEMCluster->Eta1());
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigEMCluster DUMP===========");
  ATH_MSG_DEBUG("dumpTrigEMCluster() succeeded");

  return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpxAODTrigEMCluster() {

  ATH_MSG_DEBUG("in dumpxAODTrigEMCluster()");

  ATH_MSG_INFO("REGTEST ==========START of TrigEMCluster DUMP===========");

  const DataHandle< xAOD::TrigEMCluster > EMCluster;
  const DataHandle< xAOD::TrigEMCluster > lastEMCluster;

  StatusCode sc = evtStore()->retrieve(EMCluster,lastEMCluster);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No xAOD::TrigEMCluster found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster retrieved");


  for (int i=0; EMCluster != lastEMCluster; ++EMCluster, ++i) {

    const xAOD::TrigEMCluster* thisEMCluster = &(*EMCluster);

    ATH_MSG_INFO("REGTEST Looking at xAOD::TrigEMCluster " << i);
    
    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->energy() returns " << thisEMCluster->energy());
    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->e() returns " << thisEMCluster->energy());
    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->phi() returns " << thisEMCluster->phi());

    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->eta() returns " << thisEMCluster->eta());
    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->e237() returns " << thisEMCluster->e237());
    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->e277() returns " << thisEMCluster->e277());
    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->fracs1() returns " << thisEMCluster->fracs1());
    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->weta2() returns " << thisEMCluster->weta2());
    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->ehad1() returns " << thisEMCluster->ehad1());
    ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->Eta1() returns " << thisEMCluster->eta1());
  }

  ATH_MSG_INFO("REGTEST ==========END of xAOD::TrigEMCluster DUMP===========");
  ATH_MSG_DEBUG("dumpxAODTrigEMCluster() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigTauClusterContainer() {

  ATH_MSG_DEBUG("in dumpTrigTauClusterContainer()");

  ATH_MSG_INFO("REGTEST ==========START of TrigTauClusterContainer/TrigTauClusterDetailsContainer DUMP===========");

  const DataHandle< TrigTauClusterContainer > TauCluster;
  const DataHandle< TrigTauClusterContainer > lastTauCluster;

  StatusCode sc = evtStore()->retrieve(TauCluster,lastTauCluster);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigTauClusterContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigTauClusterContainer retrieved");


  int nClusters = 0;
  for (int i=0; TauCluster != lastTauCluster; ++TauCluster, ++i) {

    ATH_MSG_INFO("REGTEST Looking at TrigTauClusterContainer " << i);

    TrigTauClusterContainer::const_iterator TauClusterItr  = TauCluster->begin();
    TrigTauClusterContainer::const_iterator TauClusterItrE = TauCluster->end();

    for (int j=0; TauClusterItr != TauClusterItrE; ++TauClusterItr, ++j ) {
      nClusters++;
      ATH_MSG_INFO("REGTEST Looking at TrigTauCluster " << j);

      ATH_MSG_INFO("REGTEST TrigTauCluster->energy() returns " << (*TauClusterItr)->energy());
      ATH_MSG_INFO("REGTEST TrigTauCluster->et() returns " << (*TauClusterItr)->et());
      ATH_MSG_INFO("REGTEST TrigTauCluster->EMCalibEnergy() returns " << (*TauClusterItr)->EMCalibEnergy());

      ATH_MSG_INFO("REGTEST TrigTauCluster->EMenergy() returns " << (*TauClusterItr)->EMenergy());
      ATH_MSG_INFO("REGTEST TrigTauCluster->HADenergy() returns " << (*TauClusterItr)->HADenergy());
      ATH_MSG_INFO("REGTEST TrigTauCluster->eta() returns " << (*TauClusterItr)->eta());
      ATH_MSG_INFO("REGTEST TrigTauCluster->phi() returns " << (*TauClusterItr)->phi());
      ATH_MSG_INFO("REGTEST TrigTauCluster->IsoFrac() returns " << (*TauClusterItr)->IsoFrac());
      ATH_MSG_INFO("REGTEST TrigTauCluster->stripWidth() returns " << (*TauClusterItr)->stripWidth());
      ATH_MSG_INFO("REGTEST TrigTauCluster->numStripCells() returns " << (*TauClusterItr)->numStripCells());
      ATH_MSG_INFO("REGTEST TrigTauCluster->CaloRadius() returns " << (*TauClusterItr)->CaloRadius());
      ATH_MSG_INFO("REGTEST TrigTauCluster->numTotCells() returns " << (*TauClusterItr)->numTotCells());


      if( (*TauClusterItr)->clusterDetails() == 0 )
        ATH_MSG_WARNING("REGTEST TrigTauCluster-> Details link is MISSING ");
      else{
        ATH_MSG_INFO("REGTEST TrigTauCluster->Energy in Narrow cone : EM[0/1/2/3]="
                     <<(*TauClusterItr)->EMenergyNarrow(0) << " " << (*TauClusterItr)->EMenergyNarrow(1) << " "
                     <<(*TauClusterItr)->EMenergyNarrow(2) << " ");
        ATH_MSG_INFO("REGTEST TrigTauCluster->Energy in Narrow cone : HAD[0/1/2]="
                     <<(*TauClusterItr)->HADenergyNarrow(0) << " " << (*TauClusterItr)->HADenergyNarrow(1) << " "
                     <<(*TauClusterItr)->HADenergyNarrow(2));
      }
    }
  }

  int nDetails = 0;
  const DataHandle< TrigTauClusterDetailsContainer > TauDetailsCluster;
  const DataHandle< TrigTauClusterDetailsContainer > lastTauDetailsCluster;
  sc = evtStore()->retrieve(TauDetailsCluster,lastTauDetailsCluster);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigTauDetailsClusterContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigTauDetailsClusterContainer retrieved");

  for (int i=0; TauDetailsCluster != lastTauDetailsCluster; ++TauDetailsCluster, ++i) {

    ATH_MSG_INFO("REGTEST Looking at TrigTauClusterDetailsContainer " << i);

    TrigTauClusterDetailsContainer::const_iterator TauDetailsClusterItr  = TauDetailsCluster->begin();
    TrigTauClusterDetailsContainer::const_iterator TauDetailsClusterItrE = TauDetailsCluster->end();

    for (int j=0; TauDetailsClusterItr != TauDetailsClusterItrE; ++TauDetailsClusterItr, ++j ) {
      nDetails++;
      ATH_MSG_INFO("REGTEST Looking at TrigTauClusterDetails " << j);

      for(unsigned int i = 0; i<4; ++i ) {
        ATH_MSG_INFO("REGTEST TrigTauClusterDetails-> EM["<<i <<"] Radius/EnergyNar/EnergyMid/EnergyWid returns "
                     << (*TauDetailsClusterItr)->EMRadius(i) << " "
                     << (*TauDetailsClusterItr)->EMenergyNarrow(i) << " "
                     << (*TauDetailsClusterItr)->EMenergyMedium(i) << " "
                     << (*TauDetailsClusterItr)->EMenergyWide(i));
      }
      for(unsigned int i = 0; i<3; ++i ) {
        ATH_MSG_INFO("REGTEST TrigTauClusterDetails-> HAD["<<i <<"] Radius/EnergyNar/EnergyMid/EnergyWid returns "
                     << (*TauDetailsClusterItr)->HADRadius(i) << " "
                     << (*TauDetailsClusterItr)->HADenergyNarrow(i) << " "
                     << (*TauDetailsClusterItr)->HADenergyMedium(i) << " "
                     << (*TauDetailsClusterItr)->HADenergyWide(i));
      }      
    }
  }

  if( nDetails != nClusters)
    ATH_MSG_WARNING("REGTEST inconsistent number of TrigTauClusters ("<< nClusters<< ") and TrigTauClusterDetails ("
                    << nDetails << ")");

  ATH_MSG_INFO("REGTEST ==========END of TrigTauClusterContainer/TrigTauClusterDetailsContainer DUMP===========");

  ATH_MSG_DEBUG("dumpTrigTauClusterContainer() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigEMClusterContainer() {

  ATH_MSG_DEBUG("in dumpTrigEMClusterContainer()");

  ATH_MSG_INFO("REGTEST ==========START of TrigEMClusterContainer DUMP===========");

  const DataHandle< TrigEMClusterContainer > EMCluster;
  const DataHandle< TrigEMClusterContainer > lastEMCluster;

  StatusCode sc = evtStore()->retrieve(EMCluster,lastEMCluster);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TrigEMClusterContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TrigEMClusterContainer retrieved");


  for (int i=0; EMCluster != lastEMCluster; ++EMCluster, ++i) {

    ATH_MSG_INFO("REGTEST Looking at TrigEMClusterContainer " << i);

    TrigEMClusterContainer::const_iterator EMClusterItr  = EMCluster->begin();
    TrigEMClusterContainer::const_iterator EMClusterItrE = EMCluster->end();

    for (int j=0; EMClusterItr != EMClusterItrE; ++EMClusterItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at TrigEMCluster " << j);

      ATH_MSG_INFO("REGTEST TrigEMCluster->energy() returns " << (*EMClusterItr)->energy());
      ATH_MSG_INFO("REGTEST TrigEMCluster->et() returns " << (*EMClusterItr)->et());
      ATH_MSG_INFO("REGTEST TrigEMCluster->eta() returns " << (*EMClusterItr)->eta());
      ATH_MSG_INFO("REGTEST TrigEMCluster->phi() returns " << (*EMClusterItr)->phi());
      ATH_MSG_INFO("REGTEST TrigEMCluster->print() gives");
      int level = msg().level();
      // little trick to print out stuff
      msg().setLevel(MSG::DEBUG);
      (*EMClusterItr)->print(msg());
      msg().setLevel(level);
    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigEMClusterContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigEMClusterContainer() succeeded");

  return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpxAODTrigEMClusterContainer() {

  ATH_MSG_DEBUG("in dumpxAODTrigEMClusterContainer()");

  ATH_MSG_INFO("REGTEST ==========START of xAODTrigEMClusterContainer DUMP===========");

  const DataHandle< xAOD::TrigEMClusterContainer > EMCluster;
  const DataHandle< xAOD::TrigEMClusterContainer > lastEMCluster;

  StatusCode sc = evtStore()->retrieve(EMCluster,lastEMCluster);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No xAOD::TrigEMClusterContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST xAOD::TrigEMClusterContainer retrieved");


  for (int i=0; EMCluster != lastEMCluster; ++EMCluster, ++i) {

    ATH_MSG_INFO("REGTEST Looking at xAOD::TrigEMClusterContainer " << i);

    xAOD::TrigEMClusterContainer::const_iterator EMClusterItr  = EMCluster->begin();
    xAOD::TrigEMClusterContainer::const_iterator EMClusterItrE = EMCluster->end();

    for (int j=0; EMClusterItr != EMClusterItrE; ++EMClusterItr, ++j ) {

      ATH_MSG_INFO("REGTEST Looking at xAOD::TrigEMCluster " << j);
      ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->energy() returns " << (*EMClusterItr)->energy());
      ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->et() returns " << (*EMClusterItr)->et());
      ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->eta() returns " << (*EMClusterItr)->eta());
      ATH_MSG_INFO("REGTEST xAOD::TrigEMCluster->phi() returns " << (*EMClusterItr)->phi());
      //msg() <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->print() gives" << endmsg;
      //int level = msg().level();
      // little trick to print out stuff
      //msg().setLevel(MSG::DEBUG);
      //(*EMClusterItr)->print(msg());
      //msg().setLevel(level);
    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TrigEMClusterContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTrigEMClusterContainer() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTileMuFeatureContainer() {

  ATH_MSG_DEBUG("in dumpTileMuFeatureContainer()");
  ATH_MSG_INFO("REGTEST ==========START of TileMuFeatureContainer DUMP===========");

  const DataHandle< TileMuFeatureContainer > TileMu;
  const DataHandle< TileMuFeatureContainer > lastTileMu;

  StatusCode sc = evtStore()->retrieve(TileMu, lastTileMu);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TileMuFeatureContainer found");
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TileMuFeatureContainer retrieved");

  for (int i=0; TileMu != lastTileMu; ++TileMu, ++i) {
    ATH_MSG_INFO("REGTEST Looking at TileMuFeatureContainer " << i);
    TileMuFeatureContainer::const_iterator TileMuItr  = TileMu->begin();
    TileMuFeatureContainer::const_iterator TileMuItrE = TileMu->end();

    for(int j=0; TileMuItr != TileMuItrE; ++TileMuItr, ++j) {
      ATH_MSG_INFO("REGTEST Looking at TileMuFeature " << j);
      ATH_MSG_INFO("REGTEST TileMuFeature->eta()          returns " << (*TileMuItr)->eta());
      ATH_MSG_INFO("REGTEST TileMuFeature->phi()          returns " << (*TileMuItr)->phi());
      ATH_MSG_INFO("REGTEST TileMuFeature->enedep().at(0) returns " << (*TileMuItr)->enedep().at(0));
      ATH_MSG_INFO("REGTEST TileMuFeature->enedep().at(1) returns " << (*TileMuItr)->enedep().at(1));
      ATH_MSG_INFO("REGTEST TileMuFeature->enedep().at(2) returns " << (*TileMuItr)->enedep().at(2));
      ATH_MSG_INFO("REGTEST TileMuFeature->enedep().at(3) returns " << (*TileMuItr)->enedep().at(3));
      ATH_MSG_INFO("REGTEST TileMuFeature->quality()      returns " << (*TileMuItr)->quality());

    }
  }

  ATH_MSG_INFO("REGTEST ==========END of TileMuFeatureContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTileMuFeatureContainer() succeeded");

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTileTrackMuFeatureContainer() {  

  ATH_MSG_DEBUG("in dumpTileTrackMuFeatureContainer()");
  ATH_MSG_INFO("REGTEST ==========START of TileTrackMuFeatureContainer DUMP===========");
  
  const DataHandle< TileTrackMuFeatureContainer > TileTrackMu;
  const DataHandle< TileTrackMuFeatureContainer > lastTileTrackMu;
  
  StatusCode sc = evtStore()->retrieve(TileTrackMu, lastTileTrackMu);
  if (sc.isFailure()) {
	ATH_MSG_INFO("REGTEST No TileTrackMuFeatureContainer found");
	return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("REGTEST TileTrackMuFeatureContainer retrieved");
  
  for (int i=0; TileTrackMu!=lastTileTrackMu; ++TileTrackMu, ++i) {
	ATH_MSG_INFO("REGTEST Looking at TileTrackMuFeatureContainer " << i);

	TileTrackMuFeatureContainer::const_iterator TileTrackMuItr = TileTrackMu->begin();
	TileTrackMuFeatureContainer::const_iterator TileTrackMuItrE= TileTrackMu->end();

	for (int j=0; TileTrackMuItr != TileTrackMuItrE; ++TileTrackMuItr, ++j) {
      ATH_MSG_INFO("REGTEST Looking at TileTrackMuFeature " << j);
      ATH_MSG_INFO("REGTEST TileTrackMuFeature->PtTR_Trk()  returns " << (*TileTrackMuItr)->PtTR_Trk());
      ATH_MSG_INFO("REGTEST TileTrackMuFeature->EtaTR_Trk() returns " << (*TileTrackMuItr)->EtaTR_Trk());
      ATH_MSG_INFO("REGTEST TileTrackMuFeature->PhiTR_Trk() returns " << (*TileTrackMuItr)->PhiTR_Trk());
      ATH_MSG_INFO("REGTEST TileTrackMuFeature->Typ_IDTrk() returns " << (*TileTrackMuItr)->Typ_IDTrk());

      ATH_MSG_INFO("REGTEST TileMuFeature info: ");
      ElementLink<TileMuFeatureContainer> TileMuEL = (*TileTrackMuItr)->TileMuLink();
      const TileMuFeature* TileMu;
      if ( !TileMuEL.isValid() ) {
        ATH_MSG_INFO("REGTEST No TileMuFeature (Something Wrong)");
        TileMu = 0;
      } else{
        TileMu = *TileMuEL;
      }

      if( TileMu != 0 ){
        ATH_MSG_INFO("REGTEST TileMuLink->eta()          returns " << TileMu->eta());
        ATH_MSG_INFO("REGTEST TileMuLink->phi()          returns " << TileMu->phi());
        ATH_MSG_INFO("REGTEST TileMuLink->enedep().at(0) returns " << TileMu->enedep().at(0));
        ATH_MSG_INFO("REGTEST TileMuLink->enedep().at(1) returns " << TileMu->enedep().at(1));
        ATH_MSG_INFO("REGTEST TileMuLink->enedep().at(2) returns " << TileMu->enedep().at(2));
        ATH_MSG_INFO("REGTEST TileMuLink->enedep().at(3) returns " << TileMu->enedep().at(3));
        ATH_MSG_INFO("REGTEST TileMuLink->quality()      returns " << TileMu->quality());
      }

      ATH_MSG_INFO("REGTEST TrigInDetTrack info: ");
      ElementLink<TrigInDetTrackCollection> IDScanEL = (*TileTrackMuItr)->IDScanLink();
      const TrigInDetTrack* Track;
      if ( !IDScanEL.isValid() ) {
        ATH_MSG_INFO("REGTEST No valid IDtracks");
        Track = 0;
      } else{
        Track = *IDScanEL;
      }

      if (Track != 0) {
        ATH_MSG_INFO("REGTEST IDScanLink->algorithmId()     returns " << Track->algorithmId());
        ATH_MSG_INFO("REGTEST IDScanLink->chi2()            returns " << Track->chi2());
        ATH_MSG_INFO("REGTEST IDScanLink->NStrawHits()      returns ");
        ATH_MSG_INFO("REGTEST IDScanLink->NStraw()          returns " << Track->NStraw());
        ATH_MSG_INFO("REGTEST IDScanLink->NStrawTime()      returns " << Track->NStrawTime());
        ATH_MSG_INFO("REGTEST IDScanLink->NTRHits()         returns " << Track->NTRHits());
        ATH_MSG_INFO("REGTEST IDScanLink->param()->phi0()   returns " << Track->param()->phi0());
        ATH_MSG_INFO("REGTEST IDScanLink->param()->eta()    returns " << Track->param()->eta());
        ATH_MSG_INFO("REGTEST IDScanLink->param()->pT()     returns " << Track->param()->pT());
      }
	}
  }

  ATH_MSG_INFO("REGTEST ==========END of TileTrackMuFeatureContainer DUMP===========");
  ATH_MSG_DEBUG("dumpTileTrackMuFeatureContainer() succeeded");
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpxAODTauJetContainer() {

  ATH_MSG_DEBUG("In dumpxAODTauJetContainer");
  ATH_MSG_INFO( "REGTEST ==========START of xAOD::TauJetContainer DUMP===========" );
  const xAOD::TauJetContainer * TauJetcont = 0;
  StatusCode sc = evtStore() -> retrieve (TauJetcont, "HLT_xAOD__TauJetContainer_TrigTauRecMerged");

  if (sc.isFailure()) {

    ATH_MSG_INFO("REGTEST No Tau container HLT_xAOD__TauJetContainer_TrigTauRecMerged");


    return StatusCode::SUCCESS;
  }


  for(xAOD::TauJetContainer::const_iterator tauIt = TauJetcont->begin(); tauIt != TauJetcont->end();++tauIt){

    ATH_MSG_INFO( "REGTEST (*tauIt)->eta() returns  " << (*tauIt)->eta() );
    ATH_MSG_INFO( "REGTEST (*tauIt)->phi() returns  " << (*tauIt)->phi() );
    ATH_MSG_INFO( "REGTEST (*tauIt)->pt() returns   " << (*tauIt)->pt() );

    // for numTracks()
    int EFnTracks = -1;
    #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
    EFnTracks = (*tauIt)->nTracks();
    #else
    (*tauIt)->detail(xAOD::TauJetParameters::nChargedTracks, EFnTracks);
    #endif

    ATH_MSG_INFO( "REGTEST (*tauIt)->nTracks() returns " << EFnTracks );

    // for nTracksIsolation()
    int EFWidenTrack = -1;
    #ifndef XAODTAU_VERSIONS_TAUJET_V3_H
    EFWidenTrack = (*tauIt)->nWideTracks();
    #else
    (*tauIt)->detail(xAOD::TauJetParameters::nIsolatedTracks, EFWidenTrack);
    #endif

    ATH_MSG_INFO( "REGTEST (*tauIt)->nWideTracks() returns " << EFWidenTrack );

    //bool test = false;
    float trkAvgDist=0;
    float etOvPtLead=0;
    float emRadius=0;
    float hadRadius=0;
    float IsoFrac=0;
    float centFrac=0;
    float ipSigLeadTrk=0;
    float trFlightPathSig=0;
    float dRmax=0;
    float massTrkSys=0;
    float PSSFraction=0;
    float EMPOverTrkSysP=0;
    float ChPiEMEOverCaloEME=0;
    float EtEm=0;
    float EtHad=0;

    if ( (*tauIt)->detail(xAOD::TauJetParameters::trkAvgDist,trkAvgDist))
      ATH_MSG_INFO( "REGTEST TauDetails->trkAvgDist() returns " << trkAvgDist);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::etOverPtLeadTrk,etOvPtLead))
      ATH_MSG_INFO( "REGTEST TauDetails->etOverPtLeadTrk() returns " << etOvPtLead);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::EMRadius,emRadius))
      ATH_MSG_INFO( "REGTEST TauDetails->EMRadius() returns " << emRadius);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::hadRadius,hadRadius))
      ATH_MSG_INFO( "REGTEST TauDetails->hadRadius() returns " << hadRadius);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::isolFrac,IsoFrac))
      ATH_MSG_INFO( "REGTEST TauDetails->isolFrac() returns " << IsoFrac);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::centFrac,centFrac))
      ATH_MSG_INFO( "REGTEST TauDetails->centFrac() returns " << centFrac);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::ipSigLeadTrk,ipSigLeadTrk))
      ATH_MSG_INFO( "REGTEST TauDetails->ipSigLeadTrk() returns " << ipSigLeadTrk);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::trFlightPathSig,trFlightPathSig))
      ATH_MSG_INFO( "REGTEST TauDetails->trFlightPathSig() returns " << trFlightPathSig);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::dRmax,dRmax))
      ATH_MSG_INFO( "REGTEST TauDetails->dRmax() returns " << dRmax);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::massTrkSys,massTrkSys)){
      massTrkSys /=1000;
      ATH_MSG_INFO( "REGTEST TauDetails->massTrkSys() returns " << massTrkSys);}

    if ( (*tauIt)->detail(xAOD::TauJetParameters::PSSFraction,PSSFraction))
      ATH_MSG_INFO( "REGTEST TauDetails->PSSFraction() returns " << PSSFraction);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::EMPOverTrkSysP,EMPOverTrkSysP))
      ATH_MSG_INFO( "REGTEST TauDetails->EMPOverTrkSysP() returns " << EMPOverTrkSysP);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME,ChPiEMEOverCaloEME))
      ATH_MSG_INFO( "REGTEST TauDetails->ChPiEMEOverCaloEME() returns " << ChPiEMEOverCaloEME);

    if ( (*tauIt)->detail(xAOD::TauJetParameters::etEMAtEMScale,EtEm)){
      EtEm /=1000;
      ATH_MSG_INFO( "REGTEST TauDetails->etEMAtEMScale() returns " << EtEm);}

    if ( (*tauIt)->detail(xAOD::TauJetParameters::etHadAtEMScale,EtHad)){
       EtHad /=1000;
       ATH_MSG_INFO( "REGTEST TauDetails->etHadAtEMScale() returns " << EtHad);}


    if( !(*tauIt)->jetLink().isValid() ) {
      ATH_MSG_WARNING("tau does not have jet seed");
      return StatusCode::SUCCESS;
    }

    const xAOD::Jet* pJetSeed = *((*tauIt)->jetLink());

    xAOD::JetConstituentVector::const_iterator clusItr  = pJetSeed->getConstituents().begin();
    xAOD::JetConstituentVector::const_iterator clusItrE = pJetSeed->getConstituents().end();

    for (int clusCount = 0; clusItr != clusItrE; ++clusItr, ++clusCount) {

      ATH_MSG_INFO( "REGTEST Tau Cluster " << clusCount << " pt = " << (*clusItr)->pt()
		    << " eta = " << (*clusItr)->eta()
		    << " phi = " << (*clusItr)->phi() );

    }


    for (unsigned int trackNum = 0;  trackNum < (*tauIt)->nTracks(); ++trackNum) {

      const xAOD::TrackParticle *linkTrack = (*tauIt)->track(trackNum)->track();
      if (!linkTrack) {
     	ATH_MSG_WARNING("can't get tau linked track");
     	return StatusCode::SUCCESS;
      } else {
     	ATH_MSG_DEBUG("Got the tau linked track");
      }

      ATH_MSG_INFO( "REGTEST Tau linked track " << trackNum << " pt = " << linkTrack->pt()
      		    << " eta = " << linkTrack->eta()
      		    << " phi = " << linkTrack->phi() );

    }


  }// end for

  return StatusCode::SUCCESS;

}


/////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTauJetContainer() {

  ATH_MSG_INFO("REGTEST ==========START of TauJetContainer DUMP===========");

  using namespace Analysis;

  StatusCode sCode=StatusCode::FAILURE;
  int ntag=1;
  std::string TauContainerTags[]={"HLT_TrigTauRecMerged"};
  for (int itag=0; itag < ntag; itag++) {
    const TauJetContainer* TauJetcont;
    sCode=evtStore()->retrieve(TauJetcont , TauContainerTags[itag]);
    if( sCode.isFailure() ){
      ATH_MSG_INFO("Failed to retrieve TauJetContainer  with key " << TauContainerTags[itag]);
      continue;
    }

    for(Analysis::TauJetContainer::const_iterator tauIt = TauJetcont->begin(); tauIt != TauJetcont->end(); ++tauIt){

      bool IsTaurec = false;
      bool Is1p3p = false;
      if ((*tauIt)->hasAuthor( TauJetParameters::tauRec)) {
        ATH_MSG_INFO("Is TauRec Seeded ");
        IsTaurec = true;
      }

      if ((*tauIt)->hasAuthor( TauJetParameters::tau1P3P)) {
        ATH_MSG_INFO("Is Tau1p3p Seeded ");
        Is1p3p = true;
      }

      if ((*tauIt)->hasAuthor( TauJetParameters::unknown)) {
        ATH_MSG_INFO("Is unknown seeded ");
      }

      const Analysis::TauCommonDetails*  TauDetails = (*tauIt)->details<const Analysis::TauCommonDetails>();
      if (TauDetails == NULL) {
        ATH_MSG_INFO(" TauDetails == NULL ");
        continue;
      }

      ATH_MSG_INFO("REGTEST (*tauIt)->eta()                    returns " << (*tauIt)->eta());
      ATH_MSG_INFO("REGTEST (*tauIt)->phi()                    returns " << (*tauIt)->phi());
      ATH_MSG_INFO("REGTEST (*tauIt)->numTrack()               returns " << (*tauIt)->numTrack());
      ATH_MSG_INFO("REGTEST TauDetails->nLooseTrk()            returns " << TauDetails->nLooseTrk());
      ATH_MSG_INFO("REGTEST TauDetails->leadTrkPt()            returns " << TauDetails->leadTrkPt());
      ATH_MSG_INFO("REGTEST TauDetails->leadLooseTrkPt()       returns " << TauDetails->leadLooseTrkPt());
      ATH_MSG_INFO("REGTEST TauDetails->ipZ0SinThetaSigLeadTrk() returns " << TauDetails->ipZ0SinThetaSigLeadTrk());
      ATH_MSG_INFO("REGTEST TauDetails->ipSigLeadTrk() returns "<<TauDetails->ipSigLeadTrk());
      ATH_MSG_INFO("REGTEST TauDetails->ipSigLeadLooseTrk() returns "<<TauDetails->ipSigLeadLooseTrk());
      ATH_MSG_INFO("REGTEST TauDetails->trkWidth2() returns "<<TauDetails->trkWidth2());
      ATH_MSG_INFO("REGTEST TauDetails->trFlightPathSig() returns "<< TauDetails->trFlightPathSig());

      if(IsTaurec) {
        ATH_MSG_INFO("Calo seeded");
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_etHadCalib()                returns " << TauDetails->seedCalo_etHadCalib());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_etEMCalib()                 returns " << TauDetails->seedCalo_etEMCalib());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_EMRadius()                  returns " << TauDetails->seedCalo_EMRadius());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_isolFrac()                  returns " << TauDetails->seedCalo_isolFrac());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_centFrac()                  returns " << TauDetails->seedCalo_centFrac());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_stripWidth2()               returns " << TauDetails->seedCalo_stripWidth2());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_nStrip()                    returns " << TauDetails->seedCalo_nStrip());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_etEMAtEMScale()             returns " << TauDetails->seedCalo_etEMAtEMScale());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_etHADAtEMScale()            returns " << TauDetails->seedCalo_etHadAtEMScale());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_hadRadius()                 returns " << TauDetails->seedCalo_hadRadius());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_nIsolLooseTrk()             returns " << TauDetails->seedCalo_nIsolLooseTrk());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_trkAvgDist()                returns " << TauDetails->seedCalo_trkAvgDist());
        ATH_MSG_INFO("REGTEST TauDetails->seedCalo_trkRmsDist()                returns " << TauDetails->seedCalo_trkRmsDist());
      }

      if(Is1p3p) {
        ATH_MSG_INFO("Track seeded");
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_EMRadius()                   returns " << TauDetails->seedTrk_EMRadius());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_isolFrac()                   returns " << TauDetails->seedTrk_isolFrac());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etChrgHadOverSumTrkPt ()     returns " << TauDetails->seedTrk_etChrgHadOverSumTrkPt ());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_isolFracWide()               returns " << TauDetails->seedTrk_isolFracWide());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etHadAtEMScale()             returns " << TauDetails->seedTrk_etHadAtEMScale());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etEMAtEMScale()              returns " << TauDetails->seedTrk_etEMAtEMScale());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etEMCL()                     returns " << TauDetails->seedTrk_etEMCL());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etChrgEM()                   returns " << TauDetails->seedTrk_etChrgEM());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etNeuEM()                    returns " << TauDetails->seedTrk_etNeuEM());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etResNeuEM()                 returns " << TauDetails->seedTrk_etResNeuEM());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_hadLeakEt()                  returns " << TauDetails->seedTrk_hadLeakEt());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_sumEMCellEtOverLeadTrkPt()   returns " << TauDetails->seedTrk_sumEMCellEtOverLeadTrkPt());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_secMaxStripEt()              returns " << TauDetails->seedTrk_secMaxStripEt());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_stripWidth2()                returns " << TauDetails->seedTrk_stripWidth2());       
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_nStrip()                     returns " << TauDetails->seedTrk_nStrip());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etChrgHad()                  returns " << TauDetails->seedTrk_etChrgHad());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_nOtherCoreTrk()              returns " << TauDetails->seedTrk_nOtherCoreTrk());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_nIsolTrk()                   returns " << TauDetails->seedTrk_nIsolTrk());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etIsolEM()                   returns " << TauDetails->seedTrk_etIsolEM());
        ATH_MSG_INFO("REGTEST TauDetails->seedTrk_etIsolHad()                  returns " << TauDetails->seedTrk_etIsolHad());
      }

      const Analysis::TauJet* tj = *tauIt;
      if ( tj->clusterLink().isValid() ) {
        ATH_MSG_INFO("REGTEST CaloCluster present");
      } else {
        ATH_MSG_INFO("REGTEST CaloCluster missing");
      }

      if( TauDetails->looseTrk().size() !=0 ) {
        ATH_MSG_INFO("REGTEST TauDetails->looseTrk() link present ");
      } else {
        ATH_MSG_INFO("REGTEST TauDetails->looseTrk() link missing ");
      }

      if ( tj->cellClusterLink().isValid() ) {
        ATH_MSG_INFO("REGTEST CellCaloCluster present");
      } else {
        ATH_MSG_INFO("REGTEST CellCaloCluster missing");
      }

      if ( tj->jetLink().isValid() ) {
        ATH_MSG_INFO("REGTEST Jet present");
      } else {
        ATH_MSG_INFO("REGTEST Jet missing");
      }

      if ( tj->jetLink().isValid() ) {
        ATH_MSG_INFO("REGTEST Jet present");
      } else {
        ATH_MSG_INFO("REGTEST Jet missing");
      }

      ATH_MSG_INFO("REGTEST numTrack                returns " << tj->numTrack());

      if ( tj->trackLinkVector().size() != 0 ) {
        ATH_MSG_INFO("REGTEST TrackLinkVector present");
      } else {
        ATH_MSG_INFO("REGTEST TrackkLinkVector missing");
      }

      ATH_MSG_INFO("REGTEST author                  returns " << tj->author());
      ATH_MSG_INFO("REGTEST ROIWord                 returns " << tj->author());
    }
  }

  const DataHandle< TauJetContainer > TauJet;
  const DataHandle< TauJetContainer > lastTauJet;
  StatusCode sc = evtStore()->retrieve(TauJet, lastTauJet);
  if (sc.isFailure()) {
    ATH_MSG_INFO("REGTEST No TauJetContainer found");
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG("Found TauJetContainer");

  ATH_MSG_INFO("REGTEST TauJetContainer retrieved");

  for ( ; TauJet != lastTauJet ; ++TauJet ) {
    ATH_MSG_INFO("REGTEST TauJetContainer key: " << TauJet.key());
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpxAODTrackParticle() {

  ATH_MSG_DEBUG("In dumpxAODTrackParticle()");

  ATH_MSG_INFO("REGTEST ==========START of xAOD::TrackParticle DUMP===========");

	std::vector<std::string> SGkeys;
	SGkeys.push_back("HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_EFID");
	SGkeys.push_back("HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_EFID");
	SGkeys.push_back("HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_EFID");
	SGkeys.push_back("HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_FullScan_EFID");
	SGkeys.push_back("HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFID");
	SGkeys.push_back("HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Photon_EFID");
	SGkeys.push_back("HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_EFID");

	StatusCode returnsc = StatusCode::SUCCESS;

	for (unsigned int SGkey = 0; SGkey < SGkeys.size(); ++SGkey) {
		const xAOD::TrackParticleContainer* trackParticleContainer=0;
		StatusCode sc = evtStore()->retrieve(trackParticleContainer,SGkeys.at(SGkey));
		if (sc.isFailure()) {
          ATH_MSG_INFO("REGTEST No track particle container found with key " << SGkeys.at(SGkey));
			continue;
		}
		ATH_MSG_INFO("REGTEST TrackParticleContainer retrieved with key " << SGkeys.at(SGkey)
                     << " and size " << trackParticleContainer->size());
        
		xAOD::TrackParticleContainer::const_iterator trackParticleItr = trackParticleContainer->begin();
		xAOD::TrackParticleContainer::const_iterator trackParticleLast = trackParticleContainer->end();

		for (int index = 0; trackParticleItr != trackParticleLast; ++trackParticleItr, ++index) {
          ATH_MSG_INFO("REGTEST Looking at Track Particle " << index);

          ATH_MSG_INFO("REGTEST IParticle functions:");
		  ATH_MSG_INFO("REGTEST pt: " << (*trackParticleItr)->pt()
                       << "/eta: " << (*trackParticleItr)->eta()
                       << "/phi: " << (*trackParticleItr)->phi()
                       << "/m: " << (*trackParticleItr)->m()
                       << "/e: " << (*trackParticleItr)->e()
                       << "/rapidity: " << (*trackParticleItr)->rapidity());

          ATH_MSG_INFO("REGTEST Defining parameters functions:");
		  ATH_MSG_INFO("REGTEST charge: " << (*trackParticleItr)->charge()
                       << "/d0: " << (*trackParticleItr)->d0()
                       << "/z0: " << (*trackParticleItr)->z0()
                       << "/phi0: " << (*trackParticleItr)->phi0()
                       << "/theta: " << (*trackParticleItr)->theta()
                       << "/qOverP: " << (*trackParticleItr)->qOverP()
                       << "/vx: " << (*trackParticleItr)->vx()
                       << "/vy: " << (*trackParticleItr)->vy()
                       << "/vz: " << (*trackParticleItr)->vz());

			// Curvilinear functions skipped

          ATH_MSG_INFO("REGTEST Fit quality functions:");
		  ATH_MSG_INFO("REGTEST chiSquared: " << (*trackParticleItr)->chiSquared()
                       << "/numberDoF: " << (*trackParticleItr)->numberDoF());

			// TrackInfo functions skipped

          ATH_MSG_INFO("REGTEST summaryValue variables:");
          msg() << MSG::INFO << "REGTEST ";
			uint8_t numberOfBLayerHits = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfBLayerHits, xAOD::numberOfBLayerHits) ) {
				msg() << "/numberOfBLayerHits: " << static_cast<int>(numberOfBLayerHits);
			} else {
				msg() << "/numberOfBLayerHits not found";
			}

			uint8_t numberOfPixelHits = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits) ) {
				msg() << "/numberOfPixelHits: " << static_cast<int>(numberOfPixelHits);
			} else {
				msg() << "/numberOfPixelHits not found";
			}

			uint8_t numberOfPixelHoles = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfPixelHoles, xAOD::numberOfPixelHoles) ) {
				msg() << "/numberOfPixelHoles: " << static_cast<int>(numberOfPixelHoles);
			} else {
				msg() << "/numberOfPixelHoles not found";
			}

			uint8_t numberOfSCTHits = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfSCTHits, xAOD::numberOfSCTHits) ) {
				msg() << "/numberOfSCTHits: " << static_cast<int>(numberOfSCTHits);
			} else {
				msg() << "/numberOfSCTHits not found";
			}

			uint8_t numberOfSCTHoles = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfSCTHoles, xAOD::numberOfSCTHoles) ) {
				msg() << "/numberOfSCTHoles: " << static_cast<int>(numberOfSCTHoles);
			} else {
				msg() << "/numberOfSCTHoles not found";
			}

			uint8_t numberOfTRTHits = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfTRTHits, xAOD::numberOfTRTHits) ) {
				msg() << "/numberOfTRTHits: " << static_cast<int>(numberOfTRTHits);
			} else {
				msg() << "/numberOfTRTHits not found";
			}

			uint8_t numberOfTRTHoles = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfTRTHoles, xAOD::numberOfTRTHoles) ) {
				msg() << "/numberOfTRTHoles: " << static_cast<int>(numberOfTRTHoles);
			} else {
				msg() << "/numberOfTRTHoles not found";
			}
			msg() << endmsg;
		}
	}

	ATH_MSG_INFO("REGTEST ==========END of xAOD::TrackParticle DUMP===========");
	ATH_MSG_DEBUG("dumpxAODTrackParticles() succeeded");

	return returnsc;
}

StatusCode TrigEDMChecker::dumpxAODVertex() {

  ATH_MSG_DEBUG("In dumpxAODVertex()");

  ATH_MSG_INFO("REGTEST ==========START of xAOD::Vertex DUMP===========");

	const xAOD::VertexContainer* vertexContainer=0;
	StatusCode sc = evtStore()->retrieve(vertexContainer,"HLT_xAOD__VertexContainer_xPrimVx");
	if (sc.isFailure()) {
      ATH_MSG_INFO("REGTEST No vertex container");
		return StatusCode::FAILURE;
	}
	ATH_MSG_INFO("REGTEST VertexContainer retrieved");

	xAOD::VertexContainer::const_iterator vertexItr = vertexContainer->begin();
	xAOD::VertexContainer::const_iterator vertexLast = vertexContainer->end();

	for (int index = 0; vertexItr != vertexLast; ++vertexItr, ++index) {
      ATH_MSG_INFO("REGTEST Looking at Vertex " << index);

      ATH_MSG_INFO("REGTEST Public Member Functions:");
      ATH_MSG_INFO("REGTEST x: " << (*vertexItr)->x()
                   << "/y: " << (*vertexItr)->y()
                   << "/z: " << (*vertexItr)->z());

      ATH_MSG_INFO("REGTEST Public Member Functions:");
      ATH_MSG_INFO("REGTEST chiSquared: " << (*vertexItr)->chiSquared()
                   << "/numberDoF: " << (*vertexItr)->numberDoF());
	}

	ATH_MSG_INFO("REGTEST ==========END of xAOD::Vertex DUMP===========");
	ATH_MSG_DEBUG("dumpxAODVertex() succeeded");

	return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpTDT() {
  ATH_MSG_INFO( "REGTEST ==========START of TDT DUMP===========" );
  // Note: This minimal TDT dumper is for use during run-3 dev
  std::vector<std::string> confChains = m_trigDec->getListOfTriggers("HLT_.*");
  for (const auto& item : confChains) {
    bool passed = m_trigDec->isPassed(item, TrigDefs::requireDecision);
    ATH_MSG_INFO("  HLT Item " << item << " passed raw? " << passed);
  }
  ATH_MSG_INFO( "REGTEST ==========END of TDT DUMP===========" );
  return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpTrigComposite() {
  ATH_MSG_INFO( "REGTEST ==========START of xAOD::TrigCompositeContainer DUMP===========" );

  if (m_doDumpAllTrigComposite) {
    m_dumpTrigCompositeContainers.clear();
    const CLID TrigCompositeCLID = static_cast<CLID>( ClassID_traits< xAOD::TrigCompositeContainer >::ID() );
    evtStore()->keys(TrigCompositeCLID, m_dumpTrigCompositeContainers);
  }

  for ( const std::string & key: m_dumpTrigCompositeContainers ) {
    // get the collection
    if ( not evtStore()->contains<xAOD::TrigCompositeContainer>(key) ) {    
      ATH_MSG_WARNING("Absent TrigCompositeContainer: " << key );
      continue;
    }
    ATH_MSG_DEBUG( " #################### Dumping container of : " << key );
    const xAOD::TrigCompositeContainer* cont= nullptr;
    ATH_CHECK( evtStore()->retrieve( cont, key ) );
    
    for ( auto tc: *cont ) {
      ATH_MSG_DEBUG("name: " << tc->name());

      // one gets a bit of info but is not that useful, we need to develop 
      //SGdebug::print_aux_vars( *tc );
      //SGdebug::dump_aux_vars( *tc );

      // find the EL and check them
      ATH_MSG_DEBUG( "Link col names   : " << tc->linkColNames() );
      ATH_MSG_DEBUG( "Link col keys    : " << tc->linkColKeys() );
      ATH_MSG_DEBUG( "Link col CLIDs   : " << tc->linkColClids() );
      ATH_MSG_DEBUG( "Link col indices : " << tc->linkColIndices() );

      // Get the objects we know of
      for (size_t i = 0; i < tc->linkColNames().size(); ++i) ATH_CHECK(checkTrigCompositeElementLink(tc, i));

    }
  }
  ATH_MSG_INFO( "REGTEST ==========END of xAOD::TrigCompositeContainer DUMP===========" );
  return StatusCode::SUCCESS;
}



StatusCode TrigEDMChecker::checkTrigCompositeElementLink(const xAOD::TrigComposite* tc, size_t element) { 

  const std::string name = tc->linkColNames().at(element);
  const CLID clid = static_cast<CLID>(tc->linkColClids().at(element));

  if (clid == ClassID_traits< TrigRoiDescriptorCollection >::ID()) { 

    const ElementLink<TrigRoiDescriptorCollection> elementLink = tc->objectLink<TrigRoiDescriptorCollection>(name);
    if (!elementLink.isValid()) ATH_MSG_WARNING("Invalid element link from '" << tc->name() << "' to '" << name << "'");
    else ATH_MSG_DEBUG("Got TrigRoiDescriptor:" << *elementLink);

  } else if (clid == ClassID_traits< DataVector< LVL1::RecEmTauRoI > >::ID()) { // There could be a few ROI types....
    // CLASS_DEF( DataVector< LVL1::RecEmTauRoI >, 6256, 1 )

    const ElementLink<DataVector< LVL1::RecEmTauRoI >> elementLink = tc->objectLink<DataVector< LVL1::RecEmTauRoI >>(name);
    if (!elementLink.isValid()) ATH_MSG_WARNING("Invalid element link from '" << tc->name() << "' to '" << name << "'");
    else ATH_MSG_DEBUG("Got LVL1::RecEmTauRoI:" << *elementLink);

   } else if (clid == ClassID_traits< xAOD::TrigCompositeContainer >::ID()) {
    
    const ElementLink<xAOD::TrigCompositeContainer> elementLink = tc->objectLink<xAOD::TrigCompositeContainer>(name);
    if (!elementLink.isValid()) ATH_MSG_WARNING("Invalid element link from '" << tc->name() << "' to '" << name << "'");
    else ATH_MSG_DEBUG("Got TrigComposite:" << (*elementLink)->name());

  } else {
    ATH_MSG_DEBUG("Ignoring link to '" << name << "' with CLID " << clid);
  }

  return StatusCode::SUCCESS;

}


StatusCode TrigEDMChecker::TrigCompositeNavigationToDot(std::string& returnValue) {

  // This constexpr is evaluated at compile time
  const CLID TrigCompositeCLID = static_cast<CLID>( ClassID_traits< xAOD::TrigCompositeContainer >::ID() );
  std::vector<std::string> keys;
  if ( m_doDumpAll ) {
    evtStore()->keys(TrigCompositeCLID, keys);
  }
  else {
    std::vector<std::string> keys = m_dumpTrigCompositeContainers;
  }
  std::string typeNameTC;
  ATH_CHECK(m_clidSvc->getTypeNameOfID(TrigCompositeCLID, typeNameTC));
  ATH_MSG_DEBUG("Got " <<  keys.size() << " keys for " << typeNameTC);

  // First retrieve them all (this should not be needed in future)
  const xAOD::TrigCompositeContainer* container = nullptr;
  for (const std::string key : keys) ATH_CHECK( evtStore()->retrieve( container, key ) );

  std::stringstream ss;
  ss << "digraph {" << std::endl;
  ss << "  node [shape=rectangle]" << std::endl;
  ss << "  rankdir = BT" << std::endl;

  // Now process them
  for (const std::string key : keys) {
    ATH_CHECK( evtStore()->retrieve( container, key ) );
    size_t index = 0;
    ss << "  subgraph " << key << " {" << std::endl;
    ss << "    label=\"" << key << "\"" << std::endl;
    // ss << "    rank=same" << std::endl; // dot cannot handle this is seems
    for (const xAOD::TrigComposite* tc : *container ) {
      // Output my name
      ss << "    \"" << tc << "\" [label=\"Container=" << typeNameTC; 
      if (tc->name() != "") ss << "\\nName=" << tc->name();
      ss << "\\nKey=" << key << "\\nIndex=" << std::to_string(index);
      std::vector<unsigned> decisions;
      if (tc->getDetail("decisions", decisions) && decisions.size() > 0) {
        ss << "\\nPass=";
        for (unsigned decisionID : decisions) {
          ss << std::hex << decisionID << "," ;
        }
      }
      ss << "\"]" << std::endl;
      // Output all the things I link to
      for (size_t i = 0; i < tc->linkColNames().size(); ++i) {
        const std::string link = tc->linkColNames().at(i);
        if (link == "seed") {
          const xAOD::TrigComposite* seed = tc->object<xAOD::TrigComposite>("seed");
          ss << "    \"" << tc << "\" -> \"" << seed << "\" [label=\"seed\"]" << std::endl; 
        } else {
          // Start with my class ID
          const CLID linkCLID = static_cast<CLID>( tc->linkColClids().at(i) );
          // Use it to get my class name
          std::string tname;
          ATH_CHECK(m_clidSvc->getTypeNameOfID(linkCLID, tname));
          // Now ge the sgkey I'm linking to & the index
          const SG::sgkey_t key = static_cast<SG::sgkey_t>( tc->linkColKeys().at(i) );
          const unsigned index = tc->linkColIndices().at(i);
          // Look it up
          CLID checkCLID;
          const std::string* keyStr = evtStore()->keyToString(key, checkCLID);
          if (keyStr != nullptr && checkCLID != linkCLID) {
            std::string tnameOfCheck;
            m_clidSvc->getTypeNameOfID(checkCLID, tnameOfCheck).ignore(); // Might be invalid. But we don't care.
            ATH_MSG_ERROR("Inconsistent CLID " << checkCLID << " [" << tnameOfCheck << "] stored in storegate for key " << key
              << ". We were expecting " << linkCLID << " [" << tname << "]");
          }
          // Print
          ss << "    \"" << tc << "\" -> \"";
          ss << "Container=" << tname << "\\nKey=";
          if (keyStr != nullptr) ss << *keyStr;
          else ss << "[KEY "<< key <<" NOT IN STORE]"; 
          ss << "\\nIndex=" << index << "\" [label=\"" << link << "\"]" << std::endl; 
        }
      }
      ++index;
    }
    ss << "  }" << std::endl;
  }

  ss << "}" << std::endl;

  returnValue.assign( ss.str() );
  return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpNavigation()
{
  // Get object from store
  const xAOD::TrigNavigation * navigationHandle = nullptr;
  ATH_CHECK( evtStore()->retrieve( navigationHandle, m_navigationHandleKey.key() ) );
  // Proper version doesn't work - conversion issue?
  //SG::ReadHandle< xAOD::TrigNavigation > navigationHandle = SG::ReadHandle< xAOD::TrigNavigation >( m_navigationHandleKey );
  //if ( !navigationHandle.isValid() ) ATH_MSG_FATAL( "Could not retrieve navigation" );

  // Get serialised navigation info
  const std::vector< unsigned int > serialisedNavigation = navigationHandle->serialized();
  ATH_MSG_INFO( "Serialised navigation size: " << serialisedNavigation.size() );

  // Convert the input
  HLT::Navigation* testNav = m_navigationTool.get();
  testNav->deserialize( serialisedNavigation );

  // Make a map of TE name hashes
  const xAOD::TriggerMenuContainer * testMenu = nullptr;
  ATH_CHECK( inputMetaStore()->retrieve( testMenu, "TriggerMenu" ) );
  std::map< int, std::string > hash2string;
  for ( auto const& sequence : testMenu->front()->sequenceInputTEs() ) {
    for ( auto const& name : sequence ) {
      int hash = TrigConf::HLTUtils::string2hash( name );
      hash2string[ hash ] = name;
    }
  }

  // Map TE names to chain names
  unsigned int chainCounter = 0;
  std::map< int, std::string > hash2chain;
  for ( auto const& chain : testMenu->front()->chainSignatureOutputTEs() ) {

    // Find the chain name
    std::string chainName = testMenu->front()->chainNames()[ chainCounter ];
    ++chainCounter;

    // Find all associated TEs
    for ( auto const& signature : chain ) {
      for ( auto const& name : signature ) {
        int hash = TrigConf::HLTUtils::string2hash( name );
        hash2string[ hash ] = name; // for decoding
        hash2chain[ hash ] = chainName;
      }
    }
  }

  // Define a map of TE features, to the TEs that use them. Needs a custom sort lambda
  auto cmpLambda = []( const HLT::TriggerElement::FeatureAccessHelper &lhs, const HLT::TriggerElement::FeatureAccessHelper &rhs) { 

    // Compare indices if CLID matches
    if ( lhs.getCLID() == rhs.getCLID() ) return ( lhs.getIndex() < rhs.getIndex() );

    // Compare CLIDs
    else return ( lhs.getCLID() < rhs.getCLID() );
  };
  std::map< HLT::TriggerElement::FeatureAccessHelper, std::vector< HLT::TriggerElement* >, decltype(cmpLambda) > feature2element(cmpLambda);

  // Retrieve all TE features and add them to the map
  std::vector< HLT::TriggerElement* > allTEs;
  testNav->getAll( allTEs, false );
  for ( auto element : allTEs ) {

    // Add TE features to the big map 
    for ( auto helper : element->getFeatureAccessHelpers() ) {
      feature2element[ helper ].push_back( element );
    }
  }

  // Debug - output all TEs and their ancestors
  // No duplication - only print terminal nodes
  for ( auto element : allTEs ) {
    if ( testNav->isTerminalNode( element ) ) {
      ATH_MSG_INFO( "+++++++++++ " << hash2string[ element->getId() ] << " is terminal node" );
      ATH_MSG_INFO( "ptr: " << element );
      std::queue< HLT::TriggerElement* > allAncestors;
      allAncestors.push( element );
      while ( allAncestors.size() ) {

        HLT::TriggerElement * thisElement = allAncestors.front();
        allAncestors.pop();
        auto theseAncestors = thisElement->getRelated( HLT::TriggerElement::Relation::seededByRelation );

        // Dump TE
        ATH_MSG_INFO( "te: " << thisElement->getId() << " " << hash2string[ thisElement->getId() ] );
        ATH_MSG_INFO( "  chain: " << hash2chain[ thisElement->getId() ] );
        for ( auto helper : thisElement->getFeatureAccessHelpers() ) {
          ATH_MSG_INFO( "   feat: " << helper );
        }
        ATH_MSG_INFO( theseAncestors.size() << " ancestors" );

        // Examine ancestors
        for ( auto ancestor : theseAncestors ) {
          allAncestors.push( ancestor );
        }
      }
    }
  }

  // Make the decision container
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > outputNavigation = TrigCompositeUtils::createAndStore( m_decisionsKey, getContext() ); 
  auto decisionOutput = outputNavigation.ptr();

  // Find unique chains associated with a feature
  std::map< HLT::TriggerElement const*, std::vector< int > > element2decisions;
  for ( auto pair : feature2element ) {

    // Get the feature info
    std::string featureName = testNav->label( pair.first.getCLID(), pair.first.getIndex().subTypeIndex() );
    auto sgKey = evtStore()->stringToKey( featureName, pair.first.getCLID() );

    // Store RoIs with appropriate label ?
    std::string storeFeatureName = "feature";
/*    if ( pair.first.getCLID() == ClassID_traits< TrigRoiDescriptor >::ID() ) {
      storeFeatureName = "roi";
    }*/

    // Make a decision object for the feature
    auto decision = TrigCompositeUtils::newDecisionIn( decisionOutput );
    decision->typelessSetObjectLink( storeFeatureName, sgKey, pair.first.getCLID(), pair.first.getIndex().objectsBegin(), pair.first.getIndex().objectsEnd() );

    // Examine associated TEs, look for chains
    std::set< std::string > passedChains;
    for ( HLT::TriggerElement const* element : pair.second ) {

      // TODO - find out what chains actually passed!
      passedChains.insert( hash2chain[ element->getId() ] );

      // Index the TE
      int decisionNumber = decisionOutput->size() - 1;
      element2decisions[ element ].push_back( decisionNumber );
    }

    // Store unique chains in the decision
    for ( auto& chain : passedChains ) {
      TrigCompositeUtils::addDecisionID( TrigConf::HLTUtils::string2hash( chain ), decision );
    }
  }

  // Store decision ancestry (had to go through once before to ensure indices populated)
  unsigned int decisionCounter = 0;
  for ( auto pair : feature2element ) {

    // Get current decision
    auto decision = decisionOutput->at( decisionCounter );
    ++decisionCounter;

    // Find ancestor TEs
    for ( auto element : pair.second ) {
      auto theseAncestors = element->getRelated( HLT::TriggerElement::Relation::seededByRelation );
      for ( auto ancestor : theseAncestors ) {
        for ( int decisionIndex : element2decisions[ ancestor ] ) {
          TrigCompositeUtils::linkToPrevious( decision, m_decisionsKey.key(), decisionIndex );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
