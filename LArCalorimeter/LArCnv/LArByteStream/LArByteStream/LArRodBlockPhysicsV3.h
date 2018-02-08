//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRODBLOCKPYSICSV3_H
#define LARBYTESTREAM_LARRODBLOCKPYSICSV3_H

/** This class provides decoding/encoding from/to ROD format. 
   * @authors B. Laforge 
   * @version  0-0-1 , April 11, 2005
   */

/** Short description of RodBlockPhysics:

    Content:
    LArDigits: only one gain, 16bit

    LArRawChannels: 
    - All E in 16 bits but written in 32 bits words without t, Q and gain
    - time in 12 bits, Quality in 4 bits 
    - gain in 2 bits per cell in 32 bits words.
    All energies (16 bits) are stored in a single block. 
    A map at the beginning of the Energy block tells in 
    which cell have time and quality information in 
    the time and Quality block.

    The Raw Data block has also a map that tells whether 
    the raw data for a certain channel is present or not.
*/

#include <stdint.h>
#include <vector>
#include "LArByteStream/LArRodBlockStructure.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"


#undef  LARBSDBGOUTPUT
#ifdef  LARBSDBGOUTPUT
#define LARBSDBG(text) m_logstr<<MSG::DEBUG<<text<<endmsg
#else
#define LARBSDBG(text)
#endif

#define MYLEVEL (MSG::VERBOSE)

class LArRodBlockPhysicsV3 : public LArRodBlockStructure
{
public:
  // constructor
  LArRodBlockPhysicsV3();

protected:
  // ----------------- Header words indexes ----------------- Every word is a 16 bits word in the header
  enum {
    NWTot,
    NWTot_h,
    FEBID,
    FEBID_h,
    FEB_SN,
    FEB_SN_h,
    NGainNSamples,         // number of samples sent by the FEB and used at the raw data level in the dsp to apply Optimal Filtering
    SumBlkOffset,          // summary block offset (fixed length)
    CounterBlkOffset,      // counter block offset (fixed length)
    EBlkOffset,            // Energy block offset (fixed length)
    GainBlkOffset,         // Gain block offset (fixed length)
    FebInfoBlkOffset,      // Technical FEB information usefull for offline correction
    TimeQualityBlkOffset,  // Time and quality variable length block
    RawDataBlkOffset,      // Raw data block offset
    Status2,               // This element and the ones below this are not used by the converter
    Status1,
    BCID,
    EventID,
    //    endtag1, // remove if the number of words is even
    endtag
  };

 public:
  std::string BlockType() { return std::string("RodBlockPhysicsV3");}
  // ----------------- Encoding methods -----------------
  // Never to be used while decoding!
  //virtual void buildHeader();
  virtual void initializeFragment(std::vector<uint32_t>& fragment);
  virtual void initializeFEB(const uint32_t id);
  virtual void setNumberOfSamples(const uint8_t n);
  virtual void setNumberOfGains(const uint8_t n);
  virtual void setNextEnergy(const int channel, const int32_t energy, const int32_t time, const int32_t quality, const uint32_t gain);
  virtual void setRawData(const int channel, const std::vector<short>& samples, const uint32_t gain);
  virtual void setEx(double Ex);
  virtual void setEy(double Ey);
  virtual void setEz(double Ez);
  virtual void finalizeFEB();
  // build full ROD fragment
  virtual void concatinateFEBs();
  //Function to sort RawData Container before encoding:
  using LArRodBlockStructure::sortDataVector;  // avoid warnings.
  virtual void sortDataVector(std::vector<const LArRawChannel*>& );
  virtual void sortDataVector( std::vector<const LArDigit*>& );
  // declare capabilities of this Rod Block Structure
  virtual bool canSetEnergy() { return true;}
  virtual bool canSetRawData() {return true;}
  //Ordering relation for channels & digits
  template<class RAWDATA>
  bool operator () (const RAWDATA* ch1, const RAWDATA* ch2) const;

  virtual void setEThreshold(uint16_t thres);
  virtual void setOffTimeCut(uint16_t TimeCut);

