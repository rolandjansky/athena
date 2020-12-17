/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArByteStream/LArRawDataContByteStreamTool.h"
#include "LArByteStream/LArRodEncoder.h"
#include "LArByteStream/LArRodDecoder.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 

#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "LArByteStream/LArRodBlockStructure.h"
//#include "LArByteStream/LArRodBlockStructure_0.h"
#include "LArByteStream/LArRodBlockTransparentV0.h"
#include "LArByteStream/LArRodBlockCalibrationV0.h"
#include "LArByteStream/LArRodBlockPhysicsV0.h"
#include "LArByteStream/LArRodBlockPhysicsV3.h"
#include "LArByteStream/LArRodBlockPhysicsV5.h"
#include "LArByteStream/LArRodBlockPhysicsV6.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "StoreGate/ReadCondHandle.h"

// STL stuff
#include <map> 


static const InterfaceID IID_ILArRawDataContByteStreamTool
            ("LArRawDataContByteStreamTool", 1, 0);


const InterfaceID& LArRawDataContByteStreamTool::interfaceID( )
{ return IID_ILArRawDataContByteStreamTool; }

// default contructor
 
LArRawDataContByteStreamTool::LArRawDataContByteStreamTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent)
{ declareInterface< LArRawDataContByteStreamTool  >( this );
  declareProperty("DSPRunMode",m_DSPRunMode=4);
  declareProperty("RodBlockVersion",m_RodBlockVersion=0);
  declareProperty("FebNoiseCut",m_nfebsigma=2);
  declareProperty("InitializeForWriting",m_initializeForWriting=false);
  declareProperty("SubDetectorId",m_subDetId=0);
  declareProperty("IncludeDigits",m_includeDigits=false);
  declareProperty("DigitsContainer",m_DigitContName="LArDigitContainer_MC_Thinned");
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
   if (m_DSPRunMode == 0) {
     // Obsolete mode 
     ATH_MSG_ERROR ( "LArRodBlockStructure type 0 is obsolete and can't be used any more." );
     return StatusCode::FAILURE;
   }

   ATH_CHECK( detStore()->retrieve (m_onlineHelper, "LArOnlineID") );

   ATH_MSG_INFO ( "Initialization done for reading and writing" );
 }
  else {
    ATH_MSG_INFO ( "Initialization done for reading only" );
  }

  ATH_CHECK( m_caloNoiseKey.initialize (m_initializeForWriting) );
  ATH_CHECK( m_onOffIdMappingKey.initialize (m_initializeForWriting) );
  ATH_CHECK( m_febRodMappingKey.initialize (m_initializeForWriting) );

  return StatusCode::SUCCESS;  
}

 
StatusCode
LArRawDataContByteStreamTool::finalize()
{
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}


