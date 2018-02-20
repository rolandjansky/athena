//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRODBLOCKPHYSICSV4_H
#define LARBYTESTREAM_LARRODBLOCKPHYSICSV4_H

/** This class provides decoding/encoding from/to ROD format. 
   * @authors W. Lampl, R. Lafaye
   * @version  0-0-1 , Jul 10, 2007
   *
   * Modified, 04.01.2006, IWS
   * new format with averages
   * Modified, 25.04.2006, IWS
   * new format with averages sub-steps 
   */

#include "LArByteStream/LArRodBlockStructure.h"
#include "CaloIdentifier/CaloGain.h"

//#define LARBSDBGOUTPUT
#ifdef LARBSDBGOUTPUT
#define MYLEVEL (MSG::FATAL)
#define LARBSDBG(text) logstr<<MYLEVEL<<text<<endmsg
#else
#define LARBSDBG(text)
#endif

class LArRodBlockPhysicsV4 : public LArRodBlockStructure
{
public:
  // ----------------- Header words indexes -----------------
  enum {
    NWTot,            // First words: DSP event header
    NWTot_h,
    FEBID,
    FEBID_h,
    FEB_SN,           // FEB serial number
    FEB_SN_h,         // FEB serial number
    ResultsOff1,      // Size of results (Physics averages in DSP)
    ResultsDim1,      // Offset to results
    ResultsOff2,      // Size of times (in physics)
    ResultsDim2,      // Offset to times (in physics)
    RawDataBlkOff,
    RawDataBlkDim, // Raw FEB event offset
    EventStatus,      // Bits describing the event
    EventStatus_h,      
    NGains,
    NSamples,   
    FirstSampleIndex,  
    FebConfig,
    InFPGAFormat, // added 08.09.2005 - wrong 28.09.2005?
    InFPGAFormat_h,
    endtag          //This tag needs to be an odd number, see *) for constructor
  };
  // constructor
  LArRodBlockPhysicsV4();
 
  // ------ Identify RodBlockStructure -------
  std::string BlockType() { return std::string("RodBlockPhysicsV4");}
 public:
  // ----------------- Decoding methods -----------------
  // Never to be used while encoding!
  // set full ROD fragment before trying to get anything!
  // in case there is more than 1 FEB in 1 fragment, jump to next FEB
  virtual inline int getNextEnergy(int& channelNumber, int32_t& energy, int32_t& time,int32_t& quality,uint32_t& gain);
  virtual int  getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain);
  virtual uint16_t getResults1Size() const;
  virtual uint16_t getResults2Size() const;
  virtual uint16_t getRawDataSize() const;
  virtual uint32_t getNumberOfSamples() const;
  virtual uint32_t getNumberOfGains() const;
  virtual uint32_t getRadd(uint32_t adc, uint32_t sample) const;
  virtual uint16_t getCtrl1(uint32_t adc) const;
  virtual uint16_t getCtrl2(uint32_t adc) const;
  virtual uint16_t getCtrl3(uint32_t adc) const;
  virtual uint32_t getStatus() const;

  virtual uint32_t onlineCheckSum() const;
  virtual uint32_t offlineCheckSum() const;

  virtual inline  uint32_t  hasCalibBlock()   const {return 0;} ; 
  virtual inline  uint32_t  hasPhysicsBlock() const {return getHeader16(ResultsOff1);} ;
  virtual inline  uint32_t  hasRawDataBlock() const {return getHeader16(RawDataBlkOff);} ;
  virtual inline  uint32_t  hasControlWords() const {return getHeader16(RawDataBlkOff);} ;

  // Decode counter block
  virtual inline uint16_t getNumberOfHotCells() const;
  virtual inline int32_t getEx() const;
  virtual inline int32_t getEy() const;
  virtual inline int32_t getEz() const;
  virtual inline int16_t getHottestCellIndex();
  virtual inline int32_t getHottestCellEnergy() const;

  virtual inline int setGain(const int /*GainValue*/) { return 1; };
  
private:
  virtual void resetPointers();
  virtual bool setPointers();
  
  int m_EnergyIndex;
  int m_RawDataIndex;
  int m_TimeQualityIndex;
  int m_TowerIndex;
  const uint16_t* m_EnergyPointer;
  const uint16_t* m_TimeQualityPointer;
  const uint16_t* m_RawDataPointer;
  const uint32_t* m_GainPointer;
  const uint32_t* m_MaskPointer;
  const uint16_t* m_HotCellsPointer;
  const uint8_t*  m_TowerPointer;
  const int32_t*  m_SumPointer;

  //For fixed gain mode
  int m_fixedGain;
  //FIXME, very ugly hack! See explanation in LArRodDecoder.h 
public:
  inline uint16_t getFebConfig() const;
  inline uint16_t getFirstSampleIndex() const;

  virtual bool canSetCalibration() {return false;}
};

inline uint16_t LArRodBlockPhysicsV4::getFebConfig() const
{
  return getHeader16(FebConfig);
}

inline uint16_t LArRodBlockPhysicsV4::getFirstSampleIndex() const
{
  return getHeader16(FirstSampleIndex);
}

