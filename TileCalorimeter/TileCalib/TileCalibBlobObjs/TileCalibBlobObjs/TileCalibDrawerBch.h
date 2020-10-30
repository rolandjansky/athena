/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILECALIBDRAWERBCH_H
#define TILECALIBBLOBOBJS_TILECALIBDRAWERBCH_H

/** 
    @brief Class for storing a 32 bit status word for each ADC.
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details This class stores a 32 bit (uint32_t) status word for
    each ADC. A "0" here means that the ADC is in good condition,
    non-zero values indicate problems. 

    The blobObjVersion variable contains the bitPatternVersion. See
    the TileBchDecoder class for more details and the interpretation of
    bits.
*/

#include "TileCalibBlobObjs/TileCalibDrawerDat.h"
#include "TileCalibBlobObjs/TileCalibType.h"
#include "TileCalibBlobObjs/TileBchDecoder.h"
#include <stdint.h>
#include <vector>
#include "CoralBase/Blob.h"

class TileCalibDrawerBch : public TileCalibDrawerDat<uint32_t>
{
 public:

  /** @brief Dtor */
  virtual ~TileCalibDrawerBch(){}
  
  /** @brief Returns TileCalibType::BCH. */
  virtual uint16_t getType() const {return TileCalibType::BCH;} 
  
  /** @brief Returns a pointer to a const TileCalibDrawerBch. */
  static const TileCalibDrawerBch* getInstance(const coral::Blob& blob);
  
  /** @brief Returns a pointer to a non-const TileCalibDrawerBch. 
      @param blob Reference to a coral::Blob
      @param def A reference to a DefType object, specifying the inital layout and values
      @param nChans Number of channels to create
      @param bitPatVer Bit pattern version used for problem encoding
      @param author String containing the author user id
      @param comment String containing the actual comment
      @param timeStamp Unix time stamp (if 0, current time is used) */
  static TileCalibDrawerBch* getInstance(coral::Blob&              blob,
					 const DefType&            def, 
					 uint16_t                  nChans,
					 TileBchDecoder::BitPatVer bitPatVer,
					 const std::string&        author="",
					 const std::string&        comment="",
					 uint64_t                  timeStamp=0);

  /** @brief Returns the bit pattern version. */
  TileBchDecoder::BitPatVer getBitPatternVersion() const;

  /** @brief Extracts the adc and channel status words
      @param channel The channel index
      @param adc The gain index 
      @param adcStatus Filled with adc status word
      @param chnStatus Filled with channel status word */
  void getStatusWords(unsigned int channel, unsigned int adc, 
		      uint32_t& adcStatus, uint32_t& chnStatus) const;
  
 protected:
  /** @brief Ctor (const). */
  TileCalibDrawerBch(const coral::Blob& blob);
  /** @brief Ctor (non-const). */
  TileCalibDrawerBch(coral::Blob& blob);
};

//
//______________________________________________________________________
inline TileBchDecoder::BitPatVer
TileCalibDrawerBch::getBitPatternVersion() const
{
  return static_cast<TileBchDecoder::BitPatVer>(getObjVersion());
}

//
//______________________________________________________________________
inline void
TileCalibDrawerBch::getStatusWords(unsigned int channel, unsigned int adc, 
				   uint32_t& adcStatus, uint32_t& chnStatus) const
{
  adcStatus = getData(channel,adc,0);
  chnStatus = getData(channel,  2,0);
}

#endif
