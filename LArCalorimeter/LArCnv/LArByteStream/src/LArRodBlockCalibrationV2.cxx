//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of a LArRODBlockStructure class
// This version contains LArDigits in fixed gain.
// See .h file for more details.

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "LArByteStream/LArRodBlockCalibrationV2.h"
//#include <cstdio>
#include <iostream>

#define LARBSDBGOUTPUT
#ifdef  LARBSDBGOUTPUT
#define LARBSDBG(text) logstr<<MSG::DEBUG<<text<<endmsg
#else
#define LARBSDBG(text)
#endif


LArRodBlockCalibrationV2::LArRodBlockCalibrationV2() : LArRodBlockStructure()
{
  m_iHeadBlockSize=endtag/2; // The implicit cast rounds down to the right size 
  m_fixedGain=CaloGain::LARNGAIN;
  LArRodBlockCalibrationV2::resetPointers();
}

// clear temporary block vectors 
void LArRodBlockCalibrationV2::clearBlocks()
{
  m_RawDataBlock.clear();
}

void LArRodBlockCalibrationV2::resetPointers() 
{
  m_Result1Counter=0;
  m_Result1Index=0;
  m_Result2Counter=0;
  m_Result2Index=0;
  m_RawDataCounter=0;
  m_RawDataIndex=0;

}


uint8_t LArRodBlockCalibrationV2::getTDCPhase() const
{
  return (getHeader16(NSamples)>>8);
}




