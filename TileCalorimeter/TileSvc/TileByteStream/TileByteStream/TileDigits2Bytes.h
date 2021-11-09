/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEDIGITS2BYTES_H
#define TILEBYTESTREAM_TILEDIGITS2BYTES_H

#include <vector>
#include <stdint.h> 
#include <array>
class TileDigits;
class TileHWID;

/**
 * @class TileDigits2Bytes
 * @brief This class converts TileDigits to and from bytestream in ROD format
 * @author Alexander Solodkov
 */

class TileDigits2Bytes {

 public:

  TileDigits2Bytes() : m_verbose(false) {}
  int getBytes(const TileDigits* digi, const TileHWID* tileHWID, std::vector<unsigned int>& v);
  /** 
   * Extract samples(digits) for 3 channels, stored in 9 words
   * @param data Pointer to ROD-data
   */
  std::array< std::vector<float>, 3 > getDigits(const uint32_t *data, int dataWordsPerChip) const;
  /**
   * Get BCID from Chip header, bit 0-11
   * @param data Pointer to first word
   * @param chipCount Number of chips
   * @param wordsPerChip Number of data words per chip
   * @return BCID from chip header with good parity
   */
  int getBCID(const uint32_t *data, int chipCount, int wordsPerChip) const;

  /**
   * Determine digitizer mode for a number
   * of channels.
   * @param data Pointer to first word
   * @param chipCount Number of chips
   * @param wordsPerChip Number of data words per chip
   * @return Digitizer mode
   */
  int getDigiMode(const uint32_t *data, int chipCount, int wordsPerChip) const;
  
  /**
   * Extract gain for <chip> in chip header
   * @param data Pointer to chip header word
   * @param chan is channel number in chip (0-2)
   * return 0-low gain, 1-high gain
   */
  int getGain(const uint32_t *data, int chan) const;

  /**
   * Verify ODD parity for one word
   * @param data Pointer to data word
   * @return If word has even parity (bool)
   */
  inline bool checkWordParity(const uint32_t *data) const;
  
  /**
   * Calculate parity for one word
   * @param data Pointer to word
   * @return Parity as LSB
   */
  inline uint32_t calculateParity(const uint32_t *data) const;

  /**
   * Verify parity for <length> words.
   * The parity bit for each word is stored in an unsigned int
   * LSB is parity bit from first word and so on.
   * @param data Pointer to first data word
   * @param length Number of data words
   * @return Result of check.
   */
  inline uint32_t checkParity(const uint32_t *data, int length) const;

  /**
   * Check that the MSB of <length> words are equal to
   * LSB of <startbit>
   * @param data Pointer to forst data word
   * @param length Number of words to check
   * @param startbit Word with LSB to check against
   * @return If all words were correct
   */
  inline bool checkStartBit(const uint32_t *data, int length, uint32_t startbit) const;
  
  inline void setVerbose(bool verbose) { m_verbose=verbose; }  
        
 private:

  bool m_verbose;
};

#endif
