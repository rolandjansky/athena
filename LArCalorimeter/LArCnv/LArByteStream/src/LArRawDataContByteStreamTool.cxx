/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArRawDataContByteStreamTool.h"
#include "LArByteStream/LArRodEncoder.h"
#include "LArByteStream/LArRodDecoder.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArCalibDigitContainer.h"

#include "LArByteStream/LArRodBlockStructure.h"
//#include "LArByteStream/LArRodBlockStructure_0.h"
#include "LArByteStream/LArRodBlockTransparentV0.h"
#include "LArByteStream/LArRodBlockCalibrationV0.h"
#include "LArByteStream/LArRodBlockPhysicsV0.h"
#include "LArByteStream/LArRodBlockPhysicsV3.h"
#include "LArByteStream/LArRodBlockPhysicsV5.h"
#include "LArByteStream/LArRodBlockPhysicsV6.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloInterface/ICaloNoiseTool.h"

#include "StoreGate/StoreGate.h"

// STL stuff
#include <map> 


static const InterfaceID IID_ILArRawDataContByteStreamTool
            ("LArRawDataContByteStreamTool", 1, 0);


const InterfaceID& LArRawDataContByteStreamTool::interfaceID( )
{ return IID_ILArRawDataContByteStreamTool; }

// default contructor
 
LArRawDataContByteStreamTool::LArRawDataContByteStreamTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AlgTool(type,name,parent),
     m_noisetool("CaloNoiseTool/CaloNoiseToolDefault")
{ declareInterface< LArRawDataContByteStreamTool  >( this );
  declareProperty("DSPRunMode",m_DSPRunMode=4);
  declareProperty("RodBlockVersion",m_RodBlockVersion=0);
  declareProperty("FebNoiseCut",m_nfebsigma=2);
  declareProperty("InitializeForWriting",m_initializeForWriting=false);
  declareProperty("NoiseTool",m_noisetool);
  declareProperty("SubDetectorId",m_subDetId=0);
  declareProperty("IncludeDigits",m_includeDigits=false);
  declareProperty("DigitsContainer",m_DigitContName="LArDigitContainer_MC_Thinned");
  m_RodBlockStructure=NULL;
  m_log=NULL;
  m_lastEvent = 0xFFFFFFFF;
}

