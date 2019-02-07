/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JobIDSvc.h"

#include <uuid/uuid.h>

JobIDSvc::JobIDSvc(const std::string& name, ISvcLocator* svc) :
  base_class(name, svc)
{}

StatusCode JobIDSvc::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  uuid_generate(m_JobID);
  return StatusCode::SUCCESS;
}

PJobID_t JobIDSvc::getJobID() const
{
  return const_cast<PJobID_t>(m_JobID);
}

std::string JobIDSvc::toString() const
{
  return reinterpret_cast<const char*>(m_JobID);
}
