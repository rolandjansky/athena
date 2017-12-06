/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: Ketevi A. Assamagan
// BNL, October 27 2003
// Digitization algorithm for the CSC hits
#include "MuonReadoutGeometry/MuonDetectorManager.h" 

#include "HepMC/GenParticle.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "PileUpTools/PileUpMergeSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "CSC_Digitization/CscDigitizationTool.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

#include "EventInfo/EventInfo.h"

using namespace MuonGM;

static constexpr unsigned int crazyParticleBarcode(
    std::numeric_limits<int32_t>::max());
// Barcodes at the HepMC level are int

CscDigitizationTool::CscDigitizationTool(const std::string& type,const std::string& name,const IInterface* pIID) 
  : PileUpToolBase(type, name, pIID), m_pcalib("CscCalibTool"), m_container(0)
  , m_geoMgr(0), m_cscDigitizer(0), m_cscIdHelper(0), m_thpcCSC(0)
  , m_vetoThisBarcode(crazyParticleBarcode), m_run(0), m_evt(0), m_mergeSvc(0)
  , m_inputObjectName("CSC_Hits"), m_outputObjectName("csc_digits"), m_rndmSvc("AtRndmGenSvc", name )
  , m_rndmEngine(0), m_rndmEngineName("MuonDigitization") {

  declareInterface<IMuonDigitizationTool>(this);

  declareProperty("InputObjectName",  m_inputObjectName = "CSC_Hits", "name of the input objects");
  declareProperty("OutputObjectName", m_outputObjectName = "csc_digits", "name of the output objects" );
  declareProperty("pedestal",m_pedestal = 0.0);
  declareProperty("WindowLowerOffset",m_timeWindowLowerOffset = -25.);
  declareProperty("WindowUpperOffset",m_timeWindowUpperOffset = +25.);
  declareProperty("isPileUp",m_isPileUp = false);

  declareProperty("RndmSvc", 		m_rndmSvc, "Random Number Service used in Muon digitization" );
  declareProperty("RndmEngine",       m_rndmEngineName,       "Random engine name");
  //    declareProperty("MCStore",  m_sgSvc, "Simulated Data Event Store");
  declareProperty("cscCalibTool",     m_pcalib);
  declareProperty("maskBadChannels",  m_maskBadChannel=true);
  declareProperty("amplification",    m_amplification=0.58e5);

  declareProperty("NewDigitEDM",      m_newDigitEDM = true);
  declareProperty("DriftVelocity",    m_driftVelocity = 60); // 60 / (1e-6 * 1e9); // 6 cm/microsecond -> mm/ns // 0.06
  declareProperty("ElectronEnergy",   m_electronEnergy   = 66); // eV
  declareProperty("NInterFixed",      m_NInterFixed   = false);
  declareProperty("IncludePileUpTruth",  m_includePileUpTruth  =  true, "Include pile-up truth info");
  declareProperty("ParticleBarcodeVeto", m_vetoThisBarcode     =  crazyParticleBarcode, "Barcode of particle to ignore");
}

CscDigitizationTool::~CscDigitizationTool()  {
  delete m_cscDigitizer;
}

