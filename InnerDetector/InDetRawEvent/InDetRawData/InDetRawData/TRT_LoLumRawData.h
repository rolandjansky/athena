/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  virtual double timeOverThreshold() const;

    // drift time in bin
  virtual int driftTimeBin() const;   // Position of first leading edge

  virtual int trailingEdge() const;   // Position of last trailing edge

  virtual bool firstBinHigh() const;  // True if first time bin is high
  virtual bool lastBinHigh() const;   // True if last time bin is high


  ///////////////////////////////////////////////////////////////////
  // Virtual methods
  ///////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////
  // Static methods:
  ///////////////////////////////////////////////////////////////////

  // Create a new TRT_LoLumRawData and return a pointer to it:
  //  static TRT_LoLumRawData *newObject(const Identifier rdoId, const unsigned int word);

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
 * Returns true if there is a high threshold hit in any bunch crossing, false
 * otherwise
 */
inline 
bool TRT_LoLumRawData::highLevel() const
{
  return (m_word & 0x04020100);
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

