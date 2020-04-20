/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "ROIInfoVecAlg.h"
#include <memory>

StatusCode ROIInfoVecAlg::initialize() {
   ATH_CHECK(m_inputEmClusterContainerKey.initialize());
   ATH_CHECK(m_outputROIInfoKey.initialize());
   return StatusCode::SUCCESS;
}


StatusCode ROIInfoVecAlg::execute(const EventContext& ctx) const
{
   SG::ReadHandle<CaloClusterROI_Collection> calo(m_inputEmClusterContainerKey,ctx);
   std::unique_ptr<InDet::ROIInfoVec> info( std::make_unique<InDet::ROIInfoVec>());
   info->reserve(calo->size());
   for( const Trk::CaloClusterROI* ccROI : *calo) {
      Amg::Vector3D pos(ccROI->globalPosition());
      if( ccROI->energy() * sin(pos.theta()) < m_minPtEm){
         continue;
      }
      info->emplace_back( pos.phi(),
                          pos.perp(),
                          pos.z() );
   }

   ATH_CHECK( SG::WriteHandle<InDet::ROIInfoVec>(m_outputROIInfoKey,ctx).record (std::move(info)) );
   return StatusCode::SUCCESS;
}
