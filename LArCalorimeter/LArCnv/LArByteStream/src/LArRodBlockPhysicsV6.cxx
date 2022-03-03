//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of a LArRODBlockStructure class
// This version contains LArDigits in fixed gain.
// See .h file for more details.

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
//#include <cstdio>
#include "LArRawEvent/LArDigit.h"
#include "LArByteStream/LArRodBlockPhysicsV6.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include <iostream>

//#define LARBSDBGOUTPUT
#ifdef LARBSDBGOUTPUT
#define MYLEVEL (MSG::FATAL)
#define LARBSDBG(text) logstr<<MYLEVEL<<text<<endmsg
#else
#define LARBSDBG(text)
#endif

//int mycheck_tot=0;
//int mycheck_err=0;

namespace {
union ShortLong {
  uint16_t s[2];
  uint32_t l;
};
}

LArRodBlockPhysicsV6::LArRodBlockPhysicsV6()
  : LArRodBlockStructure(),
    m_onlineHelper(nullptr)
{
  m_iHeadBlockSize=endtag/2; // The implicit cast rounds down to the right size 
  m_fixedGain=CaloGain::LARNGAIN;
  LArRodBlockPhysicsV6::resetPointers();
  m_EnergyThreshold1=100;
  m_EnergyThreshold2=0;
  m_requiredNSamples = 0;
  m_OffTimeCut=0;  //FIXME: Nowhere set to a sensible value ???
  m_numberHotCellOffTime=0;
  // retrieve onlineHelper
  //const LArOnlineID* online_id;
  StoreGateSvc* detStore = 0;
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StatusCode sc =svcLoc->service( "DetectorStore", detStore );
  if (sc.isFailure()) {
    std::cout << "Unable to locate DetectorStore" << std::endl;
    std::abort();
  }
  sc = detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    std::cout << "Could not get LArOnlineID helper !" << std::endl;
    std::abort();
  }
}


void LArRodBlockPhysicsV6::resetPointers() 
{
  m_EnergyIndex=0;
  m_TimeQualityIndex=0;
  m_DigitsIndex=0;
  m_DigitsChannel=0;
  m_RawDataIndex=0;

  m_GainPointer=0;
  m_MaskTimeQualityPointer=0;
  m_MaskDigitsPointer=0;
  m_RaddPointer=0;
  m_EnergyPointer=0;
  m_SumPointer=0;
  m_TimeQualityPointer=0;
  m_DigitsPointer=0;
  m_RawDataPointer=0;
  m_numberHotCell=0;
}

