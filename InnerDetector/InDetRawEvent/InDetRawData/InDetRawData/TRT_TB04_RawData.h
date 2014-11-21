/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_TB04_RawData.h
//   Header file for class TRT_TB04_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement RawData for TRT, full encoding
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef INDETRAWDATA_TRT_TB04_RAWDATA_H
#define INDETRAWDATA_TRT_TB04_RAWDATA_H

// Base class
#include "InDetRawData/TRT_RDORawData.h"

// Data members classes

class TRT_TB04_RawData : public TRT_RDORawData{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  // offline hashId of the readout element,
  // the word
  TRT_TB04_RawData(const Identifier rdoId, const unsigned int word);
  /// New constructor to have time offset into account
  TRT_TB04_RawData(const Identifier rdoId, const unsigned int word, const unsigned int timeword);

  // Destructor:
  virtual ~TRT_TB04_RawData();

  /// High level threshold:
  virtual bool highLevel() const;

  /// Time over threshold in ns for valid digits; zero otherwise:
  virtual double timeOverThreshold() const;

    // drift time in bin
  virtual int driftTimeBin() const;

  unsigned int getTrigType() const;

  // Create a new TRT_TB04_RawData and return a pointer to it:
  //  static TRT_TB04_RawData *newObject(const Identifier rdoId, const unsigned int word);

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  TRT_TB04_RawData();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  unsigned int m_timeword;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline unsigned int TRT_TB04_RawData::getTrigType() const
{
  return m_timeword;
}



#endif // INDETRAWDATA_TRT_TB04_RAWDATA_H

