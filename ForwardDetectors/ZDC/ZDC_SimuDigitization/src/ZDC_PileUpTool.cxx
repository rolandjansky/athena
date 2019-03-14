/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "ZDC_SimuDigitization/ZDC_PileUpTool.h"
#include <algorithm>
#include "ZDC_SimEvent/ZDC_SimStripHit_Collection.h"
#include "ZDC_SimEvent/ZDC_SimPixelHit_Collection.h"
#include "ZDC_SimEvent/ZDC_SimStripHit.h"
#include "ZDC_SimEvent/ZDC_SimPixelHit.h"
#include "ZdcEvent/ZdcDigitsCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "Identifier/Identifier.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <map>
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Random/RandPoissonQ.h"

ZDC_PileUpTool::ZDC_PileUpTool(const std::string& type,
			       const std::string& name,
			       const IInterface* parent) :
  PileUpToolBase(type, name, parent),
  m_mergeSvc    ("PileUpMergeSvc", name), 
  m_atRndmGenSvc("AtRndmGenSvc", name),
  m_rndEngine(0),
  m_ScaleStrip(3e-4),
  m_ScalePixel(5e-3),
  m_Pedestal(100),
  m_MaxTimeBin(5),
  m_SigmaNoiseHG_Strip(1.5),
  m_SigmaNoiseHG_Pixel(1.0),
  m_SigmaNoiseLG_Strip(1.0),
  m_SigmaNoiseLG_Pixel(1.0),
  m_DumpStrip(false),
  m_DumpPixel(false)
{
 
  // NOTE: The following variables are actually re-initialized by ZDC_DigiTop::initialize() or ZDC_PileUpTool::initialize()
  m_GainRatio_Strip[0] = 10.0; m_GainRatioError_Strip[0] = 0.5;
  m_GainRatio_Strip[1] = 10.0; m_GainRatioError_Strip[1] = 0.5;
  m_GainRatio_Strip[2] = 10.0; m_GainRatioError_Strip[2] = 0.5;
  m_GainRatio_Strip[3] = 10.0; m_GainRatioError_Strip[3] = 0.5;
  m_GainRatio_Strip[4] = 10.0; m_GainRatioError_Strip[4] = 0.5;
  m_GainRatio_Strip[5] = 10.0; m_GainRatioError_Strip[5] = 0.5;
  m_GainRatio_Strip[6] = 10.0; m_GainRatioError_Strip[6] = 0.5;
  m_GainRatio_Strip[7] = 10.0; m_GainRatioError_Strip[7] = 0.5;
  m_GainRatio_Pixel      = 10.0;
  m_GainRatioError_Pixel = 0.5;

  m_digitContainer = 0; // initializing to null pointer

  // end of ToolBox variables

  m_SimStripHitCollectionName = "ZDC_SimStripHit_Collection";
  m_SimPixelHitCollectionName = "ZDC_SimPixelHit_Collection";
  m_ZdcDigitsContainerName    = "ZdcDigitsCollection";

  m_mergedStripHitList = 0; // initialized to null pointer
  m_mergedPixelHitList = 0;

  declareProperty("SimStripHitCollection" , m_SimStripHitCollectionName, "Name of the input Collection of the simulated Strip Hits");
  declareProperty("SimPixelHitCollection" , m_SimPixelHitCollectionName, "Name of the input Collection of the simulated Pixel Hits");
  declareProperty("ZdcDigitsContainerName", m_ZdcDigitsContainerName,    "Name of the Container to hold the output from the ZDC digitization");
  declareProperty("RndmSvc",                m_atRndmGenSvc,              "Random Number Service used in ZDC digitization" );
  declareProperty("mergeSvc",               m_mergeSvc,                  "Store to hold the pile-ups");
 
  declareProperty("ScaleStrip", m_ScaleStrip);
  declareProperty("ScalePixel", m_ScalePixel);
  declareProperty("MaxTimeBin", m_MaxTimeBin);
  declareProperty("Pedestal"  , m_Pedestal);

  declareProperty("SigmaNoiseHG_Strip",  m_SigmaNoiseHG_Strip);
  declareProperty("SigmaNoiseHG_Pixel",  m_SigmaNoiseHG_Pixel);
  declareProperty("SigmaNoiseLG_Strip",  m_SigmaNoiseLG_Strip);
  declareProperty("SigmaNoiseLG_Pixel",  m_SigmaNoiseLG_Pixel);
}

