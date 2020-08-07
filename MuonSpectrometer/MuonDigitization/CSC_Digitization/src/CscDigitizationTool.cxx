/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "CSC_Digitization/CscDigitizationTool.h"
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandFlat.h"

using namespace MuonGM;

CscDigitizationTool::CscDigitizationTool(const std::string& type,const std::string& name,const IInterface* pIID) :
  PileUpToolBase(type, name, pIID) {
}

StatusCode CscDigitizationTool::initialize() {

  ATH_MSG_DEBUG ( "Initialized Properties :" );
  ATH_MSG_DEBUG ( "  InputObjectName           " << m_inputObjectName );
  ATH_MSG_DEBUG ( "  WindowLowerOffset         " << m_timeWindowLowerOffset );
  ATH_MSG_DEBUG ( "  WindowUpperOffset         " << m_timeWindowUpperOffset );
  ATH_MSG_DEBUG ( "  PileUp On?                " << m_isPileUp );
  ATH_MSG_DEBUG ( "  maskBadChannels?          " << m_maskBadChannel );
  ATH_MSG_DEBUG ( "  amplification(gain)       " << m_amplification );
  ATH_MSG_DEBUG ( "  Use NewDigitEDM?          " << m_newDigitEDM );
  ATH_MSG_DEBUG ( "  Drift Velocity Set?       " << m_driftVelocity );
  ATH_MSG_DEBUG ( "  NInteraction per layer from poisson not from energyLoss?  " << m_NInterFixed );
  ATH_MSG_DEBUG ( "  IncludePileUpTruth        " << m_includePileUpTruth );
  ATH_MSG_DEBUG ( "  ParticleBarcodeVeto       " << m_vetoThisBarcode );

  ATH_MSG_DEBUG ( "  RndmSvc                   " << m_rndmSvc.typeAndName() );
  ATH_MSG_DEBUG ( "  cscCalibTool              " << m_pcalib.typeAndName() );
  ATH_MSG_DEBUG ( "  CscSimDataCollection key  " << m_cscSimDataCollectionWriteHandleKey.key());
  ATH_MSG_DEBUG ( "  CscDigitContainer key     " << m_cscDigitContainerKey.key());

  // initialize transient detector store and MuonDetDescrManager
  ATH_CHECK(detStore()->retrieve(m_geoMgr));
  ATH_MSG_DEBUG ( "MuonDetectorManager retrieved from StoreGate.");

  if (m_onlyUseContainerName) {
    ATH_CHECK(m_mergeSvc.retrieve());
  }

  //random number initialization
  ATH_CHECK(m_rndmSvc.retrieve());

  /** CSC calibratin tool for the Condtiions Data base access */
  ATH_CHECK(m_pcalib.retrieve());

  //initialize the CSC digitizer
  m_cscDigitizer = std::make_unique<CSC_Digitizer>(CscHitIdHelper::GetHelper(), m_geoMgr, &*(m_pcalib));
  m_cscDigitizer->setAmplification(m_amplification);
  m_cscDigitizer->setDebug        ( msgLvl(MSG::DEBUG) );
  m_cscDigitizer->setDriftVelocity(m_driftVelocity);
  m_cscDigitizer->setElectronEnergy  (m_electronEnergy);
  if (m_NInterFixed) {
    m_cscDigitizer->setNInterFixed();
  }
  m_cscDigitizer->setWindow(m_timeWindowLowerOffset, m_timeWindowUpperOffset);
  ATH_CHECK(m_cscDigitizer->initialize());
  
  ATH_CHECK(m_idHelperSvc.retrieve());

  // check the input object name
  if (m_hitsContainerKey.key().empty()) {
    ATH_MSG_FATAL("Property InputObjectName not set !");
    return StatusCode::FAILURE;
  }
  if(m_onlyUseContainerName) m_inputObjectName = m_hitsContainerKey.key();
  ATH_MSG_DEBUG("Input objects in container : '" << m_inputObjectName << "'");

  // Initialize ReadHandleKey
  ATH_CHECK(m_hitsContainerKey.initialize(!m_onlyUseContainerName));

  // +++ Initialize WriteHandleKey
  ATH_CHECK(m_cscDigitContainerKey.initialize());
  ATH_CHECK(m_cscSimDataCollectionWriteHandleKey.initialize());

  return StatusCode::SUCCESS;

}

