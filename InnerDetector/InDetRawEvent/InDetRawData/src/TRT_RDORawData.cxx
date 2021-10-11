/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_RDORawData.cxx
//   Implementation file for class TRT_RDORawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetRawData/TRT_RDORawData.h"
#include "InDetRawData/InDetRawData.h"

// Constructor with parameters:
TRT_RDORawData::TRT_RDORawData(const Identifier rdoId, 
			   const unsigned int word) :
  InDetRawData(rdoId,word) //call base class constructor
{}







