/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TrigTauRec/TrigTauRecMerged.h"


using namespace std;


// Invokes base class constructor.
TrigTauRecMerged::TrigTauRecMerged(const std::string& name,ISvcLocator* pSvcLocator):
		  HLT::FexAlgo(name, pSvcLocator),
		  m_tools(this),
		  m_endtools(this),
		  m_lumiTool("LuminosityTool"),
		  m_maxeta( 2.5 ),
		  m_minpt( 10000 ),
		  m_trkcone( 0.2 )
{

	// The following properties can be specified at run-time
	// (declared in jobOptions file)

	/** Maximal eta value for the seeds to be accepted */
	declareProperty("maxeta",m_maxeta,"max eta for tau");

	/** minimal pt of a seed */
	declareProperty("minpt",m_minpt,"min pt for tau");

	/** list of tool names to be invoked */
	declareProperty( "Tools", m_tools, "List of TauToolBase tools" );

	/** list of end tool names to be invoked */
	declareProperty( "EndTools", m_endtools, "List of End TauToolBase tools" );

	/** cone for trk seed  */
	declareProperty("trkcone",m_trkcone,"max distance track seed from roi center");

	declareProperty("LuminosityTool", m_lumiTool, "Luminosity Tool");

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

	/** Number tau candidates */
	declareMonitoredVariable("EF_nCand",m_Ncand);

	/** Eta of L1 RoI */
	declareMonitoredVariable("EtaL1",m_EtaL1);

	/** Phi of L1 RoI */
	declareMonitoredVariable("PhiL1",m_PhiL1);

	/** Eta of L1 RoI */
	declareMonitoredVariable("EtaEF",m_EtaEF);

	/** Phi of L1 RoI */
	declareMonitoredVariable("PhiEF",m_PhiEF);

	/** Errors */
	declareMonitoredStdContainer("EF_calo_errors",m_calo_errors);

	/** Errors */
	declareMonitoredStdContainer("EF_track_errors",m_track_errors);

	/** Author */
	declareMonitoredStdContainer("EF_author",m_author);

	/** deltaZ0 core Trks*/
	declareMonitoredStdContainer("EF_deltaZ0coreTrks",m_deltaZ0coreTrks);

	/** deltaZ0 wide Trks*/
	declareMonitoredStdContainer("EF_deltaZ0wideTrks",m_deltaZ0wideTrks);

}

TrigTauRecMerged::~TrigTauRecMerged()
{ 
}

HLT::ErrorCode TrigTauRecMerged::hltInitialize()
{
	msg() << MSG::INFO << "TrigTauRecMerged::initialize()" << endreq;

	////////////////////
	// Allocate Tools //
	////////////////////
	// check tool names
	if ( m_tools.begin() == m_tools.end() ) {
		msg() << MSG::ERROR << " no tools given for this algorithm." << endreq;
		return HLT::BAD_JOB_SETUP;
	}

	// find tools
	//-------------------------------------------------------------------------
	ToolHandleArray<TauToolBase> ::iterator p_itT = m_tools.begin();
	ToolHandleArray<TauToolBase> ::iterator p_itTE = m_tools.end();
	msg() << MSG::INFO << "List of tools in execution sequence:" << endreq;
	msg() << MSG::INFO << "------------------------------------" << endreq;

	for(; p_itT != p_itTE; ++p_itT ) {
		StatusCode p_sc = p_itT->retrieve();
		if( p_sc.isFailure() ) {
			msg() << MSG::WARNING << "Cannot find tool named <";
			msg() << *p_itT << ">" << endreq;
			return HLT::BAD_JOB_SETUP;
		}
		else {
			msg() << MSG::INFO << "REGTEST ";
			msg() <<" add timer for tool "<< ( *p_itT )->type() <<" "<< ( *p_itT )->name() << endreq;
			if(  doTiming() ) m_mytimers.push_back(addTimer((*p_itT)->name())) ;
		}
	}

	msg() << MSG::INFO << " " << endreq;
	msg() << MSG::INFO << "------------------------------------" << endreq;

	ToolHandleArray<TauToolBase> ::iterator p_itTe = m_endtools.begin();
	ToolHandleArray<TauToolBase> ::iterator p_itTEe = m_endtools.end();

	msg() << MSG::INFO << "List of end tools in execution sequence:" << endreq;
	msg() << MSG::INFO << "------------------------------------" << endreq;

	for(; p_itTe != p_itTEe; ++p_itTe ) {
		StatusCode p_sc = p_itTe->retrieve();
		if( p_sc.isFailure() ) {
			msg() << MSG::WARNING << "Cannot find tool named <";
			msg() << *p_itTe << ">" << endreq;
			return HLT::BAD_JOB_SETUP;
		}
		else {
			msg() << MSG::INFO << "REGTEST ";
			msg() <<" add time for end tool "<< ( *p_itTe )->type() <<" "<< ( *p_itTe )->name() << endreq;
			if(  doTiming() ) m_mytimers.push_back(addTimer((*p_itTe)->name())) ;
		}
	}

	// For now, we don't try to retrieve the lumi tool
	//if (m_lumiTool.retrieve().isFailure()) {                                     
	//msg() << MSG::ERROR << "Unable to retrieve Luminosity Tool" << endreq;     
	//return HLT::ERROR;                                                         
	//} else {                                                                     
	//msg() << MSG::DEBUG << "Successfully retrieved Luminosity Tool" << endreq; 
	//}                                                                            
	
	msg() << MSG::INFO << " " << endreq;
	msg() << MSG::INFO << "------------------------------------" << endreq;

	return HLT::OK;
}