LArRawDataContByteStreamTool::~LArRawDataContByteStreamTool() {
  delete m_log;
}

 
StatusCode
LArRawDataContByteStreamTool::initialize()
{StatusCode sc=AlgTool::initialize();
 m_log = new MsgStream(msgSvc(),name());
 if (sc!=StatusCode::SUCCESS)
   return sc;
 (*m_log) << MSG::DEBUG << "Initializing LArRawDataContByteStream" << endreq;
 
 IToolSvc* toolSvc;
 if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)){
    (*m_log) << MSG::ERROR << " Can't get ToolSvc " << endreq;
    return StatusCode::FAILURE;
  }
 sc=toolSvc->retrieveTool("LArRodDecoder",m_decoder);
 if (sc!=StatusCode::SUCCESS)
   return sc;
 sc=service("StoreGateSvc", m_storeGate); 
 if (sc!=StatusCode::SUCCESS)
   return sc;

 if (m_initializeForWriting) {
   sc = m_hid2re.initialize() ;
   if(sc.isFailure()) return sc;

   sc=m_noisetool.retrieve();
   if (sc.isFailure()) {
     (*m_log) << MSG::ERROR << "Could not retrieve CaloNoiseTool " << m_noisetool << endreq;
     return sc;
   }
   else
     (*m_log) << MSG::DEBUG << "Successfully retrieved CaloNoiseTool " << endreq;
   
   //Set LArRodBlockStructure according to jobOpts.
   switch(m_DSPRunMode)
     {case 0:  //Obsolete mode 
	m_RodBlockStructure=NULL;
	(*m_log) << MSG::ERROR << "LArRodBlockStructure type 0 is obsolete and can't be used any more." << endreq;
	return StatusCode::FAILURE;
      break;
     case 2:  //Transparent mode, DSP just copies FEB-data                                            
       m_RodBlockStructure=new LArRodBlockTransparentV0<LArRodBlockHeaderTransparentV0>;
       (*m_log) << MSG::DEBUG << "Set Rod Block Type to LArRodBlockTransparent (#2)" << endreq;
      break;
     case 7: //Calibration mode
       m_RodBlockStructure=new LArRodBlockCalibrationV0<LArRodBlockHeaderCalibrationV0>;
       (*m_log) << MSG::DEBUG << "Set Rod Block Type to LArRodBlockCalibration (#7)" << endreq;
       break;
     case 4: //Physics assembly mode
       if ( m_RodBlockVersion == 10 ){
         m_RodBlockStructure=new LArRodBlockPhysicsV5;
         (*m_log) << MSG::DEBUG << "Set Rod Block Type to LArRodBlockPhysics (#5)" << endreq;
       } else if ( m_RodBlockVersion == 12 ){
         m_RodBlockStructure=new LArRodBlockPhysicsV6;
         (*m_log) << MSG::DEBUG << "Set Rod Block Type to LArRodBlockPhysics (#6)" << endreq;
       } else {
         m_RodBlockStructure=new LArRodBlockPhysicsV0;
         (*m_log) << MSG::DEBUG << "Set Rod Block Type to LArRodBlockPhysics (#4)" << endreq;
       }
       break;
     case 5: //Physics assembly mode
       m_RodBlockStructure=new LArRodBlockPhysicsV3;
       (*m_log) << MSG::DEBUG << "Set Rod Block Type to LArRodBlockPhysics (#5)" << endreq;
       break;
     default:
       m_RodBlockStructure=new LArRodBlockPhysicsV0;
       (*m_log) << MSG::WARNING << "DSP runmode " << m_DSPRunMode << " is unknown. Using physics assembly mode (#4) by default" << endreq;
       break;
     }

   // Set chosen RodBlockType
   LArRodEncoder::setRodBlockStructure(m_RodBlockStructure);
   (*m_log) << MSG::INFO << "Initialization done for reading and writing" << endreq;
 }
 else
   (*m_log) << MSG::INFO << "Initialization done for reading only" << endreq;
 return StatusCode::SUCCESS;  
}

 
StatusCode
LArRawDataContByteStreamTool::finalize()
{if (m_RodBlockStructure)
  delete m_RodBlockStructure;
 StatusCode sc = AlgTool::finalize(); 
 return sc;
}


