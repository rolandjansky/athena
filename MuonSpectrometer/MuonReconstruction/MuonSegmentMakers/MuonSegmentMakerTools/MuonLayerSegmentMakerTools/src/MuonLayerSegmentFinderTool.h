/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERSEGMENTFINDERTOOL_H
#define MUON_MUONLAYERSEGMENTFINDERTOOL_H

#include "MuonSegmentMakerToolInterfaces/IMuonLayerSegmentFinderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMuonPRDSelectionTool.h"
#include "CscSegmentMakers/ICscSegmentFinder.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinder.h"
#include "MuonHoughPatternTools/MuonLayerHoughTool.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinderTool.h"
#include "MuonRecToolInterfaces/IMuonRecoValidationTool.h"

#include <string>
#include <vector>

namespace Muon {
  
  class MuonSegment;
  struct MuonLayerPrepRawData;
  class MuonLayerROTs;
  class MdtDriftCircleOnTrack;
  class MuonClusterOnTrack;
  class MuonLayerSegmentFinderTool : virtual public IMuonLayerSegmentFinderTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonLayerSegmentFinderTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonLayerSegmentFinderTool()=default;
    StatusCode initialize();

    /**IMuonLayerSegmentFinderTool interface: find */   
    void find( const MuonSystemExtension::Intersection& intersection, std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments, MuonLayerPrepRawData& layerPrepRawData ) const;

  private:
    /** find segments from PRD clusters */
    void findClusterSegments( const MuonSystemExtension::Intersection& intersection, const MuonLayerPrepRawData& layerPrepRawData, 
                              std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const;

    /** find csc segments */
    void findCscSegments( const MuonLayerPrepRawData& layerPrepRawData, std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const;

    /** find mdt segments from hits in the layer */
    void findMdtSegments( const MuonSystemExtension::Intersection& intersection, const MuonLayerPrepRawData& layerPrepRawData,
                          std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const;

    /** find mdt segments from Hough output */
    void findMdtSegmentsFromHough( const MuonSystemExtension::Intersection& intersection, const MuonLayerPrepRawData& layerPrepRawData,
                                   std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const;

    /** find mdt segments main routine */
    void findMdtSegments( const MuonSystemExtension::Intersection& intersection,
                          const std::vector<const MdtDriftCircleOnTrack*>& mdts,
                          const std::vector<const MuonClusterOnTrack*>&    clusters,
                          std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const;

    /** storegate */
    SG::ReadHandleKey<MuonLayerHoughTool::HoughDataPerSectorVec> m_houghDataPerSectorVecKey {this, 
        "Key_MuonLayerHoughToolHoughDataPerSectorVec", "HoughDataPerSectorVec", "HoughDataPerSectorVec key"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<MuonEDMPrinterTool>                    m_printer; 
    ToolHandle<IMuonPRDSelectionTool>                 m_muonPRDSelectionTool;
    ToolHandle<IMuonSegmentMaker>                     m_segmentMaker;
    ToolHandle<ICscSegmentFinder>                     m_csc2dSegmentFinder;
    ToolHandle<ICscSegmentFinder>                     m_csc4dSegmentFinder;
    ToolHandle<IMuonClusterSegmentFinder>             m_clusterSegmentFinder;
    ToolHandle<IMuonClusterSegmentFinderTool>         m_clusterSegMakerNSW;
    mutable ToolHandle<IMuonRecoValidationTool>               m_recoValidationTool;
    MuonSectorMapping                                 m_muonSectorMapping;
  };
}



#endif
 


