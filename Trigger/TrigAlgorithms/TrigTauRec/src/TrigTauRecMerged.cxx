/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigTauRecMerged.cxx
 * PACKAGE:  Trigger/TrigAlgorithms/TrigTauRecMerged    
 *
 * AUTHOR:   S.Xella 
 * MODIFIED: Dec 14 2006 
 *           Mar 02 2011 ccuenca
 *   
 *           Run a subset of the offline tauRec/tau1p3p routines in the EF 
 *********************************************************************/

//#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/ListItem.h"
//#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/Property.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "CaloEvent/CaloCellContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "xAODJet/JetContainer.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"

// needs migration to master of RNN tools in tauRecTools
//#include "tauRecTools/TauJetRNN.h"

#include "TrigTauRec/TrigTauRecMerged.h"

using namespace std;

// Invokes base class constructor.
TrigTauRecMerged::TrigTauRecMerged(const std::string& name,ISvcLocator* pSvcLocator):
		  HLT::FexAlgo(name, pSvcLocator),
		  m_outputName("TrigTauRecMerged"),
		  m_tools(this),
		  m_endtools(this),
		  m_lumiTool("LuminosityTool"),
		  m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
		  m_maxeta( 2.5 ),
		  m_minpt( 10000 ),
		  m_trkcone( 0.2 ) //,
		  //m_rnn_evaluator(0)
{
  
  // The following properties can be specified at run-time
  // (declared in jobOptions file)
  
  /** Name of output collection */
  declareProperty("OutputCollection",m_outputName,"Name of output collection");
  
  /** Maximal eta value for the seeds to be accepted */
  declareProperty("maxeta",m_maxeta,"max eta for tau");
  
  /** minimal pt of a seed */
  declareProperty("minpt",m_minpt,"min pt for tau");
  
  /** list of tool names to be invoked */
  declareProperty("Tools", m_tools, "List of ITauToolBase tools" );
  
  /** list of end tool names to be invoked */
  declareProperty("EndTools", m_endtools, "List of End ITauToolBase tools" );

  declareProperty("BeamType", m_beamType = "collisions");
  
  /** cone for trk seed  */
  declareProperty("trkcone",m_trkcone,"max distance track seed from roi center");
  
  /** Luminosity tools - first is deprecated but kept in case of need  */
  declareProperty("LuminosityTool", m_lumiTool, "Luminosity Tool");
  declareProperty("LumiBlockMuTool", m_lumiBlockMuTool, "Luminosity Tool" );
  
  /** number of cells in ROI */
  declareMonitoredVariable("nRoI_EFTauCells",m_nCells);
  
  /** number of tracks in ROI */
  declareMonitoredVariable("nRoI_EFTauTracks",m_nTracks);
  
  /** eta of Tau - eta ROI */
  declareMonitoredVariable("dEtaEFTau_RoI",m_dEta);
  
  /** phi of Tau - phi ROI */
  declareMonitoredVariable("dPhiEFTau_RoI",m_dPhi);
  
  /** EMRadius */
  declareMonitoredVariable("EF_EMRadius",m_emRadius);
  
  /** HadRadius */
  declareMonitoredVariable("EF_HadRadius",m_hadRadius);
  
  /** Calibrated Et */
  declareMonitoredVariable("EF_EtFinal",m_EtFinal);
  
  /** Et at EM scale*/
  declareMonitoredVariable("EF_Et",m_Et);
  
  /** had Et at EM scale*/
  declareMonitoredVariable("EF_EtHad",m_EtHad);
  
  /** EM Et at EM scale */
  declareMonitoredVariable("EF_EtEm",m_EtEm);
  
  /** EM fraction of tau energy */
  declareMonitoredVariable("EF_EMFrac",m_EMFrac);
  
  /** Isolation fraction */
  declareMonitoredVariable("EF_IsoFrac",m_IsoFrac);
  
  /** seedCalo_centFrac **/
  declareMonitoredVariable("EF_centFrac",m_centFrac);
  /** seedCalo_nWideTrk **/
  declareMonitoredVariable("EF_nWideTrk",m_nWideTrk);
  /** ipSigLeadTrk **/
  declareMonitoredVariable("EF_ipSigLeadTrk",m_ipSigLeadTrk);
  /** trFlightPathSig **/
  declareMonitoredVariable("EF_trFlightPathSig",m_trFlightPathSig);
  /** massTrkSys **/
  declareMonitoredVariable("EF_massTrkSys",m_massTrkSys);
  /** seedCalo_dRmax **/
  declareMonitoredVariable("EF_dRmax",m_dRmax);
  /** Number of tracks used for tau ID */
  declareMonitoredVariable("EF_NTrk",m_numTrack);
  
  /** Track Average Distance used for tau ID */
  declareMonitoredVariable("EF_TrkAvgDist",m_trkAvgDist);
  
  /** Et over lead track pt used for tau ID */
  declareMonitoredVariable("EF_EtovPtLead",m_etovPtLead);
  
  /** presampler strip energy fraction **/
  declareMonitoredVariable("EF_PSSFraction",m_PSSFraction);
  
  /** EMPOverTrkSysP **/
  declareMonitoredVariable("EF_EMPOverTrkSysP",m_EMPOverTrkSysP);
  
  /** EM energy of charged pions over calorimetric EM energy **/
  declareMonitoredVariable("EF_ChPiEMEOverCaloEME",m_ChPiEMEOverCaloEME);
 
  /** Fraction of pt tracks **/
  declareMonitoredVariable("EF_SumPtTrkFrac",m_SumPtTrkFrac);
 
  /** inner track average distance **/
  declareMonitoredVariable("EF_innerTrkAvgDist",m_innerTrkAvgDist);

  /** Number tau candidates */
  declareMonitoredVariable("EF_nCand",m_Ncand);
 
  /** Actual number of interaction per bunch crossing */
  declareMonitoredVariable("EF_ActualInteractions",m_ActualInteractions); 
  /** Average number of interaction per bunch crossing */
  declareMonitoredVariable("EF_AvgInteractions",m_AvgInteractions); 

  /** Beamspot position */
  declareMonitoredVariable("EF_beamspot_x",m_beamspot_x);
  declareMonitoredVariable("EF_beamspot_y",m_beamspot_y);
  declareMonitoredVariable("EF_beamspot_z",m_beamspot_z);

  /** Eta of L1 RoI */
  declareMonitoredVariable("EtaL1",m_EtaL1);
  
  /** Phi of L1 RoI */
  declareMonitoredVariable("PhiL1",m_PhiL1);
  
  /** Eta of EF RoI */
  declareMonitoredVariable("EtaEF",m_EtaEF);
  
  /** Phi of EF RoI */
  declareMonitoredVariable("PhiEF",m_PhiEF);
  
  /** Errors */
  declareMonitoredStdContainer("EF_calo_errors",m_calo_errors);
  
  /** Errors */
  declareMonitoredStdContainer("EF_track_errors",m_track_errors);
  
  // RNN inputs
  // scalar
  declareMonitoredVariable("EF_RNN_scalar_ptRatioEflowApprox", m_RNN_scalar_ptRatioEflowApprox);
  declareMonitoredVariable("EF_RNN_scalar_mEflowApprox", m_RNN_scalar_mEflowApprox);
  declareMonitoredVariable("EF_RNN_scalar_pt_jetseed_log", m_RNN_scalar_pt_jetseed_log);

  // clusters
  declareMonitoredVariable("EF_RNN_Nclusters", m_RNN_Nclusters);
  declareMonitoredStdContainer("EF_RNN_cluster_et_log", m_RNN_cluster_et_log);
  declareMonitoredStdContainer("EF_RNN_cluster_dEta", m_RNN_cluster_dEta);
  declareMonitoredStdContainer("EF_RNN_cluster_dPhi", m_RNN_cluster_dPhi);
  declareMonitoredStdContainer("EF_RNN_cluster_CENTER_LAMBDA", m_RNN_cluster_CENTER_LAMBDA);
  declareMonitoredStdContainer("EF_RNN_cluster_SECOND_LAMBDA", m_RNN_cluster_SECOND_LAMBDA);
  declareMonitoredStdContainer("EF_RNN_cluster_SECOND_R", m_RNN_cluster_SECOND_R);

  // tracks
  declareMonitoredVariable("EF_RNN_Ntracks", m_RNN_Ntracks);
  declareMonitoredStdContainer("EF_RNN_track_pt_log", m_RNN_track_pt_log);
  declareMonitoredStdContainer("EF_RNN_track_dEta", m_RNN_track_dEta);
  declareMonitoredStdContainer("EF_RNN_track_dPhi", m_RNN_track_dPhi);
  declareMonitoredStdContainer("EF_RNN_track_d0_abs_log", m_RNN_track_d0_abs_log);
  declareMonitoredStdContainer("EF_RNN_track_z0sinThetaTJVA_abs_log", m_RNN_track_z0sinThetaTJVA_abs_log);
  declareMonitoredStdContainer("EF_RNN_track_nInnermostPixelHits", m_RNN_track_nInnermostPixelHits);
  declareMonitoredStdContainer("EF_RNN_track_nPixelHits", m_RNN_track_nPixelHits);
  declareMonitoredStdContainer("EF_RNN_track_nSCTHits", m_RNN_track_nSCTHits);
  // output
  declareMonitoredVariable("EF_RNNJetScore", m_RNNJetScore);
  declareMonitoredVariable("EF_RNNJetScoreSigTrans", m_RNNJetScoreSigTrans);

}

