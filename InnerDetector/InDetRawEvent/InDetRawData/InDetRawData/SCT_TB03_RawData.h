/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_TB03_RawData.h
//   Header file for class SCT_TB03_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement RawData for SCT, using TB03 for decoding word
///////////////////////////////////////////////////////////////////
// Version 1.0 11/12/2003 MJ Costa
///////////////////////////////////////////////////////////////////

#ifndef SITRACKEREVENT_SCT_TB03_RAWDATA_H
#define SITRACKEREVENT_SCT_TB03_RAWDATA_H

// Base class
#include "InDetRawData/SCT_RDORawData.h"


// Data members classes



class SCT_TB03_RawData : public SCT_RDORawData{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  // offline hash Id, last, word
  SCT_TB03_RawData(const Identifier rdoId, const unsigned int word, 
  const unsigned int Event, const unsigned int FragmentIdentifier, 
  const unsigned int Size, const unsigned int WordErrorCount, 
  const unsigned int EventCount, const int tbin, const int stream);

  // Destructor:
  virtual ~SCT_TB03_RawData() = default;

  ///////////////////////////////////////////////////////////////////
  // Virtual methods 
  ///////////////////////////////////////////////////////////////////

  // decode group of strips
  virtual int getGroupSize() const;

  unsigned int getEvent() const;

  unsigned int getFragmentType() const;

  unsigned int getSize() const;

  unsigned int getWordErrorCount() const;

  unsigned int getEventCount() const;

  int getTimeBin() const;

  int getStream() const;

  ///////////////////////////////////////////////////////////////////
  // Static methods:
  ///////////////////////////////////////////////////////////////////


  // Create a new SCT_TB03_RawData and return a pointer to it:
  //  static SCT_TB03_RawData *newObject(const Identifier rdoId, const unsigned int word);

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  SCT_TB03_RawData();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  unsigned int m_event;
  unsigned int m_fragment_type;
  unsigned int m_size;
  unsigned int m_word_error_count;
  unsigned int m_event_count;
  int m_tbin;
  int m_stream;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
// decode size of group of strips information (from Calvet RawData class)
inline int SCT_TB03_RawData::getGroupSize() const
{
// Temporary mask, should be fixed later
  return (m_word & 0xFFFF);
}

inline unsigned int SCT_TB03_RawData::getEvent() const
{
  return m_event;
}

inline unsigned int SCT_TB03_RawData::getFragmentType() const 
{
  return m_fragment_type;
}

inline unsigned int SCT_TB03_RawData::getSize() const
{
  return m_size;
}

inline unsigned int SCT_TB03_RawData::getWordErrorCount() const
{
  return m_word_error_count;
}

inline unsigned int SCT_TB03_RawData::getEventCount() const
{
  return m_event_count;
}

inline int SCT_TB03_RawData::getTimeBin() const
{
  return m_tbin;
}

inline int SCT_TB03_RawData::getStream() const
{
  return m_stream;
}


#endif // SITRACKEREVENT_SCT_TB03_RAWDATA_H
