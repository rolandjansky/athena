/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// MM_DigitizationTool
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
#include "MM_Digitization/MM_DigitizationTool.h"
#include "MM_Digitization/IMM_DigitizationTool.h"
#include "MM_Digitization/MM_DigitToolInput.h"
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

#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"

//VMM Mapping
#include "MM_Digitization/MM_StripVmmMappingTool.h"

//ROOT
// #include "TH1.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace MuonGM;

/*******************************************************************************/
MM_DigitizationTool::MM_DigitizationTool(const std::string& type, const std::string& name, const IInterface* parent):
	PileUpToolBase(type, name, parent),

	// Services
	m_storeGateService("StoreGateSvc", name),
	m_magFieldSvc("AtlasFieldSvc",name) ,
	m_mergeSvc(nullptr),
	m_rndmSvc("AtRndmGenSvc", name ),
	m_rndmEngine(nullptr),
	m_rndmEngineName("MuonDigitization"),

	// Containers
	m_digitContainer(nullptr),
	m_sdoContainer(nullptr),

	// Tools
	m_digitTool("MM_Response_DigitTool", this),
	m_file(nullptr),
	m_ntuple(nullptr),

	// Settings
	m_energyThreshold(50.),
	m_maskMultiplet(0),
	m_writeOutputFile(false),
	m_timedHitCollection_MM(nullptr),

	m_inputObjectName(""),
	m_outputObjectName(""),
	m_outputSDOName(""),

	m_checkMMSimHits(true),
	m_useTimeWindow(true),

	m_timeWindowLowerOffset(0),
	m_timeWindowUpperOffset(0),

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

	m_vmmReadoutMode(""),
	m_vmmARTMode(""),

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
	m_tofCorrection(-99999999.),
	m_bunchTime(-99999999.),
	m_globalHitTime(-99999999.),
	m_eventTime(-99999999.)
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

	// Constants vars for the MM_StripsResponseSimulation class
	// qThreshold=2e, we accept a good strip if the charge is >=2e
	declareProperty("qThreshold",                 m_qThreshold = 0.001);     // Charge Threshold
	declareProperty("TransverseDiffusionSigma",   m_transverseDiffusionSigma = 0.360/10.);   // Diffusion Constants for electron propagation
	declareProperty("LongitudinalDiffusionSigma", m_longitudinalDiffusionSigma = 0.190/10.);
	declareProperty("DriftGapWidth",              m_driftGapWidth = 5.168);  // Drift Gap Width of 5.04 mm + 0.128 mm (the amplification gap)
	declareProperty("DriftVelocity",              m_driftVelocity = 0.047);  // Drift velocity in [mm/ns]
	declareProperty("crossTalk1",		          m_crossTalk1 = 0.1);       // Strip Cross Talk with Nearest Neighbor
	declareProperty("crossTalk2",		          m_crossTalk2 = 0.03);      // Strip Cross Talk with 2nd Nearest Neighbor

	declareProperty("vmmReadoutMode",             m_vmmReadoutMode = "peak"      ); // For readout (DAQ) path. Can be "peak" or "threshold"
	declareProperty("vmmARTMode",                 m_vmmARTMode     = "threshold" ); // For ART (trigger) path. Can be "peak" or "threshold"

	// Constants vars for the MM_ElectronicsResponseSimulation
	declareProperty("peakTime",                m_peakTime = 50.);                 // The VMM peak time setting.
	declareProperty("electronicsThreshold",    m_electronicsThreshold = 6000.0);  // 2*(Intrinsic noise ~3k e)
	declareProperty("StripDeadTime",           m_stripdeadtime = 200.0);          // default value 200 ns = 8 BCs
	declareProperty("ARTDeadTime",             m_ARTdeadtime   = 200.0);          // default value 200 ns = 8 BCs

}

