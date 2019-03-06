/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
 declareProperty("NTrigger",m_nTrigger);
 declareProperty("BoardIDs",m_vBoardIDs);
 declareProperty("DelayScale",m_delayScale=(25./240.)*ns);
 declareProperty("DontRun",m_dontRun=false); //Put only Board configuration in DetectorStore
}

LArCalibDigitMaker::~LArCalibDigitMaker() 
{//empty
}

StatusCode LArCalibDigitMaker::initialize()
{
  ATH_MSG_DEBUG ( "======== LArCalibDigitMaker Initialize ========" );
  ATH_CHECK( m_calibMapKey.initialize() );
    
//  std::cout << "Pattern.size()=" << m_vPattern.size() << std::endl;
//   std::cout << "DAC.size()=" << m_vDAC.size() << std::endl;
//   std::cout << "Delay.size()=" << m_vDelay.size() << std::endl;
//   std::cout << "nTrigger=" << m_nTrigger << std::endl;
//   std::cout << "BoardIDs.size()=" << m_vBoardIDs.size() << std::endl;

  //Check if calibParams are given consistently: Either all or non
  if (!((m_vBoardIDs.size()==0 && m_vDAC.size()==0 && m_vDelay.size()==0 && m_vPattern.size()==0 && m_nTrigger==0) ||
      (m_vBoardIDs.size() && m_vDAC.size() && m_vDelay.size() && m_vPattern.size() && m_nTrigger))) {
    ATH_MSG_ERROR ( "Problem with jobOptions! Please set either ALL calibration parameters" << std:: endl 
                    << "(DAC, Delay, Pattern, BoardIDs and nTigger) or non!" );
    return StatusCode::FAILURE;
  }
  //if we have calib board params as jobOpts, set them
  if (m_vBoardIDs.size() && m_vDAC.size() && m_vDelay.size() && m_vPattern.size() && m_nTrigger) {
    if (m_vPattern.size()%4) {
      ATH_MSG_ERROR ( "Problem with jobOptions! One Pattern must conists of 4 32bit values!" );
      return StatusCode::FAILURE;
    }
    auto calibParams = std::make_unique<LArCalibParams>();
    ATH_CHECK( calibParams->initialize() ); 

    //void LArCalibParams::set(const HWIdentifier CalibModuleID, const unsigned nTrigger,
    //		 const  std::vector<unsigned>& Pattern, const std::vector<unsigned>& DAC, const std::vector<unsigned>& Delay)
    for (std::vector<unsigned>::const_iterator it=m_vBoardIDs.begin();it!=m_vBoardIDs.end();it++) {
      const HWIdentifier calibBoardHWID(*it);
      calibParams->set(calibBoardHWID,m_nTrigger,m_vPattern,m_vDAC,m_vDelay);
    }
    ATH_CHECK( detStore()->record(std::move(calibParams),"LArCalibParams") );
  }//End set calib board parameters

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

 SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_calibMapKey};
 const LArCalibLineMapping* clcabling{*clHdl};
 if(!clcabling) {
    ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_calibMapKey.key() );
    return StatusCode::FAILURE;
 }

 const DataHandle<xAOD::EventInfo> thisEventInfo;
 ATH_CHECK( evtStore()->retrieve(thisEventInfo) );
 // Modif J. Labbe from JF. Marchand - Nov. 2009
 //  const unsigned eventNb=thisEventInfo->event_ID()->event_number();
 const unsigned eventNb=(ctx.eventID().event_number())&0xffffff ;
 ATH_MSG_DEBUG ( "======== executing event "<< eventNb << " ========" );
 
 const LArCalibParams* calibParams = nullptr;
 ATH_CHECK( detStore()->retrieve(calibParams,"LArCalibParams") );

 std::vector<std::string>::const_iterator key_it=m_keylist.begin();
 std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
 for (;key_it!=key_it_e;key_it++) { //Loop over all containers that are to be processed (e.g. different gains)
   ATH_MSG_DEBUG ( "Retrieving LArDigitContainer. Key= " << (*key_it) );
   const LArDigitContainer* larDigitCont;
   StatusCode sc = evtStore()->retrieve(larDigitCont,*key_it);
   if (sc.isFailure())  {
     ATH_MSG_DEBUG ( "Cannot read LArDigitContainer from StoreGate! key=" << *key_it );
     continue; //Try next container
   }
   if (larDigitCont->size()==0) {
     ATH_MSG_DEBUG ( "LArDigitContainer with key '" << *key_it << "' is empty. Ignored." );
     continue; //Try next container
   }
   //Iterate over LArDigitContainer and build LArCalibDigitContainer
   LArCalibDigitContainer* calibDigitContainer=new LArCalibDigitContainer();
   calibDigitContainer->setDelayScale(m_delayScale);
   LArDigitContainer::const_iterator it=larDigitCont->begin();
   LArDigitContainer::const_iterator it_end=larDigitCont->end();
   for (;it!=it_end;it++) {
     HWIdentifier chid=(*it)->hardwareID();
     //Get data members of LArDigit
     const std::vector<short>& samples=(*it)->samples();
     CaloGain::CaloGain gain=(*it)->gain();
     const std::vector<HWIdentifier>& calibChannelIDs=clcabling->calibSlotLine(chid);
     if (calibChannelIDs.size()==0) 
       continue; //Disconnected channel
     //For the time beeing, I assume we are in H8 and have only one calib channel per FEB channel
     std::vector<HWIdentifier>::const_iterator csl_it=calibChannelIDs.begin();
     //Now the CalibBoard settings:
     //Get data to build LArCalibDigit:
     uint16_t dac=calibParams->DAC(eventNb,*csl_it);
     uint16_t delay=calibParams->Delay(eventNb,*csl_it);
     bool ispulsed=calibParams->isPulsed(eventNb,*csl_it);
     //build LArCalibDigit:
     // std::cout << "Event:" << eventNb 
     // 	     << "Building a LArCalibDigit with DAC=" << dac << ", Delay=" << delay << ", isPulsed=" << ispulsed << std::endl;
     LArCalibDigit* calibDigit=new LArCalibDigit(chid,gain, samples, dac, delay, ispulsed);
     calibDigitContainer->push_back(calibDigit);
   } //End iteration to build calibDigits
   ATH_CHECK( evtStore()->record(calibDigitContainer,*key_it) );
   //log << MSG::DEBUG << "LArCalibDigitContainer recorded to StoreGate. key=" << m_key << endmsg;
 } //End loop key list
 return StatusCode::SUCCESS;
}