inline int LArRodBlockPhysicsV4::getNextEnergy(int& channelNumber,int32_t& energy,int32_t& time,int32_t& quality, uint32_t& gain)
{
  #ifdef LARBSDBGOUTPUT
  MsgStream logstr(Athena::getMessageSvc(), BlockType());
  #endif

  LARBSDBG("in LArRodBlockPhysicsV4::getNextEnergy.");
  LARBSDBG("m_channelsPerFEB=" << m_channelsPerFEB);
  if (m_EnergyIndex>=m_channelsPerFEB)                       // Already beyond maximal number of channels
    return 0;
  if (!m_EnergyPointer)                                               // No data block present
    return 0;
 
  unsigned rodChannelNumber=m_EnergyIndex;      // Index of Channel in ROD-Block
  channelNumber=((rodChannelNumber&0xe)<<2) + ((rodChannelNumber&0x1)<<6) + (rodChannelNumber>>4);    //channel number of the FEB
  //channelNumber=(rodChannelNumber>>4) + ((rodChannelNumber&0xf)<<3);    //channel number of the FEB

  // get information available for all cells
  // Energy on a 16 bit word and decode ranges
  uint16_t encodedEnergy;  // 16 bits Encoded Energy word
  int32_t  aux;  
  uint16_t range;          // 2 bits range
  uint16_t  sign;

  LARBSDBG("-------->>>> in LArRodBlockPhysicsV4::getNextEnergy : decode energy.....");
  // decode energy
  if(m_EnergyIndex & 0x1) encodedEnergy = m_EnergyPointer[m_EnergyIndex-1]; // Big/Little Endien stuff
  else                    encodedEnergy = m_EnergyPointer[m_EnergyIndex+1]; // Big/Little Endien stuff

  aux = (int32_t) (encodedEnergy&0x1fff); 
  range    = (encodedEnergy & 0xc000) >> 14;
  sign     =  encodedEnergy & 0x2000;
  aux    <<= 3*range;
  if(sign) aux = -aux;
  energy = aux; 
  
  // gain in 2 bits of a 32 bits word 
  gain = (uint32_t) ((m_GainPointer[m_EnergyIndex>>4] >> (m_EnergyIndex&0xf)*2) & 0x3);
  gain=RawToOfflineGain(gain);
  
  // Get Time and Quality if the information is present according to summary block
  uint32_t hasTQ = (uint32_t) ((m_MaskPointer[m_EnergyIndex>>5] >> (m_EnergyIndex&0x1f)) &0x1);
  m_EnergyIndex++;
  if (m_TimeQualityPointer && hasTQ) // Data has Time and Quality information 
    {
      m_TimeQualityIndex += getHeader16(NSamples)+1;
      time    = m_TimeQualityPointer[m_TimeQualityIndex++]; 
      quality = m_TimeQualityPointer[m_TimeQualityIndex++]; 

#ifdef LARBSDBGOUTPUT
      logstr << MYLEVEL <<"This cell has time and Quality information "<<endmsg;
#endif
    }
  else  // Data has no Time and Quality information
    {
      time=0;
      quality=-1;
    }


#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL <<"Range = "<<range<<endmsg;
  logstr << MYLEVEL <<"Sign = "<<sign<<endmsg;
  logstr << MYLEVEL <<" Encoded Energy ="<< MSG::hex << encodedEnergy << MSG::dec << " E=" << energy
	 << " t=" << time 
	 << " Q=" << quality 
	 << " G=" << gain 
	 << " channel Number=" << channelNumber
	 << endmsg;
#endif
 
  return 1;
}

inline uint16_t  LArRodBlockPhysicsV4::getNumberOfHotCells() const
{
  if(m_HotCellsPointer) return *m_HotCellsPointer;
  return 0;
}

inline int32_t  LArRodBlockPhysicsV4::getEx() const  // To be checked
{
  if(m_SumPointer) return m_SumPointer[0];
  return 0;
}

inline int32_t  LArRodBlockPhysicsV4::getEy() const  // To be checked
{
  if(m_SumPointer) return m_SumPointer[1];
  return 0;
}

inline int32_t LArRodBlockPhysicsV4::getEz() const
{
  if(m_SumPointer) return m_SumPointer[2];
  return 0;
}

inline int16_t  LArRodBlockPhysicsV4::getHottestCellIndex() // to be checked
{
  if(m_TowerPointer && m_TowerIndex<16) return m_TowerPointer[m_TowerIndex++]; 
  return -1;
}

inline int32_t  LArRodBlockPhysicsV4::getHottestCellEnergy() const  // to be checked
{
  uint16_t encodedEnergy;  // 16 bits Encoded Energy word
  int32_t  aux;  
  uint16_t range;          // 2 bits range
  int32_t  sign=1;

  if(!m_TowerPointer) return 0;
  encodedEnergy = m_EnergyPointer[m_TowerPointer[m_TowerIndex]];
  
  if (encodedEnergy&0x8000) sign=-1;
  range = ((encodedEnergy)>>13)&0x0003; // range is encoded in bits 14 and 13
  
  aux = (int32_t) (encodedEnergy&0x1FFF); 
  if (range==1)  aux=(aux<<3)+4;                 // shift left by 3 bits and add 4 MeV
  else if (range==2) aux=(aux<<6)+32;            // shift left by 6 bits and add 32 MeV
  else if (range==3) aux=(aux<<9)+256;           // shift left by 9 bits and add 256 MeV
  
  return sign*aux;
}

#ifdef LARBSDBGOUTPUT
#undef LARBSDBGOUTPUT
#endif
#undef LARBSDBG

#endif
