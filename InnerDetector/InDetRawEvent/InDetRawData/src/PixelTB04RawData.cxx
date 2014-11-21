/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelTB04RawData.cxx
//   Implementation file for class PixelTB04RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 06/09/2004 Maria Jose Costa
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/PixelTB04RawData.h"
#include "InDetRawData/PixelRDORawData.h"

// default constructor
PixelTB04RawData::PixelTB04RawData() :
  PixelRDORawData( Identifier(), 0) //call base-class constructor
{
  m_BCID=0;
  m_LVL1ID=0;
}

// Constructor with parameters:
PixelTB04RawData::PixelTB04RawData(const Identifier rdoId, 
			   const unsigned int word, const unsigned int BCID, const unsigned int LVL1ID) :
  PixelRDORawData( rdoId, word) //call base-class constructor
{
  m_BCID = BCID;
  m_LVL1ID = LVL1ID;
}

PixelTB04RawData::PixelTB04RawData(const Identifier rdoId, 
			   const unsigned int ToT, const unsigned int BCID, const unsigned int LVL1ID, const unsigned int LVL1A) :
  PixelRDORawData( rdoId, (ToT&0xFF)+((LVL1A&0xF)<<8) ) //call base-class constructor
{
  m_BCID = BCID;
  m_LVL1ID = LVL1ID;
}

// Destructor:
PixelTB04RawData::~PixelTB04RawData()
{}





