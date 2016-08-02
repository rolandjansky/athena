//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRODBLOCKTRANSPARENTV0_H
#define LARBYTESTREAM_LARRODBLOCKTRANSPARENTV0_H

/** This class provides decoding/encoding from/to ROD format. 
   * @authors W. Lampl, R. Lafaye
   * @version  0-0-1 , Jan 5, 2004
  
*/


/** Short description of RodBlockStructureTransparent:
    In this mode, the DSP copies the FEB-Data with 
    (almost) no changes. 
    This version hold digits (=ADC samples)
    taken in fixed-gain mode. 
    Content:
    Three blocks of LArDigits, one per gain, 
    16 bits per digit.
    There are maps to indicate absent data. 
*/


#include <stdint.h>
#include <vector>
#include "LArByteStream/LArRodBlockStructure.h"

class LArRodBlockHeaderTransparentV0
{ // ----------------- Header words indexes -----------------
public:
  enum {
    NWTot,            // First words: DSP event header
    NWTot_h,
    FEBID,
    FEBID_h,
    FEB_SN,           // FEB serial number
    FEB_SN_h,
    RawDataBlkOffset, // Raw FEB event offset
    RawDataBlkOffset_h,
    NGains,
    NSamples,   
    endtag
  };
};

template<class DSPHEADER>
class LArRodBlockTransparentV0 : public LArRodBlockStructure
{
public:
  // constructor
  LArRodBlockTransparentV0();
  // ------ Identify RodBlockStructure -------
  std::string BlockType() { return std::string("RodBlockTransparentV0");}
  // ----------------- Encoding methods -----------------
  // Never to be used while decoding!
  virtual void initializeFragment(std::vector<uint32_t>& fragment);
  virtual void initializeFEB(const uint32_t id);
  virtual void setNumberOfSamples(const uint8_t n);
  virtual void setNumberOfGains(const uint8_t n);
  virtual void setTDCPhase(const uint8_t n); //useful only TB SW validation 
  virtual void setRawDataFixed(const int channel, const std::vector<short>& samples, const uint32_t gain); 
  virtual void finalizeFEB();
  // build full ROD fragment
  virtual void concatinateFEBs( );
  // declare capabilities of this Rod Block Structure

   virtual bool canSetRawDataFixed() {return true;}
  // ----------------- Decoding methods -----------------
  // Never to be used while encoding!
  // set full ROD fragment before trying to get anything!
  // in case there is more than 1 FEB in 1 fragment, jump to next FEB
  virtual uint8_t getTDCPhase() const;
  virtual int  getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain);
  virtual inline int setGain(const int GainValue); // ---------------- Define gain for decoding
  virtual uint32_t getNumberOfSamples() const;
  virtual uint32_t getNumberOfGains() const;
  virtual uint32_t getRadd(uint32_t adc, uint32_t sample) const;
  virtual uint16_t getCtrl1(uint32_t adc) const;
  virtual uint16_t getCtrl2(uint32_t adc) const;
  virtual uint16_t getCtrl3(uint32_t adc) const;
  virtual uint32_t getStatus() const;

  // ----------------- Printing methods -----------------
  // print the full ROD fragment
  //virtual void dumpFragment();
  virtual inline  uint32_t  hasRawDataBlock() const {return m_FebBlock[3];} ;
  virtual inline  uint32_t  hasControlWords() const {return m_FebBlock[3];} ;

 private:
  void clearBlocks();
  virtual void resetPointers();
  //Separated blocks for encoding

  // One raw data block per gain to start with
  std::vector<uint32_t> m_RawDataBlock;
  //Counter for channels inside of a FEB
  int m_RawDataCounter;
  int m_RawDataIndex;
  unsigned m_BlkOffset; //Offset taken from enum
  //For fixed gain mode
  int m_fixedGain;
  //FIXME, very ugly hack! See explanation in LArRodDecoder.h 
public:
};


template<class DSPHEADER>
inline int LArRodBlockTransparentV0<DSPHEADER>::setGain(const int GainValue)
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

#include "LArByteStream/LArRodBlockTransparentV0.icc"

#endif