TrigTauRecMerged::~TrigTauRecMerged()
{ 
}

HLT::ErrorCode TrigTauRecMerged::hltInitialize()
{
	ATH_MSG_INFO( "TrigTauRecMerged::initialize()" );

	m_tauEventData.setInTrigger(true);
	////////////////////
	// Allocate Tools //
	////////////////////
	// check tool names
	if ( m_tools.begin() == m_tools.end() ) {
	        ATH_MSG_ERROR( "No tools given for this algorithm." );
		return HLT::BAD_JOB_SETUP;
	}

	// find tools
	//-------------------------------------------------------------------------
	ToolHandleArray<ITauToolBase> ::iterator p_itT = m_tools.begin();
	ToolHandleArray<ITauToolBase> ::iterator p_itTE = m_tools.end();
	ATH_MSG_INFO( "List of tools in execution sequence:" );
	ATH_MSG_INFO( "------------------------------------" );

	for(; p_itT != p_itTE; ++p_itT ) {
		StatusCode p_sc = p_itT->retrieve();
		if( p_sc.isFailure() ) {
		        ATH_MSG_WARNING( "Cannot find tool named <" << *p_itT << ">" );
			return HLT::BAD_JOB_SETUP;
		}
		else {
		        ATH_MSG_INFO( "REGTEST " );
			ATH_MSG_INFO( "Add timer for tool "<< ( *p_itT )->type() <<" "<< ( *p_itT )->name() );
			if(  doTiming() ) m_mytimers.push_back(addTimer((*p_itT)->name())) ;
			(*p_itT)->setTauEventData(&m_tauEventData);

			/*
			// monitoring of RNN input variables
			ToolHandle<ITauToolBase> handle = *p_itT;
			TauJetRNNEvaluator* RNN_evaluator = dynamic_cast<TauJetRNNEvaluator*>(&(*handle));
			if(RNN_evaluator) {
			  ATH_MSG_INFO( "Retrieved TauJetRNNEvaluator for RNN inputs monitoring" );
			  m_rnn_evaluator = RNN_evaluator;
			}
			*/
		}
	}

	ATH_MSG_INFO( " " );
	ATH_MSG_INFO( "------------------------------------" );


	ToolHandleArray<ITauToolBase> ::iterator p_itTe = m_endtools.begin();
	ToolHandleArray<ITauToolBase> ::iterator p_itTEe = m_endtools.end();

	ATH_MSG_INFO( "List of end tools in execution sequence:" );
	ATH_MSG_INFO( "------------------------------------" );

	for(; p_itTe != p_itTEe; ++p_itTe ) {
		StatusCode p_sc = p_itTe->retrieve();
		if( p_sc.isFailure() ) {
		        ATH_MSG_WARNING( "Cannot find tool named <" << *p_itTe << ">" );
			return HLT::BAD_JOB_SETUP;
		}
		else {
	  	        ATH_MSG_INFO( "REGTEST " );
			ATH_MSG_INFO( "Add time for end tool "<< ( *p_itTe )->type() <<" "<< ( *p_itTe )->name() );
			if(  doTiming() ) m_mytimers.push_back(addTimer((*p_itTe)->name())) ;
			( *p_itTe )->setTauEventData(&m_tauEventData);
		}
	}

        m_lumiTool.disable();   // never used?
	// // Try to retrieve the lumi tool
	// if (m_lumiTool.retrieve().isFailure()) {                                     
	//   msg() << MSG::WARNING << "Unable to retrieve Luminosity Tool" << endmsg;     
	// } else {                                                                     
	//   msg() << MSG::DEBUG << "Successfully retrieved Luminosity Tool" << endmsg; 
	// }                                                                            

	if (m_lumiBlockMuTool.retrieve().isFailure()) {                                     
	  ATH_MSG_WARNING( "Unable to retrieve LumiBlockMuTool" );
	} else {                                                                     
	  ATH_MSG_DEBUG( "Successfully retrieved LumiBlockMuTool" ); 
	}                                                                            

	// Retrieve beam conditions
	if(m_beamSpotKey.initialize().isFailure()) {
	  ATH_MSG_WARNING( "Unable to retrieve Beamspot key" );
        } else {
          ATH_MSG_DEBUG( "Successfully retrieved Beamspot key" );
	}
	
	ATH_MSG_INFO( " " );
	ATH_MSG_INFO( "------------------------------------" );
	
	return HLT::OK;
}