StatusCode CscDigitizationTool::initialize() {

  ATH_MSG_INFO ( "Initialized Properties :" );
  ATH_MSG_INFO ( "  InputObjectName           " << m_inputObjectName );
  ATH_MSG_INFO ( "  OutputObjectName          " << m_outputObjectName );
  ATH_MSG_INFO ( "  WindowLowerOffset         " << m_timeWindowLowerOffset );
  ATH_MSG_INFO ( "  WindowUpperOffset         " << m_timeWindowUpperOffset );
  ATH_MSG_INFO ( "  PileUp On?                " << m_isPileUp );
  ATH_MSG_INFO ( "  maskBadChannels?          " << m_maskBadChannel );
  ATH_MSG_INFO ( "  amplification(gain)       " << m_amplification );
  ATH_MSG_INFO ( "  Use NewDigitEDM?          " << m_newDigitEDM );
  ATH_MSG_INFO ( "  Drift Velocity Set?       " << m_driftVelocity );
  ATH_MSG_INFO ( "  NInteraction per layer from poisson not from energyLoss?  " << m_NInterFixed );
  ATH_MSG_INFO ( "  IncludePileUpTruth        " << m_includePileUpTruth );
  ATH_MSG_INFO ( "  ParticleBarcodeVeto       " << m_vetoThisBarcode );

  ATH_MSG_INFO ( "  RndmSvc                   " << m_rndmSvc.typeAndName() );
  ATH_MSG_INFO ( "  cscCalibTool              " << m_pcalib.typeAndName() );


  ATH_MSG_INFO ( "Retrieved Active Store Service." );

  // initialize transient detector store and MuonDetDescrManager
  if ( detStore()->retrieve(m_geoMgr).isFailure() ) {
    ATH_MSG_FATAL ( "Could not retrieve MuonDetectorManager!" );
    return StatusCode::FAILURE;
  }
  else 
    ATH_MSG_INFO ( "MuonDetectorManager retrieved from StoreGate.");


  //locate the PileUpMergeSvc and initialize our local ptr
  
  const bool CREATEIF(true);
  if (!(service("PileUpMergeSvc", m_mergeSvc, CREATEIF)).isSuccess() || 
      0 == m_mergeSvc) {
    ATH_MSG_ERROR ( "Could not find PileUpMergeSvc" );
    return StatusCode::FAILURE;
  }

  // check the input object name
  if (m_inputObjectName=="") {
    ATH_MSG_FATAL ( "Property InputObjectName not set !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Input objects: '" << m_inputObjectName << "'" );
  }

  // check the output object name
  if (m_outputObjectName=="") {
    ATH_MSG_ERROR ( "Property OutputObjectName not set !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "CSC:Digitizer::INFO: Output objects: '" << m_outputObjectName << "'" );
  }
  //random number initialization
  if (!m_rndmSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR ( " Could not initialize Random Number Service" );
  }      
  // getting our random numbers stream

  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  }

  /** CSC calibratin tool for the Condtiions Data base access */
  if ( m_pcalib.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Can't get handle on CSC calibration tools" );
    return StatusCode::FAILURE;
  }

  ICscCalibTool * cscCalibTool = &*(m_pcalib);
  
  //initialize the CSC digitizer
  CscHitIdHelper * cscHitHelper = CscHitIdHelper::GetHelper();
  m_cscDigitizer = new CSC_Digitizer(cscHitHelper, m_geoMgr, cscCalibTool);
  m_cscDigitizer->setAmplification(m_amplification);
  m_cscDigitizer->setDebug        ( msgLvl(MSG::DEBUG) );
  m_cscDigitizer->setDriftVelocity(m_driftVelocity);
  m_cscDigitizer->setElectronEnergy  (m_electronEnergy);
  if (m_NInterFixed)
    m_cscDigitizer->setNInterFixed();
  
  if ( m_cscDigitizer->initialize().isFailure() ) {
    ATH_MSG_FATAL ( "Could not initialize CSC Digitizer!" );
    return StatusCode::FAILURE;
  }
  m_cscIdHelper = m_geoMgr->cscIdHelper();

  //  ATH_MSG_FATAL ( "Could not initialize CSC Digitizer!" );

  m_cscDigitizer->setWindow(m_timeWindowLowerOffset, m_timeWindowUpperOffset);

  
  // create an empty CSC digit container for filling
  try{
    m_container = new CscDigitContainer(m_cscIdHelper->module_hash_max());
  } catch(std::bad_alloc){
    ATH_MSG_FATAL ( "Could not create a new CscDigitContainer!");
    return StatusCode::FAILURE;
  }

  m_container->addRef();


  ATH_MSG_DEBUG("WP Current MSG Level FATAL ? " << msgLvl(MSG::FATAL) );
  ATH_MSG_DEBUG("WP Current MSG Level ERROR ? " << msgLvl(MSG::ERROR) );
  ATH_MSG_DEBUG("WP Current MSG Level WARNING ? " << msgLvl(MSG::WARNING) );
  ATH_MSG_DEBUG("WP Current MSG Level INFO ? " << msgLvl(MSG::INFO) );
  ATH_MSG_DEBUG("WP Current MSG Level DEBUG ? " << msgLvl(MSG::DEBUG) );
  ATH_MSG_DEBUG("WP Current MSG Level VERBOSE ? " << msgLvl(MSG::VERBOSE) );

  
  return StatusCode::SUCCESS;
    
}

