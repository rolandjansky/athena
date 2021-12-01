/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_Digitization/AFP_PileUpTool.h"
#include <algorithm>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AFP_SimEv/AFP_TDSimHit.h"
#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_DigiEv/AFP_TDDigiCollection.h"
#include "AFP_DigiEv/AFP_SiDigiCollection.h"
#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitAuxContainer.h"
#include "xAODForward/AFPToFHit.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHitAuxContainer.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "Identifier/Identifier.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <map>
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Random/RandPoissonQ.h"
//#include "AFP_G4_SD/AFP_SensitiveDetector.h" 

AFP_PileUpTool::AFP_PileUpTool(const std::string& type,
			       const std::string& name,
			       const IInterface* parent) :
  PileUpToolBase(type, name, parent),
  m_mergeSvc    ("PileUpMergeSvc", name), 
  m_atRndmGenSvc("AtRndmGenSvc", name),
  m_rndEngine(0),
  m_CollectionEff(0.6),       // Collection efficincy
  m_ConversionSpr(40.0),      // Photon-Electron conversion spread in ps
  m_Gain(50000.),             // Gain  
  m_RiseTime(400.),           // Pulse rise time in ps
  m_FallTime(1200.),          // Pulse fall time in ps 
  m_CfdThr(0.5)              // Constant fraction threshold
{
  declareInterface<AFP_PileUpTool>(this); //Temporary for back-compatibility with 17.3.X.Y
  //declareInterface<IPileUpTool>(this); //Temporary for back-compatibility with 17.3.X.Y
 
  // NOTE: The following variables are actually re-initialized by AFP_DigiTop::initialize() or AFP_PileUpTool::initialize()

  // Quantum efficincy of PMT in 100 nm steps int(WeaveLength-100)/100

  m_QuantumEff_PMT[0] = 0.00; 
  m_QuantumEff_PMT[1] = 0.15; 
  m_QuantumEff_PMT[2] = 0.20;    
  m_QuantumEff_PMT[3] = 0.15;
  m_QuantumEff_PMT[4] = 0.05;    
  m_QuantumEff_PMT[5] = 0.01; 
  m_QuantumEff_PMT[6] = 0.00; 

  m_digitCollection = 0; // initializing to null pointer
  m_SiDigiCollection = 0;

  // end of ToolBox variables

  m_TDSimHitCollectionName = "AFP_TDSimHitCollection";
  m_TDDigiCollectionName    = "AFP_TDDigiCollection";

  m_SIDSimHitCollectionName = "AFP_SIDSimHitCollection";
  m_SiDigiCollectionName    = "AFP_SiDigiCollection";


  m_AFPSiHitsContainerName = "AFPSiHitContainer";
  m_AFPHitsContainerNameToF = "AFPToFHitContainer";

  m_mergedTDSimHitList = 0; // initialized to null pointer
  m_mergedSIDSimHitList = 0;  
  
  declareProperty( "AFPSiHitsContainerName", m_AFPSiHitsContainerName = "AFPSiHitContainer" );
  declareProperty( "AFPHitsContainerNameToF", m_AFPHitsContainerNameToF = "AFPToFHitContainer" );

  declareProperty("TDSimHitCollection" , m_TDSimHitCollectionName, "Name of the input Collection of the simulated TD Hits");
  declareProperty("TDDigiCollectionName", m_TDDigiCollectionName,    "Name of the Collection to hold the output from the AFP digitization, TD part");
  
  declareProperty("SIDSimHitCollection" , m_SIDSimHitCollectionName, "Name of the input Collection of the simulated SiD hits");
  declareProperty("SiDigiCollectionName", m_SiDigiCollectionName,    "Name of the Collection to hold the output from the AFP digitization, SiD part");
   
  
  declareProperty("RndmSvc",                m_atRndmGenSvc,              "Random Number Service used in AFP digitization" );
  declareProperty("mergeSvc",               m_mergeSvc,                  "Store to hold the pile-ups");

 
  declareProperty("CollectionEff", m_CollectionEff);
  declareProperty("ConversionSpr", m_ConversionSpr);
  declareProperty("FallTime", m_FallTime);
  declareProperty("RiseTime", m_RiseTime);
  declareProperty("CfdThr", m_CfdThr);
  declareProperty("Gain", m_Gain);

  m_SignalVect.resize( int((m_RiseTime+5*m_FallTime)/5.));

  if (!(m_SignalVect.empty()))
  {
     for(unsigned long i=0; i< m_SignalVect.size(); i++) { // size is an unsigned long so abs is redundant
       m_SignalVect[i] = SignalFun(2.5+5.*i,m_RiseTime,m_FallTime);
     }
  }
  else ATH_MSG_DEBUG("m_SignalVector is empty");  


   TString Id1, Id2, Id3;

    for( int i=0; i<4; i++) {
       Id1.Form("%d",i);
       for( int j=0; j<49; j++) {
          Id2.Form("%d",j);
          for( int k=0; k<2; k++) {
             Id3.Form("%d",k);
                m_Signal[i][j][k] =  TH1F("Signal_"+Id1+"_"+Id2+"_"+Id3,"",2000,105000,115000);
                                  }
                               }
                            }


}

