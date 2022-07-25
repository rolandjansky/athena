/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SiDetectorElementStatusAlg.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "StoreGate/WriteHandle.h"

#include <map>

namespace InDet {
   SiDetectorElementStatusAlg::SiDetectorElementStatusAlg(const std::string& name, ISvcLocator* pSvcLocator)
      : ::AthReentrantAlgorithm(name, pSvcLocator)
   {
   }

   StatusCode SiDetectorElementStatusAlg::initialize()
   {
      ATH_CHECK( m_condSummaryTool.empty() ? StatusCode::FAILURE : StatusCode::SUCCESS);
      ATH_CHECK( m_condSummaryTool.retrieve());
      ATH_CHECK( m_writeKey.initialize());
      return StatusCode::SUCCESS;
   }

   StatusCode SiDetectorElementStatusAlg::execute(const EventContext& ctx) const
   {
      SG::WriteHandle<InDet::SiDetectorElementStatus> writeHandle{m_writeKey, ctx};
      auto detector_element_status = m_condSummaryTool->getDetectorElementStatus(ctx, nullptr);
      if (writeHandle.record( std::move(detector_element_status) ).isFailure()) {
         ATH_MSG_FATAL("Could not record " << writeHandle.key()  );
         return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
   }
}
