/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4ShowerLibSvc/LArG4ShowerLibSvcTest.h"

LArG4ShowerLibSvcTest::LArG4ShowerLibSvcTest (const std::string& name, ISvcLocator* pSvcLocator)
                   : AthAlgorithm(name, pSvcLocator),
                     m_showerLibSvc("LArG4ShowerLibSvc",name)
{
	  declareProperty("LArG4ShowerLibSvc",m_showerLibSvc);
}

LArG4ShowerLibSvcTest::~LArG4ShowerLibSvcTest ()
{
}

StatusCode LArG4ShowerLibSvcTest::initialize()
{

  msg(MSG::INFO) << "Initializing" << endmsg;

  if (m_showerLibSvc.retrieve().isFailure()) {
     return StatusCode::FAILURE;
  } else {
     msg(MSG::INFO) << "retrieved LArG4ShowerLibSvc" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArG4ShowerLibSvcTest::finalize()
{
  msg(MSG::INFO) << "Finalized" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode LArG4ShowerLibSvcTest::execute()
{
  msg(MSG::INFO) << "execute: stub" << endmsg;

  return StatusCode::SUCCESS;
}