StatusCode AFP_PileUpTool::initialize() {

  ATH_MSG_DEBUG ( "AFP_PileUpTool::initialize() called" );
  


  ATH_MSG_DEBUG ( "    CollectionEff: " << m_CollectionEff << endreq
//		 << " ScalePixel: " << m_ScalePixel << endreq
		    );
  
  if (m_atRndmGenSvc.retrieve().isFailure()) { 
    ATH_MSG_FATAL ( "Could not retrieve RandomNumber Service!" ); 
    return StatusCode::FAILURE; 
  }
  else { 
    ATH_MSG_DEBUG ( "Retrieved RandomNumber Service" ); 
  }

  m_mergedTDSimHitList = new AFP_TDSimHitCollection("mergedTDSimHitList");
  m_mergedSIDSimHitList = new AFP_SIDSimHitCollection("mergedSIDSimHitList");

  
  return StatusCode::SUCCESS;
}

StatusCode AFP_PileUpTool::recoAll()
{
  ATH_CHECK( recoSiHits() );
  ATH_CHECK( recoToFHits() );

  return StatusCode::SUCCESS;
}


StatusCode AFP_PileUpTool::recoSiHits()
{
  ATH_MSG_DEBUG("AFP_PileUpTool recoSiHits ");

  // create output containers
  m_SiHitCollection = new xAOD::AFPSiHitContainer();
  ATH_CHECK( evtStore()->record(m_SiHitCollection, m_AFPSiHitsContainerName) );
  xAOD::AFPSiHitAuxContainer* siHitAuxContainer = new xAOD::AFPSiHitAuxContainer();
  ATH_CHECK( evtStore()->record(siHitAuxContainer, m_AFPSiHitsContainerName + "Aux.") );
  m_SiHitCollection->setStore(siHitAuxContainer);

  // retrieve digi data
  const AFP_SiDigiCollection  *container = nullptr;

  ATH_CHECK( evtStore()->retrieve(container, m_SiDigiCollectionName) );
//  if (evtStore()->retrieve(container, m_SiDigiCollectionName).isFailure()) {
//    ATH_MSG_WARNING("AFP_PileUpTool: Could not find simulated digi container");
//    return StatusCode::SUCCESS;
//  }
//  else
   ATH_MSG_DEBUG("AFP_PileUpTool: Simulated digi container retrieved");

  newXAODHitSi (m_SiHitCollection, container);

  return StatusCode::SUCCESS;
}


void  AFP_PileUpTool::newXAODHitSi (xAOD::AFPSiHitContainer* siHitContainer, const AFP_SiDigiCollection* container) const
{
  AFP_SiDigiConstIter it    = container->begin();
  AFP_SiDigiConstIter itend = container->end();

  for (; it != itend; it++) {
  xAOD::AFPSiHit* xAODSiHit = new xAOD::AFPSiHit();
  siHitContainer->push_back(xAODSiHit);
 xAODSiHit->setStationID(it->m_nStationID);
 xAODSiHit->setPixelLayerID(it->m_nDetectorID);
 xAODSiHit->setPixelColIDChip(80-it->m_nPixelRow); // Chip is rotated by 90 degree Row-->Col
 xAODSiHit->setPixelRowIDChip(336-it->m_nPixelCol); // Chip	is rotated by 90 degree	Col-->Row 
 xAODSiHit->setTimeOverThreshold(it->m_fADC );
 xAODSiHit->setDepositedCharge(it->m_fADC );
  }
 ATH_MSG_DEBUG("AFP_PileUpTool:  Filled xAOD::AFPSiHit");

}



