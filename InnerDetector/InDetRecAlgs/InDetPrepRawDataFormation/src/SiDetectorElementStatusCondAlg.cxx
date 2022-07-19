/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SiDetectorElementStatusCondAlg.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/IOVInfiniteRange.h"

#include <map>

namespace {
   unsigned int computeSize(const std::vector<std::vector<unsigned short> > &vec2d) {
      unsigned int total_size = sizeof(vec2d);
      for (const std::vector<unsigned short> &vec : vec2d) {
         total_size += sizeof(vec) + vec.size()*sizeof(unsigned short);
      }
      return total_size;
   }
}

namespace InDet {
   SiDetectorElementStatusCondAlg::SiDetectorElementStatusCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
      : ::AthReentrantAlgorithm(name, pSvcLocator)
   {
   }

   StatusCode SiDetectorElementStatusCondAlg::initialize()
   {
      ATH_CHECK( m_condSummaryTool.empty() ? StatusCode::FAILURE : StatusCode::SUCCESS);
      ATH_CHECK( m_condSummaryTool.retrieve());
      ATH_CHECK( m_writeKey.initialize());
      return StatusCode::SUCCESS;
   }

   StatusCode SiDetectorElementStatusCondAlg::execute(const EventContext& ctx) const
   {
      SG::WriteCondHandle<InDet::SiDetectorElementStatus> writeHandle{m_writeKey, ctx};
      if (writeHandle.isValid()) {
         return StatusCode::SUCCESS;
      }

      // Start with infinite range and let the tool narrow it down.
      writeHandle.addDependency (IOVInfiniteRange::infiniteMixed());

      auto detector_element_status= m_condSummaryTool->getDetectorElementStatus(ctx, &writeHandle);
      const EventIDRange& range = writeHandle.getRange();
      if (!range.start().isValid() || !range.stop().isValid()) {
         ATH_MSG_FATAL("Invalid range for  " << writeHandle.key() << " : " <<  range.start() << ".." << range.stop());
      }
      unsigned int total_size = computeSize( static_cast<const InDet::SiDetectorElementStatus *>(detector_element_status.get())->getBadCells() );
      {
         unsigned  int expected;
         do  {
            expected = m_maxSize;
         } while (total_size > expected && !m_maxSize.compare_exchange_weak(expected, total_size,std::memory_order_release,std::memory_order_relaxed));
      }
      if (writeHandle.record( std::move(detector_element_status) ).isFailure()) {
         ATH_MSG_FATAL("Could not record " << writeHandle.key()  );
      }
      ATH_MSG_INFO( "SiDetectorElementStatus Recorded " << writeHandle.key() << " with Range : " << range );
      return StatusCode::SUCCESS;
   }

   StatusCode SiDetectorElementStatusCondAlg::finalize()
   {
      ATH_MSG_INFO("Max badCell size " << m_maxSize);
      return StatusCode::SUCCESS;
   }
}