/*******************************************************************************/
// member function implementation
//--------------------------------------------
StatusCode MM_DigitizationTool::initialize() {

	ATH_MSG_DEBUG ("MM_DigitizationTool:: in initialize()") ;
	ATH_MSG_DEBUG ( "Configuration  MM_DigitizationTool " );
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
	StoreGateSvc* detStore=nullptr;
	m_MuonGeoMgr=nullptr;
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
	if (m_rndmEngine == nullptr) {
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
		m_ntuple->Branch("tofCorrection",&m_tofCorrection);
		m_ntuple->Branch("bunchTime",&m_bunchTime);
		m_ntuple->Branch("globalHitTime",&m_globalHitTime);
		m_ntuple->Branch("eventTime",&m_eventTime);
	}

	// StripsResponseSimulation Creation
	m_StripsResponseSimulation = new MM_StripsResponseSimulation();
	m_StripsResponseSimulation->setQThreshold(m_qThreshold);
	m_StripsResponseSimulation->setTransverseDiffusionSigma(m_transverseDiffusionSigma);
	m_StripsResponseSimulation->setLongitudinalDiffusionSigma(m_longitudinalDiffusionSigma);
	m_StripsResponseSimulation->setDriftGapWidth(m_driftGapWidth);
	m_StripsResponseSimulation->setDriftVelocity(m_driftVelocity);
	m_StripsResponseSimulation->setCrossTalk1(m_crossTalk1);
	m_StripsResponseSimulation->setCrossTalk2(m_crossTalk2);
	m_StripsResponseSimulation->initialize();

	// ElectronicsResponseSimulation Creation
	m_ElectronicsResponseSimulation = new MM_ElectronicsResponseSimulation();
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


	// Configuring various VMM modes of signal readout
	//
	if(      TString(m_vmmReadoutMode).Contains("peak",     TString::kIgnoreCase) ) m_vmmReadoutMode = "peak";
	else if( TString(m_vmmReadoutMode).Contains("threshold",TString::kIgnoreCase) ) m_vmmReadoutMode = "threshold";
	else {
		ATH_MSG_ERROR("MM_DigitizationTool can't interperet vmmReadoutMode option! (Should be 'peak' or 'threshold'.) Contains: "
						<< m_vmmReadoutMode);
	}
	if(      TString(m_vmmARTMode).Contains("peak",     TString::kIgnoreCase) ) m_vmmARTMode = "peak";
	else if( TString(m_vmmARTMode).Contains("threshold",TString::kIgnoreCase) ) m_vmmARTMode = "threshold";
	else {
		ATH_MSG_ERROR("MM_DigitizationTool can't interperet vmmReadoutMode option! (Should be 'peak' or 'threshold'.) Contains: "
						<< m_vmmARTMode);
	}

	return StatusCode::SUCCESS;
}
/*******************************************************************************/
//----------------------------------------------------------------------
// PrepareEvent method:
//----------------------------------------------------------------------
StatusCode MM_DigitizationTool::prepareEvent(unsigned int nInputEvents) {

	ATH_MSG_DEBUG("MM_DigitizationTool::prepareEvent() called for " << nInputEvents << " input events" );

	m_MMHitCollList.clear();

	if(!m_timedHitCollection_MM) {
		m_timedHitCollection_MM = new TimedHitCollection<MMSimHit>();
	} else {
		ATH_MSG_ERROR ( "m_timedHitCollection_MM is not null" );
		return StatusCode::FAILURE;
	}

	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MM_DigitizationTool::processBunchXing(int bunchXing,
	SubEventIterator bSubEvents,
	SubEventIterator eSubEvents) {

	ATH_MSG_DEBUG ( "MM_DigitizationTool::in processBunchXing()"  << bunchXing );

        typedef PileUpMergeSvc::TimedList<MMSimHitCollection>::type TimedHitCollList;
        TimedHitCollList hitCollList;

        if (!(m_mergeSvc->retrieveSubSetEvtData(m_inputObjectName, hitCollList, bunchXing,
                                                bSubEvents, eSubEvents).isSuccess()) &&
            hitCollList.size() == 0) {
          ATH_MSG_ERROR("Could not fill TimedHitCollList");
          return StatusCode::FAILURE;
        } else {
          ATH_MSG_VERBOSE(hitCollList.size() << " MMSimHitCollection with key " <<
                          m_inputObjectName << " found");
        }
      
        TimedHitCollList::iterator iColl(hitCollList.begin());
        TimedHitCollList::iterator endColl(hitCollList.end());

        // Iterating over the list of collections
        for( ; iColl != endColl; iColl++){
     
          MMSimHitCollection *hitCollPtr = new MMSimHitCollection(*iColl->second);
          PileUpTimeEventIndex timeIndex(iColl->first);
     
          ATH_MSG_DEBUG("MMSimHitCollection found with " << hitCollPtr->size() <<
                        " hits");
          ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
                          << " index: " << timeIndex.index()
                          << " type: " << timeIndex.type());
     
          m_timedHitCollection_MM->insert(timeIndex, hitCollPtr);
          m_MMHitCollList.push_back(hitCollPtr);
     
        }

	return StatusCode::SUCCESS;
}

