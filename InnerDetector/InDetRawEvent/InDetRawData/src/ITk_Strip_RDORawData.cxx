/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITk_Strip_RawData.cxx
//   Implementation file for class ITk_Strip_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 0.1 15/09/2020 James Smith
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/ITk_Strip_RDORawData.h"
#include "InDetRawData/InDetRawData.h"

// Constructor with parameters:
ITk_Strip_RDORawData::ITk_Strip_RDORawData(const Identifier rdoId, 
			   const unsigned int word) :
  InDetRawData(rdoId, word) //call base class constructor
{}

// Destructor:
//should I be destructing something here?
ITk_Strip_RDORawData::~ITk_Strip_RDORawData()
{}

