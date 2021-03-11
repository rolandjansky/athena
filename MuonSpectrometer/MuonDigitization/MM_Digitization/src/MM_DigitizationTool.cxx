/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "MM_Digitization/MM_DigitToolInput.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"

//Geometry
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "PathResolver/PathResolver.h"
#include "AIDA/IHistogram1D.h"
#include "TrkSurfaces/Surface.h"

//Truth
#include "CLHEP/Units/PhysicalConstants.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "AtlasHepMC/GenParticle.h"

#include "AthenaKernel/RNGWrapper.h"

#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"

//VMM Mapping
#include "MM_Digitization/MM_StripVmmMappingTool.h"

//ROOT
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>


namespace {
 // thresholds for the shortest and longest strips
 // values from https://indico.cern.ch/event/1002207/contributions/4240818
 // slide 10

 static constexpr float maxNoise = 2600;
 static constexpr float minNoise = 1000;

}

using namespace MuonGM;

/*******************************************************************************/
MM_DigitizationTool::MM_DigitizationTool(const std::string& type, const std::string& name, const IInterface* parent):
  PileUpToolBase(type, name, parent),
  m_driftVelocity(0),
  // Tree Branches...
  m_n_Station_side(-INT_MAX),
  m_n_Station_eta(-INT_MAX),
  m_n_Station_phi(-INT_MAX),
  m_n_Station_multilayer(-INT_MAX),
  m_n_Station_layer(-INT_MAX),
  m_n_hitStripID(-INT_MAX),
  m_n_StrRespTrg_ID(-INT_MAX),
  m_n_strip_multiplicity(-INT_MAX),
  m_n_strip_multiplicity_2(-INT_MAX),
  m_n_hitPDGId(-INT_MAX),
  m_n_hitOnSurface_x(-DBL_MAX),
  m_n_hitOnSurface_y(-DBL_MAX),
  m_n_hitDistToChannel(-DBL_MAX),
  m_n_hitIncomingAngle(-DBL_MAX),
  m_n_StrRespTrg_Time(-DBL_MAX),
  m_n_hitIncomingAngleRads(-DBL_MAX),
  m_n_hitKineticEnergy(-DBL_MAX),
  m_n_hitDepositEnergy(-DBL_MAX),
  m_exitcode(0),
  // Timings
  m_tofCorrection(-FLT_MAX),
  m_bunchTime(-FLT_MAX),
  m_globalHitTime(-FLT_MAX),
  m_eventTime(-FLT_MAX),
  m_n_StrRespID(),
  m_n_StrRespCharge(),
  m_n_StrRespTime() {}
  

