/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// MmDigitizationTool
// ------------
// Authors: Nektarios Chr. Benekos <nectarios.benekos@cern.ch>
//          Konstantinos Karakostas <Konstantinos.Karakostas@cern.ch>
//
// Major Contributions From: Verena Martinez
//                           Tomoyuki Saito
//
// Major Restructuring for r21+ From: Lawrence Lee <lawrence.lee.jr@cern.ch>
//
////////////////////////////////////////////////////////////////////////////////


//Inputs
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"

//Outputs
#include "MuonDigitContainer/MmDigitContainer.h"

//MM digitization includes
#include "MM_Digitization/MmDigitizationTool.h"
#include "MM_Digitization/IMM_DigitizationTool.h"
#include "MM_Digitization/MmDigitToolInput.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"

//Geometry
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"

//Gaudi - Core
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"
#include "AIDA/IHistogram1D.h"
#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

//Geometry
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"

//Pile-up
#include "PileUpTools/PileUpMergeSvc.h"

//Truth
#include "CLHEP/Units/PhysicalConstants.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h"

//Random Numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGamma.h"
#include "CLHEP/Random/RandPoisson.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandExponential.h"

#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"

//VMM Mapping
#include "MM_Digitization/MMStripVmmMappingTool.h"

//ROOT
#include "TH1.h"
#include "TTree.h"
#include "TFile.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace MuonGM;

/*******************************************************************************/
MmDigitizationTool::MmDigitizationTool(const std::string& type, const std::string& name, const IInterface* parent):
	PileUpToolBase(type, name, parent),

	// Services
	m_storeGateService("StoreGateSvc", name),
	m_magFieldSvc("AtlasFieldSvc",name) ,
	m_mergeSvc(0),
	m_rndmSvc("AtRndmGenSvc", name ),
	m_rndmEngine(0),
	m_rndmEngineName("MuonDigitization"),

	// Containers
	m_digitContainer(NULL),
	m_sdoContainer(NULL),

	// Tools
	m_digitTool("MM_Response_DigitTool", this),
	m_file(0),
	m_ntuple(0),

	// Settings
	m_energyThreshold(50.),
	m_maskMultiplet(0),
	m_writeOutputFile(false),
	m_timedHitCollection_MM(0),

	m_inputObjectName(""),
	m_outputObjectName(""),
	m_outputSDOName(""),

	m_checkMMSimHits(true),
	m_useTimeWindow(true),

	m_timeWindowLowerOffset(0),
	m_timeWindowUpperOffset(0),
	m_DiffMagSecondMuonHit (0),

	// Strip Response
	m_StripsResponseSimulation(0),
	m_qThreshold(0),							// Strips Charge Threshold
	m_transverseDiffusionSigma(0),				// Transverse Diffusion
	m_longitudinalDiffusionSigma(0),			// Longitudinal Diffusion
	m_driftVelocity(0),							// Drift Velocity
	m_crossTalk1(0),							// Cross talk with nearest strip
	m_crossTalk2(0),							// Cross talk with 2nd nearest strip

	// Electronics Response
	m_ElectronicsResponseSimulation(0),
	m_peakTime(0),
	m_electronicsThreshold(0),
	m_stripdeadtime(0),
	m_ARTdeadtime(0),

	// Tree Branches...
	m_n_Station_side(-999),
	m_n_Station_eta(-999),
	m_n_Station_phi(-999),
	m_n_Station_multilayer(-999),
	m_n_Station_layer(-999),
	m_n_hitStripID(-999),
	m_n_StrRespTrg_ID(-999),
	m_n_strip_multiplicity(-999),
	m_n_strip_multiplicity_2(-999),
	m_n_hitPDGId(-99999999.),
	m_n_hitOnSurface_x(-99999999.),
	m_n_hitOnSurface_y(-99999999.),
	m_n_hitDistToChannel(-99999999.),
	m_n_hitIncomingAngle(-99999999.),
	m_n_StrRespTrg_Time(-99999999.),
	m_n_hitIncomingAngleRads(-99999999.),
	m_n_hitKineticEnergy(-99999999.),
	m_n_hitDepositEnergy(-99999999.),
	m_exitcode(0),

	// Timings
	tofCorrection(-99999999.),
	bunchTime(-99999999.),
	globalHitTime(-99999999.),
	eventTime(-99999999.)
{

	declareInterface<IMuonDigitizationTool>(this);

	declareProperty("MCStore",             m_storeGateService,              "help");
	declareProperty("MagFieldSvc",         m_magFieldSvc,        "Magnetic Field Service");
	declareProperty("RndmSvc",             m_rndmSvc,            "Random Number Service used in Muon digitization");
	declareProperty("RndmEngine",          m_rndmEngineName,     "Random engine name");

	declareProperty("DigitizationTool",    m_digitTool,          "Tool which handle the digitization process");
	declareProperty("EnergyThreshold",     m_energyThreshold = 50., "Minimal energy to produce a PRD"  );
	declareProperty("MaskMultiplet", m_maskMultiplet = 0,  "0: all, 1: first, 2: second, 3: both"  );

	declareProperty("SaveInternalHistos",  m_writeOutputFile = true   );

	//Object names
	declareProperty("InputObjectName",     m_inputObjectName     =  "MicromegasSensitiveDetector");
	declareProperty("OutputObjectName",    m_outputObjectName    =  "MM_DIGITS");
	declareProperty("OutputSDOName",       m_outputSDOName       =  "MM_SDO");

	//Configurations
	declareProperty("CheckSimHits",        m_checkMMSimHits      =  true,       "Control on the hit validity"); // Currently deprecated

	//Timing scheme
	declareProperty("UseTimeWindow",       m_useTimeWindow  =  true);
	declareProperty("WindowLowerOffset",   m_timeWindowLowerOffset = -300.); // processBunchXing between -250 and 150 ns (look at config file)
	declareProperty("WindowUpperOffset",   m_timeWindowUpperOffset = +300.);
	declareProperty("DiffMagSecondMuonHit",m_DiffMagSecondMuonHit = 0.1);

	// Constants vars for the StripsResponseSimulation class
	// qThreshold=2e, we accept a good strip if the charge is >=2e
	declareProperty("qThreshold",                 m_qThreshold = 0.001);     // Charge Threshold
	declareProperty("TransverseDiffusionSigma",   m_transverseDiffusionSigma = 0.360/10.);   // Diffusion Constants for electron propagation
	declareProperty("LongitudinalDiffusionSigma", m_longitudinalDiffusionSigma = 0.190/10.);
	declareProperty("DriftGapWidth",              m_driftGapWidth = 5.168);  // Drift Gap Width of 5.04 mm + 0.128 mm (the amplification gap)
	declareProperty("DriftVelocity",              m_driftVelocity = 0.047);  // Drift velocity in [mm/ns]
	declareProperty("crossTalk1",		          m_crossTalk1 = 0.1);       // Strip Cross Talk with Nearest Neighbor
	declareProperty("crossTalk2",		          m_crossTalk2 = 0.03);      // Strip Cross Talk with 2nd Nearest Neighbor

	// Constants vars for the ElectronicsResponseSimulation
	declareProperty("peakTime",                m_peakTime = 50.);            // The VMM peak time setting.
	declareProperty("electronicsThreshold",    m_electronicsThreshold = 6000.0);  // 2*(Intrinsic noise ~3k e)
	declareProperty("StripDeadTime",           m_stripdeadtime = 200.0);  // default value ?
	declareProperty("ARTDeadTime",             m_ARTdeadtime   = 200.0);  // default value ?

}

