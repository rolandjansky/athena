//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRODBLOCKCALIBRATIONV2_H
#define LARBYTESTREAM_LARRODBLOCKCALIBRATIONV2_H

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

class LArRodBlockCalibrationV2 : public LArRodBlockStructure
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
    ResultsOff1,      // Size of results (Calibration averages in DSP)
    ResultsDim1,      // Offset to results
    ResultsOff2,      // Size of times (in physics)
    ResultsDim2,      // Offset to times (in physics)
    RawDataBlkOff,
    RawDataBlkDim, // Raw FEB event offset
    EventStatus,      // Bits describing the event
    EventStatus_h,      
    NGains,
    NSamples,   
    FebConfig,
    FebConfig_h,
    InFPGAFormat, // added 08.09.2005 - wrong 28.09.2005?
    InFPGAFormat_h
  };
  enum{
    NTrigger,
    NTrigger_h,
    Dac,
    Dac_h,
    Delay,
    Delay_h,
    IsPulsed,
    IsPulsed1,
    IsPulsed2,
    IsPulsed3,
    IsPulsed4,
    IsPulsed5,
    IsPulsed6,
    IsPulsed7,
    StepIndex,
    StepIndex_h,
    feb_ssw,  
    feb_ssw1,
    feb_ssw2,
    feb_ssw3,
    feb_ssw4,
    feb_ssw5,
    feb_ssw6,
    feb_ssw7,
    endtag          //This tag needs to be an odd number, see *) for constructor
  };
  // constructor
  LArRodBlockCalibrationV2();
 
  // ------ Identify RodBlockStructure -------
  std::string BlockType() { return std::string("RodBlockCalibrationV2");}
 public:
  //void dumpFragment() { dumpFragment(m_FebBlock); }
  // ----------------- Encoding methods -----------------
  // Never to be used while decoding!
  // ----------------- Decoding methods -----------------
  // Never to be used while encoding!
  // set full ROD fragment before trying to get anything!
  // in case there is more than 1 FEB in 1 fragment, jump to next FEB
  virtual uint8_t getTDCPhase() const;
  virtual int  getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain);
  using LArRodBlockStructure::getNextAccumulatedCalibDigit;
  virtual int  getNextAccumulatedCalibDigit(int& channelNumber,  std::vector< std::vector < uint32_t > >& samplesSum,  std::vector< std::vector < uint32_t > >& samples2Sum, uint32_t& iStepTrigger, uint32_t& gain);
  virtual uint32_t getNumberOfSamples() const;
  virtual uint32_t getNumberOfGains() const;
  virtual uint32_t getRadd(uint32_t adc, uint32_t sample) const;
  virtual uint16_t getCtrl1(uint32_t adc) const;
  virtual uint16_t getCtrl2(uint32_t adc) const;
  virtual uint16_t getCtrl3(uint32_t adc) const;
  virtual uint32_t getStatus() const;

  virtual inline  uint32_t  hasCalibBlock()   const {return getHeader16(ResultsOff1);} ; 
  virtual inline  uint32_t  hasPhysicsBlock() const {return getHeader16(ResultsOff2);} ;
  virtual inline  uint32_t  hasRawDataBlock() const {return getHeader16(RawDataBlkOff);} ;
  virtual inline  uint32_t  hasControlWords() const {return getHeader16(RawDataBlkOff);} ;

  // ----------------- Printing methods -----------------
  // print the full ROD fragment
  //virtual void dumpFragment();
private:
  void clearBlocks();
  virtual void resetPointers();
  //Separated blocks for encoding
  
  // One raw data block per gain to start with
  std::vector<uint32_t> m_RawDataBlock;
  //Counter for channels inside of a FEB
  int m_Result1Counter = 0;
  int m_Result1Index = 0;
  int m_Result2Counter = 0;
  int m_Result2Index = 0;
  int m_RawDataCounter = 0;
  int m_RawDataIndex = 0;
  //For fixed gain mode
  int m_fixedGain;
  //FIXME, very ugly hack! See explanation in LArRodDecoder.h 
public:
  inline uint16_t getNTrigger() const;
  inline uint16_t getDAC() const;
  inline uint16_t getDelay() const;
  inline uint32_t getFebConfig() const;
  inline bool getPulsed(const unsigned channelNumber) const;

  virtual bool canSetCalibration() {return false;}
};


inline uint32_t  LArRodBlockCalibrationV2::getFebConfig() const
{
  return getHeader32(FebConfig);
}

inline bool  LArRodBlockCalibrationV2::getPulsed(const unsigned channelNumber) const
{
  int index=getHeader16(ResultsOff1);
  //std::cout << " ===> in getPulsed " << std::hex << (m_FebBlock[index+IsPulsed/2]) << " " << (m_FebBlock[index+(IsPulsed/2+1)]) << " " << (m_FebBlock[index+(IsPulsed/2+2)]) << " " << (m_FebBlock[index+(IsPulsed/2+3)]) << " " << std::dec << channelNumber << " i=" << i << " ii=" << ii << std::endl;
  return getBit(m_FebBlock+index+IsPulsed/2,channelNumber);
}



#endif
