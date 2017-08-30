/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** Adapted from code by A.Hamilton to check trigger EDM; R.Goncalo 21/11/07 */

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"

#include "xAODTrigger/TrigPassBitsContainer.h"
#include "xAODTrigger/TrigPassBits.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "TrigMuonEvent/MuonFeature.h"
//#include "TrigMuonEvent/CombinedMuonFeature.h"
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
//#include "AnalysisTools/IAnalysisTools.h"

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


#include <iostream>

//static const int  MAX_PARTICLES = 20;

static  int trackWarningNum;
static  int vertexWarningNum;
static  int  maxRepWarnings;


// TrigEDMChecker::TrigEDMChecker(const std::string& name,
//   ISvcLocator* pSvcLocator) : CBNT_AthenaAwareBase(name, pSvcLocator),
//   m_analysisTools(0) {
TrigEDMChecker::TrigEDMChecker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_muonPrinter("Rec::MuonPrintingTool/MuonPrintingTool")
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
}



TrigEDMChecker::~TrigEDMChecker() {}

StatusCode TrigEDMChecker::initialize() {

	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "Initializing TrigEDMChecker" << endreq;

	mLog << MSG::INFO << "REGTEST Initializing..." << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpAll                     = " <<  m_doDumpAll  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpLVL1_ROI                = " <<  m_doDumpLVL1_ROI << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigMissingET           = " <<  m_doDumpTrigMissingET  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODTrigMissingET       = " <<  m_doDumpxAODTrigMissingET  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpMuonFeature             = " <<  m_doDumpMuonFeature  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpCombinedMuonFeature     = " <<  m_doDumpCombinedMuonFeature  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTileMuFeature           = " <<  m_doDumpTileMuFeature << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTileTrackMuFeature      = " <<  m_doDumpTileTrackMuFeature << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigPhotonContainer     = " <<  m_doDumpTrigPhotonContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigL2BphysContainer    = " <<  m_doDumpTrigL2BphysContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigEFBphysContainer    = " <<  m_doDumpTrigEFBphysContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigEFBjetContainer     = " <<  m_doDumpTrigEFBjetContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigL2BjetContainer     = " <<  m_doDumpTrigL2BjetContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODJetContainer        = " <<  m_doDumpxAODJetContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigMuonEFContainer     = " <<  m_doDumpTrigMuonEFContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigMuonEFInfoContainer = " <<  m_doDumpTrigMuonEFInfoContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODMuonContainer       = " <<  m_doDumpxAODMuonContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigElectronContainer   = " <<  m_doDumpTrigElectronContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODTrigElectronContainer   = " <<  m_doDumpxAODTrigElectronContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODTrigPhotonContainer   = " <<  m_doDumpxAODTrigPhotonContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODElectronContainer   = " <<  m_doDumpxAODElectronContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODPhotonContainer   = " <<  m_doDumpxAODPhotonContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpHLTResult               = " <<  m_doDumpHLTResult  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigTauContainer        = " <<  m_doDumpTrigTauContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigTauTracksInfo       = " <<  m_doDumpTrigTauTracksInfo  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigInDetTrackCollection= " <<  m_doDumpTrigInDetTrackCollection  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigVertexCollection    = " <<  m_doDumpTrigVertexCollection  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigEMCluster           = " <<  m_doDumpTrigEMCluster  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigEMClusterContainer        = " <<  m_doDumpTrigEMClusterContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrigTauClusterContainer = " <<  m_doDumpTrigTauClusterContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTrackParticleContainer          = " <<  m_doDumpTrackParticleContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpTauJetContainer          = " <<  m_doDumpTauJetContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODTrackParticle         = " <<  m_doDumpxAODTrackParticle  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODVertex                = " <<  m_doDumpxAODVertex  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODTauJetContainer          = " << m_doDumpxAODTauJetContainer  << endreq;
	mLog << MSG::INFO << "REGTEST m_doDumpxAODTrigMinBias          = " << m_doDumpxAODTrigMinBias  << endreq;


//      puts this here for the moment
        maxRepWarnings = 5;
	mLog << MSG::INFO << "maxRepWarning      = " <<  maxRepWarnings  << endreq;

	vertexWarningNum = 0;
        trackWarningNum = 0;

	if(m_doDumpxAODMuonContainer || m_doDumpAll) {
	  StatusCode sc = m_muonPrinter.retrieve();
	  if(sc.isFailure()) {
	    ATH_MSG_ERROR("Could not retrieve MuonPrinter tool");
	    return sc;
	  }
	}

	return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode TrigEDMChecker::finalize() {
	MsgStream mLog( messageService(), name() );

	return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
/// Clear - clear CBNT members
// StatusCode TrigEDMChecker::CBNT_clear() {

//   return StatusCode::SUCCESS;
// }

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode TrigEDMChecker::execute() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in execute()" << endreq;

	if(m_doDumpTrackParticleContainer){
		StatusCode sc = dumpTrackParticleContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrackParticleContainer() failed" << endreq;

		}
	}

	if(m_doDumpAll || m_doDumpLVL1_ROI ){
		StatusCode sc = dumpLVL1_ROI();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpLVL1_ROI() failed" << endreq;

		}
	}

    /*
	if(m_doDumpAll || m_doDumpTrigMissingET){
		StatusCode sc = dumpTrigMissingET();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigMissingET() failed" << endreq;

		}
	}
	*/

	if(m_doDumpAll || m_doDumpxAODTrigMissingET){
		StatusCode sc = dumpxAODTrigMissingET();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODTrigMissingET() failed" << endreq;

		}
    }

	if(m_doDumpAll || m_doDumpMuonFeature){
		StatusCode sc = dumpMuonFeature();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpMuonFeature() failed" << endreq;

		}
	}

	if(m_doDumpAll || m_doDumpCombinedMuonFeature){
		StatusCode sc = dumpCombinedMuonFeature();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpCombinedMuonFeature() failed" << endreq;

		}
		sc = dumpCombinedMuonFeatureContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpCombinedMuonFeatureContainer() failed" << endreq;
			return StatusCode::SUCCESS;
		}
	}

	if(m_doDumpAll || m_doDumpTileMuFeature) {
		StatusCode sc = dumpTileMuFeatureContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTileMuFeatureContainer() failed"
			<< endreq;

		}
	}

	if(m_doDumpAll || m_doDumpTileTrackMuFeature) {
		StatusCode sc = dumpTileTrackMuFeatureContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR
			<< "The method dumpTileTrackMuFeatureContainer() failed" << endreq;

		}
	}

	if(m_doDumpAll || m_doDumpTrigEMCluster){
		StatusCode sc = dumpTrigEMCluster();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigEMCluster() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigEMClusterContainer){
		StatusCode sc = dumpTrigEMClusterContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigEMClusterContainer() failed" << endreq;


		}
	}

        if(m_doDumpAll || m_doDumpxAODTrigEMCluster){
                StatusCode sc = dumpxAODTrigEMCluster();
                if (sc.isFailure()) {
                        mLog << MSG::ERROR << "The method dumpxAODTrigEMCluster() failed" << endreq;


                }
        }

        if(m_doDumpAll || m_doDumpxAODTrigEMClusterContainer){
                StatusCode sc = dumpxAODTrigEMClusterContainer();
                if (sc.isFailure()) {
                        mLog << MSG::ERROR << "The method dumpxAODTrigEMClusterContainer() failed" << endreq;


                }
        }

	if(m_doDumpTrigTauClusterContainer){
		StatusCode sc = dumpTrigTauClusterContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigTauClusterContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigPhotonContainer){
		StatusCode sc = dumpTrigPhotonContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigPhotonContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpxAODJetContainer){
		StatusCode sc = dumpxAODJetContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODJetContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigL2BphysContainer){
		StatusCode sc = dumpTrigL2BphysContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigL2BphysContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigEFBphysContainer){
		StatusCode sc = dumpTrigEFBphysContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigEFBphysContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigEFBjetContainer){
		StatusCode sc = dumpTrigEFBjetContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigEFBjetContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigL2BjetContainer){
		StatusCode sc = dumpTrigL2BjetContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigL2BjetContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigMuonEFContainer){
		StatusCode sc = dumpTrigMuonEFContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigMuonEFContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigMuonEFInfoContainer){
		StatusCode sc = dumpTrigMuonEFInfoContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigMuonEFInfoContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigMuonEFIsolationContainer) {
	  StatusCode sc = dumpTrigMuonEFIsolationContainer();
	  if(sc.isFailure()) {
	    mLog << MSG::ERROR << "The method dumpTrigMuonEFIsolationContainer() failed" << endreq;
	  }
	}

	if(m_doDumpAll || m_doDumpxAODMuonContainer) {
	  StatusCode sc = dumpxAODMuonContainer();
	  if(sc.isFailure()) {
	    mLog << MSG::ERROR << "The method dumpxAODMuonContainer() failed" << endreq;
	  }
	}

	if(m_doDumpAll || m_doDumpTrigElectronContainer){
		StatusCode sc = dumpTrigElectronContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigElectronContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpxAODTrigElectronContainer){
		StatusCode sc = dumpxAODTrigElectronContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODTrigElectronContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpxAODTrigPhotonContainer){
		StatusCode sc = dumpxAODTrigPhotonContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODTrigElectronContainer() failed" << endreq;


		}
	}

        if(m_doDumpAll || m_doDumpxAODElectronContainer){
		StatusCode sc = dumpxAODElectronContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODTrigElectronContainer() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpxAODPhotonContainer){
		StatusCode sc = dumpxAODPhotonContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODTrigElectronContainer() failed" << endreq;


		}
	}

        if(m_doDumpTrigTauContainer){
		StatusCode sc = dumpTrigTauContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigTauContainer() failed" << endreq;


		}
	}

	if(m_doDumpTrigTauTracksInfo){
		StatusCode sc = dumpTrigTauTracksInfo();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigTauTracksInfo() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpHLTResult){
		StatusCode sc = dumpHLTResult();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpHLTResult() failed" << endreq;


		}
	}

	if(m_doDumpTrigInDetTrackCollection){
		StatusCode sc = dumpTrigInDetTrackCollection();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigInDetTrackCollection() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpTrigVertexCollection){
		StatusCode sc = dumpTrigVertexCollection();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigVertexCollection() failed" << endreq;


		}
	}

	if(m_doDumpAll || m_doDumpxAODTauJetContainer){
		StatusCode sc = dumpxAODTauJetContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODTauJetContainer() failed" << endreq;


		}
	}




	if(m_doDumpTauJetContainer){
		StatusCode sc = dumpTauJetContainer();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTauJetContainer() failed" << endreq;

		}
	}

	if(m_doDumpAll || m_doDumpxAODTrackParticle){
		StatusCode sc = dumpxAODTrackParticle();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODTrackParticle() failed" << endreq;

		}
	}

	if(m_doDumpAll || m_doDumpxAODVertex){
		StatusCode sc = dumpxAODVertex();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODVertex() failed" << endreq;

		}
	}


	if (m_doDumpAll || m_doDumpxAODTrigMinBias){
		StatusCode sc = dumpxAODTrigMinBias();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpxAODTrigMinBias() failed" << endreq;
		}

	}
	if (m_doDumpTrigPassBits){
		StatusCode sc = dumpTrigPassBits();
		if (sc.isFailure()) {
			mLog << MSG::ERROR << "The method dumpTrigPassBits() failed" << endreq;
		}

	}

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

void TrigEDMChecker::dumpTrigSpacePointCounts(MsgStream &mLog)
{
	mLog << MSG::INFO << "MinBias in dumpTrigSpacePointCounts()" << endreq;

	std::string METTag="HLT_xAOD__TrigSpacePointCountsContainer_spacepoints";

	const xAOD::TrigSpacePointCountsContainer* SpacePointCountsCont=0;
	StatusCode sc = evtStore()->retrieve(SpacePointCountsCont,METTag);

	if (sc.isFailure())
	         mLog << MSG::INFO << "failed to retrieve " << METTag << endreq;
        else {
	         mLog << MSG::INFO << "Accessing " << METTag << " with " << SpacePointCountsCont->size() << " elements" << endreq;

		std::string s; char buff[128];
		std::vector<float> getVec;
		float sum;

		// Loop over container content
    for(uint i = 0; i < SpacePointCountsCont->size(); i++) {
			getVec = SpacePointCountsCont->at(i)->contentsPixelClusEndcapC();
			sum = 0.;
			for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
			snprintf(buff, 128, "REGTEST %s SUM of contentsPixelClusEndcapC() =         %10.2f ", s.c_str(), sum );
             		mLog <<MSG::INFO << buff << endreq;

			getVec = SpacePointCountsCont->at(i)->contentsPixelClusBarrel();
			sum = 0.;
			for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
			snprintf(buff, 128, "REGTEST %s SUM of contentsPixelClusBarrel() =         %10.2f ", s.c_str(), sum );
             		mLog <<MSG::INFO << buff << endreq;

			getVec = SpacePointCountsCont->at(i)->contentsPixelClusEndcapA();
			sum = 0.;
			for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
			snprintf(buff, 128, "REGTEST %s SUM of contentsPixelClusEndcapA() =         %10.2f ", s.c_str(), sum );
             		mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s pixelClusTotBins() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusTotBins() );
            	 	mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s pixelClusTotMin() =        %10.2f ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusTotMin() );
            	 	mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s pixelClusTotMax() =        %10.2f ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusTotMax() );
            	 	mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s pixelClusSizeBins() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusSizeBins() );
            	 	mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s pixelClusSizeMin()  =        %10.2f ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusSizeMin() );
            	 	mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s pixelClusSizeMax() =        %10.2f ", s.c_str(), SpacePointCountsCont->at(i)->pixelClusSizeMax() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s sctSpEndcapC() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->sctSpEndcapC() );
            	 	mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s sctSpBarrel() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->sctSpBarrel() );
            	 	mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s sctSpEndcapA() =        %u ", s.c_str(), SpacePointCountsCont->at(i)->sctSpEndcapA() );
            	 	mLog <<MSG::INFO << buff << endreq;
		}
	}
}

void TrigEDMChecker::dumpTrigT2MBTSBits(MsgStream &mLog){
	mLog << MSG::INFO << "MinBias in dumpTrigT2MBTSBits()" << endreq;

	std::string METTag="HLT_xAOD__TrigT2MbtsBitsContainer_T2Mbts";

	const xAOD::TrigT2MbtsBitsContainer* T2MbtsBitsCont=0;
	StatusCode sc = evtStore()->retrieve(T2MbtsBitsCont,METTag);

	if (sc.isFailure())
	         mLog << MSG::INFO << "failed to retrieve " << METTag << endreq;
        else {
	         mLog << MSG::INFO << "Accessing " << METTag << " with " << T2MbtsBitsCont->size() << " elements" << endreq;

		std::string s; char buff[128];
		std::vector<float> getVec;
		float sum;

		// Loop over container content
            	for(uint i = 0; i < T2MbtsBitsCont->size(); i++) {
			getVec = T2MbtsBitsCont->at(i)->triggerEnergies();
			sum = 0.;
			for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
			snprintf(buff, 128, "REGTEST %s SUM of triggerEnergies() =         %10.2f ", s.c_str(), sum );
             		mLog <<MSG::INFO << buff << endreq;

			getVec = T2MbtsBitsCont->at(i)->triggerTimes();
			sum = 0.;
			for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
			snprintf(buff, 128, "REGTEST %s SUM of triggerTimes() =         %10.2f ", s.c_str(), sum );
             		mLog <<MSG::INFO << buff << endreq;
		}
	}
}

void TrigEDMChecker::dumpTrigVertexCounts(MsgStream &mLog){
	mLog << MSG::INFO << "MinBias in dumpTrigVertexCounts()" << endreq;

	std::string METTag="HLT_xAOD__TrigVertexCountsContainer_vertexcounts";

	const xAOD::TrigVertexCountsContainer* T2VertexCountsCont=0;
	StatusCode sc = evtStore()->retrieve(T2VertexCountsCont,METTag);

	if (sc.isFailure())
	         mLog << MSG::INFO << "failed to retrieve " << METTag << endreq;
        else {
	         mLog << MSG::INFO << "Accessing " << METTag << " with " << T2VertexCountsCont->size() << " elements" << endreq;

		std::string s; char buff[128];
		std::vector<float> fgetVec;
		float fsum(0.);
		std::vector<unsigned int> ugetVec;
		unsigned int usum(0);

		// Loop over container content
            	for(uint i = 0; i < T2VertexCountsCont->size(); i++) {
			ugetVec = T2VertexCountsCont->at(i)->vtxNtrks();
			for (uint j = 0; j < ugetVec.size(); ++j) usum += ugetVec[j];
			snprintf(buff, 128, "REGTEST %s SUM of vtxNtrks() =         %u ", s.c_str(), usum );
             		mLog <<MSG::INFO << buff << endreq;

			fgetVec = T2VertexCountsCont->at(i)->vtxTrkPtSqSum();
			for (uint j = 0; j < fgetVec.size(); ++j) fsum += fgetVec[j];
			snprintf(buff, 128, "REGTEST %s SUM of vtxTrkPtSqSum() =         %10.2f ", s.c_str(), fsum );
             		mLog <<MSG::INFO << buff << endreq;
		}
	}
}