/*******************************************************************************/
// member function implementation
//--------------------------------------------
StatusCode MmDigitizationTool::initialize() {

	ATH_MSG_DEBUG ("MmDigitizationTool:: in initialize()") ;
	ATH_MSG_DEBUG ( "Configuration  MmDigitizationTool " );
	ATH_MSG_DEBUG ( "RndmSvc                " << m_rndmSvc             );
	ATH_MSG_DEBUG ( "RndmEngine             " << m_rndmEngineName      );
	ATH_MSG_DEBUG ( "MCStore                " << m_storeGateService               );
	ATH_MSG_DEBUG ( "MagFieldSvc            " << m_magFieldSvc         );
	ATH_MSG_DEBUG ( "DigitizationTool       " << m_digitTool           );
	ATH_MSG_DEBUG ( "InputObjectName        " << m_inputObjectName     );
	ATH_MSG_DEBUG ( "OutputObjectName       " << m_outputObjectName    );
	ATH_MSG_DEBUG ( "OutputSDOName          " << m_outputSDOName       );
	ATH_MSG_DEBUG ( "UseTimeWindow          " << m_useTimeWindow       );
	ATH_MSG_DEBUG ( "CheckSimHits           " << m_checkMMSimHits      );
	ATH_MSG_DEBUG ( "Threshold              " << m_qThreshold          );
	ATH_MSG_DEBUG ( "DiffusSigma            " << m_transverseDiffusionSigma		);
	ATH_MSG_DEBUG ( "LogitundinalDiffusSigma" << m_longitudinalDiffusionSigma 	);
	ATH_MSG_DEBUG ( "DriftVelocity          " << m_driftVelocity      	 		);
	ATH_MSG_DEBUG ( "crossTalk1             " << m_crossTalk1 	     			);
	ATH_MSG_DEBUG ( "crossTalk2             " << m_crossTalk2 	     			);
	ATH_MSG_DEBUG ( "EnergyThreshold        " << m_energyThreshold     			);

	// Initialize transient event store
	ATH_CHECK(m_storeGateService.retrieve());
	ATH_CHECK( service("ActiveStoreSvc", m_activeStore) );

	// Initialize transient detector store and MuonGeoModel OR MuonDetDescrManager
	StoreGateSvc* detStore=0;
	m_MuonGeoMgr=0;
	ATH_CHECK( serviceLocator()->service("DetectorStore", detStore) );
	if(detStore->contains<MuonGM::MuonDetectorManager>( "Muon" )){
		ATH_CHECK( detStore->retrieve(m_MuonGeoMgr) );
		ATH_MSG_DEBUG ( "Retrieved MuonGeoModelDetectorManager from StoreGate" );
		m_idHelper = m_MuonGeoMgr->mmIdHelper();
		ATH_MSG_DEBUG ( "Retrieved MmIdHelper " << m_idHelper );
	}

	// Magnetic field service
	ATH_CHECK( m_magFieldSvc.retrieve() );

	// Digit tools
	ATH_CHECK( m_digitTool.retrieve() );

	// Random Service
	ATH_CHECK( m_rndmSvc.retrieve() );

	// Random Engine from Random Service
	ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
	m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
	if (m_rndmEngine==0) {
		ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName );
		return StatusCode::FAILURE;
	}

	//initialize the digit container
	m_digitContainer = new MmDigitContainer(m_idHelper->detectorElement_hash_max());
	m_digitContainer->addRef();

	//simulation identifier helper
	m_muonHelper = MicromegasHitIdHelper::GetHelper();

	// Locate the PileUpMergeSvc and initialize our local ptr
	ATH_CHECK(service("PileUpMergeSvc", m_mergeSvc, true));

	// Validation File Output
	if (m_writeOutputFile){
		m_file = new TFile("MM_Digitization_plots.root","RECREATE");
		m_ntuple = new TTree("fullSim","fullSim");

		m_ntuple->Branch("m_exitcode",&m_exitcode);
		m_ntuple->Branch("Station_side",&m_n_Station_side);
		m_ntuple->Branch("Station_eta",&m_n_Station_eta);
		m_ntuple->Branch("Station_phi",&m_n_Station_phi);
		m_ntuple->Branch("Station_multilayer",&m_n_Station_multilayer);
		m_ntuple->Branch("Station_layer",&m_n_Station_layer);

		m_ntuple->Branch("hitPDGId",&m_n_hitPDGId);
		m_ntuple->Branch("hitKineticEnergy",&m_n_hitKineticEnergy);
		m_ntuple->Branch("hitDepositEnergy",&m_n_hitDepositEnergy);
		m_ntuple->Branch("hitOnSurface_x",&m_n_hitOnSurface_x);
		m_ntuple->Branch("hitOnSurface_y",&m_n_hitOnSurface_y);
		m_ntuple->Branch("hitStripID",&m_n_hitStripID);
		m_ntuple->Branch("hitDistToChannel",&m_n_hitDistToChannel);
		m_ntuple->Branch("hitIncomingAngle",&m_n_hitIncomingAngle);
		m_ntuple->Branch("hitIncomingAngleRads",&m_n_hitIncomingAngleRads);

		m_ntuple->Branch("StrRespID",&m_n_StrRespID);
		m_ntuple->Branch("StrRespCharge",&m_n_StrRespCharge);
		m_ntuple->Branch("StrRespTime",&m_n_StrRespTime);
		m_ntuple->Branch("StrRespTrg_ID",&m_n_StrRespTrg_ID);
		m_ntuple->Branch("StrRespTrg_Time",&m_n_StrRespTrg_Time);
		m_ntuple->Branch("Strip_Multiplicity_byDiffer",&m_n_strip_multiplicity);
		m_ntuple->Branch("Strip_Multiplicity_2",&m_n_strip_multiplicity_2);
		m_ntuple->Branch("tofCorrection",&tofCorrection);
		m_ntuple->Branch("bunchTime",&bunchTime);
		m_ntuple->Branch("globalHitTime",&globalHitTime);
		m_ntuple->Branch("eventTime",&eventTime);
	}

	// StripsResponseSimulation Creation
	m_StripsResponseSimulation = new StripsResponseSimulation();
	m_StripsResponseSimulation->setQThreshold(m_qThreshold);
	m_StripsResponseSimulation->setTransverseDiffusionSigma(m_transverseDiffusionSigma);
	m_StripsResponseSimulation->setLongitudinalDiffusionSigma(m_longitudinalDiffusionSigma);
	m_StripsResponseSimulation->setDriftGapWidth(m_driftGapWidth);
	m_StripsResponseSimulation->setDriftVelocity(m_driftVelocity);
	m_StripsResponseSimulation->setCrossTalk1(m_crossTalk1);
	m_StripsResponseSimulation->setCrossTalk2(m_crossTalk2);
	m_StripsResponseSimulation->initialize();

	// ElectronicsResponseSimulation Creation
	m_ElectronicsResponseSimulation = new ElectronicsResponseSimulation();
	m_ElectronicsResponseSimulation->setPeakTime(m_peakTime); // VMM peak time parameter
	m_ElectronicsResponseSimulation->setTimeWindowLowerOffset(m_timeWindowLowerOffset);
	m_ElectronicsResponseSimulation->setTimeWindowUpperOffset(m_timeWindowUpperOffset);
	m_ElectronicsResponseSimulation->setElectronicsThreshold(m_electronicsThreshold);
	m_ElectronicsResponseSimulation->setStripdeadtime(m_stripdeadtime);
	m_ElectronicsResponseSimulation->setARTdeadtime(m_ARTdeadtime);
	m_ElectronicsResponseSimulation->setStripResponseQThreshold(     m_StripsResponseSimulation->getQThreshold()    );
	m_ElectronicsResponseSimulation->setStripResponseDriftVelocity(  m_StripsResponseSimulation->getDriftVelocity() );
	m_ElectronicsResponseSimulation->setStripResponseDriftGapWidth(  m_StripsResponseSimulation->getDriftGapWidth() );
	m_ElectronicsResponseSimulation->initialize();


	return StatusCode::SUCCESS;
}
/*******************************************************************************/
//----------------------------------------------------------------------
// PrepareEvent method:
//----------------------------------------------------------------------
StatusCode MmDigitizationTool::prepareEvent(unsigned int nInputEvents) {

	ATH_MSG_DEBUG("MmDigitizationTool::prepareEvent() called for " << nInputEvents << " input events" );

	m_MMHitCollList.clear();

	if(!m_timedHitCollection_MM) {
		m_timedHitCollection_MM = new TimedHitCollection<GenericMuonSimHit>();
	} else {
		ATH_MSG_ERROR ( "m_timedHitCollection_MM is not null" );
		return StatusCode::FAILURE;
	}

	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MmDigitizationTool::processBunchXing(int bunchXing,
	SubEventIterator bSubEvents,
	SubEventIterator eSubEvents) {

	ATH_MSG_DEBUG ( "MmDigitizationTool::in processBunchXing()"  << bunchXing );

	SubEventIterator iEvt = bSubEvents;

	// Loop on event and sub-events for the current bunch Xing
	for (; iEvt!=eSubEvents; ++iEvt) {

		StoreGateSvc& seStore = *iEvt->ptr()->evtStore();

		ATH_MSG_DEBUG( "SubEvt EventInfo from StoreGate " << seStore.name() << " :"
			<< " bunch crossing : " << bunchXing );

		PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index());

		const GenericMuonSimHitCollection* seHitColl = 0;
		ATH_CHECK( seStore.retrieve(seHitColl,m_inputObjectName) );
		ATH_MSG_VERBOSE ( "MicroMegas SimHitCollection found with " << seHitColl->size() << " hits" );

		const double timeOfBCID(static_cast<double>(iEvt->time()));
		ATH_MSG_DEBUG ( "timeOfBCID " << timeOfBCID );

		//Copy hit Collection
		GenericMuonSimHitCollection* MMHitColl = new GenericMuonSimHitCollection(m_inputObjectName);

		GenericMuonSimHitCollection::const_iterator i = seHitColl->begin();
		GenericMuonSimHitCollection::const_iterator e = seHitColl->end();

		// Read hits from this collection
		for (; i!=e; ++i){
			MMHitColl->Emplace(*i);
		}
		m_timedHitCollection_MM->insert(thisEventIndex, MMHitColl);

		//store these for deletion at the end of mergeEvent
		m_MMHitCollList.push_back(MMHitColl);

	}//  while (iEvt != eSubEvents) {
	return StatusCode::SUCCESS;
}