bool LArRodBlockPhysicsV6::setPointers()
{
  if (m_FebBlockSize>m_iHeadBlockSize)
    {
      int off = -8;
      int ns   = getHeader16(NSamples) & 0xff;
      if (m_requiredNSamples > 0 && m_requiredNSamples != ns) return false;
      int radd = (ns+1)/2;
      int dim1 = getHeader16(ResultsDim1);
      int off1 = getHeader16(ResultsOff1);
      int off2 = getHeader16(ResultsOff2);
      int dim2 = getHeader16(ResultsDim2);
      int off3 = getHeader16(RawDataBlkOff);
      int dim3 = getHeader16(RawDataBlkDim);
      if ( !(ns&0x1) ) radd++;

      if (off1 && dim1+off1+off<m_FebBlockSize) {
	off1 += off;
	if (dim1>=8) 
	  m_GainPointer=(const uint32_t*)(m_FebBlock+off1);
	if (dim1>=12) 
	  m_MaskTimeQualityPointer=(const uint32_t*)(m_FebBlock+off1+8);
	if (dim1>=16) 
	  m_MaskDigitsPointer=(const uint32_t*)(m_FebBlock+off1+12);
	if (dim1>=16+radd) 
	  m_RaddPointer=(const uint16_t*)(m_FebBlock+off1+16);
	if (dim1>=80+radd)
	  m_EnergyPointer=(const uint16_t*) (m_FebBlock+off1+16+radd);
	if (dim1>=84+radd) 
	  m_SumPointer=(const int32_t*)(m_FebBlock+off1+80+radd);
	if (dim1>84+radd) 
	  m_TimeQualityPointer=(const uint16_t*)(m_FebBlock+off1+84+radd);
	off1 -= off;
      }
      if (off2 && dim2+off2+off<m_FebBlockSize) {
	m_DigitsPointer=(const uint16_t*)(m_FebBlock+off2+off);
      }
      if (off3 && dim3+off3+off<m_FebBlockSize) {
	m_RawDataPointer=(const uint16_t*)(m_FebBlock+off3+off);
      }

      // Check for offsets problems
      uint32_t problem = 0;
      int n1, n2;
      int n1_tmp, n2_tmp;
      int off1_tmp, dim1_tmp;
      int off2_tmp, dim2_tmp;
      int off3_tmp, dim3_tmp;
      if(off1==0) {
	n1 = n2 = 0;
	n1_tmp = n2_tmp =0;
	off1_tmp = dim1_tmp = 0; 
	off2_tmp = dim2_tmp = 0; 
	off3_tmp = dim3_tmp = 0; 
      }
      else { 
	m_RaddPointer=(const uint16_t*)(m_FebBlock+26);
	m_MaskTimeQualityPointer=(const uint32_t*)(m_FebBlock+18);
	m_MaskDigitsPointer=(const uint32_t*)(m_FebBlock+22);
	n1 = getNbSweetCells1();
	n2 = getNbSweetCells2();
	n1_tmp = getNbSweetCells1FromMask();
	n2_tmp = getNbSweetCells2FromMask();
	off1_tmp = 10-off; 
	dim1_tmp = 84+(ns+1)/2+n1; 
        if ( !(ns&0x1) ) dim1_tmp++;
	if ( m_requiredNSamples > 0 ){
	    dim1_tmp = 84 +(m_requiredNSamples+1)/2+n1;
            if ( !(m_requiredNSamples&0x1) ) dim1_tmp++;
	}
	off2_tmp = off1_tmp+dim1_tmp;
	dim2_tmp = (n2*ns+1)/2;
	off3_tmp = off2_tmp+dim2_tmp;
	dim3_tmp = getNumberOfWords()-3-off3_tmp-off;
	if(dim2_tmp==0) off2_tmp = 0;
	if(dim3_tmp==0) off3_tmp = 0;
      }

      if(off1 != off1_tmp) problem=1;
      if(dim1 != dim1_tmp) problem=2;
      if(off2 != off2_tmp) problem=3;
      if(dim2 != dim2_tmp) problem=4;
      if(off3 != off3_tmp) problem=5;
      if(dim3 != dim3_tmp) problem=6;
      if(n1 != n1_tmp)     problem=7;
      if(n2 != n2_tmp)     problem=8;
      if (m_requiredNSamples > 0 && 
          getHeader32(NGains) != (uint32_t)0x10000 + m_requiredNSamples) problem=9;
      //if(getHeader32(NGains)!=0x10000 + (unsigned int)ns) problem=9;
      //if(getHeader32(InFPGAFormat)!=1) problem=10;
      //if(m_FebBlock[getNumberOfWords()-2]!=0x12345678) problem=11;

      if(problem) { // Try to recompute offsets
	std::cout << "LArByteStreamProblem " << problem << std::endl;
        std::cout << "NSamples = " << std::dec << ns << std::endl;
        std::cout << "getHeader32(NGains) = " << std::hex << getHeader32(NGains) << std::endl;
	std::cout << "NWTot:   " << std::hex  << getNumberOfWords() << " n1=" << n1 << " (" << n1_tmp << ") n2=" << n2 << " (" << n2_tmp << ")" << std::endl;
	std::cout << "Found 1: " << off1 << " " << dim1 << std::endl;
	std::cout << "Found 2: " << off2 << " " << dim2 << std::endl;
	std::cout << "Found 3: " << off3 << " " << dim3 << std::dec << std::endl;
	
	if(n1==n1_tmp && n2==n2_tmp) { 	// Check consistency of cells above threshold
	  off1 = off1_tmp;
	  dim1 = dim1_tmp;
	  off2 = off2_tmp;
	  dim2 = dim2_tmp;
	  off3 = off3_tmp;
	  dim3 = dim3_tmp;
	  std::cout << "Recomputed 1: " << std::hex << off1 << " " << dim1 << std::endl;
	  std::cout << "Recomputed 2: " << off2 << " " << dim2 << std::endl;
	  std::cout << "Recomputed 3: " << off3 << " " << dim3 << std::dec << std::endl;

	  if (off1 && dim1+off1+off<m_FebBlockSize) {
	    off1 += off;
	    if (dim1>=8) 
	      m_GainPointer=(const uint32_t*)(m_FebBlock+off1);
	    if (dim1>=12) 
	      m_MaskTimeQualityPointer=(const uint32_t*)(m_FebBlock+off1+8);
	    if (dim1>=16) 
	      m_MaskDigitsPointer=(const uint32_t*)(m_FebBlock+off1+12);
	    if (dim1>=16+radd) 
	      m_RaddPointer=(const uint16_t*)(m_FebBlock+off1+16);
	    if (dim1>=80+radd)
	      m_EnergyPointer=(const uint16_t*) (m_FebBlock+off1+16+radd);
	    if (dim1>=84+radd) 
	      m_SumPointer=(const int32_t*)(m_FebBlock+off1+80+radd);
	    if (dim1>84+radd) 
	      m_TimeQualityPointer=(const uint16_t*)(m_FebBlock+off1+84+radd);
	  }
	  if (off2 && dim2+off2+off<m_FebBlockSize) {
	    m_DigitsPointer=(const uint16_t*)(m_FebBlock+off2+off);
	  }
	  if (off3 && dim3+off3+off<m_FebBlockSize) {
	    m_RawDataPointer=(const uint16_t*)(m_FebBlock+off3+off);
	  }
	}
      }

      problem=0;
      // Recheck offsets
      if(off1< off2 && off1 + dim1 > off2) problem = 1;
      if(off1< off3 && off1 + dim1 > off3) problem = 2;
      if(off2< off1 && off2 + dim2 > off1) problem = 3;
      if(off2< off3 && off2 + dim2 > off3) problem = 4;
      if(off3< off1 && off3 + dim3 > off1) problem = 5;
      if(off3< off2 && off3 + dim3 > off2) problem = 6;

      if(problem) {
	resetPointers();
	std::cout << "LArByteStreamProblem " << problem << std::endl;
	std::cout << "Unrecoverable problem" << std::endl;
      }
	
      //uint32_t febId    = getHeader32(FEBID);
      //uint32_t onCheck  = onlineCheckSum();
      //uint32_t offCheck = offlineCheckSum();
      //
      //mycheck_tot++;
      //if(onCheck!=offCheck) 
      //{
      //	mycheck_err++;
      //	std::cout << "FebID  checksum " << std::hex << febId << std::endl;
      //	std::cout << "Online checksum " << std::hex << onCheck << " Offline checksum " << offCheck << std::dec << std::endl;
      //	std::cout << "Diff1  checksum " << std::hex << onCheck-offCheck << " Diff2   checksum " << offCheck-onCheck << std::dec << std::endl;
      //	double x=mycheck_err/((double) mycheck_tot)*100.0;
      //	std::cout << "Number of FEB in error:       " << mycheck_err << " / " << mycheck_tot << " = " << x << " %" << std::endl;
      //}

      //
      //if(febId==0x3b1b8000 || febId==0x398b0000 || febId==0x3a988000) {
      //if(onCheck!=offCheck) {
      //	std::cout  << "***********************************************************************"<< std::endl;
      //	std::cout  << "Problem :" << problem << std::endl;
      //	std::cout  << "Header values :"<< std::endl;
      //	std::cout  << "************************************************************************"<< std::endl;
      //	std::cout  << "FebBlockSize              = " <<  m_FebBlockSize              << std::endl;
      //	std::cout  << "EnergyIndex            = " <<  m_EnergyIndex            << std::endl;
      //	std::cout  << "TimeQualityIndex       = " <<  m_TimeQualityIndex       << std::endl;
      //	std::cout  << "DigitsIndex            = " <<  m_DigitsIndex            << std::endl;
      //	std::cout  << "DigitsChannel          = " <<  m_DigitsChannel          << std::endl;
      //	std::cout  << "RawDataIndex           = " <<  m_RawDataIndex           << std::endl;
      //	std::cout  << "GainPointer            = " <<  m_GainPointer            << std::endl;
      //	std::cout  << "MaskTimeQualityPointer = " <<  m_MaskTimeQualityPointer << std::endl;
      //	std::cout  << "MaskDigitsPointer      = " <<  m_MaskDigitsPointer      << std::endl;
      //	std::cout  << "RaddPointer            = " <<  m_RaddPointer            << std::endl;
      //	std::cout  << "EnergyPointer          = " <<  m_EnergyPointer          << std::endl;
      //	std::cout  << "SumPointer             = " <<  m_SumPointer             << std::endl;
      //	std::cout  << "TimeQualityPointer     = " <<  m_TimeQualityPointer     << std::endl;
      //	std::cout  << "DigitsPointer          = " <<  m_DigitsPointer          << std::endl;
      //	std::cout  << "RawDataPointer         = " <<  m_RawDataPointer         << std::endl;
      //	std::cout  << "numberHotCell          = " <<  std::dec << getNbSweetCells1() << " " << getNbSweetCells2() << std::endl;
      //	std::cout  << "Fragment @    = 0x" << std::hex << m_FebBlock << std::endl;
      //	std::cout  << "NWTot         =   " << std::dec << getNumberOfWords() << std::endl;
      //	std::cout  << "FebID         = 0x" << std::hex << getHeader32(FEBID) << std::endl;
      //	std::cout  << "FebSN         = 0x" << std::hex << getHeader32(FEB_SN) << std::endl;
      //	std::cout  << "ResultsOff1   = 0x" << std::hex << getHeader16(ResultsOff1) << std::endl;
      //	std::cout  << "ResultsDim1   = 0x" << std::hex << getHeader16(ResultsDim1) << std::endl;
      //	std::cout  << "ResultsOff2   = 0x" << std::hex << getHeader16(ResultsOff2) << std::endl;
      //	std::cout  << "ResultsDim2   = 0x" << std::hex << getHeader16(ResultsDim2) << std::endl;
      //	std::cout  << "RawDataBlkOff = 0x" << std::hex << getHeader16(RawDataBlkOff) << std::endl;
      //	std::cout  << "RawDataBlkDim = 0x" << std::hex << getHeader16(RawDataBlkDim) << std::endl;
      //	std::cout  << "Event status  = 0x" << std::hex << getStatus() << std::dec << std::endl;
      //	std::cout  << "************************************************************************"<< std::dec << std::endl;
      //	int size = getNumberOfWords();
      //	for(int i=0;i<size;i++) {
      //	  std::cout << std::hex << i << " : " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
      //	}
      //}

   }

  return true;
}

