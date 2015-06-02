//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of a LArRODBlockStructure class
// This version contains LArDigits in fixed gain.
// See .h file for more details.

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
//#include <cstdio>
#include "LArByteStream/LArRodBlockPhysicsV4.h"
#include <iostream>

//#define LARBSDBGOUTPUT
#ifdef LARBSDBGOUTPUT
#define MYLEVEL (MSG::FATAL)
#define LARBSDBG(text) logstr<<MYLEVEL<<text<<endreq
#else
#define LARBSDBG(text)
#endif

LArRodBlockPhysicsV4::LArRodBlockPhysicsV4() : LArRodBlockStructure()
{
  m_iHeadBlockSize=endtag/2; // The implicit cast rounds down to the right size 
  m_fixedGain=CaloGain::LARNGAIN;
  resetPointers();
}

void LArRodBlockPhysicsV4::resetPointers() 
{
  m_EnergyIndex=0;
  m_TimeQualityIndex=0;
  m_RawDataIndex=0;
  m_TowerIndex=0;

  m_EnergyPointer=0;
  m_TimeQualityPointer=0;
  m_RawDataPointer=0;
  m_GainPointer=0;
  m_MaskPointer=0;
  m_TowerPointer=0;
  m_SumPointer=0;
  m_HotCellsPointer=0;
}

bool LArRodBlockPhysicsV4::setPointers()
{
  m_EnergyIndex=0;
  m_TimeQualityIndex=0;
  m_RawDataIndex=0;
  m_TowerIndex=0;

  if (m_FebBlockSize>m_iHeadBlockSize)
    {
      int off = -8;
      if (getHeader16(ResultsOff1)) {
	if (getHeader16(ResultsDim1)>=8) 
	m_GainPointer=(const uint32_t*)(m_FebBlock+getHeader16(ResultsOff1)+off);
	if (getHeader16(ResultsDim1)>=12) 
	  m_MaskPointer=(const uint32_t*)(m_FebBlock+getHeader16(ResultsOff1)+8+off);
	if (getHeader16(ResultsDim1)>=13)
	  m_HotCellsPointer=(const uint16_t*) (m_FebBlock+getHeader16(ResultsOff1)+12+off);
	if (getHeader16(ResultsDim1)>13)
	  m_EnergyPointer=(const uint16_t*) (m_FebBlock+getHeader16(ResultsOff1)+13+off);
      }
      if (getHeader16(ResultsOff2)) {
	if (getHeader16(ResultsOff2)>=4) 
	m_TowerPointer=(const uint8_t*)(m_FebBlock+getHeader16(ResultsOff2)+off);
	if (getHeader16(ResultsOff2)>=7) 
	  m_SumPointer=(const int32_t*)(m_FebBlock+getHeader16(ResultsOff2)+4+off);
	if (getHeader16(ResultsOff2)>7) 
	  m_TimeQualityPointer=(const uint16_t*)(m_FebBlock+getHeader16(ResultsOff2)+7+off);
      }
      if (getHeader16(RawDataBlkOff))
	m_RawDataPointer=(const uint16_t*)(m_FebBlock+getHeader16(RawDataBlkOff)+off);
      
#ifdef  LARBSDBGOUTPUT
      MsgStream logstr(Athena::getMessageSvc(), BlockType());
      logstr << MYLEVEL  << "***********************************************************************"<< endreq;
      logstr << MYLEVEL  << "Header values :"<< endreq;
      logstr << MYLEVEL  << "************************************************************************"<< endreq;
      logstr << MYLEVEL  << "Fragment @    = 0x" << MSG::hex << m_FebBlock << endreq;
      logstr << MYLEVEL  << "NWTot         =   " << MSG::dec << getNumberOfWords() << endreq;
      logstr << MYLEVEL  << "FebID         = 0x" << MSG::hex << getHeader32(FEBID) << endreq;
      logstr << MYLEVEL  << "FebSN         = 0x" << MSG::hex << getHeader32(FEB_SN) << endreq;
      logstr << MYLEVEL  << "ResultsOff1   = 0x" << MSG::hex << getHeader16(ResultsOff1) << endreq;
      logstr << MYLEVEL  << "ResultsDim1   = 0x" << MSG::hex << getHeader16(ResultsDim1) << endreq;
      logstr << MYLEVEL  << "ResultsOff2   = 0x" << MSG::hex << getHeader16(ResultsOff2) << endreq;
      logstr << MYLEVEL  << "ResultsDim2   = 0x" << MSG::hex << getHeader16(ResultsDim2) << endreq;
      logstr << MYLEVEL  << "RawDataBlkOff = 0x" << MSG::hex << getHeader16(RawDataBlkOff) << endreq;
      logstr << MYLEVEL  << "RawDataBlkDim = 0x" << MSG::hex << getHeader16(RawDataBlkDim) << endreq;
      logstr << MYLEVEL  << "************************************************************************"<< MSG::dec << endreq;

      int size = getNumberOfWords();
      for(int i=0;i<size;i++) {
	logstr << MYLEVEL << MSG::hex << i << " : " << MSG::hex << m_FebBlock+i << " : " << MSG::hex << m_FebBlock[i] << endreq;
      }

      if(m_MaskPointer) size = 4; else size = 0;
      logstr << MYLEVEL  << "Mask Block size = " << MSG::hex << size << endreq;
      logstr << MYLEVEL  << "Mask Block address = " << MSG::hex << m_MaskPointer << endreq;
      for(int i=0;i<size;i++) {
	logstr << MYLEVEL << i << " : " << MSG::hex << m_MaskPointer[i] << endreq;
      }
      logstr << MYLEVEL  << "End of Mask Block" << endreq;
#endif
   }

  return true;
}

