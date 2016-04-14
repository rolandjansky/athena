/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT3_RawData.h
//   Header file for class SCT3_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement RawData for SCT
///////////////////////////////////////////////////////////////////
// Version 1.0 07/06/2005 MJ Costa
///////////////////////////////////////////////////////////////////

#ifndef SITRACKEREVENT_SCT3_RAWDATA_H
#define SITRACKEREVENT_SCT3_RAWDATA_H

// Base class
#include "InDetRawData/SCT_RDORawData.h"
#include <vector>


class SCT3_RawData : public SCT_RDORawData{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  SCT3_RawData(const Identifier rdoId, const unsigned int word, 
	       const std::vector<int>* errorHit);

  SCT3_RawData(const Identifier rdoId, const unsigned int word, 
	       std::vector<int>&& errorHit);

  // Destructor:
  virtual ~SCT3_RawData();

  ///////////////////////////////////////////////////////////////////
  // Virtual methods 
  ///////////////////////////////////////////////////////////////////

  // decode group of strips
  virtual int getGroupSize() const;

  // decode strip number
  virtual int getStrip() const;

  // decode time bin information for the 3 consecutive bunch crossings
  // This information is stored in 3 bits where the most significant bit
  // corresponds to BC-1. So the 3 bit pattern is: (BC-1), BC, (BC+1)
  int getTimeBin() const;


  int getErrors() const;

  bool OnTime() const;

  bool FirstHitError() const;
  bool SecondHitError() const;

  const std::vector<int>& getErrorCondensedHit() const;

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  SCT3_RawData();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  std::vector<int> m_errorCondensedHit;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
// decode size of group of strips information 
inline int SCT3_RawData::getGroupSize() const
{
  return (m_word & 0x7FF);
}

//decode strip information
inline int SCT3_RawData::getStrip() const
{
  return ((m_word >> 11) & 0x7FF);
}


//decode time bin information
inline int SCT3_RawData::getTimeBin() const
{
  return ((m_word >> 22) & 0x7);
}

//returns a word incoding the errors
// bit 4: error in condensed mode 1rst hit
// bit 5: error in condensed mode 2nd hit
inline int SCT3_RawData::getErrors() const
{
  return ((m_word >> 25) & 0x7);
}

// returns true if the time bin corresponding to the present BC is on
inline bool SCT3_RawData::OnTime() const
{
  bool isOnTime = false;
  if ((m_word >> 23) & 0x1) isOnTime = true;
  return isOnTime;
}

// returns true if there is an error in the first hit's data
inline bool SCT3_RawData::FirstHitError() const
{
  bool isError = false;
  if ((m_word >> 29) & 0x1) isError = true;
  return isError;
}

// returns true if there is an error in the second hit's data
inline bool SCT3_RawData::SecondHitError() const
{
  bool isError = false;
  if ((m_word >> 30) & 0x1) isError = true;
  return isError;
}

// returns a vector where each element is a number of the strip in 
// that group (starting at zero) with an error in:

inline const std::vector<int>& SCT3_RawData::getErrorCondensedHit() const
{
  return m_errorCondensedHit;
}

#endif // SITRACKEREVENT_SCT3_RAWDATA_H
