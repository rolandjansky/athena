/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT3_RawData.cxx
//   Implementation file for class SCT3_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 11/12/2004 MJ Costa
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/SCT_RDORawData.h" 

// default constructor
SCT3_RawData::SCT3_RawData() :
  SCT_RDORawData(Identifier(), 0) //call base-class constructor
{}

// Constructor with parameters:
SCT3_RawData::SCT3_RawData(const Identifier rdoId, const unsigned int word, std::vector<int>* errorhit) :
  SCT_RDORawData( rdoId, word) //call base-class constructor
{
  m_errorCondensedHit.clear();

  std::vector<int>::iterator it=errorhit->begin(); 
  std::vector<int>::iterator end = errorhit->end();
  for (; it != end; ++it ){
    m_errorCondensedHit.push_back(*it);
  }
}

// Destructor:
//should I be destructing something here?
SCT3_RawData::~SCT3_RawData()
{
}


