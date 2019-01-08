/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/src/TileDigitsCollection.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Out-of-line implementations for TileDigitsCollection.
 */


#include "TileEvent/TileDigitsCollection.h"


/**
 * @brief Copy constructor.
 * @param other Collection to copy.
 * Does a deep copy of the contents.
 */
TileDigitsCollection::TileDigitsCollection
  (const TileDigitsCollection& other)
    : TileRawDataCollection<TileDigits> (other.identify()),
      m_FragChipCRCWords (other.m_FragChipCRCWords),
      m_FragChipCRCWordsHIGH (other.m_FragChipCRCWordsHIGH),
      m_FragChipHeaderWords (other.m_FragChipHeaderWords),
      m_FragChipHeaderWordsHIGH (other.m_FragChipHeaderWordsHIGH),
      m_FragExtraWords (other.m_FragExtraWords),
      m_FragSize (other.m_FragSize),
      m_FragBCID (other.m_FragBCID)
{
  // Copy flags from the base class.
  setLvl1Id          (other.getLvl1Id());
  setLvl1Type        (other.getLvl1Type());
  setDetEvType       (other.getDetEvType());
  setRODBCID         (other.getRODBCID());

  // Copy the collection contents.
  reserve (other.size());
  for (const TileDigits* chan : other) {
    push_back (std::make_unique<TileDigits> (*chan));
  }
}


void TileDigitsCollection::printExtra() const
{
  printExtra (std::cout);
}


void TileDigitsCollection::printExtra (std::ostream& os) const
{
  os<<std::hex<<"id=0x"<<this->identify()
    <<std::dec<<", bcid="<<m_FragBCID
    <<std::hex<<"(0x"<<m_FragBCID
    <<std::dec<<"), size="<<m_FragSize
    <<std::hex;
  os<<std::endl<<"FragExtraWords:     ";
  for (unsigned int i=0; i<m_FragExtraWords.size(); ++i)
    os<<" "<<m_FragExtraWords[i];
  os<<std::endl<<"FragChipHeaderWords:";
  for (unsigned int i=0; i<m_FragChipHeaderWords.size(); ++i)
    os<<" "<<m_FragChipHeaderWords[i];
  os<<std::endl<<"FragChipCRCWords:   ";
  for (unsigned int i=0; i<m_FragChipCRCWords.size(); ++i)
    os<<" "<<m_FragChipCRCWords[i];
  os<<std::endl<<"FragChipHeaderWHigh:";
  for (unsigned int i=0; i<m_FragChipHeaderWordsHIGH.size(); ++i)
    os<<" "<<m_FragChipHeaderWordsHIGH[i];
  os<<std::endl<<"FragChipCRCWHigh:   ";
  for (unsigned int i=0; i<m_FragChipCRCWordsHIGH.size(); ++i)
    os<<" "<<m_FragChipCRCWordsHIGH[i];
  os<<std::endl<<std::dec;
}


void TileDigitsCollection::clear()
{
  m_FragChipCRCWords.clear();
  m_FragChipCRCWordsHIGH.clear();
  m_FragChipHeaderWords.clear();
  m_FragChipHeaderWordsHIGH.clear();

  m_FragExtraWords.resize(2);
  m_FragExtraWords[0] = m_FragExtraWords[1] = 0;

  m_FragSize = 0;
  m_FragBCID = 0;

  TileDigitsCollection::MyBase::clear();
}