int LArRodBlockCalibrationV2::getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain)
{
#ifdef LARBSDBGOUTPUT
  MsgStream logstr(Athena::getMessageSvc(), BlockType());
#endif
 //Debug output
  /*
    LARBSDBG("GetNextRawData for FEB 0x" << MSG::hex << (uint32_t)getHeader32(FEBID) << MSG::dec);
    LARBSDBG("m_RawDataCounter=" << m_RawDataCounter << " m_RawDataIndex="<<  m_RawDataIndex 
    << " m_channelsPerFEB=" << m_channelsPerFEB);
    LARBSDBG("requested gain= " << m_fixedGain);
  */
  if (m_RawDataCounter>=m_channelsPerFEB) { //Already beyond maximal number of channels
    LARBSDBG("Maximum number of channels reached");
    return 0;
  }
  //const uint16_t block = getHeader16(m_RawDataOff);//Position of the raw FEB data block
  const uint16_t block = getHeader16(RawDataBlkOff);
  if (!block) { //Block does not exist
    LARBSDBG("No Raw Data Block in this FEB");
    return 0; 
  }
  //The m_RawDataChannel keeps track of the last read channel
  //std::cout << "endtag=" << endtag << " m_iHeadBlockSize=" << m_iHeadBlockSize << std::endl;

  // Get next channel
  channelNumber=m_RawDataCounter;
  uint32_t febgain;
  const unsigned int nsamples = getHeader16(NSamples) & 0xff;
  const unsigned int ngains   = getHeader16(NGains);
  /*
  LARBSDBG("This FEB has " << nsamples <<  " samples");
  LARBSDBG("This FEB has " << ngains   <<  " gains");
  */
  if(ngains==0 || nsamples==0) return 0;
  // Loop over gains to look for m_fixedGain
  unsigned int this_gain=0;
  int offset;
  LARBSDBG(" ===> fixedGain= " << m_fixedGain << " CaloGain = " << CaloGain::LARNGAIN);
  if (m_fixedGain!=CaloGain::LARNGAIN) { //Fixed gain: Search for gain
    offset=block + 8 + ((channelNumber&0x3F)>>3) + ((channelNumber & 0x7)<<3);
    for(this_gain=0;this_gain<ngains;this_gain++) {
      int index = offset + 64*this_gain;
      uint32_t x = m_FebBlock[index];
      if(channelNumber>=64) 
	x = (x & 0x3000) >> 12;
      else 
	x = (x & 0x30000000) >> 28;
      int data_gain = RawToOfflineGain(x);
      if(data_gain==m_fixedGain) break;
    }
  }
  if (this_gain<ngains) { //Gain found in this fragment
    int s_size = 8 + 64 * ngains;  // Size of one sample block 16 RADD of 16 bit + 128 channels (16 bit data) 
    offset = block + 8 + ((channelNumber&0x3F)>>3) + ((channelNumber & 0x7)<<3) + 64*this_gain;
    int index  =  offset;
    uint32_t x = m_FebBlock[index];
    if(channelNumber>=64) { //low channels on lower bits
      // First decode gain
      febgain = (x & 0x3000) >> 12; // gain on bits 12 and 13
      // Than samples
      for(unsigned int s=0;s<nsamples;s++) {
	index  =  offset + s*s_size;
	x = m_FebBlock[index];
	//	if((s==0) && (channelNumber==64)) std::cout << "===> " << std::dec << channelNumber << "  " << (x&0xfff) << "  " << std::endl;
	samples.push_back((short) (x & 0x0fff));  // sample on bits 0 to 11
      } 
    } else { //high channels on higher bits
      // First decode gain
      febgain = (x & 0x30000000) >> 28; // gain on bits 12 and 13
      // Than samples
      for(unsigned int s=0;s<nsamples;s++) {
	index  =  offset + s*s_size;
	x = (m_FebBlock[index]) >> 16;
	samples.push_back((short) (x & 0x0fff));  // sample on bits 0 to 11
      }
    }
    gain=RawToOfflineGain(febgain);
  }
  //std::cout << "Gain= " << gain << " Febgain=" << febgain << std::endl;
  ++m_RawDataCounter;
  if (m_rearrangeFirstSample && m_rearrangeFirstSample<samples.size()) //FIXME: Very ugly hack! See explanation in LArRodDecoder.h file
      {//Change e.g. 3 0 1 2 4 to 0 1 2 3 4 
	short movedSample=samples[0];
	for (unsigned i=1;i<=m_rearrangeFirstSample;i++)
	  samples[i-1]=samples[i];
	samples[m_rearrangeFirstSample]=movedSample;
      }
  LARBSDBG("GetNextRawData for FEB finished 0x" << MSG::hex << (uint32_t)getHeader32(FEBID) << MSG::dec);
  return 1;
}
int LArRodBlockCalibrationV2::getNextAccumulatedCalibDigit(int& channelNumber,  std::vector< std::vector < uint32_t > >& samplesSum ,  std::vector< std::vector < uint32_t > >& samples2Sum, uint32_t& iStepTriggers, uint32_t& gain )
{
#ifdef LARBSDBGOUTPUT
  MsgStream logstr(Athena::getMessageSvc(), BlockType());
#endif
 //Debug output
  LARBSDBG("GetAccumulatedCalibDigits for FEB 0x" << MSG::hex << 
	   (uint32_t)getHeader32(FEBID) << MSG::dec << "iStepTriggers=" << iStepTriggers);
  LARBSDBG("m_Result1Counter" << m_Result1Counter << " m_Result1Index="<<  m_Result1Index 
	   << " m_channelsPerFEB=" << m_channelsPerFEB);
  LARBSDBG("requested gain= " << m_fixedGain);
  if (m_Result1Counter>=m_channelsPerFEB) { //Already beyond maximal number of channels
    LARBSDBG("Maximum number of channels reached");
    return 0;
  }
  const uint16_t block = getHeader16(ResultsOff1);//Position of the AccumulatedCalibDigits FEB data block
  if (!block) { //Block does not exist
    LARBSDBG("No Accumulated Calib Digit Block in this FEB");
    //    std::cout << "index= " << ResultsOff1 <<  "word= " << getHeader16(ResultsOff1) << std::endl; 
    return 0; 
  }
  //The m_Result1Channel keeps track of the last read channel
  //std::cout << "endtag=" << endtag << " m_iHeadBlockSize=" << m_iHeadBlockSize << std::endl;

  // Get next channel
  channelNumber=m_Result1Counter;
  uint32_t febgain=0;
  // This is just for fun; these data are not stored here but in RodBlockDecoder.cxx
  // IWS 08.08.2005 This part is useless in fact - just for debugging
  const unsigned int nsamples  = getHeader16(NSamples);
  const unsigned int ngains    = getHeader16(NGains);
  //const unsigned int ntriggers = (int) getNTrigger();
  //const unsigned int dac       = (int) getDAC();
  //const unsigned int delay     = (int) getDelay();
  //  const unsigned int nStepTriggers     = (int) getnStepTriggers();
  unsigned int nStepTriggers = 1;
  const unsigned int FebConfig = (int) getFebConfig();
//  LARBSDBG("This FEB has " << nsamples <<  " samples");
//  LARBSDBG("This FEB has " << ngains   <<  " gains");
//  LARBSDBG("This FEB has " << ntriggers   <<  " ntriggers");
//  LARBSDBG("This FEB has " << dac   <<  " dac");
//  LARBSDBG("This FEB has " << delay   <<  " delay");
//  LARBSDBG("This FEB has " << std::hex << FebConfig   <<  " FebConfig");
  if(ngains==0 || nsamples==0) return 0;
  // IWS 11.01.2006 book vectors
  if(!iStepTriggers)
    {
      samplesSum.resize(nStepTriggers);
      samples2Sum.resize(nStepTriggers);
    }
  // Loop over gains to look for m_fixedGain
  //unsigned int this_gain=0;
  int offset;
  uint32_t x,x2;
  if (m_fixedGain!=CaloGain::LARNGAIN) { //Fixed gain: Search for gain
    if(FebConfig==0) // free gain - dontknow what to do
      {
	LARBSDBG("free gain - dont know what to do Accumulated Calib Digit - return 0");
	return 0; 
      }
    else if(FebConfig <4 ) // one fixed gain
      {
	febgain=FebConfig;
      }
    else if ((FebConfig>3) && (FebConfig<10)) // two gains - not yet ready
      {
	LARBSDBG("two gains - dont know what to do yet in Accumulated Calib Digit - return 0");
	return 0; 
      }
    else if (FebConfig>9) // three gains - not yet ready
      {
	LARBSDBG("three gains - dont know what to do yet in Accumulated Calib Digit - return 0");
	return 0; 
      }
  }
  else
    {
      febgain=FebConfig&0x3; // to be done better when this is better understood
    }
  
  // nsamples*2 for Sum + Sum2
  // 11 is the size of the calibration header
  offset=block + 12 + (channelNumber&0x7F)*(nsamples*2); // needs to be updated for loop on gains
  uint32_t index=offset;
  
  /*
  if(dac==3000)
    {
      std::cout << "Chid" << channelNumber << " offset= " << std::dec << offset << std::endl;
      for(int i=0; i<50; i++)
	{
	  std::cout << "  i= "  << std::dec << i <<" x=" <<  std::hex << m_FebBlock[block+i] << std::endl;
	}
    }
  */  
  /*
  float * pfloat;
  pfloat = (float *) &m_FebBlock[index];
  */
  for (unsigned int i=0;i<nsamples;i++)
    {
      /*
      x = *pfloat++;
      x2 = *pfloat++;
      */
      x=m_FebBlock[index];
      x2=m_FebBlock[index+1];
      
      samplesSum[iStepTriggers].push_back(x);  
      samples2Sum[iStepTriggers].push_back(x2);  
      //std::cout << "isample=" << i << "  x= " << std::hex << x << "  x2=" << std::hex << x2 << std::endl;
      index+=2;
    }
  
  gain=RawToOfflineGain(febgain);
  
  ++m_Result1Counter;
  return 1;
}


