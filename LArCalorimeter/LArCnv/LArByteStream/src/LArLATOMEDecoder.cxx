/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#define DETAIL_DUMP_ON false
#define ADC_DUMP_ON false
#define VALIDATE_DUMP_ON false

#include <byteswap.h>
#include "LArByteStream/LArLATOMEDecoder.h"
#include "LArByteStream/LATOMEMapping.h"
#include "LArByteStream/Mon.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
static const InterfaceID IID_ILArLATOMEDecoder("LArLATOMEDecoder", 1, 0);

using namespace OFFLINE_FRAGMENTS_NAMESPACE;

LArLATOMEDecoder::LArLATOMEDecoder(const std::string& type, const std::string& name, const IInterface* parent) 
  : AthAlgTool(type,name,parent), m_detailDumpFileName("EmptyPath"), m_ADCDumpFileName("EmptyPath"), m_protectSourceId(true)
{
  declareInterface<LArLATOMEDecoder>(this);
  declareProperty("DumpFile", m_detailDumpFileName);
  declareProperty("RawDataFile", m_ADCDumpFileName);
  declareProperty("ProtectSourceId", m_protectSourceId); // discard main readout sourceID, should be false for reading all files from the mon path with old source IDs
}

const InterfaceID& LArLATOMEDecoder::interfaceID()
{return IID_ILArLATOMEDecoder;}

LArLATOMEDecoder::~LArLATOMEDecoder() {}

StatusCode LArLATOMEDecoder::initialize() {
#if DETAIL_DUMP_ON
  m_detailDumpFile = new std::ofstream(m_detailDumpFileName);
#endif
#if ADC_DUMP_ON
  m_ADCDumpFile = new std::ofstream(m_ADCDumpFileName);
#endif

  return StatusCode::SUCCESS;
}

StatusCode LArLATOMEDecoder::finalize() {
#if DETAIL_DUMP_ON
  m_detailDumpFile->close();
#endif
#if ADC_DUMP_ON
  m_ADCDumpFile->close();
#endif

  return StatusCode::SUCCESS;
}

StatusCode LArLATOMEDecoder::convert(const RawEvent* re, const LArLATOMEMapping *map,
                                     LArDigitContainer* adc_coll,
				     LArDigitContainer* adc_bas_coll,
				     LArRawSCContainer* et_coll,
				     LArRawSCContainer* et_id_coll,
				     LArLATOMEHeaderContainer* header_coll) const {
  bool ret = false;
  // Check fragment validity:
  try {ret = re->check();}
  catch (eformat::Issue& ex) {
    msg(MSG::WARNING) << "Exception while checking eformat fragment validity: " << ex.what() << endmsg; 
    ret = false;
  }
  if (!ret) {
    msg(MSG::ERROR) << "Got invalid RawEvent fragment" << endmsg;
    return StatusCode::FAILURE;
  }
  //Build TOC
  std::map<eformat::SubDetectorGroup, std::vector<const uint32_t*> > robIndex;
  eformat::helper::build_toc(*re, robIndex );
  for (auto mapit : robIndex) msg(MSG::DEBUG) << "Rob Index subdetgroup is " << std::hex << mapit.first << endmsg;
  std::map<eformat::SubDetectorGroup, std::vector<const uint32_t*> >::const_iterator robIt = robIndex.find(eformat::LAR);
  if (robIt!=robIndex.end()) {
    const std::vector<const uint32_t*>& robs = robIt->second;
    for (const uint32_t* pRob :robs) {
      try {
	ROBFragment robFrag(pRob);
	if(m_protectSourceId){
	  uint32_t latomeSourceID = robFrag.rod_source_id();
	  if( !(latomeSourceID & 0x1000) ){
	    msg(MSG::DEBUG) << " discarding non latome source ID " << std::hex << latomeSourceID << endmsg;
	    continue;
	  }
	  ATH_MSG_DEBUG(" found latome source ID " << std::hex << latomeSourceID);
	}
	EventProcess ev(this, map, adc_coll, adc_bas_coll, et_coll, et_id_coll, header_coll);
	ev.fillCollection(&robFrag, map);
      }	
      catch (eformat::Issue& ex) {
	ATH_MSG_WARNING ( " exception thrown by ROBFragment, badly corrupted event. Abort decoding " );
	if(adc_coll)adc_coll->clear();
	if(adc_bas_coll)adc_bas_coll->clear();
	if(et_coll)et_coll->clear();
	if(et_id_coll)et_id_coll->clear();
	if(header_coll)header_coll->clear();
	break;
      }
    }//end loop over robs
  }//end if got LAr-RODs   
  return StatusCode::SUCCESS;
}

