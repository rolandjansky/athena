//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRODBLOCKPYSICSV0_H
#define LARBYTESTREAM_LARRODBLOCKPYSICSV0_H

/** This class provides decoding/encoding from/to ROD format. 
   * @authors W. Lampl, R. Lafaye
   * @version  0-0-1 , Jan 5, 2004
   */
/** Short description of RodBlockPhysics:

    Content:
    LArDigits: only one gain, 16bit
    LArRawChannels: E in 16 bits without t, Q and gain
    or E in 32 bits, t in 14 bits, Q in 16 bits and 
    gain in 2 bits.
    Low Energies (16bit) and high energies (32bits) are 
    stored in different blocks. A map at the beginning of the 
    Low-Energy block tells in what block a certain channel 
    appears. 

    The Raw Data block has also a map that tells whether 
    the raw data for a certain channel is present or not.

*/

  

#include <stdint.h>
#include <vector>
#include "LArByteStream/LArRodBlockStructure.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"


//#define LARBSDBGOUTPUT
#ifdef  LARBSDBGOUTPUT
#define LARBSDBG(text) m_logstr<<MSG::DEBUG<<text<<endmsg
#else
#define LARBSDBG(text)
#endif

class LArRodBlockPhysicsV0 : public LArRodBlockStructure
{
public:
  // constructor
  LArRodBlockPhysicsV0();

protected:
  // ----------------- Header words indexes -----------------
  enum {
    NWTot,
    NWTot_h,
    FEBID,
    FEBID_h,
    FEB_SN,
    FEB_SN_h,
    LowEBlkOffset,
    HighEBlkOffset,
    RawDataBlkOffset,
    NGainNSamples,          //Elements below this are not used by the converter
    Status1,
    Status2,
    BCID,
    EventID,
    RADD1,
    RADD2,                 //Could have more RADD's in real data
    endtag
  };

 public:
  std::string BlockType() { return std::string("RodBlockPhysicsV0");}
  // ----------------- Encoding methods -----------------
  // Never to be used while decoding!
  //virtual void buildHeader();
  virtual void initializeFragment(std::vector<uint32_t>& fragment);
  virtual void initializeFEB(const uint32_t id);
  virtual void setNumberOfSamples(const uint8_t n);
  virtual void setNumberOfGains(const uint8_t n);
  virtual void setNextEnergy(const int channel, const int32_t energy, const int32_t time, const int32_t quality, const uint32_t gain);
  virtual void setRawData(const int channel, const std::vector<short>& samples, const uint32_t gain);
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

  // ----------------- Decoding methods -----------------
  // Never to be used while encoding!
  // set full ROD fragment before trying to get anything!
  // in case there is more than 1 FEB in 1 fragment, jump to next FEB
  virtual inline int getNextEnergy(int& channelNumber, int32_t& energy, int32_t& time,int32_t& quality,uint32_t& gain);
  virtual int  getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain);

  // ----------------- Printing methods -----------------
  // print the full ROD fragment
  //virtual void dumpFragment();
  virtual inline  uint32_t  hasPhysicsBlock() const {return LE_getHeader16(LowEBlkOffset)+LE_getHeader16(HighEBlkOffset);} ;
  virtual inline  uint32_t  hasRawDataBlock() const {return LE_getHeader16(RawDataBlkOffset);} ;

 private:
  virtual bool setPointers();
  void clearBlocks();
  virtual void resetPointers();
  //Separated blocks for encoding
  std::vector<uint32_t>  m_LowEnergyBlock;
  std::vector<uint32_t>  m_HighEnergyBlock;
  std::vector<uint32_t>  m_RawDataBlock;
  //Counter for channels inside of a FEB
  int m_ECounter;
  int m_RawDataCounter;
  int m_LowEIndex;
  int m_RawDataIndex;
  int m_HighEIndex;
  int m_EIndex; //For writing....
  const int32_t *m_HighEPtr; 
  const uint32_t *m_FlagPtr;
  const int16_t *m_LowEPtr;
  //Number of flagging words at the beginning of Block 2, 3, and 4 (Depends on the number of channels)
  unsigned short m_NFlaggingWords; 

  //LArCablingService* m_cablingSvc; 
  const LArOnlineID* m_onlineHelper;
  static const uint32_t m_DummyBitMap[4];
  //Private functions:
  inline int FebToRodChannel(int ch) const;
  void setNextEnergy(const int32_t energy, const int32_t time, const int32_t quality, const uint32_t gain);

  // log message 
  MsgStream m_logstr ; 
};

inline int LArRodBlockPhysicsV0::FebToRodChannel(int ch) const
  //{return ch/8 + 16 * (ch%8);}
{return (ch>>3) + ((ch&0x7)<<4);}

inline int LArRodBlockPhysicsV0::getNextEnergy(int& channelNumber,int32_t& energy,int32_t& time,int32_t& quality, uint32_t& gain)
{
  LARBSDBG("in LArRodBlockPhysicsV0::getEnergy.");
  LARBSDBG("m_ECounter=" << m_ECounter << "  m_RawDataIndex="<<  m_EIndex);
  LARBSDBG("m_channelsPerFEB=" << m_channelsPerFEB);
  if (m_LowEIndex+m_HighEIndex>=m_channelsPerFEB)                       //Already beyond maximal number of channels
    return 0;
  if (!m_FlagPtr || !m_LowEPtr)                                         //No data block present
    return 0;
  uint32_t gtQ;
  int rodChannelNumber=m_LowEIndex+m_HighEIndex;                        //Index of Channel in ROD-Block
  channelNumber=(rodChannelNumber>>4) + ((rodChannelNumber&0xf)<<3);    //channel number of the FEB
  if (getBit(m_FlagPtr,rodChannelNumber))                               //Data in high energy block
    {energy = m_HighEPtr[m_HighEIndex*2];
     gtQ=m_HighEPtr[m_HighEIndex*2+1];
     // Q in bits 0-15
     // t in bits 16-29
     // gain in bits 30-31
     quality  = gtQ & 0xffff;
     gtQ = gtQ >> 16;
     //time     = gtQ & 0x3fff;
     time = gtQ & 0x1fff;
     if (gtQ & 0x2000)
       time*=-1;
     gain=(gtQ >> 14);
     m_HighEIndex++;
     //std::cout << "Reading Raw data form High E block. E=" << energy << " Q=" << quality << std::endl;
    }
  else //Data in low energy block
    {if (!m_LowEPtr)
      return 0; //Block does not exist;
     energy=m_LowEPtr[m_LowEIndex];
     gain=0; //=CaloGain::LARHIGHGAIN
     time=0;
     quality=-1;
     m_LowEIndex++;
     //std::cout << "*** Reading Raw data form Low E block. E=" << energy << std::endl;
    }
  /*
   if (energy>10000000)
     std::cout << " Read: Large E found. E=" << energy
		 << " t=" << time 
		 << " Q=" << quality 
		 << " G=" << gain 
		 << " channel Number=" << channelNumber 
		 << " LowEIndex=" << m_LowEIndex
		 << " HighEIndex="<<m_HighEIndex << std::endl;
   else
     std::cout << "Read energy" << std::endl;
  */
  return 1;
}

#ifdef LARBSDBGOUTPUT
#undef LARBSDBGOUTPUT
#endif
#undef LARBSDBG

#endif
