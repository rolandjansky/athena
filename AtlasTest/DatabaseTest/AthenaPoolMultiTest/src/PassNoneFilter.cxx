/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolMultiTest/src/PassNoneFilter.cxx
 * @brief Implementation of class PassNoneFilter
 */
 
#include <string>
#include "PassNoneFilter.h"
    
PassNoneFilter::PassNoneFilter(const std::string& name, 
			ISvcLocator* pSvcLocator) :
              AthFilterAlgorithm(name, pSvcLocator)
{}

PassNoneFilter::~PassNoneFilter()
{}

StatusCode PassNoneFilter::initialize() { return StatusCode::SUCCESS; }

StatusCode PassNoneFilter::execute() 
{
       setFilterPassed(false);
       return StatusCode::SUCCESS;
}
                                                                                
StatusCode PassNoneFilter::finalize() { return StatusCode::SUCCESS; }
