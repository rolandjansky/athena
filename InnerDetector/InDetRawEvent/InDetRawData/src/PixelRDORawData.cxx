/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelRawData.cxx
//   Implementation file for class PixelRawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/PixelRDORawData.h"
#include "InDetRawData/InDetRawData.h"

// Constructor with parameters:
PixelRDORawData::PixelRDORawData(const Identifier rdoId, 
			   const unsigned int word) :
  InDetRawData(rdoId,word) //call base class constructor
{}


// Destructor:
//should I be destructing something here?
PixelRDORawData::~PixelRDORawData()
{}






