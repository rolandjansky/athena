//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRODBLOCKCALIBRATIONV0_H
#define LARBYTESTREAM_LARRODBLOCKCALIBRATIONV0_H

/** This class provides decoding/encoding from/to ROD format. 
   * @authors W. Lampl, R. Lafaye
   * @version  0-0-1 , Jul 17, 2003
   *
   * Modified, 8.9.2003, W.Lampl
   * make const-correct
   * and get-functions inline to 
   * increase performance
   */

#include "LArByteStream/LArRodBlockTransparentV0.h"

class LArRodBlockHeaderCalibrationV0
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
    RawDataBlkOffset, // Raw FEB event offset
    RawDataBlkOffset_h,
    NGains,
    NSamples,   
    Dac,
    Delay,
    IsPulsed,
    IsPulsed1,
    IsPulsed2,
    IsPulsed3,
    IsPulsed4,
    IsPulsed5,
    IsPulsed6,
    IsPulsed7,
    endtag             //This tag needs to be an odd number, see *) for constructor
  };
};

template<class DSPHEADER> 
class LArRodBlockCalibrationV0 : public LArRodBlockTransparentV0<DSPHEADER>
{
public:
  // constructor
  LArRodBlockCalibrationV0() : LArRodBlockTransparentV0<DSPHEADER>()
    {};
 
 public:
  //void dumpFragment() { dumpFragment(m_FebBlock); }
  // ----------------- Encoding methods -----------------
  virtual void setDAC(const uint16_t DacValue);
  virtual void setDelay(const uint16_t delayValue);
  virtual void setPulsed(const unsigned channelNumber);
  // ----------------- Decoding methods -----------------
  inline uint16_t getDAC() const;
  inline uint16_t getDelay() const;
  inline bool getPulsed(const unsigned channelNumber) const;

  virtual inline  uint32_t  hasRawDataBlock() const {return 1;} 

  virtual bool canSetCalibration() {return true;}
};

template<class DSPHEADER>
inline uint16_t LArRodBlockCalibrationV0<DSPHEADER>::getDAC() const
{
  return this->getHeader16(DSPHEADER::Dac);
}

template<class DSPHEADER>
inline uint16_t  LArRodBlockCalibrationV0<DSPHEADER>::getDelay() const
{
  return this->getHeader16(DSPHEADER::Delay);
}

template<class DSPHEADER>
inline bool  LArRodBlockCalibrationV0<DSPHEADER>::getPulsed(const unsigned channelNumber) const
{return this->getBit(this->m_FebBlock+(DSPHEADER::IsPulsed/2),channelNumber);}


template<class DSPHEADER>
inline void LArRodBlockCalibrationV0<DSPHEADER>::setDelay(const uint16_t delayValue)
{this->setHeader16(DSPHEADER::Delay,delayValue);}

template<class DSPHEADER>
inline void LArRodBlockCalibrationV0<DSPHEADER>::setDAC(const uint16_t dacValue)
{this->setHeader16(DSPHEADER::Dac,dacValue);
//std::cout << "Set DAC-Value to " << dacValue << std::endl;
}


template<class DSPHEADER>
inline void  LArRodBlockCalibrationV0<DSPHEADER>::setPulsed(const unsigned channelNumber)
{ 
#ifdef LARBYTESTREAMRODBLOCK_CHCKBOUNDARIES
  if (channelNumber>=128) {
    std::cout << "ERROR WRITE BEYOND ARRAY BONDARY!" << std::endl;
    exit(-1);
  }
#endif
  this->setBit((uint32_t*) (&(this->m_vFragment->front())+(DSPHEADER::IsPulsed/2)),channelNumber);
}

#endif
