/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CreateFullScanRoI.h"

CreateFullScanRoI::CreateFullScanRoI(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

CreateFullScanRoI::~CreateFullScanRoI()
{
}

StatusCode CreateFullScanRoI::initialize()
{
  //ATH_MSG_DEBUG("Use macros for logging!");
  CHECK( m_roisKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode CreateFullScanRoI::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode CreateFullScanRoI::execute_r(const EventContext& context) const
{

  auto roisCollection = std::make_unique<TrigRoiDescriptorCollection>();
  auto roi = new TrigRoiDescriptor( true ); // this is FullScan RoI
  roisCollection->push_back( roi );


  auto handle = SG::makeHandle( m_roisKey, context );
  CHECK( handle.record( std::move( roisCollection ) ) );
  return StatusCode::SUCCESS;
}

