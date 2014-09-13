/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILEBCHDECODER_H
#define TILECALIBBLOBOBJS_TILEBCHDECODER_H

/** 
    @brief Class providing the association between TileCal problems and status word bits.
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details The status of each TileCal ADC is stored in the database
    as a combination of two 32 bit integers: One 32 bit word encodes
    channel problems common to both ADCs (stored in gain=2), and one
    32 bit word encodes ADC specific problems (gain=0 or 1). 

    Each bit is assigned to a specific problem. The association
    between bits and problems is generally not static and could change
    over time. This class provides a flexible mechanism to translate
    between problems and current bit pattern implementation. The
    different problems are encoded as enums (see TileBchPrbs class).

    One TileBchDecoder object should be instanciated for each bitPatternVersion.
*/

#include <stdint.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include "TileCalibBlobObjs/TileBchPrbs.h"
#include "TileCalibBlobObjs/TileBchStatus.h"
#include "TileCalibBlobObjs/TileBchWords.h"

class TileBchDecoder
{
 public:

  enum BitPatVer{
    BitPat_ofl01,
    BitPat_onl01,
    MaxVersion
  };
  
  /** @brief Ctor. */
  TileBchDecoder(BitPatVer bitPatVer);

  /** @brief Print status bits and their assigned problem. */
  void printBitAssignment() const { printBitAssignment(std::cout); }
  /** @brief Print status bits and their assigned problem. 
      @param stm The stream to print to */
  void printBitAssignment(std::ostream& stm) const;

  /** @brief Decode the status word */
  TileBchStatus decode(uint32_t status_channel, uint32_t status_adc=0) const;

  /** @brief Decode the status word */
  TileBchStatus decode(const TileBchWords& words) const
    { return decode(words[0],words[1]); }

  /** @brief Encode an ADC status into channel and adc status word bits.
      @details The returned vector is of length 2, where the first
      word encodes the common channel problems and the second word the
      specific adc problems.
      @param status The ADC status
  */
  TileBchWords encode(const TileBchStatus& status) const;
  
 private:
  
  /** @brief Initializes bit pattern version "ofl01". */
  void init_BitPat_ofl01();
  /** @brief Initializes bit pattern version "ofl01". */
  void init_BitPat_onl01();


 /** @brief Initializes the problem to bit map. */
  void initPrbToBit();

  /** @brief Bit to problem association for the channel word. */
  std::vector<TileBchPrbs::Prb>            m_bitToPrbChn;
  /** @brief Bit to problem association for the ADC word. */
  std::vector<TileBchPrbs::Prb>            m_bitToPrbAdc;
  /** @brief Problem to status word and bit association.  
      @details pair.first==0 means channel status word, pair.first==1
      means adc status word. pair.second is the bit number.*/
  std::map<TileBchPrbs::Prb, std::pair<unsigned int,unsigned int> > m_prbToBit;
};

#endif