LArLATOMEDecoder::EventProcess::EventProcess(const LArLATOMEDecoder* decoderInput, const LArLATOMEMapping *,
                                             LArDigitContainer* adc_coll, 
                                             LArDigitContainer* adc_bas_coll,
					     LArRawSCContainer* et_coll,
					     LArRawSCContainer* et_id_coll,
					     LArLATOMEHeaderContainer* header_coll)
  : m_decoder(decoderInput),
    m_adc_coll(adc_coll),
    m_adc_bas_coll(adc_bas_coll),
    m_et_coll(et_coll),
    m_et_id_coll(et_id_coll),
    m_header_coll(header_coll)
{
  m_latomeID=0;
  m_l1ID=0;
  m_nPackets=0;
  m_iPacket=0;
  m_nWordsPerPacket=0;
  m_monHeaderSize=0;
  m_region=0;
  m_nStreams=0;
  m_streamNumber=0;
  m_at0type=(Word)MonDataType::Invalid;
  m_at1type=(Word)MonDataType::Invalid;
  m_at0nBC=0;
  m_at1nBC=0;
  m_at0BC=0;
  m_at1BC=0;
  m_activeSC=0;
  m_nsc1=0;
  m_nsc2=0;
  m_nsc3=0;
  m_nsc4=0;
  m_nsc5=0;
  m_nsc6=0;
  m_headerDecoded=false;
  
  m_nBC_rawADC=0;
  m_nBC_ADC=0;
  m_nBC_E=0;
  m_nBC_EID=0;

  m_BC_rawADC=0;
  m_BC_ADC=0;
  m_BC_E=0;
  m_BC_EID=0;
  
  m_hasRawAdc=false;
  m_hasAdc=false;
  m_hasE=false;
  m_hasEID=false;

  m_rawValuesInEvent.resize(N_LATOME_CHANNELS);
}

bool LArLATOMEDecoder::EventProcess::compareOrSet(Word& param, Word value, bool compare){
  if(!compare){
    param=value;
    return true;
  }
  return param==value;
}

unsigned int LArLATOMEDecoder::EventProcess::decodeTrailer(const uint32_t* p, unsigned int offset) {
  /// for now the trailerhas only 2 fixed words, just check them
  if(bswap_32(p[offset])!=0xc0ffee00 ||  bswap_32(p[offset+1]!=0xaaaaaaaa)){
    m_decoder->msg(MSG::WARNING) << "Problem in trailer at packet " << m_iPacket
				 << " words " << std::hex << bswap_32(p[offset]) << ", "
				 << bswap_32(p[offset+1]) << std::dec  << endmsg;
  }
  /// fixed shift for now but in case we get a dynamic trailer (hopefully not)
  return offset+m_monTrailerSize;
}