// Inherited from PileUpTools
StatusCode CscDigitizationTool::prepareEvent(unsigned int /*nInputEvents*/) {

  // clean up the digit container
  m_container->cleanup();

  // record the digit container in StoreGate
  std::string key = "CSC_DIGITS";
  if ( (evtStore()->record(m_container,key)).isFailure() )  {
    ATH_MSG_ERROR ( "Unable to record CSC digit container in StoreGate" );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_DEBUG ( "CscDigitContainer recorded in StoreGate." );

  if (0 == m_thpcCSC)
    m_thpcCSC = new TimedHitCollection<CSCSimHit>();

  m_cscHitCollList.clear();

  return StatusCode::SUCCESS;
}
///////////////////////////////

StatusCode CscDigitizationTool::digitize() {
  return this->processAllSubEvents();
}
///////////////////////////////

StatusCode CscDigitizationTool::processAllSubEvents() {

  StatusCode status;

  ATH_MSG_DEBUG ( "in processAllSubEvents()" );

  // clean up the digit container
  m_container->cleanup();

  // record the digit container in StoreGate
  std::string key = "CSC_DIGITS";
  if ( (evtStore()->record(m_container,key)).isFailure() )  {
    ATH_MSG_ERROR ( "Unable to record CSC digit container in StoreGate" );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_DEBUG ( "CscDigitContainer recorded in StoreGate." );

  if (m_isPileUp) return StatusCode::SUCCESS;

  // create and record the SDO container in StoreGate
  std::string sdoKey = "CSC_SDO";
  CscSimDataCollection* sdoContainer = new CscSimDataCollection();
  status = evtStore()->record(sdoContainer,sdoKey);
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "Unable to record CSC SDO collection in StoreGate" );
    return status;
  } else
    ATH_MSG_DEBUG ( "CscSDOCollection recorded in StoreGate." );

  //merging of the hit collection in getNextEvent method    

  if (0 == m_thpcCSC ) {
    StatusCode sc = getNextEvent();
    if (StatusCode::FAILURE == sc) {
      ATH_MSG_INFO ( "There are no CSC hits in this event" );      
      return status; // there are no hits in this event
    }
  }

  return CoreDigitization(sdoContainer);

}