int LArRodBlockPhysicsV6::getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain)
{
#ifdef LARBSDBGOUTPUT
  MsgStream logstr(Athena::getMessageSvc(), BlockType());
  //Debug output
  logstr << MYLEVEL << "Let s go in getNextRawData..." << endmsg;
  logstr << MYLEVEL << "GetNextRawData for FEB 0x" << MSG::hex << (uint32_t)getHeader32(FEBID) << MSG::dec << endmsg;
  logstr << MYLEVEL << "m_RawDataPointer=" << m_RawDataPointer << " m_RawDataIndex="<<  m_RawDataIndex 
	 << " m_channelsPerFEB=" << m_channelsPerFEB << endmsg;
#endif

  if (m_RawDataIndex>=m_channelsPerFEB) { //Already beyond maximal number of channels
#ifdef LARBSDBGOUTPUT
    logstr << MYLEVEL << "Maximum number of channels reached" << endmsg;
#endif
    return 0;
  }
  //const uint16_t block = getHeader16(m_RawDataOff);//Position of the raw FEB data block
  if (!m_RawDataPointer) { //Block does not exist
    // Try to get samples and gain from getNextDigits
    return getNextDigits(channelNumber,samples,gain);
  }

  // Get next channel
  unsigned rodChannelNumber=m_RawDataIndex;      // Index of Channel in ROD-Block
  channelNumber=((rodChannelNumber&0xe)<<2) + ((rodChannelNumber&0x1)<<6) + (rodChannelNumber>>4);    //channel number of the FEB
  //channelNumber=(rodChannelNumber>>4) + ((rodChannelNumber&0xf)<<3);    //channel number of the FEB
  uint32_t febgain;
  const unsigned int nsamples = getHeader16(NSamples) & 0xff;
  const unsigned int ngains   = getHeader16(NGains);

#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL << "This FEB has " << nsamples <<  " samples" << endmsg;
  logstr << MYLEVEL << "This FEB has " << ngains   <<  " gains" << endmsg;
#endif

  if(ngains==0 || nsamples==0) return 0;
  int s_size = nsamples+1;
  int offset = (10+nsamples)&0xfffc;
  int index;
  index = s_size*m_RawDataIndex + offset;
  uint16_t s[2];
  //for(unsigned int i=0;i<nsamples+1;i++) {
  //  if(m_RawDataPointer[index+i]>>14) {
  //  std::cout << "Trying to decode strange raw data value: " << std::hex << m_RawDataPointer[index+i] << std::dec << std::endl;
  // }
  //}
  if((nsamples+1)&0x7) {
     s[0]    = m_RawDataPointer[index++]>>2;
     febgain = m_RawDataPointer[index++];
     samples.push_back(s[0]);
     for(unsigned int i=0;i<nsamples/2;i++) {
      s[1]    = m_RawDataPointer[index++]>>2;
      s[0]    = m_RawDataPointer[index++]>>2;
      samples.push_back(s[0]);
      samples.push_back(s[1]);
     }
   } // End of check for 5 samples
   else {
  if (!(m_RawDataIndex%2)) {
  s[0]    = m_RawDataPointer[index++]>>2;
  febgain = m_RawDataPointer[index++];
  samples.push_back(s[0]);
  for(unsigned int i=0;i<nsamples/2;i++) {
    s[1]    = m_RawDataPointer[index++]>>2;
    s[0]    = m_RawDataPointer[index++]>>2;
    samples.push_back(s[0]);
    samples.push_back(s[1]);
  }
  } else {
  for(unsigned int i=0;i<nsamples/2;i++) {
    s[1]    = m_RawDataPointer[index++]>>2;
    s[0]    = m_RawDataPointer[index++]>>2;
    samples.push_back(s[0]);
    samples.push_back(s[1]);
  }
  febgain = m_RawDataPointer[index++];
  s[0]    = m_RawDataPointer[index++]>>2;
  samples.push_back(s[0]);
  }
  } // End of >5 check
  gain=RawToOfflineGain(febgain);

#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL << " ===> ROD Channel = " << m_RawDataIndex << endmsg; 
  logstr << MYLEVEL << " ===> FEB Channel = " << channelNumber << endmsg; 
  logstr << MYLEVEL << " ===> Gain        = " << gain << endmsg;
  for(int i=0;i<nsamples;i++)
    logstr << MYLEVEL << " ===> sample " << i << "    = " << samples[i] << endmsg;
  int n = m_RawDataIndex;
  int32_t e,t,q;
  uint32_t g;
  LArRodBlockPhysicsV6::getNextEnergy(n,e,t,q,g);
#endif
  //std::cout << "Gain= " << gain << " Febgain=" << febgain << std::endl;
  ++m_RawDataIndex;
  unsigned  rearrangeFirstSample=0;
  if (m_rearrangeFirstSample)
    rearrangeFirstSample=m_rearrangeFirstSample; //Overwrite by jobOptions
  else
    rearrangeFirstSample=getFirstSampleIndex();
  //std::cout << "FebConfig: "<< getFebConfig() << " FirstSampleIndex " << getFirstSampleIndex() <<std::endl;
  if (rearrangeFirstSample && rearrangeFirstSample<samples.size()) //FIXME: Very ugly hack! See explanation in LArRodDecoder.h file
      {//Change e.g. 3 0 1 2 4 to 0 1 2 3 4 
	short movedSample=samples[0];
	for (unsigned i=1;i<=rearrangeFirstSample;i++)
	  samples[i-1]=samples[i];
	samples[rearrangeFirstSample]=movedSample;
   }
#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL << "GetNextRawData for FEB finished 0x" << MSG::hex << (uint32_t)getHeader32(FEBID) << MSG::dec << endmsg;
#endif
  return 1;
}