uint32_t LArRodBlockCalibrationV2::getNumberOfSamples() const
{ 
  return getHeader16(NSamples);
}

uint32_t LArRodBlockCalibrationV2::getNumberOfGains() const
{ 
  return getHeader16(NGains);
}

uint16_t LArRodBlockCalibrationV2::getNTrigger() const
{ 

  int index=getHeader16(ResultsOff1);
  uint32_t x=m_FebBlock[index+NTrigger/2];
  return (uint16_t) x; 
}

uint16_t LArRodBlockCalibrationV2::getDAC() const
{ 
  int index=getHeader16(ResultsOff1);
  uint32_t x = m_FebBlock[index+Dac/2];
  return (uint16_t) x; 
}

uint16_t LArRodBlockCalibrationV2::getDelay() const
{ 
  int index=getHeader16(ResultsOff1);
  uint32_t x = m_FebBlock[index+Delay/2];
  return (uint32_t) x; 
}
/* IWS 06.01.2006 prepare for future data
uint16_t LArRodBlockCalibrationV2::getnStepTriggers() const
{ 
  int index=getHeader16(ResultsOff1);
  uint32_t x = m_FebBlock[index+nStepTriggers/2];
  return (uint32_t) x; 
}
*/
uint32_t LArRodBlockCalibrationV2::getRadd(uint32_t adc, uint32_t sample)  const
{ 
  int ngain=getHeader16(NGains);
  int index=getHeader16(RawDataBlkOff);
  if(index<=0) return 0;
  index+=(8+64*ngain)*sample+adc/2;
  uint32_t x=m_FebBlock[index];
  if(adc&0x1) return x>>16;
  return x&0xffff;
}

