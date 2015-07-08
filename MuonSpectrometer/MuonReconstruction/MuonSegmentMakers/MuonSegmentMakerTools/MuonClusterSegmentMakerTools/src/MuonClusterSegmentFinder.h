/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
//// MuonClusterSegmentFinder.h (c) ATLAS Detector software
//// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
/////////////////////////////////////////////////////////////////////////
#ifndef MUON_MUONCLUSTERSEGMENTFINDER_H
#define MUON_MUONCLUSTERSEGMENTFINDER_H

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "TrkParameters/TrackParameters.h"

#include "MuonLinearSegmentMakerUtilities/ClusterNtuple.h"

// fwd declares
class TTree;
class TFile;
class PRD_MultiTruthCollection;

namespace Trk {
  class Track;
  class MeasurementBase;
  class ITrackFitter;
  class ITrackAmbiguityProcessorTool;
}

namespace Muon {

  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class MuonPrepRawDataCollectionProviderTool;
  class IMuonPRDSelectionTool;
  class IMuonSegmentMaker;
  class IMuonClusterOnTrackCreator;
  class IMuonTrackToSegmentTool;
  class MuonEDMHelperTool;
  class IMuonTrackCleaner;
  class IMuonSegmentOverlapRemovalTool;

  class MuonClusterSegmentFinder : virtual public IMuonClusterSegmentFinder, public AthAlgTool{
  public:
    /** Default AlgTool functions */
    MuonClusterSegmentFinder(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonClusterSegmentFinder();
    StatusCode initialize();
    StatusCode finalize();
    
    /** tgc segment finding */
    void findSegments( const std::vector<const TgcPrepDataCollection*>& tgcCols, std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const;

  private:
    ToolHandle<MuonIdHelperTool>                      m_idHelper; 
    ToolHandle<MuonEDMPrinterTool>                    m_printer; 
    ToolHandle<MuonPrepRawDataCollectionProviderTool> m_muonPrepRawDataCollectionProviderTool;
    ToolHandle<IMuonPRDSelectionTool>                 m_muonPRDSelectionTool;
    ToolHandle<IMuonSegmentMaker>                     m_segmentMaker;
    ToolHandle<IMuonClusterOnTrackCreator>            m_clusterCreator;
    ToolHandle<IMuonTrackToSegmentTool>               m_trackToSegmentTool; //<! track to segment converter
    ToolHandle<Trk::ITrackFitter>                     m_slTrackFitter;  //<! fitter, always use straightline
    ToolHandle<Trk::ITrackAmbiguityProcessorTool>     m_ambiguityProcessor; //!< Tool for ambiguity solving
    ToolHandle<MuonEDMHelperTool>                     m_helper;   //<! Id helper tool
    ToolHandle<IMuonTrackCleaner>                     m_trackCleaner;
    ToolHandle<IMuonSegmentOverlapRemovalTool>        m_segmentOverlapRemovalTool;


    bool m_doNtuple;
    bool m_doTruth;
    TFile* m_file;
    TTree* m_tree;
    ClusterSeg::ClusterNtuple* m_ntuple;

    const PRD_MultiTruthCollection* getTruth(std::string name) const;
    bool matchTruth(const PRD_MultiTruthCollection& truthCol, const Identifier& id, int& barcode) const;
    Trk::Track* fit( const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar ) const;

  };

}

#endif