int LArRodBlockPhysicsV6::getNextDigits(int& channelNumber, std::vector<short>& samples, uint32_t& gain)
{
  //std::cout << " I am here !!!!!!!!!!!!!!!!!!!!!! " << std::endl;
#ifdef LARBSDBGOUTPUT
  MsgStream logstr(Athena::getMessageSvc(), BlockType());
  //Debug output
  logstr << MYLEVEL << "Let s go in getNextDigits..." << endmsg;
  logstr << MYLEVEL << "GetNextDigits for FEB 0x" << MSG::hex << (uint32_t)getHeader32(FEBID) << MSG::dec << endmsg;
  logstr << MYLEVEL << "m_DigitsPointer=" << m_DigitsPointer << " m_DigitsIndex="<<  m_DigitsIndex 
	 << " m_DigitsChannel="<<  m_DigitsChannel 
	 << " m_channelsPerFEB=" << m_channelsPerFEB << endmsg;
#endif

  if (m_DigitsChannel>=m_channelsPerFEB) { //Already beyond maximal number of channels
#ifdef LARBSDBGOUTPUT
    logstr << MYLEVEL << "Maximum number of channels reached" << endmsg;
#endif
    return 0;
  }
  //const uint16_t block = getHeader16(m_DigitsOff);//Position of the raw FEB data block
  if (!m_DigitsPointer) { //Block does not exist
#ifdef LARBSDBGOUTPUT
    logstr << MYLEVEL << "No Digits Block in this FEB" << endmsg;
#endif
    return 0; 
  }
  if (!m_MaskDigitsPointer) { //Block does not exist
#ifdef LARBSDBGOUTPUT
    logstr << MYLEVEL << "No Mask Digits Block in this FEB" << endmsg;
#endif
    return 0; 
  }

  // Get Digits if the information is present according to summary block
  uint32_t hasDigits;

  hasDigits = (uint32_t) ((m_MaskDigitsPointer[m_DigitsChannel>>5] >> (m_DigitsChannel&0x1f)) &0x1);
  // Increment channel number until digits are found
  while(hasDigits==0) {
    m_DigitsChannel++;
    if (m_DigitsChannel>=m_channelsPerFEB) { //Already beyond maximal number of channels
#ifdef LARBSDBGOUTPUT
      logstr << MYLEVEL << "Maximum number of channels reached" << endmsg;
#endif
      return 0;
    }
    hasDigits = (uint32_t) ((m_MaskDigitsPointer[m_DigitsChannel>>5] >> (m_DigitsChannel&0x1f)) &0x1);
  }

  // Get next channel
  unsigned rodChannelNumber=m_DigitsChannel;      // Index of Channel in ROD-Block
  channelNumber=((rodChannelNumber&0xe)<<2) + ((rodChannelNumber&0x1)<<6) + (rodChannelNumber>>4);    //channel number of the FEB
  //channelNumber=(rodChannelNumber>>4) + ((rodChannelNumber&0xf)<<3);    //channel number of the FEB
  const unsigned int nsamples = getHeader16(NSamples) & 0xff;

  // gain in 2 bits of a 32 bits word 
  if(m_GainPointer) {
    gain = (uint32_t) ((m_GainPointer[m_DigitsChannel>>4] >> (m_DigitsChannel&0xf)*2) & 0x3);
    gain=RawToOfflineGain(gain);
  } else gain=0xffffffff;

#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL << "This FEB has " << nsamples <<  " samples" << endmsg;
#endif

  if(nsamples==0) return 0;
  int s_size = nsamples;
  int index;
  index = s_size*m_DigitsIndex;
  //uint16_t s;
  //for(unsigned int i=0;i<nsamples;i++) {
  //  s = m_DigitsPointer[index++]>>2;
  //  samples.push_back(s);
  //}
  //int ok=1;
  //for(unsigned int i=0;i<nsamples;i++) {
  //  if(m_DigitsPointer[index+i]>>14 && m_DigitsIndex<getNbSweetCells2()-1) {
  //    std::cout << "Trying to decode strange digits value: " << std::hex << m_DigitsPointer[index+i] << std::dec << std::endl;
  //    ok=0;
  //  }
  //}
  if( nsamples&0x1){
  if(m_DigitsIndex&0x1) {
    samples.push_back(m_DigitsPointer[index-1]>>2);
    samples.push_back(m_DigitsPointer[index+2]>>2);
    samples.push_back(m_DigitsPointer[index+1]>>2);
    samples.push_back(m_DigitsPointer[index+4]>>2);
    samples.push_back(m_DigitsPointer[index+3]>>2);
    if(nsamples==7) {
      samples.push_back(m_DigitsPointer[index+6]>>2);
      samples.push_back(m_DigitsPointer[index+5]>>2);
    }
  } else {
    samples.push_back(m_DigitsPointer[index+1]>>2);
    samples.push_back(m_DigitsPointer[index+0]>>2);
    samples.push_back(m_DigitsPointer[index+3]>>2);
    samples.push_back(m_DigitsPointer[index+2]>>2);
    samples.push_back(m_DigitsPointer[index+5]>>2);
    if(nsamples==7) {
      samples.push_back(m_DigitsPointer[index+4]>>2);
      samples.push_back(m_DigitsPointer[index+7]>>2);
    }
  }
  } else {
    samples.push_back(m_DigitsPointer[index+1]>>2);
    samples.push_back(m_DigitsPointer[index+0]>>2);
    samples.push_back(m_DigitsPointer[index+3]>>2);
    samples.push_back(m_DigitsPointer[index+2]>>2);
  }

#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL << " ===> ROD Channel = " << m_DigitsChannel << endmsg; 
  logstr << MYLEVEL << " ===> FEB Channel = " << channelNumber << endmsg; 
  logstr << MYLEVEL << " ===> Gain        = " << gain << endmsg;
  for(int i=0;i<nsamples;i++)
    logstr << MYLEVEL << " ===> sample " << i << "    = " << samples[i] << endmsg;
#endif
  //std::cout << "Gain= " << gain << " Febgain=" << febgain << std::endl;
  m_DigitsIndex++;
  m_DigitsChannel++;
  unsigned  rearrangeFirstSample=0;
  if (m_rearrangeFirstSample)
    rearrangeFirstSample=m_rearrangeFirstSample; //Overwrite by jobOptions
  else
    rearrangeFirstSample=getFirstSampleIndex();
  //std::cout << "FebConfig: "<< getFebConfig() << " FirstSampleIndex " << getFirstSampleIndex() <<std::endl;
  if (rearrangeFirstSample && rearrangeFirstSample<samples.size()) //FIXME: Very ugly hack! See explanation in LArRodDecoder.h file
      {//Change e.g. 3 0 1 2 4 to 0 1 2 3 4 
	short movedSample=samples[0];
	for (unsigned i=1;i<=rearrangeFirstSample;i++)
	  samples[i-1]=samples[i];
	samples[rearrangeFirstSample]=movedSample;
   }
#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL << "GetNextDigits for FEB finished 0x" << MSG::hex << (uint32_t)getHeader32(FEBID) << MSG::dec << endmsg;
#endif
  return 1;
}