/*******************************************************************************/
StatusCode MM_DigitizationTool::getNextEvent() {

// Get next event and extract collection of hit collections:
// This is applicable to non-PileUp Event...

	ATH_MSG_DEBUG ( "MM_DigitizationTool::getNextEvent()" );

	if (!m_mergeSvc) {
		ATH_CHECK(service("PileUpMergeSvc", m_mergeSvc, true));
	}

	//  get the container(s)
	typedef PileUpMergeSvc::TimedList<MMSimHitCollection>::type TimedHitCollList;

	//this is a list<info<time_t, DataLink<MMSimHitCollection> > >
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
	if(!m_timedHitCollection_MM) {
		m_timedHitCollection_MM = new TimedHitCollection<MMSimHit>();
	}else{
		ATH_MSG_ERROR ( "m_timedHitCollection_MM is not null" );
		return StatusCode::FAILURE;
	}

	//now merge all collections into one
	TimedHitCollList::iterator iColl(hitCollList.begin());
	TimedHitCollList::iterator endColl(hitCollList.end());

	// loop on the hit collections
	while(iColl != endColl) {
		const MMSimHitCollection* tmpColl(iColl->second);
		m_timedHitCollection_MM->insert(iColl->first, tmpColl);
		ATH_MSG_DEBUG ( "MMSimHitCollection found with " << tmpColl->size() << " hits"  );
		++iColl;
	}
	return StatusCode::SUCCESS;

}
/*******************************************************************************/
StatusCode MM_DigitizationTool::mergeEvent() {

	ATH_MSG_VERBOSE ( "MM_DigitizationTool::in mergeEvent()" );

	// Cleanup and record the Digit container in StoreGate
	ATH_CHECK( recordDigitAndSdoContainers() );
	ATH_CHECK( doDigitization() );

	// reset the pointer (delete null pointer should be safe)
	if (m_timedHitCollection_MM){
		delete m_timedHitCollection_MM;
		m_timedHitCollection_MM = nullptr;
	}

	// remove cloned one in processBunchXing......
	std::list<MMSimHitCollection*>::iterator MMHitColl = m_MMHitCollList.begin();
	std::list<MMSimHitCollection*>::iterator MMHitCollEnd = m_MMHitCollList.end();
	while(MMHitColl!=MMHitCollEnd) {
		delete (*MMHitColl);
		++MMHitColl;
	}
	m_MMHitCollList.clear();
	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MM_DigitizationTool::digitize() {
	return this->processAllSubEvents();
}
/*******************************************************************************/
StatusCode MM_DigitizationTool::processAllSubEvents() {

	ATH_MSG_DEBUG ("MM_DigitizationTool::processAllSubEvents()");

	ATH_CHECK( recordDigitAndSdoContainers() );

	//merging of the hit collection in getNextEvent method

	if (m_timedHitCollection_MM == nullptr) ATH_CHECK( getNextEvent() );

	ATH_CHECK( doDigitization() );

	// reset the pointer (delete null pointer should be safe)
	if (m_timedHitCollection_MM){
		delete m_timedHitCollection_MM;
		m_timedHitCollection_MM = nullptr;
	}
	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MM_DigitizationTool::finalize() {
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
StatusCode MM_DigitizationTool::recordDigitAndSdoContainers() {

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
StatusCode MM_DigitizationTool::doDigitization() {


	MMSimHitCollection* inputSimHitColl=nullptr;

	IdentifierHash detectorElementHash=0;

	inputSimHitColl = new MMSimHitCollection("MicromegasSensitiveDetector");
	ATH_CHECK( m_storeGateService->record(inputSimHitColl,"InputMicroMegasHits") );

	if( m_maskMultiplet == 3 ) {

		return StatusCode::SUCCESS;
	}

	// Perform null check on m_thpcCSC
	if(!m_timedHitCollection_MM) {
		ATH_MSG_ERROR ( "m_timedHitCollection_MM is null" );
		return StatusCode::FAILURE;
	}

	std::vector<MM_ElectronicsToolInput> v_stripDigitOutput;
	v_stripDigitOutput.clear();

	//iterate over hits and fill id-keyed drift time map
	TimedHitCollection< MMSimHit >::const_iterator i, e;

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


			TimedHitPtr<MMSimHit> phit = *i++;
			m_eventTime = phit.eventTime();
			const MMSimHit& hit(*phit);

			m_n_hitPDGId = hit.particleEncoding();
			m_n_hitDepositEnergy = hit.depositEnergy();
			m_n_hitKineticEnergy = hit.kineticEnergy();

			const Amg::Vector3D globalHitPosition = hit.globalPosition();

			m_globalHitTime = hit.globalTime();
			m_tofCorrection = globalHitPosition.mag()/CLHEP::c_light;
			m_bunchTime = m_globalHitTime - m_tofCorrection + m_eventTime;

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

			const int hitID = hit.MMId();
			// the G4 time or TOF from IP
			// double G4Time(hit.globalTime());
			// see what are the members of MMSimHit

			// convert sim id helper to offline id
			MM_SimIdToOfflineId simToOffline(*m_idHelper);

			//get the hit Identifier and info
			int simId=hit.MMId();
			layerID = simToOffline.convert(simId);

			// Read the information about the Micro Megas hit
			ATH_MSG_DEBUG ( "> hitID  "
							<<     hitID
							<< " Hit bunch time  "
							<<     m_bunchTime
							<< " tot "
							<<     m_globalHitTime
							<< " tof/G4 time "
							<<     hit.globalTime()
							<< " globalHitPosition "
							<<     globalHitPosition
							<< "hit: r "
							<<     globalHitPosition.perp()
							<< " z "
							<<     globalHitPosition.z()
							<< " mclink "
							<<     hit.particleLink()
							<< " station eta "
							<<     m_idHelper->stationEta(layerID)
							<< " station phi "
							<<     m_idHelper->stationPhi(layerID)
							<< " multiplet "
							<<     m_idHelper->multilayer(layerID)
							);

			// For collection of inputs to throw back in SG

			MMSimHit* copyHit = new MMSimHit( hitID,
																m_globalHitTime+m_eventTime,
																globalHitPosition,
																hit.particleEncoding(),
																hit.kineticEnergy(),
																hit.globalDirection(),
																hit.depositEnergy(),
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
				ATH_MSG_WARNING("layerID does not represent a valid MM layer: "
								<< m_idHelper->stationNameString(m_idHelper->stationName(layerID)) );
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
				continue;
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
									<<     isSmall
									<< " eta "
									<<     m_idHelper->stationEta(layerID)
									<< " phi "
									<<     m_idHelper->stationPhi(layerID)
									<< " ml "
									<<     m_idHelper->multilayer(layerID)
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

			// This is not an incident angle yet. It's atan(z/x),
			// ... so it's the complement of the angle w.r.t. a vector normal to the detector surface
			float inAngleCompliment_XZ =  localHitDirection.angleXZ() / CLHEP::degree;
			float inAngleCompliment_YZ =  localHitDirection.angleYZ() / CLHEP::degree;

			// This is basically to handle the atan ambiguity
			if(inAngleCompliment_XZ < 0.0) inAngleCompliment_XZ += 180;
			if(inAngleCompliment_YZ < 0.0) inAngleCompliment_YZ += 180;

			// This gets the actual incidence angle from its complement.
			float inAngle_XZ = 90. - inAngleCompliment_XZ;
			float inAngle_YZ = 90. - inAngleCompliment_YZ;

			ATH_MSG_DEBUG(  "At eta: "
							<<     m_idHelper->stationEta(layerID)
							<< " phi: "
							<<     m_idHelper->stationPhi(layerID)
							<< " Readout Side: "
							<<     (roParam.readoutSide).at(m_muonHelper->GetLayer(simId)-1)
							<< " Layer: "
							<<     m_muonHelper->GetLayer(simId)
							<< "\n\t\t\t inAngle_XZ (degrees): "
							<<     inAngle_XZ
							<< " inAngle_YZ (degrees): "
							<<     inAngle_YZ
							);


			// compute the hit position on the readout plane (same as in MuonFastDigitization)
			Amg::Vector3D stripLayerPosition = surf.transform().inverse()*globalHitPosition;
			Amg::Vector2D positionOnSurfaceUnprojected(stripLayerPosition.x(),stripLayerPosition.y());

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
			Amg::Vector2D positionOnSurface (hitOnSurface.x(), hitOnSurface.y());

			// Account For Time Offset
			double shiftTimeOffset = (m_globalHitTime - m_tofCorrection)* m_driftVelocity;
			Amg::Vector3D hitAfterTimeShift(hitOnSurface.x(),hitOnSurface.y(),shiftTimeOffset);
			Amg::Vector3D hitAfterTimeShiftOnSurface = hitAfterTimeShift - (shiftTimeOffset/localDirectionTime.z())*localDirectionTime;

			if( fabs(hitAfterTimeShiftOnSurface.z()) > 0.1 ) ATH_MSG_WARNING("Bad propagation to surface after time shift " << hitAfterTimeShiftOnSurface );

			// Don't consider electron hits below m_energyThreshold
			if( hit.kineticEnergy() < m_energyThreshold && abs(hit.particleEncoding())==11) {
				m_exitcode = 5;
				if(m_writeOutputFile) m_ntuple->Fill();
				continue;
			}

			// Perform Bound Check
			if( !surf.insideBounds(positionOnSurface) ){
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

			int stripNumber = detectorReadoutElement->stripNumber(positionOnSurface,layerID);
			Amg::Vector2D tmp (stripLayerPosition.x(), stripLayerPosition.y());

			if( stripNumber == -1 ){
				ATH_MSG_WARNING("!!! Failed to obtain strip number "
								<< m_idHelper->print_to_string(layerID)
								<<  "\n\t\t with pos "
								<< positionOnSurface
								<< " z "
								<< stripLayerPosition.z()
								<< " eKin: "
								<< hit.kineticEnergy()
								<< " eDep: "
								<< hit.depositEnergy()
								<< " unprojectedStrip: "
								<< detectorReadoutElement->stripNumber(positionOnSurfaceUnprojected, layerID)
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


			const MuonGM::MuonChannelDesign* mmChannelDesign = detectorReadoutElement->getDesign(digitID);
                        // As of September 12 2018, to reduce the number of errors and warnings in MM digitization,
                        // we assign the strips in the dead regions stripNumber=1 (short term fix).
                        // The distToChannel validation breaks when we do this so we must add another if statement
                        // These changes should be reverted once we arrive to a proper solution for these dead strips
                        // Alexandre Laurier 12 Sept 2018
	                    	double distToChannelWithStripID;
	                    	double distToChannel;
                        if (stripNumber ==1){
	                    	    distToChannelWithStripID =0.; 
	                    	    distToChannel =0.; 
                        }
                        else{
	                    	    distToChannelWithStripID = mmChannelDesign->distanceToChannel(positionOnSurface, stripNumber);
	                    	    distToChannel = mmChannelDesign->distanceToChannel(positionOnSurface);
                        }
	                    	ATH_MSG_DEBUG(" looking up collection using detectorElementHash "
	                    					<< (int)detectorElementHash
	                    					<< " "
	                    					<< m_idHelper->print_to_string(layerID)
	                    					<< " digitID: "
	                    					<< m_idHelper->print_to_string(digitID)
	                    					);

	                    	if ( fabs(distToChannelWithStripID - distToChannel) > mmChannelDesign->channelWidth(positionOnSurface)) {
	                    		ATH_MSG_WARNING( "Found: distToChannelWithStripID: " << distToChannelWithStripID << " != distToChannel: " << distToChannel  );
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

			const MM_DigitToolInput stripDigitInput( stripNumber,
													distToChannel,
													inAngle_XZ,
													inAngle_YZ,
													localMagneticField,
													detectorReadoutElement->numberOfStrips(layerID),
													m_idHelper->gasGap(layerID),
													m_eventTime+m_globalHitTime
													);


			// fill the SDO collection in StoreGate
			// create here deposit for MuonSimData, link and tof
			//
			// Since we have output based on channel, instead of hit, the SDO and digit ID are No longer meaningless. 2016/06/27 T.Saito
			//
			// digitize input for strip response

			MuonSimData::Deposit deposit(hit.particleLink(), MuonMCData(hitOnSurface.x(),hitOnSurface.y()));

			//Record the SDO collection in StoreGate
			std::vector<MuonSimData::Deposit> deposits;
			deposits.push_back(deposit);
                        MuonSimData simData(deposits,0);
                        simData.setPosition(hitOnSurfaceGlobal);
                        simData.setTime(m_globalHitTime);
                        m_sdoContainer->insert ( std::make_pair ( digitID, simData ) );
			ATH_MSG_DEBUG(" added MM SDO " <<  m_sdoContainer->size());


			m_n_hitStripID=stripNumber;
			m_n_hitDistToChannel=distToChannel;
			m_n_hitIncomingAngle=inAngle_XZ;
			m_n_hitIncomingAngleRads = inAngle_XZ * CLHEP::degree;
			m_n_hitOnSurface_x=positionOnSurface.x();
			m_n_hitOnSurface_y = positionOnSurface.y();

			MM_StripToolOutput tmpStripOutput = m_StripsResponseSimulation->GetResponseFrom(stripDigitInput);
			MM_ElectronicsToolInput stripDigitOutput( tmpStripOutput.NumberOfStripsPos(), tmpStripOutput.chipCharge(), tmpStripOutput.chipTime(), digitID , hit.kineticEnergy());

			// This block is purely validation
			if (stripNumber!=1){ // Extra if statement from quick fix from deadstrip = #1
			    for(size_t i = 0; i<tmpStripOutput.NumberOfStripsPos().size(); i++){
			    	int tmpStripID = tmpStripOutput.NumberOfStripsPos().at(i);
			    	bool isValid;
			    	Identifier cr_id = m_idHelper->channelID(stName, m_idHelper->stationEta(layerID), m_idHelper->stationPhi(layerID), m_idHelper->multilayer(layerID), m_idHelper->gasGap(layerID), tmpStripID, true, &isValid);
			    	if (!isValid) {
			    		ATH_MSG_WARNING( "MicroMegas digitization: failed to create a valid ID for (chip response) strip n. " << tmpStripID << "; associated positions will be set to 0.0." );
			    	} else {
			    		Amg::Vector2D cr_strip_pos(0., 0.);
			    		if ( !detectorReadoutElement->stripPosition(cr_id,cr_strip_pos) ) {
			    			ATH_MSG_WARNING("MicroMegas digitization: failed to associate a valid local position for (chip response) strip n. "
			    							<< tmpStripID
			    							<< "; associated positions will be set to 0.0."
			    							);
			    		}
			    	}
			    }
                        }


			v_stripDigitOutput.push_back(stripDigitOutput);

			//
			// Strip Response Simulation For This Hit
			//
			////////////////////////////////////////////////////////////////////


		} // Hit Loop

		// Now at Detector Element Level (VMM)

		if(v_stripDigitOutput.size()==0){
			ATH_MSG_DEBUG ( "MM_DigitizationTool::doDigitization() -- there is no strip response on this VMM." );
			continue;
		}


		////////////////////////////////////////////////////////////////////
		//
		// VMM Simulation
		//


		// Combine all strips (for this VMM) into a single VMM-level object
		//
		MM_ElectronicsToolInput stripDigitOutputAllHits = combinedStripResponseAllHits(v_stripDigitOutput);



		// Create Electronics Output with peak finding setting
		//
		MM_DigitToolOutput electronicsPeakOutput( m_ElectronicsResponseSimulation->getPeakResponseFrom(stripDigitOutputAllHits) );
		if(!electronicsPeakOutput.isValid())
			ATH_MSG_DEBUG ( "MM_DigitizationTool::doDigitization() -- there is no electronics response (peak finding mode) even though there is a strip response." );

		// Create Electronics Output with threshold setting
		//
		MM_DigitToolOutput electronicsThresholdOutput( m_ElectronicsResponseSimulation->getThresholdResponseFrom(stripDigitOutputAllHits) );
		if(!electronicsThresholdOutput.isValid())
			ATH_MSG_DEBUG ( "MM_DigitizationTool::doDigitization() -- there is no electronics response (threshold mode) even though there is a strip response." );



		// Choose which of the above outputs is used for readout
		//
		MM_DigitToolOutput * electronicsOutputForReadout(0);
		if (m_vmmReadoutMode      ==   "peak"     ) electronicsOutputForReadout = & electronicsPeakOutput;
		else if (m_vmmReadoutMode ==   "threshold") electronicsOutputForReadout = & electronicsThresholdOutput;
		else ATH_MSG_ERROR("Failed to setup readout signal from VMM. Readout mode incorrectly set");
		// but this should be impossible from initialization checks

		// Choose which of the above outputs is used for triggering
		//
		MM_DigitToolOutput * electronicsOutputForTriggerPath(0);
		if (m_vmmARTMode          ==   "peak"     ) electronicsOutputForTriggerPath = & electronicsPeakOutput;
		else if (m_vmmARTMode     ==   "threshold") electronicsOutputForTriggerPath = & electronicsThresholdOutput;
		else ATH_MSG_ERROR("Failed to setup trigger signal from VMM. Readout mode incorrectly set");
		// but this should be impossible from initialization checks



		// Apply Dead-time for strip
		//
		MM_DigitToolOutput electronicsOutputForTriggerPathWStripDeadTime (m_ElectronicsResponseSimulation->applyDeadTimeStrip(*electronicsOutputForTriggerPath));

		// ART: The fastest strip signal per VMM id should be selected for trigger
		//
		int chMax = m_idHelper->channelMax(layerID);
		int stationEta = m_idHelper->stationEta(layerID);
		MM_ElectronicsToolTriggerOutput electronicsTriggerOutput (m_ElectronicsResponseSimulation->getTheFastestSignalInVMM(electronicsOutputForTriggerPathWStripDeadTime, chMax, stationEta));

		// Apply Dead-time in ART
		//
		MM_ElectronicsToolTriggerOutput electronicsTriggerOutputAppliedARTDeadTime (m_ElectronicsResponseSimulation->applyDeadTimeART(electronicsTriggerOutput));

		// To apply an arbitrary time-smearing of VMM signals
		//
		MM_ElectronicsToolTriggerOutput electronicsTriggerOutputAppliedARTTiming (m_ElectronicsResponseSimulation->applyARTTiming(electronicsTriggerOutputAppliedARTDeadTime,0.,0.));

		MM_ElectronicsToolTriggerOutput finalElectronicsTriggerOutput( electronicsTriggerOutputAppliedARTTiming );


		//
		// VMM Simulation
		//
		////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////
		//
		// (VMM-Level) Output Of Digitization
		//

		MmDigit*  newDigit = new MmDigit(   stripDigitOutputAllHits.digitID(),
											electronicsOutputForReadout->stripTime(),
											electronicsOutputForReadout->stripPos(),
											electronicsOutputForReadout->stripCharge(),
											electronicsOutputForReadout->stripTime(),
											electronicsOutputForReadout->stripPos(),
											electronicsOutputForReadout->stripCharge(),
											finalElectronicsTriggerOutput.chipTime(),
											finalElectronicsTriggerOutput.NumberOfStripsPos(),
											finalElectronicsTriggerOutput.chipCharge(),
											finalElectronicsTriggerOutput.VMMid(),
											finalElectronicsTriggerOutput.MMFEVMMid()
										);

		// The collections should use the detector element hashes not the module hashes to be consistent with the PRD granularity.
		// IdentifierHash detIdhash ;
		// set RE hash id
		const Identifier elemId = m_idHelper->elementID( stripDigitOutputAllHits.digitID() );
		m_idHelper->get_detectorElement_hash( elemId, detectorElementHash );

		MmDigitCollection* digitCollection = nullptr;
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
			digitCollection = const_cast<MmDigitCollection*>( *it_coll );
			digitCollection->push_back(newDigit);
		}

		//
		// (VMM-Level) Output Of Digitization
		//
		////////////////////////////////////////////////////////////////////



		v_stripDigitOutput.clear();

	}

	ATH_MSG_DEBUG ( "MM_Digitization Done!"  );

	if (m_timedHitCollection_MM){
		delete m_timedHitCollection_MM;
		m_timedHitCollection_MM = nullptr;
	}

	return StatusCode::SUCCESS;
}

MM_ElectronicsToolInput MM_DigitizationTool::combinedStripResponseAllHits(const std::vector< MM_ElectronicsToolInput > & v_stripDigitOutput){

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

	MM_ElectronicsToolInput stripDigitOutputAllHits( v_stripStripResponseAllHits,
													v_qStripResponseAllHits,
													v_timeStripResponseAllHits,
													digitID,
													max_kineticEnergy
													);

	return stripDigitOutputAllHits;
}
/*******************************************************************************/
bool MM_DigitizationTool::checkMMSimHit( const MMSimHit& /*hit*/ ) const {
	return true;
}
