/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetRawData.cxx
//   Implementation file for class InDetRawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "InDetRawData/InDetRawData.h"
#include "GaudiKernel/MsgStream.h"

InDetRawData::InDetRawData(): 
  m_word(0)
{}

// Constructor with parameters:
InDetRawData::InDetRawData(const Identifier rdoId, const unsigned int word) :
  Identifiable(),
  m_rdoId(rdoId),
  m_word(word)
{

}

// Destructor:
InDetRawData::~InDetRawData()
{

}

Identifier InDetRawData::identify() const
{
  return m_rdoId;

}
unsigned int InDetRawData::getWord() const
{
  return m_word;
}


void InDetRawData::merge (const InDetRawData& other)
{
  m_word |= other.m_word;
}


MsgStream& operator << ( MsgStream& sl, const InDetRawData& rdo) {
  sl << " Identifier " << rdo.identify() << " word " << rdo.getWord();
  return sl;
}

std::ostream& operator << ( std::ostream& sl, const InDetRawData& rdo) {
  sl << " Identifier " << rdo.identify() << " word " << rdo.getWord();
  return sl;
}
