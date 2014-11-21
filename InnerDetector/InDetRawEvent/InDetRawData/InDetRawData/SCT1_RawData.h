/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT1_RawData.h
//   Header file for class SCT1_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement RawData for SCT, using method 1 for decoding word
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef SITRACKEREVENT_SCT1_RAWDATA_H
#define SITRACKEREVENT_SCT1_RAWDATA_H

// Base class
#include "InDetRawData/SCT_RDORawData.h"


// Data members classes



class SCT1_RawData : public SCT_RDORawData{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  // offline hash Id, last, word
  SCT1_RawData(const Identifier rdoId, const unsigned int word);

  // Destructor:
  virtual ~SCT1_RawData();

  ///////////////////////////////////////////////////////////////////
  // Virtual methods 
  ///////////////////////////////////////////////////////////////////


  // decode group of strips
  virtual int getGroupSize() const;

  // decode strip number
  virtual int getStrip() const;
  ///////////////////////////////////////////////////////////////////
  // Static methods:
  ///////////////////////////////////////////////////////////////////


  // Create a new SCT1_RawData and return a pointer to it:
  //  static SCT1_RawData *newObject(const Identifier rdoId, const unsigned int word);

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  SCT1_RawData();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:


};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
// decode size of group of strips information (from Calvet RawData class)
inline int SCT1_RawData::getGroupSize() const
{
// Temporary mask, should be fixed later
  return (m_word & 0xFFFF);
}
// decode strip information (from Calvet RawData class)
inline int SCT1_RawData::getStrip() const
{
// Temporary mask, should be fixed later
  return ((m_word >> 16) & 0xFFFF);
}


#endif // SITRACKEREVENT_SCT1_RAWDATA_H