StatusCode AFP_PileUpTool::recoToFHits()
{
  ATH_MSG_DEBUG("AFP_PileUpTool recoToFHits ");

  // create output containers
  m_TDHitCollection   = new xAOD::AFPToFHitContainer();
  ATH_CHECK( evtStore()->record(m_TDHitCollection, m_AFPHitsContainerNameToF) );
  xAOD::AFPToFHitAuxContainer* tofHitAuxContainer = new xAOD::AFPToFHitAuxContainer();
  ATH_CHECK( evtStore()->record(tofHitAuxContainer, m_AFPHitsContainerNameToF + "Aux.") );
  m_TDHitCollection->setStore(tofHitAuxContainer);


  // retrieve digi data
  const AFP_TDDigiCollection  *container = nullptr;
  if (evtStore()->retrieve(container, m_TDDigiCollectionName).isFailure()) {
    ATH_MSG_WARNING("AFP_PileUpTool: Could not find simulated digi container");
    return StatusCode::SUCCESS;
  }
  else
    ATH_MSG_DEBUG("AFP_PileUpTool: Simulated digi container retrieved");

  newXAODHitToF (m_TDHitCollection, container);

  return StatusCode::SUCCESS;
}

void  AFP_PileUpTool::newXAODHitToF (xAOD::AFPToFHitContainer* tofHitContainer, const AFP_TDDigiCollection* container) const
{
  AFP_TDDigiConstIter it    = container->begin();
  AFP_TDDigiConstIter itend = container->end();

  for (; it != itend; it++) {
  xAOD::AFPToFHit* xAODToFHit = new xAOD::AFPToFHit();
  tofHitContainer->push_back(xAODToFHit);
 xAODToFHit->setStationID(it->m_nStationID);
 xAODToFHit->setHptdcChannel(-1);
 xAODToFHit->setBarInTrainID(it->m_nDetectorID%10-1);
 xAODToFHit->setTrainID(it->m_nDetectorID/10-1);
 xAODToFHit->setHptdcID(-1);
 xAODToFHit->setPulseLength(it->m_fADC);
 xAODToFHit->setTime(it->m_fTDC);
  }
 ATH_MSG_DEBUG("AFP_PileUpTool:  Filled xAOD::AFPToFHit");

}