// Inherited from PileUpTools
StatusCode CscDigitizationTool::prepareEvent(const EventContext& /*ctx*/, unsigned int /*nInputEvents*/) {

  if (0 == m_thpcCSC)
    m_thpcCSC = new TimedHitCollection<CSCSimHit>();

  m_cscHitCollList.clear();

  return StatusCode::SUCCESS;
}
///////////////////////////////

StatusCode CscDigitizationTool::processAllSubEvents(const EventContext& ctx) {

  ATH_MSG_DEBUG ( "in processAllSubEvents()" );

  //create and record CscDigitContainer in SG
  SG::WriteHandle<CscDigitContainer> cscDigits(m_cscDigitContainerKey, ctx);
  ATH_CHECK(cscDigits.record(std::make_unique<CscDigitContainer>(m_idHelperSvc->cscIdHelper().module_hash_max())));
  ATH_MSG_DEBUG("recorded CscDigitContainer with name "<<cscDigits.key());

  if (m_isPileUp) return StatusCode::SUCCESS;

  // create and record the SDO container in StoreGate
  SG::WriteHandle<CscSimDataCollection> cscSimData(m_cscSimDataCollectionWriteHandleKey, ctx);
  ATH_CHECK(cscSimData.record(std::make_unique<CscSimDataCollection>()));

  //merging of the hit collection in getNextEvent method
  if (0 == m_thpcCSC ) {
    StatusCode sc = getNextEvent(ctx);
    if (StatusCode::FAILURE == sc) {
      ATH_MSG_INFO ( "There are no CSC hits in this event" );
      return sc; // there are no hits in this event
    }
  }

  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );
  return CoreDigitization(cscDigits.ptr(),cscSimData.ptr(), rngWrapper->getEngine(ctx));

}

