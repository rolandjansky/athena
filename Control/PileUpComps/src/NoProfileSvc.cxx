/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NoProfileSvc.h"

NoProfileSvc::NoProfileSvc(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc)
{
}

NoProfileSvc::~NoProfileSvc()
{
}

StatusCode NoProfileSvc::initialize()
{
  ATH_MSG_INFO("No change in pileup vs. (run,lb).");
  return StatusCode::SUCCESS;
}

float NoProfileSvc::scaleFactor(unsigned int, unsigned int, bool & updated)
{
  updated = false;
  return 1.0;
}