uint16_t LArRodBlockPhysicsV6::getNbSweetCells1()  const 
{ 
  if(!m_RaddPointer) return 0;
  return m_RaddPointer[1]>>8;
}

uint16_t LArRodBlockPhysicsV6::getNbSweetCells2()  const 
{ 
  if(!m_RaddPointer) return 0;
  return m_RaddPointer[1] & 0xff;
}

uint16_t LArRodBlockPhysicsV6::getNbSweetCells1FromMask()  const 
{ 
  if(!m_MaskTimeQualityPointer) return 0;
  int n=0;
  for(int i=0;i<4;i++)
    for(int j=0;j<32;j++)
      if((m_MaskTimeQualityPointer[i] >> j) &0x1) n++;
  return n;
}

uint16_t LArRodBlockPhysicsV6::getNbSweetCells2FromMask()  const 
{ 
  if(!m_MaskDigitsPointer) return 0;
  int n=0;
  for(int i=0;i<4;i++)
    for(int j=0;j<32;j++)
      if((m_MaskDigitsPointer[i] >> j) &0x1) n++;
  return n;
}

uint32_t LArRodBlockPhysicsV6::getNumberOfSamples() const
{ 
  return getHeader16(NSamples); 
}

uint32_t LArRodBlockPhysicsV6::getNumberOfGains() const
{ 
  return  getHeader16(NGains);
}

uint16_t LArRodBlockPhysicsV6::getResults1Size() const
{
  return getHeader16(ResultsDim1);
}

uint16_t LArRodBlockPhysicsV6::getResults2Size() const
{
  return getHeader16(ResultsDim2);
}

uint16_t LArRodBlockPhysicsV6::getRawDataSize() const
{
  return getHeader16(RawDataBlkDim);
}

uint32_t LArRodBlockPhysicsV6::getRadd(uint32_t adc, uint32_t sample)  const
{ 
  if(!m_RawDataPointer) {
    if(!m_RaddPointer) return 0;
    if(sample%2) sample+=2;
    return m_RaddPointer[sample];
  }
  int index;
  if(sample==0)         index=6;
  else if(sample & 0x1) index=7+sample-1;
  else                  index=7+sample+1;
  uint32_t x=m_RawDataPointer[index];
  if(adc>=8) return x>>8; 
  return x&0xff;
}