StatusCode LArRawDataContByteStreamTool::WriteLArDigits(const LArDigitContainer* digitCont, FullEventAssembler<Hid2RESrcID> *fea) {
 if (!m_initializeForWriting) {
   (*m_log) << MSG::ERROR << "Tool not setup for writing! Use property " 
	  << name()<<".InitializeForWriting" <<endreq;
   return false;
 }
 (*m_log) << MSG::DEBUG << "Writing LArDigitContainer to ByteStream" << endreq;
 if (!digitCont)
   {(*m_log) << MSG::ERROR << "Null pointer passed to WriteLArDigit routine!" << endreq;
   return StatusCode::FAILURE;
   }
 if (!m_RodBlockStructure->canSetRawData() && !m_RodBlockStructure->canSetRawDataFixed())
    {(*m_log) << MSG::DEBUG << "This instance of LArRodBlockStructure can't hold LArDigits!" << endreq;
    return StatusCode::FAILURE;
   }
 //prepareWriting();
 fea->setRodMinorVersion(m_RodBlockVersion);
 fea->setDetEvtType(m_DSPRunMode);
 (*m_log) << MSG::DEBUG << "Setting Detector Event Type to " << m_DSPRunMode 
     << " and Minor Version Number to " << m_RodBlockVersion << endreq;
 FullEventAssembler<Hid2RESrcID>::RODDATA*  theROD;
 LArDigitContainer::const_iterator it_b=digitCont->begin();
 LArDigitContainer::const_iterator it_e=digitCont->end();
 if (it_b==it_e)
   {(*m_log) << MSG::WARNING << "Attempt to persitify a empty LArDigitContainer to ByteStream" << endreq;
    return StatusCode::SUCCESS;
   }

 std::map<uint32_t, LArRodEncoder> mapEncoder; 
 unsigned n=0;
 if (m_RodBlockStructure->canSetRawDataFixed() && checkGainConsistency(digitCont))
   {//Set fixed gain raw data
     int fixgain=(*it_b)->gain();
     (*m_log) <<MSG::DEBUG<<" number of Digits in LArDigitContainer for gain " << fixgain << ": " 
	 << digitCont->size() << endreq;
     // Sorting Channels by ROD
     for(; it_b!=it_e; ++it_b){
       const LArDigit* digit = *it_b; 
       HWIdentifier  chid = digit->hardwareID() ; 
       uint32_t reid      = m_hid2re.getRodID( chid ); 
       mapEncoder[reid].add(digit, fixgain);
       n++;
     }  
     (*m_log) <<MSG::VERBOSE<<" number of channels in the LArDigitContainer for gain " 
	 << fixgain << ": "<<n <<endreq;
   } // end if
 else 
   if (m_RodBlockStructure->canSetRawData()) //Set free gain raw data
     {(*m_log) <<MSG::DEBUG<<" number of channels in LArDigit container: "<< digitCont->size() <<endreq;

      // Sorting Channels by ROD
      for(; it_b!=it_e; ++it_b){
	const LArDigit* digit = *it_b; 
	HWIdentifier  chid = digit->hardwareID() ; 
	uint32_t reid      = m_hid2re.getRodID( chid ); 
	mapEncoder[reid].add(digit);
	n++;
      }  
      (*m_log) <<MSG::VERBOSE<<" number of channels added to framgent: "<<n <<endreq;
     }//  end else-if(can set Raw data)

 // Now loop over map and fill all ROD Data Blocks
 std::map<uint32_t,LArRodEncoder>::iterator it  =mapEncoder.begin(); 
 std::map<uint32_t,LArRodEncoder>::iterator it_end=mapEncoder.end();
 // LArRodEncoder has collected all the channels, now can fill the
 // ROD block data.
 for(; it!=it_end;++it) {
   theROD  = fea->getRodData( (*it).first );
   ((*it).second).fillROD(*theROD,*m_log, &(*m_noisetool), m_nfebsigma ) ; 
 } 
 (*m_log) << MSG::DEBUG << "Filled " << mapEncoder.size() << " Rod Blocks" << endreq;
 // Finally, fill full event
 //fea->fill(re,log); 
 return StatusCode::SUCCESS;
}



