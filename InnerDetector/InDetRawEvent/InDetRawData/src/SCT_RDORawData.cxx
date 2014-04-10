/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_RawData.cxx
//   Implementation file for class SCT_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetRawData/InDetRawData.h"

// Constructor with parameters:
SCT_RDORawData::SCT_RDORawData(const Identifier rdoId, 
			   const unsigned int word) :
  InDetRawData(rdoId, word) //call base class constructor
{}

// Destructor:
//should I be destructing something here?
SCT_RDORawData::~SCT_RDORawData()
{}

