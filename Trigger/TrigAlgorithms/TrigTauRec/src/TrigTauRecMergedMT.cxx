/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigTauRecMergedMT.cxx
 * PACKAGE:  Trigger/TrigAlgorithms/TrigTauRecMerged    
 *
 * AUTHOR:   E.Carquin
 * MODIFIED: Nov 23 2018
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

#include "TrigTauRecMergedMT.h"
#include "AthenaMonitoring/MonitoredScope.h"


using namespace std;


// Invokes base class constructor.
TrigTauRecMergedMT::TrigTauRecMergedMT(const std::string& name,ISvcLocator* pSvcLocator)
  :AthAlgorithm(name, pSvcLocator),
		  m_tools(this),
		  m_endtools(this),
		  m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
		  m_beamSpotSvc("BeamCondSvc", name)
{
  declareProperty("Tools", m_tools, "List of ITauToolBase tools" );
  declareProperty("EndTools", m_endtools, "List of End ITauToolBase tools" );
  declareProperty("LumiBlockMuTool", m_lumiBlockMuTool, "Luminosity Tool" );
}

TrigTauRecMergedMT::~TrigTauRecMergedMT()
{ 
}

StatusCode TrigTauRecMergedMT::initialize()
{
   ATH_MSG_DEBUG("TrigTauRecMergedMT::initialize()");

   m_tauEventData.setInTrigger(true);
   if ( m_tools.begin() == m_tools.end() ) {
      ATH_MSG_DEBUG(" no tools given for this algorithm.");
      return StatusCode::FAILURE;
   }

   ToolHandleArray<ITauToolBase> ::iterator p_itT = m_tools.begin();
   ToolHandleArray<ITauToolBase> ::iterator p_itTE = m_tools.end();
   ATH_MSG_DEBUG("List of tools in execution sequence:");

   for(; p_itT != p_itTE; ++p_itT ) {
      StatusCode p_sc = p_itT->retrieve();
      if( p_sc.isFailure() ) {
         ATH_MSG_DEBUG("Cannot find tool named <" << *p_itT << ">");
         return StatusCode::FAILURE;
      }
      else {
         ATH_MSG_DEBUG("Add timer for tool "<< ( *p_itT )->type() <<" "<< ( *p_itT )->name());
         //if(  doTiming() ) m_mytimers.push_back(addTimer((*p_itT)->name())) ;
         (*p_itT)->setTauEventData(&m_tauEventData);
      }
   }

   ToolHandleArray<ITauToolBase> ::iterator p_itTe = m_endtools.begin();
   ToolHandleArray<ITauToolBase> ::iterator p_itTEe = m_endtools.end();

   for(; p_itTe != p_itTEe; ++p_itTe ) {
      StatusCode p_sc = p_itTe->retrieve();
      if( p_sc.isFailure() ) {
         ATH_MSG_DEBUG("Cannot find tool named <" << *p_itTe << ">");
         return StatusCode::FAILURE;
      }
      else {
         ATH_MSG_DEBUG(" Add time for end tool "<< ( *p_itTe )->type() <<" "<< ( *p_itTe )->name());
         //if(  doTiming() ) m_mytimers.push_back(addTimer((*p_itTe)->name())) ;
         ( *p_itTe )->setTauEventData(&m_tauEventData);
      }
   }

   if (m_lumiBlockMuTool.retrieve().isFailure()) {
     ATH_MSG_WARNING("Unable to retrieve LumiBlockMuTool");
   } else {
     ATH_MSG_DEBUG("Successfully retrieved LumiBlockMuTool");
   }

   // Retrieve beam conditions
   if(m_beamSpotSvc.retrieve().isFailure()) {
     ATH_MSG_WARNING("Unable to retrieve Beamspot service");
        } else {
          ATH_MSG_WARNING("Successfully retrieved Beamspot service");
   }

   if ( not m_monTool.name().empty() ) {
     ATH_CHECK( m_monTool.retrieve() );
   }

   ATH_MSG_DEBUG( "Initialising HandleKeys" );
   CHECK( m_roIInputKey.initialize()        );
   CHECK( m_L1RoIKey.initialize()           );
   CHECK( m_clustersKey.initialize()        );
   CHECK( m_tracksKey.initialize()          );
   CHECK( m_vertexKey.initialize()          );
   CHECK( m_trigTauJetKey.initialize()      );
   CHECK( m_trigtauSeedOutKey.initialize()  );
   CHECK( m_trigtauRecOutKey.initialize()   );
   CHECK( m_trigtauTrkOutKey.initialize()   );

   return StatusCode::SUCCESS;
}