StatusCode CscDigitizationTool::CoreDigitization(CscSimDataCollection* sdoContainer) {
  
  // get the iterator pairs for this DetEl
  //iterate over hits
  const EventInfo* pevt = 0;
  StatusCode sc_ev = evtStore()->retrieve(pevt, "");
  m_evt = pevt->event_ID()->event_number();
  m_run = pevt->event_ID()->run_number();
  
  
  std::map <IdentifierHash,deposits> myDeposits;
  csc_map    data_map;
  csc_newmap data_SampleMap, data_SampleMapOddPhase;
  
  TimedHitCollection< CSCSimHit >::const_iterator i, e;  
  
  // Perform null check on m_thpcCSC
  if(!m_thpcCSC) {
    ATH_MSG_ERROR ( "m_thpcCSC is null" );
    return StatusCode::FAILURE;
  }

  while( m_thpcCSC->nextDetectorElement(i, e) ) {
 
    // Loop over the hits:
    while (i != e) {
        
      TimedHitPtr<CSCSimHit> phit(*i++);
      const CSCSimHit& hit(*phit);

      //      const HepMcParticleLink McLink = HepMcParticleLink(phit->trackNumber(),phit.eventId());
      //      const HepMC::GenParticle* genPart = McLink.cptr(); // some times empty pointer returned
        
      ATH_MSG_DEBUG(hit.print());

    

      double globalHitTime(hitTime(phit));
      double bunchTime(globalHitTime - hit.globalTime());

      // Hit time is the G4 hit time plus the drift time.
      // Gloabl time add the bnch crossing time to the hit time.
      ATH_MSG_DEBUG ( "bunch time (ns) " << bunchTime  << "globalHitTime (ns) " << globalHitTime << " hit Time (ns) " << hitTime(phit) );
      m_cscDigitizer->set(bunchTime);

      std::vector<IdentifierHash> hashVec;
      StatusCode status = (m_newDigitEDM)
        ? m_cscDigitizer->digitize_hit(&hit, hashVec, data_SampleMap, data_SampleMapOddPhase, m_rndmEngine)
        : m_cscDigitizer->digitize_hit(&hit, hashVec, data_map, m_rndmEngine);

      if (status.isFailure()) {
	ATH_MSG_ERROR ( "CSC Digitizer Failed to digitize a hit!" );
	return status;
      }

      std::vector<IdentifierHash>::const_iterator vecBeg = hashVec.begin();
      std::vector<IdentifierHash>::const_iterator vecEnd = hashVec.end();
      //const HepMcParticleLink & particleLink = hit.particleLink();
      // Fetch the energy deposit.
      const double energy = hit.energyDeposit();
      // Determine where hit crosses the wire plane (x=0).
      float ypos = -99999.9;
      float zpos = ypos;
      double xi = hit.getHitStart().x();
      double yi = hit.getHitStart().y();
      double zi = hit.getHitStart().z();
      double xf = hit.getHitEnd().x();
      double yf = hit.getHitEnd().y();
      double zf = hit.getHitEnd().z();
      double dx = xf - xi;
      double dy = yf - yi;
      double dz = zf - zi;
      if ( dx>0.0 && xi<=0.0 && xf>=0.0 ) {
        double f = -xi/dx;
        ypos = yi + f*dy;
        zpos = zi + f*dz;
      }

      if (!m_includePileUpTruth &&
          ((phit->trackNumber() == 0) || (phit->trackNumber() == m_vetoThisBarcode))) {
        hashVec.clear();
        continue;
      }

      for (; vecBeg != vecEnd; vecBeg++) {
        CscSimData::Deposit deposit(HepMcParticleLink(phit->trackNumber(),phit.eventId()), CscMcData(energy, ypos, zpos));
        myDeposits[(*vecBeg)].push_back(deposit); 
      }
      hashVec.clear();
    }
  }

  // reset the pointer if it is not null
  if (m_thpcCSC) {
    delete m_thpcCSC;
    m_thpcCSC=0;
  } 
  
  // now loop over the digit map
  // build the digits
  // build the digit collections and record them

  if (m_newDigitEDM) {
    double flat = CLHEP::RandFlat::shoot(m_rndmEngine, 0.0,1.0);                 // for other particles
    bool phaseToSet = (flat<0.5) ? true : false;
    if (phaseToSet)
      return FillCollectionWithNewDigitEDM(data_SampleMapOddPhase, myDeposits, sdoContainer, phaseToSet);
    else
      return FillCollectionWithNewDigitEDM(data_SampleMap, myDeposits, sdoContainer, phaseToSet);
  }  else
    return FillCollectionWithOldDigitEDM(data_map, myDeposits, sdoContainer);

}  
  

