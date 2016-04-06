/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestAttrReader.cxx
 *
 * @brief Test Algorithm for reading an AttributeList from the
 * transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTestAttrReader.cxx,v 1.4 2005-06-01 07:56:30 schaffer Exp $
 *
 */


// INCLUDE HEADER FILES:


#include "AthenaPoolTestAttrReader.h"

AthenaPoolTestAttrReader::AthenaPoolTestAttrReader(const std::string& name,
					 ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator)
{}

AthenaPoolTestAttrReader::~AthenaPoolTestAttrReader()
{}

StatusCode AthenaPoolTestAttrReader::initialize()
{
    return StatusCode::SUCCESS; 
}

StatusCode AthenaPoolTestAttrReader::execute()
{ 
    ATH_MSG_DEBUG("Executing AthenaPoolTestAttrReader");
    return StatusCode::SUCCESS;
}

StatusCode AthenaPoolTestAttrReader::finalize() 
{
    return StatusCode::SUCCESS;
}


