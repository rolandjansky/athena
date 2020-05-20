/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SolenoidParametrizationCondAlg.h"
#include <limits>
namespace Trk
{

SolenoidParametrizationCondAlg::SolenoidParametrizationCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
  , m_condSvc{"CondSvc", name}
{
}

StatusCode SolenoidParametrizationCondAlg::initialize() {
  ATH_CHECK( m_condSvc.retrieve());
  ATH_CHECK( m_fieldCacheCondObjInputKey.initialize());
  ATH_CHECK( m_writeKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode SolenoidParametrizationCondAlg::execute(const EventContext &ctx) const {
   SG::WriteCondHandle<SolenoidParametrization> write_handle(m_writeKey,ctx);
   if (write_handle.isValid()) {
      return StatusCode::SUCCESS;
   }

   SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
   if (!readHandle.isValid()) {
      ATH_MSG_FATAL("Failed to get magnetic field conditions data " << m_fieldCacheCondObjInputKey.key() );
      return StatusCode::FAILURE;
   }

   write_handle.addDependency(readHandle);
   ATH_CHECK( write_handle.record(std::make_unique<SolenoidParametrization>(*(readHandle.cptr())) ));
   return StatusCode::SUCCESS;
}


StatusCode SolenoidParametrizationCondAlg::finalize() {
  return StatusCode::SUCCESS;
}

}
