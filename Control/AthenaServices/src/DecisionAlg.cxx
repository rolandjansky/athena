/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionAlg.h"


//****************************************************************************


// Standard Constructor
DecisionAlg::DecisionAlg(const std::string& name, ISvcLocator* pSvcLocator)
        : FilteredAlgorithm(name, pSvcLocator)
{
   assert(pSvcLocator);
}

// Standard Destructor
DecisionAlg::~DecisionAlg() {
}

