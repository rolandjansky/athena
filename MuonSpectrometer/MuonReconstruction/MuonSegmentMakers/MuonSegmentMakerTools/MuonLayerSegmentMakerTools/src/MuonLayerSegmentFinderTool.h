/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERSEGMENTFINDERTOOL_H
#define MUON_MUONLAYERSEGMENTFINDERTOOL_H

#include "MuonSegmentMakerToolInterfaces/IMuonLayerSegmentFinderTool.h"

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"

class ICscSegmentFinder;
namespace Muon {
  
  class MuonSegment;
  struct MuonLayerPrepRawData;
  class MuonLayerROTs;
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class MuonPrepRawDataCollectionProviderTool;
  class IMuonPRDSelectionTool;
  class IMuonSegmentMaker;
  class IMuonClusterSegmentFinder;
  class IMuonRecoValidationTool;
  class MuonLayerHoughTool;
  class MdtDriftCircleOnTrack;
  class MuonClusterOnTrack;
  class MuonLayerSegmentFinderTool : virtual public IMuonLayerSegmentFinderTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonLayerSegmentFinderTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonLayerSegmentFinderTool();
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonLayerSegmentFinderTool interface: find */   
    void find( const MuonSystemExtension::Intersection& intersection, std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const;

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

    /** tool handles */
    ToolHandle<MuonIdHelperTool>                      m_idHelper; 
    ToolHandle<MuonEDMPrinterTool>                    m_printer; 
    ToolHandle<MuonPrepRawDataCollectionProviderTool> m_muonPrepRawDataCollectionProviderTool;
    ToolHandle<IMuonPRDSelectionTool>                 m_muonPRDSelectionTool;
    ToolHandle<IMuonSegmentMaker>                     m_segmentMaker;
    ToolHandle<ICscSegmentFinder>                     m_csc2dSegmentFinder;
    ToolHandle<ICscSegmentFinder>                     m_csc4dSegmentFinder;
    ToolHandle<IMuonClusterSegmentFinder>             m_clusterSegmentFinder;
    ToolHandle<MuonLayerHoughTool>                    m_layerHoughTool;
    mutable ToolHandle<IMuonRecoValidationTool>               m_recoValidationTool;
    MuonSectorMapping                                 m_muonSectorMapping;
  };
}



#endif
 