void TrigEDMChecker::dumpTrigTrackCounts(MsgStream &mLog){
	mLog << MSG::INFO << "MinBias in dumpTrigTrackCounts()" << endreq;

	std::string METTag="HLT_xAOD__TrigTrackCountsContainer_trackcounts";

	const xAOD::TrigTrackCountsContainer* T2TrackCountsCont=0;
	StatusCode sc = evtStore()->retrieve(T2TrackCountsCont,METTag);

	if (sc.isFailure())
	         mLog << MSG::INFO << "failed to retrieve " << METTag << endreq;
        else {
	         mLog << MSG::INFO << "Accessing " << METTag << " with " << T2TrackCountsCont->size() << " elements" << endreq;

		std::string s; char buff[128];
		std::vector<float> getVec;
		float sum;

		// Loop over container content
            	for(uint i = 0; i < T2TrackCountsCont->size(); i++) {
			getVec = T2TrackCountsCont->at(i)->z0_pt();
			sum = 0.;
			for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
			snprintf(buff, 128, "REGTEST %s SUM of z0_pt =         %10.2f ", s.c_str(), sum );
             		mLog <<MSG::INFO << buff << endreq;

			getVec = T2TrackCountsCont->at(i)->eta_phi();
			sum = 0.;
			for (uint j = 0; j < getVec.size(); ++j) sum += getVec[j];
			snprintf(buff, 128, "REGTEST %s SUM of eta_phi() =         %10.2f ", s.c_str(), sum );
             		mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s z0Bins() =        %u ", s.c_str(), T2TrackCountsCont->at(i)->z0Bins() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s z0Min() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->z0Min() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s z0Max() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->z0Max() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s ptBins() =        %u ", s.c_str(), T2TrackCountsCont->at(i)->ptBins() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s ptMin() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->ptMin() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s ptMax() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->ptMax() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s etaBins() =        %u ", s.c_str(), T2TrackCountsCont->at(i)->etaBins() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s etaMin() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->etaMin() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s etaMax() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->etaMax() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s phiBins() =        %u ", s.c_str(), T2TrackCountsCont->at(i)->phiBins() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s phiMin() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->phiMin() );
			mLog <<MSG::INFO << buff << endreq;

			snprintf(buff, 128, "REGTEST %s phiMax() =        %10.2f ", s.c_str(), T2TrackCountsCont->at(i)->phiMax() );
			mLog <<MSG::INFO << buff << endreq;
		}
	}
}