StatusCode LArRawDataContByteStreamTool::WriteLArCalibDigits(const LArCalibDigitContainer* digitCont, 
							     FullEventAssembler<Hid2RESrcID> *fea) {

 if (!m_initializeForWriting) {
   (*m_log) << MSG::ERROR << "Tool not setup for writing! Use property " 
	  << name()<<".InitializeForWriting" <<endreq;
   return false;
 }
 (*m_log) << MSG::DEBUG << "Writing LArCalibDigitContainer to ByteStream" << endreq;
 if (!digitCont)
   {(*m_log) << MSG::DEBUG << "Null pointer passed to WriteLArCalibDigit routine!" << endreq;
   return StatusCode::FAILURE;
   }
 if (!m_RodBlockStructure->canSetCalibration()|| !m_RodBlockStructure->canSetRawDataFixed())
    {(*m_log) << MSG::DEBUG << "This instance of LArRodBlockStructure can't hold LArCalibDigits!" << endreq;
    return StatusCode::FAILURE;
   }
 //prepareWriting(); 
 fea->setRodMinorVersion(m_RodBlockVersion);
 fea->setDetEvtType(m_DSPRunMode);
 (*m_log) << MSG::DEBUG << "Setting Detector Event Type to " << m_DSPRunMode 
     << "and Minor Version Number to " << m_RodBlockVersion << endreq;
 FullEventAssembler<Hid2RESrcID>::RODDATA*  theROD;
 LArCalibDigitContainer::const_iterator it_b=digitCont->begin();
 LArCalibDigitContainer::const_iterator it_e=digitCont->end();
 if (it_b==it_e)
   {(*m_log) << MSG::WARNING << "Attempt to persistify a empty LArDigitContainer to ByteStream" << endreq;
    return StatusCode::SUCCESS;
   }
 if (!checkGainConsistency(digitCont))
   {(*m_log) << MSG::ERROR << "Inconsistent gain in LArCalibDigitContainer" << endreq;
    return StatusCode::FAILURE;
   }

 std::map<uint32_t, LArRodEncoder> mapEncoder; 
 unsigned n=0;
 int fixgain=(*it_b)->gain();
 //(*m_log) <<MSG::DEBUG<<" number of Digits in LArCalibDigitContainer of gain " << fixgain << ": " 
 //  << digitCont->size() << endreq;
 
 // Sorting Channels by ROD
 for(; it_b!=it_e; ++it_b){
   const LArCalibDigit* digit = *it_b; 
   HWIdentifier  chid = digit->hardwareID() ; 
   uint32_t reid      = m_hid2re.getRodID( chid ); 
   mapEncoder[reid].add(digit, fixgain);
   n++;
 } 
 
 (*m_log) <<MSG::VERBOSE<<" number of channels in the LArCalibDigitContainer for gain " 
     << fixgain << ": "<<n <<endreq;

 // Now loop over map and fill all ROD Data Blocks
 std::map<uint32_t,LArRodEncoder>::iterator it  =mapEncoder.begin(); 
 std::map<uint32_t,LArRodEncoder>::iterator it_end=mapEncoder.end();
 // LArRodEncoder has collected all the channels, now can fill the
 // ROD block data.
 for(; it!=it_end;++it) {
   theROD  = fea->getRodData( (*it).first ); 
   ((*it).second).fillROD( *theROD,*m_log, &(*m_noisetool), m_nfebsigma ) ; 
 } 
 (*m_log) << MSG::DEBUG << "Filled " << mapEncoder.size() << " Rod Blocks" << endreq;
 return StatusCode::SUCCESS;
}