/////////////////////////////////////////////////////////////////
HLT::ErrorCode TrigTauRecMerged::hltFinalize()
{
	return HLT::OK;
}


/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:
HLT::ErrorCode TrigTauRecMerged::hltExecute(const HLT::TriggerElement* inputTE, 
		HLT::TriggerElement* outputTE)
{

	m_nCells  = 0;
	m_nTracks = -10;
	m_dEta    = -10;
	m_dPhi    = -10;
	m_emRadius     = -0.099;
	m_hadRadius     = -0.099;

	m_Et = 0.;
	m_EtFinal = 0.;
	m_EMFrac = -10.;
	m_IsoFrac = -1.;
	m_numTrack = -10;
	m_trkAvgDist = -1.0;
	m_etovPtLead = -10.0;
	m_Ncand = 0;
        m_ActualInteractions = -999.9; 
        m_AvgInteractions = -999.9; 

	m_beamspot_x = -999.9;
	m_beamspot_y = -999.9;
	m_beamspot_z = -999.9;

	m_centFrac=-10;
	m_nWideTrk=0;
	m_ipSigLeadTrk=-1000;
	m_trFlightPathSig=-10;
	m_massTrkSys=-10;
	m_dRmax=-10.;

	m_PSSFraction=-999;
	m_EMPOverTrkSysP=-999;
	m_ChPiEMEOverCaloEME=-999;
	m_innerTrkAvgDist=-999;
	m_SumPtTrkFrac=-999;

	m_calo_errors.clear();
	m_track_errors.clear();
	m_EtEm =-10.;
	m_EtHad=-10.;

	m_EtaL1 = -99;
	m_PhiL1 = -99;

	m_EtaEF = -99;
	m_PhiEF = -99;

	// RNN variables monitoring
	m_RNN_scalar_ptRatioEflowApprox = 0.; m_RNN_scalar_mEflowApprox = 0.; m_RNN_scalar_pt_jetseed_log = 0.;
	m_RNN_Nclusters = -1;
	m_RNN_cluster_et_log.clear(); m_RNN_cluster_dEta.clear(); m_RNN_cluster_dPhi.clear();
	m_RNN_cluster_CENTER_LAMBDA.clear(); m_RNN_cluster_SECOND_LAMBDA.clear(); m_RNN_cluster_SECOND_R.clear();
	m_RNN_Ntracks = -1;
	m_RNN_track_pt_log.clear(); m_RNN_track_dEta.clear(); m_RNN_track_dPhi.clear();
	m_RNN_track_d0_abs_log.clear(); m_RNN_track_z0sinThetaTJVA_abs_log.clear();
	m_RNN_track_nInnermostPixelHits.clear(); m_RNN_track_nPixelHits.clear(); m_RNN_track_nSCTHits.clear();
	m_RNNJetScore = -999; m_RNNJetScoreSigTrans = -999.;

	// Retrieve store.
	if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "Executing TrigTauRecMerged" );

	// Get RoiDescriptor
	const TrigRoiDescriptor* roiDescriptor = 0;
	HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);

	if ( hltStatus==HLT::OK && roiDescriptor!= nullptr ) {
		if( msgLvl() <= MSG::DEBUG )
		    ATH_MSG_DEBUG( "REGTEST: RoI " << *roiDescriptor );
	}
	else {
	        ATH_MSG_ERROR( "Failed to find RoiDescriptor " );
		m_calo_errors.push_back(NoROIDescr);
		return hltStatus;
	}

	// get CaloCellContainer
	// Probably not necessary
	vector<const CaloCellContainer*> vectorCaloCellContainer;
	hltStatus = getFeatures(inputTE, vectorCaloCellContainer);

	if(hltStatus!=HLT::OK ) {
	        ATH_MSG_ERROR( " No CaloCellContainers retrieved for the trigger element" );
		m_calo_errors.push_back(NoCellCont);
		return hltStatus;
	}

	if (vectorCaloCellContainer.size() < 1) {
	        ATH_MSG_ERROR( "Size of vector CaloCell container is not 1. Is " << vectorCaloCellContainer.size() );
		m_calo_errors.push_back(NoCellCont);
		return HLT::ERROR;
	}

	const CaloCellContainer* RoICaloCellContainer = vectorCaloCellContainer.back();

	if(RoICaloCellContainer != nullptr) {
		m_nCells = RoICaloCellContainer->size();
		ATH_MSG_DEBUG("REGTEST: Size of vector CaloCell container is " << RoICaloCellContainer->size() );
		if(RoICaloCellContainer->size()==0) {
		        ATH_MSG_INFO( "Cannot proceed, size of vector CaloCell container is " << RoICaloCellContainer->size() );
			m_calo_errors.push_back(EmptyCellCont);
			return HLT::OK;
		}
	}
	else {
	        ATH_MSG_ERROR( "No CaloCell container found" );
		m_calo_errors.push_back(NoCellCont);
		return HLT::ERROR;
	}

	// Retrieve Calocluster container
	std::vector<const xAOD::CaloClusterContainer*> vectorCaloClusterContainer;
	hltStatus = getFeatures(inputTE, vectorCaloClusterContainer);
  
	if(hltStatus!=HLT::OK ) {
	  ATH_MSG_ERROR( "No CaloClusterContainers retrieved for the trigger element" );
	  m_calo_errors.push_back(NoClustCont);
	  return hltStatus;
	}

	if (vectorCaloClusterContainer.size() < 1) {
	  ATH_MSG_ERROR( "CaloCluster container is empty" );
	  m_calo_errors.push_back(NoClustCont);
	  return HLT::ERROR;
	}
  
	if( msgLvl() <= MSG::DEBUG )
	  ATH_MSG_DEBUG( "CaloCluster container size is " << vectorCaloClusterContainer.size() );
  
	// Grab the last cluster collection attached
	const xAOD::CaloClusterContainer* RoICaloClusterContainer = vectorCaloClusterContainer.back();

	std::string collKey;
	hltStatus = getStoreGateKey( RoICaloClusterContainer, collKey );

	if(hltStatus!=HLT::OK ) {
	        ATH_MSG_ERROR( "Cluster has no key" );
		m_calo_errors.push_back(NoClustKey);
		return HLT::ERROR;
	}

	if( msgLvl() <= MSG::DEBUG )
	  ATH_MSG_DEBUG( "Cluster key for back cluster is " << collKey );

	
	// Not necessary anymore
	/*
	const INavigable4MomentumCollection*  RoICaloClusterContainer;
	StatusCode sc = store()->retrieve(RoICaloClusterContainer,collKey);

	if(sc.isFailure()) {
		msg() << MSG :: ERROR << "failed to find Cluster container"<< endmsg;
		m_calo_errors.push_back(NoClustCont);
		return HLT :: ERROR;
	}

	if(RoICaloClusterContainer != NULL) {
		msg() << MSG::DEBUG
				<< "REGTEST: Size of vector CaloCluster container is "
				<< RoICaloClusterContainer->size()
				<< endmsg;
		if(RoICaloClusterContainer->size()==0) {
			msg() << MSG::DEBUG
					<< "Cannot proceed, size of vector CaloCluster container is "
					<< RoICaloClusterContainer->size()
					<< endmsg;
			m_calo_errors.push_back(EmptyClustCont);
			return HLT::OK;
		}
	}
	else {
		msg() << MSG::ERROR << "no CaloCluster container found "<< endmsg;
		m_calo_errors.push_back(NoClustCont);
		return HLT::ERROR;
	}
	*/
	

	// get TrackContainer
	vector<const xAOD::TrackParticleContainer*> vectorTrackContainer;
	hltStatus = getFeatures(inputTE,vectorTrackContainer);

	const xAOD::TrackParticleContainer* RoITrackContainer = nullptr;

	if (hltStatus!=HLT::OK) {
	        ATH_MSG_INFO( "REGTEST: No Track container found." );
		m_track_errors.push_back(NoTrkCont);
	}
	else {
		if (vectorTrackContainer.size()<1) {
		        ATH_MSG_DEBUG( "Size of vector Track container is not 1. Is " << vectorTrackContainer.size() );
			m_track_errors.push_back(NoTrkCont);
		}

		if(vectorTrackContainer.size()>0) {
			RoITrackContainer = vectorTrackContainer.back();
			if( msgLvl() <= MSG::DEBUG )
			  ATH_MSG_DEBUG( "REGTEST: Size of vector Track container is " << RoITrackContainer->size() );
		}
		if(RoITrackContainer != nullptr) m_nTracks = RoITrackContainer->size();
	}

	// get Vertex Container
	vector<const xAOD::VertexContainer*> vectorVxContainer;
	hltStatus = getFeatures(inputTE,vectorVxContainer);
	const xAOD::VertexContainer* RoIVxContainer = nullptr;

	if(hltStatus!=HLT::OK) {
	        ATH_MSG_INFO( "No VxContainers retrieved for the trigger element" );
		m_track_errors.push_back(NoVtxCont);
	}
	else {
		if (vectorVxContainer.size() < 1) {
		        ATH_MSG_DEBUG( "Size of vector Vertex  container is not 1. Is " << vectorVxContainer.size() );
			m_track_errors.push_back(NoVtxCont);
		}

		if(vectorVxContainer.size() >0) {
			RoIVxContainer = vectorVxContainer.back();
			if( msgLvl() <= MSG::DEBUG )
			  ATH_MSG_DEBUG( "REGTEST: Size of vector Vertex  container " << RoIVxContainer->size() );
		}
	}

	//-------------------------------------------------------------------------
	// Get the online luminosity                                                                                                               
	//-------------------------------------------------------------------------
	// double mu = 0.0;
        // double avg_mu = 0.0;
        // mu = m_lumiTool->lbLuminosityPerBCID() / m_lumiTool->muToLumi(); // (retrieve mu for the current BCID)                                             
	// avg_mu = m_lumiTool->lbAverageInteractionsPerCrossing();
        // msg() << MSG::DEBUG << "REGTEST: lbLuminosityPerBCID : " << m_lumiTool->lbLuminosityPerBCID() << endmsg;
        // msg() << MSG::DEBUG << "REGTEST: muToLumi            : " << m_lumiTool->muToLumi() << endmsg;
	// msg() << MSG::DEBUG << "REGTEST: Retrieved Mu Value  : " << mu << endmsg;
        // msg() << MSG::DEBUG << "REGTEST: Average Mu Value    : " << avg_mu << endmsg;

	double mu = 0.0;
        double avg_mu = 0.0;
	if(m_lumiBlockMuTool){

	  mu = m_lumiBlockMuTool->actualInteractionsPerCrossing(); // (retrieve mu for the current BCID)
	  avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
          m_ActualInteractions = mu;
          m_AvgInteractions = avg_mu;
	  ATH_MSG_DEBUG( "REGTEST: Retrieved Mu Value : " << mu );
	  ATH_MSG_DEBUG( "REGTEST: Average Mu Value   : " << avg_mu );
	}
	

	//-------------------------------------------------------------------------
	// Get beamspot
	//-------------------------------------------------------------------------

	// Copy the first vertex from a const object
	xAOD::Vertex theBeamspot;
	theBeamspot.makePrivateStore();
	const xAOD::Vertex* ptrBeamspot = 0;
        SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
	if(beamSpotHandle.isValid()){
	
	  // Alter the position of the vertex
	  theBeamspot.setPosition(beamSpotHandle->beamPos());
	
	  m_beamspot_x=theBeamspot.x();
	  m_beamspot_y=theBeamspot.y();
	  m_beamspot_z=theBeamspot.z();

	  // Create a AmgSymMatrix to alter the vertex covariance mat.
	  const AmgSymMatrix(3) &cov = beamSpotHandle->beamVtx().covariancePosition();
	  theBeamspot.setCovariancePosition(cov);

	  ptrBeamspot = &theBeamspot;

	}


	//-------------------------------------------------------------------------
	// Creating jet container used as "tau seed" for tau reconstruction
	//-------------------------------------------------------------------------

	// Ensure that we are using a TrigAuxContainer
	xAOD::JetContainer* theJetCollection = new xAOD::JetContainer();
	xAOD::JetTrigAuxContainer theJetContainer;
	theJetCollection->setStore(&theJetContainer);
	
	xAOD::Jet* aJet = new xAOD::Jet();

	// xxx ToDo: necessary?? no xAOD setter yet
	/*
	// set initial tau values to RoI position
	// pass a name modifier to the offline tools to adjust container
	// names based on RoI number
	int roiNumber = roiDescriptor->roiId();
	std::string TrigRoIName;
	std::stringstream strm; strm << roiNumber;
	TrigRoIName=strm.str();
	aJet->set_RoIword( roiNumber);
	*/

	theJetCollection->push_back(aJet);
	
	// Build the jet, also keep track of the kinematics by hand
	// We are using calibrated clusters, we need to keep track of this
	aJet->setConstituentsSignalState(xAOD::JetConstitScale::CalibratedJetConstituent);
	
	// Make a minimal effort to speed things up ;)
	// Eventually, want to use FastJet here?
	TLorentzVector myCluster;
  	TLorentzVector TauBarycenter(0., 0., 0., 0.);
  
	xAOD::CaloClusterContainer::const_iterator clusterIt;
	for (clusterIt=RoICaloClusterContainer->begin(); clusterIt != RoICaloClusterContainer->end(); ++clusterIt) {
	  if( msgLvl() <= MSG::DEBUG )
	    ATH_MSG_DEBUG( "Cluster (e, eta, phi) : ("<< (*clusterIt)->e() << " , " <<(*clusterIt)->eta()<<" , "<<(*clusterIt)->phi()<< " )" );
    
	  if((*clusterIt)->e() < 0)
	    {
	      if( msgLvl() <= MSG::DEBUG )
		ATH_MSG_DEBUG( "Negative energy cluster is rejected" );
	      continue;
	    }
    
	  myCluster.SetPtEtaPhiE((*clusterIt)->pt(), (*clusterIt)->eta(), (*clusterIt)->phi(), (*clusterIt)->e());
	  aJet->addConstituent(*clusterIt);

	  TauBarycenter += myCluster;
	}
	
	aJet->setJetP4(xAOD::JetFourMom_t(TauBarycenter.Pt(), TauBarycenter.Eta(), TauBarycenter.Phi(), TauBarycenter.M() ) ); 

	if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "Jet formed "<< aJet->eta() <<" , " << aJet->phi() <<" , " << aJet->pt() << " , "<< aJet->e() );
	
	hltStatus=attachFeature(outputTE, theJetCollection, "TrigTauJet");
	
	if (hltStatus!=HLT::OK ) {
	  ATH_MSG_ERROR( "Unable to record JetCollection  in TDS" );
	  m_calo_errors.push_back(NoJetAttach);
	  return hltStatus;
	}
	else {
	  if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "JetCollection is recorded with key " << "HLT_" << "_" << "TrigTauJet" );
	}


	StatusCode processStatus    = StatusCode::SUCCESS;

	//-------------------------------------------------------------------------
	// setup TauCandidate data
	//-------------------------------------------------------------------------
	m_tauEventData.clear();
	xAOD::TauJetContainer *pContainer = new xAOD::TauJetContainer();
	xAOD::TauJetAuxContainer pAuxContainer;

	xAOD::TauTrackContainer *pTrackContainer = new xAOD::TauTrackContainer();
	xAOD::TauTrackAuxContainer pTrackAuxContainer;

	// make dummy container to pass to TauVertexVariables, not actually used in trigger though
	xAOD::VertexContainer* dummyVxCont = new xAOD::VertexContainer();

	// Set the store: eventually, we want to use a dedicated trigger version
	pContainer->setStore(&pAuxContainer);

	// Set the store: eventually, we want to use a dedicated trigger version
	pTrackContainer->setStore(&pTrackAuxContainer);
	m_tauEventData.setObject("TauTrackContainer", pTrackContainer);

	// set TauCandidate properties (xAOD style)
	m_tauEventData.xAODTauContainer = pContainer;

	// This is potentially a bit dangerous, but all the tools using m_tauEventData
	// are run in the current scope
	m_tauEventData.tauAuxContainer = &pAuxContainer;

	// Set the Objects that we can attach right now
	// m_tauEventData.setObject("InTrigger?", true ); Set this in initialize, now a member variable of TauEventData
	m_tauEventData.setObject("TrackContainer", RoITrackContainer);
	m_tauEventData.setObject("VxPrimaryCandidate", RoIVxContainer);
	if(m_lumiBlockMuTool) m_tauEventData.setObject("AvgInteractions", avg_mu);
	if(beamSpotHandle.isValid()) m_tauEventData.setObject("Beamspot", ptrBeamspot);
	if(m_beamType == ("cosmics")) m_tauEventData.setObject("IsCosmics?", true );


	//-------------------------------------------------------------------------
	// eventInitialize tauRec colls
	//-------------------------------------------------------------------------
	ToolHandleArray<ITauToolBase> ::iterator firstTool = m_tools.begin();
	ToolHandleArray<ITauToolBase> ::iterator lastTool  = m_tools.end();
	for ( ; firstTool != lastTool; firstTool++ ) {
		processStatus = (*firstTool)->eventInitialize();

		if( processStatus != StatusCode :: SUCCESS ) {
		        ATH_MSG_ERROR( "Tool "<<(*firstTool)->name()<< "failed in eventInitialize" );
			return HLT :: TOOL_FAILURE;
		}
	}
	if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "initialize all good" );

	//-------------------------------------------------------------------------
	// using Jet collection
	// setup tau candidate structure
	//-------------------------------------------------------------------------
	xAOD::Jet* p_seed =    (*theJetCollection->begin());  //there is only one jet stored

	xAOD::TauJet* p_tau = new xAOD::TauJet();
	pContainer->push_back(p_tau);
	p_tau->setROIWord(roiDescriptor->roiWord());
	p_tau->setJet(theJetCollection, p_seed);
	m_tauEventData.seedContainer = theJetCollection;

	// This sets one track and link. Need to have at least 1 track linked to retrieve track container
	// can't we instead implement in the EDM a function to retrieve the track container of a tau?
	setEmptyTauTrack(p_tau, pTrackContainer);

	if(p_seed->e()<=0) {
                ATH_MSG_DEBUG( "Roi: changing eta due to energy " << p_seed->e() );
		p_tau->setP4(p_tau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), p_tau->m());
		
		ATH_MSG_DEBUG( "Roi: " << roiDescriptor->roiId() << " Tau eta: " << p_tau->eta() << " Tau phi: " << p_tau->phi() );
	}

	if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "roidescriptor roiword " << roiDescriptor->roiWord() << " saved " << p_tau->ROIWord() );

	m_tauEventData.setObject("JetCollection", theJetCollection );

	//-------------------------------------------------------------------------
	// loop over booked tau tools
	//-------------------------------------------------------------------------
	int toolnum = 0;
	firstTool = m_tools.begin();
	lastTool  = m_tools.end();
	processStatus    = StatusCode::SUCCESS;
	if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "Starting tool loop with seed jet" );
	std::vector<TrigTimer* >::iterator itimer =  m_mytimers.begin();
	while ( ! processStatus.isFailure() && firstTool != lastTool ) {
	        // loop stops only when Failure indicated by one of the tools
	        if( msgLvl() <= MSG::DEBUG ) {
		        ATH_MSG_DEBUG( "Starting Tool:");
			ATH_MSG_DEBUG( (*firstTool)->name() );
		}
		// time in the various tools
		++toolnum;
		if ( doTiming() && itimer != m_mytimers.end() ) (*itimer)->start();

		if ( (*firstTool)->name().find("VertexVariables") != std::string::npos){
		  processStatus = (*firstTool)->executeVertexVariables(*p_tau, *dummyVxCont);
		}
		else {
		  processStatus = (*firstTool)->execute( *p_tau );
		}

		if ( !processStatus.isFailure() ) {
			if( msgLvl() <= MSG::DEBUG ) {
			  ATH_MSG_DEBUG( "REGTEST: "<< (*firstTool)->name() << " executed successfully ");
			  ATH_MSG_DEBUG( "REGTEST: Roi: " << roiDescriptor->roiId()
					 << " Tau eta: " << p_tau->eta() << " Tau phi: " << p_tau->phi()
					 << " Tau pT : "<< p_tau->pt() );
			}
		}
		else {
		  if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "REGTEST: "<< (*firstTool)->name() << " execution failed" );
		}
		++firstTool;
		++itimer;
		if ( doTiming() && itimer != m_mytimers.end() ) (*itimer)->stop();
	}

	//check status
	if ( !processStatus.isSuccess() )  {   // some problem
	        if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "The tau object has NOT been registered in the tau container" );
		// ToolHandleArray<ITauToolBase> ::iterator tool = m_tools.begin();
		// for(; tool != firstTool; ++tool ) (*tool)->cleanup( &m_tauEventData );
		// (*tool)->cleanup( &m_tauEventData );

		xAOD::TauJet* bad_tau = pContainer->back();
		ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
		pTrackContainer->erase(pTrackContainer->end()-bad_tau->nAllTracks(), pTrackContainer->end());

		pContainer->pop_back();

		if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "Clean up done after jet seed" );
	}
	else if( processStatus.isSuccess()) {

	  float fJetEnergy = (*p_tau->jetLink())->e();
	  ATH_MSG_DEBUG( "Roi: jet e "<< fJetEnergy );
	  
	  if( fJetEnergy < 0.00001 ) {
	    ATH_MSG_DEBUG( "Roi: changing eta phi to L1 ones due to energy negative (PxPyPzE flips eta and phi)" );
	    ATH_MSG_DEBUG( "Roi: this is probably not needed anymore, method PxPyPzE has been corrected" );
	    
	    //p_tau->setEta(roiDescriptor->eta0());
	    //p_tau->setPhi(roiDescriptor->phi0());
	    // Direct accessors not available anymore
	    p_tau->setP4(p_tau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), p_tau->m());
	    
	    ATH_MSG_DEBUG( " Roi: " << roiDescriptor->roiId()
			   << " Tau eta: " << p_tau->eta()
			   << " Tau phi: " << p_tau->phi()
			   << " Tau pT : "<< p_tau->pt() );
	  }
	  
	  // loop over end tools
	  ToolHandleArray<ITauToolBase> ::iterator p_itET = m_endtools.begin();
	  ToolHandleArray<ITauToolBase> ::iterator p_itETE = m_endtools.end();
	  for (; p_itET != p_itETE; ++p_itET ) {
	    ATH_MSG_VERBOSE( "Invoking endTool " << ( *p_itET )->name() );
	    
	    processStatus = ( *p_itET )->execute( *p_tau);
	    if( processStatus.isFailure() ) break;
	  }
	  // Get L1 RoiDescriptor
	  const TrigRoiDescriptor* roiL1Descriptor = 0;
	  HLT::ErrorCode tmpStatus = getFeature(inputTE, roiL1Descriptor,"initialRoI");
	  
	  if(tmpStatus==HLT::OK && roiL1Descriptor) {
	    m_EtaL1         =  roiL1Descriptor->eta();
	    m_PhiL1         =  roiL1Descriptor->phi();
	  }
	  
	  // get tau detail variables for Monitoring
	  m_numTrack      = p_tau->nTracks();
	  m_nWideTrk = p_tau->nTracksIsolation();
	  p_tau->detail(xAOD::TauJetParameters::trkAvgDist, m_trkAvgDist);
	  p_tau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, m_etovPtLead);
	  p_tau->detail(xAOD::TauJetParameters::EMRadius, m_emRadius);
	  p_tau->detail(xAOD::TauJetParameters::hadRadius, m_hadRadius);
	  p_tau->detail(xAOD::TauJetParameters::isolFrac, m_IsoFrac);
	  p_tau->detail(xAOD::TauJetParameters::centFrac, m_centFrac);
	  p_tau->detail(xAOD::TauJetParameters::ipSigLeadTrk, m_ipSigLeadTrk);
	  p_tau->detail(xAOD::TauJetParameters::trFlightPathSig, m_trFlightPathSig);
	  p_tau->detail(xAOD::TauJetParameters::dRmax, m_dRmax);
	  p_tau->detail(xAOD::TauJetParameters::massTrkSys, m_massTrkSys);
	  
	  p_tau->detail(xAOD::TauJetParameters::PSSFraction, m_PSSFraction);
	  p_tau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, m_EMPOverTrkSysP);
	  p_tau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, m_ChPiEMEOverCaloEME);
	  p_tau->detail(xAOD::TauJetParameters::innerTrkAvgDist, m_innerTrkAvgDist);	 
          p_tau->detail(xAOD::TauJetParameters::SumPtTrkFrac, m_SumPtTrkFrac);

	  m_massTrkSys /= 1000.; // make GeV
	  p_tau->detail(xAOD::TauJetParameters::etEMAtEMScale, m_EtEm);
	  m_EtEm /= 1000.;  // make GeV
	  p_tau->detail(xAOD::TauJetParameters::etHadAtEMScale, m_EtHad);
	  m_EtHad /= 1000.; // make GeV
	  m_Et            = m_EtEm + m_EtHad;
	  m_EtFinal       = p_tau->pt()/1000.;
	  
	  m_EtaEF = p_tau->eta();
	  m_PhiEF = p_tau->phi();
	  
	  if( m_Et !=0) m_EMFrac =  m_EtEm/ m_Et ;
	  
	  m_dEta =  m_EtaEF - roiDescriptor->eta();
	  m_dPhi =  m_PhiEF - roiDescriptor->phi();
	  if(m_dPhi<-M_PI) m_dPhi += 2.0*M_PI;
	  if(m_dPhi>M_PI)  m_dPhi -= 2.0*M_PI;
	  
	  
	  // uncomment when RNN tools migrated to master
	  /*
	  // RNN monitoring
	  if(m_rnn_evaluator) {

	    TauJetRNN* rnn = 0;
	    if(m_numTrack==0) rnn = m_rnn_evaluator->get_rnn_0p();
	    else if(m_numTrack==1) rnn = m_rnn_evaluator->get_rnn_1p();
	    else rnn = m_rnn_evaluator->get_rnn_3p();
	    
	    const std::map<std::string, std::map<std::string, double> >* rnn_scalar = rnn->getScalarInputs();
	    const std::map<std::string, std::map<std::string, std::vector<double>> >* rnn_vector = rnn->getVectorInputs();

	    m_RNN_scalar_ptRatioEflowApprox = rnn_scalar->at("scalar").at("ptRatioEflowApprox");
	    m_RNN_scalar_mEflowApprox = rnn_scalar->at("scalar").at("mEflowApprox");
	    // this is obviously a scalar, but is stored in tracks and clusters
	    if(rnn_vector->at("tracks").at("pt_log").size()>0) m_RNN_scalar_pt_jetseed_log = rnn_vector->at("tracks").at("pt_jetseed_log")[0];
	    else if(rnn_vector->at("clusters").at("et_log").size()>0) m_RNN_scalar_pt_jetseed_log = rnn_vector->at("clusters").at("pt_jetseed_log")[0];

	    m_RNN_Nclusters = rnn_vector->at("clusters").at("et_log").size();
	    m_RNN_cluster_et_log = rnn_vector->at("clusters").at("et_log");
	    m_RNN_cluster_dEta = rnn_vector->at("clusters").at("dEta");
	    m_RNN_cluster_dPhi = rnn_vector->at("clusters").at("dPhi");	    
	    m_RNN_cluster_CENTER_LAMBDA = rnn_vector->at("clusters").at("CENTER_LAMBDA");
	    m_RNN_cluster_SECOND_LAMBDA = rnn_vector->at("clusters").at("SECOND_LAMBDA"); 
	    m_RNN_cluster_SECOND_R = rnn_vector->at("clusters").at("SECOND_R");

	    m_RNN_Ntracks = rnn_vector->at("tracks").at("pt_log").size();
	    m_RNN_track_pt_log = rnn_vector->at("tracks").at("pt_log");
	    m_RNN_track_dEta = rnn_vector->at("tracks").at("dEta");
	    m_RNN_track_dPhi = rnn_vector->at("tracks").at("dPhi");
	    m_RNN_track_d0_abs_log = rnn_vector->at("tracks").at("d0_abs_log");
	    m_RNN_track_z0sinThetaTJVA_abs_log = rnn_vector->at("tracks").at("z0sinThetaTJVA_abs_log");
	    m_RNN_track_nInnermostPixelHits = rnn_vector->at("tracks").at("nIBLHitsAndExp");
	    m_RNN_track_nPixelHits = rnn_vector->at("tracks").at("nPixelHitsPlusDeadSensors");
	    m_RNN_track_nSCTHits = rnn_vector->at("tracks").at("nSCTHitsPlusDeadSensors");

	    if( !p_tau->hasDiscriminant(xAOD::TauJetParameters::RNNJetScore) || !p_tau->isAvailable<float>("RNNJetScore") )
	      ATH_MSG_WARNING( "RNNJetScore not available. Should not happen when TauJetRNNEvaluator is run!" );
	    else m_RNNJetScore = p_tau->discriminant(xAOD::TauJetParameters::RNNJetScore);
	    if( !p_tau->hasDiscriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans) || !p_tau->isAvailable<float>("RNNJetScoreSigTrans") )
	      ATH_MSG_WARNING( "RNNJetScoreSigTrans not available. Make sure TauWPDecorator is run!" );
	    else m_RNNJetScoreSigTrans = p_tau->discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans);
	  }
	  */  
	
	  //
	  // copy CaloOnly four vector, if present
	  //

	  std::vector<const xAOD::TauJetContainer*> tempCaloOnlyContVec;
	  HLT::ErrorCode tmpCaloOnlyStatus = getFeatures(inputTE, tempCaloOnlyContVec, "TrigTauRecCaloOnly");

	  if( tmpCaloOnlyStatus != HLT::OK){ 

	    ATH_MSG_DEBUG( "Can't get container TrigTauRecCaloOnly to copy four-vector" );

	  } else {

	    ATH_MSG_DEBUG( "Got container TrigTauRecCaloOnly size :" << tempCaloOnlyContVec.size() );
	     
	    if ( tempCaloOnlyContVec.size() != 0 ) {

	      // const xAOD::TauJetContainer* tempCaloOnlyTauCont = tempCaloOnlyContVec.back();
	      // for(xAOD::TauJetContainer::const_iterator tauIt = tempCaloOnlyTauCont->begin(); tauIt != tempCaloOnlyTauCont->end(); tauIt++){ 

	      // const xAOD::TauJetContainer* tempCaloOnlyTauCont = tempCaloOnlyContVec.back();

	      for(xAOD::TauJetContainer::const_iterator tauIt = tempCaloOnlyContVec.back()->begin(); tauIt != tempCaloOnlyContVec.back()->end(); tauIt++){ 

	   	ATH_MSG_DEBUG( "pT(tau) = " << (*tauIt)->pt() << " pT(caloOnly) = " << (*tauIt)->ptTrigCaloOnly() );
	  	
	   	p_tau->setP4(xAOD::TauJetParameters::TrigCaloOnly, (*tauIt)->ptTrigCaloOnly(), (*tauIt)->etaTrigCaloOnly(), (*tauIt)->phiTrigCaloOnly(), (*tauIt)->mTrigCaloOnly());

	      }
	      
	    }
	    
	  }


	  //
	  // Set NUMVERTICES Aux variable
	  //

	  // static SG::AuxElement::Accessor<int> acc_nVertex("NUMVERTICES");
	  // acc_nVertex(*p_tau) = avg_mu;



	  ATH_MSG_DEBUG( "REGTEST: Roi: " << roiDescriptor->roiId()
			 << " Tau being saved eta: " << m_EtaEF << " Tau phi: " << m_PhiEF
			 << " wrt L1 dEta "<< m_dEta<<" dPhi "<<m_dPhi
			 << " Tau Et (GeV): "<< m_EtFinal );
	  
	  ++m_Ncand;
	}
	else {
	  xAOD::TauJet* bad_tau = pContainer->back();
	  ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
	  pTrackContainer->erase(pTrackContainer->end()-bad_tau->nAllTracks(), pTrackContainer->end());
	  pContainer->pop_back();
	  
	  if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "Deleted tau done after jet seed" );
	}
	
	// call eventFinalize on the booked tau tools
	for ( firstTool = m_tools.begin(); firstTool != lastTool; firstTool++ ) {
	  processStatus = (*firstTool)->eventFinalize();
	  if( processStatus != StatusCode :: SUCCESS ) {
	    ATH_MSG_INFO( "Tool "<<(*firstTool)->name()<< "failed in eventFinalize" );
	    return HLT :: TOOL_FAILURE;
	  }
	}
	ATH_MSG_DEBUG( "Tools succeed in eventFinalize" );
	
	
	//-------------------------------------------------------------------------
	// all done, register the tau Container in TDS.
	//-------------------------------------------------------------------------

	hltStatus=attachFeature(outputTE, pContainer, m_outputName);
	hltStatus=attachFeature(outputTE, pTrackContainer, m_outputName+"Tracks");
	if (hltStatus!=HLT::OK )  {
	        ATH_MSG_ERROR( "Unable to record tau Container in TDS" );
		m_calo_errors.push_back(NoHLTtauAttach);
		return hltStatus;
	}
	else {
	  if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "Recorded a tau container: " << "HLT_" << "TrigTauRecMerged" );
	}

	if( msgLvl() <= MSG::DEBUG ) ATH_MSG_DEBUG( "The tau object has been registered in the tau container" );


	// workaround to "fix" broken element links in data

	// skip bad taus which have been discarded
	if(pContainer->size()==0)
	  return HLT::OK;

	static SG::AuxElement::Accessor< xAOD::TauJet::JetLink_t > jetAcc( "jetLink" );
	jetAcc( *p_tau ).toPersistent();

	static SG::AuxElement::Accessor< xAOD::TauJet::VertexLink_t > vertexAcc( "vertexLink" );
	vertexAcc( *p_tau ).toPersistent();

	xAOD::TauJet::TauTrackLinks_t new_tauTrackLinks;
	static SG::AuxElement::Accessor< xAOD::TauJet::TauTrackLinks_t > tauTrackAcc( "tauTrackLinks" );
	static SG::AuxElement::Accessor< xAOD::TauTrack::TrackParticleLinks_t > trackAcc( "trackLinks" );

	for( ElementLink< xAOD::TauTrackContainer > tautrack_link : tauTrackAcc( *p_tau ) ) {

	  xAOD::TauTrack::TrackParticleLinks_t new_trackLinks;

	  for( ElementLink< xAOD::TrackParticleContainer > track_link : trackAcc( *(*tautrack_link) ) ) {
	    track_link.toPersistent();

	    ElementLink< xAOD::TrackParticleContainer > newTrackLink;
	    newTrackLink.toPersistent();
	    newTrackLink.resetWithKeyAndIndex( track_link.persKey(), track_link.persIndex() );
	    new_trackLinks.push_back(newTrackLink);	    
	  }

	  pTrackContainer->at(tautrack_link.index())->clearTrackLinks();
	  pTrackContainer->at(tautrack_link.index())->setTrackLinks(new_trackLinks);

	  tautrack_link.toPersistent();

	  ElementLink< xAOD::TauTrackContainer > newTauTrackLink;
	  newTauTrackLink.toPersistent();
	  newTauTrackLink.resetWithKeyAndIndex( tautrack_link.persKey(), tautrack_link.persIndex() );
	  new_tauTrackLinks.push_back(newTauTrackLink);
	}

	p_tau->clearTauTrackLinks();
	p_tau->setAllTauTrackLinks(new_tauTrackLinks);

	// no action needs to be taken for tau -> clusters links
	
	// set status of TE to always true for FE algorithms
	return HLT::OK;
}

void TrigTauRecMerged::setEmptyTauTrack(xAOD::TauJet* &pTau,
                                       xAOD::TauTrackContainer* &tauTrackContainer)
{
  // Make a new tau track, add to container
  xAOD::TauTrack* pTrack = new xAOD::TauTrack();
  tauTrackContainer->push_back(pTrack);

  // Create an element link for that track
  ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
  linkToTauTrack.toContainedElement(*tauTrackContainer, pTrack);
  pTau->addTauTrackLink(linkToTauTrack);
}