StatusCode TrigTauRecMergedMT::finalize()
{
   return StatusCode::SUCCESS;
}

StatusCode TrigTauRecMergedMT::execute()
{

   ATH_MSG_DEBUG("Execution");

   auto ctx = getContext();

   using namespace Monitored;
   // variables to initialize and keep values for monitoring variables
   std::vector<unsigned char> calo_errors(0);
   std::vector<unsigned char> track_errors(0);
   std::vector<unsigned char> author(0);
   std::vector<float> deltaZ0coreTrks(0);
   std::vector<float> deltaZ0wideTrks(0);

   auto nCells             = MonitoredScalar::declare<int>("nRoI_EFTauCells",    0);
   auto nTracks            = MonitoredScalar::declare<int>("nRoI_EFTauTracks", -10);
   auto dEta               = MonitoredScalar::declare("dEtaEFTau_RoI",    -10);
   auto dPhi               = MonitoredScalar::declare("dPhiEFTau_RoI",    -10);
   auto emRadius           = MonitoredScalar::declare<float>("EF_EMRadius", -0.099);
   auto hadRadius          = MonitoredScalar::declare<float>("EF_EMRadius", -0.099);
   auto EtFinal            = MonitoredScalar::declare<float>("EF_EtFinal", 0.);
   auto Et                 = MonitoredScalar::declare<float>("EF_Et", 0.);
   auto EtHad              = MonitoredScalar::declare<float>("EF_EtHad",-10.);
   auto EtEm               = MonitoredScalar::declare<float>("EF_EtEm",-10.);
   auto EMFrac             = MonitoredScalar::declare("EF_EMFrac",-10.);
   auto IsoFrac            = MonitoredScalar::declare<float>("EF_IsoFrac",-1.);
   auto centFrac           = MonitoredScalar::declare("EF_centFrac",-10);
   auto nWideTrk           = MonitoredScalar::declare<int>("EF_nWideTrk",0);
   auto ipSigLeadTrk       = MonitoredScalar::declare("EF_ipSigLeadTrk",-1000);
   auto trFlightPathSig    = MonitoredScalar::declare("EF_trFlightPathSig",-10);
   auto massTrkSys         = MonitoredScalar::declare("EF_massTrkSys",-10);
   auto dRmax              = MonitoredScalar::declare<float>("EF_dRmax",-10.);
   auto numTrack           = MonitoredScalar::declare<int>("EF_NTrk", -10);
   auto trkAvgDist         = MonitoredScalar::declare<float>("EF_TrkAvgDist",-1.0);
   auto etovPtLead         = MonitoredScalar::declare<float>("EF_EtovPtLead",-10.);
   auto PSSFraction        = MonitoredScalar::declare("EF_PSSFraction",-999);
   auto EMPOverTrkSysP     = MonitoredScalar::declare("EF_EMPOverTrkSysP",-999);
   auto ChPiEMEOverCaloEME = MonitoredScalar::declare("EF_ChPiEMEOverCaloEME",-999);
   auto SumPtTrkFrac       = MonitoredScalar::declare("EF_SumPtTrkFrac",-999);
   auto innerTrkAvgDist    = MonitoredScalar::declare<float>("EF_innerTrkAvgDist",-1.0);
   auto Ncand              = MonitoredScalar::declare<int>("EF_nCand",0);
   auto ActualInteractions = MonitoredScalar::declare("EF_ActualInteractions",-999.9);
   auto AvgInteractions    = MonitoredScalar::declare("EF_AvgInteractions",-999.9);
   auto beamspot_x         = MonitoredScalar::declare("EF_beamspot_x",-999.9);
   auto beamspot_y         = MonitoredScalar::declare("EF_beamspot_y",-999.9);
   auto beamspot_z         = MonitoredScalar::declare("EF_beamspot_z",-999.9);
   auto EtaL1                 = MonitoredScalar::declare("EtaL1",-99);
   auto PhiL1                 = MonitoredScalar::declare("PhiL1",-99);
   auto EtaEF                 = MonitoredScalar::declare("EtaEF",-99);
   auto PhiEF                 = MonitoredScalar::declare("PhiEF",-99);

   auto EF_calo_errors        = MonitoredCollection::declare("calo_errors",calo_errors);
   auto EF_track_errors       = MonitoredCollection::declare("track_errors",track_errors);
   auto EF_author             = MonitoredCollection::declare("author",author);
   auto EF_deltaZ0coreTrks    = MonitoredCollection::declare("deltaZ0coreTrks",deltaZ0coreTrks);
   auto EF_deltaZ0wideTrks    = MonitoredCollection::declare("deltaZ0wideTrks",deltaZ0wideTrks);

   auto monitorIt = MonitoredScope::declare( m_monTool, nCells, nTracks, dEta, dPhi, emRadius, hadRadius,
   EtFinal, Et, EtHad, EtEm, EMFrac, IsoFrac, centFrac, nWideTrk, ipSigLeadTrk, trFlightPathSig, massTrkSys,
   dRmax, numTrack, trkAvgDist, etovPtLead, PSSFraction, EMPOverTrkSysP, ChPiEMEOverCaloEME, SumPtTrkFrac,
   innerTrkAvgDist, Ncand, ActualInteractions, AvgInteractions, beamspot_x, beamspot_y, beamspot_z, EtaL1,
   PhiL1, EtaEF, PhiEF );

	// Retrieve store.
	ATH_MSG_DEBUG("Executing TrigTauRecMergedMT");

	// Get RoiDescriptor
   SG::ReadHandle< TrigRoiDescriptorCollection > roisHandle = SG::makeHandle( m_roIInputKey, ctx );
   if(roisHandle->size()==0){
     ATH_MSG_DEBUG("RoIHandle size = Zero");
     return StatusCode::SUCCESS;
   }
   const TrigRoiDescriptor *roiDescriptor = roisHandle->at(0);

	if ( roiDescriptor!= NULL ) {
		ATH_MSG_DEBUG("REGTEST: RoI " << *roiDescriptor);
	}
	else {
		ATH_MSG_DEBUG("Failed to find RoiDescriptor ");
		calo_errors.push_back(NoROIDescr);
		return StatusCode::SUCCESS;
	}

	// get CaloCellContainer
	// Probably not necessary
	/*vector<const CaloCellContainer*> vectorCaloCellContainer;
	hltStatus = getFeatures(inputTE, vectorCaloCellContainer);

	if(hltStatus!=HLT::OK ) {
		msg() << MSG::ERROR << " No CaloCellContainers retrieved for the trigger element" << endmsg;
		m_calo_errors.push_back(NoCellCont);
		return hltStatus;
	}

	if (vectorCaloCellContainer.size() < 1) {
		msg() << MSG::ERROR
				<< "Size of vector CaloCell container is not 1. Is"
				<< vectorCaloCellContainer.size()
				<< endmsg;
		m_calo_errors.push_back(NoCellCont);
		return HLT::ERROR;
	}

	const CaloCellContainer* RoICaloCellContainer = vectorCaloCellContainer.back();

	if(RoICaloCellContainer != NULL) {
		m_nCells = RoICaloCellContainer->size();

		msg() << MSG::DEBUG
				<< "REGTEST: Size of vector CaloCell container is "
				<< RoICaloCellContainer->size()
				<< endmsg;
		if(RoICaloCellContainer->size()==0) {
			msg() << MSG::INFO
					<< "Cannot proceed, size of vector CaloCell container is "
					<< RoICaloCellContainer->size()
					<< endmsg;
			m_calo_errors.push_back(EmptyCellCont);
			return HLT::OK;
		}
	}
	else {
		msg() << MSG::ERROR << "no CaloCell container found "<< endmsg;
		m_calo_errors.push_back(NoCellCont);
		return HLT::ERROR;
	}*/

	// Retrieve Calocluster container
   SG::ReadHandle< xAOD::CaloClusterContainer > CCContainerHandle = SG::makeHandle( m_clustersKey,ctx );
   CHECK( CCContainerHandle.isValid() );

   const xAOD::CaloClusterContainer *RoICaloClusterContainer = CCContainerHandle.get();
   ATH_MSG_DEBUG( "Found " << RoICaloClusterContainer->size() << " caloClusters, updating the corresponding RoI ... " );

   if(RoICaloClusterContainer != NULL) {
     ATH_MSG_DEBUG( "REGTEST: Size of vector CaloCluster container is " << RoICaloClusterContainer->size());
     if(RoICaloClusterContainer->size()==0) {
       ATH_MSG_DEBUG( "Cannot proceed, size of vector CaloCluster container is " << RoICaloClusterContainer->size());
       return StatusCode::SUCCESS;
     }
   }else {
     ATH_MSG_DEBUG( "no CaloCluster container found " );
     return StatusCode::SUCCESS;
   }

	/*if(hltStatus!=HLT::OK ) {
	  msg() << MSG::ERROR << " No CaloClusterContainers retrieved for the trigger element" << endmsg;
	  m_calo_errors.push_back(NoClustCont);
	  return hltStatus;
	}

	if (vectorCaloClusterContainer.size() < 1) {
	  msg() << MSG::ERROR
		<< "  CaloCluster container is empty"
		<< endmsg;
	  m_calo_errors.push_back(NoClustCont);
	  return HLT::ERROR;
	}
  
	if( msgLvl() <= MSG::DEBUG )
	  msg() << MSG::DEBUG << " CaloCluster container size is " << vectorCaloClusterContainer.size() << endmsg;
  
	// Grab the last cluster collection attached
	const xAOD::CaloClusterContainer* RoICaloClusterContainer = vectorCaloClusterContainer.back();

	std::string collKey;
	hltStatus = getStoreGateKey( RoICaloClusterContainer, collKey );

	if(hltStatus!=HLT::OK ) {
		msg() << MSG::ERROR << "Cluster has no key " << endmsg;
		m_calo_errors.push_back(NoClustKey);
		return HLT::ERROR;
	}

	if( msgLvl() <= MSG::DEBUG )
		msg() << MSG::DEBUG << " cluster key for back cluster is " << collKey << endmsg;

	*/
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
   SG::ReadHandle< xAOD::TrackParticleContainer > TPContainerHandle = SG::makeHandle( m_tracksKey,ctx );

   const xAOD::TrackParticleContainer *RoITrackParticleContainer = NULL;

	if ( !TPContainerHandle.isValid() ) {
		ATH_MSG_INFO("REGTEST: No Track container found.");
		track_errors.push_back(NoTrkCont);
	}
	else {
      RoITrackParticleContainer=TPContainerHandle.get();
		ATH_MSG_DEBUG("REGTEST: Size of vector Track container is " << RoITrackParticleContainer->size());
		if(RoITrackParticleContainer != NULL) nTracks = RoITrackParticleContainer->size();
	}

	// get Vertex Container
   SG::ReadHandle< xAOD::VertexContainer > VertexContainerHandle = SG::makeHandle( m_vertexKey,ctx );

	const xAOD::VertexContainer* RoIVxContainer = NULL;

	if( !VertexContainerHandle.isValid() ) {
		ATH_MSG_INFO(" No VxContainers retrieved for the trigger element");
		track_errors.push_back(NoVtxCont);
	}
	else {
			RoIVxContainer = VertexContainerHandle.get();
			ATH_MSG_DEBUG("REGTEST: Size of vector Vertex  container " << RoIVxContainer->size());
	}

	double mu = 0.0;
   double avg_mu = 0.0;
	if(m_lumiBlockMuTool){
	  mu     = m_lumiBlockMuTool->actualInteractionsPerCrossing(); // (retrieve mu for the current BCID)
	  avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
     ActualInteractions = mu;
     AvgInteractions    = avg_mu;
	  ATH_MSG_DEBUG("REGTEST: Retrieved Mu Value : " << mu);
	  ATH_MSG_DEBUG("REGTEST: Average Mu Value   : " << avg_mu);
	}
	

	//-------------------------------------------------------------------------
	// Get beamspot
	//-------------------------------------------------------------------------

	// Copy the first vertex from a const object
	xAOD::Vertex theBeamspot;
	theBeamspot.makePrivateStore();
	const xAOD::Vertex* ptrBeamspot = 0;

	if(m_beamSpotSvc){
	
	  // Alter the position of the vertex
	  theBeamspot.setPosition(m_beamSpotSvc->beamPos());
	
	  beamspot_x=theBeamspot.x();
	  beamspot_y=theBeamspot.y();
	  beamspot_z=theBeamspot.z();

	  // Create a AmgSymMatrix to alter the vertex covariance mat.
	  AmgSymMatrix(3) cov = m_beamSpotSvc->beamVtx().covariancePosition();
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
	  ATH_MSG_DEBUG(" Cluster (e, eta, phi) : ("<< (*clusterIt)->e() << " , " <<(*clusterIt)->eta()<<" , "<<(*clusterIt)->phi()<< " )");
    
	  if((*clusterIt)->e() < 0)
	    {
	      ATH_MSG_DEBUG(" Negative energy cluster is rejected");
	      continue;
	    }
    
	  myCluster.SetPtEtaPhiE((*clusterIt)->pt(), (*clusterIt)->eta(), (*clusterIt)->phi(), (*clusterIt)->e());
	  aJet->addConstituent(*clusterIt);

	  TauBarycenter += myCluster;
	}
	
	aJet->setJetP4(xAOD::JetFourMom_t(TauBarycenter.Pt(), TauBarycenter.Eta(), TauBarycenter.Phi(), TauBarycenter.M() ) ); 

	ATH_MSG_DEBUG("jet formed"<< aJet->eta() <<" , " << aJet->phi() <<" , " << aJet->pt() << " , "<< aJet->e() );

   // Prepare Outputs
   std::unique_ptr< xAOD::JetContainer > tauSeedContainer( new xAOD::JetContainer() );

   // Save Outputs
   SG::WriteHandle< xAOD::JetContainer > outTauSeedHandle = SG::makeHandle( m_trigtauSeedOutKey,ctx );
   CHECK( outTauSeedHandle.record( std::move( tauSeedContainer ) ) );

	StatusCode processStatus    = StatusCode::SUCCESS;

	//-------------------------------------------------------------------------
	// setup TauCandidate data
	//-------------------------------------------------------------------------
	m_tauEventData.clear();
	xAOD::TauJetContainer *pContainer = new xAOD::TauJetContainer();
	xAOD::TauJetAuxContainer pAuxContainer;

	xAOD::TauTrackContainer *pTrackContainer = new xAOD::TauTrackContainer();
	xAOD::TauTrackAuxContainer pTrackAuxContainer;

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
	m_tauEventData.setObject("TrackContainer", RoITrackParticleContainer);
	m_tauEventData.setObject("VxPrimaryCandidate", RoIVxContainer);
	if(m_lumiBlockMuTool) m_tauEventData.setObject("AvgInteractions", avg_mu);
	if(m_beamSpotSvc) m_tauEventData.setObject("Beamspot", ptrBeamspot);
	if(m_beamType == ("cosmics")) m_tauEventData.setObject("IsCosmics?", true );


	//-------------------------------------------------------------------------
	// eventInitialize tauRec colls
	//-------------------------------------------------------------------------
	ToolHandleArray<ITauToolBase> ::iterator firstTool = m_tools.begin();
	ToolHandleArray<ITauToolBase> ::iterator lastTool  = m_tools.end();
	for ( ; firstTool != lastTool; firstTool++ ) {
		processStatus = (*firstTool)->eventInitialize();

		if( processStatus != StatusCode :: SUCCESS ) {
			ATH_MSG_ERROR("tool "<<(*firstTool)->name()<< "failed in eventInitialize");
			return StatusCode::FAILURE;
		}
	}
	ATH_MSG_ERROR(" initialize all good ");

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
	setEmptyTauTrack(p_tau, pTrackContainer);

	if(p_seed->e()<=0) {
		msg() << MSG::DEBUG << " Roi: changing eta due to energy " << p_seed->e() << endmsg;
		p_tau->setP4(p_tau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), p_tau->m());
		
		msg() << MSG::DEBUG << "Roi: " << roiDescriptor->roiId()
        		  << " Tau eta: " << p_tau->eta() << " Tau phi: " << p_tau->phi()
        		  << endmsg;
	}

	ATH_MSG_DEBUG(" roidescriptor roiword " << roiDescriptor->roiWord() << " saved " << p_tau->ROIWord() );

	m_tauEventData.setObject("JetCollection", theJetCollection );

	//-------------------------------------------------------------------------
	// loop over booked tau tools
	//-------------------------------------------------------------------------
	int toolnum = 0;
	firstTool = m_tools.begin();
	lastTool  = m_tools.end();
	processStatus    = StatusCode::SUCCESS;
	ATH_MSG_DEBUG("Starting tool loop with seed jet");
	//std::vector<TrigTimer* >::iterator itimer =  m_mytimers.begin();
	while ( ! processStatus.isFailure() && firstTool != lastTool ) {
	   // loop stops only when Failure indicated by one of the tools
	   ATH_MSG_DEBUG("Starting Tool: " <<  (*firstTool)->name() );
		// time in the various tools
		++toolnum;
		//if ( doTiming() && itimer != m_mytimers.end() ) (*itimer)->start();

		processStatus = (*firstTool)->execute( *p_tau );
		if ( !processStatus.isFailure() ) {
			ATH_MSG_DEBUG("REGTEST: "<< (*firstTool)->name() << " executed successfully ");
	      ATH_MSG_DEBUG("REGTEST: Roi: " << roiDescriptor->roiId()
            						<< " Tau eta: " << p_tau->eta() << " Tau phi: " << p_tau->phi()
            						<< " Tau pT : "<< p_tau->pt());
		}
		else {
			ATH_MSG_DEBUG("REGTEST: "<< (*firstTool)->name() << " execution failed ");
		}
		++firstTool;
		//++itimer;
		//if ( doTiming() && itimer != m_mytimers.end() ) (*itimer)->stop();
	}

	//check status
	if ( !processStatus.isSuccess() )  {   // some problem
		ATH_MSG_DEBUG("the tau object has NOT been registered in the tau container");
		// ToolHandleArray<ITauToolBase> ::iterator tool = m_tools.begin();
		// for(; tool != firstTool; ++tool ) (*tool)->cleanup( &m_tauEventData );
		// (*tool)->cleanup( &m_tauEventData );

		xAOD::TauJet* bad_tau = pContainer->back();
		ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
		pTrackContainer->erase(pTrackContainer->end()-bad_tau->nAllTracks(), pTrackContainer->end());

		pContainer->pop_back();

		ATH_MSG_DEBUG("clean up done after jet seed");
	}
	else if( processStatus.isSuccess()) {

	  float fJetEnergy = (*p_tau->jetLink())->e();
	  ATH_MSG_DEBUG(" Roi: jet e "<< fJetEnergy);
	  
	  if( fJetEnergy < 0.00001 ) {
	    ATH_MSG_DEBUG(" Roi: changing eta phi to L1 ones due to energy negative (PxPyPzE flips eta and phi)");
	    ATH_MSG_DEBUG(" Roi: this is probably not needed anymore, method PxPyPzE has been corrected");
	    
	    //p_tau->setEta(roiDescriptor->eta0());
	    //p_tau->setPhi(roiDescriptor->phi0());
	    // Direct accessors not available anymore
	    p_tau->setP4(p_tau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), p_tau->m());
	    
	    ATH_MSG_DEBUG(" Roi: " << roiDescriptor->roiId()
		  << " Tau eta: " << p_tau->eta()
		  << " Tau phi: " << p_tau->phi()
		  << " Tau pT : "<< p_tau->pt());
	  }
	  
	  // loop over end tools
	  ToolHandleArray<ITauToolBase> ::iterator p_itET = m_endtools.begin();
	  ToolHandleArray<ITauToolBase> ::iterator p_itETE = m_endtools.end();
	  for (; p_itET != p_itETE; ++p_itET ) {
	    ATH_MSG_VERBOSE("Invoking endTool " << ( *p_itET )->name() );
	    
	    processStatus = ( *p_itET )->execute( *p_tau);
	    if( processStatus.isFailure() ) break;
	  }
	  // Get L1 RoiDescriptor
     SG::ReadHandle< TrigRoiDescriptorCollection > L1roisHandle = SG::makeHandle( m_L1RoIKey, ctx );
     const TrigRoiDescriptor *roiL1Descriptor = L1roisHandle->at(0);

	  EtaL1         =  roiL1Descriptor->eta();
	  PhiL1         =  roiL1Descriptor->phi();

	  // get tau detail variables for Monitoring
	  numTrack      = p_tau->nTracks();
	  nWideTrk = p_tau->nTracksIsolation();
	  p_tau->detail(xAOD::TauJetParameters::trkAvgDist, trkAvgDist);
	  p_tau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etovPtLead);
	  p_tau->detail(xAOD::TauJetParameters::EMRadius, emRadius);
	  p_tau->detail(xAOD::TauJetParameters::hadRadius, hadRadius);
	  p_tau->detail(xAOD::TauJetParameters::isolFrac, IsoFrac);
	  p_tau->detail(xAOD::TauJetParameters::centFrac, centFrac);
	  p_tau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk);
	  p_tau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSig);
	  p_tau->detail(xAOD::TauJetParameters::dRmax, dRmax);
	  p_tau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys);
	  
	  p_tau->detail(xAOD::TauJetParameters::PSSFraction, PSSFraction);
	  p_tau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP);
	  p_tau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME);
	  p_tau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist);	 
     p_tau->detail(xAOD::TauJetParameters::SumPtTrkFrac, SumPtTrkFrac);

	  massTrkSys /= 1000.; // make GeV
	  p_tau->detail(xAOD::TauJetParameters::etEMAtEMScale, EtEm);
	  EtEm /= 1000.;  // make GeV
	  p_tau->detail(xAOD::TauJetParameters::etHadAtEMScale, EtHad);
	  EtHad /= 1000.; // make GeV
	  Et            = EtEm + EtHad;
	  EtFinal       = p_tau->pt()/1000.;
	  
	  EtaEF = p_tau->eta();
	  PhiEF = p_tau->phi();
	  
	  if( Et !=0) EMFrac =  EtEm/ Et ;
	  
	  dEta =  EtaEF - roiDescriptor->eta();
	  dPhi =  PhiEF - roiDescriptor->phi();
	  if(dPhi<-M_PI) dPhi += 2.0*M_PI;
	  if(dPhi>M_PI)  dPhi -= 2.0*M_PI;
	  
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
	   msg() << MSG::DEBUG << endmsg;
	   
	   msg() << MSG::DEBUG << "REGTEST: deltaZ0 for wide trk ";
	   for ( unsigned int i=0; i<m_deltaZ0wideTrks.size(); ++i) msg() << MSG::DEBUG << i << ": " << m_deltaZ0wideTrks[i] << ", ";
	   msg() << MSG::DEBUG << endmsg;
	   }
	  */

	  //
	  // copy CaloOnly four vector, if present
	  //

	  std::vector<const xAOD::TauJetContainer*> tempCaloOnlyContVec;

     // get TauJetContainer
     SG::ReadHandle< xAOD::TauJetContainer > TJContainerHandle = SG::makeHandle( m_trigTauJetKey,ctx );
     const xAOD::TauJetContainer *tempCaloOnlyCont=TJContainerHandle.get();
     tempCaloOnlyContVec.push_back(tempCaloOnlyCont);

	  /*if( !tempCaloOnlyContVec.isValid()){

	    ATH_MSG_DEBUG( "Can't get container TrigTauRecCaloOnly to copy four-vector");

	  } else {*/
	    if (tempCaloOnlyCont) {

	      // const xAOD::TauJetContainer* tempCaloOnlyTauCont = tempCaloOnlyContVec.back();
	      // for(xAOD::TauJetContainer::const_iterator tauIt = tempCaloOnlyTauCont->begin(); tauIt != tempCaloOnlyTauCont->end(); tauIt++){ 

	      // const xAOD::TauJetContainer* tempCaloOnlyTauCont = tempCaloOnlyContVec.back();

	      for(auto tauIt: *tempCaloOnlyCont){
           ATH_MSG_DEBUG("On the loop");
	   	  ATH_MSG_DEBUG("pT(tau) = " << tauIt->pt() << " pT(caloOnly) = " << tauIt->ptTrigCaloOnly() );
	  	
	   	  p_tau->setP4(xAOD::TauJetParameters::TrigCaloOnly, tauIt->ptTrigCaloOnly(), tauIt->etaTrigCaloOnly(), tauIt->phiTrigCaloOnly(), tauIt->mTrigCaloOnly());

	      }
	      
	    //}
	    
	  }else{
               ATH_MSG_WARNING( "TauJetContainer not found :");
     }


	  //
	  // Set NUMVERTICES Aux variable
	  //

	  // static SG::AuxElement::Accessor<int> acc_nVertex("NUMVERTICES");
	  // acc_nVertex(*p_tau) = avg_mu;



	  ATH_MSG_DEBUG("REGTEST: Roi: " << roiDescriptor->roiId()
		<< " Tau being saved eta: " << EtaEF << " Tau phi: " << PhiEF
		<< " wrt L1 dEta "<< dEta<<" dPhi "<<dPhi
		<< " Tau Et (GeV): "<< EtFinal);
	  
	  ++Ncand;
	}
	else {
	  xAOD::TauJet* bad_tau = pContainer->back();
	  ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
	  pTrackContainer->erase(pTrackContainer->end()-bad_tau->nAllTracks(), pTrackContainer->end());
	  pContainer->pop_back();
	  
	  ATH_MSG_DEBUG("deleted tau done after jet seed");
	}
	
	// call eventFinalize on the booked tau tools
	for ( firstTool = m_tools.begin(); firstTool != lastTool; firstTool++ ) {
	  processStatus = (*firstTool)->eventFinalize();
	  if( processStatus != StatusCode :: SUCCESS ) {
	    ATH_MSG_DEBUG("tool "<<(*firstTool)->name()<< "failed in eventFinalize");
	    return StatusCode::FAILURE;
	  }
	}
	ATH_MSG_DEBUG("tools succeed in eventFinalize");
	
	
	//-------------------------------------------------------------------------
	// all done, register the tau Container in TDS.
	//-------------------------------------------------------------------------

   // Prepare Outputs
   //std::unique_ptr< xAOD::TauJetContainer > taujetContainer( new xAOD::TauJetContainer() );
   //std::unique_ptr< xAOD::TauTrackContainer > tautrkContainer( new xAOD::TauTrackContainer() );

   // Save Outputs
   SG::WriteHandle<xAOD::TauJetContainer> outTauJetHandle( m_trigtauRecOutKey );
   SG::WriteHandle<xAOD::TauTrackContainer> outTauTrackHandle( m_trigtauTrkOutKey );

   //if(pContainer) taujetContainer->push_back(&pContainer);
   //if(pTrackContainer) tautrkContainer->push_back(pTrackContainer);
   ATH_MSG_DEBUG("Output TauJetContainer size:"<< pContainer->size());
   ATH_MSG_DEBUG("Output TauTrackJetContainer size:"<< pTrackContainer->size());

   CHECK( outTauJetHandle.record(std::unique_ptr<xAOD::TauJetContainer>{pContainer}, std::unique_ptr<xAOD::TauJetAuxContainer>{&pAuxContainer}) );
   //if(pTrackContainer->size()!=0) ATH_CHECK( outTauTrackHandle.record(std::unique_ptr<xAOD::TauTrackContainer>{pTrackContainer}, std::unique_ptr<xAOD::TauTrackAuxContainer>{&pTrackAuxContainer}) );

	//hltStatus=attachFeature(outputTE, pContainer, m_outputName);
	//hltStatus=attachFeature(outputTE, pTrackContainer, m_outputName+"Tracks");
	//if (hltStatus!=HLT::OK )  {
	//	ATH_MSG_ERROR("Unable to record tau Container in TDS");
	//	calo_errors.push_back(NoHLTtauAttach);
	//	return StatusCode::FAILURE;
	//}
	//else {
		ATH_MSG_DEBUG("Recorded a tau container: HLT_TrigTauRecMergedMT");
	//}

	ATH_MSG_DEBUG("the tau object has been registered in the tau container");
	
	// set status of TE to always true for FE algorithms
	return StatusCode::SUCCESS;
}

void TrigTauRecMergedMT::setEmptyTauTrack(xAOD::TauJet* &pTau,
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