/*******************************************************************************/
// member function implementation
//--------------------------------------------
StatusCode MM_DigitizationTool::initialize() {

	ATH_MSG_DEBUG ("MM_DigitizationTool:: in initialize()") ;

	ATH_CHECK(m_rndmSvc.retrieve());
  

	// Initialize transient detector store and MuonGeoModel OR MuonDetDescrManager
	if(detStore()->contains<MuonGM::MuonDetectorManager>( "Muon" )){
		ATH_CHECK( detStore()->retrieve(m_MuonGeoMgr) );
		ATH_MSG_DEBUG ( "Retrieved MuonGeoModelDetectorManager from StoreGate" );
	}

	ATH_CHECK(m_idHelperSvc.retrieve());

	// Digit tools
	ATH_CHECK(m_digitTool.retrieve());

  if (m_hitsContainerKey.key().empty()) {
    ATH_MSG_FATAL("Property InputObjectName not set !");
    return StatusCode::FAILURE;
  }

  if (m_onlyUseContainerName) m_inputObjectName = m_hitsContainerKey.key();
  ATH_MSG_DEBUG("Input objects in container: '" << m_inputObjectName << "'");

  // Pile-up merge service
  if (m_onlyUseContainerName) {
    ATH_CHECK(m_mergeSvc.retrieve());
  }

  // Initialize ReadHandleKey
  ATH_CHECK(m_hitsContainerKey.initialize(!m_onlyUseContainerName));

  // Initialize the output WriteHandleKeys
  ATH_CHECK(m_outputDigitCollectionKey.initialize());
  ATH_CHECK(m_outputSDO_CollectionKey.initialize());
  ATH_MSG_DEBUG("Output Digits: '"<<m_outputDigitCollectionKey.key()<<"'");

  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  ATH_CHECK(m_calibrationTool.retrieve());

	//simulation identifier helper
	m_muonHelper = MicromegasHitIdHelper::GetHelper();

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
	m_StripsResponseSimulation = std::make_unique<MM_StripsResponseSimulation>();
	m_StripsResponseSimulation->writeOutputFile(m_writeOutputFile); 
	m_StripsResponseSimulation->setQThreshold(m_qThreshold);
	m_StripsResponseSimulation->setDriftGapWidth(m_driftGapWidth);
	m_StripsResponseSimulation->setCrossTalk1(m_crossTalk1);
	m_StripsResponseSimulation->setCrossTalk2(m_crossTalk2);
	
	// get gas properties from calibration tool 
    float longDiff, transDiff, vDrift, interactionDensityMean, interactionDensitySigma;
    TF1* lorentzAngleFunction;

    ATH_CHECK(m_calibrationTool->mmGasProperties(vDrift, longDiff, transDiff, interactionDensityMean, interactionDensitySigma,  lorentzAngleFunction));

	m_driftVelocity = vDrift;

	m_StripsResponseSimulation->setTransverseDiffusionSigma(transDiff);
	m_StripsResponseSimulation->setLongitudinalDiffusionSigma(longDiff);
	m_StripsResponseSimulation->setDriftVelocity(vDrift);
	m_StripsResponseSimulation->setAvalancheGain(m_avalancheGain);
	m_StripsResponseSimulation->setInteractionDensityMean(interactionDensityMean);
	m_StripsResponseSimulation->setInteractionDensitySigma(interactionDensitySigma);
	m_StripsResponseSimulation->setLorentzAngleFunction(lorentzAngleFunction);
	m_StripsResponseSimulation->initialize();

	// ElectronicsResponseSimulation Creation
	m_ElectronicsResponseSimulation = std::make_unique<MM_ElectronicsResponseSimulation>();
	m_ElectronicsResponseSimulation->setPeakTime(m_peakTime); // VMM peak time parameter
	m_ElectronicsResponseSimulation->setTimeWindowLowerOffset(m_timeWindowLowerOffset);
	m_timeWindowUpperOffset += m_peakTime; // account for peak time in time window
	m_ElectronicsResponseSimulation->setTimeWindowUpperOffset(m_timeWindowUpperOffset);
	m_ElectronicsResponseSimulation->setStripdeadtime(m_stripdeadtime);
	m_ElectronicsResponseSimulation->setARTdeadtime(m_ARTdeadtime);
	m_ElectronicsResponseSimulation->setNeighborLogic(m_vmmNeighborLogic);

	m_ElectronicsResponseSimulation->initialize();


	// Configuring various VMM modes of signal readout
	//
    std::string vmmReadoutMode = m_vmmReadoutMode;
    // convert vmmReadoutMode to lower case
    std::for_each(vmmReadoutMode.begin(), vmmReadoutMode.end(), [](char & c) {
        c = ::tolower(c);
    });
    if (vmmReadoutMode.find("peak")!=std::string::npos) m_vmmReadoutMode = "peak";
    else if (vmmReadoutMode.find("threshold")!=std::string::npos) m_vmmReadoutMode = "threshold";
	else ATH_MSG_ERROR("MM_DigitizationTool can't interperet vmmReadoutMode option! (Should be 'peak' or 'threshold'.) Contains: " << m_vmmReadoutMode);
    std::string vmmARTMode = m_vmmARTMode;
    // convert vmmARTMode to lower case
    std::for_each(vmmARTMode.begin(), vmmARTMode.end(), [](char & c) {
        c = ::tolower(c);
    });
    if (vmmARTMode.find("peak")!=std::string::npos) m_vmmARTMode = "peak";
    else if (vmmARTMode.find("threshold")!=std::string::npos) m_vmmARTMode = "threshold";
    else ATH_MSG_ERROR("MM_DigitizationTool can't interperet vmmARTMode option! (Should be 'peak' or 'threshold'.) Contains: " << m_vmmARTMode);

	if (m_doSmearing) ATH_MSG_INFO("Running in smeared mode!");

  // get shortest and longest strip length for threshold scaling
  //
  Identifier tmpId; // temporary identifier to work with ReadoutElement
  
  // identifier for first gas gap in a large MM sector, layer is eta layer, station Eta = 1 to get shortest strip
  tmpId = m_idHelperSvc->mmIdHelper().channelID("MML",1,1,1,1,1); 
  
  const MuonGM::MMReadoutElement* detectorReadoutElement = m_MuonGeoMgr->getMMReadoutElement(tmpId);
  int stripNumberShortestStrip = (detectorReadoutElement->getDesign(tmpId))->nMissedBottomEta + 1;
  Identifier tmpIdShortestStrip = m_idHelperSvc->mmIdHelper().channelID("MML",1,1,1,1,stripNumberShortestStrip); // identifier for first gas gap in a large MM sector, layer is eta layer
  float shortestStripLength = detectorReadoutElement->stripLength(tmpIdShortestStrip);

  // identifier for first gas gap in a large MM sector, layer is eta layer, station Eta = 2 to get the longest strip
  Identifier tmpId2 = m_idHelperSvc->mmIdHelper().channelID("MML",2,1,1,1,1);
  
  detectorReadoutElement = m_MuonGeoMgr->getMMReadoutElement(tmpId2);
  int stripNumberLongestStrip = (detectorReadoutElement->getDesign(tmpId))->totalStrips - (detectorReadoutElement->getDesign(tmpId))->nMissedTopEta;
  Identifier tmpIdLongestStrip = m_idHelperSvc->mmIdHelper().channelID("MML",2,1,1,1,stripNumberLongestStrip); // identifier for first gas gap in a large MM sector, layer is eta layer
  float longestStripLength = detectorReadoutElement->stripLength(tmpIdLongestStrip);

  // now get the slope and intercept for the threshold scaling
  // function is m_noiseSlope * stripLength + m_noiseIntercept
  m_noiseSlope = (maxNoise - minNoise)/(longestStripLength-shortestStripLength);
  m_noiseIntercept = minNoise - m_noiseSlope*shortestStripLength;


  ATH_MSG_DEBUG(" Shortest strip number: " << stripNumberShortestStrip  << " length: " << shortestStripLength << " longest strip number: " << stripNumberLongestStrip << " length " << longestStripLength);

	ATH_MSG_DEBUG ( "Configuration  MM_DigitizationTool " );
	ATH_MSG_DEBUG ( "DigitizationTool       " << m_digitTool           );
	ATH_MSG_INFO ( "RndmSvc                " << m_rndmSvc             );
	ATH_MSG_INFO ( "RndmEngine             " << m_rndmEngineName      );
	ATH_MSG_DEBUG ( "InputObjectName        " << m_inputObjectName     );
	ATH_MSG_DEBUG ( "OutputObjectName       " << m_outputDigitCollectionKey.key());
	ATH_MSG_DEBUG ( "OutputSDOName          " << m_outputSDO_CollectionKey.key());
	ATH_MSG_DEBUG ( "UseTimeWindow          " << m_useTimeWindow       );
	ATH_MSG_DEBUG ( "CheckSimHits           " << m_checkMMSimHits      );
	ATH_MSG_DEBUG ( "Threshold              " << m_qThreshold          );
	ATH_MSG_DEBUG ( "TransverseDiffusSigma  " << m_StripsResponseSimulation->getTransversDiffusionSigma() );
	ATH_MSG_DEBUG ( "LogitundinalDiffusSigma" << m_StripsResponseSimulation->getLongitudinalDiffusionSigma() );
	ATH_MSG_DEBUG ( "Interaction density mean: " << m_StripsResponseSimulation->getInteractionDensityMean() );
	ATH_MSG_DEBUG ( "Interaction density sigma: " << m_StripsResponseSimulation->getInteractionDensitySigma() );
	ATH_MSG_DEBUG ( "DriftVelocity stripResponse: " << m_StripsResponseSimulation->getDriftVelocity() );
	ATH_MSG_DEBUG ( "LorentzAngleFunktion stripResponse: " << m_StripsResponseSimulation->getLorentzAngleFunction()->GetName() );
	ATH_MSG_DEBUG ( "DriftVelocity          " << m_driftVelocity       	 		);
	ATH_MSG_DEBUG ( "crossTalk1             " << m_crossTalk1 	     			);
	ATH_MSG_DEBUG ( "crossTalk2             " << m_crossTalk2 	     			);
	ATH_MSG_DEBUG ( "EnergyThreshold        " << m_energyThreshold     			);


	return StatusCode::SUCCESS;
}
/*******************************************************************************/
//----------------------------------------------------------------------
// PrepareEvent method:
//----------------------------------------------------------------------
StatusCode MM_DigitizationTool::prepareEvent(const EventContext& /*ctx*/, unsigned int nInputEvents) {

	ATH_MSG_DEBUG("MM_DigitizationTool::prepareEvent() called for " << nInputEvents << " input events" );

	m_MMHitCollList.clear();

	if (m_timedHitCollection_MM == nullptr) {
		m_timedHitCollection_MM = std::make_unique<TimedHitCollection<MMSimHit>>();
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

      auto hitCollPtr = std::make_unique<MMSimHitCollection>(*iColl->second);
      PileUpTimeEventIndex timeIndex(iColl->first);

      ATH_MSG_DEBUG("MMSimHitCollection found with " << hitCollPtr->size() <<
                    " hits");
      ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
                      << " index: " << timeIndex.index()
                      << " type: " << timeIndex.type());

      m_timedHitCollection_MM->insert(timeIndex, hitCollPtr.get());
      m_MMHitCollList.push_back(std::move(hitCollPtr));

    }
	return StatusCode::SUCCESS;
}

