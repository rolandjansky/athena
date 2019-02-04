/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_TB03_RawData.cxx
//   Implementation file for class SCT_TB03_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 11/12/2003 MJ Costa
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/SCT_TB03_RawData.h"
#include "InDetRawData/SCT_RDORawData.h"

// default constructor
SCT_TB03_RawData::SCT_TB03_RawData() :
  SCT_RDORawData(Identifier(), 0) //call base-class constructor
{
	m_event = 0;
  m_fragment_type = 0;
  m_size = 0; 
  m_word_error_count = 0;
  m_event_count = 0;
  m_tbin = 0;
  m_stream = 0;  
}

// Constructor with parameters:
SCT_TB03_RawData::SCT_TB03_RawData(const Identifier rdoId, const unsigned int word, 
  const unsigned int Event, const unsigned int FragmentIdentifier, 
  const unsigned int Size, const unsigned int WordErrorCount, 
  const unsigned int EventCount, const int tbin, const int stream) :
  SCT_RDORawData( rdoId, word) //call base-class constructor
{
  m_event = Event;
  m_fragment_type = FragmentIdentifier;
  m_size = Size; 
  m_word_error_count = WordErrorCount;
  m_event_count = EventCount;
  m_tbin = tbin;
  m_stream = stream;  
}

//SCT_TB03_RawData *SCT_TB03_RawData::newObject(const Identifier rdoId, const unsigned int word)
//{
//  SCT_TB03_RawData *p_rdo=0;

  // create a new object and check
//  try {
//    p_rdo=new SCT_TB03_RawData(rdoId, word);
//  } catch (...) {
//    p_rdo=0;
//  }
//  return p_rdo;
//}

