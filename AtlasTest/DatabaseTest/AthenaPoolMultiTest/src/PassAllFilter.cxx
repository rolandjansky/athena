/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PassAllFilter.cxx
 * @brief Implementation of class PassAllFilter
 */
  
#include <string>
#include "PassAllFilter.h"
    
PassAllFilter::PassAllFilter(const std::string& name, 
			ISvcLocator* pSvcLocator) :
              AthFilterAlgorithm(name, pSvcLocator)
{}

PassAllFilter::~PassAllFilter()
{}

StatusCode PassAllFilter::initialize() { return StatusCode::SUCCESS; }

StatusCode PassAllFilter::execute() 
{
       setFilterPassed(true);	
       return StatusCode::SUCCESS; 
}

StatusCode PassAllFilter::finalize() { return StatusCode::SUCCESS; }