uint16_t LArRodBlockPhysicsV6::getCtrl1(uint32_t /*adc*/)  const
{ 
  if(!m_RawDataPointer) return 0;
  int index=5;
  uint16_t x=m_RawDataPointer[index];
  return x;
}

uint16_t LArRodBlockPhysicsV6::getCtrl2(uint32_t /*adc*/)  const
{ 
  if(!m_RawDataPointer) return 0;
  int index=4;
  uint16_t x=m_RawDataPointer[index];
  return x;
}

uint16_t LArRodBlockPhysicsV6::getCtrl3(uint32_t /*adc*/)  const
{ 
  if(!m_RawDataPointer) return 0;
  int index=7;
  uint16_t x=m_RawDataPointer[index];
  return x;
}

uint32_t LArRodBlockPhysicsV6::getStatus()  const
{ 
  if(getNumberOfWords()<EventStatus/2) return 0;
  uint32_t x=getHeader32(EventStatus);
  return x;
}

/*
uint32_t LArRodBlockPhysicsV6::onlineCheckSum() const
{ 
  //int size   = getNumberOfWords();
  int index  = getNumberOfWords()-1;
  if(index<m_iHeadBlockSize) return 0;
  uint32_t sum = m_FebBlock[index];
  //for(int i=size-10;i<size;i++) {
  //  std::cout << i << " : " << std::hex << m_FebBlock+i << " : " << m_FebBlock[i] << std::endl;
  //}
  return sum;
}

uint32_t LArRodBlockPhysicsV6::offlineCheckSum() const
{
  int end      = getNumberOfWords()-3;
  //int start    = 0; //m_iHeadBlockSize;
  uint32_t sum = 0;
  for(int i=0;i<end;i++) {
    sum += m_FebBlock[i];
    //std::cout << i << " : " << std::hex << sum << " : " << m_FebBlock[i] << std::endl;
  }
  return sum & 0x7fffffff;
}
*/

// start of encoding methods
void LArRodBlockPhysicsV6::initializeFragment(std::vector<uint32_t>& fragment ){
  m_pRODblock=&fragment; //remember pointer to fragment
  if (fragment.size()>m_iHeadBlockSize) {  //Got filled fragment
    unsigned int sizeRead=0;
    //Store existing data in the FEB-Map
    while (sizeRead<fragment.size()) {
      std::vector<uint32_t>::iterator FebIter;
      FebIter=fragment.begin()+sizeRead;     //Store pointer to current Feb-Header
      m_FebBlock=&(*FebIter); //Set m_FebBlock in order to use getHeader-functions.
      uint32_t currFEBid=getHeader32(FEBID); //Get this FEB-ID
      uint16_t currFebSize=getNumberOfWords(); //Size of this FEB-Block
      if (FebIter+currFebSize>fragment.end()) {
        fragment.clear(); //Clear existing vector
        //*m_logstr << MSG::ERROR  << "Got inconsistent ROD-Fragment!" << endmsg;
        return;
      }
      m_mFebBlocks[currFEBid].assign(FebIter,FebIter+currFebSize); //Copy data from ROD-fragment into FEB-Block
      sizeRead+=currFebSize+m_MiddleHeaderSize;  //6 is the middle header size
      //LARBSDBG("Found FEB-id " << currFEBid << " in existing ROD-Fragment");
    } // end while
  }
  fragment.clear(); //Clear existing vector
  return;

}

//For writing: Initalizes a single FEB-Block
void LArRodBlockPhysicsV6::initializeFEB(const uint32_t id)
{
 m_vFragment=&(m_mFebBlocks[id]);
 if (m_vFragment->size()<m_iHeadBlockSize) //Got empty or spoiled fragment
  {
    m_vFragment->resize(m_iHeadBlockSize,0); //Initialize FEB-Header
    setHeader32(FEBID,id);                //Set Feb ID
    // At least 10 (head) + 16 (gain/sumblks) + 64 (energies)
    m_vFragment->reserve(90);
  }

 m_SumBlkBlockE1.resize(4);
 for(unsigned int i=0;i<4;i++) m_SumBlkBlockE1[i]=0x0;
 m_SumBlkBlockE2.resize(4);
 for(unsigned int i=0;i<4;i++) m_SumBlkBlockE2[i]=0x0;
 m_GainBlock.resize(8);
 for(unsigned int i=0;i<8;i++) m_GainBlock[i]=0x0;
// m_RawDataBlock.resize(0);
 m_TimeQualityBlock.resize(8);
 for(unsigned int i=0;i<8;i++) m_TimeQualityBlock[i]=0x0;
 m_EnergyBlockEncode.resize(128);
 for(unsigned int i=0;i<128;i++) m_EnergyBlockEncode[i]=0x0;
 m_DigitsEncode.clear();

 resetPointers();

}