int LArRodBlockPhysicsV4::getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain)
{
  //std::cout << " I am here !!!!!!!!!!!!!!!!!!!!!! " << std::endl;
#ifdef LARBSDBGOUTPUT
  MsgStream logstr(Athena::getMessageSvc(), BlockType());
  //Debug output
  logstr << MYLEVEL << "Let s go in getNextRawData..." << endreq;
  logstr << MYLEVEL << "GetNextRawData for FEB 0x" << MSG::hex << (uint32_t)getHeader32(FEBID) << MSG::dec << endreq;
  logstr << MYLEVEL << "m_RawDataPointer=" << m_RawDataPointer << " m_RawDataIndex="<<  m_RawDataIndex 
	 << " m_channelsPerFEB=" << m_channelsPerFEB << endreq;
#endif

  if (m_RawDataIndex>=m_channelsPerFEB) { //Already beyond maximal number of channels
#ifdef LARBSDBGOUTPUT
    logstr << MYLEVEL << "Maximum number of channels reached" << endreq;
#endif
    return 0;
  }
  //const uint16_t block = getHeader16(m_RawDataOff);//Position of the raw FEB data block
  if (!m_RawDataPointer) { //Block does not exist
#ifdef LARBSDBGOUTPUT
    logstr << MYLEVEL << "No Raw Data Block in this FEB" << endreq;
#endif
    return 0; 
  }

  // Get next channel
  unsigned rodChannelNumber=m_RawDataIndex;      // Index of Channel in ROD-Block
  channelNumber=((rodChannelNumber&0xe)<<2) + ((rodChannelNumber&0x1)<<6) + (rodChannelNumber>>4);    //channel number of the FEB
  //channelNumber=(rodChannelNumber>>4) + ((rodChannelNumber&0xf)<<3);    //channel number of the FEB
  uint32_t febgain;
  const unsigned int nsamples = getHeader16(NSamples) & 0xff;
  const unsigned int ngains   = getHeader16(NGains);

#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL << "This FEB has " << nsamples <<  " samples" << endreq;
  logstr << MYLEVEL << "This FEB has " << ngains   <<  " gains" << endreq;
#endif

  if(ngains==0 || nsamples==0) return 0;
  int s_size = nsamples+1;
  int offset = 12;
  int index;
  index = s_size*m_RawDataIndex + offset;
  uint16_t s[2];
  s[0]    = m_RawDataPointer[index++]>>2;
  febgain = m_RawDataPointer[index++];
  samples.push_back(s[0]);
  for(unsigned int i=0;i<nsamples/2;i++) {
    s[1]    = m_RawDataPointer[index++]>>2;
    s[0]    = m_RawDataPointer[index++]>>2;
    samples.push_back(s[0]);
    samples.push_back(s[1]);
  }
  gain=RawToOfflineGain(febgain);

#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL << " ===> ROD Channel = " << m_RawDataIndex << endreq; 
  logstr << MYLEVEL << " ===> FEB Channel = " << channelNumber << endreq; 
  logstr << MYLEVEL << " ===> Gain        = " << gain << endreq;
  for(int i=0;i<nsamples;i++)
    logstr << MYLEVEL << " ===> sample " << i << "    = " << samples[i] << endreq;
  int n = m_RawDataIndex;
  int32_t e,t,q;
  uint32_t g;
  LArRodBlockPhysicsV4::getNextEnergy(n,e,t,q,g);
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
  logstr << MYLEVEL << "GetNextRawData for FEB finished 0x" << MSG::hex << (uint32_t)getHeader32(FEBID) << MSG::dec << endreq;
#endif
  return 1;
}

