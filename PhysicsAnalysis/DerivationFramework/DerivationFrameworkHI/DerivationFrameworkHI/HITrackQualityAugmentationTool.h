/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//// HITrackQualityAugmentationTool.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HITrackQualityAugmentationTool_H
#define DERIVATIONFRAMEWORK_HITrackQualityAugmentationTool_H
 
#include <string>
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/TrackParticle.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"


namespace DerivationFramework {
 
  class HITrackQualityAugmentationTool : public AthAlgTool, public IAugmentationTool {
    public:
   enum{
      PP_MIN_BIAS=2,
      HI_LOOSE=4,
      HI_TIGHT=8,
      HI_TIGHT_TIGHTER_D0_Z0=16,
      HI_LOOSE_7SCT_HITS    =32,
      HI_TIGHT_LOOSE_D0_Z0  =64,
      HI_LOOSE_TIGHT_D0_Z0  =128,
      HI_LOOSE_TIGHTER_D0_Z0=256,
   };
      HITrackQualityAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode addBranches() const;
    private:
      unsigned short GetTrackQuality   (const xAOD::TrackParticle* track,float z_vtx           ) const;
      unsigned short GetTrackQualityNew(const xAOD::TrackParticle* track,const xAOD::Vertex* pv) const;
      ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelTool_pp      ;
      ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelTool_hi_loose;
      ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelTool_hi_tight;
  };
}
 
#endif // DERIVATIONFRAMEWORK_HITrackQualityAugmentationTool_H
