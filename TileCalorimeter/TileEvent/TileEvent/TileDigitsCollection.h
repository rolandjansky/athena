/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileDigitsCollection_H
#define TileDigitsCollection_H

#include "TileEvent/TileDigits.h"
#include "TileEvent/TileRawDataCollection.h"

// STL include
#include <vector>

// Lang include
#include <inttypes.h>

class TileDigitsCollection : public TileRawDataCollection<TileDigits>
{
  friend class TileDigitsContainerCnv;
public:

  typedef TileRawDataCollection<TileDigits> MyBase;
  typedef MyBase::ID ID;
  typedef TileDigits _TElement;

  TileDigitsCollection ( ID id, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS  )
    : MyBase(id,ownPolicy), m_FragSize(0), m_FragBCID(0) { m_FragExtraWords.resize(2); }
  // Alternate constructor for use with ConstDataVector.
  TileDigitsCollection ( SG::OwnershipPolicy ownPolicy, ID id  )
    : MyBase(id,ownPolicy), m_FragSize(0), m_FragBCID(0) { m_FragExtraWords.resize(2); }
  TileDigitsCollection ( SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) 
    : MyBase(ownPolicy), m_FragSize(0), m_FragBCID(0) { m_FragExtraWords.resize(2); }

  ~TileDigitsCollection() { }

  /**
   * Clear everything for next event
   */
  void clear ();

  /**
   * Check if this Collection holds data from calibration mode
   * @return true if calibration mode
   */
  bool isCalibMode() const { return (size() == 96); }
  /**
   * Set Frag size
   * @param s Size of rod in words
   */
  void setFragSize(uint32_t s) { m_FragSize = s; }
  /**
   * Get Frag size
   * @return Frag size in words
   */
  inline uint32_t getFragSize() const { return m_FragSize; }
  /**
   * Set vector of extra words for this collection
   * @param extra - all words from frag trailer
   */
  void setFragExtraWords(std::vector<uint32_t> & extra) { m_FragExtraWords = extra;
                                  if (extra.size() < 2) m_FragExtraWords.resize(2); }
  /**
   * Get Frag extra words for this collection
   * @return vector with all words
   */
  inline std::vector<uint32_t> getFragExtraWords() const { return m_FragExtraWords; }
  inline uint32_t              getFragExtraWord(unsigned int i) const { 
    if (i<m_FragExtraWords.size()) return m_FragExtraWords[i]; else return 0; }
  /**
   * Get Frag DMU mask for this collection
   * @return DMU mask
   */
  inline uint32_t getFragDMUMask() const { return m_FragExtraWords[0]; }
  /**
   * Set CRC word for collection
   * @param crc CRC word from Frag Trailer
   */
  inline uint32_t getFragCRC() const { return m_FragExtraWords[1]; }
  /**
   * Set BCID
   * @param bcid BCID
   */
  void setFragBCID(uint32_t bcid) { m_FragBCID = bcid; }
  /**
   * Get BCID
   * @return BCID
   */
  inline uint32_t getFragBCID() const { return (m_FragBCID & 0xFFFF); }
  inline uint32_t getFragStatus() const { return (m_FragBCID >> 16); }

  /**
   * Set chip header words for this collection
   * If calib mode, this must be LOW gain headers
   * @param chipHWords Reference to vector filled with 16 Chip header words
   */
  void setFragChipHeaderWords(std::vector<uint32_t>& chipHWords) {
        m_FragChipHeaderWords = chipHWords;
  }
  /**
   * Header words in calib mode
   */
  void setFragChipHeaderWordsHigh(std::vector<uint32_t>& chipHWordsH) {
        m_FragChipHeaderWordsHIGH = chipHWordsH;
  }
  
