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
      ATH_CHECK( m_readKey.initialize(!m_readKey.empty()));
      return StatusCode::SUCCESS;
   }

   StatusCode SiDetectorElementStatusAlg::execute(const EventContext& ctx) const
   {
      SG::WriteHandle<InDet::SiDetectorElementStatus> writeHandle{m_writeKey, ctx};
      if (!m_readKey.empty()) {
         std::unique_ptr<InDet::SiDetectorElementStatus> si_detector_element_status( m_condSummaryTool->getDetectorElementStatus(ctx, m_activeOnly));
         SG::ReadHandle<InDet::SiDetectorElementStatus> input_element_status{m_readKey, ctx};
         try {
            *si_detector_element_status &= *input_element_status;
         }
         catch (std::runtime_error &) {
            ATH_MSG_FATAL("Status array sizes do not match:"  << si_detector_element_status->getElementStatus().size() << " != " << input_element_status->getElementStatus().size()
                          << " || " << si_detector_element_status->getElementChipStatus().size() << " != " << input_element_status->getElementChipStatus().size());
            return StatusCode::FAILURE;
         }

         if (writeHandle.record( std::move(si_detector_element_status) ).isFailure()) {
            ATH_MSG_FATAL("Could not record " << writeHandle.key()  );
            return StatusCode::FAILURE;
         }
      }
      else {
         if (writeHandle.record( m_condSummaryTool->getDetectorElementStatus(ctx, m_activeOnly) ).isFailure()) {
            ATH_MSG_FATAL("Could not record " << writeHandle.key()  );
            return StatusCode::FAILURE;
         }
      }

      return StatusCode::SUCCESS;
   }
}
