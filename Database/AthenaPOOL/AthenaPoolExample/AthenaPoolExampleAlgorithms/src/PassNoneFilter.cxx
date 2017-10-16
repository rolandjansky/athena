/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolExampleAlgorithms/src/PassNoneFilter.cxx
 *  @brief This file contains the implementation for the PassNoneFilter class.
 *  $Id: PassNoneFilter.cxx,v 1.2 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "PassNoneFilter.h"

using namespace AthPoolEx;

PassNoneFilter::PassNoneFilter(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) {}

PassNoneFilter::~PassNoneFilter() {}

StatusCode PassNoneFilter::initialize() { return StatusCode::SUCCESS; }

StatusCode PassNoneFilter::execute() {
   setFilterPassed(false);
   return StatusCode::SUCCESS;
}

StatusCode PassNoneFilter::finalize() { return StatusCode::SUCCESS; }