StatusCode TrigEDMChecker::dumpxAODTrigMinBias() {
	MsgStream mLog( messageService(), name() );

	dumpTrigSpacePointCounts(mLog);
	dumpTrigT2MBTSBits(mLog);
	dumpTrigVertexCounts(mLog);
	dumpTrigTrackCounts(mLog);

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////



StatusCode TrigEDMChecker::dumpxAODTrigMissingET() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::INFO << "dumpxAODTrigMissingET()" << endreq;

	int ntag=4;
	std::string METTags[]={"HLT_xAOD__TrigMissingETContainer_EFJetEtSum","HLT_xAOD__TrigMissingETContainer_TrigEFMissingET", "HLT_xAOD__TrigMissingETContainer_TrigL2MissingET_FEB","HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl"};

        for(int itag=0; itag <ntag; itag++) {

           const xAOD::TrigMissingETContainer* MissingETCont=0;
           StatusCode sc = evtStore()->retrieve(MissingETCont,METTags[itag]);
           if (sc.isFailure())
	         mLog << MSG::INFO << "failed to retrieve " << METTags[itag] << endreq;
           else {
	         mLog << MSG::INFO << "Accessing " << METTags[itag] << " with " << MissingETCont->size() << " elements" << endreq;

            // Loop over container content
            for(uint i = 0; i < MissingETCont->size(); i++) {

             std::string s; char buff[128];
             
             snprintf(buff, 128, "REGTEST %s Ex =         %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->ex() );
             mLog <<MSG::INFO << buff << endreq;
             snprintf(buff, 128, "REGTEST %s Ey =         %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->ey() );
             mLog <<MSG::INFO << buff << endreq;
             snprintf(buff, 128, "REGTEST %s Ez =         %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->ez() );
             mLog <<MSG::INFO << buff << endreq;
             snprintf(buff, 128, "REGTEST %s SumET =         %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->sumEt() );
             mLog <<MSG::INFO << buff << endreq;
             snprintf(buff, 128, "REGTEST %s SumE =       %10.2f CLHEP::MeV", s.c_str(), MissingETCont->at(i)->sumE() );
             mLog <<MSG::INFO << buff << endreq;
             snprintf(buff, 128, "REGTEST %s Flag =       %d", s.c_str(), MissingETCont->at(i)->flag() );
             mLog <<MSG::INFO << buff << endreq;
             snprintf(buff, 128, "REGTEST %s Flag =       %d", s.c_str(), MissingETCont->at(i)->roiWord() );
             mLog <<MSG::INFO << buff << endreq;
             

     		unsigned int Nc = MissingETCont->at(i)->getNumberOfComponents();
		    if (Nc > 0) { s="REGTEST __name____status_usedChannels__sumOfSigns__calib1_calib0";
			              s+="/MeV__ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/CLHEP::MeV";
			              mLog << MSG::INFO << s << endreq; }
             
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

				snprintf(buff, 128,
						"REGTEST   %s   %6d %12d %10d   %6.2f  %6.3f %10.2f %10.2f %10.2f %10.2f %10.2f",
						name.c_str(), status, usedChan, sumOfSigns, calib1, calib0,
						ex, ey, ez, sumE, sumEt);
				mLog << MSG::INFO << buff << endreq;
				
             }
              

            }

          }

        }

        return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////



StatusCode TrigEDMChecker::dumpTrigMissingET() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigMissingET()" << endreq;

	int ntag=3;
	std::string METTags[]={"HLT_TrigMissingETContainer_TrigEFMissingET", "HLT_TrigMissingETContainer_TrigEFMissingET_FEB", "HLT_TrigMissingETContainer_TrigEFMissingET_topocl"};


	/// >= 14.2.10 /// ----------------------------
	for (int itag=0; itag < ntag; itag++) { // loop over L2, EF
		const TrigMissingETContainer* trigMETcont;
		StatusCode sc=evtStore()->retrieve(trigMETcont , METTags[itag]);
		if( sc.isFailure() ){
			mLog << MSG::INFO << "Failed to retrieve TrigMissingETContainer with key " << METTags[itag] << endreq;
			continue;
		}

		mLog << MSG::INFO << "Got TrigMissingETContainer with key \"" << METTags[itag]<< "\"" << endreq;

		TrigMissingETContainer::const_iterator trigMETfirst  = trigMETcont->begin();
		TrigMissingETContainer::const_iterator trigMETlast = trigMETcont->end();

		for (int j=0; trigMETfirst !=  trigMETlast;  trigMETfirst++, j++  ) {

			mLog <<MSG::INFO << "REGTEST ==========START of TrigMissingET DUMP===========" << endreq;


			std::string s;
			char buff[128];

			snprintf(buff, 128, "REGTEST %s Ex =         %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->ex() );
			mLog <<MSG::INFO << buff << endreq;
			snprintf(buff, 128, "REGTEST %s Ey =         %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->ey() );
			mLog <<MSG::INFO << buff << endreq;
			snprintf(buff, 128, "REGTEST %s Ez =         %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->ez() );
			mLog <<MSG::INFO << buff << endreq;
			snprintf(buff, 128, "REGTEST %s Et =         %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->et() );
			mLog <<MSG::INFO << buff << endreq;
			snprintf(buff, 128, "REGTEST %s SumEt =      %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->sumEt() );
			mLog <<MSG::INFO << buff << endreq;
			snprintf(buff, 128, "REGTEST %s SumE =       %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->sumE() );
			mLog <<MSG::INFO << buff << endreq;
			snprintf(buff, 128, "REGTEST %s E =          %10.2f CLHEP::MeV", s.c_str(), (*trigMETfirst)->e() );
			mLog <<MSG::INFO << buff << endreq;
			snprintf(buff, 128, "REGTEST %s flag =    %10d",       s.c_str(), (*trigMETfirst)->getFlag() );
			mLog <<MSG::INFO << buff << endreq;
			snprintf(buff, 128, "REGTEST %s RoIword = %10ld",      s.c_str(), (*trigMETfirst)->RoIword() );
			mLog <<MSG::INFO << buff << endreq;

			unsigned int Nc = (*trigMETfirst)->getNumOfComponents();
			if (Nc > 0) {
				s="REGTEST __name____status_usedChannels__sumOfSigns__calib1_calib0";
				s+="/MeV__ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/CLHEP::MeV";
				mLog << MSG::INFO << s << endreq;

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

				    snprintf(buff, 128,
							"REGTEST   %s   %6d %12d %10d   %6.2f  %6.3f %10.2f %10.2f %10.2f %10.2f %10.2f",
							name.c_str(), status, usedChan, sumOfSigns, calib1, calib0,
							ex, ey, ez, sumE, sumEt);
					mLog << MSG::INFO << buff << endreq;
				} // loop over components
			}
		} // loop over TrigMissingET objects
	} // loop over TrigMissingETContainers

	// if( sc.isSuccess() ) return sc; // Commented out by FB (12.07.14)

	/// up to 14.2.0 /// ----------------------------
	mLog <<MSG::INFO << "Trying to fetch TrigMissingET objects from older releases" << endreq;

	const DataHandle<TrigMissingET> trigMETfirst ,trigMETlast;
	StatusCode sc=evtStore()->retrieve(trigMETfirst ,trigMETlast);
	if( sc.isFailure() ){
		mLog << MSG::INFO << "Failed to retrieve TrigMissingET (rel. <= 14.2.0)" << endreq;
	}

	for( ; trigMETfirst != trigMETlast ; ++trigMETfirst ){ // loop over TrigMissingET objects
		std::string name(trigMETfirst.key());
		mLog << MSG::INFO << "Got TrigMissingET object with key \"" << name << "\"" << endreq;

		std::string s;
		char buff[128];

		if( name.find("TrigEFMissingET") != std::string::npos ) {
			s="REGTEST EF: ";
		} else if( name.find("T2MissingET") != std::string::npos ){
			s="REGTEST L2: ";
		} else {
			mLog << MSG::WARNING << " This is UNKNOWN! " << name << endreq;
			s="REGTEST ??? ";
		}

		snprintf(buff, 128, "%s Ex =         %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->ex() );
		mLog <<MSG::INFO << buff << endreq;
		snprintf(buff, 128, "%s Ey =         %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->ey() );
		mLog <<MSG::INFO << buff << endreq;
		snprintf(buff, 128, "%s Ez =         %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->ez() );
		mLog <<MSG::INFO << buff << endreq;
		snprintf(buff, 128, "%s Et =         %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->et() );
		mLog <<MSG::INFO << buff << endreq;
		snprintf(buff, 128, "%s SumE =       %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->sumE() );
		mLog <<MSG::INFO << buff << endreq;
		snprintf(buff, 128, "%s SumEt =      %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->sumEt() );
		mLog <<MSG::INFO << buff << endreq;
		snprintf(buff, 128, "%s E =          %10.2f CLHEP::MeV", s.c_str(), trigMETfirst->e() );
		mLog <<MSG::INFO << buff << endreq;
		snprintf(buff, 128, "%s flag =    %10d",       s.c_str(), trigMETfirst->getFlag() );
		mLog <<MSG::INFO << buff << endreq;
		snprintf(buff, 128, "%s RoIword = %10ld",      s.c_str(), trigMETfirst->RoIword() );
		mLog <<MSG::INFO << buff << endreq;

		unsigned int Nc = trigMETfirst->getNumOfComponents();
		if (Nc > 0) {
			s="REGTEST __name____status_usedChannels__sumOfSigns__calib1_calib0";
			s+="/MeV__ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/CLHEP::MeV";
			mLog << MSG::INFO << s << endreq;

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

				snprintf(buff, 128,
						"REGTEST   %s   %6d %12d %10d   %6.2f  %6.3f %10.2f %10.2f %10.2f %10.2f %10.2f",
						name.c_str(), status, usedChan, sumOfSigns, calib1, calib0,
						ex, ey, ez, sumE, sumEt);
				mLog << MSG::INFO << buff << endreq;
			} // loop over components
		}
	} // loop over TrigMissingET objects

	mLog << MSG::INFO << "REGTEST ==========END of TrigMissingET DUMP===========" << endreq;

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrackParticleContainer() {

	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrackParticleContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrackParticleContainer DUMP===========" << endreq;

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
		const Rec::TrackParticleContainer*  pTrackParticleC;
		StatusCode sc = evtStore()->retrieve(pTrackParticleC, trackPtags[itag]);
		if (sc.isFailure()) {
			mLog << MSG::INFO << "REGTEST No TrackParticleContainer found with tag " << trackPtags[itag] << endreq;
			continue;
		}
		mLog << MSG::INFO << "TrackParticleContainer found with tag " << trackPtags[itag]
		                                                                            << " and size " << pTrackParticleC->size() << endreq;

		Rec::TrackParticleContainer::const_iterator trackItr  = pTrackParticleC->begin();
		Rec::TrackParticleContainer::const_iterator trackItrE = pTrackParticleC->end();
		for (int ind=1; trackItr != trackItrE; ++trackItr, ind++) {
			const Rec::TrackParticle * trackParticle = (*trackItr);
			mLog << MSG::INFO << " TrackParticle " << ind << " charge "
			<< trackParticle->charge() << " p "
			<< trackParticle->p()<< " eta " << trackParticle->eta()
			<< " phi " <<  trackParticle->phi() << endreq;


			/// track
			const Trk::Track * track = trackParticle->originalTrack();
			if ( track ) {
				mLog << MSG::INFO << " Got attached track" << endreq;
				const Trk::TrackParameters* perigee = track->perigeeParameters();
				if (!perigee) {
					const auto& parameterVector = perigee->parameters();
					mLog << MSG::INFO << " q/P " << parameterVector[Trk::qOverP] <<
					" theta " << parameterVector[Trk::theta] <<
					" phi   " <<parameterVector[Trk::phi] << endreq;
				} else {
					mLog << MSG::INFO << " No perigee attached to track" << endreq;
				}

			} else {
			  if(   trackWarningNum <= maxRepWarnings ) {
			    mLog << MSG::DEBUG << " No attached track" << endreq;
			    if(  trackWarningNum == maxRepWarnings) {
			      mLog << MSG::WARNING << " Max attached track warning reached, no further warnings given" << endreq;
			    }
			    trackWarningNum++;
			  }
			}

			/// track vertex position
			const Trk::VxCandidate * vertex = trackParticle->reconstructedVertex();
			if ( vertex ) {
				const Trk::RecVertex vtx = vertex->recVertex();
				const Amg::Vector3D position = vtx.position();
				mLog << MSG::INFO << " vertex position (" << position[0] << ", " <<
				position[1] << ", " << position[2] << ") " << endreq;
			} else {
			  if(   vertexWarningNum <= maxRepWarnings ) {
			    mLog << MSG::DEBUG << " No attached vertex" << endreq;
			    if(  vertexWarningNum == maxRepWarnings) {
			      mLog << MSG::WARNING << " Max attached vertex warning reached, no further warnings given" << endreq;
			    }
			    vertexWarningNum++;
			  }
			}

			const Trk::Perigee* perigee = trackParticle->measuredPerigee();
			if (perigee) {
				const auto& parameters = perigee->parameters();
				mLog << MSG::INFO << "Trk::Perigee parameters:" << endreq;
				mLog << MSG::INFO << " * d_0   : "<< parameters[Trk::d0]       << endreq;
				mLog << MSG::INFO << " * z_0   : "<< parameters[Trk::z0]       << endreq;
				mLog << MSG::INFO << " * phi   : "<< parameters[Trk::phi]      << endreq;
				mLog << MSG::INFO << " * Theta : "<< parameters[Trk::theta]    << endreq;
				mLog << MSG::INFO << " * q/p   : "<< parameters[Trk::qOverP]   << endreq;
			} else {
				mLog << MSG::WARNING << " No attached perigee" << endreq;
			}
			/// access to TrackSummary information
			const Trk::TrackSummary* summary = trackParticle->trackSummary();
			if (summary) {
				mLog << MSG::DEBUG<<"Track summary information:"<<endreq;
				mLog << MSG::DEBUG<<" * Number of B layer hits : "<<summary->get(Trk::numberOfBLayerHits)<<endreq;
				mLog << MSG::DEBUG<<" * Number of pixel hits : "<<summary->get(Trk::numberOfPixelHits)<<endreq;
				mLog << MSG::DEBUG<<" * Number of SCT hits : "<<summary->get(Trk::numberOfSCTHits)<<endreq;
				mLog << MSG::DEBUG<<" * Number of TRT hits : "<<summary->get(Trk::numberOfTRTHits)<<endreq;

			}


		}
	}
	return returnsc;
}




//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpLVL1_ROI() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpLVL1_ROI()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of LVL1_ROI DUMP===========" << endreq;

	const LVL1_ROI * lvl1ROI;
	StatusCode sc = evtStore()->retrieve(lvl1ROI);
	if (sc.isFailure() )
	{
		mLog << MSG::INFO << "REGTEST No LVL1_ROI found" << endreq;
		return  StatusCode::SUCCESS;
	}

	mLog << MSG::INFO << "REGTEST LVL1_ROI retrieved" << endreq;

	LVL1_ROI::emtaus_type::const_iterator itEMTau   =
		(lvl1ROI->getEmTauROIs()).begin();
	LVL1_ROI::emtaus_type::const_iterator itEMTau_e =
		(lvl1ROI->getEmTauROIs()).end();
	int j = 0;
	for( ; itEMTau != itEMTau_e; ++itEMTau, ++j) {
		mLog <<MSG::INFO << "REGTEST Looking at LVL1_ROI " << j << endreq;
		mLog << MSG::INFO << "REGTEST LVL1 EmTauROI Eta     is " << itEMTau->getEta() << endreq;
		mLog << MSG::INFO << "REGTEST LVL1 EmTauROI Phi     is " << itEMTau->getPhi() << endreq;
		mLog << MSG::INFO << "REGTEST LVL1 EmTauROI EMClus  is " << itEMTau->getEMClus() << endreq;
		mLog << MSG::INFO << "REGTEST LVL1 EmTauROI TauClus is " << itEMTau->getTauClus() << endreq;
		mLog << MSG::INFO << "REGTEST LVL1 EmTauROI EMIsol  is " << itEMTau->getEMIsol() << endreq;
		mLog << MSG::INFO << "REGTEST LVL1 EmTauROI HadIsol is " << itEMTau->getHadIsol() << endreq;
		mLog << MSG::INFO << "REGTEST LVL1 EmTauROI Core    is " << itEMTau->getCore() << endreq;
		mLog << MSG::INFO << "REGTEST LVL1 EmTauROI HadCore is " << itEMTau->getHadCore() << endreq;
		mLog << MSG::INFO << "REGTEST LVL1 EmTauROI roiWord is " << itEMTau->getROIWord() << endreq;
	}

	mLog <<MSG::INFO << "REGTEST ==========END of LVL1_ROI DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpLVL1_ROI() succeeded" << endreq;

	return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigPhotonContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigPhotonContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigPhotonContainer DUMP===========" << endreq;

	const DataHandle< TrigPhotonContainer > trigPhoton;
	const DataHandle< TrigPhotonContainer > lastTrigPhoton;

	StatusCode sc = evtStore()->retrieve(trigPhoton,lastTrigPhoton);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigPhotonContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigPhotonContainers retrieved" << endreq;


	for (int i=0; trigPhoton != lastTrigPhoton; ++trigPhoton, ++i) {

	  mLog << MSG::INFO << "REGTEST Looking at TrigPhotonContainer " << i << endreq;

	  TrigPhotonContainer::const_iterator PhotonItr  = trigPhoton->begin();
	  TrigPhotonContainer::const_iterator PhotonItrE = trigPhoton->end();

	  for (int j=0; PhotonItr != PhotonItrE; ++PhotonItr, ++j ) {

	    mLog <<MSG::INFO << "REGTEST Looking at TrigPhoton " << j << endreq;

	    mLog <<MSG::INFO << "REGTEST TrigPhoton->dPhi() returns " << (*PhotonItr)->dPhi() << endreq;
	    mLog <<MSG::INFO << "REGTEST TrigPhoton->dEta() returns " << (*PhotonItr)->dEta() << endreq;
	    mLog <<MSG::INFO << "REGTEST TrigPhoton->rCore() returns " << (*PhotonItr)->rCore() << endreq;
	    mLog <<MSG::INFO << "REGTEST TrigPhoton->isValid() returns " << (*PhotonItr)->isValid() << endreq;
	    mLog <<MSG::INFO << "REGTEST TrigPhoton->Et() returns " << (*PhotonItr)->Et() << endreq;

	    if ( (*PhotonItr)->cluster() != NULL ) {
	      mLog <<MSG::INFO << "REGTEST Cluster info: " << endreq;
	      mLog <<MSG::INFO << "REGTEST cluster->e() returns " << (*PhotonItr)->cluster()->e() << endreq;
	      mLog <<MSG::INFO << "REGTEST cluster->e237() returns " << (*PhotonItr)->cluster()->e237() << endreq;
	      mLog <<MSG::INFO << "REGTEST cluster->e277() returns " << (*PhotonItr)->cluster()->e277() << endreq;
	      mLog <<MSG::INFO << "REGTEST cluster->fracs1() returns " << (*PhotonItr)->cluster()->fracs1() << endreq;
	      mLog <<MSG::INFO << "REGTEST cluster->weta2() returns " << (*PhotonItr)->cluster()->weta2() << endreq;
	      mLog <<MSG::INFO << "REGTEST cluster->ehad() returns " << (*PhotonItr)->cluster()->ehad1() << endreq;
	      mLog <<MSG::INFO << "REGTEST cluster->emaxs1() returns " << (*PhotonItr)->cluster()->emaxs1() << endreq;

	      mLog <<MSG::INFO << "REGTEST Looking at P4PtEtaPhiM " << j << endreq;

	      mLog <<MSG::INFO << "REGTEST P4PtEtaPhiM->Pt() returns " << (*PhotonItr)->pt() << endreq;
	      mLog <<MSG::INFO << "REGTEST P4PtEtaPhiM->Eta() returns " << (*PhotonItr)->eta() << endreq;
	      mLog <<MSG::INFO << "REGTEST P4PtEtaPhiM->Phi() returns " << (*PhotonItr)->phi() << endreq;
	      mLog <<MSG::INFO << "REGTEST P4PtEtaPhiM->m() returns " << (*PhotonItr)->m() << endreq;
	    }

	    // printout variables using the new << operator
	    mLog << MSG::INFO << "TrigPhoton printout:" << endreq;
	    mLog << MSG::INFO << "REGTEST " << (**PhotonItr) << endreq;

	    // do second loop to compare TrigPhotons using comparison operators
	    TrigPhotonContainer::const_iterator PhotonItr2  = PhotonItr;
	    TrigPhotonContainer::const_iterator PhotonItr2E = trigPhoton->end();

	    for (int k=0; PhotonItr2 != PhotonItr2E; ++PhotonItr2, ++k ) {
	      // find if TrigPhotons are the same (i.e. have same cluster and track
	      if ( (**PhotonItr) == (**PhotonItr2) ) {
		mLog << MSG::INFO << "REGTEST TrigPhoton nr. " << j << " is the same as TrigPhoton nr. " << k << endreq;
	      } else {
		// TrigPhotons are different, print out differences
		std::map<std::string, double> v_diff;
		diff(*(*PhotonItr),*(*PhotonItr2), v_diff);
		std::map<std::string, double>::iterator it=v_diff.begin();
		mLog << MSG::INFO << "TrigPhoton " << k << " different form TrigPhoton " << j << " :" << endreq;
		for (int m=0;it !=v_diff.end();++it, ++m) {
		  mLog << MSG::INFO << "TrigPhoton Delta_" << (*it).first << " = " << (*it).second << endreq;
		}
	      }
	    }
	  }
	}

	mLog <<MSG::INFO << "REGTEST ==========END of TrigPhotonContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigPhotonContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigMuonEFContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigMuonEFContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigMuonEFContainer DUMP===========" << endreq;

	const DataHandle< TrigMuonEFContainer > trigMuon;
	const DataHandle< TrigMuonEFContainer > lastTrigMuon;

	StatusCode sc = evtStore()->retrieve(trigMuon,lastTrigMuon);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigMuonEFContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigMuonEFContainers retrieved" << endreq;


	for (int i=0; trigMuon != lastTrigMuon; ++trigMuon, ++i) {

		mLog << MSG::INFO << "REGTEST Looking at TrigMuonEFContainer " << i << endreq;

		TrigMuonEFContainer::const_iterator MuonItr  = trigMuon->begin();
		TrigMuonEFContainer::const_iterator MuonItrE = trigMuon->end();

		for (int j=0; MuonItr != MuonItrE; ++MuonItr, ++j ) {

			mLog <<MSG::INFO << "REGTEST Looking at TrigMuonEF " << j << endreq;

			mLog <<MSG::INFO << "REGTEST TrigMuonEF->muonCode() returns " << (*MuonItr)->MuonCode() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigMuonEF->roi() returns " << (*MuonItr)->RoINum() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigMuonEF->charge() returns " << (*MuonItr)->Charge() << endreq;

			mLog <<MSG::INFO << "REGTEST Looking at P4IPtCotThPhiM " << j << endreq;

			mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->iPt() returns " << (*MuonItr)->iPt() << endreq;
			mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->CotTh() returns " << (*MuonItr)->cotTh() << endreq;
			mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->Phi() returns " << (*MuonItr)->phi() << endreq;
			mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->m() returns " << (*MuonItr)->m() << endreq;
		}
	}

	mLog <<MSG::INFO << "REGTEST ==========END of TrigMuonEFContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigMuonEFContainer() succeeded" << endreq;

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
  msg(MSG::INFO) << output << endreq;

  ATH_MSG_INFO( "REGTEST ==========END of xAOD::MuonContainer DUMP===========" );

  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigMuonEFInfoContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigMuonEFInfoContainer()" << endreq;

	mLog << MSG::INFO << "REGTEST ==========START of TrigMuonEFInfoContainer DUMP===========" << endreq;

	const DataHandle< TrigMuonEFInfoContainer > trigMuon;
	const DataHandle< TrigMuonEFInfoContainer > lastTrigMuon;

	StatusCode sc = evtStore()->retrieve(trigMuon,lastTrigMuon);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigMuonEFInfoContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigMuonEFInfoContainers retrieved" << endreq;


	for (int i=0; trigMuon != lastTrigMuon; ++trigMuon, ++i) {

		mLog << MSG::INFO << "REGTEST -+-+-+-+ Looking at TrigMuonEFInfoContainer " << i << endreq;

		TrigMuonEFInfoContainer::const_iterator MuonItr  = trigMuon->begin();
		TrigMuonEFInfoContainer::const_iterator MuonItrE = trigMuon->end();

		for (int j=0; MuonItr != MuonItrE; ++MuonItr, ++j ) {

			mLog <<MSG::INFO << "REGTEST Looking at TrigMuonEFInfo " << j << endreq;

			TrigMuonEFInfo* muonInfo = (*MuonItr);

			mLog <<MSG::INFO << "REGTEST Test self equality " << endreq;
			if (*muonInfo == *muonInfo) {
				mLog <<MSG::INFO << "REGTEST passed " << endreq;
			}
			else {
				mLog <<MSG::INFO << "REGTEST failed " << endreq;
			}


			if (muonInfo->hasTrack()) {
				mLog <<MSG::INFO << "REGTEST Test new version " << endreq;

				mLog <<MSG::INFO << "REGTEST hasTracks()=true " << endreq;
				const TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();
				mLog <<MSG::INFO << "REGTEST TrackContainer size: " <<  tc->size() << endreq;

				for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = tc->begin() ; TrackItr!=tc->end(); TrackItr++) {

					TrigMuonEFInfoTrack* muonInfo = (*TrackItr);
					mLog <<MSG::INFO << "REGTEST MuonType(): " << muonInfo->MuonType()<< endreq;

					TrigMuonEFTrack* muonTrack = muonInfo->SpectrometerTrack();
					if (muonTrack) {
						printMuonTrk(mLog,muonTrack);
					} else {
						mLog << MSG::INFO << "REGTEST no SpectrometerTrack track found" << endreq;
					}

					mLog << MSG::INFO << "REGTEST Looking at TrigMuonEFTrack ExtrapolatedTrack()" << endreq;
					muonTrack = muonInfo->ExtrapolatedTrack();
					if (muonTrack) {
						printMuonTrk(mLog,muonTrack);
					} else {
						mLog << MSG::INFO << "REGTEST no ExtrapolatedTrack track found" << endreq;
					}

					mLog << MSG::INFO << "REGTEST Looking at TrigMuonEFTrack CombinedTrack()" << endreq;
					TrigMuonEFCbTrack* muonCbTrack = muonInfo->CombinedTrack();
					if (muonCbTrack) {
						printMuonTrk(mLog,muonTrack);
					} else {
						mLog << MSG::INFO << "REGTEST no CombinedTrack track found" << endreq;
					}
				}

			}

			if (!muonInfo) {
				mLog << MSG::INFO << "REGTEST no TrigMuonEFInfo found" << endreq;
				return StatusCode::SUCCESS;
			}
			mLog << MSG::INFO << "REGTEST TrigMuonEFInfo->RoINum() returns " << muonInfo->RoINum() << endreq;
			mLog <<MSG::INFO << "REGTEST Test the backwards compatibility " << endreq;
			TrigMuonEFTrack* muonTrack = muonInfo->SpectrometerTrack();
			if (muonTrack) {
				printMuonTrk(mLog,muonTrack);
			} else {
				mLog << MSG::INFO << "REGTEST no SpectrometerTrack track found" << endreq;
			}

			mLog << MSG::INFO << "REGTEST old Looking at TrigMuonEFTrack ExtrapolatedTrack()" << endreq;
			muonTrack = muonInfo->ExtrapolatedTrack();
			if (muonTrack) {
				printMuonTrk(mLog,muonTrack);
			} else {
				mLog << MSG::INFO << "REGTEST old no ExtrapolatedTrack track found" << endreq;
			}

			mLog << MSG::INFO << "REGTEST old Looking at TrigMuonEFTrack CombinedTrack()" << endreq;
			TrigMuonEFCbTrack* muonCbTrack = muonInfo->CombinedTrack();
			if (muonCbTrack) {
				printMuonTrk(mLog,muonCbTrack);
			} else {
				mLog << MSG::INFO << "REGTEST old no CombinedTrack track found" << endreq;
			}
		}
	}

	mLog <<MSG::INFO << "REGTEST ==========END of TrigMuonEFInfoContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigMuonEFInfoContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigMuonEFIsolationContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigMuonEFIsolationContainer()" << endreq;

	mLog << MSG::INFO << "REGTEST ==========START of TrigMuonEFIsolationContainer DUMP===========" << endreq;

	const DataHandle< TrigMuonEFIsolationContainer > trigMuon;
	const DataHandle< TrigMuonEFIsolationContainer > lastTrigMuon;

	StatusCode sc = evtStore()->retrieve(trigMuon,lastTrigMuon);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigMuonEFIsolationContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigMuonEFIsolationContainers retrieved" << endreq;


	for (int i=0; trigMuon != lastTrigMuon; ++trigMuon, ++i) {

		mLog << MSG::INFO << "REGTEST -+-+-+-+ Looking at TrigMuonEFIsolationContainer " << i << endreq;

		TrigMuonEFIsolationContainer::const_iterator MuonItr  = trigMuon->begin();
		TrigMuonEFIsolationContainer::const_iterator MuonItrE = trigMuon->end();

		for (int j=0; MuonItr != MuonItrE; ++MuonItr, ++j ) {

			mLog <<MSG::INFO << "REGTEST Looking at TrigMuonEFIsolation " << j << endreq;

			TrigMuonEFIsolation* muonIsolation = (*MuonItr);

			mLog <<MSG::INFO << "REGTEST Test self equality " << endreq;
			if (*muonIsolation == *muonIsolation) {
				mLog <<MSG::INFO << "REGTEST passed " << endreq;
			}
			else {
				mLog <<MSG::INFO << "REGTEST failed " << endreq;
			}

			mLog << MSG::INFO << "REGTEST sumTrkPtCone02(): " << muonIsolation->sumTrkPtCone02() << endreq;
			mLog << MSG::INFO << "REGTEST sumTrkPtCone03(): " << muonIsolation->sumTrkPtCone03() << endreq;
			mLog << MSG::INFO << "REGTEST sumEtCone01()   : " << muonIsolation->sumEtCone01() << endreq;
			mLog << MSG::INFO << "REGTEST sumEtCone02()   : " << muonIsolation->sumEtCone02() << endreq;
			mLog << MSG::INFO << "REGTEST sumEtCone03()   : " << muonIsolation->sumEtCone03() << endreq;
			mLog << MSG::INFO << "REGTEST sumEtCone04()   : " << muonIsolation->sumEtCone04() << endreq;
			mLog << MSG::INFO << "REGTEST trackPosition() : " << muonIsolation->trackPosition()  << endreq;
			// access MuonInfo* - only works for >=2012 data
			if(muonIsolation->getMuonInfo()) {
			  mLog << MSG::INFO << "REGTEST Link MuonEFInfo found: " << muonIsolation->getMuonInfo() << endreq;
			  if(!muonIsolation->getEFMuonInfoTrack()) {
			    mLog << MSG::WARNING << "REGTEST No InfoTrack attached to this EFIsolation object" << endreq;
			  }
			  else {
			    const TrigMuonEFInfoTrack* trk = muonIsolation->getEFMuonInfoTrack();
			    mLog << MSG::INFO << "REGTEST Linke EFInfoTrack has MuonType : " << trk->MuonType() << endreq;
			    if(trk->hasCombinedTrack()) mLog << MSG::INFO << "REGTEST Linked EFInfoTrack combined pt : " << trk->CombinedTrack()->pt() << endreq;
			    if(trk->hasExtrapolatedTrack()) mLog << MSG::INFO << "REGTEST Linked EFInfoTrack extrapolated pt : " << trk->ExtrapolatedTrack()->pt() << endreq;
			    if(trk->hasSpectrometerTrack()) mLog << MSG::INFO << "REGTEST Linked EFInfoTrack MS pt : " << trk->SpectrometerTrack()->pt() << endreq;
			    if(trk->hasCombinedTrack()) {
			      if(trk->CombinedTrack()->getIDTrackParticle()) mLog << MSG::INFO << "REGTEST Linked EFInfoTrack ID track pt : " << trk->CombinedTrack()->getIDTrackParticle()->pt() << endreq;
			    }

			  }
			}else {
			  mLog << MSG::INFO << "REGTEST No Linked MuonEFInfo, expected for pre 2012 data" << endreq;
			}

		}//TrigMuonEFIsolation loop
	}//TrigMuonEFIsolationContainer loop

	mLog <<MSG::INFO << "REGTEST ==========END of TrigMuonEFIsolationContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigMuonEFIsolationContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;

}//dumpTrigMuonEFIsolationContainer


void TrigEDMChecker::printMuonTrk(MsgStream &mLog, const TrigMuonEFTrack* muonTrack) {
	mLog <<MSG::INFO << "POINTER TrigMuonEFTrack: " << muonTrack << endreq;
	//if(muonTrack) mLog << MSG::INFO << "REGTEST TrigMuonEFTrack: " << *muonTrack << endreq;

// 	mLog <<MSG::INFO << "REGTEST TrigMuonEFTrack->charge() returns " << muonTrack->Charge() << endreq;
// 	mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->iPt() returns " << muonTrack->iPt() << endreq;
// 	mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->CotTh() returns " << muonTrack->cotTh() << endreq;
// 	mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->Phi() returns " << muonTrack->phi() << endreq;
// 	mLog <<MSG::INFO << "REGTEST P4IPtCotThPhiM->m() returns " << muonTrack->m() << endreq;
// 	mLog <<MSG::INFO << "REGTEST chi2() returns " << muonTrack->chi2() << endreq;
// 	mLog <<MSG::INFO << "REGTEST chi2prob() returns " << muonTrack->chi2prob() << endreq;
// 	mLog <<MSG::INFO << "REGTEST posX() returns " << muonTrack->posX() << endreq;
// 	mLog <<MSG::INFO << "REGTEST posY() returns " << muonTrack->posY() << endreq;
// 	mLog <<MSG::INFO << "REGTEST posZ() returns " << muonTrack->posZ() << endreq;
// 	mLog <<MSG::INFO << "REGTEST NCscHitsEta() returns " << muonTrack->NCscHitsEta() << endreq;
// 	mLog <<MSG::INFO << "REGTEST NCscHitsPhi() returns " << muonTrack->NCscHitsPhi() << endreq;
// 	mLog <<MSG::INFO << "REGTEST NTgcHitsEta() returns " << muonTrack->NTgcHitsEta() << endreq;
// 	mLog <<MSG::INFO << "REGTEST NTgcHitsPhi() returns " << muonTrack->NTgcHitsPhi() << endreq;
//  	mLog <<MSG::INFO << "REGTEST NMdtHitsEta() returns " << muonTrack->NMdtHitsEta() << endreq;
//  	mLog <<MSG::INFO << "REGTEST NMdtHitsPhi() returns " << muonTrack->NMdtHitsPhi() << endreq;
// 	mLog <<MSG::INFO << "REGTEST NRpcHitsEta() returns " << muonTrack->NRpcHitsEta() << endreq;
// 	mLog <<MSG::INFO << "REGTEST NRpcHitsPhi() returns " << muonTrack->NRpcHitsPhi() << endreq;
// 	mLog <<MSG::INFO << "REGTEST d0() returns " << muonTrack->d0() << endreq;
// 	mLog <<MSG::INFO << "REGTEST z0() returns " << muonTrack->z0() << endreq;
}

void TrigEDMChecker::printMuonTrk(MsgStream &mLog, const TrigMuonEFCbTrack* muonTrack) {
	mLog <<MSG::INFO << " POINTER TrigMuonEFCbTrack: " << muonTrack << endreq;
	//if(muonTrack) mLog << MSG::INFO << "REGTEST TrigMuonEFCbTrack: " << *muonTrack << endreq;

// 	printMuonTrk(mLog,(TrigMuonEFTrack*)muonTrack);
//  	mLog <<MSG::INFO << "REGTEST TrigMuonEFCbTrack " << muonTrack << endreq;
// 	mLog <<MSG::INFO << "REGTEST NIdPixelHits() returns " << muonTrack->NIdPixelHits() << endreq;
// 	mLog <<MSG::INFO << "REGTEST NIdSctHits() returns " << muonTrack->NIdSctHits() << endreq;
// 	mLog <<MSG::INFO << "REGTEST NTrtHits() returns " << muonTrack->NTrtHits() << endreq;
// 	mLog <<MSG::INFO << "REGTEST matchChi2() returns " << muonTrack->matchChi2() << endreq;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigElectronContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigElectronContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigElectronContainer DUMP===========" << endreq;

	const DataHandle< TrigElectronContainer > trigElec;
	const DataHandle< TrigElectronContainer > lastTrigElec;

	StatusCode sc = evtStore()->retrieve(trigElec,lastTrigElec);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigelectronContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigElectronContainers retrieved" << endreq;

	// declare pointer to the last trigElectron to test comparison operators
	TrigElectronContainer::const_iterator firstTrigEl ;

	for (int i=0; trigElec != lastTrigElec; ++trigElec, ++i) {

	  mLog << MSG::INFO << "REGTEST Looking at TrigElectronContainer " << i << endreq;

	  TrigElectronContainer::const_iterator elecItr  = trigElec->begin();
	  TrigElectronContainer::const_iterator elecItrE = trigElec->end();

	  for (int j=0; elecItr != elecItrE; ++elecItr, ++j ) {

	    mLog <<MSG::INFO << "REGTEST Looking at TrigElectron " << j << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->isValid()      returns " << (*elecItr)->isValid() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->pt()           returns " << (*elecItr)->pt() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->eta()          returns " << (*elecItr)->eta() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->phi()          returns " << (*elecItr)->phi() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->Zvtx()         returns " << (*elecItr)->Zvtx() << endreq;

	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->charge()       returns " << (*elecItr)->charge() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trackIndx()    returns " << (*elecItr)->trackIndx() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trkClusDphi()  returns " << (*elecItr)->trkClusDphi() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trkClusDeta()  returns " << (*elecItr)->trkClusDeta() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->Pt() (track)   returns " << (*elecItr)->Pt() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->err_Pt()       returns " << (*elecItr)->err_Pt() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->err_eta()      returns " << (*elecItr)->err_eta() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->err_phi()      returns " << (*elecItr)->err_phi() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->err_Zvtx()     returns " << (*elecItr)->err_Zvtx() << endreq;

	    // 			mLog <<MSG::INFO << "REGTEST New methods: " << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->roiWord()      returns " << (*elecItr)->roiWord() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trkEtaAtCalo() returns " << (*elecItr)->trkEtaAtCalo() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->trkPhiAtCalo() returns " << (*elecItr)->trkPhiAtCalo() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->caloEta()      returns " << (*elecItr)->caloEta() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->caloPhi()      returns " << (*elecItr)->caloPhi() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->Rcore()        returns " << (*elecItr)->Rcore() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->Eratio()       returns " << (*elecItr)->Eratio() << endreq;
	    // 			mLog <<MSG::INFO << "REGTEST TrigElectron->EThad()        returns " << (*elecItr)->EThad() << endreq;

	    // printout variables using the new << operator
	    mLog << MSG::INFO << "TrigElectron printout:" << endreq;
	    mLog << MSG::INFO << "REGTEST " << (**elecItr) << endreq;

	    // do second loop to compare TrigElectrons using comparison operators
	    TrigElectronContainer::const_iterator elecItr2  = elecItr; //trigElec->begin();
	    TrigElectronContainer::const_iterator elecItr2E = trigElec->end();

	    for (int k=0; elecItr2 != elecItr2E; ++elecItr2, ++k ) {
	      // find if TrigElectrons are the same (i.e. have same cluster and track
	      if ( (**elecItr) == (**elecItr2) ) {
		mLog << MSG::INFO << "REGTEST TrigElectron nr. " << j << " is the same as TrigElectron nr. " << k << endreq;
	      } else {
		// TrigElectrons are different, print out differences
		std::map<std::string, double> v_diff;
		diff(*(*elecItr),*(*elecItr2), v_diff);
		std::map<std::string, double>::iterator it=v_diff.begin();
		mLog << MSG::INFO << "TrigElectron " << k << " different form TrigElectron " << j << " :" << endreq;
		for (int m=0;it !=v_diff.end();++it, ++m) {
		  mLog << MSG::INFO << "TrigElectron Delta_" << (*it).first << " = " << (*it).second << endreq;
		}
	      }
	    }
	  }
	}

	mLog <<MSG::INFO << "REGTEST ==========END of TrigElectronContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigElectronContainer() succeeded" << endreq;

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

      //msg() << MSG::VERBOSE << " REGTEST: cluster variables " << endreq;
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
      //msg() << MSG::VERBOSE << " REGTEST: cluster variables " << endreq;
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
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigTauContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigTauContainer DUMP===========" << endreq;

	const DataHandle< TrigTauContainer > trigTau;
	const DataHandle< TrigTauContainer > lastTrigTau;

	StatusCode sc = evtStore()->retrieve(trigTau,lastTrigTau);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigTauContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigTauContainer retrieved" << endreq;

	for (int i=0; trigTau != lastTrigTau; ++trigTau, ++i) {

                mLog << MSG::INFO << "REGTEST Looking at TrigTauContainer " << i << endreq;

                TrigTauContainer::const_iterator TrigTauItr  = trigTau->begin();
                TrigTauContainer::const_iterator TrigTauItrE = trigTau->end();

                for (int j=0; TrigTauItr != TrigTauItrE; ++TrigTauItr, ++j ) {

		  mLog <<MSG::INFO << "REGTEST Looking at TrigTau " << j << endreq;

	 	  mLog <<MSG::INFO << "REGTEST TrigTau->pt() returns " << (*TrigTauItr)->pt() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTau->eta() returns " << (*TrigTauItr)->eta() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTau->phi() returns " << (*TrigTauItr)->phi() << endreq;

		  mLog <<MSG::INFO << "REGTEST TrigTau->nMatchedTracks() returns " << (*TrigTauItr)->nMatchedTracks() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTau->simpleEtFlow() returns " << (*TrigTauItr)->simpleEtFlow() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTau->m() returns " << (*TrigTauItr)->m() << endreq;
                }
	}

	mLog <<MSG::INFO << "REGTEST ==========END of TrigTau DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigTauContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpTrigTauTracksInfo() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigTauTracksInfo()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigTauTracksInfo DUMP===========" << endreq;

	const DataHandle< TrigTauTracksInfo > trigTau;
	const DataHandle< TrigTauTracksInfo > lastTrigTau;

	StatusCode sc = evtStore()->retrieve(trigTau,lastTrigTau);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigTauTracksInfo found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigTauTracksInfos retrieved" << endreq;


	for (int i=0; trigTau != lastTrigTau; ++trigTau, ++i) {

		const TrigTauTracksInfo* thisTrigTau = &(*trigTau);

		mLog <<MSG::INFO << "REGTEST Looking at TrigTauTracksInfo " << i << endreq;

		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->pt() returns " << thisTrigTau->pt() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->eta() returns " << thisTrigTau->eta() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->phi() returns " << thisTrigTau->phi() << endreq;

		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->nCoreTracks() returns " << thisTrigTau->nCoreTracks() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->nSlowTracks() returns " << thisTrigTau->nSlowTracks() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->nIsoTracks() returns " << thisTrigTau->nIsoTracks() << endreq;

		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->charge() returns " << thisTrigTau->charge() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->leadingTrackPt() returns " <<
		thisTrigTau->leadingTrackPt() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->scalarPtSumCore() returns " <<
		thisTrigTau->scalarPtSumCore() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->scalarPtSumIso() returns " <<
		thisTrigTau->scalarPtSumIso() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigTauTracksInfo->threeFastestTracks().pt() returns " <<
		thisTrigTau->threeFastestTracks().pt() << endreq;

	}

	mLog <<MSG::INFO << "REGTEST ==========END of TrigTauTracksInfo DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigTauTracksInfo() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpHLTResult() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpHLTResult()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of HLTResult DUMP===========" << endreq;

	const HLT::HLTResult* hltResult = 0;
	StatusCode sc=evtStore()->retrieve( hltResult, "HLTResult_L2");
	if( sc.isFailure()  ||  !hltResult ) {
		mLog << MSG::INFO
		<< "No HLTResult_L2 found in TDS"
		<< endreq;
		return StatusCode::SUCCESS;
	}
	mLog <<MSG::INFO << "REGTEST HLTResult_L2 Successfully Retrieved" << endreq;

	mLog <<MSG::INFO << "REGTEST HLTResult_L2->isEmpty() returns " << hltResult->isEmpty() << endreq;
	mLog <<MSG::INFO << "REGTEST HLTResult_L2->size() returns " << hltResult->size() << endreq;
	mLog <<MSG::INFO << "REGTEST HLTResult_L2->isPassThrough() returns " << hltResult->isPassThrough() << endreq;
	mLog <<MSG::INFO << "REGTEST HLTResult_L2->isAccepted() returns " << hltResult->isAccepted() << endreq;


	mLog <<MSG::INFO << "REGTEST ==========END of HLTResult DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpHLTResult() succeeded" << endreq;

	return StatusCode::SUCCESS;
}




//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigInDetTrackCollection() {
	MsgStream mLog( messageService(), name() );

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


	mLog << MSG::DEBUG << "in dumpTrigInDetTrackCollection()" << endreq;

	const TrigInDetTrackTruthMap* pTruthMap(nullptr);
	bool gotTruthMap = false;

	if (evtStore()->contains<TrigInDetTrackTruthMap>("TrigInDetTrackTruthMap"))
   	{
         	StatusCode sc=evtStore()->retrieve(pTruthMap,"TrigInDetTrackTruthMap");
		if (sc.isFailure())
       		{
			mLog << MSG::WARNING << " could not retrieve TrackTruthMap with key TrigInDetTruthMap" << endreq;
		} else {
			gotTruthMap=true;
			//      pTruthMap->print();
		}
	}
 	else mLog << MSG::DEBUG << " didn't find any TrackTruthMap objects with key TrigInDetTruthMap" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigInDetTrackCollection DUMP===========" << endreq;

	for (int iTag=0; iTag < ntag; iTag++) {
		const TrigInDetTrackCollection* trigInDetTrackCollection;
		StatusCode sc = evtStore()->retrieve(trigInDetTrackCollection,TrigInDetTrackTags[iTag] );
		if (sc.isFailure()) {
			mLog << MSG::DEBUG << "REGTEST No TrigInDetTrackCollection found with key " << TrigInDetTrackTags[iTag] << endreq;
			continue;
		}
		mLog << MSG::INFO << "REGTEST TrigInDetTrackCollections retrieved with key "
		<< TrigInDetTrackTags[iTag] << endreq;


		mLog << MSG::INFO << "REGTEST TrigInDetTrackCollection->size() returns " << trigInDetTrackCollection->size() << endreq;

		TrigInDetTrackCollection::const_iterator trkItr  = trigInDetTrackCollection->begin();
		TrigInDetTrackCollection::const_iterator trkItrE = trigInDetTrackCollection->end();

		for (int j=0; trkItr != trkItrE; ++trkItr, ++j ) {

			mLog <<MSG::INFO << "REGTEST Looking at TrigInDetTrack " << j << endreq;

			mLog <<MSG::INFO << "REGTEST TrigInDetTrack->algorithmId() returns " << (*trkItr)->algorithmId() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrack->chi2() returns " << (*trkItr)->chi2() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrack->NStrawHits() returns " << (*trkItr)->NStrawHits() << endreq;

			mLog <<MSG::INFO << "REGTEST TrigInDetTrack->NStraw() returns " << (*trkItr)->NStraw() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrack->NStrawTime() returns " << (*trkItr)->NStrawTime() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrack->NTRHits() returns " << (*trkItr)->NTRHits() << endreq;

			mLog <<MSG::INFO << "REGTEST Looking at TrigInDetFitTrack->param()" << endreq;
			const TrigInDetTrackFitPar* my_param = (*trkItr)->param();

			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->a0() returns " << my_param->a0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->z0() returns " << my_param->z0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->phi0() returns " << my_param->phi0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->eta() returns " << my_param->eta() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->pT() returns " << my_param->pT() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->ea0() returns " << my_param->ea0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->ez0() returns " << my_param->ez0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->ephi0() returns " << my_param->ephi0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->eeta() returns " << my_param->eeta() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->epT() returns " << my_param->epT() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->surfaceType() returns " << my_param->surfaceType() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->surfaceCoordinate() returns " << my_param->surfaceCoordinate() << endreq;
			mLog <<MSG::INFO << "REGTEST Looking at covarience matrix: " << endreq;

			const std::vector<double>* const my_cov = my_param->cov();
			if (!my_cov) {
				mLog <<MSG::INFO << "REGTEST covarience matrix NULL (not stored)" << endreq;
			} else {
				std::vector<double> tempcov = *my_cov;
				std::vector<double>::iterator iter;
				int k;
				for(iter = tempcov.begin(), k=0; iter != tempcov.end(); iter++, k++){
					mLog <<MSG::INFO << "REGTEST TrigInDetTrackFitPar->cov() element " << k << " is " << (*iter) << endreq;
				}
			}
			if (gotTruthMap) {
				if (!(pTruthMap->hasTruth((*trkItr)))) {
					mLog << MSG::DEBUG << " REGTEST Track has no truth info  " << endreq;
				} else {
					mLog << MSG::DEBUG << " Track has truth info:  " << endreq;
					const TrigInDetTrackTruth* pTRTtruth = pTruthMap->truth((*trkItr));
					int nMatches = pTRTtruth->nrMatches() ;
					if (nMatches==0) {
						mLog << MSG::INFO << " REGTEST Track has no truth matches" <<  endreq;
					} else {
						mLog << MSG::INFO << "REGTEST number of matched truth hits: Si: " <<  pTruthMap->bestMatchSiHits((*trkItr))
						<< " TRT: " <<   pTruthMap->bestMatchTRTHits((*trkItr)) << endreq;
						if (pTruthMap->bestMatchSiHits((*trkItr)) > 0) {

							const HepMcParticleLink* pSiTruth = pTruthMap->bestMatchSi((*trkItr));

							mLog << MSG::INFO << "REGTEST Si match to kine ref " <<  pSiTruth->barcode() << endreq;
							if (pSiTruth->cptr() != 0) {
							  mLog << MSG::INFO << "REGTEST Si PDG id " << pSiTruth->cptr()->pdg_id() << endreq;
							}
						}


						if (pTruthMap->bestMatchTRTHits((*trkItr)) > 0) {
							const HepMcParticleLink* pTRTtruth = pTruthMap->bestMatchTRT((*trkItr));

							mLog << MSG::INFO << "REGTEST TRT match to kine ref " <<  pTRTtruth->barcode() << endreq;
							if (pTRTtruth->cptr() != 0) {
							  mLog << MSG::INFO << "REGTEST TRT PDG id " << pTRTtruth->cptr()->pdg_id() << endreq;
							}
						}
					}
				}

			} // if (gotTruthMap)

		}
	}


	mLog <<MSG::INFO << "REGTEST ==========END of TrigInDetTrackCollection DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigInDetTrackCollection() succeeded" << endreq;

	return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigVertexCollection() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigVertexCollection()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigVertexCollection DUMP===========" << endreq;

	const DataHandle< TrigVertexCollection > trigVertex;
	const DataHandle< TrigVertexCollection > lastTrigVertex;

	StatusCode sc = evtStore()->retrieve(trigVertex,lastTrigVertex);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigVertexCollection found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigVertexCollection retrieved" << endreq;


	for (int i=0; trigVertex != lastTrigVertex; ++trigVertex, ++i) {

		mLog << MSG::INFO << "REGTEST Looking at TrigVertexCollection " << i << endreq;

		TrigVertexCollection::const_iterator VertexItr  = trigVertex->begin();
		TrigVertexCollection::const_iterator VertexItrE = trigVertex->end();

		for (int j=0; VertexItr != VertexItrE; ++VertexItr, ++j ) {

			mLog <<MSG::INFO << "REGTEST Looking at TrigVertex " << j << endreq;

			mLog <<MSG::INFO << "REGTEST TrigVertex->x() returns " << (*VertexItr)->x() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigVertex->y() returns " << (*VertexItr)->y() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigVertex->z() returns " << (*VertexItr)->z() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigVertex->energyFraction() returns " << (*VertexItr)->energyFraction() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigVertex->ndof() returns " << (*VertexItr)->ndof() << endreq;

			if(((*VertexItr)->tracks())!=NULL ){
				//	mLog <<MSG::INFO << "REGTEST *** ((*VertexItr)->tracks())!=NULL *** " << endreq;
				TrackInVertexList::const_iterator trkItr  = (*VertexItr)->tracks()->begin();
				TrackInVertexList::const_iterator trkItrE = (*VertexItr)->tracks()->end();

				for (int p=0; trkItr != trkItrE; ++trkItr, ++p ) {

					mLog <<MSG::INFO << "REGTEST Looking at track " << p << endreq;

					mLog <<MSG::INFO << "REGTEST Tracks info: " << endreq;
					mLog <<MSG::INFO << "REGTEST tracks->NStrawHits() returns " << (*trkItr)->NStrawHits() << endreq;
					mLog <<MSG::INFO << "REGTEST tracks->NStrawTime() returns " << (*trkItr)->NStrawTime() << endreq;
					mLog <<MSG::INFO << "REGTEST tracks->NTRHits() returns " << (*trkItr)->NTRHits() << endreq;
				}
			}
		}
	}

	mLog <<MSG::INFO << "REGTEST ==========END of TrigVertexCollection DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigVertexCollection() succeeded" << endreq;

	return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////


StatusCode TrigEDMChecker::dumpTrigEFBphysContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigEFBphysContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigEFBphysContainer DUMP===========" << endreq;

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
			mLog << MSG::INFO << "REGTEST No TrigEFBphysContainer found with tag " << EFBphysTags[itag] << endreq;
			continue;
		}

		mLog << MSG::INFO << "REGTEST TrigEFBphysContainer found with tag " << EFBphysTags[itag]
		                                                                                   << " and size " << trigEFBphys->size() << endreq;

		//  for (int i=0; trigEFBphys != lastTrigEFBphys; ++trigEFBphys, ++i) {

		//mLog << MSG::INFO << "REGTEST Looking at TrigEFBphysContainer " << i << endreq;

        xAOD::TrigBphysContainer::const_iterator EFBphysItr  = trigEFBphys->begin();
        xAOD::TrigBphysContainer::const_iterator EFBphysItrE = trigEFBphys->end();

		for (int j=0; EFBphysItr != EFBphysItrE; ++EFBphysItr, ++j ) {

			mLog <<MSG::INFO << "REGTEST Looking at TrigEFBphys " << j << endreq;

			mLog <<MSG::INFO << "REGTEST TrigEFBphys->eta() returns " << (*EFBphysItr)->eta() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBphys->phi() returns " << (*EFBphysItr)->phi() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBphys->mass() returns " << (*EFBphysItr)->mass() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBphys->fitmass() returns " << (*EFBphysItr)->fitmass() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigEFBphys->isValid() returns " << (*EFBphysItr)->isValid() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBphys->roiId() returns " << (*EFBphysItr)->roiId() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBphys->particleType() returns " << (*EFBphysItr)->particleType() << endreq;

			if( (*EFBphysItr)->secondaryDecay() != NULL){
                const xAOD::TrigBphys * psecond =(*EFBphysItr)->secondaryDecay();
				mLog <<MSG::INFO << "REGTEST Secondary decay info: " << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->eta() returns " << psecond->eta() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->phi() returns " << psecond->phi() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->mass() returns " << psecond->mass() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->fitmass() returns " << psecond->fitmass() << endreq;
				//	mLog <<MSG::INFO << "REGTEST pSecondDecay->isValid() returns " << (*EFBphysItr)->secondaryDecayLink()->isValid() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->roiId() returns " << psecond->roiId() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->particleType() returns " << psecond->particleType() << endreq;

			} // end if secondary exists



			const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*EFBphysItr)->trackParticleLinks();
 			if (trackVector.size() != 0) {
				mLog << MSG::INFO << " REGTEST got track vector size: " << trackVector.size() << endreq;
			} else {
				mLog << MSG::INFO << " REGTEST no track vector!!! "  << endreq;
			}
			std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
			for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
			  if (!(trkIt->isValid())) {
			    mLog << MSG::WARNING << "TrackParticleContainer::Invalid ElementLink to track " << endreq;
			    continue;
			  }
				//const Trk::Perigee* trackPerigee=(*(*trkIt))->measuredPerigee();
                const Trk::Perigee* trackPerigee=&((*(*trkIt))->perigeeParameters());

				//      msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
				double phi = trackPerigee->parameters()[Trk::phi];
				double theta = trackPerigee->parameters()[Trk::theta];
				double px = trackPerigee->momentum()[Trk::px];
				double py = trackPerigee->momentum()[Trk::py];
				double pt = sqrt(px*px + py*py);
				double eta = -std::log(tan(theta/2));

				mLog << MSG::INFO << "track " << itrk << " pt phi eta " << pt << " " <<
				phi << " " << eta << endreq;
			}

		}
	}
	mLog <<MSG::INFO << "REGTEST ==========END of TrigEFBphysContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigEFBphysContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
StatusCode TrigEDMChecker::dumpTrigL2BphysContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigL2BphysContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigL2BphysContainer DUMP===========" << endreq;


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
			mLog << MSG::INFO << "REGTEST No TrigL2BphysContainer found with tag " << L2BphysTags[itag] << endreq;
			continue;
		}

		mLog << MSG::INFO << "REGTEST TrigL2BphysContainer found with tag " << L2BphysTags[itag]
		                  << " and size " << trigL2Bphys->size() << endreq;

		//  for (int i=0; trigL2Bphys != lastTrigL2Bphys; ++trigL2Bphys, ++i) {

		// mLog << MSG::INFO << "REGTEST Looking at TrigL2BphysContainer " << i << endreq;

		xAOD::TrigBphysContainer::const_iterator L2BphysItr  = trigL2Bphys->begin();
		xAOD::TrigBphysContainer::const_iterator L2BphysItrE = trigL2Bphys->end();

		for (int j=0; L2BphysItr != L2BphysItrE; ++L2BphysItr, ++j ) {

			mLog <<MSG::INFO << "REGTEST Looking at TrigL2Bphys " << j << endreq;

			mLog <<MSG::INFO << "REGTEST TrigL2Bphys->eta() returns " << (*L2BphysItr)->eta() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bphys->phi() returns " << (*L2BphysItr)->phi() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bphys->mass() returns " << (*L2BphysItr)->mass() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bphys->fitmass() returns " << (*L2BphysItr)->fitmass() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigL2Bphys->isValid() returns " << (*L2BphysItr)->isValid() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bphys->roiId() returns " << (*L2BphysItr)->roiId() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bphys->particleType() returns " << (*L2BphysItr)->particleType() << endreq;

			if( (*L2BphysItr)->secondaryDecay() != NULL){
                const xAOD::TrigBphys * psecond =(*L2BphysItr)->secondaryDecay();
				mLog <<MSG::INFO << "REGTEST Secondary decay info: " << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->eta() returns " << psecond->eta() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->phi() returns " << psecond->phi() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->mass() returns " << psecond->mass() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->fitmass() returns " << psecond->fitmass() << endreq;
				//	mLog <<MSG::INFO << "REGTEST pSecondDecay->isValid() returns " << (*L2BphysItr)->secondaryDecayLink()->isValid() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->roiId() returns " << psecond->roiId() << endreq;
				mLog <<MSG::INFO << "REGTEST pSecondDecay->particleType() returns " << psecond->particleType() << endreq;

			} // end if secondary exists



			const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*L2BphysItr)->trackParticleLinks();
 			if (trackVector.size() != 0) {
				mLog << MSG::INFO << " REGTEST got track vector size: " << trackVector.size() << endreq;
			} else {
				mLog << MSG::INFO << " REGTEST no track vector!!! "  << endreq;
			}
			std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
			for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
			  if (!(trkIt->isValid())) {
			    mLog << MSG::WARNING << "TrackParticleContainer::Invalid ElementLink to track " << endreq;
			    continue;
			  }
				//const Trk::Perigee* trackPerigee=(*(*trkIt))->measuredPerigee();
                const Trk::Perigee* trackPerigee=&((*(*trkIt))->perigeeParameters());

				//      msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
				double phi = trackPerigee->parameters()[Trk::phi];
				double theta = trackPerigee->parameters()[Trk::theta];
				double px = trackPerigee->momentum()[Trk::px];
				double py = trackPerigee->momentum()[Trk::py];
				double pt = sqrt(px*px + py*py);
				double eta = -std::log(tan(theta/2));

				mLog << MSG::INFO << "track " << itrk << " pt phi eta " << pt << " " <<
				phi << " " << eta << endreq;
			}

		}
	}



	mLog <<MSG::INFO << "REGTEST ==========END of TrigL2BphysContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigL2BphysContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;
}



//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpxAODJetContainer() {
    MsgStream mLog( messageService(), name() );
    
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
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigEFBjetContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigEFBjetContainer DUMP===========" << endreq;

	const DataHandle< TrigEFBjetContainer > trigEFBjet;
	const DataHandle< TrigEFBjetContainer > lastTrigEFBjet;

	StatusCode sc = evtStore()->retrieve(trigEFBjet,lastTrigEFBjet);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigEFBjetContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigEFBjetContainers retrieved" << endreq;


	for (int i=0; trigEFBjet != lastTrigEFBjet; ++trigEFBjet, ++i) {

		mLog << MSG::INFO << "REGTEST Looking at TrigEFBjetContainer " << i << endreq;

		TrigEFBjetContainer::const_iterator EFBjetItr  = trigEFBjet->begin();
		TrigEFBjetContainer::const_iterator EFBjetItrE = trigEFBjet->end();

		for (int j=0; EFBjetItr != EFBjetItrE; ++EFBjetItr, ++j ) {

			mLog <<MSG::INFO << "REGTEST Looking at TrigEFBjet " << j << endreq;

			mLog <<MSG::INFO << "REGTEST TrigEFBjet->prmVtx() returns " << (*EFBjetItr)->prmVtx() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->xComb() returns " << (*EFBjetItr)->xComb() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigEFBjet->x2D() returns " << (*EFBjetItr)->x2D() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->xIP1D() returns " << (*EFBjetItr)->xIP1D() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->isValid() returns " << (*EFBjetItr)->isValid() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->roiId() returns " << (*EFBjetItr)->roiId() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigEFBjet->xD0() returns " << (*EFBjetItr)->xD0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->xIP2D() returns " << (*EFBjetItr)->xIP2D() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigEFBjet->xZ0() returns " << (*EFBjetItr)->xZ0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->xIP3D() returns " << (*EFBjetItr)->xIP3D() << endreq;
			// new one:
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->xCHI2() returns " << (*EFBjetItr)->xCHI2() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigEFBjet->x3D() returns " << (*EFBjetItr)->x3D() << endreq; // replaced by :
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->xSV() returns " << (*EFBjetItr)->xSV() << endreq;
			//
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->xMVtx() returns " << (*EFBjetItr)->xMVtx() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBjet->xEVtx() returns " << (*EFBjetItr)->xEVtx() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigEFBjet-> xNVtx() returns " << (*EFBjetItr)-> xNVtx() << endreq;
		}
	}
	mLog <<MSG::INFO << "REGTEST ==========END of TrigEFBjetContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigEFBjetContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigL2BjetContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigL2BjetContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigL2BjetContainer DUMP===========" << endreq;

	const DataHandle< TrigL2BjetContainer > trigL2Bjet;
	const DataHandle< TrigL2BjetContainer > lastTrigL2Bjet;

	StatusCode sc = evtStore()->retrieve(trigL2Bjet,lastTrigL2Bjet);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigL2BjetContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigL2BjetContainers retrieved" << endreq;


	for (int i=0; trigL2Bjet != lastTrigL2Bjet; ++trigL2Bjet, ++i) {

		mLog << MSG::INFO << "REGTEST Looking at TrigL2BjetContainer " << i << endreq;

		TrigL2BjetContainer::const_iterator L2BjetItr  = trigL2Bjet->begin();
		TrigL2BjetContainer::const_iterator L2BjetItrE = trigL2Bjet->end();

		for (int j=0; L2BjetItr != L2BjetItrE; ++L2BjetItr, ++j ) {

			mLog <<MSG::INFO << "REGTEST Looking at TrigL2Bjet " << j << endreq;

			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->prmVtx() returns " << (*L2BjetItr)->prmVtx() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xComb() returns " << (*L2BjetItr)->xComb() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigL2Bjet->x2D() returns " << (*L2BjetItr)->x2D() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xIP1D() returns " << (*L2BjetItr)->xIP1D() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->isValid() returns " << (*L2BjetItr)->isValid() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->roiId() returns " << (*L2BjetItr)->roiId() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xD0() returns " << (*L2BjetItr)->xD0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xIP2D() returns " << (*L2BjetItr)->xIP2D() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xZ0() returns " << (*L2BjetItr)->xZ0() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xIP3D() returns " << (*L2BjetItr)->xIP3D() << endreq;
			// new one:
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xCHI2() returns " << (*L2BjetItr)->xCHI2() << endreq;
			//      mLog <<MSG::INFO << "REGTEST TrigL2Bjet->x3D() returns " << (*L2BjetItr)->x3D() << endreq; // replaced by :
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xSV() returns " << (*L2BjetItr)->xSV() << endreq;
			//
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xMVtx() returns " << (*L2BjetItr)->xMVtx() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet->xEVtx() returns " << (*L2BjetItr)->xEVtx() << endreq;
			mLog <<MSG::INFO << "REGTEST TrigL2Bjet-> xNVtx() returns " << (*L2BjetItr)-> xNVtx() << endreq;
		}
	}
	mLog <<MSG::INFO << "REGTEST ==========END of TrigL2BjetContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigL2BjetContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpMuonFeature() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpMuonFeature()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of MuonFeature DUMP===========" << endreq;

	const DataHandle< MuonFeature > MuFeature;
	const DataHandle< MuonFeature > lastMuFeature;

	StatusCode sc = evtStore()->retrieve(MuFeature,lastMuFeature);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No MuonFeature found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST MuonFeature retrieved" << endreq;


	for (int i=0; MuFeature != lastMuFeature; ++MuFeature, ++i) {

		const MuonFeature* thisMuFeature = &(*MuFeature);

		mLog <<MSG::INFO << "REGTEST Looking at MuonFeature " << i << endreq;

		mLog <<MSG::INFO << "REGTEST MuonFeature->roiId() returns " << thisMuFeature->roiId() << endreq;
		mLog <<MSG::INFO << "REGTEST MuonFeature->eta() returns " << thisMuFeature->eta() << endreq;
		mLog <<MSG::INFO << "REGTEST MuonFeature->phi() returns " << thisMuFeature->phi() << endreq;

		mLog <<MSG::INFO << "REGTEST MuonFeature->saddress() returns " << thisMuFeature->saddress() << endreq;
		mLog <<MSG::INFO << "REGTEST MuonFeature->pt() returns " << thisMuFeature->pt() << endreq;
		mLog <<MSG::INFO << "REGTEST MuonFeature->radius() returns " << thisMuFeature->radius() << endreq;
		mLog <<MSG::INFO << "REGTEST MuonFeature->dir_phi() returns " << thisMuFeature->dir_phi() << endreq;
		mLog <<MSG::INFO << "REGTEST MuonFeature->zeta() returns " << thisMuFeature->zeta() << endreq;
		mLog <<MSG::INFO << "REGTEST MuonFeature->dir_zeta() returns " << thisMuFeature->dir_zeta() << endreq;
		mLog <<MSG::INFO << "REGTEST MuonFeature->beta() returns " << thisMuFeature->beta() << endreq;
	}

	mLog <<MSG::INFO << "REGTEST ==========END of MuonFeature DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpMuonFeature() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpCombinedMuonFeature() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpCombinedMuonFeature()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of CombinedMuonFeature DUMP===========" << endreq;

	const DataHandle< CombinedMuonFeature > CombMuon;
	const DataHandle< CombinedMuonFeature > lastCombMuon;

	StatusCode sc = evtStore()->retrieve(CombMuon,lastCombMuon);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No CombinedMuonFeature found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST CombinedMuonFeatures retrieved" << endreq;

	for (int i=0; CombMuon != lastCombMuon; ++CombMuon, ++i) {

		const CombinedMuonFeature* thisCombMuFeature = &(*CombMuon);

		mLog << MSG::INFO << "REGTEST Looking at CombinedMuonFeature " << i << endreq;

		mLog <<MSG::INFO << "REGTEST TrigPhoton->pt() returns " << (thisCombMuFeature)->pt() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigPhoton->sigma_pt() returns " << (thisCombMuFeature)->sigma_pt() << endreq;

		mLog <<MSG::INFO << "REGTEST MuonFeature info: " << endreq;
		//      if ((thisCombMuFeature)->muFastTrack()) {
		if ((thisCombMuFeature)->muFastTrackLink().isValid() ) {
			mLog <<MSG::INFO << "REGTEST muFastTrack->eta() returns " << (thisCombMuFeature)->muFastTrack()->eta() << endreq;
			mLog <<MSG::INFO << "REGTEST muFastTrack->phi() returns " << (thisCombMuFeature)->muFastTrack()->phi() << endreq;
			mLog <<MSG::INFO << "REGTEST muFastTrack->radius() returns " << (thisCombMuFeature)->muFastTrack()->radius() << endreq;
			mLog <<MSG::INFO << "REGTEST muFastTrack->zeta() returns " << (thisCombMuFeature)->muFastTrack()->zeta() << endreq;
		} else {
			mLog <<MSG::INFO << "CombinedMuonFeature has no muFastTrack"  << endreq;
		}

		mLog <<MSG::INFO << "REGTEST TrigInDetTrack info: " << endreq;
		//      if ((thisCombMuFeature)->IDTrack()) {
		if ((thisCombMuFeature)->IDTrackLink().isValid() ) {
			mLog <<MSG::INFO << "REGTEST IDTrack->algorithmId() returns " <<(thisCombMuFeature)->IDTrack()->algorithmId() << endreq;
			mLog <<MSG::INFO << "REGTEST IDTrack->chi2() returns " << (thisCombMuFeature)->IDTrack()->chi2() << endreq;
			mLog <<MSG::INFO << "REGTEST IDTrack->NStrawHits() returns " <<(thisCombMuFeature)->IDTrack()->NStrawHits() << endreq;

			mLog <<MSG::INFO << "REGTEST IDTrack->NStraw() returns " << (thisCombMuFeature)->IDTrack()->NStraw() << endreq;
			mLog <<MSG::INFO << "REGTEST IDTrack->NStrawTime() returns " <<(thisCombMuFeature)->IDTrack()->NStrawTime() << endreq;
			mLog <<MSG::INFO << "REGTEST IDTrack->NTRHits() returns " <<(thisCombMuFeature)->IDTrack()->NTRHits() << endreq;
		} else {
			mLog <<MSG::INFO << "CombinedMuonFeature has no IDTrack"  << endreq;
		}

	}

	mLog <<MSG::INFO << "REGTEST ==========END of CombinedMuonFeature DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpCombinedMuonFeature() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpCombinedMuonFeatureContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpCombinedMuonFeatureContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of CombinedMuonFeatureContainer DUMP===========" << endreq;

	const DataHandle< CombinedMuonFeatureContainer > CombMuon;
	const DataHandle< CombinedMuonFeatureContainer > lastCombMuon;

	StatusCode sc = evtStore()->retrieve(CombMuon,lastCombMuon);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No CombinedMuonFeatureContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST CombinedMuonFeaturesContainer retrieved" << endreq;

	for (int j=0; CombMuon != lastCombMuon; ++CombMuon, ++j) {
		mLog << MSG::INFO << "REGTEST Looking at CombinedMuonFeatureContainer " << j << endreq;

		const CombinedMuonFeatureContainer* container = &(*CombMuon);
		CombinedMuonFeatureContainer::const_iterator muon;

		int i = 0;
		for ( muon = container->begin() ; muon != container->end(); ++i, ++muon ) {
			const CombinedMuonFeature* thisCombMuFeature = *muon;
			mLog << MSG::INFO << "REGTEST Looking at CombinedMuonFeature " << i << endreq;

			mLog <<MSG::INFO << "REGTEST CombinedMuonFeature->pt() returns " << (thisCombMuFeature)->pt() << endreq;
			mLog <<MSG::INFO << "REGTEST CombinedMuonFeature->sigma_pt() returns " << (thisCombMuFeature)->sigma_pt() << endreq;

			mLog <<MSG::INFO << "REGTEST MuonFeature info: " << endreq;
			if ( thisCombMuFeature->muFastTrackLink().isValid() ) {
				mLog <<MSG::INFO << "REGTEST muFastTrack->eta() returns " << (thisCombMuFeature)->muFastTrack()->eta() << endreq;
				mLog <<MSG::INFO << "REGTEST muFastTrack->phi() returns " << (thisCombMuFeature)->muFastTrack()->phi() << endreq;
				mLog <<MSG::INFO << "REGTEST muFastTrack->radius() returns " << (thisCombMuFeature)->muFastTrack()->radius() << endreq;
				mLog <<MSG::INFO << "REGTEST muFastTrack->zeta() returns " << (thisCombMuFeature)->muFastTrack()->zeta() << endreq;
			} else {
				mLog <<MSG::WARNING << "MuonFeature has no muFastTrack!"  << endreq;
			}

			mLog <<MSG::INFO << "REGTEST TrigInDetTrack info: " << endreq;
			if ( thisCombMuFeature->IDTrackLink().isValid() ) {
				mLog <<MSG::INFO << "REGTEST IDTrack->algorithmId() returns " <<(thisCombMuFeature)->IDTrack()->algorithmId() << endreq;
				mLog <<MSG::INFO << "REGTEST IDTrack->chi2() returns " << (thisCombMuFeature)->IDTrack()->chi2() << endreq;
				mLog <<MSG::INFO << "REGTEST IDTrack->NStrawHits() returns " <<(thisCombMuFeature)->IDTrack()->NStrawHits() << endreq;

				mLog <<MSG::INFO << "REGTEST IDTrack->NStraw() returns " << (thisCombMuFeature)->IDTrack()->NStraw() << endreq;
				mLog <<MSG::INFO << "REGTEST IDTrack->NStrawTime() returns " <<(thisCombMuFeature)->IDTrack()->NStrawTime() << endreq;
				mLog <<MSG::INFO << "REGTEST IDTrack->NTRHits() returns " <<(thisCombMuFeature)->IDTrack()->NTRHits() << endreq;
			} else {
				mLog <<MSG::WARNING << "MuonFeature has no IDTrack!"  << endreq;
			}

		}
	}
	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigEMCluster() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigEMCluster()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigEMCluster DUMP===========" << endreq;

	const DataHandle< TrigEMCluster > EMCluster;
	const DataHandle< TrigEMCluster > lastEMCluster;

	StatusCode sc = evtStore()->retrieve(EMCluster,lastEMCluster);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigEMCluster found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigEMCluster retrieved" << endreq;


	for (int i=0; EMCluster != lastEMCluster; ++EMCluster, ++i) {

		const TrigEMCluster* thisEMCluster = &(*EMCluster);

		mLog <<MSG::INFO << "REGTEST Looking at TrigEMCluster " << i << endreq;

		mLog <<MSG::INFO << "REGTEST TrigEMCluster->energy() returns " << thisEMCluster->energy() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigEMCluster->e() returns " << thisEMCluster->e() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigEMCluster->phi() returns " << thisEMCluster->phi() << endreq;

		mLog <<MSG::INFO << "REGTEST TrigEMCluster->eta() returns " << thisEMCluster->eta() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigEMCluster->e237() returns " << thisEMCluster->e237() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigEMCluster->e277() returns " << thisEMCluster->e277() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigEMCluster->fracs1() returns " << thisEMCluster->fracs1() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigEMCluster->weta2() returns " << thisEMCluster->weta2() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigEMCluster->ehad1() returns " << thisEMCluster->ehad1() << endreq;
		mLog <<MSG::INFO << "REGTEST TrigEMCluster->Eta1() returns " << thisEMCluster->Eta1() << endreq;
	}

	mLog <<MSG::INFO << "REGTEST ==========END of TrigEMCluster DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigEMCluster() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpxAODTrigEMCluster() {
        MsgStream mLog( messageService(), name() );

        mLog << MSG::DEBUG << "in dumpxAODTrigEMCluster()" << endreq;

        mLog <<MSG::INFO << "REGTEST ==========START of TrigEMCluster DUMP===========" << endreq;

        const DataHandle< xAOD::TrigEMCluster > EMCluster;
        const DataHandle< xAOD::TrigEMCluster > lastEMCluster;

        StatusCode sc = evtStore()->retrieve(EMCluster,lastEMCluster);
        if (sc.isFailure()) {
                mLog << MSG::INFO << "REGTEST No xAOD::TrigEMCluster found" << endreq;
                return  StatusCode::SUCCESS;
        }
        mLog << MSG::INFO << "REGTEST xAOD::TrigEMCluster retrieved" << endreq;


        for (int i=0; EMCluster != lastEMCluster; ++EMCluster, ++i) {

                const xAOD::TrigEMCluster* thisEMCluster = &(*EMCluster);

                mLog <<MSG::INFO << "REGTEST Looking at xAOD::TrigEMCluster " << i << endreq;

                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->energy() returns " << thisEMCluster->energy() << endreq;
                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->e() returns " << thisEMCluster->energy() << endreq;
                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->phi() returns " << thisEMCluster->phi() << endreq;

                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->eta() returns " << thisEMCluster->eta() << endreq;
                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->e237() returns " << thisEMCluster->e237() << endreq;
                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->e277() returns " << thisEMCluster->e277() << endreq;
                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->fracs1() returns " << thisEMCluster->fracs1() << endreq;
                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->weta2() returns " << thisEMCluster->weta2() << endreq;
                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->ehad1() returns " << thisEMCluster->ehad1() << endreq;
                mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->Eta1() returns " << thisEMCluster->eta1() << endreq;
        }

        mLog <<MSG::INFO << "REGTEST ==========END of xAOD::TrigEMCluster DUMP===========" << endreq;

        mLog << MSG::DEBUG << "dumpxAODTrigEMCluster() succeeded" << endreq;

        return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigTauClusterContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTrigTauClusterContainer()" << endreq;

	mLog <<MSG::INFO << "REGTEST ==========START of TrigTauClusterContainer/TrigTauClusterDetailsContainer DUMP===========" << endreq;

	const DataHandle< TrigTauClusterContainer > TauCluster;
	const DataHandle< TrigTauClusterContainer > lastTauCluster;

	StatusCode sc = evtStore()->retrieve(TauCluster,lastTauCluster);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TrigTauClusterContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigTauClusterContainer retrieved" << endreq;


	int nClusters = 0;
	for (int i=0; TauCluster != lastTauCluster; ++TauCluster, ++i) {

                mLog << MSG::INFO << "REGTEST Looking at TrigTauClusterContainer " << i << endreq;

                TrigTauClusterContainer::const_iterator TauClusterItr  = TauCluster->begin();
                TrigTauClusterContainer::const_iterator TauClusterItrE = TauCluster->end();

                for (int j=0; TauClusterItr != TauClusterItrE; ++TauClusterItr, ++j ) {
		  nClusters++;
                  mLog <<MSG::INFO << "REGTEST Looking at TrigTauCluster " << j << endreq;

		  mLog <<MSG::INFO << "REGTEST TrigTauCluster->energy() returns " << (*TauClusterItr)->energy() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTauCluster->et() returns " << (*TauClusterItr)->et() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTauCluster->EMCalibEnergy() returns " << (*TauClusterItr)->EMCalibEnergy() << endreq;

		  mLog <<MSG::INFO << "REGTEST TrigTauCluster->EMenergy() returns " << (*TauClusterItr)->EMenergy() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTauCluster->HADenergy() returns " << (*TauClusterItr)->HADenergy() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTauCluster->eta() returns " << (*TauClusterItr)->eta() << endreq;
	   	  mLog <<MSG::INFO << "REGTEST TrigTauCluster->phi() returns " << (*TauClusterItr)->phi() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTauCluster->IsoFrac() returns " << (*TauClusterItr)->IsoFrac() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTauCluster->stripWidth() returns " << (*TauClusterItr)->stripWidth() << endreq;
		  mLog <<MSG::INFO << "REGTEST TrigTauCluster->numStripCells() returns " << (*TauClusterItr)->numStripCells() << endreq;
                  mLog <<MSG::INFO << "REGTEST TrigTauCluster->CaloRadius() returns " << (*TauClusterItr)->CaloRadius() << endreq;
                  mLog <<MSG::INFO << "REGTEST TrigTauCluster->numTotCells() returns " << (*TauClusterItr)->numTotCells() << endreq;


		  if( (*TauClusterItr)->clusterDetails() == 0 )
		    mLog <<MSG::WARNING << "REGTEST TrigTauCluster-> Details link is MISSING " << endreq;
		  else{
		    mLog <<MSG::INFO << "REGTEST TrigTauCluster->Energy in Narrow cone : EM[0/1/2/3]="
			 <<(*TauClusterItr)->EMenergyNarrow(0) << " " << (*TauClusterItr)->EMenergyNarrow(1) << " "
			 <<(*TauClusterItr)->EMenergyNarrow(2) << " " << (*TauClusterItr)->EMenergyNarrow(3) <<endreq;
		    mLog <<MSG::INFO << "REGTEST TrigTauCluster->Energy in Narrow cone : HAD[0/1/2]="
			 <<(*TauClusterItr)->HADenergyNarrow(0) << " " << (*TauClusterItr)->HADenergyNarrow(1) << " "
			 <<(*TauClusterItr)->HADenergyNarrow(2) <<endreq;
		  }
                }
	}

	int nDetails = 0;
	const DataHandle< TrigTauClusterDetailsContainer > TauDetailsCluster;
	const DataHandle< TrigTauClusterDetailsContainer > lastTauDetailsCluster;
	sc = evtStore()->retrieve(TauDetailsCluster,lastTauDetailsCluster);
	if (sc.isFailure()) {
	        mLog << MSG::INFO << "REGTEST No TrigTauDetailsClusterContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TrigTauDetailsClusterContainer retrieved" << endreq;

	for (int i=0; TauDetailsCluster != lastTauDetailsCluster; ++TauDetailsCluster, ++i) {

                mLog << MSG::INFO << "REGTEST Looking at TrigTauClusterDetailsContainer " << i << endreq;

                TrigTauClusterDetailsContainer::const_iterator TauDetailsClusterItr  = TauDetailsCluster->begin();
                TrigTauClusterDetailsContainer::const_iterator TauDetailsClusterItrE = TauDetailsCluster->end();

                for (int j=0; TauDetailsClusterItr != TauDetailsClusterItrE; ++TauDetailsClusterItr, ++j ) {
		  nDetails++;
                  mLog <<MSG::INFO << "REGTEST Looking at TrigTauClusterDetails " << j << endreq;

		  for(unsigned int i = 0; i<4; ++i )
		    {
		      mLog <<MSG::INFO << "REGTEST TrigTauClusterDetails-> EM["<<i <<"] Radius/EnergyNar/EnergyMid/EnergyWid returns "
			   << (*TauDetailsClusterItr)->EMRadius(i) << " "
			   << (*TauDetailsClusterItr)->EMenergyNarrow(i) << " "
			   << (*TauDetailsClusterItr)->EMenergyMedium(i) << " "
			   << (*TauDetailsClusterItr)->EMenergyWide(i) << endreq;
		    }
		  for(unsigned int i = 0; i<3; ++i )
		    {
		      mLog <<MSG::INFO << "REGTEST TrigTauClusterDetails-> HAD["<<i <<"] Radius/EnergyNar/EnergyMid/EnergyWid returns "
			   << (*TauDetailsClusterItr)->HADRadius(i) << " "
			   << (*TauDetailsClusterItr)->HADenergyNarrow(i) << " "
			   << (*TauDetailsClusterItr)->HADenergyMedium(i) << " "
			   << (*TauDetailsClusterItr)->HADenergyWide(i) << endreq;
		    }

		}
	}

	if( nDetails != nClusters)
	  mLog <<MSG::WARNING << "REGTEST inconsistent number of TrigTauClusters ("<< nClusters<< ") and TrigTauClusterDetails ("
	       << nDetails << ")" << endreq;



	mLog <<MSG::INFO << "REGTEST ==========END of TrigTauClusterContainer/TrigTauClusterDetailsContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTrigTauClusterContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTrigEMClusterContainer() {
        MsgStream mLog( messageService(), name() );

        mLog << MSG::DEBUG << "in dumpTrigEMClusterContainer()" << endreq;

        mLog <<MSG::INFO << "REGTEST ==========START of TrigEMClusterContainer DUMP===========" << endreq;

        const DataHandle< TrigEMClusterContainer > EMCluster;
        const DataHandle< TrigEMClusterContainer > lastEMCluster;

        StatusCode sc = evtStore()->retrieve(EMCluster,lastEMCluster);
        if (sc.isFailure()) {
                mLog << MSG::INFO << "REGTEST No TrigEMClusterContainer found" << endreq;
                return  StatusCode::SUCCESS;
        }
        mLog << MSG::INFO << "REGTEST TrigEMClusterContainer retrieved" << endreq;


        for (int i=0; EMCluster != lastEMCluster; ++EMCluster, ++i) {

                mLog << MSG::INFO << "REGTEST Looking at TrigEMClusterContainer " << i << endreq;

                TrigEMClusterContainer::const_iterator EMClusterItr  = EMCluster->begin();
                TrigEMClusterContainer::const_iterator EMClusterItrE = EMCluster->end();

                for (int j=0; EMClusterItr != EMClusterItrE; ++EMClusterItr, ++j ) {

                  mLog <<MSG::INFO << "REGTEST Looking at TrigEMCluster " << j << endreq;

                  mLog <<MSG::INFO << "REGTEST TrigEMCluster->energy() returns " << (*EMClusterItr)->energy() << endreq;
                  mLog <<MSG::INFO << "REGTEST TrigEMCluster->et() returns " << (*EMClusterItr)->et() << endreq;
                  mLog <<MSG::INFO << "REGTEST TrigEMCluster->eta() returns " << (*EMClusterItr)->eta() << endreq;
                  mLog <<MSG::INFO << "REGTEST TrigEMCluster->phi() returns " << (*EMClusterItr)->phi() << endreq;
                  mLog <<MSG::INFO << "REGTEST TrigEMCluster->print() gives" << endreq;
		  int level = mLog.level();
		  // little trick to print out stuff
		  mLog.setLevel(MSG::DEBUG);
		  (*EMClusterItr)->print(mLog);
		  mLog.setLevel(level);
                }
        }

        mLog <<MSG::INFO << "REGTEST ==========END of TrigEMClusterContainer DUMP===========" << endreq;

        mLog << MSG::DEBUG << "dumpTrigEMClusterContainer() succeeded" << endreq;

        return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpxAODTrigEMClusterContainer() {
        MsgStream mLog( messageService(), name() );

        mLog << MSG::DEBUG << "in dumpxAODTrigEMClusterContainer()" << endreq;

        mLog <<MSG::INFO << "REGTEST ==========START of xAODTrigEMClusterContainer DUMP===========" << endreq;

        const DataHandle< xAOD::TrigEMClusterContainer > EMCluster;
        const DataHandle< xAOD::TrigEMClusterContainer > lastEMCluster;

        StatusCode sc = evtStore()->retrieve(EMCluster,lastEMCluster);
        if (sc.isFailure()) {
                mLog << MSG::INFO << "REGTEST No xAOD::TrigEMClusterContainer found" << endreq;
                return  StatusCode::SUCCESS;
        }
        mLog << MSG::INFO << "REGTEST xAOD::TrigEMClusterContainer retrieved" << endreq;


        for (int i=0; EMCluster != lastEMCluster; ++EMCluster, ++i) {

                mLog << MSG::INFO << "REGTEST Looking at xAOD::TrigEMClusterContainer " << i << endreq;

                xAOD::TrigEMClusterContainer::const_iterator EMClusterItr  = EMCluster->begin();
                xAOD::TrigEMClusterContainer::const_iterator EMClusterItrE = EMCluster->end();

                for (int j=0; EMClusterItr != EMClusterItrE; ++EMClusterItr, ++j ) {

                  mLog <<MSG::INFO << "REGTEST Looking at xAOD::TrigEMCluster " << j << endreq;

                  mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->energy() returns " << (*EMClusterItr)->energy() << endreq;
                  mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->et() returns " << (*EMClusterItr)->et() << endreq;
                  mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->eta() returns " << (*EMClusterItr)->eta() << endreq;
                  mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->phi() returns " << (*EMClusterItr)->phi() << endreq;
                  //mLog <<MSG::INFO << "REGTEST xAOD::TrigEMCluster->print() gives" << endreq;
                  //int level = mLog.level();
                  // little trick to print out stuff
                  //mLog.setLevel(MSG::DEBUG);
                  //(*EMClusterItr)->print(mLog);
                  //mLog.setLevel(level);
                }
          }

          mLog <<MSG::INFO << "REGTEST ==========END of TrigEMClusterContainer DUMP===========" << endreq;

          mLog << MSG::DEBUG << "dumpTrigEMClusterContainer() succeeded" << endreq;

          return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTileMuFeatureContainer() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "in dumpTileMuFeatureContainer()" << endreq;
	mLog <<MSG::INFO << "REGTEST ==========START of TileMuFeatureContainer DUMP===========" << endreq;

	const DataHandle< TileMuFeatureContainer > TileMu;
	const DataHandle< TileMuFeatureContainer > lastTileMu;

	StatusCode sc = evtStore()->retrieve(TileMu, lastTileMu);
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No TileMuFeatureContainer found" << endreq;
		return  StatusCode::SUCCESS;
	}
	mLog << MSG::INFO << "REGTEST TileMuFeatureContainer retrieved" << endreq;

	for (int i=0; TileMu != lastTileMu; ++TileMu, ++i) {
		mLog << MSG::INFO << "REGTEST Looking at TileMuFeatureContainer " << i << endreq;
		TileMuFeatureContainer::const_iterator TileMuItr  = TileMu->begin();
		TileMuFeatureContainer::const_iterator TileMuItrE = TileMu->end();

		for(int j=0; TileMuItr != TileMuItrE; ++TileMuItr, ++j) {
			mLog <<MSG::INFO << "REGTEST Looking at TileMuFeature " << j << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuFeature->eta()          returns " << (*TileMuItr)->eta() << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuFeature->phi()          returns " << (*TileMuItr)->phi() << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuFeature->enedep().at(0) returns " << (*TileMuItr)->enedep().at(0) << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuFeature->enedep().at(1) returns " << (*TileMuItr)->enedep().at(1) << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuFeature->enedep().at(2) returns " << (*TileMuItr)->enedep().at(2) << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuFeature->enedep().at(3) returns " << (*TileMuItr)->enedep().at(3) << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuFeature->quality()      returns " << (*TileMuItr)->quality() << endreq;

		}
	}

	mLog << MSG::INFO  << "REGTEST ==========END of TileMuFeatureContainer DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpTileMuFeatureContainer() succeeded" << endreq;

	return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode TrigEDMChecker::dumpTileTrackMuFeatureContainer() {  MsgStream mLog( messageService(), name() );

mLog << MSG::DEBUG << "in dumpTileTrackMuFeatureContainer()" << endreq;
mLog <<MSG::INFO << "REGTEST ==========START of TileTrackMuFeatureContainer DUMP===========" << endreq;

const DataHandle< TileTrackMuFeatureContainer > TileTrackMu;
const DataHandle< TileTrackMuFeatureContainer > lastTileTrackMu;

StatusCode sc = evtStore()->retrieve(TileTrackMu, lastTileTrackMu);
if (sc.isFailure()) {
	mLog << MSG::INFO << "REGTEST No TileTrackMuFeatureContainer found" << endreq;
	return StatusCode::SUCCESS;
}
mLog << MSG::INFO << "REGTEST TileTrackMuFeatureContainer retrieved" << endreq;

for (int i=0; TileTrackMu!=lastTileTrackMu; ++TileTrackMu, ++i) {
	mLog << MSG::INFO << "REGTEST Looking at TileTrackMuFeatureContainer " << i << endreq;

	TileTrackMuFeatureContainer::const_iterator TileTrackMuItr = TileTrackMu->begin();
	TileTrackMuFeatureContainer::const_iterator TileTrackMuItrE= TileTrackMu->end();

	for (int j=0; TileTrackMuItr != TileTrackMuItrE; ++TileTrackMuItr, ++j) {
		mLog <<MSG::INFO << "REGTEST Looking at TileTrackMuFeature " << j << endreq;
		mLog <<MSG::INFO << "REGTEST TileTrackMuFeature->PtTR_Trk()  returns " << (*TileTrackMuItr)->PtTR_Trk() << endreq;
		mLog <<MSG::INFO << "REGTEST TileTrackMuFeature->EtaTR_Trk() returns " << (*TileTrackMuItr)->EtaTR_Trk() << endreq;
		mLog <<MSG::INFO << "REGTEST TileTrackMuFeature->PhiTR_Trk() returns " << (*TileTrackMuItr)->PhiTR_Trk() << endreq;
		mLog <<MSG::INFO << "REGTEST TileTrackMuFeature->Typ_IDTrk() returns " << (*TileTrackMuItr)->Typ_IDTrk() << endreq;

		mLog <<MSG::INFO << "REGTEST TileMuFeature info: " << endreq;
		ElementLink<TileMuFeatureContainer> TileMuEL = (*TileTrackMuItr)->TileMuLink();
		const TileMuFeature* TileMu;
		if ( !TileMuEL.isValid() ) {
			mLog <<MSG::INFO << "REGTEST No TileMuFeature (Something Wrong)"
			<< endreq;
			TileMu = 0;
		} else{
			TileMu = *TileMuEL;
		}

		if( TileMu != 0 ){
			mLog <<MSG::INFO << "REGTEST TileMuLink->eta()          returns " << TileMu->eta() << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuLink->phi()          returns " << TileMu->phi() << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuLink->enedep().at(0) returns " << TileMu->enedep().at(0) << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuLink->enedep().at(1) returns " << TileMu->enedep().at(1) << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuLink->enedep().at(2) returns " << TileMu->enedep().at(2) << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuLink->enedep().at(3) returns " << TileMu->enedep().at(3) << endreq;
			mLog <<MSG::INFO << "REGTEST TileMuLink->quality()      returns " << TileMu->quality() << endreq;
		}

		mLog <<MSG::INFO << "REGTEST TrigInDetTrack info: " << endreq;
		ElementLink<TrigInDetTrackCollection> IDScanEL = (*TileTrackMuItr)->IDScanLink();
		const TrigInDetTrack* Track;
		if ( !IDScanEL.isValid() ) {
			mLog <<MSG::INFO << "REGTEST No valid IDtracks" << endreq;
			Track = 0;
		} else{
			Track = *IDScanEL;
		}

		if (Track != 0) {
			mLog <<MSG::INFO << "REGTEST IDScanLink->algorithmId()     returns " << Track->algorithmId() << endreq;
			mLog <<MSG::INFO << "REGTEST IDScanLink->chi2()            returns " << Track->chi2() << endreq;
			mLog <<MSG::INFO << "REGTEST IDScanLink->NStrawHits()      returns " << Track->NStrawHits()<< endreq;
			mLog <<MSG::INFO << "REGTEST IDScanLink->NStraw()          returns " << Track->NStraw() << endreq;
			mLog <<MSG::INFO << "REGTEST IDScanLink->NStrawTime()      returns " <<
			Track->NStrawTime() << endreq;
			mLog <<MSG::INFO << "REGTEST IDScanLink->NTRHits()         returns " << Track->NTRHits() << endreq;
			mLog <<MSG::INFO << "REGTEST IDScanLink->param()->phi0()   returns " << Track->param()->phi0() << endreq;
			mLog <<MSG::INFO << "REGTEST IDScanLink->param()->eta()    returns " << Track->param()->eta() << endreq;
			mLog <<MSG::INFO << "REGTEST IDScanLink->param()->pT()     returns " << Track->param()->pT() << endreq;
		}
	}
}

mLog << MSG::INFO <<  "REGTEST ==========END of TileTrackMuFeatureContainer DUMP===========" << endreq;

mLog << MSG::DEBUG << "dumpTileTrackMuFeatureContainer() succeeded" << endreq;
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


  for(xAOD::TauJetContainer::const_iterator tauIt = TauJetcont->begin(); tauIt != TauJetcont->end();tauIt++){

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
	MsgStream mLog( messageService(), name() );
	//  mLog << MSG::INFO << "REGTEST" << endreq;
	mLog << MSG::INFO << "REGTEST ==========START of TauJetContainer DUMP===========" << endreq;

	using namespace Analysis;

	StatusCode sCode=StatusCode::FAILURE;
	int ntag=1;
	std::string TauContainerTags[]={"HLT_TrigTauRecMerged"};
	for (int itag=0; itag < ntag; itag++) {
	  const TauJetContainer* TauJetcont;
	  sCode=evtStore()->retrieve(TauJetcont , TauContainerTags[itag]);
	  if( sCode.isFailure() ){
	    mLog << MSG::INFO << "Failed to retrieve TauJetContainer  with key " << TauContainerTags[itag] << endreq;
	    continue;
	  }

	  for(Analysis::TauJetContainer::const_iterator tauIt = TauJetcont->begin(); tauIt != TauJetcont->end(); tauIt++){

	    bool IsTaurec = false;
	    bool Is1p3p = false;
	    if ((*tauIt)->hasAuthor( TauJetParameters::tauRec)) {
	      mLog <<MSG::INFO << "Is TauRec Seeded " << endreq;
	      IsTaurec = true;
	    }

	    if ((*tauIt)->hasAuthor( TauJetParameters::tau1P3P)) {
	      mLog <<MSG::INFO << "Is Tau1p3p Seeded " << endreq;
	      Is1p3p = true;
	    }

	    if ((*tauIt)->hasAuthor( TauJetParameters::unknown)) {
	      mLog <<MSG::INFO << "Is unknown seeded " << endreq;
	    }

	    const Analysis::TauCommonDetails*  TauDetails = (*tauIt)->details<const Analysis::TauCommonDetails>();
	    if (TauDetails == NULL) {
	      mLog <<MSG::INFO << " TauDetails == NULL " << endreq;
	      continue;
	    }

	    mLog <<MSG::INFO << "REGTEST (*tauIt)->eta()                    returns " << (*tauIt)->eta() << endreq;
	    mLog <<MSG::INFO << "REGTEST (*tauIt)->phi()                    returns " << (*tauIt)->phi() << endreq;
	    mLog <<MSG::INFO << "REGTEST (*tauIt)->numTrack()               returns " << (*tauIt)->numTrack() << endreq;
	    mLog <<MSG::INFO << "REGTEST TauDetails->nLooseTrk()            returns " << TauDetails->nLooseTrk() << endreq;
	    mLog <<MSG::INFO << "REGTEST TauDetails->leadTrkPt()            returns " << TauDetails->leadTrkPt() << endreq;
	    mLog <<MSG::INFO << "REGTEST TauDetails->leadLooseTrkPt()       returns " << TauDetails->leadLooseTrkPt() << endreq;
	    mLog <<MSG::INFO << "REGTEST TauDetails->ipZ0SinThetaSigLeadTrk() returns " << TauDetails->ipZ0SinThetaSigLeadTrk() << endreq;
	    mLog <<MSG::INFO << "REGTEST TauDetails->ipSigLeadTrk() returns "<<TauDetails->ipSigLeadTrk() << endreq;
	    mLog <<MSG::INFO << "REGTEST TauDetails->ipSigLeadLooseTrk() returns "<<TauDetails->ipSigLeadLooseTrk() << endreq;
	    mLog <<MSG::INFO << "REGTEST TauDetails->trkWidth2() returns "<<TauDetails->trkWidth2()<< endreq;
	    mLog <<MSG::INFO << "REGTEST TauDetails->trFlightPathSig() returns "<< TauDetails->trFlightPathSig() << endreq;


	    if(IsTaurec) {
	      mLog <<MSG::INFO << "Calo seeded" << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_etHadCalib()                returns " << TauDetails->seedCalo_etHadCalib() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_etEMCalib()                 returns " << TauDetails->seedCalo_etEMCalib() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_EMRadius()                  returns " << TauDetails->seedCalo_EMRadius() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_isolFrac()                  returns " << TauDetails->seedCalo_isolFrac() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_centFrac()                  returns " << TauDetails->seedCalo_centFrac() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_stripWidth2()               returns " << TauDetails->seedCalo_stripWidth2() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_nStrip()                    returns " << TauDetails->seedCalo_nStrip() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_etEMAtEMScale()             returns " << TauDetails->seedCalo_etEMAtEMScale() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_etHADAtEMScale()            returns " << TauDetails->seedCalo_etHadAtEMScale() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_hadRadius()                 returns " << TauDetails->seedCalo_hadRadius() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_nIsolLooseTrk()             returns " << TauDetails->seedCalo_nIsolLooseTrk() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_trkAvgDist()                returns " << TauDetails->seedCalo_trkAvgDist() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedCalo_trkRmsDist()                returns " << TauDetails->seedCalo_trkRmsDist() << endreq;
	    }

	    if(Is1p3p) {
	      mLog <<MSG::INFO << "Track seeded" << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_EMRadius()                   returns " << TauDetails->seedTrk_EMRadius() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_isolFrac()                   returns " << TauDetails->seedTrk_isolFrac() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etChrgHadOverSumTrkPt ()     returns " << TauDetails->seedTrk_etChrgHadOverSumTrkPt () << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_isolFracWide()               returns " << TauDetails->seedTrk_isolFracWide() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etHadAtEMScale()             returns " << TauDetails->seedTrk_etHadAtEMScale() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etEMAtEMScale()              returns " << TauDetails->seedTrk_etEMAtEMScale() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etEMCL()                     returns " << TauDetails->seedTrk_etEMCL() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etChrgEM()                   returns " << TauDetails->seedTrk_etChrgEM() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etNeuEM()                    returns " << TauDetails->seedTrk_etNeuEM() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etResNeuEM()                 returns " << TauDetails->seedTrk_etResNeuEM() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_hadLeakEt()                  returns " << TauDetails->seedTrk_hadLeakEt() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_sumEMCellEtOverLeadTrkPt()   returns " << TauDetails->seedTrk_sumEMCellEtOverLeadTrkPt() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_secMaxStripEt()              returns " << TauDetails->seedTrk_secMaxStripEt() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_stripWidth2()                returns " << TauDetails->seedTrk_stripWidth2() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_nStrip()                     returns " << TauDetails->seedTrk_nStrip() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etChrgHad()                  returns " << TauDetails->seedTrk_etChrgHad() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_nOtherCoreTrk()              returns " << TauDetails->seedTrk_nOtherCoreTrk() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_nIsolTrk()                   returns " << TauDetails->seedTrk_nIsolTrk() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etIsolEM()                   returns " << TauDetails->seedTrk_etIsolEM() << endreq;
	      mLog <<MSG::INFO << "REGTEST TauDetails->seedTrk_etIsolHad()                  returns " << TauDetails->seedTrk_etIsolHad() << endreq;
	    }

	    const Analysis::TauJet* tj = *tauIt;
	    if ( tj->clusterLink().isValid() ) {
	      mLog <<MSG::INFO << "REGTEST CaloCluster present" << endreq;
	    } else {
	      mLog <<MSG::INFO << "REGTEST CaloCluster missing" << endreq;
	    }

	    if( TauDetails->looseTrk().size() !=0 ) {
	      mLog <<MSG::INFO << "REGTEST TauDetails->looseTrk() link present " << endreq;
	    } else {
	      mLog <<MSG::INFO << "REGTEST TauDetails->looseTrk() link missing " << endreq;
	    }

	    if ( tj->cellClusterLink().isValid() ) {
	      mLog <<MSG::INFO << "REGTEST CellCaloCluster present" << endreq;
	    } else {
	      mLog <<MSG::INFO << "REGTEST CellCaloCluster missing" << endreq;
	    }

	    if ( tj->jetLink().isValid() ) {
	      mLog <<MSG::INFO << "REGTEST Jet present" << endreq;
	    } else {
	      mLog <<MSG::INFO << "REGTEST Jet missing" << endreq;
	    }

	    if ( tj->jetLink().isValid() ) {
	      mLog <<MSG::INFO << "REGTEST Jet present" << endreq;
	    } else {
	      mLog <<MSG::INFO << "REGTEST Jet missing" << endreq;
	    }

	    mLog <<MSG::INFO << "REGTEST numTrack                returns " << tj->numTrack() << endreq;

	    if ( tj->trackLinkVector().size() != 0 ) {
	      mLog <<MSG::INFO << "REGTEST TrackLinkVector present" << endreq;
	    } else {
	      mLog <<MSG::INFO << "REGTEST TrackkLinkVector missing" << endreq;
	    }

	    mLog <<MSG::INFO << "REGTEST author                  returns " << tj->author() << endreq;
	    mLog <<MSG::INFO << "REGTEST ROIWord                 returns " << tj->author() << endreq;

	  }
	}

	const DataHandle< TauJetContainer > TauJet;
	const DataHandle< TauJetContainer > lastTauJet;
	StatusCode sc = evtStore()->retrieve(TauJet, lastTauJet);
	if (sc.isFailure()) {
	  mLog << MSG::INFO << "REGTEST No TauJetContainer found" << endreq;
	  return StatusCode::FAILURE;
	}
	else if (mLog.level() <= MSG::DEBUG) mLog << MSG::DEBUG << "Found TauJetContainer" << endreq;

	mLog << MSG::INFO << "REGTEST TauJetContainer retrieved" << endreq;

	for ( ; TauJet != lastTauJet ; ++TauJet ) {
	  std::string name(TauJet.key());

	  mLog << "REGTEST TauJetContainer key: " << TauJet.key() << endreq;
	}

	return StatusCode::SUCCESS;
}

StatusCode TrigEDMChecker::dumpxAODTrackParticle() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "In dumpxAODTrackParticle()" << endreq;

	mLog << MSG::INFO << "REGTEST ==========START of xAOD::TrackParticle DUMP===========" << endreq;

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
			mLog << MSG::INFO << "REGTEST No track particle container found with key " << SGkeys.at(SGkey) << endreq;
			continue;
		}
		mLog << MSG::INFO << "REGTEST TrackParticleContainer retrieved with key " << SGkeys.at(SGkey)
		     << " and size " << trackParticleContainer->size() << endreq;

		xAOD::TrackParticleContainer::const_iterator trackParticleItr = trackParticleContainer->begin();
		xAOD::TrackParticleContainer::const_iterator trackParticleLast = trackParticleContainer->end();

		for (int index = 0; trackParticleItr != trackParticleLast; ++trackParticleItr, ++index) {
			mLog << MSG::INFO << "REGTEST Looking at Track Particle " << index << endreq;

			mLog << MSG::INFO << "REGTEST IParticle functions:" << endreq;
			mLog << MSG::INFO << "REGTEST pt: " << (*trackParticleItr)->pt();
			mLog << "/eta: " << (*trackParticleItr)->eta();
			mLog << "/phi: " << (*trackParticleItr)->phi();
			mLog << "/m: " << (*trackParticleItr)->m();
			mLog << "/e: " << (*trackParticleItr)->e();
			mLog << "/rapidity: " << (*trackParticleItr)->rapidity() << endreq;

			mLog << MSG::INFO << "REGTEST Defining parameters functions:" << endreq;
			mLog << MSG::INFO << "REGTEST charge: " << (*trackParticleItr)->charge();
			mLog << "/d0: " << (*trackParticleItr)->d0();
			mLog << "/z0: " << (*trackParticleItr)->z0();
			mLog << "/phi0: " << (*trackParticleItr)->phi0();
			mLog << "/theta: " << (*trackParticleItr)->theta();
			mLog << "/qOverP: " << (*trackParticleItr)->qOverP();
			mLog << "/vx: " << (*trackParticleItr)->vx();
			mLog << "/vy: " << (*trackParticleItr)->vy();
			mLog << "/vz: " << (*trackParticleItr)->vz() << endreq;

			// Curvilinear functions skipped

			mLog << MSG::INFO << "REGTEST Fit quality functions:" << endreq;
			mLog << MSG::INFO << "REGTEST chiSquared: " << (*trackParticleItr)->chiSquared();
			mLog << "/numberDoF: " << (*trackParticleItr)->numberDoF() << endreq;

			// TrackInfo functions skipped

			mLog << MSG::INFO << "REGTEST summaryValue variables:" << endreq;
			mLog << MSG::INFO << "REGTEST ";
			uint8_t numberOfBLayerHits = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfBLayerHits, xAOD::numberOfBLayerHits) ) {
				mLog << "/numberOfBLayerHits: " << static_cast<int>(numberOfBLayerHits);
			} else {
				mLog << "/numberOfBLayerHits not found";
			}

			uint8_t numberOfPixelHits = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits) ) {
				mLog << "/numberOfPixelHits: " << static_cast<int>(numberOfPixelHits);
			} else {
				mLog << "/numberOfPixelHits not found";
			}

			uint8_t numberOfPixelHoles = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfPixelHoles, xAOD::numberOfPixelHoles) ) {
				mLog << "/numberOfPixelHoles: " << static_cast<int>(numberOfPixelHoles);
			} else {
				mLog << "/numberOfPixelHoles not found";
			}

			uint8_t numberOfSCTHits = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfSCTHits, xAOD::numberOfSCTHits) ) {
				mLog << "/numberOfSCTHits: " << static_cast<int>(numberOfSCTHits);
			} else {
				mLog << "/numberOfSCTHits not found";
			}

			uint8_t numberOfSCTHoles = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfSCTHoles, xAOD::numberOfSCTHoles) ) {
				mLog << "/numberOfSCTHoles: " << static_cast<int>(numberOfSCTHoles);
			} else {
				mLog << "/numberOfSCTHoles not found";
			}

			uint8_t numberOfTRTHits = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfTRTHits, xAOD::numberOfTRTHits) ) {
				mLog << "/numberOfTRTHits: " << static_cast<int>(numberOfTRTHits);
			} else {
				mLog << "/numberOfTRTHits not found";
			}

			uint8_t numberOfTRTHoles = 0;
			if ( (*trackParticleItr)->summaryValue(numberOfTRTHoles, xAOD::numberOfTRTHoles) ) {
				mLog << "/numberOfTRTHoles: " << static_cast<int>(numberOfTRTHoles);
			} else {
				mLog << "/numberOfTRTHoles not found";
			}
			mLog << endreq;
		}
	}

	mLog << MSG::INFO << "REGTEST ==========END of xAOD::TrackParticle DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpxAODTrackParticles() succeeded" << endreq;

	return returnsc;
}