StatusCode LArRawDataContByteStreamTool::WriteLArRawChannels(const LArRawChannelContainer* channelCont, 
							     FullEventAssembler<Hid2RESrcID> *fea) {
 if (!m_initializeForWriting) {
   (*m_log) << MSG::ERROR << "Tool not setup for writing! Use property " 
	  << name()<<".InitializeForWriting" <<endreq;
   return false;
 }
 if (!channelCont)
  {(*m_log) << MSG::DEBUG << "Null pointer passed to WriteLArCalibDigit routine!" << endreq;
  return StatusCode::FAILURE;
  }
 if (!m_RodBlockStructure->canSetEnergy())
   {(*m_log) << MSG::DEBUG << "This instance of LArRodBlockStructure can't hold LArRawChannels!" << endreq;
    return StatusCode::FAILURE;
   }
 FullEventAssembler<Hid2RESrcID>::RODDATA*  theROD;
 fea->setRodMinorVersion(m_RodBlockVersion);
 fea->setDetEvtType(m_DSPRunMode);
 if ( (*m_log).level() <= MSG::DEBUG )
  (*m_log) << MSG::DEBUG << "Setting Detector Event Type to " << m_DSPRunMode 
	<< "and Minor Version Number to " << m_RodBlockVersion << endreq;
 LArRawChannelContainer::const_iterator it = channelCont->begin(); 
 LArRawChannelContainer::const_iterator it_e= channelCont->end(); 
 if ( it==it_e)
   {(*m_log) << MSG::WARNING << "Attempt to persistify a empty LArDigitContainer to ByteStream" << endreq;
    return StatusCode::SUCCESS;
   }
 std::map<uint32_t, LArRodEncoder> mapEncoder; 
 //LArRodEncoder* Encoder = NULL;
 //uint32_t last_reid(0x0);
 //unsigned n=0; //For debug only
 if ( (*m_log).level() <= MSG::DEBUG )
  (*m_log) <<MSG::DEBUG<<" number of LArRawChannel container "<< channelCont->size() <<endreq;
 for(; it!=it_e; ++it){
     const LArRawChannel& rawChan = *it; 
     HWIdentifier  chid = rawChan.channelID() ; 
     uint32_t reid      = m_hid2re.getRodID( chid ); 

/*
     if ( reid != last_reid ) {
	last_reid = reid;
	// The guy does not exist
	// This will create it
	mapEncoder[reid].add(&rawChan);
	// This will get its address
	Encoder = &(mapEncoder[reid]);
     } else Encoder->add(&rawChan) ; // Encoder already there
*/
     mapEncoder[reid].add(&rawChan);
   } 
 //(*m_log) <<MSG::DEBUG<<" number of channels in the LArRawChannelcollection: "<<n <<endreq;
   // I may want to also include the digits
   if ( m_includeDigits ) {
	const DataHandle<LArDigitContainer> digitCont;
	if ( m_storeGate->retrieve(digitCont,m_DigitContName).isFailure() ){
		(*m_log) << MSG::ERROR << "Digits required but not really found" << endreq;
	} else {
	  if ( m_RodBlockStructure->canIncludeRawData() ){
	   LArDigitContainer::const_iterator it_b=digitCont->begin();
	   LArDigitContainer::const_iterator it_e=digitCont->end();
	   if (it_b==it_e)
		{(*m_log) << MSG::WARNING << "Attempt to persitify a empty LArDigitContainer to ByteStream" << endreq;
	   }
	   for(; it_b!=it_e; ++it_b){
		const LArDigit* digit = *it_b;
		HWIdentifier  chid = digit->hardwareID() ;
		uint32_t reid      = m_hid2re.getRodID( chid );
		// Lets use anygain for the moment
		mapEncoder[reid].add(digit);
     	   }
	  } // End of check whether format allows to include RawData
	} // Finish checking for Digit container in SG
   } // End of check for digits inclusion
      
 // Now loop over map and fill all ROD Data Blocks
 std::map<uint32_t,LArRodEncoder>::iterator it_m  =mapEncoder.begin(); 
 std::map<uint32_t,LArRodEncoder>::iterator it_m_e=mapEncoder.end();
 // LArRodEncoder has collected all the channels, now can fill the
 // ROD block data.
 for(; it_m!=it_m_e;++it_m) {
   theROD  = fea->getRodData( (*it_m).first ); 
   ((*it_m).second).fillROD( *theROD,*m_log, &(*m_noisetool), m_nfebsigma ) ; 
   // delete ((*it_m).second);
   // ((*it_m).second)=NULL;
 } 
 (*m_log) << MSG::DEBUG << "Filled " << mapEncoder.size() << " Rod Blocks" << endreq;
 return StatusCode::SUCCESS;
}

 
StatusCode LArRawDataContByteStreamTool::prepareRobIndex(const RawEvent* re)
{
  // This `optimization' leads to disaster if there ever happen to be
  // two adjacent events with identical L1 IDs --- in that case,
  // we'd be using the dangling ROB pointers from the last event.
  // We should generally come through here only once per event anyway,
  // so just remove this test.
if ( 1/*re->lvl1_id() != m_lastEvent*/ ) {
  m_lastEvent = re->lvl1_id();
  if ( m_log->level() <=MSG::DEBUG )
    (*m_log) <<MSG::DEBUG<< "Converting event (from ByteStream)" <<endreq;

  if (!re)
    {(*m_log) << MSG::ERROR << "RawEvent passed to 'convert'-function is a null pointer!" << endreq;
     return StatusCode::FAILURE;
    }

  if (!m_decoder->check_valid (re, *m_log)) return StatusCode::FAILURE;

  if ( m_log->level() <=MSG::VERBOSE ) {
     (*m_log) << MSG::VERBOSE << "Full Event:\n";
     (*m_log) << MSG::VERBOSE << MSG::hex << "Full source ID: " << re->source_id() << MSG::dec <<endreq;
     (*m_log) << MSG::VERBOSE <<"Fragment size in words: " << re->fragment_size_word() << endreq;
  }
  // Need to clear up or it will accumulate
  m_robIndex.clear();
  eformat::helper::build_toc(*re, m_robIndex );
  if ( m_robIndex.empty() ) { // This is a problem
	return StatusCode::FAILURE;
  }
  }
  return StatusCode::SUCCESS;
}