/////////////////////////////////////////////////////////////////
HLT::ErrorCode TrigTauRecMerged::hltFinalize()
{
	//msg() << MSG::DEBUG << "Finalizing TrigTauRecMerged" << endreq;
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

	m_centFrac=-10;
	m_nWideTrk=0;
	m_ipSigLeadTrk=-1000;
	m_trFlightPathSig=-10;
	m_massTrkSys=-10;
	m_dRmax=-10.;

	m_PSSFraction=-999;
	m_EMPOverTrkSysP=-999;
	m_ChPiEMEOverCaloEME=-999;

	m_author.clear();
	m_calo_errors.clear();
	m_track_errors.clear();
	m_EtEm =-10.;
	m_EtHad=-10.;

	m_EtaL1 = -99;
	m_PhiL1 = -99;

	m_EtaEF = -99;
	m_PhiEF = -99;

	// Retrieve store.
	if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Executing TrigTauRecMerged" << endreq;

	// Get RoiDescriptor
	const TrigRoiDescriptor* roiDescriptor = 0;
	HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);

	if ( hltStatus==HLT::OK && roiDescriptor!= NULL ) {
		if( msgLvl() <= MSG::DEBUG )
			msg() << MSG::DEBUG << "REGTEST: RoI " << *roiDescriptor<< endreq;
	}
	else {
		msg() <<  MSG::ERROR << "Failed to find RoiDescriptor " << endreq;
		m_calo_errors.push_back(NoROIDescr);
		return hltStatus;
	}

	// get CaloCellContainer
	vector<const CaloCellContainer*> vectorCaloCellContainer;
	hltStatus = getFeatures(inputTE, vectorCaloCellContainer);

	if(hltStatus!=HLT::OK ) {
		msg() << MSG::ERROR << " No CaloCellContainers retrieved for the trigger element" << endreq;
		m_calo_errors.push_back(NoCellCont);
		return hltStatus;
	}

	if (vectorCaloCellContainer.size() < 1) {
		msg() << MSG::ERROR
				<< "Size of vector CaloCell container is not 1. Is"
				<< vectorCaloCellContainer.size()
				<< endreq;
		m_calo_errors.push_back(NoCellCont);
		return HLT::ERROR;
	}

	const CaloCellContainer* RoICaloCellContainer = vectorCaloCellContainer.back();

	if(RoICaloCellContainer != NULL) {
		m_nCells = RoICaloCellContainer->size();

		msg() << MSG::DEBUG
				<< "REGTEST: Size of vector CaloCell container is "
				<< RoICaloCellContainer->size()
				<< endreq;
		if(RoICaloCellContainer->size()==0) {
			msg() << MSG::INFO
					<< "Cannot proceed, size of vector CaloCell container is "
					<< RoICaloCellContainer->size()
					<< endreq;
			m_calo_errors.push_back(EmptyCellCont);
			return HLT::OK;
		}
	}
	else {
		msg() << MSG::ERROR << "no CaloCell container found "<< endreq;
		m_calo_errors.push_back(NoCellCont);
		return HLT::ERROR;
	}

	// Retrieve Calocluster container
	std::vector<const xAOD::CaloClusterContainer*> vectorCaloClusterContainer;
	hltStatus = getFeatures(inputTE, vectorCaloClusterContainer);
  
	if(hltStatus!=HLT::OK ) {
	  msg() << MSG::ERROR << " No CaloClusterContainers retrieved for the trigger element" << endreq;
	  m_calo_errors.push_back(NoClustCont);
	  return hltStatus;
	}

	if (vectorCaloClusterContainer.size() < 1) {
	  msg() << MSG::ERROR
		<< "  CaloCluster container is empty"
		<< endreq;
	  m_calo_errors.push_back(NoClustCont);
	  return HLT::ERROR;
	}
  
	if( msgLvl() <= MSG::DEBUG )
	  msg() << MSG::DEBUG << " CaloCluster container size is " << vectorCaloClusterContainer.size() << endreq;
  
	// Grab the last cluster collection attached
	const xAOD::CaloClusterContainer* RoICaloClusterContainer = vectorCaloClusterContainer.back();

	std::string collKey;
	hltStatus = getStoreGateKey( RoICaloClusterContainer, collKey );

	if(hltStatus!=HLT::OK ) {
		msg() << MSG::ERROR << "Cluster has no key " << endreq;
		m_calo_errors.push_back(NoClustKey);
		return HLT::ERROR;
	}

	if( msgLvl() <= MSG::DEBUG )
		msg() << MSG::DEBUG << " cluster key for back cluster is " << collKey << endreq;

	
	// Not sure why this would be necessary...
	/*
	const INavigable4MomentumCollection*  RoICaloClusterContainer;
	StatusCode sc = store()->retrieve(RoICaloClusterContainer,collKey);

	if(sc.isFailure()) {
		msg() << MSG :: ERROR << "failed to find Cluster container"<< endreq;
		m_calo_errors.push_back(NoClustCont);
		return HLT :: ERROR;
	}

	if(RoICaloClusterContainer != NULL) {
		msg() << MSG::DEBUG
				<< "REGTEST: Size of vector CaloCluster container is "
				<< RoICaloClusterContainer->size()
				<< endreq;
		if(RoICaloClusterContainer->size()==0) {
			msg() << MSG::DEBUG
					<< "Cannot proceed, size of vector CaloCluster container is "
					<< RoICaloClusterContainer->size()
					<< endreq;
			m_calo_errors.push_back(EmptyClustCont);
			return HLT::OK;
		}
	}
	else {
		msg() << MSG::ERROR << "no CaloCluster container found "<< endreq;
		m_calo_errors.push_back(NoClustCont);
		return HLT::ERROR;
	}
	*/
	

	// get TrackContainer
	// xxx ToDo: this needs to be a xAOD container
	vector<const xAOD::TrackParticleContainer*> vectorTrackContainer;
	hltStatus = getFeatures(inputTE,vectorTrackContainer);

	const xAOD::TrackParticleContainer*  RoITrackContainer=NULL;

	if (hltStatus!=HLT::OK) {
		msg() << MSG::INFO << "REGTEST: No Track container found." << endreq;
		m_track_errors.push_back(NoTrkCont);
	}
	else {
		if (vectorTrackContainer.size()<1) {
			msg() << MSG::DEBUG << "Size of vector Track container is not 1. Is " << vectorTrackContainer.size() << endreq;
			m_track_errors.push_back(NoTrkCont);
		}

		if(vectorTrackContainer.size()>0) {
			RoITrackContainer = vectorTrackContainer.back();
			if( msgLvl() <= MSG::DEBUG )
				msg() << MSG::DEBUG << "REGTEST: Size of vector Track container is " << RoITrackContainer->size() << endreq;
		}
		if(RoITrackContainer != NULL) m_nTracks = RoITrackContainer->size();
	}

	// get Vertex Container
	// xxx ToDo: this needs to be a xAOD container
	vector<const xAOD::VertexContainer*> vectorVxContainer;
	hltStatus = getFeatures(inputTE,vectorVxContainer);
	const xAOD::VertexContainer* RoIVxContainer = NULL;

	if(hltStatus!=HLT::OK) {
		msg() << MSG::INFO << "No VxContainers retrieved for the trigger element" << endreq;
		m_track_errors.push_back(NoVtxCont);
	}
	else {
		if (vectorVxContainer.size() < 1) {
			msg() << MSG::DEBUG << "Size of vector Vertex  container is not 1. Is " << vectorVxContainer.size() << endreq;
			m_track_errors.push_back(NoVtxCont);
		}

		if(vectorVxContainer.size() >0) {
			RoIVxContainer = vectorVxContainer.back();
			if( msgLvl() <= MSG::DEBUG )
				msg() << MSG::DEBUG << "REGTEST: Size of vector Vertex  container " << RoIVxContainer->size() << endreq;
		}
	}

	// Eventually: get Online Luminosity 
	//double mu = m_lumiTool->lbLuminosityPerBCID() / m_lumiTool->muToLumi(); // (retrieve mu for the current BCID) 
	//std::cout << "TrigTauRecMerged: mu = " << mu << std::endl;

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
	TLorentzVector myCluster;
  	TLorentzVector TauBarycenter(0., 0., 0., 0.);
  
	xAOD::CaloClusterContainer::const_iterator clusterIt;
	for (clusterIt=RoICaloClusterContainer->begin(); clusterIt != RoICaloClusterContainer->end(); ++clusterIt) {
	  if( msgLvl() <= MSG::DEBUG )
	    msg()<< MSG::DEBUG <<" Cluster (e, eta, phi) : ("<< (*clusterIt)->e() << " , " <<(*clusterIt)->eta()<<" , "<<(*clusterIt)->phi()<< " )" << endreq;
    
	  if((*clusterIt)->e() < 0)
	    {
	      if( msgLvl() <= MSG::DEBUG )
		msg()<< MSG::DEBUG <<" Negative energy cluster is rejected" << endreq;
	      continue;
	    }
    
	  myCluster.SetPtEtaPhiE((*clusterIt)->pt(), (*clusterIt)->eta(), (*clusterIt)->phi(), (*clusterIt)->e());
	  aJet->addConstituent(*clusterIt);

	  TauBarycenter += myCluster;

	}
	
	aJet->setJetP4(xAOD::JetFourMom_t(TauBarycenter.Pt(), TauBarycenter.Eta(), TauBarycenter.Phi(), TauBarycenter.M() ) ); 

	if( msgLvl() <= MSG::DEBUG ) msg() << MSG :: DEBUG << "jet formed"<< aJet->eta() <<" , " << aJet->phi() <<" , " << aJet->pt() << " , "<< aJet->e() << endreq;
	
	std::string tauKey = "";
	hltStatus=recordAndAttachFeature(outputTE, theJetCollection,tauKey,"TrigTauJet");
	
	if (hltStatus!=HLT::OK ) {
	  msg() << MSG::ERROR << "Unable to record JetCollection  in TDS" << endreq;
	  m_calo_errors.push_back(NoJetAttach);
	  return hltStatus;
	}
	else {
	  if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " JetCollection is recorded with key " << "HLT_" << tauKey << "_" << "TrigTauJet" << endreq;
	}


	StatusCode processStatus    = StatusCode::SUCCESS;

	//-------------------------------------------------------------------------
	// setup TauCandidate data
	//-------------------------------------------------------------------------
	TauCandidateData rTauData;
	xAOD::TauJetContainer *pContainer = new xAOD::TauJetContainer();
	xAOD::TauJetAuxContainer pAuxContainer;

	// Set the store: eventually, we want to use a dedicated trigger version
	pContainer->setStore(&pAuxContainer);

	// set TauCandidate properties (xAOD style)
	rTauData.xAODTau = 0;
	rTauData.xAODTauContainer = pContainer;

	// This is potentially a bit dangerous, but all the tools using rTauData
	// are run in the current scope
	rTauData.tauAuxContainer = &pAuxContainer;

	// Set the Objects that we can attach right now
	rTauData.setObject("InTrigger?", true );
	rTauData.setObject("TrackContainer", RoITrackContainer);
	rTauData.setObject("VxPrimaryCandidate", RoIVxContainer);

	//-------------------------------------------------------------------------
	// eventInitialize tauRec colls
	//-------------------------------------------------------------------------
	ToolHandleArray<TauToolBase> ::iterator firstTool = m_tools.begin();
	ToolHandleArray<TauToolBase> ::iterator lastTool  = m_tools.end();
	for ( ; firstTool != lastTool; firstTool++ ) {
		processStatus = (*firstTool)->eventInitialize( &rTauData );

		if( processStatus != StatusCode :: SUCCESS ) {
			msg() << MSG :: ERROR << "tool "<<(*firstTool)->name()<< "failed in eventInitialize" << endreq;
			return HLT :: TOOL_FAILURE;
		}
	}
	if( msgLvl() <= MSG::DEBUG ) msg() << MSG ::DEBUG << " initialize all good " << endreq;

	//-------------------------------------------------------------------------
	// using Jet collection
	// setup tau candidate structure
	//-------------------------------------------------------------------------
	xAOD::Jet* p_seed =    (*theJetCollection->begin());  //there is only one jet stored

	rTauData.xAODTau = new xAOD::TauJet();
	pContainer->push_back(rTauData.xAODTau);
	rTauData.xAODTau->setROIWord(roiDescriptor->roiWord());
	rTauData.seed = p_seed;
	rTauData.seedContainer = theJetCollection;

	if(p_seed->e()<=0) {
		msg() << MSG::DEBUG << " Roi: changing eta due to energy " << p_seed->e() << endreq;
		//rTauData.xAODTau->setEta(roiDescriptor->eta0());
		//rTauData.xAODTau->setPhi(roiDescriptor->phi0());
		// Direct accessors not avilable anymore
		rTauData.xAODTau->setP4(rTauData.xAODTau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), rTauData.xAODTau->m());
		
		msg() << MSG::DEBUG << "Roi: " << roiDescriptor->roiId()
        		  << " Tau eta: " << rTauData.xAODTau->eta() << " Tau phi: " << rTauData.xAODTau->phi()
        		  << endreq;
	}

	if( msgLvl() <= MSG::DEBUG ) msg() << MSG ::DEBUG <<" roidescriptor roiword " << roiDescriptor->roiWord() << " saved " << rTauData.xAODTau->ROIWord() << endreq;

	rTauData.setObject("JetCollection", theJetCollection );

	//-------------------------------------------------------------------------
	// loop over booked tau tools
	//-------------------------------------------------------------------------
	int toolnum = 0;
	firstTool = m_tools.begin();
	lastTool  = m_tools.end();
	processStatus    = StatusCode::SUCCESS;

	if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Starting tool loop with seed jet" << endreq;
	std::vector<TrigTimer* >::iterator itimer =  m_mytimers.begin();
	while ( ! processStatus.isFailure() && firstTool != lastTool ) {
		// loop stops only when Failure indicated by one of the tools
		if( msgLvl() <= MSG::DEBUG ) {
			msg() << MSG::DEBUG << "Starting Tool: " << endreq;
			msg() << MSG::DEBUG <<  (*firstTool)->name() << endreq;
		}
		// time in the various tools
		++toolnum;
		if ( doTiming() && itimer != m_mytimers.end() ) (*itimer)->start();

		processStatus = (*firstTool)->execute( &rTauData);
		if ( !processStatus.isFailure() ) {
			if( msgLvl() <= MSG::DEBUG ) {
				msg() << MSG::DEBUG << "REGTEST: "<< (*firstTool)->name() << " executed successfully " << endreq;
				msg() << MSG::DEBUG << "REGTEST: Roi: " << roiDescriptor->roiId()
            						<< " Tau eta: " << rTauData.xAODTau->eta() << " Tau phi: " << rTauData.xAODTau->phi()
            						<< " Tau pT : "<< rTauData.xAODTau->pt()<< endreq;
			}
		}
		else {
			if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<"REGTEST: "<< (*firstTool)->name() << " execution failed " << endreq;
		}

		++firstTool;
		++itimer;
		if ( doTiming() && itimer != m_mytimers.end() ) (*itimer)->stop();
	}

	//check status
	if ( !processStatus.isSuccess() )  {   // some problem
		if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "the tau object has NOT been registered in the tau container" << endreq;

		ToolHandleArray<TauToolBase> ::iterator tool = m_tools.begin();
		for(; tool != firstTool; ++tool ) (*tool)->cleanup( &rTauData );
		(*tool)->cleanup( &rTauData );

		pContainer->pop_back();
		rTauData.xAODTau = 0;

		if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "clean up done after jet seed" << endreq;
	}
	else if( processStatus.isSuccess()) {
	  // if this is a tau candidate.. save it
	  // To-do: will need to solve this, as it's a bare minimum already
	  //rTauData.xAODTauContainer->push_back( rTauData.xAODTau );

		float fJetEnergy = (*rTauData.xAODTau->jetLink())->e();
		msg() << MSG::DEBUG << " Roi: jet e "<< fJetEnergy <<endreq;

		if( fJetEnergy < 0.00001 ) {
			msg() << MSG::DEBUG << " Roi: changing eta phi to L1 ones due to energy negative (PxPyPzE flips eta and phi)"<<endreq;
			msg() << MSG::DEBUG << " Roi: this is probably not needed anymore, method PxPyPzE has been corrected"<<endreq;

			//rTauData.xAODTau->setEta(roiDescriptor->eta0());
			//rTauData.xAODTau->setPhi(roiDescriptor->phi0());
			// Direct accessors not available anymore
			rTauData.xAODTau->setP4(rTauData.xAODTau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), rTauData.xAODTau->m());

			msg() << MSG::DEBUG << " Roi: " << roiDescriptor->roiId()
            				    << " Tau eta: " << rTauData.xAODTau->eta()
            				    << " Tau phi: " << rTauData.xAODTau->phi()
            				    << " Tau pT : "<< rTauData.xAODTau->pt()<< endreq;
		}

		// loop over end tools
		ToolHandleArray<TauToolBase> ::iterator p_itET = m_endtools.begin();
		ToolHandleArray<TauToolBase> ::iterator p_itETE = m_endtools.end();
		for (; p_itET != p_itETE; ++p_itET ) {
			msg() << MSG::VERBOSE << "Invoking endTool ";
			msg() << ( *p_itET )->name() << endreq;

			processStatus = ( *p_itET )->execute( &rTauData );
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
		m_numTrack      = rTauData.xAODTau->nTracks();
		m_nWideTrk      = rTauData.xAODTau->nWideTracks();
		rTauData.xAODTau->detail(xAOD::TauJetParameters::trkAvgDist, m_trkAvgDist);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, m_etovPtLead);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::EMRadius, m_emRadius);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::hadRadius, m_hadRadius);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::isolFrac, m_IsoFrac);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::centFrac, m_centFrac);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::ipSigLeadTrk, m_ipSigLeadTrk);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::trFlightPathSig, m_trFlightPathSig);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::dRmax, m_dRmax);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::massTrkSys, m_massTrkSys);

		rTauData.xAODTau->detail(xAOD::TauJetParameters::PSSFraction, m_PSSFraction);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, m_EMPOverTrkSysP);
		rTauData.xAODTau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, m_ChPiEMEOverCaloEME);

		m_massTrkSys /= 1000.; // make GeV
		rTauData.xAODTau->detail(xAOD::TauJetParameters::etEMAtEMScale, m_EtEm);
		m_EtEm /= 1000.;  // make GeV
		rTauData.xAODTau->detail(xAOD::TauJetParameters::etHadAtEMScale, m_EtHad);
		m_EtHad /= 1000.; // make GeV
		m_Et            = m_EtEm + m_EtHad;
		m_EtFinal       = rTauData.xAODTau->pt()/1000.;

		m_EtaEF = rTauData.xAODTau->eta();
		m_PhiEF = rTauData.xAODTau->phi();

		if( m_Et !=0) m_EMFrac =  m_EtEm/ m_Et ;

		m_dEta =  m_EtaEF - roiDescriptor->eta();
		m_dPhi =  m_PhiEF - roiDescriptor->phi();
		if(m_dPhi<-M_PI) m_dPhi += 2.0*M_PI;
		if(m_dPhi>M_PI)  m_dPhi -= 2.0*M_PI;

		// author variable removed. There are no different tau reco algs anymor

		// write out delta Z0
		/*
		 * FF, March 2014: deactivated.
		 * If the output of these variables is still needed, drop me a line.
		 * We can either print them here as done before or put them into tauEDM to have them available globally
		if (m_useTauPVTool) {
			m_tauPVTool->getDeltaZ0Values(m_deltaZ0coreTrks, m_deltaZ0wideTrks);

			msg() << MSG::DEBUG << "REGTEST: deltaZ0 for core trk ";
			for ( unsigned int i=0; i<m_deltaZ0coreTrks.size(); ++i) msg() << MSG::DEBUG << i << ": " << m_deltaZ0coreTrks[i] << ", ";
			msg() << MSG::DEBUG << endreq;

			msg() << MSG::DEBUG << "REGTEST: deltaZ0 for wide trk ";
			for ( unsigned int i=0; i<m_deltaZ0wideTrks.size(); ++i) msg() << MSG::DEBUG << i << ": " << m_deltaZ0wideTrks[i] << ", ";
			msg() << MSG::DEBUG << endreq;
		}
		*/

		msg() << MSG::DEBUG << "REGTEST: Roi: " << roiDescriptor->roiId()
        					<< " Tau being saved eta: " << m_EtaEF << " Tau phi: " << m_PhiEF
        					<< " wrt L1 dEta "<< m_dEta<<" dPhi "<<m_dPhi
        					<< " Tau Et (GeV): "<< m_EtFinal << endreq;

		++m_Ncand;
	}
	else {
		pContainer->pop_back();
		rTauData.xAODTau = 0;

		if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "deleted tau done after jet seed" << endreq;
	}

	// call eventFinalize on the booked tau tools
	for ( firstTool = m_tools.begin(); firstTool != lastTool; firstTool++ ) {
		processStatus = (*firstTool)->eventFinalize( &rTauData );
		if( processStatus != StatusCode :: SUCCESS ) {
			msg() << MSG :: INFO << "tool "<<(*firstTool)->name()<< "failed in eventFinalize" << endreq;
			return HLT :: TOOL_FAILURE;
		}
	}
	msg() << MSG :: DEBUG << "tools succeed in eventFinalize" << endreq;


	/* xxx ToDo: is this needed for trigger??
	 *
	Analysis::TauJetContainer::iterator tau_it = Tau_data.tauContainer->begin();
	Analysis::TauJetContainer::iterator tau_end = Tau_data.tauContainer->end();
	for( ; tau_it != tau_end; tau_it++ ) {
		(*tau_it)->cellClusterLink().reset();
		delete (*tau_it)->cellCluster();

		const Analysis::TauCommonDetails*  taudetConst = (*tau_it)->details<const Analysis::TauCommonDetails>();
		Analysis::TauCommonDetails*  taudet = const_cast< Analysis::TauCommonDetails*> (taudetConst);
		if(taudet!=NULL) delete taudet->cellEM012Cluster();
		if(taudet!=NULL) taudet->cellEM012ClusterLink().reset();
	}
	*/

	//-------------------------------------------------------------------------
	// all done, register the tau Container in TDS.
	//-------------------------------------------------------------------------
	// xxx ToDo: check recording. The new container are of type xAOD. Can HLT handle this?
	tauKey = "";
	hltStatus=recordAndAttachFeature(outputTE, pContainer,tauKey,"TrigTauRecMerged");
	if (hltStatus!=HLT::OK )  {
		msg() << MSG::ERROR << "Unable to record tau Container in TDS" << endreq;
		m_calo_errors.push_back(NoHLTtauAttach);
		return hltStatus;
	}
	else {
		if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Recorded a tau container: " << "HLT_" <<tauKey << "_" << "TrigTauRecMerged" << endreq;
	}

	/*
	tauKey = "";
	hltStatus=recordAndAttachFeature(outputTE, pAuxContainer,tauKey,"TrigTauDetailsMerged");
	if (hltStatus!=HLT::OK){
		msg() << MSG::ERROR << "Unable to record tau details Container in TDS" << endreq;
		m_calo_errors.push_back(NoHLTtauDetAttach);
		return hltStatus;
	}
	else {
		if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Recorded a tau details container: " << "HLT_" <<tauKey << "_" << "TrigTauDetailsMerged" << endreq;
	}
	*/
	if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "the tau object has been registered in the tau container" << endreq;

	// set status of TE to always true for FE algorithms
	return HLT::OK;
}

