/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JobInfo.h"


//================ Constructor =================================================

JobInfo::JobInfo(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_printFATAL(false)
{
  declareProperty("PrintFATAL", m_printFATAL,"flag to decide if a FATAL should be printed - protection for Tier0");
}

StatusCode JobInfo::execute()
{
  return StatusCode::SUCCESS;
}