  // ----------------- Decoding methods -----------------
  // Never to be used while encoding!
  // set full ROD fragment before trying to get anything!
  // in case there is more than 1 FEB in 1 fragment, jump to next FEB
  virtual inline int setGain(const int /*GainValue*/) { return 1; };
  virtual bool setPointers();
  virtual inline int getNextEnergy(int& channelNumber, int32_t& energy, int32_t& time,int32_t& quality,uint32_t& gain);
  virtual int  getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain);
  virtual uint32_t getNumberOfSamples() const;
  virtual uint32_t getNumberOfGains() const;
  virtual uint32_t getRadd(uint32_t adc, uint32_t sample) const;
  virtual uint16_t getCtrl1(uint32_t adc) const;
  virtual uint16_t getCtrl2(uint32_t adc) const;
  virtual uint16_t getCtrl3(uint32_t adc) const;
  virtual uint32_t getStatus() const;

  // Decode counter block
  virtual inline uint16_t getNumberOfHotCells() const;
  virtual inline uint16_t getNumberOfHotCellsInTime() const;
  virtual inline uint16_t getHotCellThreshold() const;
  virtual inline uint16_t getOfftimeThreshold() const;
  virtual inline int32_t getEx() const;
  virtual inline int32_t getEy() const;
  virtual inline int32_t getEz() const;
  virtual inline uint16_t getHottestCellIndex();
  virtual inline uint32_t getHottestCellEnergy() const;

  // ----------------- Printing methods -----------------
  // print the full ROD fragment
  // virtual void dumpFragment();
  virtual inline  uint32_t  hasPhysicsBlock() const {return LE_getHeader16(EBlkOffset);} ;
  virtual inline  uint32_t  hasRawDataBlock() const {return LE_getHeader16(RawDataBlkOffset);} ;
  virtual inline  uint32_t  hasControlWordBlock() const {return LE_getHeader16(RawDataBlkOffset);} ;

 private:
  void clearBlocks();
  virtual void resetPointers();
  //Separated blocks for encoding
     //    SumBlkOffset,          // summary block offset (fixed length)
     //    CounterBlkOffset,      // counter block offset (fixed length)
     //    EBlkOffset,            // Energy block offset (fixed length)
     //    GainBlkOffset,         // Gain block offset (fixed length)
     //    TimeQualityBlkOffset,  // Time and quality variable length block
     //    RawDataBlkOffset,      // Raw data block offset (How do we know which cell is concerned ?)

  std::vector<uint32_t>  m_SumBlkBlock;
  std::vector<uint32_t>  m_CounterBlkBlock;
  std::vector<uint32_t>  m_EnergyBlock;
  std::vector<uint32_t>  m_GainBlock;
  std::vector<uint32_t>  m_FebInfoBlock;
  std::vector<uint32_t>  m_TimeQualityBlock;
  std::vector<uint32_t>  m_RawDataBlock;

  // Counter for channels inside of a FEB

  int m_RawDataCounter;
  int m_RawDataIndex;
  int m_CounterIndex;
  int m_EnergyIndex;
  int m_FebInfoIndex;
  int m_GainIndex;
  int m_TimeQualityIndex; 
  int m_SumBlkIndex; //For writing...

  const uint32_t *m_SumBlkPtr; 
  const uint16_t *m_CounterPtr;
  const uint16_t *m_EnergyPtr;
  const int16_t *m_TimeQualityPtr;
  const uint32_t *m_GainPtr; 
  const int16_t  *m_FebInfoDataPtr; 
  const int16_t  *m_RawDataPtr; 
  const uint32_t *m_RawDataFlagsPtr; 

  int m_NSumBlkWords;
  int m_NCounterBlkWords;
  int m_NEnergyBlkWords;
  int m_NGainBlkWords;
  int m_NFebInfoBlkWords;

  int m_NFlaggingWords;

  uint16_t m_numberHotCell; 
  uint16_t m_numberHotCellOffTime;
  uint16_t m_EnergyThreshold;
  int16_t m_OffTimeCut;
  uint16_t m_HottestCellIndex;
  uint32_t m_HottestCellEnergy; // is it a problem that this energy has to be positive ? I hope not !

  //LArCablingService* m_cablingSvc; 
  const LArOnlineID* m_onlineHelper;
  static const uint32_t m_DummyBitMap[4];
  //Private functions:
  inline int FebToRodChannel(int ch) const;
  void setNextEnergy(const uint16_t energy,const int16_t time, const int16_t quality, const uint32_t gain);
  //  void setNextEnergy(const int32_t energy, const int32_t time, const int32_t quality, const uint32_t gain);

  MsgStream m_logstr;

};

inline int LArRodBlockPhysicsV3::FebToRodChannel(int ch) const
//{return ch/8 + 16 * (ch%8);}
{return (ch>>3) + ((ch&0x7)<<4);
//	return ch;
}