StatusCode AFP_PileUpTool::processAllSubEvents() {

  ATH_MSG_DEBUG ( "AFP_PileUpTool::processAllSubEvents()" );
 
  //retrieve the PileUpMergeSvc if necessary
  if(!m_mergeSvc) {
    if(!m_mergeSvc.retrieve().isSuccess() || !m_mergeSvc) {
      ATH_MSG_FATAL("digitize: Could not find PileUpMergeSvc");
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("digitize: retrieved PileUpMergeSvc");
  }
  else ATH_MSG_DEBUG("digitize: PileUpMergeSvc already available"); 

  typedef PileUpMergeSvc::TimedList<AFP_TDSimHitCollection>::type TimedTDSimHitCollList;
  typedef PileUpMergeSvc::TimedList<AFP_SIDSimHitCollection>::type TimedSIDSimHitCollList;  
  
  TimedTDSimHitCollList TDSimHitCollList;
  TimedSIDSimHitCollList SIDSimHitCollList;  
  
  if (m_mergeSvc->retrieveSubEvtsData(m_TDSimHitCollectionName, TDSimHitCollList).isFailure()) { 


    ATH_MSG_FATAL ( "Could not fill TimedTDSimHitCollList" ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( "Retrieved TimedTDSimHitCollList" ); }
  
  ATH_MSG_DEBUG ( " PileUp: Merge " << TDSimHitCollList.size() << " AFP_TDSimHitCollection with key " << m_TDSimHitCollectionName );
  
  
  if (m_mergeSvc->retrieveSubEvtsData(m_SIDSimHitCollectionName, SIDSimHitCollList).isFailure()) { 


    ATH_MSG_FATAL ( "Could not fill TimedSIDSimHitCollList" ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( "Retrieved TimedSIDSimHitCollList" ); }
  
  ATH_MSG_DEBUG ( " PileUp: Merge " << SIDSimHitCollList.size() << " AFP_SIDSimHitCollection with key " << m_SIDSimHitCollectionName );

  
  TimedHitCollection<AFP_TDSimHit> thpcAFP_TDPmt;

  TimedTDSimHitCollList::iterator iColl  (TDSimHitCollList.begin());
  TimedTDSimHitCollList::iterator endColl(TDSimHitCollList.end());
  
  while (iColl != endColl) {
    
    const AFP_TDSimHitCollection* tmpColl(iColl->second);
    
    thpcAFP_TDPmt.insert(iColl->first, tmpColl);

    ATH_MSG_DEBUG ( " AFP_TDSimHitCollection found with " << tmpColl->size() << " hits " << iColl->first );
    
    ++iColl;
  }

  
  TimedHitCollection<AFP_SIDSimHit> thpcAFP_SiPmt;

  TimedSIDSimHitCollList::iterator iSiColl  (SIDSimHitCollList.begin());
  TimedSIDSimHitCollList::iterator endSiColl(SIDSimHitCollList.end());
  
  while (iSiColl != endSiColl) {
    
    const AFP_SIDSimHitCollection* tmpSiColl(iSiColl->second);
    
    thpcAFP_SiPmt.insert(iSiColl->first, tmpSiColl);

    ATH_MSG_DEBUG ( " AFP_SIDSimHitCollection found with " << tmpSiColl->size() << " hits " << iSiColl->first );
    
    ++iSiColl;
  }

  

  m_rndEngine = m_atRndmGenSvc->GetEngine("AFPRndEng");

  if (recordContainers(this->evtStore(), m_TDDigiCollectionName).isFailure()) { 

    ATH_MSG_FATAL ( " AFP DigiTop :: Could not record the empty TiD digit container in StoreGate " ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( " AFP DigiTop :: TiD digit container is recorded in StoreGate " ); }


  if (recordSiCollection(this->evtStore(), m_SiDigiCollectionName).isFailure()) { 

    ATH_MSG_FATAL ( " AFP DigiTop :: Could not record the empty SiD digit container in StoreGate " ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( " AFP DigiTop :: SiD digit container is recorded in StoreGate " ); }

  
 
  fillTDDigiCollection(thpcAFP_TDPmt, m_rndEngine);
  fillSiDigiCollection(thpcAFP_SiPmt);  
  
  ATH_CHECK( recoAll() );
  
  return StatusCode::SUCCESS;
}


StatusCode AFP_PileUpTool::prepareEvent(const unsigned int nInputEvents){

  ATH_MSG_DEBUG ( "AFP_PileUpTool::prepareEvent() called for " << nInputEvents << " input events" );
 
  StatusCode sc = recordContainers(this->evtStore(), m_TDDigiCollectionName);
 
  if (sc.isFailure()) { ATH_MSG_FATAL ( " Could not record the empty TiD digit container in StoreGate " ); return sc; }
  else                { ATH_MSG_DEBUG ( " TiD digit container is recorded in StoreGate " ); }
  
  m_mergedTDSimHitList->clear();
  
  sc = recordSiCollection(this->evtStore(), m_SiDigiCollectionName);
 
  if (sc.isFailure()) { ATH_MSG_FATAL ( " Could not record the empty SiD digit container in StoreGate " ); return sc; }
  else                { ATH_MSG_DEBUG ( " SiD digit container is recorded in StoreGate " ); }
  
  m_mergedSIDSimHitList->clear();
  
  
  return StatusCode::SUCCESS;
}

StatusCode AFP_PileUpTool::processBunchXing(int bunchXing,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents) {
  ATH_MSG_DEBUG ( "AFP_PileUpTool::processBunchXing() " << bunchXing );
  SubEventIterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; iEvt++) {
    StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
    //PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index());   // does not do anything
    ATH_MSG_VERBOSE ("SubEvt StoreGate " << seStore.name() << " :"
                     << " bunch crossing : " << bunchXing
                     << " time offset : " << iEvt->time()
                     << " event number : " << iEvt->ptr()->eventNumber()
                     << " run number : " << iEvt->ptr()->runNumber()
                     );

    const AFP_TDSimHitCollection* tmpColl = 0;
   
    if (!seStore.retrieve(tmpColl, m_TDSimHitCollectionName).isSuccess()) {
      
      ATH_MSG_ERROR ( "SubEvent AFP_TDSimHitCollection not found in StoreGate " << seStore.name() );
      
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG ( "AFP_TDSimHitCollection found with " << tmpColl->size() << " hits" );
    
    AFP_TDSimHitCollection::const_iterator iPmt = tmpColl->begin();
    AFP_TDSimHitCollection::const_iterator ePmt = tmpColl->end();
   
    for (; iPmt!=ePmt; ++iPmt) m_mergedTDSimHitList->push_back((*iPmt));


    const AFP_SIDSimHitCollection* tmpSiColl = 0;
   
    if (!seStore.retrieve(tmpSiColl, m_SIDSimHitCollectionName).isSuccess()) {
      
      ATH_MSG_ERROR ( "SubEvent AFP_SIDSimHitCollection not found in StoreGate " << seStore.name() );
      
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG ( "AFP_TDSimHitCollection found with " << tmpSiColl->size() << " hits" );
    
    AFP_SIDSimHitCollection::const_iterator iSiPmt = tmpSiColl->begin();
    AFP_SIDSimHitCollection::const_iterator eSiPmt = tmpSiColl->end();
   
    for (; iSiPmt!=eSiPmt; ++iSiPmt) m_mergedSIDSimHitList->push_back((*iSiPmt));
    
    
    m_rndEngine = m_atRndmGenSvc->GetEngine("AFPRndEng");
  
  }
  
  return StatusCode::SUCCESS;
}

StatusCode AFP_PileUpTool::mergeEvent(){

 
  fillTDDigiCollection(m_mergedTDSimHitList, m_rndEngine);
  fillSiDigiCollection(m_mergedSIDSimHitList);
  

  return StatusCode::SUCCESS;
}



StatusCode AFP_PileUpTool::finalize() { 
return StatusCode::SUCCESS; }
 


void AFP_PileUpTool::fillTDDigiCollection(TimedHitCollection<AFP_TDSimHit>& thpcAFP, CLHEP::HepRandomEngine* rndEngine) 
{
  TimedHitCollection<AFP_TDSimHit>                thpc = thpcAFP;
  TimedHitCollection<AFP_TDSimHit>::const_iterator i, e, it;
 
     for( int i=0; i<4; i++) {
       for( int j=0; j<49; j++) {
          for( int k=0; k<2; k++) {
                m_Signal[i][j][k].Reset();
                                  }
                               }
                            }
 
 

  while (thpc.nextDetectorElement(i, e)) for (it = i; it != e; it++) {
    
    int Station     = (*it)->m_nStationID;
    int Detector    = (*it)->m_nDetectorID;
    int SensitiveElement = (*it)->m_nSensitiveElementID;
    float GlobalTime     =  (*it)->m_fGlobalTime;
    float WaveLength = (*it)->m_fWaveLength;

    if(SensitiveElement%2 == 1) createTDDigi(Station, Detector, SensitiveElement, GlobalTime, WaveLength, rndEngine);
  }
    StoreTDDigi();
}

void AFP_PileUpTool::fillTDDigiCollection(const AFP_TDSimHitCollection* AFP_TDSimHitCollection, 
CLHEP::HepRandomEngine* rndEngine)
{

    for( int i=0; i<4; i++) {
       for( int j=0; j<49; j++) {
          for( int k=0; k<2; k++) {
                m_Signal[i][j][k].Reset();
                                  }
                               }
                            }


  AFP_TDSimHitConstIter it    = AFP_TDSimHitCollection->begin();
  AFP_TDSimHitConstIter itend = AFP_TDSimHitCollection->end();

  
  for (; it != itend; it++) {

    int Station     = it->m_nStationID;
    int Detector = it->m_nDetectorID;
    int SensitiveElement = it->m_nSensitiveElementID;
    float GlobalTime     =  it->m_fGlobalTime;
    float WaveLength = it->m_fWaveLength;


    if(SensitiveElement%2 == 1) createTDDigi(Station, Detector, SensitiveElement, GlobalTime, WaveLength, rndEngine);
  }
    StoreTDDigi();
}


void AFP_PileUpTool::fillSiDigiCollection(TimedHitCollection<AFP_SIDSimHit>& thpcAFP) 
{
  TimedHitCollection<AFP_SIDSimHit>           thpc = thpcAFP;
  TimedHitCollection<AFP_SIDSimHit>::const_iterator i, e, it;

  //ATH_MSG_DEBUG ( "bum 1" ); 

  long index = 0;
//  const long indexend = (long)width * (long)height * (long)depth;
  while ( index != 645120 ) // here just 6 layers per detector are considered
  {
            m_deposited_charge[index++] = 0;
  } 
 
 
  while (thpc.nextDetectorElement(i, e)) for (it = i; it != e; it++) {
    
    int Station     = (*it)->m_nStationID;
    int Detector    = (*it)->m_nDetectorID;
    int PixelRow    = (*it)->m_nPixelRow;
    int PixelCol    = (*it)->m_nPixelCol;
    float PreStepX  = (*it)->m_fPreStepX;
    float PreStepY  = (*it)->m_fPreStepY;
    float PreStepZ  = (*it)->m_fPreStepZ;
    float PostStepX = (*it)->m_fPostStepX;
    float PostStepY = (*it)->m_fPostStepY;
    float PostStepZ = (*it)->m_fPostStepZ;
    float DepEnergy = (*it)->m_fEnergyDeposit;

    createSiDigi(Station, Detector, PixelRow, PixelCol, PreStepX, PreStepY, PreStepZ, PostStepX, PostStepY, PostStepZ,  DepEnergy);
  }
    StoreSiDigi();
}


void AFP_PileUpTool::fillSiDigiCollection(const AFP_SIDSimHitCollection* AFP_SIDSimHitCollection)
{
  AFP_SIDSimHitConstIter it    = AFP_SIDSimHitCollection->begin();
  AFP_SIDSimHitConstIter itend = AFP_SIDSimHitCollection->end();

 //ATH_MSG_DEBUG ( "bum 2" );


  long index = 0;
//  const long indexend = (long)width * (long)height * (long)depth;
  while ( index != 645120 ) // here just 6 layers per detector are considered
  {
            m_deposited_charge[index++] = 0;
  } 


  
  for (; it != itend; it++) {

    int Station     = it->m_nStationID;
    int Detector    = it->m_nDetectorID;
    int PixelRow    = it->m_nPixelRow;
    int PixelCol    = it->m_nPixelCol;
    float PreStepX  = it->m_fPreStepX;
    float PreStepY  = it->m_fPreStepY;
    float PreStepZ  = it->m_fPreStepZ;
    float PostStepX = it->m_fPostStepX;
    float PostStepY = it->m_fPostStepY;
    float PostStepZ = it->m_fPostStepZ;
    float DepEnergy = it->m_fEnergyDeposit;

    createSiDigi(Station, Detector, PixelRow, PixelCol, PreStepX, PreStepY, PreStepZ, PostStepX, PostStepY, PostStepZ, DepEnergy);
  }
    StoreSiDigi();
}



void AFP_PileUpTool::createTDDigi(int Station, int Detector, int SensitiveElement, float GlobalTime, float 
                                                  WaveLength, CLHEP::HepRandomEngine* rndEngine)
{
  ATH_MSG_DEBUG ( " iterating Pmt " << Station << "  " << Detector << "  " << SensitiveElement << " " << GlobalTime 
                                    << WaveLength ); 

  int id =  (int(WaveLength)-100)/100;
  if (id > 6) id=6;
  if (id < 0) id=0;

  
  if (CLHEP::RandFlat::shoot(rndEngine, 0.0, 1.0) > m_QuantumEff_PMT[id]*m_CollectionEff) return;
  double PheTime = CLHEP::RandGaussQ::shoot(rndEngine, GlobalTime + 5.* m_ConversionSpr, m_ConversionSpr);



  if ( Station >3 || Station < 0 || Detector >49 || Detector < 10 || (SensitiveElement-1)/2>1 || (SensitiveElement-1)/2<0) {      
    ATH_MSG_FATAL ( "Wrong station, detector or sensitive detector id" );
    return;
     }

// at the moment just calculate average time of collected photoelectrons
// in the future real signal will be formed and time will be reconstructed by the code commented below   


   m_Signal[Station][Detector][(SensitiveElement-1)/2].Fill(PheTime);

//  int iStart =  m_Signal[Station][Detector][(SensitiveElement-1)/2].FindBin(PheTime);

//  if (!(m_SignalVect.empty()))
//  {
//    for (unsigned long i = 0; i < (m_SignalVect.size()); i++) {
//        if( (int)i+iStart > m_Signal[Station][Detector][(SensitiveElement-1)/2].GetNbinsX()) break ;
//        m_Signal[Station][Detector][(SensitiveElement-1)/2].AddBinContent(i+iStart,m_SignalVect[i]*NumberOfSecondaries/m_Gain);     
//    }
//  }  
//  else ATH_MSG_DEBUG("m_SignalVector is empty"); 


return;

}

void AFP_PileUpTool::StoreTDDigi(void)
{


       TString Id1, Id2, Id3;

    for( int i=0; i<4; i++) {
       for( int j=0; j<49; j++) {
          for( int k=0; k<2; k++) { 
//                               double ADC = m_Signal[i][j][k].GetMaximum();
//                               double TDC =m_Signal[i][j][k].GetBinCenter( m_Signal[i][j][k].FindFirstBinAbove(ADC*m_CfdThr));
 double TDC = m_Signal[i][j][k].GetMean();
 double ADC = m_Signal[i][j][k].GetEntries();
                               if( ADC>0) {
//// Saturation
//    double curr=0;
//    for( int l =0; l<m_Signal[i][j][k].GetNbinsX(); l++ ) {
//      curr = m_Signal[i][j][k].GetBinContent(l);
//      if( curr > 100) curr=50.*log10(curr);
//      m_Signal[i][j][k].SetBinContent(l,curr); 
//    }
//    ADC = m_Signal[i][j][k].GetMaximum();
//    TDC = m_Signal[i][j][k].GetBinCenter( m_Signal[i][j][k].FindFirstBinAbove(ADC*m_CfdThr));
//// Constant fraction discriminator

//    int delay = ((m_RiseTime/4)/5.);
//    for(int  l= m_Signal[i][j][k].GetNbinsX(); l>0; l-- ) {
//      curr = m_Signal[i][j][k].GetBinContent(l);
//      if ( l > delay) { curr = m_Signal[i][j][k].GetBinContent(l-delay) - curr;}
//      else { curr = -1.;}
//      m_Signal[i][j][k].SetBinContent(l,curr);
//    }
//
// at the moment TDC is average time and ADC is number of pfotoelectrons. No CFD, saturation, ...
// in the future constant fraction disciminator and saturation effects will be added by the code
// commented above



 AFP_TDDigi* tddigi = new AFP_TDDigi ();
 tddigi->m_nStationID=i;
 tddigi->m_nDetectorID=j;
 tddigi->m_nSensitiveElementID=2*k+1;
 tddigi->m_fADC=ADC;
 tddigi->m_fTDC=TDC;

 m_digitCollection->Insert(*tddigi);                                              
                                          }
                                  }
                               }
                            }

return;
}


void AFP_PileUpTool::createSiDigi(int Station, int Detector, int PixelRow, int PixelCol, float PreStepX, float PreStepY, float PreStepZ, float PostStepX, float PostStepY, float PostStepZ, float DepEnergy)
{
  ATH_MSG_DEBUG ( " iterating Pmt, station " << Station << ", detector " << Detector << ", pixel_col " << PixelCol << ", pixel_row " << PixelRow << ", dep_energy" << DepEnergy << " (x,y,z)_pre (" << PreStepX <<"," << PreStepY <<"," << PreStepZ <<"), (x,y,z)_post (" << PostStepX <<"," << PostStepY <<"," << PostStepZ <<")" ); 

  float distance; // in mm
  distance = sqrt(pow(PostStepX-PreStepX,2)+pow(PostStepY-PreStepY,2)+pow(PostStepZ-PreStepZ,2));
  
  ATH_MSG_DEBUG ( "actual distance in pixel: " << distance); 
  
  distance = 1000.0* distance; // in mu 
  
  // we do not consider energy dependende
  // 1mu corresponds to ~ 110 e-hole pairs
  // this we should slighlty modify to some distribution
  
    ATH_MSG_DEBUG ( "deposted charge for given hit " << distance*110.0 ); 
  
  //m_deposited_charge[Station][Detector][PixelCol][PixelRow] += distance * 110.0; 
  //array[station_max=4][layers=6_max][PixelCol_max=336][PxelRow=80]
  
  if ((PixelCol>=336) || (PixelRow >= 80) || (Station >= 4) || (Detector >= 6 ) )
  {
     if (Detector == 11) 
     {
        ATH_MSG_DEBUG ( "Hit in the vacuum layer in front of the station " << Station );
	  
     }
     else
     {
        ATH_MSG_WARNING ( "WRONG NUMBER of PIXEL coordinates or station or detector !!!:" );
        ATH_MSG_WARNING ( "station [max 4] " << Station << ", detector [max 6]" << Detector << ", pixel_col [max 336] " << PixelCol << ", pixel_row [max 80] " << PixelRow );
     }
     return;
  }   
  
  m_deposited_charge[6*336*80*Station + 80*336*Detector + 80*PixelCol + PixelRow] += distance * 110.0;
  m_deposited_energy[6*336*80*Station + 80*336*Detector + 80*PixelCol + PixelRow] += DepEnergy;

return;

}

void AFP_PileUpTool::StoreSiDigi(void)
{
//ATH_MSG_DEBUG ( "bum 3" );


  long index = 0;
//  const long indexend = (long)width * (long)height * (long)depth;
  while ( index != 645120 ) // here just 6 layers per detector are considered
  {
          // here should  come noise
  
          if (m_deposited_charge[index] > 1000.)
	    {
	    
	    	  ATH_MSG_DEBUG ( " total # of pairs from length " << m_deposited_charge[index]);
	        ATH_MSG_DEBUG ( " total # of pairs from dep_energy " << 1000000.*m_deposited_energy[index]/3.6);
	    
	        int station  = (int)(index/(80*336*6)); 
		  int detector = (int)((index-station*80*336*6)/(80*336)); 
		  int column   = (int)((index-station*80*336*6-detector*80*336)/80);
		  int row      = (int)(index-station*80*336*6-detector*80*336-column*80);
		  
		  ATH_MSG_DEBUG ( " reversed mapping, station " << station << ", detector " << detector << ", pixel_col " << column << ", pixel_row " << row ); 

	    
		  AFP_SiDigi* sidigi = new AFP_SiDigi();	
		  
		  //ATH_MSG_DEBUG ( "bum 3a" );	  
              
		  sidigi->m_nStationID = station;
              sidigi->m_nDetectorID = detector;
              sidigi->m_nPixelCol = column;
		  sidigi->m_nPixelRow = row;
		  
		  //ATH_MSG_DEBUG ( "bum 3b" );
              sidigi->m_fADC = 1000000.*m_deposited_energy[index]/3.6;
              sidigi->m_fTDC = 0.;

              //ATH_MSG_DEBUG ( "bum 3c" );

              

		  
		  ATH_MSG_DEBUG ( " size: " << m_SiDigiCollection->size());
		  
		  //ATH_MSG_DEBUG ( "bum 3d" );
		  
		  m_SiDigiCollection->Insert(*sidigi); 
		  //m_SiDigiCollection->push_back(sidigi); 
		  
		  
		  
		  //ATH_MSG_DEBUG ( "bum 3e" );
		
          }
	    
          index++;	    
  } 

return;
}


double AFP_PileUpTool::SignalFun(double Time, double RiseTime, double FallTime)
   {
      double f=0;
      if ( Time < 0) return f;
      double p = (FallTime-RiseTime*TMath::Log(1.+FallTime/RiseTime))/TMath::Log(10.);
      f = TMath::Power(Time/p,RiseTime/p)*TMath::Exp(-(Time/p));
      f = f/(TMath::Power(RiseTime/p,RiseTime/p)*TMath::Exp(-(RiseTime/p)));
      return f;
   }
   
/*   
double AFP_PileUpTool::SiSignalFun(double Time, double RiseTime, double FallTime)
   {
      double f=0;

      if ( Time < 0) return f;
      double p = (FallTime-RiseTime*TMath::Log(1.+FallTime/RiseTime))/TMath::Log(10.);
      f = TMath::Power(Time/p,RiseTime/p)*TMath::Exp(-(Time/p));
      f = f/(TMath::Power(RiseTime/p,RiseTime/p)*TMath::Exp(-(RiseTime/p)));

      return f;
   }   
*/

StatusCode AFP_PileUpTool::recordContainers(ServiceHandle<StoreGateSvc>& evtStore, std::string key_digitCnt) 
{
  m_digitCollection = new AFP_TDDigiCollection();

  StatusCode sc = evtStore->record(m_digitCollection, key_digitCnt);
  return sc;
}

StatusCode AFP_PileUpTool::recordSiCollection(ServiceHandle<StoreGateSvc>& evtStore, std::string key_SidigitCnt) 
{
  m_SiDigiCollection = new AFP_SiDigiCollection();

  StatusCode sc = evtStore->record(m_SiDigiCollection, key_SidigitCnt);
  
  return sc;
  //return StatusCode::SUCCESS;
}