StatusCode
LArRawDataContByteStreamTool::WriteLArDigits(const LArDigitContainer* digitCont,
                                             FEA_t& fea) const
{
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

 std::unique_ptr<LArRodBlockStructure> blstruct = makeRodBlockStructure();

 if (!blstruct->canSetRawData() && !blstruct->canSetRawDataFixed()) {
   ATH_MSG_DEBUG ( "This instance of LArRodBlockStructure can't hold LArDigits!" );
   return StatusCode::FAILURE;
 }
 //prepareWriting();
 fea.setRodMinorVersion(m_RodBlockVersion);
 fea.setDetEvtType(m_DSPRunMode);
 ATH_MSG_DEBUG ( "Setting Detector Event Type to " << m_DSPRunMode 
                 << " and Minor Version Number to " << m_RodBlockVersion );
 FEA_t::RODDATA*  theROD;
 LArDigitContainer::const_iterator it_b=digitCont->begin();
 LArDigitContainer::const_iterator it_e=digitCont->end();
 if (it_b==it_e) {
   ATH_MSG_WARNING ( "Attempt to persistify an empty LArDigitContainer to ByteStream" );
   return StatusCode::SUCCESS;
 }

 const CaloDetDescrManager* calodd = nullptr;
 ATH_CHECK( detStore()->retrieve (calodd, "CaloMgr") );
 const EventContext& ctx = Gaudi::Hive::currentContext();
 SG::ReadCondHandle<LArOnOffIdMapping> onOffMapping (m_onOffIdMappingKey, ctx);
 SG::ReadCondHandle<LArFebRodMapping> febRodMapping (m_febRodMappingKey, ctx);
 const Hid2RESrcID& hid2re = getHid2RESrcID (**febRodMapping);

 std::map<uint32_t, LArRodEncoder> mapEncoder; 

 auto getEncoder = [&] (uint32_t reid) -> LArRodEncoder&
                   { return mapEncoder.try_emplace (reid,
                                                    *m_onlineHelper,
                                                    *calodd,
                                                    **onOffMapping,
                                                    blstruct.get()).first->second; };

 unsigned n=0;
 if (blstruct->canSetRawDataFixed() && checkGainConsistency(digitCont))
   {//Set fixed gain raw data
     int fixgain=(*it_b)->gain();
     ATH_MSG_DEBUG(" number of Digits in LArDigitContainer for gain " << fixgain << ": " 
                   << digitCont->size() );
     // Sorting Channels by ROD
     for(; it_b!=it_e; ++it_b){
       const LArDigit* digit = *it_b; 
       HWIdentifier  chid = digit->hardwareID() ; 
       uint32_t reid      = hid2re.getRodID( **febRodMapping, chid );
       getEncoder(reid).add (digit, fixgain);
       n++;
     }  
     ATH_MSG_VERBOSE(" number of channels in the LArDigitContainer for gain " 
                     << fixgain << ": "<<n );
   } // end if
 else 
   if (blstruct->canSetRawData()) { //Set free gain raw data
     ATH_MSG_DEBUG(" number of channels in LArDigit container: "<< digitCont->size() );

      // Sorting Channels by ROD
      for(; it_b!=it_e; ++it_b){
	const LArDigit* digit = *it_b; 
	HWIdentifier  chid = digit->hardwareID() ; 
	uint32_t reid      = hid2re.getRodID( **febRodMapping, chid ); 
	getEncoder(reid).add (digit);
	n++;
      }  
      ATH_MSG_VERBOSE(" number of channels added to framgent: "<<n );
     }//  end else-if(can set Raw data)

 SG::ReadCondHandle<CaloNoise> noise (m_caloNoiseKey, ctx);

 // Now loop over map and fill all ROD Data Blocks
 std::map<uint32_t,LArRodEncoder>::iterator it  =mapEncoder.begin(); 
 std::map<uint32_t,LArRodEncoder>::iterator it_end=mapEncoder.end();
 // LArRodEncoder has collected all the channels, now can fill the
 // ROD block data.
 for(; it!=it_end;++it) {
   theROD  = fea.getRodData( (*it).first );
   ((*it).second).fillROD(*theROD,msg(), **noise, m_nfebsigma ) ; 
 } 
 ATH_MSG_DEBUG ( "Filled " << mapEncoder.size() << " Rod Blocks" );
 // Finally, fill full event
 //fea.fill(re,log); 
 return StatusCode::SUCCESS;
}



StatusCode
LArRawDataContByteStreamTool::WriteLArCalibDigits(const LArCalibDigitContainer* digitCont, 
                                                  FEA_t& fea) const
{
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

 std::unique_ptr<LArRodBlockStructure> blstruct = makeRodBlockStructure();

 if (!blstruct->canSetCalibration()|| !blstruct->canSetRawDataFixed()) {
   ATH_MSG_DEBUG ( "This instance of LArRodBlockStructure can't hold LArCalibDigits!" );
   return StatusCode::FAILURE;
 }
 //prepareWriting(); 
 fea.setRodMinorVersion(m_RodBlockVersion);
 fea.setDetEvtType(m_DSPRunMode);
 ATH_MSG_DEBUG ( "Setting Detector Event Type to " << m_DSPRunMode 
                 << "and Minor Version Number to " << m_RodBlockVersion );
 FEA_t::RODDATA*  theROD;
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

 const CaloDetDescrManager* calodd = nullptr;
 ATH_CHECK( detStore()->retrieve (calodd, "CaloMgr") );
 const EventContext& ctx = Gaudi::Hive::currentContext();
 SG::ReadCondHandle<LArOnOffIdMapping> onOffMapping (m_onOffIdMappingKey, ctx);
 SG::ReadCondHandle<LArFebRodMapping> febRodMapping (m_febRodMappingKey, ctx);
 const Hid2RESrcID& hid2re = getHid2RESrcID (**febRodMapping);

 std::map<uint32_t, LArRodEncoder> mapEncoder; 

 auto getEncoder = [&] (uint32_t reid) -> LArRodEncoder&
                   { return mapEncoder.try_emplace (reid,
                                                    *m_onlineHelper,
                                                    *calodd,
                                                    **onOffMapping,
                                                    blstruct.get()).first->second; };

 unsigned n=0;
 int fixgain=(*it_b)->gain();
 
 // Sorting Channels by ROD
 for(; it_b!=it_e; ++it_b){
   const LArCalibDigit* digit = *it_b; 
   HWIdentifier  chid = digit->hardwareID() ; 
   uint32_t reid      = hid2re.getRodID( **febRodMapping, chid );
   getEncoder(reid).add (digit, fixgain);
   n++;
 } 
 
 ATH_MSG_VERBOSE(" number of channels in the LArCalibDigitContainer for gain " 
                 << fixgain << ": "<<n );

 SG::ReadCondHandle<CaloNoise> noise (m_caloNoiseKey, ctx);

 // Now loop over map and fill all ROD Data Blocks
 std::map<uint32_t,LArRodEncoder>::iterator it  =mapEncoder.begin(); 
 std::map<uint32_t,LArRodEncoder>::iterator it_end=mapEncoder.end();
 // LArRodEncoder has collected all the channels, now can fill the
 // ROD block data.
 for(; it!=it_end;++it) {
   theROD  = fea.getRodData( (*it).first ); 
   ((*it).second).fillROD( *theROD,msg(), **noise, m_nfebsigma ) ; 
 } 
 ATH_MSG_DEBUG ( "Filled " << mapEncoder.size() << " Rod Blocks" );
 return StatusCode::SUCCESS;
}