  /**
   * Get chip headers for this collcetion
   * If calib mode, this is LOW gain headers
   * @return Vector with 16 Frag chip header words
   */
  inline std::vector<uint32_t> getFragChipHeaderWords() const {
                         return m_FragChipHeaderWords;
  }
  /**
   * High gain headers
   */
  std::vector<uint32_t> getFragChipHeaderWordsHigh() const {
                  return m_FragChipHeaderWordsHIGH;
  }

  /**
   * Set chip CRC words for collection
   * If calib mode, this is LOW gain CRC
   * @param chipCRCWords Reference to vector filled with 16 chip CRC words
   */
  void setFraghipCRCWords(std::vector<uint32_t>& chipCRCWords) {
       m_FragChipCRCWords = chipCRCWords;
  }
  /**
   * High gain CRC
   */
  void setFraghipCRCWordsHigh(std::vector<uint32_t>& chipCRCWordsH) {
       m_FragChipCRCWordsHIGH = chipCRCWordsH;
  }

  /**
   * Get chip CRC words for collection
   * If calib mode, LOW gain CRC
   * @return vector with 16 Frag chip CRC words
   */
  inline std::vector<uint32_t> getFragChipCRCWords() const {
                         return m_FragChipCRCWords;
  }
  /**
   * High gain CRC
   */
  inline std::vector<uint32_t> getFragChipCRCWordsHigh() const {
                         return m_FragChipCRCWordsHIGH;
  }
  
  void printExtra() const {
    std::cout<<std::hex<<"id=0x"<<this->identify()
             <<std::dec<<", bcid="<<m_FragBCID
             <<std::hex<<"(0x"<<m_FragBCID
             <<std::dec<<"), size="<<m_FragSize
             <<std::hex;
    std::cout<<std::endl<<"FragExtraWords:     ";
    for (unsigned int i=0; i<m_FragExtraWords.size(); ++i)
      std::cout<<" "<<m_FragExtraWords[i];
    std::cout<<std::endl<<"FragChipHeaderWords:";
    for (unsigned int i=0; i<m_FragChipHeaderWords.size(); ++i)
      std::cout<<" "<<m_FragChipHeaderWords[i];
    std::cout<<std::endl<<"FragChipCRCWords:   ";
    for (unsigned int i=0; i<m_FragChipCRCWords.size(); ++i)
      std::cout<<" "<<m_FragChipCRCWords[i];
    std::cout<<std::endl<<"FragChipHeaderWHigh:";
    for (unsigned int i=0; i<m_FragChipHeaderWordsHIGH.size(); ++i)
      std::cout<<" "<<m_FragChipHeaderWordsHIGH[i];
    std::cout<<std::endl<<"FragChipCRCWHigh:   ";
    for (unsigned int i=0; i<m_FragChipCRCWordsHIGH.size(); ++i)
      std::cout<<" "<<m_FragChipCRCWordsHIGH[i];
    std::cout<<std::endl<<std::dec;
  }

 private:
  /**
   * CRC words from Frag data, one word for each chip
   * LOW gain if calib mode
   */
  std::vector<uint32_t> m_FragChipCRCWords;
  /**
   * CRC words from Frag data, HIGH gain if in calib mode
   */
  std::vector<uint32_t> m_FragChipCRCWordsHIGH;
  /**
   * Header words from Frag data, one word for each chip
   * LOW gain if calib mode
   */
  std::vector<uint32_t> m_FragChipHeaderWords;
  /**
   * Header words from Frag data, one word for each chip
   * HIGH gain if calib mode
   */
  std::vector<uint32_t> m_FragChipHeaderWordsHIGH;

  /**
   * all extra words from the trailer of the drawer fragment
   * first two words are always DMU Mask and CRC word
   * upper and lower 16 bits for DMU mask and CRC word are equal
   */
  std::vector<uint32_t> m_FragExtraWords;
  /**
   * Size of drawer fragment
   */
  uint32_t m_FragSize;
  /**
   * BCID
   */
  uint32_t m_FragBCID;
};

CLASS_DEF(TileDigitsCollection,2926,0)

#endif

