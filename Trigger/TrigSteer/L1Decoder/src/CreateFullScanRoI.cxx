/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CreateFullScanRoI.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

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

StatusCode CreateFullScanRoI::execute(const EventContext& ctx) const
{
  using namespace TrigCompositeUtils;
  SG::WriteHandle<TrigRoiDescriptorCollection> handle1 = createAndStoreNoAux(m_roisKey, ctx ); 
  auto roisCollection = handle1.ptr();
  auto roi = new TrigRoiDescriptor( true ); // this is FullScan RoI
  roisCollection->push_back( roi );
  return StatusCode::SUCCESS;
}