StatusCode
LArRawDataContByteStreamTool::WriteLArRawChannels(const LArRawChannelContainer* channelCont, 
                                                  FEA_t& fea) const
{
 if (!m_initializeForWriting) {
   ATH_MSG_ERROR ( "Tool not setup for writing! Use property " 
                   << name()<<".InitializeForWriting" );
   return StatusCode::FAILURE;
 }
 if (!channelCont) {
   ATH_MSG_DEBUG ( "Null pointer passed to WriteLArCalibDigit routine!" );
  return StatusCode::FAILURE;
 }

 std::unique_ptr<LArRodBlockStructure> blstruct = makeRodBlockStructure();

 if (!blstruct->canSetEnergy()) {
   ATH_MSG_DEBUG ( "This instance of LArRodBlockStructure can't hold LArRawChannels!" );
    return StatusCode::FAILURE;
 }
 FEA_t::RODDATA*  theROD;
 fea.setRodMinorVersion(m_RodBlockVersion);
 fea.setDetEvtType(m_DSPRunMode);
 ATH_MSG_DEBUG ( "Setting Detector Event Type to " << m_DSPRunMode 
                 << "and Minor Version Number to " << m_RodBlockVersion );
 LArRawChannelContainer::const_iterator it = channelCont->begin(); 
 LArRawChannelContainer::const_iterator it_e= channelCont->end(); 
 if ( it==it_e) {
   ATH_MSG_WARNING ( "Attempt to persistify a empty LArDigitContainer to ByteStream" );
   return StatusCode::SUCCESS;
 }

 const CaloDetDescrManager* calodd = nullptr;
 ATH_CHECK( detStore()->retrieve (calodd, "CaloMgr") );
 const EventContext& ctx = Gaudi::Hive::currentContext();
 SG::ReadCondHandle<LArOnOffIdMapping> onOffMapping (m_onOffIdMappingKey, ctx);
 SG::ReadCondHandle<LArFebRodMapping> febRodMapping (m_febRodMappingKey, ctx);
 const Hid2RESrcID& hid2re = getHid2RESrcID (**febRodMapping);

 std::map<uint32_t, LArRodEncoder> mapEncoder; 

 auto getEncoder = [&] (uint32_t reid) -> LArRodEncoder&
                   { return mapEncoder.try_emplace (reid,
                                                    *m_onlineHelper,
                                                    *calodd,
                                                    **onOffMapping,
                                                    blstruct.get()).first->second; };

 //LArRodEncoder* Encoder = NULL;
 //uint32_t last_reid(0x0);
 //unsigned n=0; //For debug only
 ATH_MSG_DEBUG(" number of LArRawChannel container "<< channelCont->size() );
 for(; it!=it_e; ++it){
     const LArRawChannel& rawChan = *it; 
     HWIdentifier  chid = rawChan.channelID() ; 
     uint32_t reid      = hid2re.getRodID( **febRodMapping, chid ); 

/*
     if ( reid != last_reid ) {
	last_reid = reid;
	// The guy does not exist
	// This will create it
	getEncoder(reid).add(&rawChan);
	// This will get its address
	Encoder = &(mapEncoder[reid]);
     } else Encoder->add(&rawChan) ; // Encoder already there
*/
     getEncoder(reid).add (&rawChan);
   } 
   // I may want to also include the digits
   if ( m_includeDigits ) {
        const LArDigitContainer* digitCont = nullptr;
	if ( evtStore()->retrieve(digitCont,m_DigitContName).isFailure() ){
          ATH_MSG_ERROR ( "Digits required but not really found" );
	} else {
	  if ( blstruct->canIncludeRawData() ){
	   LArDigitContainer::const_iterator it_b=digitCont->begin();
	   LArDigitContainer::const_iterator it_e=digitCont->end();
	   if (it_b==it_e) {
             ATH_MSG_WARNING ( "Attempt to persistify a empty LArDigitContainer to ByteStream" );
	   }
	   for(; it_b!=it_e; ++it_b){
		const LArDigit* digit = *it_b;
		HWIdentifier  chid = digit->hardwareID() ;
		uint32_t reid      = hid2re.getRodID( **febRodMapping, chid );
		// Lets use anygain for the moment
		getEncoder(reid).add (digit);
     	   }
	  } // End of check whether format allows to include RawData
	} // Finish checking for Digit container in SG
   } // End of check for digits inclusion

 SG::ReadCondHandle<CaloNoise> noise (m_caloNoiseKey, ctx);
      
 // Now loop over map and fill all ROD Data Blocks
 std::map<uint32_t,LArRodEncoder>::iterator it_m  =mapEncoder.begin(); 
 std::map<uint32_t,LArRodEncoder>::iterator it_m_e=mapEncoder.end();
 // LArRodEncoder has collected all the channels, now can fill the
 // ROD block data.
 for(; it_m!=it_m_e;++it_m) {
   theROD  = fea.getRodData( (*it_m).first ); 
   ((*it_m).second).fillROD( *theROD,msg(), **noise, m_nfebsigma ) ; 
   // delete ((*it_m).second);
   // ((*it_m).second)=NULL;
 } 
 ATH_MSG_DEBUG ( "Filled " << mapEncoder.size() << " Rod Blocks" );
 return StatusCode::SUCCESS;
}

 
StatusCode
LArRawDataContByteStreamTool::prepareRobIndex(const RawEvent* re,
                                              RobIndex_t& robIndex) const
{
  // This `optimization' leads to disaster if there ever happen to be
  // two adjacent events with identical L1 IDs --- in that case,
  // we'd be using the dangling ROB pointers from the last event.
  // We should generally come through here only once per event anyway.
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
  robIndex.clear();
  eformat::helper::build_toc(*re, robIndex );
  if ( robIndex.empty() ) { // This is a problem
	return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


/** Construct a RodBlockStructure instance of the proper concrete type. */
std::unique_ptr<LArRodBlockStructure>
LArRawDataContByteStreamTool::makeRodBlockStructure() const
{
  switch(m_DSPRunMode) {
  case 0:  // Obsolete; shouldn't get here.
    std::abort();

  case 2:  //Transparent mode, DSP just copies FEB-data                                            
    ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockTransparent (#2)" );
    return std::make_unique<LArRodBlockTransparentV0<LArRodBlockHeaderTransparentV0> >();
    break;

  case 7: //Calibration mode
    ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockCalibration (#7)" );
    return std::make_unique<LArRodBlockCalibrationV0<LArRodBlockHeaderCalibrationV0> >();
    break;

  case 4: //Physics assembly mode
    if ( m_RodBlockVersion == 10 ){
      ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockPhysics (#5)" );
      return std::make_unique<LArRodBlockPhysicsV5>();
    }
    else if ( m_RodBlockVersion == 12 ){
      ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockPhysics (#6)" );
      return std::make_unique<LArRodBlockPhysicsV6>();
    }
    else {
      ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockPhysics (#4)" );
      return std::make_unique<LArRodBlockPhysicsV0>();
    }
    break;

  case 5: //Physics assembly mode
    ATH_MSG_DEBUG ( "Set Rod Block Type to LArRodBlockPhysics (#5)" );
    return std::make_unique<LArRodBlockPhysicsV3>();

  default:
    ATH_MSG_WARNING ( "DSP runmode " << m_DSPRunMode << " is unknown. Using physics assembly mode (#4) by default" );
    return std::make_unique<LArRodBlockPhysicsV0>();
  }
}


const Hid2RESrcID&
LArRawDataContByteStreamTool::getHid2RESrcID (const LArFebRodMapping& rodMapping) const
{
  if (!m_hid2re) {
    auto hid2re = std::make_unique<Hid2RESrcID>();
    if (hid2re->initialize (rodMapping).isFailure()) {
      std::abort();
    }
    m_hid2re.set (std::move (hid2re));
  }
  return *m_hid2re.get();
}