void LArRodBlockPhysicsV6::setNextEnergy(const int channel, const int32_t energy,
                                         const int32_t time, const int32_t quality, const uint32_t gain)
{
 //LARBSDBG("setNextEnergy-------------------->>>>>********************** format V4 ***********");
 //LARBSDBG("Channel=" << channel << " energy =" << energy);
 int rcNb=FebToRodChannel(channel);
 //int rcNb=(channel);
 //rcNb ist supposed to equal or bigger than m_EIndex.
 //In the latter case, we fill up the missing  channels with zero
 if (rcNb<m_EnergyIndex) {
   //*m_logstr << MSG::ERROR  << "LArRODBlockStructure ERROR: Internal error. Channels not ordered correctly. rcNb=" << rcNb
   //          << " m_EnergyIndex=" << m_EnergyIndex << endmsg;
   return;
 }

 //Fill up missing channels with zeros:
 while (m_EnergyIndex<rcNb)
   setNextEnergy((int16_t)0,(int16_t)32767,(int16_t)-32767,(uint32_t)0);

 // transform 32 bits data into 16 bits data

 uint16_t theenergy;
 uint32_t abse,EncodedE;
 int16_t thetime,thequality;
 int32_t sign;

 //Time is in 10 ps in ByteStream, hence the factor 10 to convert from ps
 thetime = (int16_t) time/10;
 thequality = (int16_t) quality;

 sign=(energy>=0?1:-1); // get sign of energy
 abse=(uint32_t)abs(energy);

 EncodedE=abse; // range 0 

 if ((abse>8192)&&(abse<65536))
   {
     EncodedE=((abse>>3)|0x4000); // range 1 : drop last 3 bits and put range bits (bits 14 and 13 = 01)
   }
 else if ((abse>65535)&&(abse<524288))
   {
     EncodedE=((abse>>6)|0x8000); // range 2 : drop last 6 bits and put range bits (bits 14 and 13 = 10)
   }
 else if ((abse>524288))
   {
     EncodedE=((abse>>9)|0xc000); // range 3 : drop last 9 bits and put range bits (bits 14 and 13 = 11)
   }

 // treat sign now :

 if (sign<0) EncodedE |= 0x2000;
 theenergy = (uint16_t) EncodedE;


 // Add data...

 //LARBSDBG("setNextEnergy-------------------->>>>> Energy = "<< energy << "   Encoded Energy =" << theenergy);

 if (abse> m_EnergyThreshold1)
   {
     setNextEnergy(theenergy,thetime,thequality,gain);
   }
 else
   {
     setNextEnergy(theenergy,(int16_t)32767,(int16_t)-32767,gain);
   }
 return;
}

//Private function, expects channel number is rod-style ordering
void LArRodBlockPhysicsV6::setNextEnergy(const uint16_t energy,const int16_t time, const int16_t quality, const uint32_t gain)
{
  if (m_EnergyIndex>=m_channelsPerFEB)        //Use m_EIndex to count total number of channels
    {//*m_logstr << MSG::ERROR  << "LArRodBlockStructure ERROR: Attempt to write Energy for channel "
     //       << m_EnergyIndex << " channels into a FEB!" <<endmsg;
      return;
    }
  //LARBSDBG("LArRodBlockStructure: Setting Energy for channel " << m_EnergyIndex << ". E=" << energy);

  //LARBSDBG("In setNextEnergy-------------------->>>>> time = " << time << " quality=" << quality);

  // Energy
  int endianindex;
  if (m_EnergyIndex & 0x1) endianindex = m_EnergyIndex-1;
  else                    endianindex = m_EnergyIndex+1;
  m_EnergyBlockEncode[endianindex] = energy;

  // Find correct position

  //LARBSDBG("Writing Raw data to E block. E=" << energy);

  // update summary block 
  // Gain is composed of two bits per cell
  uint16_t gain_idx=m_EnergyIndex>>4;
  uint16_t gain_bit=(m_EnergyIndex&0xf)*2;
  uint32_t gain1 = OfflineToRawGain(gain);
  m_GainBlock[gain_idx] |= (gain1 << gain_bit);

  // write Time and Chi2 for cells above HighEnergyCellCut threshold

  if (quality!=-32767) // Do write Time and Chi2 information
    {
      // count the number of hot cells
      m_numberHotCell++;
      // count the number of cells offtime
      if (abs(time)>m_OffTimeCut) m_numberHotCellOffTime++;
      uint16_t mask_idx=m_EnergyIndex>>5;
      uint16_t mask_bit=(m_EnergyIndex&0x1f);
      m_SumBlkBlockE1[mask_idx] |= (0x1 << mask_bit);

      m_TimeQualityBlock.push_back(*((uint16_t*)&time));
      m_TimeQualityBlock.push_back(*((uint16_t*)&quality));
    }
  m_EnergyIndex++; //Use m_EIndex to count the channels put in the Energy block

}

void LArRodBlockPhysicsV6::setRawData(const int chIdx, const std::vector<short>& samples , const uint32_t /* gain_not_used */ ){

      // First of all, set the bits
      int cchIdx = FebToRodChannel(chIdx);
      uint16_t mask_idx=cchIdx>>5;
      uint16_t mask_bit=(cchIdx&0x1f);
      m_SumBlkBlockE2[mask_idx] |= (0x1 << mask_bit);
      for(std::vector<short>::const_iterator i=samples.begin();i!=samples.end();++i){
	m_DigitsEncode.push_back((*i)<<2);
      }

}