/*******************************************************************************/
StatusCode MM_DigitizationTool::getNextEvent(const EventContext& ctx) {

// Get next event and extract collection of hit collections:
// This is applicable to non-PileUp Event...

	ATH_MSG_DEBUG ( "MM_DigitizationTool::getNextEvent()" );

	//  get the container(s)
	typedef PileUpMergeSvc::TimedList<MMSimHitCollection>::type TimedHitCollList;

  // In case of single hits container just load the collection using read handles
  if (!m_onlyUseContainerName) {
    SG::ReadHandle<MMSimHitCollection> hitCollection(m_hitsContainerKey, ctx);
    if (!hitCollection.isValid()) {
      ATH_MSG_ERROR("Could not get MMSimHitCollection container " << hitCollection.name() << " from store " << hitCollection.store());
      return StatusCode::FAILURE;
    }

    // create a new hits collection
    m_timedHitCollection_MM = std::make_unique<TimedHitCollection<MMSimHit>>(1);
    m_timedHitCollection_MM->insert(0, hitCollection.cptr());
    ATH_MSG_DEBUG("MMSimHitCollection found with " << hitCollection->size() << " hits");
    return StatusCode::SUCCESS;
  }

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
	if (m_timedHitCollection_MM == nullptr) {
		m_timedHitCollection_MM = std::make_unique<TimedHitCollection<MMSimHit>>();
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
StatusCode MM_DigitizationTool::mergeEvent(const EventContext& ctx) {

	ATH_MSG_VERBOSE ( "MM_DigitizationTool::in mergeEvent()" );

	ATH_CHECK( doDigitization(ctx) );

	// reset the pointer
  m_timedHitCollection_MM.reset();

	// clear cloned list
	m_MMHitCollList.clear();
	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MM_DigitizationTool::digitize(const EventContext& ctx) {
	return this->processAllSubEvents(ctx);
}
/*******************************************************************************/
StatusCode MM_DigitizationTool::processAllSubEvents(const EventContext& ctx) {

	ATH_MSG_DEBUG ("MM_DigitizationTool::processAllSubEvents()");

	//merging of the hit collection in getNextEvent method

	if (m_timedHitCollection_MM == nullptr) ATH_CHECK( getNextEvent(ctx) );

	ATH_CHECK( doDigitization(ctx) );

	// reset the pointer
	m_timedHitCollection_MM.reset();
	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MM_DigitizationTool::finalize() {

	if (m_writeOutputFile) {
		TDirectory*backup=gDirectory;
		m_file->cd();
		m_ntuple->Write();
		gDirectory=backup;
		m_file->Close();
	}

	return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MM_DigitizationTool::doDigitization(const EventContext& ctx) {

  CLHEP::HepRandomEngine* rndmEngine = getRandomEngine(m_rndmEngineName, ctx);

  // create and record the Digit container in StoreGate
  SG::WriteHandle<MmDigitContainer> digitContainer(m_outputDigitCollectionKey, ctx);
  ATH_CHECK(digitContainer.record(std::make_unique<MmDigitContainer>(m_idHelperSvc->mmIdHelper().detectorElement_hash_max())));
  ATH_MSG_DEBUG ( "MmDigitContainer recorded in StoreGate." );

  // Create and record the SDO container in StoreGate
  SG::WriteHandle<MuonSimDataCollection> sdoContainer(m_outputSDO_CollectionKey, ctx);
  ATH_CHECK(sdoContainer.record(std::make_unique<MuonSimDataCollection>()));
  ATH_MSG_DEBUG ( "MmSDOCollection recorded in StoreGate." );

  MMSimHitCollection* inputSimHitColl=nullptr;
  
  IdentifierHash moduleHash=0;
  
  inputSimHitColl = new MMSimHitCollection("MicromegasSensitiveDetector");
  ATH_CHECK( evtStore()->record(inputSimHitColl,"InputMicroMegasHits") );
  
  if( m_maskMultiplet == 3 ) {
    
    return StatusCode::SUCCESS;
  }
  
  // Perform null check on m_thpcCSC
  if (m_timedHitCollection_MM == nullptr) {
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
      MM_SimIdToOfflineId simToOffline(&m_idHelperSvc->mmIdHelper());
      
      //get the hit Identifier and info
      int simId=hit.MMId();
      layerID = simToOffline.convert(simId);

      /// check if the hit has to be dropped, based on efficiency
      if ( m_doSmearing ) {
	bool acceptHit = true;
	ATH_CHECK(m_smearingTool->isAccepted(layerID,acceptHit));
	if ( !acceptHit ) {
	  ATH_MSG_DEBUG("Dropping the hit - smearing tool");
	  continue;
	}
      }
      const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
      const HepMcParticleLink::PositionFlag idxFlag = (phit.eventId()==0) ? HepMcParticleLink::IS_POSITION: HepMcParticleLink::IS_INDEX;
      const HepMcParticleLink particleLink(phit->trackNumber(),phit.eventId(),evColl,idxFlag);
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
		      <<     particleLink
		      << " station eta "
		      <<     m_idHelperSvc->mmIdHelper().stationEta(layerID)
		      << " station phi "
		      <<     m_idHelperSvc->mmIdHelper().stationPhi(layerID)
		      << " multiplet "
		      <<     m_idHelperSvc->mmIdHelper().multilayer(layerID)
		      );
      
      // For collection of inputs to throw back in SG
     
      inputSimHitColl->Emplace(hitID,
		       m_globalHitTime+m_eventTime,
		       globalHitPosition,
		       hit.particleEncoding(),
		       hit.kineticEnergy(),
		       hit.globalDirection(),
		       hit.depositEnergy(),
		       particleLink);
      
      // remove hits in masked multiplet
      if( m_maskMultiplet == m_idHelperSvc->mmIdHelper().multilayer(layerID) ) continue;
      
      
      //
      // Hit Information And Preparation
      //
      ////////////////////////////////////////////////////////////////////
      
      
      ////////////////////////////////////////////////////////////////////
      //
      // Sanity Checks
      //      
      if( !m_idHelperSvc->isMM(layerID) ){
	ATH_MSG_WARNING("layerID does not represent a valid MM layer: "
			<< m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(layerID)) );
	continue;
      }
      
      std::string stName = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(layerID));
      int isSmall = stName[2] == 'S';
      
      if( m_idHelperSvc->isMdt(layerID)
	  || m_idHelperSvc->isRpc(layerID)
	  || m_idHelperSvc->isTgc(layerID)
	  || m_idHelperSvc->isCsc(layerID)
	  || m_idHelperSvc->issTgc(layerID)
	  ){
	ATH_MSG_WARNING("MM id has wrong technology type! ");
	m_exitcode = 9;
	if(m_writeOutputFile) m_ntuple->Fill();
	continue;
      }
      
      if( m_idHelperSvc->mmIdHelper().stationPhi(layerID) == 0 ){
	ATH_MSG_WARNING("unexpected phi range " << m_idHelperSvc->mmIdHelper().stationPhi(layerID) );
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
			 <<     m_idHelperSvc->mmIdHelper().stationEta(layerID)
			 << " phi "
			 <<     m_idHelperSvc->mmIdHelper().stationPhi(layerID)
			 << " ml "
			 <<     m_idHelperSvc->mmIdHelper().multilayer(layerID)
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
      char side = m_idHelperSvc->mmIdHelper().stationEta(layerID) < 0 ? 'C' : 'A';
      MMDetectorHelper aHelper;
      MMDetectorDescription* mm = aHelper.Get_MMDetector( stName[2],
							  std::abs(m_idHelperSvc->mmIdHelper().stationEta(layerID)),
							  m_idHelperSvc->mmIdHelper().stationPhi(layerID),
							  m_idHelperSvc->mmIdHelper().multilayer(layerID),
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
		      <<     m_idHelperSvc->mmIdHelper().stationEta(layerID)
		      << " phi: "
		      <<     m_idHelperSvc->mmIdHelper().stationPhi(layerID)
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
      if ((roParam.readoutSide).at(m_idHelperSvc->mmIdHelper().gasGap(layerID)-1)==1) {
	localDirectionTime  = localDirection;
      inAngle_XZ = (-inAngle_XZ);
      }
      else
	localDirectionTime  = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(),
										hit.globalDirection().y(),
										-hit.globalDirection().z()
										);
      
      /// move the initial track point to the readout plane
      int gasGap = m_idHelperSvc->mmIdHelper().gasGap(layerID);
      double shift = 0.5*detectorReadoutElement->getDesign(layerID)->thickness;
      double scale = 0.0;
      if ( gasGap==1 || gasGap == 3) {
	scale = -(stripLayerPosition.z() + shift)/localDirection.z();
      } else if ( gasGap==2 || gasGap == 4) {
	scale = -(stripLayerPosition.z() - shift)/localDirection.z();
      }
      
      Amg::Vector3D hitOnSurface = stripLayerPosition + scale*localDirection;
      Amg::Vector2D positionOnSurface (hitOnSurface.x(), hitOnSurface.y());
      
      // Account For Time Offset
      double shiftTimeOffset = (m_globalHitTime - m_tofCorrection)* m_driftVelocity;
      Amg::Vector3D hitAfterTimeShift(hitOnSurface.x(),hitOnSurface.y(),shiftTimeOffset);
      Amg::Vector3D hitAfterTimeShiftOnSurface = hitAfterTimeShift - (shiftTimeOffset/localDirectionTime.z())*localDirectionTime;
      
      if( std::abs(hitAfterTimeShiftOnSurface.z()) > 0.1 ) ATH_MSG_WARNING("Bad propagation to surface after time shift " << hitAfterTimeShiftOnSurface );
 
      //  moving the hit position to the center of the gap for the SDO position 
      double scaleSDO = -stripLayerPosition.z()/localDirection.z();
      Amg::Vector3D hitAtCenterOfGasGap =  stripLayerPosition + scaleSDO*localDirection;
      Amg::Vector3D hitAtCenterOfGasGapGlobal = surf.transform() * hitAtCenterOfGasGap; 
      ATH_MSG_DEBUG("strip layer position z"<< stripLayerPosition.z() <<"hitAtCenterOfGasGap x" 
		    << hitAtCenterOfGasGap.x() << " y " <<  hitAtCenterOfGasGap.y() 
		    << " z " << hitAtCenterOfGasGap.z() << " gas gap "<< gasGap); 

      // Don't consider electron hits below m_energyThreshold
      if( hit.kineticEnergy() < m_energyThreshold && std::abs(hit.particleEncoding())==11) {
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
			    << m_idHelperSvc->mmIdHelper().print_to_string(layerID)
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
      Identifier parentID = m_idHelperSvc->mmIdHelper().parentID(layerID);
      Identifier digitID  = m_idHelperSvc->mmIdHelper().channelID(parentID,
						  m_idHelperSvc->mmIdHelper().multilayer(layerID),
						  m_idHelperSvc->mmIdHelper().gasGap(layerID),
						  stripNumber
						  );
      
      ++nhits;
      
      // contain (name, eta, phi, multiPlet)
      m_idHelperSvc->mmIdHelper().get_module_hash(layerID, moduleHash);

      ATH_MSG_DEBUG(" looking up collection using moduleHash "
		    << (int)moduleHash
		    << " "
		    << m_idHelperSvc->mmIdHelper().print_to_string(layerID)
		    << " digitID: "
		    << m_idHelperSvc->mmIdHelper().print_to_string(digitID)
		    );
      
      const MuonGM::MuonChannelDesign* mmChannelDesign = detectorReadoutElement->getDesign(digitID);
      double distToChannel = mmChannelDesign->distanceToChannel(positionOnSurface, stripNumber);

      // check whether retrieved distance is greater than strip width
      // first retrieve the strip number from position by geometrical check
      int geoStripNumber = mmChannelDesign->channelNumber(positionOnSurface);
      if (geoStripNumber == -1) ATH_MSG_WARNING("Failed to retrieve strip number");
      // retrieve channel position of closest active strip
      Amg::Vector2D chPos;
      if (!mmChannelDesign->channelPosition(geoStripNumber, chPos)) {
        ATH_MSG_DEBUG("Failed to retrieve channel position for closest strip number " << geoStripNumber
                       << ". Can happen if hit was found in non-active strip. Will not digitize it, since in data, "
                       << "we would probably not find a cluster formed well enough to survive reconstruction.");
        if(m_writeOutputFile) m_ntuple->Fill();
        continue;
      }

      MagField::AtlasFieldCache fieldCache;
      SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
      const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
      if (!fieldCondObj) {
        ATH_MSG_ERROR("doDigitization: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
        return StatusCode::FAILURE;
      }
      fieldCondObj->getInitializedCache(fieldCache);

      // Obtain Magnetic Field At Detector Surface
      Amg::Vector3D hitOnSurfaceGlobal = surf.transform()*hitOnSurface;
      Amg::Vector3D magneticField;
      fieldCache.getField(hitOnSurfaceGlobal.data(), magneticField.data());
      
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
					       detectorReadoutElement->numberOfMissingBottomStrips(layerID)+1,
					       detectorReadoutElement->numberOfStrips(layerID)-detectorReadoutElement->numberOfMissingTopStrips(layerID),
					       m_idHelperSvc->mmIdHelper().gasGap(layerID),
					       m_eventTime+m_globalHitTime
					       );
      
      
      // fill the SDO collection in StoreGate
      // create here deposit for MuonSimData, link and tof
      //
      // Since we have output based on channel, instead of hit, the SDO and digit ID are No longer meaningless. 2016/06/27 T.Saito
      //
      // digitize input for strip response
      
      MuonSimData::Deposit deposit(particleLink, MuonMCData(hitOnSurface.x(),hitOnSurface.y()));
      
      //Record the SDO collection in StoreGate
      std::vector<MuonSimData::Deposit> deposits;
      deposits.push_back(deposit);
      MuonSimData simData(deposits,0);
      simData.setPosition(hitAtCenterOfGasGapGlobal);
      simData.setTime(m_globalHitTime);
      sdoContainer->insert ( std::make_pair ( digitID, simData ) );
      ATH_MSG_DEBUG(" added MM SDO " <<  sdoContainer->size());
      
      m_n_hitStripID=stripNumber;
      m_n_hitDistToChannel=distToChannel;
      m_n_hitIncomingAngle=inAngle_XZ;
      m_n_hitIncomingAngleRads = inAngle_XZ * CLHEP::degree;
      m_n_hitOnSurface_x=positionOnSurface.x();
      m_n_hitOnSurface_y = positionOnSurface.y();

      float gainFraction = 1.0;
      if ( m_doSmearing ) {
        // build identifier including the strip since layerId does not contain teh strip number
        Identifier id = m_idHelperSvc->mmIdHelper().channelID(layerID, m_idHelperSvc->mmIdHelper().multilayer(layerID), m_idHelperSvc->mmIdHelper().gasGap(layerID), stripNumber);
        ATH_CHECK(m_smearingTool->getGainFraction(id, gainFraction));
      }
      double stripPitch = detectorReadoutElement->getDesign(layerID)->channelWidth(positionOnSurface); 
      
      MM_StripToolOutput tmpStripOutput = m_StripsResponseSimulation->GetResponseFrom(stripDigitInput,gainFraction, stripPitch, rndmEngine);
      MM_ElectronicsToolInput stripDigitOutput( tmpStripOutput.NumberOfStripsPos(), tmpStripOutput.chipCharge(), tmpStripOutput.chipTime(), digitID , hit.kineticEnergy());
      
      // This block is purely validation
      for(size_t i = 0; i<tmpStripOutput.NumberOfStripsPos().size(); i++){
        int tmpStripID = tmpStripOutput.NumberOfStripsPos().at(i);
        bool isValid;
        Identifier cr_id = m_idHelperSvc->mmIdHelper().channelID(stName, m_idHelperSvc->mmIdHelper().stationEta(layerID), m_idHelperSvc->mmIdHelper().stationPhi(layerID), m_idHelperSvc->mmIdHelper().multilayer(layerID), m_idHelperSvc->mmIdHelper().gasGap(layerID), tmpStripID, true, &isValid);
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
    MM_DigitToolOutput* electronicsOutputForReadout=nullptr;
    if (m_vmmReadoutMode      ==   "peak"     ) electronicsOutputForReadout = &electronicsPeakOutput;
    else if (m_vmmReadoutMode ==   "threshold") electronicsOutputForReadout = &electronicsThresholdOutput;
    else {
        ATH_MSG_ERROR("Failed to setup readout signal from VMM. Readout mode incorrectly set");
        return StatusCode::FAILURE;
    }
    // but this should be impossible from initialization checks
    
    // Choose which of the above outputs is used for triggering
    //
    MM_DigitToolOutput* electronicsOutputForTriggerPath=nullptr;
    if (m_vmmARTMode          ==   "peak"     ) electronicsOutputForTriggerPath = &electronicsPeakOutput;
    else if (m_vmmARTMode     ==   "threshold") electronicsOutputForTriggerPath = &electronicsThresholdOutput;
    else {
        ATH_MSG_ERROR("Failed to setup trigger signal from VMM. Readout mode incorrectly set");
        return StatusCode::FAILURE;
    }
    // but this should be impossible from initialization checks
    
    
    
    // Apply Dead-time for strip
    //
    MM_DigitToolOutput electronicsOutputForTriggerPathWStripDeadTime (m_ElectronicsResponseSimulation->applyDeadTimeStrip(*electronicsOutputForTriggerPath));
    
    // ART: The fastest strip signal per VMM id should be selected for trigger
    //
    int chMax = m_idHelperSvc->mmIdHelper().channelMax(layerID);
    int stationEta = m_idHelperSvc->mmIdHelper().stationEta(layerID);
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
    std::unique_ptr<MmDigit> newDigit = nullptr;

    if ( !m_doSmearing ) { 
      newDigit = std::make_unique<MmDigit>(stripDigitOutputAllHits.digitID(),
							 electronicsOutputForReadout->stripTime(),
							 electronicsOutputForReadout->stripPos(),
							 electronicsOutputForReadout->stripCharge(),
							 electronicsOutputForReadout->stripTime(),
							 electronicsOutputForReadout->stripPos(),
							 electronicsOutputForReadout->stripCharge(),
							 finalElectronicsTriggerOutput.chipTime(),
							 finalElectronicsTriggerOutput.NumberOfStripsPos(),
							 finalElectronicsTriggerOutput.chipCharge(),
							 finalElectronicsTriggerOutput.MMFEVMMid(),
							 finalElectronicsTriggerOutput.VMMid()
							 );
    }
    else {

      std::vector<int> stripPosSmeared;
      std::vector<float> stripChargeSmeared;
      std::vector<float> stripTimeSmeared;
      Identifier digitId = stripDigitOutputAllHits.digitID();
      for ( unsigned int i = 0 ; i<electronicsOutputForReadout->stripTime().size() ; ++i ) {
	int   pos  = electronicsOutputForReadout->stripPos()[i];
	float time = electronicsOutputForReadout->stripTime()[i];
	float charge = electronicsOutputForReadout->stripCharge()[i];
	bool acceptStrip = true;

	/// use the smearing tool to update time and charge 
	ATH_CHECK(m_smearingTool->smearTimeAndCharge(digitId,time,charge,acceptStrip));
	
	if ( acceptStrip ) {
	  stripPosSmeared.push_back(pos);
	  stripTimeSmeared.push_back(time);
	  stripChargeSmeared.push_back(charge);
	} 
	else {
	  /// drop the strip
	  continue;
	}

      }

      if ( stripPosSmeared.size() > 0 ) { 
	newDigit = std::make_unique<MmDigit>(digitId,
							   stripTimeSmeared,
							   stripPosSmeared,
							   stripChargeSmeared,
							   stripTimeSmeared,
							   stripPosSmeared,
							   stripChargeSmeared,
							   finalElectronicsTriggerOutput.chipTime(),
							   finalElectronicsTriggerOutput.NumberOfStripsPos(),
							   finalElectronicsTriggerOutput.chipCharge(),
							   finalElectronicsTriggerOutput.MMFEVMMid(),
							   finalElectronicsTriggerOutput.VMMid()
							   );
      }
      else {
	continue;
      }
 
    }

    
    // The collections should use the detector element hashes not the module hashes to be consistent with the PRD granularity.
    // IdentifierHash detIdhash ;
    // set RE hash id
    const Identifier elemId = m_idHelperSvc->mmIdHelper().elementID( stripDigitOutputAllHits.digitID() );
    if (!m_idHelperSvc->isMM(elemId)) {
        ATH_MSG_WARNING("given Identifier "<<elemId.get_compact()<<" is not a MM Identifier, skipping");
        continue;
    }
    m_idHelperSvc->mmIdHelper().get_module_hash( elemId, moduleHash );
    
    MmDigitCollection* digitCollection = nullptr;
    // put new collection in storegate
    // Get the messaging service, print where you are
    const MmDigitCollection* coll = digitContainer->indexFindPtr(moduleHash );
    if (coll == nullptr) {
      digitCollection = new MmDigitCollection( elemId, moduleHash );
      digitCollection->push_back(std::move(newDigit));
      ATH_CHECK(digitContainer->addCollection(digitCollection, moduleHash ) );
    }
    else {
      digitCollection = const_cast<MmDigitCollection*>( coll );
      digitCollection->push_back(std::move(newDigit));
    }
    
    //
    // (VMM-Level) Output Of Digitization
    //
    ////////////////////////////////////////////////////////////////////
    
    
    
    v_stripDigitOutput.clear();
    
  }
  
  ATH_MSG_DEBUG ( "MM_Digitization Done!"  );
  
  m_timedHitCollection_MM.reset();
  
  return StatusCode::SUCCESS;
}

MM_ElectronicsToolInput MM_DigitizationTool::combinedStripResponseAllHits(const std::vector< MM_ElectronicsToolInput > & v_stripDigitOutput){

	std::vector <int> v_stripStripResponseAllHits;
	std::vector < std::vector <float> > v_timeStripResponseAllHits;
	std::vector < std::vector <float> > v_qStripResponseAllHits;
	std::vector<float> v_stripThresholdResponseAllHits;
	v_stripStripResponseAllHits.clear();
	v_timeStripResponseAllHits.clear();
	v_qStripResponseAllHits.clear();
	v_stripThresholdResponseAllHits.clear();
	v_stripThresholdResponseAllHits.clear();


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
				if(m_useThresholdScaling) {

					Identifier id = m_idHelperSvc->mmIdHelper().channelID(digitID, m_idHelperSvc->mmIdHelper().multilayer(digitID), m_idHelperSvc->mmIdHelper().gasGap(digitID), strip_id);
					const MuonGM::MMReadoutElement* detectorReadoutElement = m_MuonGeoMgr->getMMReadoutElement(id);
					float stripLength = detectorReadoutElement->stripLength(id);
					float threshold = (m_noiseSlope*stripLength + m_noiseIntercept) * m_thresholdScaleFactor;
					v_stripThresholdResponseAllHits.push_back(threshold);        
				}
				else {
					v_stripThresholdResponseAllHits.push_back(m_electronicsThreshold);
				}
			}
		}
	}

	MM_ElectronicsToolInput stripDigitOutputAllHits( v_stripStripResponseAllHits,
													v_qStripResponseAllHits,
													v_timeStripResponseAllHits,
													v_stripThresholdResponseAllHits,
													digitID,
													max_kineticEnergy
													);

	return stripDigitOutputAllHits;
}
/*******************************************************************************/
bool MM_DigitizationTool::checkMMSimHit( const MMSimHit& /*hit*/ ) const {
	return true;
}

CLHEP::HepRandomEngine* MM_DigitizationTool::getRandomEngine(const std::string& streamName, const EventContext& ctx) const
{
		ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this, streamName);
		rngWrapper->setSeed( streamName, ctx );
		return rngWrapper->getEngine(ctx);
}


