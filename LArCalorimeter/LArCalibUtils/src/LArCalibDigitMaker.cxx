/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArCalibDigitMaker.h"

#include "GaudiKernel/ToolHandle.h"

#include "xAODEventInfo/EventInfo.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRawConditions/LArCalibParams.h"

#include <algorithm>
#include <sstream>


using CLHEP::ns;


LArCalibDigitMaker::LArCalibDigitMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_nTrigger(0)
{
  //declareProperty("DigitKey",m_key="");
 declareProperty("KeyList",m_keylist);
 declareProperty("DAC",m_vDAC);
 declareProperty("Delay",m_vDelay);
 declareProperty("Pattern",m_vPattern);
 declareProperty("nPattern",m_nPatterns);
 declareProperty("NTrigger",m_nTrigger);
 declareProperty("BoardIDs",m_vBoardIDs);
 declareProperty("DelayScale",m_delayScale=(25./240.)*ns);
 declareProperty("DontRun",m_dontRun=false); //Put only Board configuration in DetectorStore
 declareProperty("isSC",m_isSC=false);
 declareProperty("skipDuplicates",m_skipDuplicates=false);
}

LArCalibDigitMaker::~LArCalibDigitMaker() 
{//empty
}

StatusCode LArCalibDigitMaker::initialize()
{

  m_oldeventNb=-999;
  m_eventNb=-1;

  ATH_MSG_DEBUG ( "======== LArCalibDigitMaker Initialize ========" );
  // bool containsKeySC = false;
  for (unsigned int i = 0; i < m_keylist.size(); ++i) {
    if (m_keylist.at(i).compare("SC") == 0) {
       m_isSC = true;
       ATH_MSG_DEBUG ( "======== LArCalibDigitMaker isSC is True  ========" );
    }
  }
  if(m_isSC){
    ATH_CHECK( m_calibMapSCKey.initialize() );
  }else{
    ATH_CHECK( m_calibMapKey.initialize() );
  }
    
  //if we have calib board params as jobOpts, set them

  std::vector<std::vector<unsigned>> theseBoardIDs;
  std::vector<std::vector<unsigned>> thesePatterns;
  std::vector<std::vector<unsigned>> theseDACs;
  theseBoardIDs.reserve(m_vBoardIDs.size());
  thesePatterns.reserve(m_vPattern.size());
  theseDACs.reserve(m_vDAC.size());
  for (const auto& elem : m_vBoardIDs) {
    theseBoardIDs.emplace_back(elem.begin(), elem.end());
  }
  for (const auto& elem : m_vPattern) {
    thesePatterns.emplace_back(elem.begin(), elem.end());
  }
  for (const auto& elem : m_vDAC) {
    theseDACs.emplace_back(elem.begin(), elem.end());
  }
  std::vector<unsigned> cutPattern;

  auto calibParams = std::make_unique<LArCalibParams>();
  ATH_CHECK( calibParams->initialize() ); 
  for( long unsigned int i=0; i < theseBoardIDs.size(); i++ ){
    if (theseBoardIDs[i].size() && m_vDAC[i].size() && m_vDelay.size() && thesePatterns[i].size() && m_nTrigger) {
      cutPattern = thesePatterns[i];

      if (thesePatterns[i].size() != ((unsigned int)m_nPatterns[i])){
	ATH_MSG_WARNING("Going to change pattern length from "<<thesePatterns[i].size()<<" to "<<m_nPatterns[i]);
	cutPattern.resize(m_nPatterns[i]);
      }
      if (thesePatterns[i].size()%4) {
	ATH_MSG_ERROR ( "Problem with jobOptions! One Pattern must conists of 4 32bit values! Pattern "<< i );
	return StatusCode::FAILURE;
      }
      for (unsigned id : theseBoardIDs[i]) {
	const HWIdentifier calibBoardHWID(id);
	calibParams->set(calibBoardHWID,m_nTrigger,cutPattern,theseDACs[i],m_vDelay);
      }
    }
  }

  ATH_CHECK( detStore()->record(std::move(calibParams),"LArCalibParams") );
  // End set calib board parameters

  if (m_keylist.size()==0) {
    m_keylist.push_back("HIGH");
    m_keylist.push_back("MEDIUM");
    m_keylist.push_back("LOW");
  }

  ATH_MSG_DEBUG ( "======== LArCalibDigitMaker initialize successfully ========" );
  return StatusCode::SUCCESS;
}


