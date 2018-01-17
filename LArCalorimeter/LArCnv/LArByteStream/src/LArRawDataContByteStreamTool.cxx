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
  :  AthAlgTool(type,name,parent),
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
  m_lastEvent = 0xFFFFFFFF;
}

LArRawDataContByteStreamTool::~LArRawDataContByteStreamTool() {
}

 
StatusCode
LArRawDataContByteStreamTool::initialize()
{
  ATH_CHECK( AthAlgTool::initialize() );
  ATH_MSG_DEBUG ( "Initializing LArRawDataContByteStream" );
 
  ATH_CHECK( toolSvc()->retrieveTool("LArRodDecoder",m_decoder) );

  if (m_initializeForWriting) {
   ATH_CHECK( m_hid2re.initialize() );

   ATH_CHECK( m_noisetool.retrieve() );
   ATH_MSG_DEBUG ( "Successfully retrieved CaloNoiseTool " );
   
   //Set LArRodBlockStructure according to jobOpts.
   switch(m_DSPRunMode)
     {case 0:  //Obsolete mode 
	m_RodBlockStructure=NULL;
	ATH_MSG_ERROR ( "LArRodBlockStructure type 0 is obsolete and can't be used any more." );
	return StatusCode::FAILURE;
      break;
     case 2:  //Transparent mode, DSP just copies FEB-data                                            
       m_RodBlockStructure=new LArRodBlockTransparentV0<LArRodBlockHeaderTransparentV0>;
       ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockTransparent (#2)" );
      break;
     case 7: //Calibration mode
       m_RodBlockStructure=new LArRodBlockCalibrationV0<LArRodBlockHeaderCalibrationV0>;
       ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockCalibration (#7)" );
       break;
     case 4: //Physics assembly mode
       if ( m_RodBlockVersion == 10 ){
         m_RodBlockStructure=new LArRodBlockPhysicsV5;
         ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockPhysics (#5)" );
       } else if ( m_RodBlockVersion == 12 ){
         m_RodBlockStructure=new LArRodBlockPhysicsV6;
         ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockPhysics (#6)" );
       } else {
         m_RodBlockStructure=new LArRodBlockPhysicsV0;
         ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockPhysics (#4)" );
       }
       break;
     case 5: //Physics assembly mode
       m_RodBlockStructure=new LArRodBlockPhysicsV3;
       ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockPhysics (#5)" );
       break;
     default:
       m_RodBlockStructure=new LArRodBlockPhysicsV0;
       ATH_MSG_WARNING ( "DSP runmode " << m_DSPRunMode << " is unknown. Using physics assembly mode (#4) by default" );
       break;
     }

   // Set chosen RodBlockType
   LArRodEncoder::setRodBlockStructure(m_RodBlockStructure);
   ATH_MSG_INFO ( "Initialization done for reading and writing" );
 }
 else
   ATH_MSG_INFO ( "Initialization done for reading only" );
 return StatusCode::SUCCESS;  
}

 
StatusCode
LArRawDataContByteStreamTool::finalize()
{
  delete m_RodBlockStructure;
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}


StatusCode LArRawDataContByteStreamTool::WriteLArDigits(const LArDigitContainer* digitCont, FullEventAssembler<Hid2RESrcID> *fea) {
 if (!m_initializeForWriting) {
   ATH_MSG_ERROR ( "Tool not setup for writing! Use property " 
                   << name()<<".InitializeForWriting" );
   return StatusCode::FAILURE;
 }
 ATH_MSG_DEBUG ( "Writing LArDigitContainer to ByteStream" );
 if (!digitCont) {
   ATH_MSG_ERROR ( "Null pointer passed to WriteLArDigit routine!" );
   return StatusCode::FAILURE;
 }
 if (!m_RodBlockStructure->canSetRawData() && !m_RodBlockStructure->canSetRawDataFixed()) {
   ATH_MSG_DEBUG ( "This instance of LArRodBlockStructure can't hold LArDigits!" );
   return StatusCode::FAILURE;
 }
 //prepareWriting();
 fea->setRodMinorVersion(m_RodBlockVersion);
 fea->setDetEvtType(m_DSPRunMode);
 ATH_MSG_DEBUG ( "Setting Detector Event Type to " << m_DSPRunMode 
                 << " and Minor Version Number to " << m_RodBlockVersion );
 FullEventAssembler<Hid2RESrcID>::RODDATA*  theROD;
 LArDigitContainer::const_iterator it_b=digitCont->begin();
 LArDigitContainer::const_iterator it_e=digitCont->end();
 if (it_b==it_e) {
   ATH_MSG_WARNING ( "Attempt to persitify a empty LArDigitContainer to ByteStream" );
   return StatusCode::SUCCESS;
 }

 std::map<uint32_t, LArRodEncoder> mapEncoder; 
 unsigned n=0;
 if (m_RodBlockStructure->canSetRawDataFixed() && checkGainConsistency(digitCont))
   {//Set fixed gain raw data
     int fixgain=(*it_b)->gain();
     ATH_MSG_DEBUG(" number of Digits in LArDigitContainer for gain " << fixgain << ": " 
                   << digitCont->size() );
     // Sorting Channels by ROD
     for(; it_b!=it_e; ++it_b){
       const LArDigit* digit = *it_b; 
       HWIdentifier  chid = digit->hardwareID() ; 
       uint32_t reid      = m_hid2re.getRodID( chid ); 
       mapEncoder[reid].add(digit, fixgain);
       n++;
     }  
     ATH_MSG_VERBOSE(" number of channels in the LArDigitContainer for gain " 
                     << fixgain << ": "<<n );
   } // end if
 else 
   if (m_RodBlockStructure->canSetRawData()) { //Set free gain raw data
     ATH_MSG_DEBUG(" number of channels in LArDigit container: "<< digitCont->size() );

      // Sorting Channels by ROD
      for(; it_b!=it_e; ++it_b){
	const LArDigit* digit = *it_b; 
	HWIdentifier  chid = digit->hardwareID() ; 
	uint32_t reid      = m_hid2re.getRodID( chid ); 
	mapEncoder[reid].add(digit);
	n++;
      }  
      ATH_MSG_VERBOSE(" number of channels added to framgent: "<<n );
     }//  end else-if(can set Raw data)

 // Now loop over map and fill all ROD Data Blocks
 std::map<uint32_t,LArRodEncoder>::iterator it  =mapEncoder.begin(); 
 std::map<uint32_t,LArRodEncoder>::iterator it_end=mapEncoder.end();
 // LArRodEncoder has collected all the channels, now can fill the
 // ROD block data.
 for(; it!=it_end;++it) {
   theROD  = fea->getRodData( (*it).first );
   ((*it).second).fillROD(*theROD,msg(), &(*m_noisetool), m_nfebsigma ) ; 
 } 
 ATH_MSG_DEBUG ( "Filled " << mapEncoder.size() << " Rod Blocks" );
 // Finally, fill full event
 //fea->fill(re,log); 
 return StatusCode::SUCCESS;
}



StatusCode LArRawDataContByteStreamTool::WriteLArCalibDigits(const LArCalibDigitContainer* digitCont, 
							     FullEventAssembler<Hid2RESrcID> *fea) {

 if (!m_initializeForWriting) {
   ATH_MSG_ERROR ( "Tool not setup for writing! Use property " 
                   << name()<<".InitializeForWriting" );
   return StatusCode::FAILURE;
 }
 ATH_MSG_DEBUG ( "Writing LArCalibDigitContainer to ByteStream" );
 if (!digitCont) {
   ATH_MSG_DEBUG ( "Null pointer passed to WriteLArCalibDigit routine!" );
   return StatusCode::FAILURE;
 }
 if (!m_RodBlockStructure->canSetCalibration()|| !m_RodBlockStructure->canSetRawDataFixed()) {
   ATH_MSG_DEBUG ( "This instance of LArRodBlockStructure can't hold LArCalibDigits!" );
   return StatusCode::FAILURE;
 }
 //prepareWriting(); 
 fea->setRodMinorVersion(m_RodBlockVersion);
 fea->setDetEvtType(m_DSPRunMode);
 ATH_MSG_DEBUG ( "Setting Detector Event Type to " << m_DSPRunMode 
                 << "and Minor Version Number to " << m_RodBlockVersion );
 FullEventAssembler<Hid2RESrcID>::RODDATA*  theROD;
 LArCalibDigitContainer::const_iterator it_b=digitCont->begin();
 LArCalibDigitContainer::const_iterator it_e=digitCont->end();
 if (it_b==it_e) {
   ATH_MSG_WARNING ( "Attempt to persistify a empty LArDigitContainer to ByteStream" );
   return StatusCode::SUCCESS;
 }
 if (!checkGainConsistency(digitCont)) {
   ATH_MSG_ERROR ( "Inconsistent gain in LArCalibDigitContainer" );
   return StatusCode::FAILURE;
 }

 std::map<uint32_t, LArRodEncoder> mapEncoder; 
 unsigned n=0;
 int fixgain=(*it_b)->gain();
 
 // Sorting Channels by ROD
 for(; it_b!=it_e; ++it_b){
   const LArCalibDigit* digit = *it_b; 
   HWIdentifier  chid = digit->hardwareID() ; 
   uint32_t reid      = m_hid2re.getRodID( chid ); 
   mapEncoder[reid].add(digit, fixgain);
   n++;
 } 
 
 ATH_MSG_VERBOSE(" number of channels in the LArCalibDigitContainer for gain " 
                 << fixgain << ": "<<n );

 // Now loop over map and fill all ROD Data Blocks
 std::map<uint32_t,LArRodEncoder>::iterator it  =mapEncoder.begin(); 
 std::map<uint32_t,LArRodEncoder>::iterator it_end=mapEncoder.end();
 // LArRodEncoder has collected all the channels, now can fill the
 // ROD block data.
 for(; it!=it_end;++it) {
   theROD  = fea->getRodData( (*it).first ); 
   ((*it).second).fillROD( *theROD,msg(), &(*m_noisetool), m_nfebsigma ) ; 
 } 
 ATH_MSG_DEBUG ( "Filled " << mapEncoder.size() << " Rod Blocks" );
 return StatusCode::SUCCESS;
}


StatusCode LArRawDataContByteStreamTool::WriteLArRawChannels(const LArRawChannelContainer* channelCont, 
							     FullEventAssembler<Hid2RESrcID> *fea) {
 if (!m_initializeForWriting) {
   ATH_MSG_ERROR ( "Tool not setup for writing! Use property " 
                   << name()<<".InitializeForWriting" );
   return StatusCode::FAILURE;
 }
 if (!channelCont) {
   ATH_MSG_DEBUG ( "Null pointer passed to WriteLArCalibDigit routine!" );
  return StatusCode::FAILURE;
 }
 if (!m_RodBlockStructure->canSetEnergy()) {
   ATH_MSG_DEBUG ( "This instance of LArRodBlockStructure can't hold LArRawChannels!" );
    return StatusCode::FAILURE;
 }
 FullEventAssembler<Hid2RESrcID>::RODDATA*  theROD;
 fea->setRodMinorVersion(m_RodBlockVersion);
 fea->setDetEvtType(m_DSPRunMode);
 ATH_MSG_DEBUG ( "Setting Detector Event Type to " << m_DSPRunMode 
                 << "and Minor Version Number to " << m_RodBlockVersion );
 LArRawChannelContainer::const_iterator it = channelCont->begin(); 
 LArRawChannelContainer::const_iterator it_e= channelCont->end(); 
 if ( it==it_e) {
   ATH_MSG_WARNING ( "Attempt to persistify a empty LArDigitContainer to ByteStream" );
   return StatusCode::SUCCESS;
 }
 std::map<uint32_t, LArRodEncoder> mapEncoder; 
 //LArRodEncoder* Encoder = NULL;
 //uint32_t last_reid(0x0);
 //unsigned n=0; //For debug only
 ATH_MSG_DEBUG(" number of LArRawChannel container "<< channelCont->size() );
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
   // I may want to also include the digits
   if ( m_includeDigits ) {
	const DataHandle<LArDigitContainer> digitCont;
	if ( evtStore()->retrieve(digitCont,m_DigitContName).isFailure() ){
          ATH_MSG_ERROR ( "Digits required but not really found" );
	} else {
	  if ( m_RodBlockStructure->canIncludeRawData() ){
	   LArDigitContainer::const_iterator it_b=digitCont->begin();
	   LArDigitContainer::const_iterator it_e=digitCont->end();
	   if (it_b==it_e) {
             ATH_MSG_WARNING ( "Attempt to persitify a empty LArDigitContainer to ByteStream" );
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
   ((*it_m).second).fillROD( *theROD,msg(), &(*m_noisetool), m_nfebsigma ) ; 
   // delete ((*it_m).second);
   // ((*it_m).second)=NULL;
 } 
 ATH_MSG_DEBUG ( "Filled " << mapEncoder.size() << " Rod Blocks" );
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
  ATH_MSG_DEBUG( "Converting event (from ByteStream)" );

  if (!re) {
    ATH_MSG_ERROR ( "RawEvent passed to 'convert'-function is a null pointer!" );
    return StatusCode::FAILURE;
  }

  if (!m_decoder->check_valid (re, msg())) return StatusCode::FAILURE;

  ATH_MSG_VERBOSE ( "Full Event:" );
  ATH_MSG_VERBOSE ( MSG::hex << "Full source ID: " << re->source_id() << MSG::dec );
  ATH_MSG_VERBOSE ("Fragment size in words: " << re->fragment_size_word() );

  // Need to clear up or it will accumulate
  m_robIndex.clear();
  eformat::helper::build_toc(*re, m_robIndex );
  if ( m_robIndex.empty() ) { // This is a problem
	return StatusCode::FAILURE;
  }
  }
  return StatusCode::SUCCESS;
}