uint16_t LArRodBlockCalibrationV2::getCtrl1(uint32_t adc)  const
{ 
  int index=getHeader16(RawDataBlkOff)-16+adc/2;
  uint32_t x=m_FebBlock[index];
  if(adc&0x1) x=x>>16;
  else x=x&0xffff;
  uint16_t ctrl=x;
  return ctrl;
  /*
  int index=getHeader16(RawDataBlkOff)-16;
  uint32_t x=m_FebBlock[index];
  if(adc&0x1) return x>>16;
  return x&0xffff;
  */
}

uint16_t LArRodBlockCalibrationV2::getCtrl2(uint32_t adc)  const
{ 
  int index=getHeader16(RawDataBlkOff)-8+adc/2;
  uint32_t x=m_FebBlock[index];
  if(adc&0x1) x=x>>16;
  else x=x&0xffff;
  uint16_t ctrl=x;
  return ctrl;
  /*
  int index=getHeader16(RawDataBlkOff)-8 ;
  uint32_t x=m_FebBlock[index];
  if(adc&0x1) return x>>16;
  return x&0xffff;
  */
}

uint16_t LArRodBlockCalibrationV2::getCtrl3(uint32_t adc)  const
{ 
  int nsamples=getHeader16(NSamples);
  int ngains=getHeader16(NGains);
  int offset=nsamples*(8+64*ngains)+adc/2;
  int index=getHeader16(RawDataBlkOff)+offset;
  uint32_t x=m_FebBlock[index];
  if(adc&0x1) x=x>>16;
  else x=x&0xffff;
  uint16_t ctrl=x;
  return ctrl;
  /*
  int index=getHeader16(RawDataBlkOff)-3;
  uint32_t x=m_FebBlock[index];
  x=x>>16;
  uint16_t ctrl=x;
  return ctrl;
  */
}

uint32_t LArRodBlockCalibrationV2::getStatus()  const
{ 
  // Old version: get Dsp status word
  // if(getNumberOfWords()<EventStatus/2) return 0;
  // uint32_t x=getHeader32(EventStatus);
  // return x;

  // New verion: get Rod status word
  int nsamples = getHeader16(NSamples);
  int ngains   = getHeader16(NGains);
  int offset   = nsamples*(8+64*ngains)+8;
  int index    = getHeader16(RawDataBlkOff)+offset;
  uint32_t x   = m_FebBlock[index];
  return x;

}


#ifdef LARBSDBGOUTPUT
#undef LARBSDBGOUTPUT
#endif
#undef LARBSDBG