/*******************************************************************************/
StatusCode MmDigitizationTool::getNextEvent() {

// Get next event and extract collection of hit collections:
// This is applicable to non-PileUp Event...

	ATH_MSG_DEBUG ( "MmDigitizationTool::getNextEvent()" );

	if (!m_mergeSvc) {
		ATH_CHECK(service("PileUpMergeSvc", m_mergeSvc, true));
	}

	//  get the container(s)
	typedef PileUpMergeSvc::TimedList<GenericMuonSimHitCollection>::type TimedHitCollList;

	//this is a list<info<time_t, DataLink<GenericMuonSimHitCollection> > >
	TimedHitCollList hitCollList;

	ATH_CHECK( m_mergeSvc->retrieveSubEvtsData(m_inputObjectName, hitCollList) );
	if (hitCollList.size()==0) {
		ATH_MSG_ERROR ( "TimedHitCollList has size 0" );
		return StatusCode::FAILURE;
	}
	else {
		ATH_MSG_DEBUG ( hitCollList.size() << " MicroMegas SimHitCollections with key " << m_inputObjectName << " found" );
	}

	// create a new hits collection - Define Hit Collection
	// m_timedHitCollection_MM = new TimedHitCollection<GenericMuonSimHit>() ;
	if(!m_timedHitCollection_MM) {
		m_timedHitCollection_MM = new TimedHitCollection<GenericMuonSimHit>();
	}else{
		ATH_MSG_ERROR ( "m_timedHitCollection_MM is not null" );
		return StatusCode::FAILURE;
	}

	//now merge all collections into one
	TimedHitCollList::iterator iColl(hitCollList.begin());
	TimedHitCollList::iterator endColl(hitCollList.end());

	// loop on the hit collections
	while(iColl != endColl) {
		const GenericMuonSimHitCollection* tmpColl(iColl->second);
		m_timedHitCollection_MM->insert(iColl->first, tmpColl);
		ATH_MSG_DEBUG ( "MMSimHitCollection found with " << tmpColl->size() << " hits"  );
		++iColl;
	}
	return StatusCode::SUCCESS;

}
/*******************************************************************************/
StatusCode MmDigitizationTool::mergeEvent() {

	// ATH_MSG_DEBUG ( "MmDigitizationTool::in mergeEvent()" );

	// Cleanup and record the Digit container in StoreGate
	ATH_CHECK( recordDigitAndSdoContainers() );
	ATH_CHECK( doDigitization() );

	// reset the pointer (delete null pointer should be safe)
	if (m_timedHitCollection_MM){
		delete m_timedHitCollection_MM;
		m_timedHitCollection_MM = 0;
	}

	// remove cloned one in processBunchXing......
	std::list<GenericMuonSimHitCollection*>::iterator MMHitColl = m_MMHitCollList.begin();
	std::list<GenericMuonSimHitCollection*>::iterator MMHitCollEnd = m_MMHitCollList.end();
	while(MMHitColl!=MMHitCollEnd) {
		delete (*MMHitColl);
		++MMHitColl;
	}
	m_MMHitCollList.clear();
	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MmDigitizationTool::digitize() {
	return this->processAllSubEvents();
}
/*******************************************************************************/
StatusCode MmDigitizationTool::processAllSubEvents() {

	ATH_MSG_DEBUG ("MmDigitizationTool::processAllSubEvents()");

	ATH_CHECK( recordDigitAndSdoContainers() );

	//merging of the hit collection in getNextEvent method

	if (0 == m_timedHitCollection_MM ) ATH_CHECK( getNextEvent() );

	ATH_CHECK( doDigitization() );

	// reset the pointer (delete null pointer should be safe)
	if (m_timedHitCollection_MM){
		delete m_timedHitCollection_MM;
		m_timedHitCollection_MM = 0;
	}
	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MmDigitizationTool::finalize() {
	m_digitContainer->release();

	if (m_writeOutputFile) {
		TDirectory*backup=gDirectory;
		m_file->cd();
		m_ntuple->Write();
		gDirectory=backup;
		m_file->Close();
	}


	delete m_StripsResponseSimulation;
	delete m_ElectronicsResponseSimulation;

	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MmDigitizationTool::recordDigitAndSdoContainers() {

	// cleanup digit container
	m_digitContainer->cleanup();

	// record the digit container in StoreGate
	m_activeStore->setStore(&*m_storeGateService);
	ATH_CHECK( m_storeGateService->record(m_digitContainer, m_outputObjectName) );

	// create and record the SDO container in StoreGate
	m_sdoContainer = new MuonSimDataCollection();
	ATH_CHECK( m_storeGateService->record(m_sdoContainer, m_outputSDOName) );

	return StatusCode::SUCCESS;
}

/*******************************************************************************/
StatusCode MmDigitizationTool::doDigitization() {


	GenericMuonSimHitCollection* inputSimHitColl=NULL;

	IdentifierHash detectorElementHash=0;

	inputSimHitColl = new GenericMuonSimHitCollection("MicromegasSensitiveDetector");
	ATH_CHECK( m_storeGateService->record(inputSimHitColl,"InputMicroMegasHits") );

	if( m_maskMultiplet == 3 ) {

		return StatusCode::SUCCESS;
	}

	// Perform null check on m_thpcCSC
	if(!m_timedHitCollection_MM) {
		ATH_MSG_ERROR ( "m_timedHitCollection_MM is null" );
		return StatusCode::FAILURE;
	}

	std::vector<MmElectronicsToolInput> v_stripDigitOutput;
	v_stripDigitOutput.clear();

	//iterate over hits and fill id-keyed drift time map
	TimedHitCollection< GenericMuonSimHit >::const_iterator i, e;
	const GenericMuonSimHit* previousHit = 0;

	// ATH_MSG_DEBUG("create PRD container of size " << m_idHelper->detectorElement_hash_max());
	std::map<Identifier,int> hitsPerChannel;
	int nhits = 0;

	// nextDetectorElement-->sets an iterator range with the hits of current detector element , returns a bool when done
	while( m_timedHitCollection_MM->nextDetectorElement(i, e) ) {

		Identifier layerID;
		// Loop over the hits:
		while (i != e) {

			////////////////////////////////////////////////////////////////////
			//
			// Hit Information And Preparation
			//


			TimedHitPtr<GenericMuonSimHit> phit = *i++;
			eventTime = phit.eventTime();
			const GenericMuonSimHit& hit(*phit);

			if( previousHit && abs(hit.particleEncoding())==13 && abs(previousHit->particleEncoding())==13 ) {
				Amg::Vector3D diff = previousHit->localPosition() - hit.localPrePosition();
				ATH_MSG_DEBUG("second hit from a muon: prev " <<  previousHit->localPosition() << " current " << hit.localPrePosition() << " diff " << diff );
				if( diff.mag() < m_DiffMagSecondMuonHit ) continue;
			}
			m_n_hitPDGId = hit.particleEncoding();
			m_n_hitDepositEnergy = hit.depositEnergy();
			m_n_hitKineticEnergy = hit.kineticEnergy();

			const Amg::Vector3D globalHitPosition = hit.globalPosition();

			globalHitTime = hit.globalpreTime();
			tofCorrection = globalHitPosition.mag()/CLHEP::c_light;
			bunchTime = globalHitTime - tofCorrection + eventTime;

			m_n_Station_side=-999;
			m_n_Station_eta=-999;
			m_n_Station_phi=-999;
			m_n_Station_multilayer=-999;
			m_n_Station_layer=-999;
			m_n_hitStripID=-999;
			m_n_StrRespTrg_ID=-999;
			m_n_hitOnSurface_x=-99999999.;
			m_n_hitOnSurface_y=-99999999.;
			m_n_hitDistToChannel=-99999999.;
			m_n_hitIncomingAngle=-99999999.;
			m_n_StrRespTrg_Time=-99999999.;
			m_n_strip_multiplicity =-99999999.;
			m_exitcode = 0;

			m_n_StrRespID.clear();
			m_n_StrRespCharge.clear();
			m_n_StrRespTime.clear();

			const int hitID = hit.GenericId();
			// the G4 time or TOF from IP
			// double G4Time(hit.globalTime());
			// see what are the members of GenericMuonSimHit

			// convert sim id helper to offline id
			MM_SimIdToOfflineId simToOffline(*m_idHelper);

			//get the hit Identifier and info
			int simId=hit.GenericId();
			layerID = simToOffline.convert(simId);

			// Read the information about the Micro Megas hit
			ATH_MSG_DEBUG ( "> hitID  "
							<< hitID
							<< " Hit bunch time  "
							<< bunchTime
							<< " tot "
							<< globalHitTime
							<< " tof/G4 time "
							<< hit.globalTime()
							<< " globalHitPosition "
							<< globalHitPosition
							<< "hit: r "
							<< globalHitPosition.perp()
							<< " z "
							<< globalHitPosition.z()
							<< " mclink "
							<< hit.particleLink()
							<< " station eta "
							<< m_idHelper->stationEta(layerID)
							<< " station phi "
							<< m_idHelper->stationPhi(layerID)
							<< " multiplet "
							<< m_idHelper->multilayer(layerID)
							);

			// For collection of inputs to throw back in SG

			GenericMuonSimHit* copyHit = new GenericMuonSimHit( hitID,
																globalHitTime+eventTime,
																eventTime,
																globalHitPosition,
																hit.localPosition(),
																hit.globalPrePosition(),
																hit.localPrePosition(),
																hit.particleEncoding(),
																hit.kineticEnergy(),
																hit.globalDirection(),
																hit.depositEnergy(),
																hit.StepLength(),
																hit.trackNumber()
																);

			inputSimHitColl->Insert(*copyHit);

			// remove hits in masked multiplet
			if( m_maskMultiplet == m_idHelper->multilayer(layerID) ) continue;


			//
			// Hit Information And Preparation
			//
			////////////////////////////////////////////////////////////////////


			////////////////////////////////////////////////////////////////////
			//
			// Sanity Checks
			//


			if( !m_idHelper->is_mm(layerID) ){
				ATH_MSG_WARNING("layerID does not represent a valid MM layer: " << m_idHelper->stationNameString(m_idHelper->stationName(layerID)) );
				continue;
			}

			std::string stName = m_idHelper->stationNameString(m_idHelper->stationName(layerID));
			int isSmall = stName[2] == 'S';

			if( m_idHelper->is_mdt(layerID)
					|| m_idHelper->is_rpc(layerID)
					|| m_idHelper->is_tgc(layerID)
					|| m_idHelper->is_csc(layerID)
					|| m_idHelper->is_stgc(layerID)
					){
				ATH_MSG_WARNING("MM id has wrong technology type! ");
				m_exitcode = 9;
				if(m_writeOutputFile) m_ntuple->Fill();
				continue
			}

			if( m_idHelper->stationPhi(layerID) == 0 ){
				ATH_MSG_WARNING("unexpected phi range " << m_idHelper->stationPhi(layerID) );
				m_exitcode = 9;
				if(m_writeOutputFile) m_ntuple->Fill();
				continue;
			}

			// get readout element
			const MuonGM::MMReadoutElement* detectorReadoutElement = m_MuonGeoMgr->getMMReadoutElement(layerID);
			if( !detectorReadoutElement ){
				ATH_MSG_WARNING( "Failed to retrieve detector element for: isSmall "
									<< isSmall
									<< " eta "
									<< m_idHelper->stationEta(layerID)
									<< " phi "
									<< m_idHelper->stationPhi(layerID)
									<< " ml "
									<< m_idHelper->multilayer(layerID)
									);
				m_exitcode = 10;
				if(m_writeOutputFile) m_ntuple->Fill();
				continue;
			}

			//
			// Sanity Checks
			//
			////////////////////////////////////////////////////////////////////


			m_n_Station_side = m_muonHelper->GetSide(simId);
			m_n_Station_eta = m_muonHelper->GetZSector(simId);
			m_n_Station_phi = m_muonHelper->GetPhiSector(simId);
			m_n_Station_multilayer = m_muonHelper->GetMultiLayer(simId);
			m_n_Station_layer = m_muonHelper->GetLayer(simId);

			// Get MM_READOUT from MMDetectorDescription
			char side = m_idHelper->stationEta(layerID) < 0 ? 'C' : 'A';
			MMDetectorHelper aHelper;
			MMDetectorDescription* mm = aHelper.Get_MMDetector( stName[2],
																abs(m_idHelper->stationEta(layerID)),
																m_idHelper->stationPhi(layerID),
																m_idHelper->multilayer(layerID),
																side
																);
			MMReadoutParameters roParam = mm->GetReadoutParameters();



			////////////////////////////////////////////////////////////////////
			//
			// Angles, Geometry, and Coordinates. Oh my!
			//



			// Surface
			const Trk::PlaneSurface& surf = detectorReadoutElement->surface(layerID);

			// Calculate The Inclination Angle
			// Angle
			const Amg::Vector3D globalHitDirection( hit.globalDirection().x(),
													hit.globalDirection().y(),
													hit.globalDirection().z()
													);
			Trk::LocalDirection localHitDirection;
			surf.globalToLocalDirection(globalHitDirection, localHitDirection);

			float inAngle_XZ =  localHitDirection.angleXZ() / CLHEP::degree;
			if(inAngle_XZ < 0.0) inAngle_XZ += 180;
			inAngle_XZ = 90. - inAngle_XZ ;
			inAngle_XZ = (roParam.stereoAngel).at(m_muonHelper->GetLayer(simId)-1)*inAngle_XZ ;

			ATH_MSG_DEBUG(  "At eta "
							<< m_idHelper->stationEta(layerID)
							<< " phi "
							<< m_idHelper->stationPhi(layerID)
							<<  "\n IncomingAngle: "
							<<  localHitDirection.angleXZ() / CLHEP::degree
							<< "\n inAngle_XZ, "
							<< inAngle_XZ
							<< " , "
							<< inAngle_XZ * CLHEP::degree
							<< "   ..   "
							<< CLHEP::degree
							);


			// compute the hit position on the readout plane (same as in MuonFastDigitization)
			Amg::Vector3D stripLayerPosition = surf.transform().inverse()*globalHitPosition;
			Amg::Vector2D posOnSurfUnProjected(stripLayerPosition.x(),stripLayerPosition.y());

			Amg::Vector3D localDirection = surf.transform().inverse().linear()*globalHitDirection;
			Amg::Vector3D localDirectionTime(0., 0., 0.);

			// drift direction in backwards-chamber should be opposite to the incident direction.
			if ((roParam.readoutSide).at(m_idHelper->gasGap(layerID)-1)==1)
				localDirectionTime  = localDirection;
			else
				localDirectionTime  = surf.transform()
										.inverse()
										.linear()*Amg::Vector3D(
											hit.globalDirection().x(),
											hit.globalDirection().y(),
											-hit.globalDirection().z()
										);



			double scale = -stripLayerPosition.z()/localDirection.z();

			Amg::Vector3D hitOnSurface = stripLayerPosition + scale*localDirection;
			Amg::Vector2D posOnSurf (hitOnSurface.x(), hitOnSurface.y());

			// Account For Time Offset
			double shiftTimeOffset = (globalHitTime - tofCorrection)* m_driftVelocity;
			Amg::Vector3D hitAfterTimeShift(hitOnSurface.x(),hitOnSurface.y(),shiftTimeOffset);
			Amg::Vector3D hitAfterTimeShiftOnSurface = hitAfterTimeShift - (shiftTimeOffset/localDirectionTime.z())*localDirectionTime;

			if( fabs(hitAfterTimeShiftOnSurface.z()) > 0.1 ) ATH_MSG_WARNING("bad propagation to surface after time shift " << hitAfterTimeShiftOnSurface );

			// Don't consider electron hits below m_energyThreshold
			if( hit.kineticEnergy() < m_energyThreshold && abs(hit.particleEncoding())==11) {
				m_exitcode = 5;
				if(m_writeOutputFile) m_ntuple->Fill();
				continue;
			}

			// perform bound check
			if( !surf.insideBounds(posOnSurf) ){
				m_exitcode = 1;
				if(m_writeOutputFile) m_ntuple->Fill();
				ATH_MSG_DEBUG( "m_exitcode = 1 : shiftTimeOffset = "
								<< shiftTimeOffset
								<< "hitOnSurface.z  = "
								<< hitOnSurface.z()
								<< ", hitOnSurface.x  = "
								<< hitOnSurface.x()
								<< ", hitOnSurface.y  = "
								<< hitOnSurface.y()
								);
				continue;
			}

			int stripNumber = detectorReadoutElement->stripNumber(posOnSurf,layerID);
			Amg::Vector2D tmp (stripLayerPosition.x(), stripLayerPosition.y());

			if( stripNumber == -1 ){
				ATH_MSG_WARNING("!!! Failed to obtain strip number "
								<< m_idHelper->print_to_string(layerID)
								<<  "\n\t\t with pos "
								<< posOnSurf
								<< " z "
								<< stripLayerPosition.z()
								<< " eKin: "
								<< hit.kineticEnergy()
								<< " eDep: "
								<< hit.depositEnergy()
								<< " unprojectedStrip: "
								<< detectorReadoutElement->stripNumber(posOnSurfUnProjected, layerID)
								);
				m_exitcode = 2;
				if(m_writeOutputFile) m_ntuple->Fill();
				ATH_MSG_DEBUG( "m_exitcode = 2 " );
				continue;
			}

			// Re-definition Of ID
			Identifier parentID = m_idHelper->parentID(layerID);
			Identifier digitID  = m_idHelper->channelID(parentID,
														m_idHelper->multilayer(layerID),
														m_idHelper->gasGap(layerID),
														stripNumber
														);

			++nhits;

			// contain (name, eta, phi, multiPlet)
			m_idHelper->get_detectorElement_hash(layerID, detectorElementHash);

			const MuonGM::MuonChannelDesign* mmChannelDes = detectorReadoutElement->getDesign(digitID);
			double distToChannel_withStripID = mmChannelDes->distanceToChannel(posOnSurf, stripNumber);
			double distToChannel = mmChannelDes->distanceToChannel(posOnSurf);
			ATH_MSG_DEBUG(" looking up collection using detectorElementHash "
							<< (int)detectorElementHash
							<< " "
							<< m_idHelper->print_to_string(layerID)
							<< " digitID: "
							<< m_idHelper->print_to_string(digitID)
							);

			if ( fabs(distToChannel_withStripID - distToChannel) > mmChannelDes->channelWidth(posOnSurf)) {
				ATH_MSG_WARNING( "Found: distToChannel_withStripID: " << distToChannel_withStripID << " != distToChannel: " << distToChannel  );
				m_exitcode = 12;
				if(m_writeOutputFile) m_ntuple->Fill();
				continue;
			}

			// Obtain Magnetic Field At Detector Surface
			Amg::Vector3D hitOnSurfaceGlobal = surf.transform()*hitOnSurface;
			Amg::Vector3D magneticField;
			m_magFieldSvc->getField(&hitOnSurfaceGlobal, &magneticField);

			// B-field in local cordinate, X ~ #strip, increasing to outer R, Z ~ global Z but positive to IP
			Amg::Vector3D localMagneticField = surf.transform().inverse()*magneticField
													- surf.transform().inverse()*Amg::Vector3D(0,0,0);
			if( (roParam.readoutSide).at(m_muonHelper->GetLayer(simId)-1) == -1 )
				localMagneticField = Amg::Vector3D(localMagneticField.x(), -localMagneticField.y(), -localMagneticField.z() );


			//
			// Angles, Geometry, and Coordinates. Oh my!
			//
			////////////////////////////////////////////////////////////////////


			////////////////////////////////////////////////////////////////////
			//
			// Strip Response Simulation For This Hit
			//

			const MmDigitToolInput stripDigitInput( stripNumber,
													distToChannel,
													inAngle_XZ,
													localMagneticField,
													detectorReadoutElement->numberOfStrips(layerID),
													m_idHelper->gasGap(layerID),
													eventTime+globalHitTime
													);

			m_n_hitStripID=stripNumber;
			m_n_hitDistToChannel=distToChannel;
			m_n_hitIncomingAngle=inAngle_XZ;
			m_n_hitIncomingAngleRads = inAngle_XZ * CLHEP::degree;
			m_n_hitOnSurface_x=posOnSurf.x();
			m_n_hitOnSurface_y = posOnSurf.y();

			MmStripToolOutput tmpStripOutput = m_StripsResponseSimulation->GetResponseFrom(stripDigitInput);
			MmElectronicsToolInput stripDigitOutput( tmpStripOutput.NumberOfStripsPos(), tmpStripOutput.chipCharge(), tmpStripOutput.chipTime(), digitID , hit.kineticEnergy());

			// This block is purely validation
			for(size_t i = 0; i<tmpStripOutput.NumberOfStripsPos().size(); i++){
				int tmpStripID = tmpStripOutput.NumberOfStripsPos().at(i);
				bool isValid;
				Identifier cr_id = m_idHelper->channelID(stName, m_idHelper->stationEta(layerID), m_idHelper->stationPhi(layerID), m_idHelper->multilayer(layerID), m_idHelper->gasGap(layerID), tmpStripID, true, &isValid);
				if (!isValid) {
					ATH_MSG_WARNING( "MicroMegas digitization: failed to create a valid ID for (chip response) strip n. " << tmpStripID << "; associated positions will be set to 0.0." );
				} else {
					Amg::Vector2D cr_strip_pos(0., 0.);
					if ( !detectorReadoutElement->stripPosition(cr_id,cr_strip_pos) ) {
						ATH_MSG_WARNING("MicroMegas digitization: failed to associate a valid local position for (chip response) strip n. " << tmpStripID << "; associated positions will be set to 0.0.");
					}
				}
			}


			v_stripDigitOutput.push_back(stripDigitOutput);

			//
			// Strip Response Simulation For This Hit
			//
			////////////////////////////////////////////////////////////////////

			previousHit = &hit;

		} // Hit Loop

		// Now at Detector Element Level (VMM)

		if(v_stripDigitOutput.size()==0){
			ATH_MSG_DEBUG ( "MmDigitizationTool::doDigitization() -- there is no strip response on this VMM." );
			continue;
		}


		////////////////////////////////////////////////////////////////////
		//
		// VMM Simulation
		//


		// Combine all strips (for this VMM) into a single VMM-level object
		MmElectronicsToolInput stripDigitOutputAllHits = combinedStripResponseAllHits(v_stripDigitOutput);


		//-----------------------------------------------------------
		// Create Electronics Output with peak finding algorithm
		//-----------------------------------------------------------
		MmDigitToolOutput electronicsOutput( m_ElectronicsResponseSimulation->getPeakResponseFrom(stripdigitOutputAllHits) );
		if(!electronicsOutput.isValid()) {
			ATH_MSG_DEBUG ( "MmDigitizationTool::doDigitization() -- there is no electronics response even though there is a strip response." );
		}
		//-----------------------------------------------------------
		// Create Electronics Output with threshold
		//-----------------------------------------------------------
		MmDigitToolOutput electronicsThresholdOutput( m_ElectronicsResponseSimulation->getThresholdResponseFrom(stripdigitOutputAllHits) );
		if(!electronicsThresholdOutput.isValid())
			ATH_MSG_DEBUG ( "MmDigitizationTool::doDigitization() -- there is no electronics response for TRIGGER even though there is a strip response." );

		//
		// Apply Dead-time for strip
		//
		MmDigitToolOutput electronicsThresholdOutputAppliedStripDeadTime (m_ElectronicsResponseSimulation->applyDeadTimeStrip(electronicsThresholdOutput));
		//
		// ART:The fastest strip signal per VMM id should be selected for trigger
		//
		int chMax = m_idHelper->channelMax(layerID);
		int stationEta = m_idHelper->stationEta(layerID);
		MmElectronicsToolTriggerOutput electronicsTriggerOutput (m_ElectronicsResponseSimulation->getTheFastestSignalInVMM(electronicsThresholdOutputAppliedStripDeadTime, chMax, stationEta));

		// Apply Dead-time in ART
		MmElectronicsToolTriggerOutput electronicsTriggerOutputAppliedARTDeadTime (m_ElectronicsResponseSimulation->applyDeadTimeART(electronicsTriggerOutput));

		// To apply an arbitrary time-smearing of VMM signals
		MmElectronicsToolTriggerOutput electronicsTriggerOutputAppliedARTTiming (m_ElectronicsResponseSimulation->applyARTTiming(electronicsTriggerOutputAppliedARTDeadTime,0.,0.));


		//
		// VMM Simulation
		//
		////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////
		//
		// (VMM-Level) Output Of Digitization
		//

		MmDigit*  newDigit = new MmDigit(   stripdigitOutputAllHits.digitID(),
											// --- We had ElectronicsOutput here, instead of StripResponse Output because but it's no longer useful
											electronicsOutput.stripTime(),
											electronicsOutput.stripPos(),
											electronicsOutput.stripCharge(),
											// ---------------------
											electronicsOutput.stripTime(),
											electronicsOutput.stripPos(),
											electronicsOutput.stripCharge(),
											electronicsTriggerOutputAppliedARTTiming.chipTime(),
											electronicsTriggerOutputAppliedARTTiming.NumberOfStripsPos(),
											electronicsTriggerOutputAppliedARTTiming.chipCharge(),
											electronicsTriggerOutputAppliedARTTiming.VMMid(),
											electronicsTriggerOutputAppliedARTTiming.MMFEVMMid()
										);

		// The collections should use the detector element hashes not the module hashes to be consistent with the PRD granularity.
		// IdentifierHash detIdhash ;
		// set RE hash id
		const Identifier elemId = m_idHelper->elementID( stripdigitOutputAllHits.digitID() );
		m_idHelper->get_detectorElement_hash( elemId, detectorElementHash );

		MmDigitCollection* digitCollection = 0;
		// put new collection in storegate
		// Get the messaging service, print where you are
		m_activeStore->setStore( &*m_storeGateService );
		MmDigitContainer::const_iterator it_coll = m_digitContainer->indexFind(detectorElementHash );
		if (m_digitContainer->end() ==  it_coll) {
			digitCollection = new MmDigitCollection( elemId, detectorElementHash );
			digitCollection->push_back(newDigit);
			m_activeStore->setStore( &*m_storeGateService );
			ATH_CHECK( m_digitContainer->addCollection(digitCollection, detectorElementHash ) );
		}
		else {
			digitCollection = const_cast<MmDigitCollection*>( it_coll->cptr() );
			digitCollection->push_back(newDigit);
		}

		//
		// (VMM-Level) Output Of Digitization
		//
		////////////////////////////////////////////////////////////////////



		v_StripdigitOutput.clear();

	}

	ATH_MSG_DEBUG ( "MmDigitization Done!"  );

	if (m_timedHitCollection_MM){
		delete m_timedHitCollection_MM;
		m_timedHitCollection_MM = 0;
	}

	return StatusCode::SUCCESS;
}

MmElectronicsToolInput MmDigitizationTool::combinedStripResponseAllHits(const std::vector< MmElectronicsToolInput > & v_stripDigitOutput){

	std::vector <int> v_stripStripResponseAllHits;
	std::vector < std::vector <float> > v_timeStripResponseAllHits;
	std::vector < std::vector <float> > v_qStripResponseAllHits;
	v_stripStripResponseAllHits.clear();
	v_timeStripResponseAllHits.clear();
	v_qStripResponseAllHits.clear();

	Identifier digitID;
	float max_kineticEnergy = 0.0;

	// Loop over strip digit output elements
	for(auto& i_stripDigitOutput:v_stripDigitOutput){

		//--- Just to get Digit id with the largest kinetic energy, but the Digit id is no longer meaningful
		if(i_stripDigitOutput.kineticEnergy()>max_kineticEnergy){
			digitID = i_stripDigitOutput.digitID();
			max_kineticEnergy = i_stripDigitOutput.kineticEnergy();
		}
		//---
		for(size_t i = 0; i<i_stripDigitOutput.NumberOfStripsPos().size(); i++){
			int strip_id = i_stripDigitOutput.NumberOfStripsPos()[i];
			bool found = false;

			for(size_t ii = 0; ii<v_stripStripResponseAllHits.size(); ii++){
				if(v_stripStripResponseAllHits[ii]==strip_id){
					for(size_t iii = 0; iii<(i_stripDigitOutput.chipTime()[i]).size(); iii++){
						v_timeStripResponseAllHits[ii].push_back(i_stripDigitOutput.chipTime()[i].at(iii));
						v_qStripResponseAllHits[ii].push_back(i_stripDigitOutput.chipCharge()[i].at(iii));
					}
					found=true;
				}
			}
			if(!found){ // strip id not in vector, add new entry
				v_stripStripResponseAllHits.push_back(strip_id);
				v_timeStripResponseAllHits.push_back(i_stripDigitOutput.chipTime()[i]);
				v_qStripResponseAllHits.push_back(i_stripDigitOutput.chipCharge()[i]);
			}
		}
	}

	MmElectronicsToolInput stripDigitOutputAllHits( v_stripStripResponseAllHits,
													v_qStripResponseAllHits,
													v_timeStripResponseAllHits,
													digitID,
													max_kineticEnergy
													);

	return stripDigitOutputAllHits;
}
/*******************************************************************************/
bool MmDigitizationTool::checkMMSimHit( const GenericMuonSimHit& /*hit*/ ) const {
	return true;
}