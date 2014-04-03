/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BCM_DigitizationTool.h"

#include "InDetBCM_RawData/BCM_RawData.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"

#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"

#include "InDetSimEvent/SiHitCollection.h"

#include "InDetSimData/InDetSimDataCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "PileUpTools/PileUpMergeSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
BCM_DigitizationTool::BCM_DigitizationTool(const std::string &type, const std::string &name, const IInterface *parent) :
    PileUpToolBase(type,name,parent),
    m_rndmEngine(NULL),
    m_mipDeposit(0.0f),
    m_effPrmDistance(0.0f),
    m_effPrmSharpness(0.0f),
    m_timeDelay(0.0f),
    m_rdoContainer(NULL),
    m_simDataCollMap(NULL),
    m_rndmEngineName("BCM_Digitization"),
    m_mergeSvc(NULL), //("PileUpMergeSvc",name),
    m_atRndmGenSvc("AtRndmGenSvc",name)
{
  declareInterface<IPileUpTool>(this);
  //declareProperty("PileupMergeSvc", m_mergeSvc, "Pileup merging service");
  declareProperty("RndmSvc", m_atRndmGenSvc, "Random number service used in BCM digitization");
  declareProperty("HitCollName", m_hitCollName="BCMHits", "Input simulation hits collection name");
  declareProperty("ModNoise", m_modNoise, "RMS noise averaged over modules");
  declareProperty("ModSignal", m_modSignal, "Average MIP signal in modules");
  declareProperty("NinoThr", m_ninoThr, "NINO threshold voltage");
  declareProperty("MIPDeposit", m_mipDeposit, "Most probable MIP deposit in BCM pad");
  declareProperty("EffDistanceParam", m_effPrmDistance, "Distance parameter for efficiency calculation");
  declareProperty("EffSharpnessParam", m_effPrmSharpness, "Sharpness parameter for efficiency calculation");
  declareProperty("TimeDelay", m_timeDelay, "Pulse time delay");
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode BCM_DigitizationTool::initialize() {
  msg(MSG::VERBOSE) << "BCM_DigitizationTool::initialize()" << endreq;

  StatusCode sc;

  // get random service
  sc = m_atRndmGenSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not initialize random number service." << endreq;
    return sc;
  };
  m_rndmEngine = m_atRndmGenSvc->GetEngine(m_rndmEngineName) ;
  if (m_rndmEngine==0) {
    msg(MSG::ERROR) << "Could not find RndmEngine : " << m_rndmEngineName << endreq;
    return StatusCode::FAILURE ;
  }

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// createOutpuContainers method:
//----------------------------------------------------------------------
StatusCode BCM_DigitizationTool::createOutputContainers() {

  // Create output RDO container and record it to StoreGate
  try {
    m_rdoContainer = new BCM_RDO_Container();
  } catch (std::bad_alloc) {
    msg(MSG::FATAL) << "Could not create a new BCM RawDataContainer!" << endreq;
    return StatusCode::FAILURE;
  }
  if (evtStore()->record(m_rdoContainer,"BCM_RDOs").isFailure()) {
    msg(MSG::FATAL) << "Container '" << "BCM_RDOs" << "' could not be registered in StoreGate" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Container '" << "BCM_RDOs" << "' registered in StoreGate" << endreq;
  }

  // Create a map for the SDO and register it into StoreGate
  m_simDataCollMap = new InDetSimDataCollection();
  if (evtStore()->record(m_simDataCollMap,"BCM_SDO_Map").isFailure()) {
    msg(MSG::FATAL) << "InDetSimData map '" << "BCM_SDO_Map" << "' could not be registered in StoreGate" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "InDetSimData map '" << "BCM_SDO_Map" << "' registered in StoreGate" << endreq;
  }

  // Clear G4 hit info vectors
  for (unsigned int iMod=0; iMod<8; ++iMod) {
    m_enerVect[iMod].clear();
    m_timeVect[iMod].clear();
    m_depositVect[iMod].clear();
  }

  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------
// ProcessSiHit method:
//----------------------------------------------------------------------
void BCM_DigitizationTool::processSiHit(const SiHit &currentHit, double eventTime, unsigned int evtIndex) {
  int moduleNo = currentHit.getLayerDisk();
  float enerDep = computeEnergy(currentHit.energyLoss(), currentHit.localStartPosition(), currentHit.localEndPosition());
  float hitTime = currentHit.meanTime() + eventTime + m_timeDelay;
  // Fill vectors with hit info
  m_enerVect[moduleNo].push_back(enerDep);
  m_timeVect[moduleNo].push_back(hitTime);
  // Create new deposit and add to vector
  InDetSimData::Deposit deposit(HepMcParticleLink(currentHit.trackNumber(), evtIndex),currentHit.energyLoss());
  int barcode = deposit.first.barcode();
  if (barcode == 0 || barcode == 10001){
      return;
  }
  m_depositVect[moduleNo].push_back(deposit);
  return;
}

//----------------------------------------------------------------------
// createRDOs method:
//----------------------------------------------------------------------
void BCM_DigitizationTool::createRDOsAndSDOs() {
  // Digitize hit info and create RDO for each module
  for (int iMod=0; iMod<8; ++iMod) {
    if (m_depositVect[iMod].size()) m_simDataCollMap->insert(std::make_pair(Identifier(iMod), InDetSimData(m_depositVect[iMod])));
    std::vector<float> analog = createAnalog(iMod,m_enerVect[iMod],m_timeVect[iMod]);
    addNoise(iMod,analog);
    for (int iGain=0; iGain<2; ++iGain) {
      std::bitset<64> digital = applyThreshold(iGain*8+iMod,analog);
      int p1x,p1w,p2x,p2w;
      applyFilter(digital);
      findPulses(digital,p1x,p1w,p2x,p2w);
      fillRDO(iGain*8+iMod,p1x,p1w,p2x,p2w);
    }
  }
  return;
}

//----------------------------------------------------------------------
// PrepareEvent method:
//----------------------------------------------------------------------
StatusCode BCM_DigitizationTool::prepareEvent(unsigned int nInputEvents) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "BCM_DigitizationTool::prepareEvent() called for " << nInputEvents << " input events" << endreq;

  if (createOutputContainers().isFailure()) {
    msg(MSG::FATAL) << "Could not create output containers" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------//
// Digitize method:                                                     //
//----------------------------------------------------------------------//
StatusCode BCM_DigitizationTool::processAllSubEvents() {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "BCM_Digitization::processAllSubEvents()" << endreq;


  if(!m_mergeSvc) {
    //locate the PileUpMergeSvc
    const bool CREATEIF(true);
    if(!(service("PileUpMergeSvc", m_mergeSvc, CREATEIF)).isSuccess() ||
       !m_mergeSvc) {
      ATH_MSG_FATAL("digitize: Could not find PileUpMergeSvc");
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("digitize: retrieved PileUpMergeSvc");
  }
  else ATH_MSG_DEBUG("digitize: PileUpMergeSvc already available");  

  if (createOutputContainers().isFailure()) {
    msg(MSG::FATAL) << "Could not create output containers" << endreq;
    return StatusCode::FAILURE;
  }

  // Fetch SiHitCollections for this bunch crossing
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
  TimedHitCollList hitCollList;
  if (!(m_mergeSvc->retrieveSubEvtsData(m_hitCollName, hitCollList).isSuccess()) && hitCollList.size()==0) {
    msg(MSG::ERROR) << "Could not fill TimedHitCollList" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << hitCollList.size() << " SiHitCollections with key " << m_hitCollName << " found" << endreq;
  }

  // Store hit info in vectors and fill SDO map
  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());
  for (; iColl != endColl; ++iColl) {
    const SiHitCollection* tmpColl(iColl->second);
    HepMcParticleLink::index_type evtIndex = (iColl->first).index();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SiHitCollection found with " << tmpColl->size() << " hits" << endreq;
    // Read hits from this collection
    SiHitCollection::const_iterator i = tmpColl->begin();
    SiHitCollection::const_iterator e = tmpColl->end();
    for (; i!=e; ++i) {
      processSiHit(*i, (iColl->first).time(), evtIndex);
    }
  }

  createRDOsAndSDOs();

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// ProcessBunchXing method:
//----------------------------------------------------------------------
StatusCode BCM_DigitizationTool::processBunchXing(int bunchXing, PileUpEventInfo::SubEvent::const_iterator bSubEvents, PileUpEventInfo::SubEvent::const_iterator eSubEvents) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "BCM_DigitizationTool::processBunchXing() " << bunchXing << endreq;

  PileUpEventInfo::SubEvent::const_iterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; ++iEvt) {
    StoreGateSvc& seStore = *iEvt->pSubEvtSG;
    const EventInfo* pEI = 0;
    if (seStore.retrieve(pEI).isSuccess()) {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "SubEvt EventInfo from StoreGate " << seStore.name() << " :"
                                                  << " bunch crossing : " << bunchXing
                                                  << " time offset : " << iEvt->time()
                                                  << " event number : " << pEI->event_ID()->event_number()
                                                  << " run number : " << pEI->event_ID()->run_number()
                                                  << endreq;
    }
    const SiHitCollection* seHitColl = 0;
    if (!seStore.retrieve(seHitColl,m_hitCollName).isSuccess()) {
      msg(MSG::ERROR) << "SubEvent BCM SiHitCollection not found in StoreGate " << seStore.name() << endreq;
      return StatusCode::FAILURE;
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SiHitCollection found with " << seHitColl->size() << " hits" << endreq;
    SiHitCollection::const_iterator i = seHitColl->begin();
    SiHitCollection::const_iterator e = seHitColl->end();
    // Read hits from this collection
    for (; i!=e; ++i) {
      processSiHit(*i, iEvt->time(), iEvt->index());
    }
  }

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// MergeEvent method:                                                   
//----------------------------------------------------------------------
StatusCode BCM_DigitizationTool::mergeEvent() {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "BCM_DigitizationTool::mergeEvent()" << endreq;

  createRDOsAndSDOs();

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// ComputeEnergy method:
//----------------------------------------------------------------------
float BCM_DigitizationTool::computeEnergy(float simEner, HepGeom::Point3D<double> startPos, HepGeom::Point3D<double> endPos) {
  // Initialize output energy
  float calcEner = 0;
  // Break hit up into 10 discrete energy deposits
  // For each deposit, weight energy by charge collection efficiency based on position on diamond surface
  float xStep, yStep, rStep, r0Step, effStep;
  for (int iStep=0; iStep<10; ++iStep) {
    xStep = startPos.x()+iStep*(endPos.x()-startPos.x())/9;
    yStep = startPos.y()+iStep*(endPos.y()-startPos.y())/9;
    if (xStep==0 && yStep==0) effStep = 1.;
    else {
      rStep = sqrt(pow(xStep,2)+pow(yStep,2));
      r0Step = fabs(yStep)>fabs(xStep) ? rStep*m_effPrmDistance/fabs(yStep) : rStep*m_effPrmDistance/fabs(xStep);
      effStep = 1/(1+exp((rStep-r0Step)/m_effPrmSharpness));
    }
    calcEner+= 0.1*simEner*effStep;
  }
  return calcEner;
}

//----------------------------------------------------------------------
// CreateAnalog method:
//----------------------------------------------------------------------
std::vector<float> BCM_DigitizationTool::createAnalog(int iMod, std::vector<float> enerVect, std::vector<float> timeVect)
{
  std::vector<float> analog(64,0);
  for (unsigned int iHit=0; iHit<enerVect.size(); ++iHit) {
    float enerDep = enerVect.at(iHit);
    float hitTime = timeVect.at(iHit);
    int startBin = (int)(hitTime*64/25);
    float signalMax = enerDep*m_modSignal[iMod]/m_mipDeposit;
    float slopeup = signalMax/5; // pulse rise in 2ns ~ 5 bins @ 390ps/bin
    float slopedown = signalMax/10; // pulse fall in 3.8ns ~ 10 bins @ 390ps/bin
    int iBin = startBin-1;
    float signal = 0;
    if (iBin>=0 && startBin<64) {
      while (signal>=0 && iBin<64) {
        analog[iBin] += signal;
        signal += slopeup;
        if (iBin > startBin+4) signal -= slopeup+slopedown;
        iBin++;
      }
    }
  }
  return analog;
}

//----------------------------------------------------------------------
// AddNoise method:
//----------------------------------------------------------------------
void BCM_DigitizationTool::addNoise(int iMod, std::vector<float> &analog)
{
  for (unsigned int iBin=0; iBin<analog.size(); ++iBin) analog[iBin]+=CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,0.,m_modNoise[iMod]);
}

//----------------------------------------------------------------------
// ApplyThreshold method:
//----------------------------------------------------------------------
std::bitset<64> BCM_DigitizationTool::applyThreshold(int iChan, std::vector<float> analog)
{
  std::bitset<64> digital;
  digital.reset();
  float factor = iChan<8 ? 1./13 : 12./13;
  for (int iBin=0; iBin<64; ++iBin)
    if (analog[iBin]*factor>m_ninoThr[iChan%8]) digital.set(iBin);
  return digital;
}

//----------------------------------------------------------------------
// ApplyFilter method:
//----------------------------------------------------------------------
void BCM_DigitizationTool::applyFilter(std::bitset<64> &digital) {
  // 1101, 1011 -> 1111
  for (int iSamp=2; iSamp<63; iSamp++) {
    if (digital[iSamp-2] && digital[iSamp-1] && !digital[iSamp] && digital[iSamp+1]) digital[iSamp] = 1;
    if (digital[iSamp-2] && !digital[iSamp-1] && digital[iSamp] && digital[iSamp+1]) digital[iSamp-1] = 1;
  }
  // 010 -> 000
  if (digital[0] && !digital[1]) digital[0] = 0;
  for (int iSamp=1; iSamp<63; iSamp++) {
    if (!digital[iSamp-1] && digital[iSamp] && !digital[iSamp+1]) digital[iSamp] = 0;
  }
  if (!digital[62] && digital[63]) digital[63] = 0;
}

//----------------------------------------------------------------------
// FindPulses method:
//----------------------------------------------------------------------
void BCM_DigitizationTool::findPulses(std::bitset<64> digital, int &p1x, int &p1w, int &p2x, int &p2w)
{
  p1x = 0; p1w = 0; p2x = 0; p2w = 0;

  if (!digital.count()) return; // skip if waveform is empty

  bool p1done = false, p2done = false; bool ignorepulse = false;
  if (digital[0] && digital[1]) ignorepulse = true;
  else if (!digital[0] && digital[1] && digital[2]) p1x = 1;
  for (int iBin=2; iBin<63; iBin++) {
    if (!digital[iBin-2] && !digital[iBin-1] && digital[iBin] && digital[iBin+1]) { // rising edge
      if (!p1done && !p2done) p1x = iBin;
      else if (p1done && !p2done) p2x = iBin;
    }
    else if (digital[iBin-2] && digital[iBin-1] && !digital[iBin] && !digital[iBin+1]) { // falling edge
      if (!ignorepulse) {
        if (!p1done && !p2done) {
          p1w = iBin - p1x;
          p1done = true;
          if (p1w >= 32) {
            p2x = p1x + 31;
            p2w = p1w - 31;
            p2done = true;
            p1w = 31;
          }
        }
        else if (p1done && !p2done) {
          p2w = iBin - p2x;
          p2done = true;
          if (p2w >= 32) p2w = 31;
        }
      }
      else ignorepulse = false;
    }
  }
  if (digital[61] && digital[62] && !digital[63]) {
    if (!ignorepulse) {
      if (!p1done && !p2done) {
        p1w = 63 - p1x;
        p1done = true;
        if (p1w >= 32) {
          p2x = p1x + 31;
          p2w = p1w - 31;
          p2done = true;
          p1w = 31;
        }
      }
      else if (p1done && !p2done) {
        p2w = 63 - p2x;
        p2done = true;
        if (p2w >= 32) p2w = 31;
      }
    }
    else ignorepulse = false;
  }
  else if (digital[62] && digital[63]) {
    if (!ignorepulse) {
      if (!p1done && !p2done) {
        p1w = 1;
        p1done = true;
        if (64 - p1x >= 32) {
          p2x = p1x + 31;
          p2w = 1;
          p2done = true;
          p1w = 31;
        }
      }
      else if (p1done && !p2done) {
        p2w = 1;
        p2done = true;
        if (64 - p2x >= 32) p2w = 31;
      }
    }
    else ignorepulse = false;
  }
}

//----------------------------------------------------------------------
// FillRDO method:
//----------------------------------------------------------------------
void BCM_DigitizationTool::fillRDO(unsigned int chan, int p1x, int p1w, int p2x, int p2w)
{
  BCM_RDO_Collection *RDOColl = 0;
  // Check if collection for this channel already exists
  bool collExists = false;
  BCM_RDO_Container::const_iterator it_coll = m_rdoContainer->begin();
  BCM_RDO_Container::const_iterator it_collE= m_rdoContainer->end();
  for (; it_coll!=it_collE; ++it_coll) {
    if ((*it_coll)->getChannel()==chan) {
      collExists = true;
      RDOColl = const_cast<BCM_RDO_Collection*>(&**it_coll);
    }
  }
  // Create the RDO collection if needed
  if (!collExists) {
    RDOColl = new BCM_RDO_Collection(chan);
    // Add collection to container
    m_rdoContainer->push_back(RDOColl);
  }
  // Create RDO & add it to collection
  BCM_RawData *bcmrdo = new BCM_RawData(chan,p1x,p1w,p2x,p2w,0,0,0);
  if (bcmrdo) RDOColl->push_back(bcmrdo);
}