inline int LArRodBlockPhysicsV3::getNextEnergy(int& channelNumber,int32_t& energy,int32_t& time,int32_t& quality, uint32_t& gain)
{
  LARBSDBG("in LArRodBlockPhysicsV3::getNextEnergy.");
  LARBSDBG("m_channelsPerFEB=" << m_channelsPerFEB);
  if (m_EnergyIndex>=m_channelsPerFEB)                       //Already beyond maximal number of channels
    return 0;
  if (!m_EnergyPtr)                                          //No data block present
    return 0;
  uint16_t tQ; // TimeQuality word
 
  unsigned rodChannelNumber=m_EnergyIndex;    // Index of Channel in ROD-Block
  // channelNumber=rodChannelNumber;             // Arno claims he is using FEB numbering
  channelNumber=(rodChannelNumber>>4) + ((rodChannelNumber&0xf)<<3);    //channel number of the FEB

  // get information available for all cells
  // Energy on a 16 bit word and decode ranges

  uint16_t encodedEnergy;  // 16 bits Encoded Energy word
  int32_t aux;  
  uint16_t range;        // 2 bits range
  int32_t sign=1;

  LARBSDBG("-------->>>> in LArRodBlockPhysicsV3::getNextEnergy : decode energy.....");
  // decode energy
  encodedEnergy = m_EnergyPtr[m_EnergyIndex];
  m_EnergyIndex++;

  if (encodedEnergy&0x8000) sign=-1;
  range = ((encodedEnergy)>>13)&0x0003; // range is encoded in bits 14 and 13

  aux = (int32_t) (encodedEnergy&0x1FFF); 
  if (range==1)  aux=(aux<<3)+4;                 // shift left by 3 bits and add 4 MeV
  else if (range==2) aux=(aux<<6)+32;                // shift left by 6 bits and add 32 MeV
  else if (range==3) aux=(aux<<9)+256;               // shift left by 9 bits and add 256 MeV
  energy = sign*aux; 
 
  // gain in 2 bits of a 32 bits word 

  gain=(uint32_t)((m_GainPtr[channelNumber/16] >> (channelNumber%16)*2) & 0x3);

  // Decode Time and Quality if the information is present according to summary block

  if (getBit(m_SumBlkPtr,rodChannelNumber)) // Data has Time and Quality information 
    {
      tQ=m_TimeQualityPtr[m_TimeQualityIndex]; 

#ifdef LARBSDBGOUTPUT
      m_logstr <<MSG::DEBUG<<"This cell has time and Quality information "<<endmsg;
#endif
	
      // Decode Time, Quality
      // Q in bits 0-3
      // t in bits 4-15
      
      quality  = (int32_t) (tQ & 0x003f);
      tQ = tQ>>4;
      time = (tQ<<4) + 8; // unit is 16 ps
      m_TimeQualityIndex++;
    }
  else  // Data has no Time and Quality information
    {
      time=0;
      quality=-1;
    }


#ifdef LARBSDBGOUTPUT
  m_logstr <<MSG::DEBUG<<"Range = "<<range<<endmsg;
  m_logstr <<MSG::DEBUG<<"Sign = "<<sign<<endmsg;
  m_logstr << MSG::DEBUG<<" Encoded Energy ="<< encodedEnergy << " E=" << energy
	 << " t=" << time 
	 << " Q=" << quality 
	 << " G=" << gain 
	 << " channel Number=" << channelNumber
	 << endmsg;
#endif
 
  return 1;
}

inline uint16_t  LArRodBlockPhysicsV3::getNumberOfHotCells() const
{
  return (uint16_t) (*m_CounterPtr);
}
inline uint16_t  LArRodBlockPhysicsV3::getNumberOfHotCellsInTime() const
{
  return (uint16_t) *(m_CounterPtr+1);
}

inline uint16_t  LArRodBlockPhysicsV3::getHotCellThreshold() const
{
  return (uint16_t) (*(m_CounterPtr+2));
}
inline uint16_t  LArRodBlockPhysicsV3::getOfftimeThreshold() const
{
  return (uint16_t) (*(m_CounterPtr+3));
}

inline int32_t  LArRodBlockPhysicsV3::getEx() const  // To be checked
{
/*  int32_t ex;
  uint16_t aux;
  aux = *(m_CounterPtr+4);
  ex = (int32_t)((*(m_CounterPtr+5)<<16)|(aux & (~(1<<16))));
  ex=(ex&0x01FFFFFF);
  if ((*(m_CounterPtr+5))&(1<<15)) // number id negative
    ex = -ex;
  return ex;*/
  uint32_t* copy32u = (uint32_t*)(m_CounterPtr+4);
  int32_t* ex = (int32_t*)copy32u;
  return *ex;
}

inline int32_t  LArRodBlockPhysicsV3::getEy() const  // To be checked
{
/*  int32_t ey;
  uint16_t aux;
  aux = *(m_CounterPtr+6);
  ey = (int32_t)((*(m_CounterPtr+7)<<16)|(aux & (~(1<<16))));
  ey=(ey&0x01FFFFFF);
  if ((*(m_CounterPtr+7))&(1<<15)) // number id negative
    ey = -ey;
  return ey;*/
  uint32_t* copy32u = (uint32_t*)(m_CounterPtr+6);
  int32_t* ey = (int32_t*)copy32u;
  return *ey;
}

inline int32_t LArRodBlockPhysicsV3::getEz() const
{
  uint32_t* aux = (uint32_t*)(m_CounterPtr+10);
  int32_t* ez = (int32_t*)aux;
  return *ez;
}

inline uint16_t  LArRodBlockPhysicsV3::getHottestCellIndex() // to be checked
{
  return ((uint16_t) (*(m_CounterPtr+8)>>9));
}

inline uint32_t  LArRodBlockPhysicsV3::getHottestCellEnergy() const  // to be checked
{

  uint32_t aux;
  aux = * ((uint32_t *)(m_CounterPtr+8));
  return (aux&0x01FFFFFF);
}



#ifdef LARBSDBGOUTPUT
#undef LARBSDBGOUTPUT
#endif
#undef LARBSDBG

#endif
