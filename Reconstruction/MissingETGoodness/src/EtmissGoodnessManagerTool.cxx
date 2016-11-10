/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodnessManagerTool.h"
#include "MissingETGoodness/EtmissGoodnessManager.h"
#include "MissingETGoodness/Goodies.h"
#include "MissingETGoodness/TMsgLogger.h"
#include "TROOT.h"

EtmissGoodnessManagerTool::EtmissGoodnessManagerTool( const std::string& type, const std::string& name, const IInterface* parent ) 
 : AthAlgTool( type, name, parent )
 , m_version(-1) // latest version
 , m_quality(-1) // unset
{
  declareInterface<EtmissGoodnessManagerTool>( this );

  declareProperty( "SetAndLockVersion", m_version );
  declareProperty( "SetAndLockQuality", m_quality );
}


EtmissGoodnessManagerTool::~EtmissGoodnessManagerTool()
{
}


StatusCode
EtmissGoodnessManagerTool::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("initialize()");

  // set output level
  MET::TMsgLogger::SetMinLevel(static_cast<MET::TMsgLevel>(msgLevel()));

  // now set version and quality
  MET::EtmissGoodnessManager& manager = MET::EtmissGoodnessManager::instance();

  if (m_version>=0) manager.SetAndLockVersion(m_version);
  if (m_quality>=0) {
    if (m_quality>=MET::MAX_QUALITY) { return StatusCode::FAILURE; }
    manager.SetAndLockQuality( static_cast<MET::Quality>(m_quality) );
  }

  ATH_MSG_DEBUG ("initialize() successful");

  return sc;
}


StatusCode
EtmissGoodnessManagerTool::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("execute()");

  MET::EtmissGoodnessManager::instance().ClearEvent();

  ATH_MSG_DEBUG ("execute() successful");

  return sc;
}


StatusCode
EtmissGoodnessManagerTool::finalize()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("finalize()");

  ATH_MSG_DEBUG ("finalize() successful");

  return sc;
}