StatusCode LArCalibDigitMaker::execute() {

 if (m_dontRun) return StatusCode::SUCCESS;

 const EventContext& ctx = Gaudi::Hive::currentContext();


 const LArCalibLineMapping *clcabling=0;
 if(m_isSC) {
   ATH_MSG_DEBUG ( "======== LArCalibDigitMaker: using SC calib line map" );
   SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_calibMapSCKey};
   clcabling=*clHdl;
 } else {
   SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_calibMapKey};
   clcabling=*clHdl;
 }
 

 if(!clcabling) {
    ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_calibMapKey.key() );
    return StatusCode::FAILURE;
 }

 const DataHandle<xAOD::EventInfo> thisEventInfo;
 ATH_CHECK( evtStore()->retrieve(thisEventInfo) );
 // Modif J. Labbe from JF. Marchand - Nov. 2009
 //  const unsigned eventNb=thisEventInfo->event_ID()->event_number();
 unsigned eventNbtmp=ctx.eventID().event_number();
 if((int)eventNbtmp!=m_oldeventNb){
   m_oldeventNb=eventNbtmp;
   m_eventNb++;
 }
 const unsigned eventNb=(int)m_eventNb;

 ATH_MSG_DEBUG ( "======== executing event "<< eventNb << " ========" );
 
 const LArCalibParams* calibParams = nullptr;
 ATH_CHECK( detStore()->retrieve(calibParams,"LArCalibParams") );

 for (const std::string& key : m_keylist) { //Loop over all containers that are to be processed (e.g. different gains)
   ATH_MSG_DEBUG ( "Retrieving LArDigitContainer. Key= " << key );
   const LArDigitContainer* larDigitCont;
   StatusCode sc = evtStore()->retrieve(larDigitCont,key);
   if (sc.isFailure())  {
     ATH_MSG_DEBUG ( "Cannot read LArDigitContainer from StoreGate! key=" << key );
     continue; //Try next container
   }
   if (larDigitCont->size()==0) {
     ATH_MSG_DEBUG ( "LArDigitContainer with key '" << key << "' is empty. Ignored." );
     continue; //Try next container
   }
   if (!m_pulsedChids.empty()) m_pulsedChids.clear();
   //Iterate over LArDigitContainer and build LArCalibDigitContainer
   LArCalibDigitContainer* calibDigitContainer=new LArCalibDigitContainer();
   calibDigitContainer->setDelayScale(m_delayScale);
   for (const LArDigit* digit : *larDigitCont) {
     HWIdentifier chid=digit->hardwareID();
     
     if(std::find(m_pulsedChids.begin(), m_pulsedChids.end(), chid) != m_pulsedChids.end()) {
       if(m_skipDuplicates) continue; // skip this channel if it was already pulsed
       m_pulsedChids.push_back(chid);
     }

     //Get data members of LArDigit
     const std::vector<short>& samples=digit->samples();
     CaloGain::CaloGain gain=digit->gain();
     const std::vector<HWIdentifier>& calibChannelIDs=clcabling->calibSlotLine(chid);
     if (calibChannelIDs.size()==0) {
       continue; //Disconnected channel
     }
     //For the time being, I assume we are in H8 and have only one calib channel per FEB channel
     std::vector<HWIdentifier>::const_iterator csl_it=calibChannelIDs.begin();
     //Now the CalibBoard settings:
     //Get data to build LArCalibDigit:
     unsigned ddac=calibParams->DAC(eventNb,*csl_it);
     if(ddac==kmaxDAC) {
        ATH_MSG_WARNING("SKIP "<<chid<<" "<<gain<<" "<<ddac<<"  event no "<<eventNb);
        continue;
     }
     uint16_t dac=ddac;
     uint16_t delay=calibParams->Delay(eventNb,*csl_it);
     bool ispulsed=false;
     for(; csl_it != calibChannelIDs.end(); ++csl_it) {
       if(calibParams->isPulsed(eventNb,*csl_it)){
	 ispulsed=true;
	 break;
       }
     }
     //build LArCalibDigit:
     if ( ispulsed ){
       ATH_MSG_DEBUG("HERE!! "<<chid<<" "<<gain<<" "<<dac<<" "<<delay<<" "<<ispulsed<<" event no "<<eventNb<<" "<<calibChannelIDs.size()<<" calib lines, first is "<<*csl_it);

     }
     LArCalibDigit* calibDigit=new LArCalibDigit(chid,gain, samples, dac, delay, ispulsed);
     calibDigitContainer->push_back(calibDigit);
   } //End iteration to build calibDigits
   ATH_MSG_DEBUG("Trying to store with key "<<key);
   ATH_CHECK( evtStore()->record(calibDigitContainer,key) );
   ATH_MSG_DEBUG ("LArCalibDigitContainer recorded to StoreGate. key=" << key );
 } //End loop key list
 return StatusCode::SUCCESS;
}

