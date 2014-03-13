/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodnessSelectorAlg.h"
#include "MissingETGoodness/EtmissGoodnessManager.h"
#include "MissingETGoodness/EtmissGoodness.h"
#include "TROOT.h"


EtmissGoodnessSelectorAlg::EtmissGoodnessSelectorAlg(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator)
 , m_quality(1) // unset
{
  declareProperty( "GoodnessConfig", m_qualifier = "All" );
  declareProperty( "Quality", m_quality = 1 );
}


EtmissGoodnessSelectorAlg::~EtmissGoodnessSelectorAlg()
{ 
} 


StatusCode EtmissGoodnessSelectorAlg::initialize() 
{
  ATH_MSG_DEBUG ("initialize()");

  ATH_MSG_DEBUG ("initialize() successful");

  return StatusCode::SUCCESS;
}


StatusCode 
EtmissGoodnessSelectorAlg::execute() 
{
  ATH_MSG_DEBUG ("execute()");

  // this updates all inputs to all met selection requirements
  MET::EtmissGoodnessManager& manager = MET::EtmissGoodnessManager::instance();
  manager.execute();

  // now determine met quality
  bool statusOk = m_etmissgoodness.IsGood( static_cast<MET::Quality>(m_quality), m_qualifier.c_str() );

  // This skips the execution of following algs for this event
  this->setFilterPassed (statusOk);

  ATH_MSG_DEBUG ("execute() successful");

  return StatusCode::SUCCESS;
}


StatusCode EtmissGoodnessSelectorAlg::finalize() 
{
  ATH_MSG_DEBUG ("finalize()");

  ATH_MSG_DEBUG ("finalize() successful");

  return StatusCode::SUCCESS;
}