unsigned int LArLATOMEDecoder::EventProcess::decodeHeader(const uint32_t* p, unsigned int offset) {

  //// lets do it quick and dirty now. Should be handled more properly when we have the LatomeHeaderClass
  int monheadererror=0;
  int monheadererrorbit=0;
  if(!compareOrSet(m_latomeID, bswap_32(p[0+offset]), m_headerDecoded)) monheadererror |= (1<<monheadererrorbit++);
  Word l1IDtmp=m_l1ID;
  if(!compareOrSet(l1IDtmp, bswap_32(p[1+offset]), m_headerDecoded)) monheadererror |= (1<<monheadererrorbit++);
  if(l1IDtmp != m_l1ID){
    m_decoder->msg(MSG::WARNING) << "Mon header L1ID " << l1IDtmp << " different from rod header L1ID " << m_l1ID << endmsg;
  }
  m_decoder->msg(MSG::DEBUG) << " latomeID: " << m_latomeID  << " l1ID: " << m_l1ID << endmsg;
  
  Word monHeaderMarker=bswap_32(p[2+offset]);
  Word monCheckPoint = bswap_32(p[4+offset]);
  if(s_monHeaderMarker!=monHeaderMarker){
    monheadererror |= (1<<monheadererrorbit++);
    m_decoder->msg(MSG::WARNING) << "Problem in monHeaderMarker: " << monHeaderMarker << endmsg;
  }
  if(s_monCheckPoint!=monCheckPoint){
    monheadererror |= (1<<monheadererrorbit++);
    m_decoder->msg(MSG::WARNING) << "Problem in monCheckPoint: " <<monCheckPoint << endmsg;
  }

  if(!compareOrSet(m_nPackets, bswap_32(p[3+offset])>>24, m_headerDecoded ))  monheadererror |= (1<<monheadererrorbit++);

  if(m_headerDecoded)++m_iPacket;
  else m_iPacket=0;
  if(!compareOrSet(m_iPacket,  (bswap_32(p[3+offset])>>16)&0xf, m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  m_nWordsPerPacket = (bswap_32(p[3+offset])&0xffff)/4.;
  if(!compareOrSet(m_monHeaderSize, bswap_32(p[5+offset]), m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);

  //// for now we require more that 18
  //// we will need to understand how to support different header format with firmware version, but we should minimize changes (only add words).
  if(m_monHeaderSize<18){
    m_decoder->msg(MSG::ERROR)  << " Mon header size should not be less that 18 : =" << m_monHeaderSize << " are you reading old data file?" << endmsg;
    return 0;
  }

  m_decoder->msg(MSG::DEBUG) << " nPackets: " << m_nPackets  << " iPacket: " << m_iPacket 
			     << " nWordsPerPacket: " << m_nWordsPerPacket  << " monHeaderSize: " << m_monHeaderSize 
			     << endmsg;

  /// now these are taken from the ROD header but the word are still here (maybe we will use them for something else)
  //if(!compareOrSet(m_at0type, bswap_32(p[9+offset]), m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  // if(!compareOrSet(m_at1type, bswap_32(p[12+offset]), m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  monheadererrorbit+=2;
  
  if(!compareOrSet(m_at0nBC, bswap_32(p[10+offset]), m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet(m_at1nBC, bswap_32(p[13+offset]), m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet(m_at0BC, bswap_32(p[11+offset]), m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet(m_at1BC, bswap_32(p[14+offset]), m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet(m_activeSC, bswap_32(p[15+offset]), m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet(m_nsc1, bswap_32(p[16])>>24, m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet(m_nsc2, (bswap_32(p[16])>>16)&0xff, m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet( m_nsc3, (bswap_32(p[16])>>8)&0xff, m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet(m_nsc4, (bswap_32(p[16]))&0xff, m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet(m_nsc5, bswap_32(p[17])>>24, m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);
  if(!compareOrSet(m_nsc6, (bswap_32(p[17])>>16)&0xff, m_headerDecoded))  monheadererror |= (1<<monheadererrorbit++);

  m_decoder->msg(MSG::DEBUG) << " at0type " <<m_at0type
			     << " at1type " <<m_at1type
			     << " at0nBC " <<m_at0nBC
			     << " at1nBC " <<m_at1nBC
			     << " at0BC " <<m_at0BC
			     << " at1BC " <<m_at1BC
			     << " nsc1 " <<m_nsc1
			     << " nsc2 " <<m_nsc2
			     << " nsc3 " <<m_nsc3
			     << " nsc4 " <<m_nsc4
			     << " nsc5 " <<m_nsc5
			     << " nsc6 " <<m_nsc6
			     << endmsg;


  if(monheadererror){
    m_decoder->msg(MSG::WARNING) << " consistency error in mon checker at packet " << m_iPacket << " errorbits " << std::hex<< monheadererror << std::dec << endmsg;
  }

  m_headerDecoded=true;
  return m_monHeaderSize+m_nWordsPerPacket+offset;

}

int LArLATOMEDecoder::EventProcess::signEnergy(unsigned int energy){

  if( energy & (1<<17) )return energy-pow(2,18); else return energy;
}


unsigned int LArLATOMEDecoder::EventProcess::bytesPerChannel(MonDataType at0, MonDataType at1){
  unsigned int b=0;
  //// 2 bytes per stream and 1 bytes for sat/val if energy
  if(at0!= MonDataType::Invalid) b+=2;
  if(at1!= MonDataType::Invalid) b+=2;
  if(at0 == MonDataType::Energy || at1 == MonDataType::Energy ||
     at0 == MonDataType::SelectedEnergy || at1 == MonDataType::SelectedEnergy) ++b;
  return b;
}

void LArLATOMEDecoder::EventProcess::increaseWordShift(unsigned int& wordshift){
  ++wordshift;
  if(m_packetEnd[m_iPacket] == wordshift){
    ++m_iPacket;
    wordshift+=(m_monHeaderSize+m_monTrailerSize);
    m_decoder->msg(MSG::DEBUG) << "switch packet " << m_iPacket << "  "  <<  wordshift <<endmsg;
  }

}


void LArLATOMEDecoder::EventProcess::increaseByteShift(unsigned int& wordshift, unsigned int& byteshift){
  ++byteshift;
  if(byteshift==4){
    increaseWordShift(wordshift);
    byteshift=0;
  }
}

void LArLATOMEDecoder::EventProcess::decodeByte(unsigned int& byte, unsigned int wordshift, unsigned int byteshift, const uint32_t* p){
  byte = ((bswap_32(p[wordshift]))>>(8*(4-1-byteshift)))&0xff;
}

void LArLATOMEDecoder::EventProcess::decodeWord(unsigned int& word, unsigned int& wordshift, unsigned int& byteshift, const uint32_t* p){
  unsigned int msb=0;
  unsigned int lsb=0;
  decodeByte(msb, wordshift, byteshift, p);
  increaseByteShift(wordshift, byteshift);
  decodeByte(lsb, wordshift, byteshift, p);
  increaseByteShift(wordshift, byteshift);
  word = lsb | (msb<<8);
}

void LArLATOMEDecoder::EventProcess::decodeChannel(unsigned int& wordshift, unsigned int& byteshift, const uint32_t* p,
						   MonDataType at0, MonDataType at1,
						   unsigned int& at0Data, unsigned int& at1Data, unsigned int& satData,
						   bool& at0val, bool& at1val){

  // the structure of data is always consisting of 2,3,4 or 5 bytes depending on the recipe.
  // If there is an energy it means there is the quality/saturation byte thus odd number of bytes. Otherwise ADC or Energy spans <mostly> over 2 bytes.
  // avoid vectors and loops and bitsets to make it faster (can change later if needed but there is only few configurations so no need to make it infinitely flexible.
  // use unsigned int everywhere for now since we have 18 bits for energy which does not fit in short, cast later.
  unsigned int nbytesPerChannel = bytesPerChannel(at0, at1);
  bool satByte = nbytesPerChannel%2; // do we have satbyte

  at0val=false;
  at1val=false;
  at0Data=0;
  at1Data=0;
  satData=0;

  unsigned int word1=0;
  unsigned int word2=0;
  decodeWord(word1, wordshift, byteshift, p);
  if(nbytesPerChannel>3){
    decodeWord(word2, wordshift, byteshift, p);
  }
  at0Data = word1 & 0x7fff;
  at0val =  word1 & 0x8000;
  if(nbytesPerChannel>3){
    at1Data = word2 & 0x7fff;
    at1val =  word2 & 0x8000;
  }
  if(satByte){
    decodeByte(satData, wordshift, byteshift, p);
    increaseByteShift(wordshift, byteshift);
  }
  
  /// now that we have the basic words we can rearrange the bits in case it is energy, ADC do not need further arrangement
  /// this is assuming energy lsb is at bit[0-2] and energy-id at [5-7]. Make sure this is the case for all recipe in the firmware.
  /// assumptions are in https://its.cern.ch/jira/browse/LDPBFW-2976
  
  if(at0 == MonDataType::Energy && at1 == MonDataType::SelectedEnergy){
    at0Data = (at0Data<<3) | (satData&0x7);
    at1Data = (at1Data<<3) | ((satData&0x70)>>4);
    satData &= 0x88;
  }
  else if(at1 == MonDataType::Energy && at0 == MonDataType::SelectedEnergy){
    at0Data = (at0Data<<3) | ((satData&0x70)>>4);
    at1Data = (at1Data<<3) | (satData&0x7);
    satData &= 0x88;
  }
  else{
    if(at0 == MonDataType::Energy || at0 == MonDataType::SelectedEnergy){
      at0Data = (at0Data<<3) | (satData&0x7);
      satData &= 0xf8;
    }
    if(at1 == MonDataType::Energy || at1 == MonDataType::SelectedEnergy){
      at1Data = (at1Data<<3) | (satData&0x7);
      satData &= 0xf8;
    }
  }
     
}



void LArLATOMEDecoder::EventProcess::fillCollection(const ROBFragment* robFrag, const LArLATOMEMapping *map) {
  //Mon* mon = new Mon;

  /// some of this info should be used in the LatomeHeader class and for cross checks also (same as for the mon header)
  // const unsigned int rod_Size_words = robFrag->rod_ndata();
  // const unsigned int rob_Size_words = robFrag->payload_size_word();
  // const unsigned int sourceID = robFrag->rob_source_id();
  // const unsigned int rod_fragment_size_word = robFrag->rod_fragment_size_word();
  // const unsigned int rod_header_size_word = robFrag->rod_header_size_word();
  // const unsigned int rod_trailer_size_word = robFrag->rod_trailer_size_word();
  // const unsigned int rod_bc_id = robFrag->rod_bc_id();
  // const unsigned int rod_nstatus = robFrag->rod_nstatus();
  // const unsigned int rod_status_position = robFrag->rod_status_position();
  // const uint32_t* rod_start = robFrag->rod_start();
  
  m_l1ID = robFrag->rod_lvl1_id();
  const uint32_t* p = robFrag->rod_data();
  const unsigned int n = robFrag->rod_ndata();
  m_latomeBCID = robFrag->rod_bc_id();
  const uint32_t *rod_status =  robFrag->rod_status();
  const unsigned int rod_nstatus = robFrag->rod_nstatus();
  if(rod_nstatus != 27){
    m_decoder->msg(MSG::WARNING) << "Inconsistent number of rod header status elements: nstatus= " << rod_nstatus << endmsg;
    return;
  }
  if(rod_nstatus>8){
    uint32_t status8 = rod_status[8];
    m_at0type = status8 & 0x3;
    m_at1type = (status8>>2) & 0x3;
  }
  
  m_nthLATOME = robFrag->rod_source_id();
  const HWIdentifier hwidEmpty;

  ////lets first decode the mon header in the first packet and get the info.

  unsigned int offset = decodeHeader(p,0);
  m_packetEnd.push_back(offset);
  offset=decodeTrailer(p, offset);
  /// not we have the packet size from the first packet, check all packet headers before decoding
  /// we can decide later if we drop decoding if we have inconsistency
  for(unsigned int ip=1; ip<m_nPackets; ++ip){
    offset = decodeHeader(p,offset);
    m_packetEnd.push_back(offset);
    offset=decodeTrailer(p, offset);
  }
  

  m_decoder->msg(MSG::DEBUG) << " end of header check computed offset=" << std::dec
			     << offset << " nwords in payload=" << n << endmsg;

  /// OK all headers checked and we have all info we need to decode each packet, so lets start  
  m_iPacket=0;

  if(m_packetEnd[m_nPackets-1]+m_monTrailerSize != n){
    m_decoder->msg(MSG::WARNING) << "problem in packet size loop "
				 << m_packetEnd[m_nPackets-1] << " != " << n << endmsg;
  }


  std::vector<unsigned int> timeslot_nsc = {m_nsc1, m_nsc2, m_nsc3, m_nsc4, m_nsc5, m_nsc6};
  std::vector<unsigned int> bc_size;
  /// for now just getting the ADCs from AT0
  /// Change later to add energy as well and use the BC shift to determine at which BC there is energy

  short nBC=0;
  short nBC1=0;
  short startBC1=0;
  MonDataType type0 = MonDataType::Invalid;
  MonDataType type1 = MonDataType::Invalid;

  if(m_at0type!=(Word)MonDataType::Invalid && m_at1type!=(Word)MonDataType::Invalid){
    if(m_at0nBC>=m_at1nBC){
      nBC=m_at0nBC;
      nBC1=m_at1nBC;
      startBC1=m_at1BC-m_at0BC;
      type0=static_cast<MonDataType>(m_at0type);
      type1=static_cast<MonDataType>(m_at1type);
    }
    else{
      nBC1=m_at0nBC;
      nBC=m_at1nBC;
      startBC1=m_at0BC-m_at1BC;
      type1=static_cast<MonDataType>(m_at0type);
      type0=static_cast<MonDataType>(m_at1type);
    }
  }
  else if(m_at0type!=(Word)MonDataType::Invalid){
      nBC=m_at0nBC;
      type0=static_cast<MonDataType>(m_at0type);
  }
  else if(m_at1type!=(Word)MonDataType::Invalid){
    nBC=m_at1nBC;
    type0=static_cast<MonDataType>(m_at1type);
  }
  else{
    m_decoder->msg(MSG::ERROR) << "No valid data type in Mon Header"<< endmsg;
    return;
  }
  
  switch(type0){
  case MonDataType::RawADC:
    m_hasRawAdc=true;
    m_nBC_rawADC=nBC;
    break;
  case MonDataType::ADC:
    m_hasAdc=true;
    m_nBC_ADC=nBC;
    break;
  case MonDataType::Energy:
    m_hasE=true;
    m_nBC_E=nBC;
    break;
  case MonDataType::SelectedEnergy:
    m_hasEID=true;
    m_nBC_EID=nBC;
    break;
  case MonDataType::Invalid:
    break;
  }

  switch(type1){
  case MonDataType::RawADC:
    m_hasRawAdc=true;
    m_nBC_rawADC=nBC1;
    m_BC_rawADC=startBC1;
    break;
  case MonDataType::ADC:
    m_hasAdc=true;
    m_nBC_ADC=nBC1;
    m_BC_ADC=startBC1;
    break;
  case MonDataType::Energy:
    m_hasE=true;
    m_nBC_E=nBC1;
    m_BC_E=startBC1;
    break;
  case MonDataType::SelectedEnergy:
    m_hasEID=true;
    m_nBC_EID=nBC1;
    m_BC_EID=startBC1;
    break;
  case MonDataType::Invalid:
    break;
  }
  
  for(auto &val: m_rawValuesInEvent){
    if(m_hasRawAdc)val.adc.resize(m_nBC_rawADC);
    if(m_hasAdc)val.adc_bas.resize(m_nBC_ADC);
    if(m_hasE)val.et.resize(m_nBC_E);
    if(m_hasEID)val.et_id.resize(m_nBC_EID);
    val.latomeChannel = 99999;
  }
  m_BCIDsInEvent.resize(nBC);
  
  unsigned int s = m_monHeaderSize; /// lets start from here
  unsigned int bcid=s_nBunches;

  for(short iBC=0;iBC<nBC; ++iBC){
    MonDataType at0=type0;
    MonDataType at1=type1;
    if(type1 != MonDataType::Invalid){
      if(iBC<startBC1 || iBC>=startBC1+nBC1)at1=MonDataType::Invalid;
    }
    unsigned int nbytesPerChannel = bytesPerChannel(at0,at1);
  
    int nsc=0;
    unsigned int oldipacket=0;
    for(unsigned int itimeslot=0; itimeslot<6; ++itimeslot){

      unsigned int l_bcid = (bswap_32(p[s]))>>16;
      if(itimeslot!=0){
	if(l_bcid!=bcid){
	  m_decoder->msg(MSG::WARNING) << "ERROR: inconsistent BCID between time slots" << endmsg;
	}
      }
      else{
	if(bcid!=s_nBunches){ /// start of packet, bcid still unvalid
	  //// should increase by one but take care of rotation at s_nBunches;
	  unsigned int bcid_c = bcid+1;
	  if(bcid_c==s_nBunches){
            bcid=0;
	    bcid_c = 0;
	  }
	  if(bcid_c != l_bcid){
	   m_decoder->msg(MSG::WARNING) << "ERROR: BCID not increasing properly between samples, L1ID is: " << m_l1ID << ", BCID is from payload: " << l_bcid << ", expected BCID is: " << bcid_c << ", LATOME channel is: " << nsc << endmsg;
	  }
	}
	m_BCIDsInEvent[iBC] = l_bcid;
      }
      bcid=l_bcid;

      unsigned int mux = ((bswap_32(p[s]))>>8)&0xff;
      increaseWordShift(s);
      //// skip trailing 0
      increaseWordShift(s);
      if(s>=n) break;

  	
      unsigned int timeslotsize = timeslot_nsc[itimeslot];
      unsigned int nbytes = timeslotsize*nbytesPerChannel;
      unsigned int n64word= nbytes/8;  /// the data is packed with 64 bits words. Will be padded with 0s at the end of the timeslot time slot.
      if(nbytes%8) ++n64word;
      m_decoder->msg(MSG::DEBUG) << " at BC " << iBC<< " timeslot " <<itimeslot
				 << "  " << bcid << "  " << mux << " n64word " <<n64word
				 << " at0 " << (int)at0 << " at1 " << (int)at1 << " l_bcid " <<bcid  << endmsg;
      
      unsigned int wordshift=s;
      unsigned int byteshift=0;

      //// for the padding consistency check, need to know if we skipped a packet.
      oldipacket=m_iPacket;
      for( unsigned int ichan=0; ichan<timeslotsize; ++ichan){
	unsigned int at0Data=0, at1Data=0, satData=0;
	bool at0val=false, at1val=false;
		
	decodeChannel(wordshift, byteshift, p, at0, at1,
		      at0Data, at1Data, satData, at0val, at1val);
	m_decoder->msg(MSG::DEBUG) << " wordshift " <<  wordshift << " byteshift " <<byteshift
				   <<  " at0data " << at0Data << " at1Data " << at1Data << " satData "
				   << satData << " at0val " << at0val << " at1val " << at1val
				   << " nsc " << nsc << endmsg;
	
	/// lets fill now, using the older code structure but this should be change to support having energy.
	const auto SCID = map ? map->getChannelID(m_nthLATOME, nsc):hwidEmpty;
	if (SCID == hwidEmpty) {
	  m_decoder->msg(MSG::DEBUG) << "No mapping for ch: " << std::dec << nsc << endmsg;
	}
	int RAWValue0 = -999;
	if (!at0val && SCID != hwidEmpty && at0!=MonDataType::Invalid) { 
	  m_decoder->msg(MSG::INFO) << "at0 bad quality bit for SC:" << nsc << " BC " << iBC
				       << " latome " << robFrag->rod_source_id() << endmsg;
	  RAWValue0 = -1; 
	} else{
	  //// no need to reinterpret the bits. for ADC there will be an implicit case to short
	  RAWValue0 = at0Data;
	}
	switch(at0){
	case MonDataType::RawADC:
	  m_rawValuesInEvent[nsc].adc[iBC] = RAWValue0;
	  break;
	case MonDataType::ADC:
	  m_rawValuesInEvent[nsc].adc_bas[iBC] = RAWValue0;
	  break;
	case MonDataType::Energy:
	  m_rawValuesInEvent[nsc].et[iBC] = signEnergy(RAWValue0);
	  break;
	case MonDataType::SelectedEnergy:
	  m_rawValuesInEvent[nsc].et_id[iBC] = signEnergy(RAWValue0);
	  break;
	case MonDataType::Invalid:
	  break;
	}
    

	int RAWValue1 = -999;
	if (!at1val && SCID != hwidEmpty && at1!=MonDataType::Invalid) { 
	  m_decoder->msg(MSG::INFO) << "at1 bad quality bit for SC:" << nsc << " BC " << iBC
				       << " latome " << robFrag->rod_source_id() << endmsg;
	  RAWValue1 = -1; 
	}else{
	  //// no need to reinterpret the bits. for ADC there will be an implicit case to short
	  RAWValue1 = at1Data;
	}
	switch(at1){
	case MonDataType::RawADC:
	  m_rawValuesInEvent[nsc].adc[iBC-startBC1] = RAWValue1;
	  break;
	case MonDataType::ADC:
	  m_rawValuesInEvent[nsc].adc_bas[iBC-startBC1] = RAWValue1;
	  break;
	case MonDataType::Energy:
	  m_rawValuesInEvent[nsc].et[iBC-startBC1] = signEnergy(RAWValue1);
	  break;
	case MonDataType::SelectedEnergy:
	  m_rawValuesInEvent[nsc].et_id[iBC-startBC1] = signEnergy(RAWValue1);
	  break;
	case MonDataType::Invalid:
	  break;
	}
	
	
	m_rawValuesInEvent[nsc].latomeChannel=nsc;
	
	++nsc;
	  
      } /// Loop over SC

	//// lets make sure the 0 padding is correct
      if((wordshift-s)%2) increaseWordShift(wordshift);
      if( (wordshift-s-((m_iPacket-oldipacket)*(m_monHeaderSize+m_monTrailerSize))) != n64word*2){
	m_decoder->msg(MSG::WARNING) << " ERROR: time slice end is not padded properly " << (wordshift-s-m_iPacket*(m_monHeaderSize+m_monTrailerSize)) << "!=" << n64word*2  <<  " m_ipacket " << endmsg;
      }
      s=wordshift;
      oldipacket=m_iPacket;
    } /// Loop over time slots 

  
  } /// Loop over BC

  
  fillRaw(map);
  fillHeader();
}

// Pass ADC values from an event
void LArLATOMEDecoder::EventProcess::fillRaw(const LArLATOMEMapping *map) {
  //const CaloGain::CaloGain dummyGain = CaloGain::LARHIGHGAIN;
  const HWIdentifier hwidEmpty;
  for (SuperCell ch = 0; ch < N_LATOME_CHANNELS; ++ch) {
    const auto SCID = map ? map->getChannelID(m_nthLATOME, ch):hwidEmpty;
    if (SCID == hwidEmpty) {
      m_decoder->msg(MSG::DEBUG) << "No mapping for ch: " << std::dec << ch << endmsg;
      continue;
    }
    std::vector<short> adcValues_inChannel_inEvent;

    if(m_hasRawAdc){ /// don't copy vectors for nothing
      adcValues_inChannel_inEvent=m_rawValuesInEvent[ch].adc;
    }
   
    if(m_hasRawAdc && m_adc_coll){
      /// need to shift the BCID as well
      /// do it at the same time
      
      std::vector<unsigned short> bcid_in_event;
      for(short b=m_BC_rawADC; b<m_BC_rawADC+m_nBC_rawADC; ++b){
	bcid_in_event.push_back(m_BCIDsInEvent[b]);
      }
      
      LArSCDigit* scDigit = new LArSCDigit(SCID, m_rawValuesInEvent[ch].latomeChannel,m_nthLATOME,
					   adcValues_inChannel_inEvent, bcid_in_event);
      m_adc_coll->push_back(scDigit);
    }

    if(m_hasAdc && m_adc_bas_coll){
      std::vector<unsigned short> bcid_in_event;
      if(m_nBC_ADC==(short)m_BCIDsInEvent.size()){
	bcid_in_event=m_BCIDsInEvent;
      }
      else{
	for(short b=m_BC_ADC; b<m_BC_ADC+m_nBC_ADC; ++b){
	  bcid_in_event.push_back(m_BCIDsInEvent[b]);
	}
      }
      LArSCDigit* scDigit = new LArSCDigit(SCID, m_rawValuesInEvent[ch].latomeChannel,m_nthLATOME,
					   m_rawValuesInEvent[ch].adc_bas, bcid_in_event);
      m_adc_bas_coll->push_back(scDigit);

    }

    if(m_hasE && m_et_coll){
      std::vector<unsigned short> bcid_in_event;
      std::vector<bool> satur;
      if(m_nBC_E==(unsigned short)m_BCIDsInEvent.size()){
	bcid_in_event=m_BCIDsInEvent;
      }
      else{
	for(short b=m_BC_E; b<m_BC_E+m_nBC_E; ++b){
	  bcid_in_event.push_back(m_BCIDsInEvent[b]);
	  satur.push_back(false);
	}
      }
      LArRawSC* scDigit = new LArRawSC(SCID, m_rawValuesInEvent[ch].latomeChannel,m_nthLATOME,
				       m_rawValuesInEvent[ch].et, bcid_in_event, satur);
      m_et_coll->push_back(scDigit);

    }

    if(m_hasEID && m_et_id_coll){
      std::vector<unsigned short> bcid_in_event;
      std::vector<bool> satur;
      if(m_nBC_EID==(short)m_BCIDsInEvent.size()){
	bcid_in_event=m_BCIDsInEvent;
      }
      else{
	for(short b=m_BC_EID; b<m_BC_EID+m_nBC_EID; ++b){
	  bcid_in_event.push_back(m_BCIDsInEvent[b]);
	  satur.push_back(false);
	}
      }
      LArRawSC* scDigit = new LArRawSC(SCID, m_rawValuesInEvent[ch].latomeChannel,m_nthLATOME,
				       m_rawValuesInEvent[ch].et_id, bcid_in_event,satur);
      m_et_id_coll->push_back(scDigit);

    }

  }
  
}


void LArLATOMEDecoder::EventProcess::fillHeader() {

  if(m_header_coll){
    LArLATOMEHeader* latome = new LArLATOMEHeader(m_nthLATOME, m_latomeID,  m_activeSC, m_latomeBCID, m_l1ID);
    m_header_coll->push_back(latome);
  }
  
}