StatusCode ZDC_PileUpTool::initialize() {

  ATH_MSG_DEBUG ( "ZDC_PileUpTool::initialize() called" );
  
  ATH_MSG_INFO ( " ScaleStrip: " << m_ScaleStrip << endmsg
		 << " ScalePixel: " << m_ScalePixel << endmsg
		 << " MaxTimeBin: " << m_MaxTimeBin << endmsg
		 << " Pedestal  : " << m_Pedestal   );
  
  if (m_atRndmGenSvc.retrieve().isFailure()) { 
    ATH_MSG_FATAL ( "Could not retrieve RandomNumber Service!" ); 
    return StatusCode::FAILURE; 
  }
  else { 
    ATH_MSG_DEBUG ( "Retrieved RandomNumber Service" ); 
  }

  m_mergedStripHitList = new ZDC_SimStripHit_Collection("mergedStripHitList");
  m_mergedPixelHitList = new ZDC_SimPixelHit_Collection("mergedPixelHitList");
  
  return StatusCode::SUCCESS;
}

StatusCode ZDC_PileUpTool::processAllSubEvents() {

  ATH_MSG_DEBUG ( "ZDC_PileUpTool::processAllSubEvents()" );

  //retrieve the PileUpMergeSvc if necessary
  if(!m_mergeSvc) {
    if(!m_mergeSvc.retrieve().isSuccess() || !m_mergeSvc) {
      ATH_MSG_FATAL("digitize: Could not find PileUpMergeSvc");
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("digitize: retrieved PileUpMergeSvc");
  }
  else ATH_MSG_DEBUG("digitize: PileUpMergeSvc already available"); 

  typedef PileUpMergeSvc::TimedList<ZDC_SimStripHit_Collection>::type TimedStripHitCollList;
  typedef PileUpMergeSvc::TimedList<ZDC_SimPixelHit_Collection>::type TimedPixelHitCollList;
  
  TimedStripHitCollList StripHitCollList;
  TimedPixelHitCollList PixelHitCollList;
  
  if (m_mergeSvc->retrieveSubEvtsData(m_SimStripHitCollectionName, StripHitCollList).isFailure()) { 

    ATH_MSG_FATAL ( "Could not fill TimedStripHitCollList" ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( "Retrieved TimedStripHitCollList" ); }
  
  ATH_MSG_DEBUG ( " PileUp: Merge " << StripHitCollList.size() << " ZDC_SimStripHit_Collection with key " << m_SimStripHitCollectionName );
  
  if (m_mergeSvc->retrieveSubEvtsData(m_SimPixelHitCollectionName, PixelHitCollList).isFailure()) { 

    ATH_MSG_FATAL ( "Could not fill TimedPixelHitCollList" ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( "Retrieved TimedPixelHitCollList" ); }
  
  ATH_MSG_DEBUG ( " PileUp: Merge " << PixelHitCollList.size() << " ZDC_SimPixelHit_Collection with key " << m_SimPixelHitCollectionName );
  
  TimedHitCollection<ZDC_SimStripHit> thpczdcstrip;

  TimedStripHitCollList::iterator iStripColl  (StripHitCollList.begin());
  TimedStripHitCollList::iterator endStripColl(StripHitCollList.end());
  
  while (iStripColl != endStripColl) {
    
    const ZDC_SimStripHit_Collection* tmpColl(iStripColl->second);
    
    thpczdcstrip.insert(iStripColl->first, tmpColl);
    
    ATH_MSG_DEBUG ( " ZDC_SimStripHit_Collection found with " << tmpColl->size() << " hits " << iStripColl->first );
    
    ++iStripColl;
  }
  
  TimedHitCollection<ZDC_SimPixelHit> thpczdcpixel;

  TimedPixelHitCollList::iterator iPixelColl  (PixelHitCollList.begin());
  TimedPixelHitCollList::iterator endPixelColl(PixelHitCollList.end());
  
  while (iPixelColl != endPixelColl) {
    
    const ZDC_SimPixelHit_Collection* tmpColl(iPixelColl->second);
    
    thpczdcpixel.insert(iPixelColl->first, tmpColl);
    
    ATH_MSG_DEBUG ( " ZDC_SimPixelHit_Collection found with " << tmpColl->size() << " hits " << iPixelColl->first );
    
    ++iPixelColl;
  }

  m_rndEngine = m_atRndmGenSvc->GetEngine("ZDCRndEng");

  if (recordContainers(this->evtStore(), m_ZdcDigitsContainerName).isFailure()) { 
    
    ATH_MSG_FATAL ( " ZDC DigiTop :: Could not record the empty digit container in StoreGate " ); return StatusCode::FAILURE; 
  }
  else { ATH_MSG_DEBUG ( " ZDC DigiTop :: Digit container is recorded in StoreGate " ); }
  
  fillStripDigitContainer(thpczdcstrip, m_rndEngine);
  fillPixelDigitContainer(thpczdcpixel, m_rndEngine);
  
  return StatusCode::SUCCESS;
}
StatusCode ZDC_PileUpTool::prepareEvent(const unsigned int nInputEvents){

  ATH_MSG_DEBUG ( "ZDC_PileUpTool::prepareEvent() called for " << nInputEvents << " input events" );
 
  StatusCode sc = recordContainers(this->evtStore(), m_ZdcDigitsContainerName);
 
  if (sc.isFailure()) { ATH_MSG_FATAL ( " Could not record the empty digit container in StoreGate " ); return sc; }
  else                { ATH_MSG_DEBUG ( " Digit container is recorded in StoreGate " ); }
  
  m_mergedStripHitList->clear();
  m_mergedPixelHitList->clear();
  
  return StatusCode::SUCCESS;
}

StatusCode ZDC_PileUpTool::processBunchXing(int bunchXing,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents) {
  ATH_MSG_DEBUG ( "ZDC_PileUpTool::processBunchXing() " << bunchXing );
  SubEventIterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; iEvt++) {
    StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
    //PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index());
    ATH_MSG_VERBOSE("SubEvt StoreGate " << seStore.name() << " :"
                    << " bunch crossing : " << bunchXing
                    << " time offset : " << iEvt->time()
                    << " event number : " << iEvt->ptr()->eventNumber()
                    << " run number : " << iEvt->ptr()->runNumber()
                    );

    const ZDC_SimStripHit_Collection* tmpCollStrip = 0;
   
    if (!seStore.retrieve(tmpCollStrip, m_SimStripHitCollectionName).isSuccess()) {
      
      ATH_MSG_ERROR ( "SubEvent ZDC_SimStripHit_Collection not found in StoreGate " << seStore.name() );
      
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG ( "ZDC_SimStripHit_Collection found with " << tmpCollStrip->size() << " hits" );
    
    ZDC_SimStripHit_Collection::const_iterator iStrip = tmpCollStrip->begin();
    ZDC_SimStripHit_Collection::const_iterator eStrip = tmpCollStrip->end();
   
    for (; iStrip!=eStrip; ++iStrip) m_mergedStripHitList->push_back((*iStrip));
    
    const ZDC_SimPixelHit_Collection* tmpCollPixel = 0;
   
    if (!seStore.retrieve(tmpCollPixel, m_SimPixelHitCollectionName).isSuccess()) {
      
      ATH_MSG_ERROR ( "SubEvent ZDC_SimPixelHit_Collection not found in StoreGate " << seStore.name() );
      
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG ( "ZDC_SimPixelHit_Collection found with " << tmpCollPixel->size() << " hits" );
    
    ZDC_SimPixelHit_Collection::const_iterator iPixel = tmpCollPixel->begin();
    ZDC_SimPixelHit_Collection::const_iterator ePixel = tmpCollPixel->end();
   
    for (; iPixel!=ePixel; ++iPixel) m_mergedPixelHitList->push_back((*iPixel));
    
    m_rndEngine = m_atRndmGenSvc->GetEngine("ZDCRndEng");
  }
  
  return StatusCode::SUCCESS;
}

StatusCode ZDC_PileUpTool::mergeEvent(){
 
  fillStripDigitContainer(m_mergedStripHitList, m_rndEngine);
  fillPixelDigitContainer(m_mergedPixelHitList, m_rndEngine);
  
  return StatusCode::SUCCESS;
}

StatusCode ZDC_PileUpTool::finalize() { return StatusCode::SUCCESS; }
 
void ZDC_PileUpTool::fillStripDigitContainer(TimedHitCollection<ZDC_SimStripHit>& thpczdc, CLHEP::HepRandomEngine* rndEngine) 
{
  TimedHitCollection<ZDC_SimStripHit>                 thpc = thpczdc;
  TimedHitCollection<ZDC_SimStripHit>::const_iterator i, e, it;
  
  while (thpc.nextDetectorElement(i, e)) for (it = i; it != e; it++) {
    
    int Side     = (*it)->GetSide();
    int ModuleNo = (*it)->GetMod();
    int NPhotons = (*it)->GetNPhotons();
    
    createAndStoreStripDigit(Side, ModuleNo, NPhotons, rndEngine);
  }
}

void ZDC_PileUpTool::fillStripDigitContainer(const ZDC_SimStripHit_Collection* ZDC_SimStripHit_Collection, CLHEP::HepRandomEngine* rndEngine)
{
  ZDC_SimStripHit_ConstIterator it    = ZDC_SimStripHit_Collection->begin();
  ZDC_SimStripHit_ConstIterator itend = ZDC_SimStripHit_Collection->end();
  
  for (; it != itend; it++) {

    int Side     = it->GetSide();
    int ModuleNo = it->GetMod();
    int NPhotons = it->GetNPhotons();

    createAndStoreStripDigit(Side, ModuleNo, NPhotons, rndEngine);
  }
}

void ZDC_PileUpTool::createAndStoreStripDigit(int Side, int ModuleNo, int NPhotons, CLHEP::HepRandomEngine* rndEngine)
{
  ATH_MSG_DEBUG ( " iterating Strips " << Side << "  " << ModuleNo << "  " << NPhotons ); 
  
  int StripNum = Side*4 + ModuleNo; // In my convention side=0 is C-side and side=1 is A-Side
  
  Identifier::value_type X=0;
  
  if      (StripNum == 0) X=0xec000000;
  else if (StripNum == 1) X=0xec200000;
  else if (StripNum == 2) X=0xec400000;
  else if (StripNum == 3) X=0xec600000;
  else if (StripNum == 4) X=0xed000000;
  else if (StripNum == 5) X=0xed200000;
  else if (StripNum == 6) X=0xed400000;
  else if (StripNum == 7) X=0xed600000;
  else { ATH_MSG_DEBUG ( " Unknown Side or Module " <<Side << " " << ModuleNo ); return; }
  
  Identifier Id(X);
  
  std::vector<int>    gain0_delay0, gain1_delay0, gain0_delay1, gain1_delay1;
  std::vector<double> V_Temp1, V_Temp2;
  
  gain0_delay0.resize(m_MaxTimeBin);
  gain1_delay0.resize(m_MaxTimeBin);
  gain0_delay1.resize(m_MaxTimeBin);
  gain1_delay1.resize(m_MaxTimeBin);

  V_Temp1.resize(m_MaxTimeBin);
  V_Temp2.resize(m_MaxTimeBin);
  
  int TimeFinal = m_MaxTimeBin*25;
  
  for (int Time=21; Time<TimeFinal; Time++) {
    
    int TimeBin = Time/25;    
    
    float Signal1 = NPhotons*0.47*pow((Time-21)/10.0, 3.4)*exp(-(Time-21)/10.0);
    float Signal2 = NPhotons*0.47*pow((Time-20)/10.0, 3.4)*exp(-(Time-20)/10.0);
    
    float Signal  = (Signal1+Signal2)/2.0;  
    
    V_Temp1[TimeBin] += Signal; // the step is 1ns so I don't have to multiply by dT 
    
    Signal1 = NPhotons*0.47*pow((Time + 12.5 - 21)/10.0, 3.4)*exp(-(Time+12.5-21)/10.0); //delayed by 12.5 ns
    Signal2 = NPhotons*0.47*pow((Time + 12.5 - 20)/10.0, 3.4)*exp(-(Time+12.5-20)/10.0); //delayed by 12.5 ns
    
    Signal  = (Signal1+Signal2)/2.0;  
    
    V_Temp2[TimeBin] += Signal;
  }
  
  for (int I=0; I<m_MaxTimeBin; I++) {
    
    V_Temp1[I] = V_Temp1[I]*m_ScaleStrip;
    V_Temp2[I] = V_Temp2[I]*m_ScaleStrip;
    
    gain1_delay0[I] = CLHEP::RandGaussQ::shoot(rndEngine, V_Temp1[I] + m_Pedestal, m_SigmaNoiseHG_Strip);
    gain1_delay1[I] = CLHEP::RandGaussQ::shoot(rndEngine, V_Temp2[I] + m_Pedestal, m_SigmaNoiseHG_Strip);
    
    V_Temp1[I] = HighToLow(V_Temp1[I], m_GainRatio_Strip[StripNum], m_GainRatioError_Strip[StripNum], rndEngine);
    V_Temp2[I] = HighToLow(V_Temp2[I], m_GainRatio_Strip[StripNum], m_GainRatioError_Strip[StripNum], rndEngine);
    
    gain0_delay0[I] = CLHEP::RandGaussQ::shoot(rndEngine, V_Temp1[I] + m_Pedestal, m_SigmaNoiseLG_Strip);
    gain0_delay1[I] = CLHEP::RandGaussQ::shoot(rndEngine, V_Temp2[I] + m_Pedestal, m_SigmaNoiseLG_Strip);
    
    if (gain0_delay0[I] > 1023) gain0_delay0[I] = 1023; 
    if (gain0_delay1[I] > 1023) gain0_delay1[I] = 1023; 
    if (gain1_delay0[I] > 1023) gain1_delay0[I] = 1023; 
    if (gain1_delay1[I] > 1023) gain1_delay1[I] = 1023; 
  } 
  
  ZdcDigits* ZDC_Strip_Digit = new ZdcDigits(Id);
  
  ZDC_Strip_Digit->set_digits_gain1_delay0(gain1_delay0);
  ZDC_Strip_Digit->set_digits_gain1_delay1(gain1_delay1);
  ZDC_Strip_Digit->set_digits_gain0_delay0(gain0_delay0);
  ZDC_Strip_Digit->set_digits_gain0_delay1(gain0_delay1);
  
  m_digitContainer->push_back(ZDC_Strip_Digit);
  
  if (m_DumpStrip == 1) {
    
    m_MyFile << Side << "  " << ModuleNo << "  " << NPhotons;
    
    for (int I=0; I<m_MaxTimeBin; I++) { m_MyFile << "  " << gain1_delay0[I]; }
    
    m_MyFile << std::endl;  
  }
}

double ZDC_PileUpTool::HighToLow(double signal, double gain_ratio ,double gain_error, CLHEP::HepRandomEngine* rndEngine)
{
  gain_ratio = CLHEP::RandGaussQ::shoot(rndEngine, gain_ratio, gain_error);
  
  if (gain_ratio <= 0) { ATH_MSG_WARNING ( ":: ?? gain=0 !!!!!" ); return 10.0; }
  
  ATH_MSG_DEBUG ( "RANDOM CHECK ::" << gain_ratio ) ;
  
  return signal/gain_ratio;
}

void ZDC_PileUpTool::fillPixelDigitContainer(TimedHitCollection<ZDC_SimPixelHit>& thpczdc, CLHEP::HepRandomEngine* rndEngine) 
{
  TimedHitCollection<ZDC_SimPixelHit>                 thpc = thpczdc;
  TimedHitCollection<ZDC_SimPixelHit>::const_iterator i, e, it;
  
  while (thpc.nextDetectorElement(i, e)) for (it = i; it != e; it++) {
    
    int Side     = (*it)->GetSide();
    int ModuleNo = (*it)->GetMod() ;
    int PixNum   = (*it)->GetPix() ;
    int NPhotons = (*it)->GetNPhotons();
    
    createAndStorePixelDigit(Side, ModuleNo, PixNum, NPhotons, rndEngine);
  }
}

void ZDC_PileUpTool::fillPixelDigitContainer(const ZDC_SimPixelHit_Collection* ZDC_SimPixelHit_Collection, CLHEP::HepRandomEngine* rndEngine)
{
  ZDC_SimPixelHit_ConstIterator it    = ZDC_SimPixelHit_Collection->begin();
  ZDC_SimPixelHit_ConstIterator itend = ZDC_SimPixelHit_Collection->end();

  for (; it != itend; it++) {

    int Side     = it->GetSide();
    int ModuleNo = it->GetMod() ;
    int PixNum   = it->GetPix() ;
    int NPhotons = it->GetNPhotons();

    createAndStorePixelDigit(Side, ModuleNo, PixNum, NPhotons, rndEngine);
  }
}

void ZDC_PileUpTool::createAndStorePixelDigit(int Side, int ModuleNo, int PixNum, int NPhotons, CLHEP::HepRandomEngine* rndEngine)
{
  ATH_MSG_DEBUG ( " iterating Pixel " << Side << "  " << ModuleNo << "  " << PixNum << "   " << NPhotons ); 
  
  Identifier::value_type X = PixelID(Side, ModuleNo, PixNum);
  
  Identifier Id(X);
  
  std::vector<int>    gain0_delay0, gain1_delay0;
  std::vector<double> V_Temp1;
  
  gain0_delay0.resize(m_MaxTimeBin);
  gain1_delay0.resize(m_MaxTimeBin);
  
  V_Temp1.resize(m_MaxTimeBin);
  
  int TimeFinal = m_MaxTimeBin*25;
  
  for(int Time=21; Time<TimeFinal; Time++) {
    
    int TimeBin = Time/25;    
    
    float Signal1 = NPhotons*0.47*pow((Time - 21)/10.0, 3.4)*exp(-(Time-21)/10.0);
    float Signal2 = NPhotons*0.47*pow((Time - 20)/10.0, 3.4)*exp(-(Time-20)/10.0);
    
    float Signal  = (Signal1+Signal2)/2.0;  
    
    V_Temp1[TimeBin] += Signal; // the step is 1ns so I don't have to multiply by dT 
  }
  
  for (int I=0; I<m_MaxTimeBin; I++) {
    
    V_Temp1[I] = V_Temp1[I]*m_ScalePixel;
    
    gain1_delay0[I] = CLHEP::RandGaussQ::shoot(rndEngine, V_Temp1[I] + m_Pedestal, m_SigmaNoiseHG_Pixel);
    
    V_Temp1[I] = HighToLow(V_Temp1[I], m_GainRatio_Pixel, m_GainRatioError_Pixel, rndEngine);
    
    gain0_delay0[I] = CLHEP::RandGaussQ::shoot(rndEngine, V_Temp1[I] + m_Pedestal, m_SigmaNoiseLG_Pixel);
    
    if (gain0_delay0[I] > 1023) gain0_delay0[I] = 1023; 
    if (gain1_delay0[I] > 1023) gain1_delay0[I] = 1023; 
  } 
  
  ZdcDigits* ZDC_Pixel_Digit = new ZdcDigits(Id);
  
  ZDC_Pixel_Digit->set_digits_gain1_delay0(gain1_delay0);
  ZDC_Pixel_Digit->set_digits_gain0_delay0(gain0_delay0);
  
  m_digitContainer->push_back(ZDC_Pixel_Digit);
  
  if (m_DumpPixel == 1) {
    
    m_MyFile << Side << "  " << ModuleNo << "  " << PixNum << "   " << NPhotons;
    
    for (int I=0; I<m_MaxTimeBin; I++) { m_MyFile << "  " << gain1_delay0[I]; }
    
    m_MyFile << std::endl;  
  }
}

unsigned int ZDC_PileUpTool::PixelID(int Side, int Module, int PixNo)
{
  if( Module==1) return (0Xed240000 + PixNo*0X800 + (Side-1)*0X1000000); //HM-XY Modules (Side=0=>C-side , Side=1=>A-Side) 
  else           return (0Xec040000 + PixNo*0X800                 ); //EM-XY Module  (Only C-Side)
}

void ZDC_PileUpTool::SetDumps(bool Flag1, bool Flag2)
{
  m_DumpStrip = Flag1;
  m_DumpPixel = Flag2;
  
  if (m_DumpStrip || m_DumpPixel) { m_MyFile.open("DumpAll.txt"); }
}

StatusCode ZDC_PileUpTool::recordContainers(ServiceHandle<StoreGateSvc>& evtStore, std::string key_digitCnt) 
{
  m_digitContainer = new ZdcDigitsCollection();

  StatusCode sc = evtStore->record(m_digitContainer, key_digitCnt);

  return sc;
}