StatusCode CscDigitizationTool::
FillCollectionWithNewDigitEDM(csc_newmap& data_SampleMap, 
                              std::map<IdentifierHash,deposits>& myDeposits,
                              CscSimDataCollection* sdoContainer, 
                              bool phaseToSet
                              ) {

  CscDigitCollection * collection = 0;

  IdContext context    = m_cscIdHelper->channel_context();
  IdContext cscContext = m_cscIdHelper->module_context();

  Identifier prevId;

  csc_newmap::const_iterator cscMap    = data_SampleMap.begin();
  csc_newmap::const_iterator cscMapEnd = data_SampleMap.end();
  
  for (; cscMap != cscMapEnd; ++cscMap) {
    Identifier digitId;
    IdentifierHash hashId = (*cscMap).first;
    if (m_cscIdHelper->get_id( hashId, digitId, &context ) ) {
      ATH_MSG_ERROR ( "cannot get CSC channel identifier from hash " << hashId );
      return StatusCode::FAILURE;
    }

    Identifier elementId = m_cscIdHelper->parentID(digitId);
    IdentifierHash coll_hash;
    if (m_cscIdHelper->get_hash(elementId, coll_hash, &cscContext)) {
      ATH_MSG_ERROR ( "Unable to get CSC hash id from CSC Digit collection " 
                      << "context begin_index = " << cscContext.begin_index()
                      << " context end_index  = " << cscContext.end_index()
                      << " the identifier is " );
      elementId.show();
    } 
    

    // get the charge 
    double stripCharge = 0.0;
    double driftTime = 0.0;
    //if (stripCharge < m_noiseLevel) continue;
    
    
    const std::vector<float> samples = (*cscMap).second;
    //    csc_newmap::iterator ii= data_SampleMapOddPhase.find(hashId);
    //    const std::vector<float> samplesOddPhase = (*ii).second;
    
    
    //    if (msgLvl(MSG::DEBUG)) {
    unsigned int samplingPhase =0;
    double samplingTime = m_pcalib->getSamplingTime();
    m_pcalib->findCharge(samplingTime, samplingPhase, samples, stripCharge, driftTime);
    //    }
    driftTime += m_pcalib->getLatency();
    
    /** mask this readout channel if it is a dead channel or a hot channel */
    if ( !m_pcalib->isGood( hashId ) && m_maskBadChannel ) {
      stripCharge = 0.0;
      driftTime   = 2*m_timeWindowUpperOffset; 
    }
    
    int zsec = m_cscIdHelper->stationEta(digitId);;
    int phisec = m_cscIdHelper->stationPhi(digitId);;
    int istation = m_cscIdHelper->stationName(digitId) - 49;
    
    int wlay = m_cscIdHelper->wireLayer(digitId);
    int measphi = m_cscIdHelper->measuresPhi(digitId);
    int istrip = m_cscIdHelper->strip(digitId);
    
    int sector = zsec*(2*phisec-istation+1);
    
    auto depositsForHash = myDeposits.find(hashId);
    if (depositsForHash != myDeposits.end() && depositsForHash->second.size()) {
      depositsForHash->second[0].second.setCharge(stripCharge);
      sdoContainer->insert ( std::make_pair(digitId, CscSimData(depositsForHash->second,0)) );
    }
    
    // fill the digit collections in StoreGate
    //    CscDigit * newDigit  = new CscDigit(digitId, int(stripCharge+1) );
    // Now, we pass driftTime as well as stripCharge.
    // SimHIT time should be added to distinguish prompt muons from secondary.... 11/19/2009 WP
    
    //   CscDigit * newDigit  = new CscDigit(digitId, samples);
    //    CscDigit * newDigitOddPhase  = new CscDigit(digitId, samplesOddPhase);

    ATH_MSG_DEBUG ( "NEWDigit sec:measphi:wlay:istr:chg:t(w/latency) "
                    << m_run << " " << m_evt << " " << m_cscIdHelper->show_to_string(digitId,&context) 
                    << " hash:eleId = " << hashId << " " << elementId << " " << prevId << "   "
                    << sector << " " << measphi << " " <<  wlay << " " << istrip << "   "
                    << int(stripCharge+1) << " " << float(driftTime) 
                    << " phase=" << phaseToSet
                    << "  samps: " << samples[0] << " " << samples[1] << " "
                    << samples[2] << " " << samples[3]
                    //                    << "  sampsOddPh: " << samplesOddPhase[0] << " " << samplesOddPhase[1] << " "
                    //                    << samplesOddPhase[2] << " " << samplesOddPhase[3]
                    //                    << "    "  << (newDigit->sampleCharges()).size()
                    );

    if (prevId != elementId) {
      auto  it_coll = m_container->indexFindPtr(coll_hash);
      if (nullptr ==  it_coll) {
        CscDigitCollection * newCollection = new CscDigitCollection(elementId,coll_hash);

        if (phaseToSet) newCollection->set_samplingPhase();

        //        const std::vector<float> samplesToPut
        //          = (phaseToSet) ? samplesOddPhase : samples ;
        
        CscDigit * newDigit  = new CscDigit(digitId, samples);
        newCollection->push_back(newDigit);

        if ( m_container->addCollection(newCollection, coll_hash).isFailure() ) 
          ATH_MSG_ERROR ( "Couldn't record CscDigitCollection with key=" << coll_hash 
                          << " in StoreGate!" );

        collection = newCollection;

      } else {  
        CscDigitCollection * existingCollection = const_cast<CscDigitCollection*>( it_coll );
        if (phaseToSet) existingCollection->set_samplingPhase();
        //        const std::vector<float> samplesToPut
        //          = (existingCollection->samplingPhase()) ? samplesOddPhase : samples ;
        
        CscDigit * newDigit  = new CscDigit(digitId, samples);

        existingCollection->push_back(newDigit);
        collection = existingCollection;
      }
      prevId = elementId;
    } else {
      // According to coverity, collection could still be null here.
      if (!collection) {
        ATH_MSG_ERROR("Trying to use NULL CscDigitCollection");
        return StatusCode::FAILURE;
      }

      if (phaseToSet) collection->set_samplingPhase();
      //      const std::vector<float> samplesToPut
      //        = (collection->samplingPhase()) ? samplesOddPhase : samples ;
      
      CscDigit * newDigit  = new CscDigit(digitId, samples);

      collection->push_back(newDigit);
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode CscDigitizationTool::
FillCollectionWithOldDigitEDM(csc_map& data_map, std::map<IdentifierHash,deposits>& myDeposits,
                              CscSimDataCollection* sdoContainer) {

  CscDigitCollection * collection = 0;
  IdContext context    = m_cscIdHelper->channel_context();
  IdContext cscContext = m_cscIdHelper->module_context();

  Identifier prevId;
  csc_map::const_iterator cscMap    = data_map.begin();
  csc_map::const_iterator cscMapEnd = data_map.end();
  for (; cscMap != cscMapEnd; ++cscMap) {
    Identifier digitId;
    IdentifierHash hashId = (*cscMap).first;
    if (m_cscIdHelper->get_id( hashId, digitId, &context ) ) {
      ATH_MSG_ERROR ( "cannot get CSC channel identifier from hash " << hashId );
      return StatusCode::FAILURE;
    }
    
    // get the charge 
    double stripCharge = 0.0;
    //double gaus = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,0.0,1.0);
    
    //if (stripCharge < m_noiseLevel) continue;
    stripCharge   = ((*cscMap).second).second + m_pedestal; // + m_noiseLevel*gaus;
    double driftTime =((*cscMap).second).first;  // SimHIT time is added yet 12/03/2009
    
    //    stripCharge   = ((*cscMap).second).stripCharge + m_pedestal; // + m_noiseLevel*gaus;  v2
    //    double driftTime =((*cscMap).second).driftTime0; // for version2 v2
    
    /** mask this readout channel if it is a dead channel or a hot channel */
    if ( !m_pcalib->isGood( hashId ) && m_maskBadChannel ) {
      stripCharge = 0.0;
      driftTime   = 2*m_timeWindowUpperOffset; 
    }
    
    ATH_MSG_VERBOSE ( "CSC Digit Id = " << m_cscIdHelper->show_to_string(digitId,&context) 
                      << " hash = " << hashId 
                      << " charge = " << int (stripCharge+1) );
    
    int zsec = m_cscIdHelper->stationEta(digitId);;
    int phisec = m_cscIdHelper->stationPhi(digitId);;
    int istation = m_cscIdHelper->stationName(digitId) - 49;
    
    int wlay = m_cscIdHelper->wireLayer(digitId);
    int measphi = m_cscIdHelper->measuresPhi(digitId);
    int istrip = m_cscIdHelper->strip(digitId);
    
    int sector = zsec*(2*phisec-istation+1);
    
    auto depositsForHash = myDeposits.find(hashId);
    if (depositsForHash != myDeposits.end() && depositsForHash->second.size()) {
      depositsForHash->second[0].second.setCharge(stripCharge);
      sdoContainer->insert ( std::make_pair(digitId, CscSimData(depositsForHash->second,0)) );
    }
    
    // fill the digit collections in StoreGate
    //    CscDigit * newDigit  = new CscDigit(digitId, int(stripCharge+1) );
    // Now, we pass driftTime as well as stripCharge.
    // SimHIT time should be added to distinguish prompt muons from secondary.... 11/19/2009 WP
    CscDigit * newDigit  = new CscDigit(digitId, int(stripCharge+1), float(driftTime) );
    Identifier elementId = m_cscIdHelper->parentID(digitId);
    
    ATH_MSG_DEBUG ( "CSC Digit sector:measphi:wlay:istrip:charge "
                    << m_run << " " << m_evt << " " << sector << " "
                    << measphi << " " <<  wlay << " " << istrip
                    << " " << int(stripCharge+1) << " " << float(driftTime) << " " << (newDigit->sampleCharges()).size());
    
    
    IdentifierHash coll_hash;
    if (m_cscIdHelper->get_hash(elementId, coll_hash, &cscContext)) {
      ATH_MSG_ERROR ( "Unable to get CSC hash id from CSC Digit collection " 
                      << "context begin_index = " << cscContext.begin_index()
                      << " context end_index  = " << cscContext.end_index()
                      << " the identifier is " );
      elementId.show();
    } 
    
    if (prevId != elementId) {
      auto it_coll = m_container->indexFindPtr(coll_hash);
      if (nullptr ==  it_coll) {
        CscDigitCollection * newCollection = new CscDigitCollection(elementId,coll_hash);
        newCollection->push_back(newDigit);
        collection = newCollection;
        StatusCode status = m_container->addCollection(collection, coll_hash );
        if (status.isFailure())
          ATH_MSG_ERROR ( "Couldn't record CscDigitCollection with key=" << coll_hash 
                          << " in StoreGate!" );
      } else {  
        CscDigitCollection * existingCollection = const_cast<CscDigitCollection*>( it_coll );
        existingCollection->push_back(newDigit);
        collection = existingCollection;
      }
      prevId = elementId;
    } else {
      // According to coverity, collection could still be null here.
      if (collection) {
        collection->push_back(newDigit);
      } else {
        ATH_MSG_ERROR("Trying to push back NULL CscDigitCollection");
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}
  


StatusCode CscDigitizationTool::finalize() {
    
  ATH_MSG_DEBUG ( "finalize." );

  m_container->release();

  return StatusCode::SUCCESS;
}

// Get next event and extract collection of hit collections:
StatusCode CscDigitizationTool::getNextEvent() // This is applicable to non-PileUp Event... 
{
  // Get the messaging service, print where you are

  // initialize pointer
  m_thpcCSC = 0;

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<CSCSimHitCollection>::type TimedHitCollList;

  //this is a list<pair<time_t, DataLink<CSCSimHitCollection> > >
  TimedHitCollList hitCollList;
  
  if (!(m_mergeSvc->retrieveSubEvtsData(m_inputObjectName, hitCollList).isSuccess()) ) {
    ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
    return StatusCode::FAILURE;
  }
  if (hitCollList.size()==0) {
    ATH_MSG_ERROR ( "TimedHitCollList has size 0" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( hitCollList.size() 
                    << " CSCSimHitCollections with key " << m_inputObjectName 
                    << " found" );
  }
  
  // create a new hits collection
  m_thpcCSC = new TimedHitCollection<CSCSimHit>();
  
  //now merge all collections into one
  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());
  while (iColl != endColl) {
    const CSCSimHitCollection* p_collection(iColl->second);
    m_thpcCSC->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "CSCSimHitCollection found with "
                    << p_collection->size() << " hits" );    // loop on the hit collections
    ++iColl;
  }
  return StatusCode::SUCCESS;
}



///////////////////////////
StatusCode CscDigitizationTool::processBunchXing(int bunchXing,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents)
{
  ATH_MSG_DEBUG("CscDigitizationTool::processBunchXing() " << bunchXing);
  SubEventIterator iEvt = bSubEvents;
  while (iEvt != eSubEvents)
    {
      StoreGateSvc& seStore = *(iEvt->ptr()->evtStore());
      ATH_MSG_VERBOSE( "SubEvt StoreGate " << seStore.name() << " :"
                       << " bunch crossing : " << bunchXing
                       << " time offset : " << iEvt->time()
                       << " event number : " << iEvt->ptr()->eventNumber()
                       << " run number : " << iEvt->ptr()->runNumber());
      PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index());

      const CSCSimHitCollection* seHitColl(nullptr);
      if (!seStore.retrieve(seHitColl,m_inputObjectName).isSuccess())
        { // CSC_HITS
          ATH_MSG_ERROR ("SubEvent CSCSimHitCollection not found in StoreGate " << seStore.name());
          return StatusCode::FAILURE;
        }
      ATH_MSG_VERBOSE ("CSCSimHitCollection found with " << seHitColl->size() << " hits");
      //Copy Hit Collection
      CSCSimHitCollection* cscHitColl = new CSCSimHitCollection(m_inputObjectName);
      CSCSimHitCollection::const_iterator i = seHitColl->begin();
      CSCSimHitCollection::const_iterator e = seHitColl->end();
      // Read hits from this collection
      for (; i!=e; ++i)
        {
          cscHitColl->Emplace(*i);
        }
      m_thpcCSC->insert(thisEventIndex, cscHitColl);
      //store these for deletion at the end of mergeEvent
      m_cscHitCollList.push_back(cscHitColl);
      //John's Hacks END
      ++iEvt;
    }//  while (iEvt != eSubEvents) {

  return StatusCode::SUCCESS;
}

//////////////////////////
StatusCode CscDigitizationTool::mergeEvent() {

  ATH_MSG_DEBUG ( "in mergeEvent()" );

  // create and record the SDO container in StoreGate
  std::string sdoKey = "CSC_SDO";
  CscSimDataCollection *sdoCollection = new CscSimDataCollection();
  if ( (evtStore()->record(sdoCollection,sdoKey)).isFailure() )  {
    ATH_MSG_ERROR ( "Unable to record CSC SDO collection in StoreGate" );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_DEBUG ( "CscSDOCollection recorded in StoreGate." );


  if ( CoreDigitization(sdoCollection).isFailure() ) { // 
    ATH_MSG_ERROR ("mergeEvent() got failure from CoreDigitization()");
    return StatusCode::FAILURE;
  }

  // remove cloned one in processBunchXing......
  std::list<CSCSimHitCollection*>::iterator cscHitColl = m_cscHitCollList.begin();
  std::list<CSCSimHitCollection*>::iterator cscHitCollEnd = m_cscHitCollList.end();
  while(cscHitColl!=cscHitCollEnd)
    {
      delete (*cscHitColl);
      ++cscHitColl;
    }
  m_cscHitCollList.clear();
  
  return StatusCode::SUCCESS;
}
