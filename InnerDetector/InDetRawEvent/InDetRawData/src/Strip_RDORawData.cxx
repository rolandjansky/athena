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
#include "InDetRawData/Strip_RDORawData.h"
#include "InDetRawData/InDetRawData.h"

// Constructor with parameters:
Strip_RDORawData::Strip_RDORawData(const Identifier rdoId, 
			   const unsigned int word) :
  InDetRawData(rdoId, word) //call base class constructor
{
    std::cout << "We are using Strip_RDORawData!" << std::endl;
}

// Destructor:
//should I be destructing something here?
Strip_RDORawData::~Strip_RDORawData()
{}

