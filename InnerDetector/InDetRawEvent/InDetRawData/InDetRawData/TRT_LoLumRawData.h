/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_LoLumRawData.h
//   Header file for class TRT_LoLumRawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement RawData for TRT, full encoding
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef INDETRAWDATA_TRT_LOLUMRAWDATA_H
#define INDETRAWDATA_TRT_LOLUMRAWDATA_H

// Base class
#include "InDetRawData/TRT_RDORawData.h"

// Data members classes

class TRT_LoLumRawData : public TRT_RDORawData{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  // offline hashId of the readout element,
  // the word
  TRT_LoLumRawData(const Identifier rdoId, const unsigned int word);

  // Destructor:
  virtual ~TRT_LoLumRawData();

  // High level threshold:
  virtual bool highLevel() const;
  virtual bool highLevel(int /* BX */) const;

  // Time over threshold in ns for valid digits; zero otherwise:
  virtual double timeOverThreshold() const {
    return timeOverThreshold(m_word);
  };

  // drift time in bin
  virtual int driftTimeBin() const {
    return driftTimeBin(m_word);
  };

  virtual int trailingEdge() const {
    return trailingEdge(m_word);
  };

  virtual bool firstBinHigh() const;  // True if first time bin is high
  virtual bool lastBinHigh() const;   // True if last time bin is high


  ///////////////////////////////////////////////////////////////////
  // Virtual methods
  ///////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////
  // Static methods:
  ///////////////////////////////////////////////////////////////////

protected:
  // width of the drift time bins
  static const double m_driftTimeBinWidth;

  // bit masks used in interpretation of bit pattern
  static const unsigned int m_maskFourLastBits;
  static const unsigned int m_maskThreeLastBits;

public:
  // width of the drift time bins
  static double getDriftTimeBinWidth() {
    return m_driftTimeBinWidth;
  };

  // Find the relevant island of bits from the bit pattern, defined as the largest island with the latest leading edge
  static bool findLargestIsland(unsigned int word, unsigned int& leadingEdge, unsigned int& trailingEdge);

  // Determine the drift time bin, i.e. the leading edge of the relevant island, from the bit pattern
  static unsigned int driftTimeBin(unsigned int word);

  // Determine the trailing edge of the relevant island from the bit pattern
  static unsigned int trailingEdge(unsigned int word);

  // Determine the time over threshold, i.e. width of the relevant island, in ns from the bit pattern
  static double timeOverThreshold(unsigned int word);

  // Check if the middle HT bit is set
  inline
  static bool highLevel(unsigned int word) {
    // return (m_word & 0x04020100); // check any of the three HT bits
    return (word & 0x00020000); // check only middle HT bit
  }

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  TRT_LoLumRawData();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/*
 * highLevel() -
 * Returns true if there is a high threshold hit in the middle bunch crossing, false
 * otherwise
 */
inline 
bool TRT_LoLumRawData::highLevel() const
{
  return highLevel(m_word);
}

/*
 * highLevel( BX ) -
 * Returns true if there is a high threshold hit in bunch crossing BX, false
 * otherwise.  BX is 1 for the earliest bunch crossing and 3 for the latest 
 * bunch crossing.
 */
inline 
bool TRT_LoLumRawData::highLevel(int BX) const
{
  if ( (BX < 1) || (BX > 3) )
    return false;

  return (m_word & ( 1 << (9 * BX - 1) ));

}

/*
 * firstBinHigh() -
 * Returns true if the first low threshold time bin it high, false otherwise.
 */
inline bool
TRT_LoLumRawData::firstBinHigh() const 
{ 
  return (m_word & 0x02000000);
}


/*
 * lastBinHigh() -
 * Returns true if the last low threshold time bin it high, false otherwise.
 */
inline bool
TRT_LoLumRawData::lastBinHigh() const
{
  return (m_word & 0x1);
}


#endif // INDETRAWDATA_TRT_LOLUMRAWDATA_H

