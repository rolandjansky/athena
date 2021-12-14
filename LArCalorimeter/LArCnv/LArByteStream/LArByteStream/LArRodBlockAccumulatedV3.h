//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRODBLOCKACCUMULATEDV3_H
#define LARBYTESTREAM_LARRODBLOCKACCUMULATEDV3_H

/** This class provides decoding/encoding from/to ROD format. 
   * @authors W. Lampl, R. Lafaye
   * @version  0-0-1 , Jul 17, 2003
   *
   * Modified, 04.01.2006, IWS
   * new format with averages
   * Modified, 25.04.2006, IWS
   * new format with averages sub-steps 
   */

#include "LArByteStream/LArRodBlockStructure.h"
#include "CaloIdentifier/CaloGain.h"

class LArRodBlockAccumulatedV3 : public LArRodBlockStructure
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
    ResultsOff1,      // Size of results (Accumulated averages in DSP)
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
  enum{
    NTrigger,
    NTrigger_h,
    StepIndex,
    NStep
  };
  // constructor
  LArRodBlockAccumulatedV3();
 
  // ------ Identify RodBlockStructure -------
  std::string BlockType() { return std::string("RodBlockAccumulatedV3");}
 public:
  // ----------------- Encoding methods -----------------
  // Never to be used while decoding!
  // ----------------- Decoding methods -----------------
  // Never to be used while encoding!
  // set full ROD fragment before trying to get anything!
  // in case there is more than 1 FEB in 1 fragment, jump to next FEB
  virtual uint8_t getTDCPhase() const;
  virtual int  getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain);
  virtual int  getNextAccumulatedDigit(int& channelNumber, std::vector<uint64_t>& SamplesSum, std::vector < uint64_t >& corr2Sum, uint32_t& gain);
  virtual uint32_t getNumberOfSamples() const;
  virtual uint32_t getNumberOfGains() const;
  virtual uint32_t getRadd(uint32_t adc, uint32_t sample) const;
  virtual uint16_t getResults1Size() const;
  virtual uint16_t getResults2Size() const;
  virtual uint16_t getRawDataSize() const;
  virtual uint16_t getCtrl1(uint32_t adc) const;
  virtual uint16_t getCtrl2(uint32_t adc) const;
  virtual uint16_t getCtrl3(uint32_t adc) const;
  virtual uint32_t getStatus() const;

  virtual inline int setGain(const int GainValue); // ---------------- Define gain for decoding
  virtual inline  uint32_t  hasAccumBlock()   const {return getHeader16(ResultsOff1);} ; 
  virtual inline  uint32_t  hasRawDataBlock() const {return getHeader16(RawDataBlkOff);} ;
  virtual inline  uint32_t  hasControlWords() const {return getHeader16(RawDataBlkOff);} ;

  // ----------------- Printing methods -----------------
  // print the full ROD fragment
  //virtual void dumpFragment();
private:
  void clearBlocks();
  virtual void resetPointers();
  //Separated blocks for encoding
  
  //Counter for channels inside of a FEB
  int m_Result1Counter = 0;
  int m_Result1Index = 0;
  int m_Result2Counter = 0;
  int m_Result2Index = 0;
  int m_RawDataCounter = 0;
  int m_RawDataIndex = 0;
  //For fixed gain mode
  unsigned m_fixedGain;
  //FIXME, very ugly hack! See explanation in LArRodDecoder.h 
public:
  inline uint16_t getNTrigger() const;
  inline uint16_t getStepIndex() const;
  inline uint16_t getNStep() const;
  inline uint16_t getFebConfig() const;
  inline uint16_t getFirstSampleIndex() const;

  virtual bool canSetAccumulated() {return false;}
};


inline int LArRodBlockAccumulatedV3::setGain(const int GainValue)
{//This RodBlockStructure allows only predefined gains 
  /*
  std::cout << " Set fixed gain Value " << GainValue << std::endl;
  std::cout << " Number of words in Gain Block " << std::endl;
  std::cout << "\tHigh (1): " << getHeader16(NWRawData1) << std::endl;
  std::cout << "\tMedium (2): " << getHeader16(NWRawData2) << std::endl;
  std::cout << "\tLow (3): " << getHeader16(NWRawData3) << std::endl;
  */
  if (GainValue<0 || GainValue>3)
    return 0;
  m_fixedGain=GainValue;
  return 1;
}

inline uint16_t  LArRodBlockAccumulatedV3::getFebConfig() const
{
  return getHeader16(FebConfig);
}

inline uint16_t  LArRodBlockAccumulatedV3::getFirstSampleIndex() const
{
  return getHeader16(FirstSampleIndex);
}


#endif