StatusCode CscDigitizationTool::CoreDigitization(CscDigitContainer* cscDigits,CscSimDataCollection* cscSimData, CLHEP::HepRandomEngine* rndmEngine) {

  std::map <IdentifierHash,deposits> myDeposits;
  csc_map    data_map;
  csc_newmap data_SampleMap, data_SampleMapOddPhase;

  TimedHitCollection< CSCSimHit >::const_iterator i, e;

  // Perform null check on m_thpcCSC
  if(!m_thpcCSC) {
    ATH_MSG_ERROR ( "m_thpcCSC is null" );
    return StatusCode::FAILURE;
  }

  const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
  // get the iterator pairs for this DetEl
  while( m_thpcCSC->nextDetectorElement(i, e) ) {

    // Loop over the hits:
    while (i != e) {

      TimedHitPtr<CSCSimHit> phit(*i++);
      const CSCSimHit& hit(*phit);


      ATH_MSG_DEBUG(hit.print());



      double globalHitTime(hitTime(phit));
      double bunchTime(globalHitTime - hit.globalTime());

      // Hit time is the G4 hit time plus the drift time.
      // Gloabl time add the bnch crossing time to the hit time.
      ATH_MSG_DEBUG ( "bunch time (ns) " << bunchTime  << "globalHitTime (ns) " << globalHitTime << " hit Time (ns) " << hitTime(phit) );
      m_cscDigitizer->set(bunchTime);

      std::vector<IdentifierHash> hashVec;
      StatusCode status = (m_newDigitEDM)
        ? m_cscDigitizer->digitize_hit(&hit, hashVec, data_SampleMap, data_SampleMapOddPhase, rndmEngine)
        : m_cscDigitizer->digitize_hit(&hit, hashVec, data_map, rndmEngine);

      if (status.isFailure()) {
        ATH_MSG_ERROR ( "CSC Digitizer Failed to digitize a hit!" );
        return status;
      }

      std::vector<IdentifierHash>::const_iterator vecBeg = hashVec.begin();
      std::vector<IdentifierHash>::const_iterator vecEnd = hashVec.end();
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
      const HepMcParticleLink::PositionFlag idxFlag = (phit.eventId()==0) ? HepMcParticleLink::IS_POSITION: HepMcParticleLink::IS_INDEX;
      const HepMcParticleLink trackLink(phit->trackNumber(), phit.eventId(), evColl, idxFlag);
      for (; vecBeg != vecEnd; vecBeg++) {
        CscSimData::Deposit deposit(trackLink, CscMcData(energy, ypos, zpos));
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
    double flat = CLHEP::RandFlat::shoot(rndmEngine, 0.0,1.0);                 // for other particles
    bool phaseToSet = (flat<0.5) ? true : false;
    if (phaseToSet)
      return FillCollectionWithNewDigitEDM(data_SampleMapOddPhase, myDeposits, phaseToSet, cscDigits, cscSimData);
    else
      return FillCollectionWithNewDigitEDM(data_SampleMap, myDeposits, phaseToSet, cscDigits, cscSimData);
  }  else
    return FillCollectionWithOldDigitEDM(data_map, myDeposits, cscDigits, cscSimData);

}


StatusCode CscDigitizationTool::
FillCollectionWithNewDigitEDM(csc_newmap& data_SampleMap,
                              std::map<IdentifierHash,deposits>& myDeposits,
                              bool phaseToSet, CscDigitContainer* cscDigits,CscSimDataCollection* cscSimData
                              ) {

  CscDigitCollection * collection = 0;

  IdContext context    = m_idHelperSvc->cscIdHelper().channel_context();
  IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();

  Identifier prevId;

  csc_newmap::const_iterator cscMap    = data_SampleMap.begin();
  csc_newmap::const_iterator cscMapEnd = data_SampleMap.end();

  for (; cscMap != cscMapEnd; ++cscMap) {
    Identifier digitId;
    IdentifierHash hashId = (*cscMap).first;
    if (m_idHelperSvc->cscIdHelper().get_id( hashId, digitId, &context ) ) {
      ATH_MSG_ERROR ( "cannot get CSC channel identifier from hash " << hashId );
      return StatusCode::FAILURE;
    }

    Identifier elementId = m_idHelperSvc->cscIdHelper().parentID(digitId);
    IdentifierHash coll_hash;
    if (m_idHelperSvc->cscIdHelper().get_hash(elementId, coll_hash, &cscContext)) {
      ATH_MSG_ERROR ( "Unable to get CSC hash id from CSC Digit collection "
                      << "context begin_index = " << cscContext.begin_index()
                      << " context end_index  = " << cscContext.end_index()
                      << " the identifier is " );
      elementId.show();
    }


    // get the charge
    double stripCharge = 0.0;
    double driftTime = 0.0;
    const std::vector<float> samples = (*cscMap).second;

    unsigned int samplingPhase =0;
    double samplingTime = m_pcalib->getSamplingTime();
    m_pcalib->findCharge(samplingTime, samplingPhase, samples, stripCharge, driftTime);
    driftTime += m_pcalib->getLatency();

    /** mask this readout channel if it is a dead channel or a hot channel */
    if ( !m_pcalib->isGood( hashId ) && m_maskBadChannel ) {
      stripCharge = 0.0;
      driftTime   = 2*m_timeWindowUpperOffset;
    }

    int zsec = m_idHelperSvc->cscIdHelper().stationEta(digitId);
    int phisec = m_idHelperSvc->cscIdHelper().stationPhi(digitId);
    int istation = m_idHelperSvc->cscIdHelper().stationName(digitId) - 49;

    int wlay = m_idHelperSvc->cscIdHelper().wireLayer(digitId);
    int measphi = m_idHelperSvc->cscIdHelper().measuresPhi(digitId);
    int istrip = m_idHelperSvc->cscIdHelper().strip(digitId);

    int sector = zsec*(2*phisec-istation+1);

    auto depositsForHash = myDeposits.find(hashId);
    if (depositsForHash != myDeposits.end() && depositsForHash->second.size()) {
      depositsForHash->second[0].second.setCharge(stripCharge);
      cscSimData->insert ( std::make_pair(digitId, CscSimData(depositsForHash->second,0)) );
    }

    // fill the digit collections in StoreGate
    // Now, we pass driftTime as well as stripCharge.
    // SimHIT time should be added to distinguish prompt muons from secondary.... 11/19/2009 WP
    ATH_MSG_DEBUG ( "NEWDigit sec:measphi:wlay:istr:chg:t(w/latency) "
                    << m_idHelperSvc->cscIdHelper().show_to_string(digitId,&context)
                    << " hash:eleId = " << hashId << " " << elementId << " " << prevId << "   "
                    << sector << " " << measphi << " " <<  wlay << " " << istrip << "   "
                    << int(stripCharge+1) << " " << float(driftTime)
                    << " phase=" << phaseToSet
                    << "  samps: " << samples[0] << " " << samples[1] << " "
                    << samples[2] << " " << samples[3]
                    );

    if (prevId != elementId) {
      auto  it_coll = cscDigits->indexFindPtr(coll_hash);
      if (nullptr ==  it_coll) {
        CscDigitCollection * newCollection = new CscDigitCollection(elementId,coll_hash);

        if (phaseToSet) newCollection->set_samplingPhase();

        CscDigit * newDigit  = new CscDigit(digitId, samples);
        newCollection->push_back(newDigit);

        if ( cscDigits->addCollection(newCollection, coll_hash).isFailure() )
          ATH_MSG_ERROR( "Couldn't record CscDigitCollection with key=" << coll_hash << " in StoreGate!");

        collection = newCollection;

      } else {
        CscDigitCollection * existingCollection = const_cast<CscDigitCollection*>( it_coll );
        if (phaseToSet) existingCollection->set_samplingPhase();

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

      CscDigit * newDigit  = new CscDigit(digitId, samples);

      collection->push_back(newDigit);
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode CscDigitizationTool::
FillCollectionWithOldDigitEDM(csc_map& data_map, std::map<IdentifierHash,deposits>& myDeposits,CscDigitContainer* cscDigits,CscSimDataCollection* cscSimData) {

  CscDigitCollection * collection = 0;
  IdContext context    = m_idHelperSvc->cscIdHelper().channel_context();
  IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();

  Identifier prevId;
  csc_map::const_iterator cscMap    = data_map.begin();
  csc_map::const_iterator cscMapEnd = data_map.end();
  for (; cscMap != cscMapEnd; ++cscMap) {
    Identifier digitId;
    IdentifierHash hashId = (*cscMap).first;
    if (m_idHelperSvc->cscIdHelper().get_id( hashId, digitId, &context ) ) {
      ATH_MSG_ERROR ( "cannot get CSC channel identifier from hash " << hashId );
      return StatusCode::FAILURE;
    }

    // get the charge
    double stripCharge = 0.0;
    stripCharge   = ((*cscMap).second).second + m_pedestal; // + m_noiseLevel*gaus;
    double driftTime =((*cscMap).second).first;  // SimHIT time is added yet 12/03/2009

    /** mask this readout channel if it is a dead channel or a hot channel */
    if ( !m_pcalib->isGood( hashId ) && m_maskBadChannel ) {
      stripCharge = 0.0;
      driftTime   = 2*m_timeWindowUpperOffset;
    }

    ATH_MSG_VERBOSE ( "CSC Digit Id = " << m_idHelperSvc->cscIdHelper().show_to_string(digitId,&context)
                      << " hash = " << hashId
                      << " charge = " << int (stripCharge+1) );

    int zsec = m_idHelperSvc->cscIdHelper().stationEta(digitId);
    int phisec = m_idHelperSvc->cscIdHelper().stationPhi(digitId);
    int istation = m_idHelperSvc->cscIdHelper().stationName(digitId) - 49;

    int wlay = m_idHelperSvc->cscIdHelper().wireLayer(digitId);
    int measphi = m_idHelperSvc->cscIdHelper().measuresPhi(digitId);
    int istrip = m_idHelperSvc->cscIdHelper().strip(digitId);

    int sector = zsec*(2*phisec-istation+1);

    auto depositsForHash = myDeposits.find(hashId);
    if (depositsForHash != myDeposits.end() && depositsForHash->second.size()) {
      depositsForHash->second[0].second.setCharge(stripCharge);
      cscSimData->insert ( std::make_pair(digitId, CscSimData(depositsForHash->second,0)) );
    }

    // fill the digit collections in StoreGate
    // Now, we pass driftTime as well as stripCharge.
    // SimHIT time should be added to distinguish prompt muons from secondary.... 11/19/2009 WP
    CscDigit * newDigit  = new CscDigit(digitId, int(stripCharge+1), float(driftTime) );
    Identifier elementId = m_idHelperSvc->cscIdHelper().parentID(digitId);

    ATH_MSG_DEBUG ( "CSC Digit sector:measphi:wlay:istrip:charge "
                    << sector << " "
                    << measphi << " " <<  wlay << " " << istrip
                    << " " << int(stripCharge+1) << " " << float(driftTime) << " " << (newDigit->sampleCharges()).size());


    IdentifierHash coll_hash;
    if (m_idHelperSvc->cscIdHelper().get_hash(elementId, coll_hash, &cscContext)) {
      ATH_MSG_ERROR ( "Unable to get CSC hash id from CSC Digit collection "
                      << "context begin_index = " << cscContext.begin_index()
                      << " context end_index  = " << cscContext.end_index()
                      << " the identifier is " );
      elementId.show();
    }

    if (prevId != elementId) {
      auto it_coll = cscDigits->indexFindPtr(coll_hash);
      if (nullptr ==  it_coll) {
        CscDigitCollection * newCollection = new CscDigitCollection(elementId,coll_hash);
        newCollection->push_back(newDigit);
        collection = newCollection;
        StatusCode status = cscDigits->addCollection(collection, coll_hash );
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


// Get next event and extract collection of hit collections:
StatusCode CscDigitizationTool::getNextEvent(const EventContext& ctx) // This is applicable to non-PileUp Event...
{

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<CSCSimHitCollection>::type TimedHitCollList;

  // In case of single hits container just load the collection using read handles
  if (!m_onlyUseContainerName) {
    SG::ReadHandle<CSCSimHitCollection> hitCollection(m_hitsContainerKey, ctx);
    if (!hitCollection.isValid()) {
      ATH_MSG_ERROR("Could not get CSCSimHitCollection container " << hitCollection.name() << " from store " << hitCollection.store());
      return StatusCode::FAILURE;
    }

    // create a new hits collection
    m_thpcCSC = new TimedHitCollection<CSCSimHit>{1};
    m_thpcCSC->insert(0, hitCollection.cptr());
    ATH_MSG_DEBUG("CSCSimHitCollection found with " << hitCollection->size() << " hits");

    return StatusCode::SUCCESS;
  }

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

  typedef PileUpMergeSvc::TimedList<CSCSimHitCollection>::type TimedHitCollList;
  TimedHitCollList hitCollList;

  if (!(m_mergeSvc->retrieveSubSetEvtData(m_inputObjectName, hitCollList, bunchXing,
                                          bSubEvents, eSubEvents).isSuccess()) &&
        hitCollList.size() == 0) {
    ATH_MSG_ERROR("Could not fill TimedHitCollList");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE(hitCollList.size() << " CSCSimHitCollection with key " <<
                    m_inputObjectName << " found");
  }

  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());

  // Iterating over the list of collections
  for( ; iColl != endColl; iColl++){

    CSCSimHitCollection *hitCollPtr = new CSCSimHitCollection(*iColl->second);
    PileUpTimeEventIndex timeIndex(iColl->first);

    ATH_MSG_DEBUG("CSCSimHitCollection found with " << hitCollPtr->size() <<
                  " hits");
    ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
                    << " index: " << timeIndex.index()
                    << " type: " << timeIndex.type());

    m_thpcCSC->insert(timeIndex, hitCollPtr);
    m_cscHitCollList.push_back(hitCollPtr);

  }

  return StatusCode::SUCCESS;
}

//////////////////////////
StatusCode CscDigitizationTool::mergeEvent(const EventContext& ctx) {

  ATH_MSG_DEBUG ( "in mergeEvent()" );

  //create and record CscDigitContainer in SG
  SG::WriteHandle<CscDigitContainer> cscDigits(m_cscDigitContainerKey, ctx);
  ATH_CHECK(cscDigits.record(std::make_unique<CscDigitContainer>(m_idHelperSvc->cscIdHelper().module_hash_max())));
  ATH_MSG_DEBUG("recorded CscDigitContainer with name "<<cscDigits.key());

  // create and record the SDO container in StoreGate
  SG::WriteHandle<CscSimDataCollection> cscSimData(m_cscSimDataCollectionWriteHandleKey, ctx);
  ATH_CHECK(cscSimData.record(std::make_unique<CscSimDataCollection>()));

  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );
  ATH_CHECK(CoreDigitization(cscDigits.ptr(),cscSimData.ptr(), rngWrapper->getEngine(ctx)));

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
