/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT1_RawData.cxx
//   Implementation file for class SCT1_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/SCT1_RawData.h"
#include "InDetRawData/SCT_RDORawData.h"

// default constructor
SCT1_RawData::SCT1_RawData() :
  SCT_RDORawData(Identifier(), 0) //call base-class constructor
{}

// Constructor with parameters:
SCT1_RawData::SCT1_RawData(const Identifier rdoId, 
			   const unsigned int word) :
  SCT_RDORawData( rdoId, word) //call base-class constructor
{}

// Destructor:
//should I be destructing something here?
SCT1_RawData::~SCT1_RawData()
{}


//SCT1_RawData *SCT1_RawData::newObject(const Identifier rdoId, const unsigned int word)
//{
//  SCT1_RawData *p_rdo=0;

  // create a new object and check
//  try {
//    p_rdo=new SCT1_RawData(rdoId, word);
//  } catch (...) {
//    p_rdo=0;
//  }
//  return p_rdo;
//}

