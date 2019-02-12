/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_TB04_RawData.h
//   Header file for class SCT_TB03_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement RawData for SCT, using TB03 for decoding word
///////////////////////////////////////////////////////////////////
// Version 1.0 11/12/2003 MJ Costa
///////////////////////////////////////////////////////////////////

#ifndef SITRACKEREVENT_SCT_TB04_RAWDATA_H
#define SITRACKEREVENT_SCT_TB04_RAWDATA_H

// Base class
#include "InDetRawData/SCT_RDORawData.h"


// Data members classes



class SCT_TB04_RawData : public SCT_RDORawData{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  // Mustard Constructor
  SCT_TB04_RawData(const Identifier rdoId, const unsigned int word, 
  const unsigned int Event, const unsigned int FragmentIdentifier, 
  const unsigned int Size, const unsigned int WordErrorCount, 
  const unsigned int EventCount, const unsigned int onlineId);
  // ROD Constructor
  SCT_TB04_RawData(const Identifier rdoId, const unsigned int word, 
  const unsigned int onlineId, float errorhit[20]);

  // Destructor:
  virtual ~SCT_TB04_RawData() = default;

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

  int getErrors() const;

  int getStream() const;

  int getROB() const;

  bool OnTime() const;

  bool SyncronizationError() const;

  bool PreambleError() const;
  bool LVL1Error() const;
  bool BCIDError() const;
  bool FormatterError() const;
  bool FirstHitError() const;
  bool SecondHitError() const;

  float *getErrorCondensedHit();



  ///////////////////////////////////////////////////////////////////
  // Static methods:
  ///////////////////////////////////////////////////////////////////


  // Create a new SCT_TB04_RawData and return a pointer to it:
  //  static SCT_TB04_RawData *newObject(const Identifier rdoId, const unsigned int word);

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  SCT_TB04_RawData();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  unsigned int m_event;
  unsigned int m_fragment_type;
  unsigned int m_size;
  unsigned int m_word_error_count;
  unsigned int m_event_count;
  unsigned int m_onlineId;
  float m_errorCondensedHit[20];

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
// decode size of group of strips information 
inline int SCT_TB04_RawData::getGroupSize() const
{
  return (m_word & 0x7FF);
}

inline unsigned int SCT_TB04_RawData::getEvent() const
{
  return m_event;
}

inline unsigned int SCT_TB04_RawData::getFragmentType() const 
{
  return m_fragment_type;
}

inline unsigned int SCT_TB04_RawData::getSize() const
{
  return m_size;
}

inline unsigned int SCT_TB04_RawData::getWordErrorCount() const
{
  return m_word_error_count;
}

inline unsigned int SCT_TB04_RawData::getEventCount() const
{
  return m_event_count;
}

//decode time bin information
inline int SCT_TB04_RawData::getTimeBin() const
{
  return ((m_word >> 22) & 0x7);
}

//decode errors information
inline int SCT_TB04_RawData::getErrors() const
{
  return ((m_word >> 25) & 0x7);
}

inline int SCT_TB04_RawData::getStream() const
{
  //  return ((m_onlineId >>16)&0xFFFF);
  //Kondo 14/02/07: New calculation of the online Id in the cabling packages
  //  return ((m_onlineId >>8)&0xFFFF);
  //Kondo 28/05/07: New calculation of the online Id in the cabling packages
  return ((m_onlineId >>24)&0xFFFF);
}

inline int SCT_TB04_RawData::getROB() const
{
  return (m_onlineId &0xFFFF);
}

inline bool SCT_TB04_RawData::OnTime() const
{
  bool isOnTime = false;
  if ((m_word >> 23) & 0x1) isOnTime = true;
  return isOnTime;
}

inline bool SCT_TB04_RawData::SyncronizationError() const
{
  bool isError = false;
  if (((m_word >> 26) & 0x1) || (((m_word >> 27) & 0x1))) isError = true;
  return isError;
}

inline bool SCT_TB04_RawData::PreambleError() const
{
  bool isError = false;
  if ((m_word >> 25) & 0x1) isError = true;
  return isError;
}

inline bool SCT_TB04_RawData::LVL1Error() const
{
  bool isError = false;
  if ((m_word >> 26) & 0x1) isError = true;
  return isError;
}

inline bool SCT_TB04_RawData::BCIDError() const
{
  bool isError = false;
  if ((m_word >> 27) & 0x1) isError = true;
  return isError;
}

inline bool SCT_TB04_RawData::FormatterError() const
{
  bool isError = false;
  if ((m_word >> 28) & 0x1) isError = true;
  return isError;
}


inline bool SCT_TB04_RawData::FirstHitError() const
{
  bool isError = false;
  if ((m_word >> 29) & 0x1) isError = true;
  return isError;
}

inline bool SCT_TB04_RawData::SecondHitError() const
{
  bool isError = false;
  if ((m_word >> 30) & 0x1) isError = true;
  return isError;
}

inline float* SCT_TB04_RawData::getErrorCondensedHit()
{
  return m_errorCondensedHit;
}




#endif // SITRACKEREVENT_SCT_TB04_RAWDATA_H