uint32_t LArRodBlockPhysicsV4::getNumberOfSamples() const
{ 
  return getHeader16(NSamples); 
}

uint32_t LArRodBlockPhysicsV4::getNumberOfGains() const
{ 
  return  getHeader16(NGains);
}

uint16_t LArRodBlockPhysicsV4::getResults1Size() const
{
  return getHeader16(ResultsDim1);
}

uint16_t LArRodBlockPhysicsV4::getResults2Size() const
{
  return getHeader16(ResultsDim2);
}

uint16_t LArRodBlockPhysicsV4::getRawDataSize() const
{
  return getHeader16(RawDataBlkDim);
}

uint32_t LArRodBlockPhysicsV4::getRadd(uint32_t adc, uint32_t sample)  const
{ 
  if(!m_RawDataPointer) return 0;
  int index;
  if(sample==0)         index=6;
  else if(sample & 0x1) index=7+sample-1;
  else                  index=7+sample+1;
  uint32_t x=m_RawDataPointer[index];
  if(adc>=8) return x>>8; 
  return x&0xff;
}

uint16_t LArRodBlockPhysicsV4::getCtrl1(uint32_t /*adc*/)  const
{ 
  if(!m_RawDataPointer) return 0;
  int index=5;
  uint16_t x=m_RawDataPointer[index];
  return x;
}

uint16_t LArRodBlockPhysicsV4::getCtrl2(uint32_t /*adc*/)  const
{ 
  if(!m_RawDataPointer) return 0;
  int index=4;
  uint16_t x=m_RawDataPointer[index];
  return x;
}

uint16_t LArRodBlockPhysicsV4::getCtrl3(uint32_t /*adc*/)  const
{ 
  if(!m_RawDataPointer) return 0;
  int index=7;
  uint16_t x=m_RawDataPointer[index];
  return x;
}

uint32_t LArRodBlockPhysicsV4::getStatus()  const
{ 
  if(getNumberOfWords()<EventStatus/2) return 0;
  uint32_t x=getHeader32(EventStatus);
  return x;
}

uint32_t LArRodBlockPhysicsV4::onlineCheckSum() const
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

uint32_t LArRodBlockPhysicsV4::offlineCheckSum() const
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

#ifdef LARBSDBGOUTPUT
#undef LARBSDBGOUTPUT
#endif
#undef LARBSDBG