StatusCode TrigEDMChecker::dumpxAODVertex() {
	MsgStream mLog( messageService(), name() );

	mLog << MSG::DEBUG << "In dumpxAODVertex()" << endreq;

	mLog << MSG::INFO << "REGTEST ==========START of xAOD::Vertex DUMP===========" << endreq;

	const xAOD::VertexContainer* vertexContainer=0;
	StatusCode sc = evtStore()->retrieve(vertexContainer,"HLT_xAOD__VertexContainer_xPrimVx");
	if (sc.isFailure()) {
		mLog << MSG::INFO << "REGTEST No vertex container" << endreq;
		return StatusCode::FAILURE;
	}
	mLog << MSG::INFO << "REGTEST VertexContainer retrieved" << endreq;

	xAOD::VertexContainer::const_iterator vertexItr = vertexContainer->begin();
	xAOD::VertexContainer::const_iterator vertexLast = vertexContainer->end();

	for (int index = 0; vertexItr != vertexLast; ++vertexItr, ++index) {
		mLog << MSG::INFO << "REGTEST Looking at Vertex " << index << endreq;

		mLog << MSG::INFO << "REGTEST Public Member Functions:" << endreq;
		mLog << MSG::INFO << "REGTEST x: " << (*vertexItr)->x();
		mLog << "/y: " << (*vertexItr)->y();
		mLog << "/z: " << (*vertexItr)->z() << endreq;

		mLog << MSG::INFO << "REGTEST Public Member Functions:" << endreq;
		mLog << MSG::INFO << "REGTEST chiSquared: " << (*vertexItr)->chiSquared();
		mLog << "/numberDoF: " << (*vertexItr)->numberDoF() << endreq;

	}

	mLog << MSG::INFO << "REGTEST ==========END of xAOD::Vertex DUMP===========" << endreq;

	mLog << MSG::DEBUG << "dumpxAODVertex() succeeded" << endreq;

	return StatusCode::SUCCESS;
}