void LArRodBlockPhysicsV6::finalizeFEB()
{
//Complete non-complete Energy block
  while (m_EnergyIndex<m_channelsPerFEB)
    setNextEnergy((uint16_t)0,(int16_t)32767,(int32_t)-32767,(uint32_t)0);//E=0,t=32767,q=-32767,G=0

uint16_t n;
//uint16_t BlockOffset;
uint16_t nsamples=5;
// checkSum value
uint32_t sum=0;

// Will Hardcode here for the moment FIXME. Minimal 1 sample
setHeader16(NGains,1);
setHeader16(NSamples,nsamples);
// These will never be used form MC. Nice to put in here thought
setHeader16(FEB_SN,0xfefe);
setHeader16(FEB_SN_h,0xdede);
setHeader16(InFPGAFormat,0x0);
setHeader16(InFPGAFormat_h,0x2);

// Gain block...
n = m_GainBlock.size();
//BlockOffset=0;
//LARBSDBG("Checking Gain Block n=" << n << "BlockOffset=" << BlockOffset);
//Check if Gain-Block exists and is not yet part of the fragment
if (n)
  {
    //LARBSDBG(MSG::DEBUG  << "In finalyseFEB-------------------->>>>> " << "Checking for Gain Block :  length= " << n << "  BlockOffset=" << BlockOffset);
    for(unsigned int i=0;i<n;i++){
      m_vFragment->push_back(m_GainBlock[i]);
      sum+=m_GainBlock[i];
    }
  }

 // Cells above energy threshold E1
 n = m_SumBlkBlockE1.size();
 //Check if Summary Block exists and is not yet part of the fragment
 if (n)
   {
      //LARBSDBG("In finalizeFEB-------------------->>>>> " << "Checking for Summary Block :  length= " << n << "  BlockOffset=" << BlockOffset);
     for (unsigned i=0;i<n;i++){
       m_vFragment->push_back(m_SumBlkBlockE1[i]);
       sum+=m_SumBlkBlockE1[i];
     }
   }

 // Cells above energy threshold E2 (not included so far)
 n = m_SumBlkBlockE2.size();
 //Check if Summary Block exists and is not yet part of the fragment
 //LARBSDBG("Checking for Summary Block n=" << n << "BlockOffset=" << BlockOffset);
 if (n)
   {
     //LARBSDBG("In finalizeFEB-------------------->>>>> " << "Checking for Summary Block :  length= " << n << "  BlockOffset=" << BlockOffset);
     for (unsigned i=0;i<n;i++){
       m_vFragment->push_back(m_SumBlkBlockE2[i]);
       sum+=m_SumBlkBlockE2[i];
     }
   }

 // fill info from counters
 // for moment just include 1 fake words (32 bits) to put radd
 uint32_t radd_nANC=0x0;
 // Second threshold missing (FIXME)
 radd_nANC = ((m_numberHotCell<<8))+(m_DigitsEncode.size()/nsamples);
 radd_nANC = (radd_nANC<<16);
 m_vFragment->push_back(radd_nANC);
 sum+=radd_nANC;
 // Need to include radd nsamples-1
 // No need to include in sum's for now
 for( int i=0; i < (nsamples-1)/2; i++)
 	m_vFragment->push_back(0x0);


 // Energy block...
 n = 128 ; // Fixed size m_EnergyBlock.size();
 // BlockOffset=getVectorHeader16(ResultsOff1); xxx
 // Block also include time, whenever necessary
 int size_of_block=80+(nsamples+1)/2+(m_TimeQualityBlock.size())/2;
 //LARBSDBG("Checking Energy Block n=" << n << "BlockOffset=" << BlockOffset);
 //Check if Energy-Block exists and is not yet part of the fragment
 if (n)
   {
     setHeader16(ResultsOff1,18);
     setHeader16(ResultsDim1,size_of_block);
     //LARBSDBG("In finalyseFEB-------------------->>>>> " << "Checking for Energy Block :  length= " << n << "  BlockOffset=" << BlockOffset);
     for(unsigned int i=0;i<n/2;i++) {
       // WARNING witch one should be >>16 2*i or 2*i+1? To be tested
       uint32_t Encode = m_EnergyBlockEncode[2*i]+(m_EnergyBlockEncode[2*i+1]<<16);
       m_vFragment->push_back(Encode);
       sum+=Encode;
     }
   }

 // Magic numbers (4 or 8) for Ex, Ey and Ez
 n = m_TimeQualityBlock.size();
 //LARBSDBG("Checking Time and Quality Block n=" << n << "BlockOffset=" << BlockOffset);
 //Check if Time and Quality Block exists and is not yet part of the fragment
 if (n)
   {
     unsigned int imax = n/2;
     for(unsigned int i=0;i<imax;i++){
       ShortLong to_push{};
       to_push.s[0] = m_TimeQualityBlock[i*2];
       to_push.s[1] = m_TimeQualityBlock[i*2+1];
       m_vFragment->push_back(to_push.l);
       sum+=to_push.l;
     }
   }
   // Now include digits
   n = m_DigitsEncode.size();
   if ( n ) {
     // First make sure it is not and odd number to store
     if ( m_DigitsEncode.size() & 0x1 ) m_DigitsEncode.push_back(0x0);
     unsigned int imax=m_DigitsEncode.size()/2;
     for(unsigned int i=0;i<imax;i++){
	// Better by-swap
	ShortLong to_push{};
	to_push.s[1]=m_DigitsEncode[i*2];
	to_push.s[0]=m_DigitsEncode[i*2+1];
        m_vFragment->push_back(to_push.l);
        sum+=to_push.l;
     }
     setHeader16(ResultsDim2,m_DigitsEncode.size()/2);
     setHeader16(ResultsOff2,18+size_of_block);
   } // End of check for format

   // Need to add header to check sum
   for(size_t ii=0;ii<endtag/2;ii++){
	sum+=((*m_vFragment)[ii]);
   }
   // Three final magic words
   m_vFragment->push_back(0x0); // For the moment
   m_vFragment->push_back(0x12345678); // For the moment
   //sum+=0x12345678;
   sum+=m_vFragment->size()+1;
   m_vFragment->push_back(sum& 0x7fffffff);
   
   setHeader32(NWTot,m_vFragment->size());
   return;

}


void  LArRodBlockPhysicsV6::concatinateFEBs()
{
 FEBMAPTYPE::const_iterator feb_it_b=m_mFebBlocks.begin();
 FEBMAPTYPE::const_iterator feb_it_e=m_mFebBlocks.end();
 FEBMAPTYPE::const_iterator feb_it;
 for (feb_it=feb_it_b;feb_it!=feb_it_e;++feb_it) {
   if (feb_it!=feb_it_b) //Not first Feb
       m_pRODblock->resize( m_pRODblock->size()+m_MiddleHeaderSize);

   //Add feb data to rod data block
   m_pRODblock->insert (m_pRODblock->end(),
                        feb_it->second.begin(), feb_it->second.end());
 } //end for feb_it

  m_mFebBlocks.clear();
  return;
}

//Sort functions & ordering relation:
template<class RAWDATA>
bool LArRodBlockPhysicsV6::operator () 
  (const RAWDATA* ch1, const RAWDATA* ch2) const
{
  HWIdentifier id1 = ch1->channelID(); 
  HWIdentifier id2 = ch2->channelID(); 
  
  HWIdentifier febId1= m_onlineHelper->feb_Id(id1); 
  HWIdentifier febId2= m_onlineHelper->feb_Id(id2); 

  if(febId1 == febId2 ){
    int cId1 =  m_onlineHelper->channel(id1); 
    int cId2 =  m_onlineHelper->channel(id2);
    return FebToRodChannel(cId1) < FebToRodChannel(cId2);
  } 

  return febId1 < febId2 ; 
}


#ifdef LARBSDBGOUTPUT
#undef LARBSDBGOUTPUT
#endif
#undef LARBSDBG
