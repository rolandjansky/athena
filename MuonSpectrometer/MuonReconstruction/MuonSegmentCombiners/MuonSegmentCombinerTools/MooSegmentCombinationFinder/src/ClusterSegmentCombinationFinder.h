/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
//// ClusterSegmentCombinationFinder.h (c) ATLAS Detector software
//// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
/////////////////////////////////////////////////////////////////////////
#ifndef CLUSTERSEGMENTCOMBINATIONFINDER_H
#define CLUSTERSEGMENTCOMBINATIONFINDER_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonLinearSegmentMakerUtilities/ClusterNtuple.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"
#include <iostream>
#include <fstream>

// fwd declares
class TTree;
class TFile;
class PRD_MultiTruthCollection;

namespace Muon
{

  class ClusterSegmentCombinationFinder : virtual public IMooSegmentCombinationFinder, public AthAlgTool
    { 
    public: 

      ClusterSegmentCombinationFinder( const std::string&,const std::string&, const IInterface*);

      /** default destructor */
      virtual ~ClusterSegmentCombinationFinder(); 

      /** standard Athena-Algoithm method */
      virtual StatusCode  initialize();
      /** standard Athena-Algoithm method */
      virtual StatusCode  finalize();

      IMooSegmentCombinationFinder::Output*
        findSegments( const MdtPrepDataContainer* mdtCont,
                      const TgcPrepDataContainer* tgcCont );
      using IMooSegmentCombinationFinder::findSegments;

    private: 

      ToolHandle<Muon::IMuonClusterOnTrackCreator>       m_clusterCreator;
      ToolHandle<Muon::MuonIdHelperTool>      m_idHelperTool;
      ToolHandle<Muon::IMuonTrackToSegmentTool>           m_trackToSegmentTool; //<! track to segment converter
      ToolHandle<Trk::ITrackFitter>                 m_slTrackFitter;  //<! fitter, always use straightline
      ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor; //!< Tool for ambiguity solving
      ToolHandle<Muon::MuonEDMHelperTool>                 m_helper;   //<! Id helper tool
      ToolHandle<Muon::IMuonTrackCleaner>                 m_trackCleaner;
      ToolHandle<Trk::IResidualPullCalculator>   m_residualPullCalculator;
      ToolHandle<Trk::IExtrapolator>      m_extrapolator;
      ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>   m_mdtCreator;
      ToolHandle<Muon::IMuonSegmentMaker>             m_segmentMaker;  
      ToolHandle<Muon::IMuonSegmentOverlapRemovalTool> m_segmentOverlapRemovalTool;

      std::string m_segmentCollectionName;

      bool m_doNtuple;
      bool m_doTruth;
      TFile* m_file;
      TTree* m_tree;
      ClusterSeg::ClusterNtuple* m_ntuple;

      std::vector< std::string >                             m_truthNames;
      const PRD_MultiTruthCollection*                   m_truthCollectionTGC;
      const PRD_MultiTruthCollection*                   m_truthCollectionMDT;

      std::string         m_keyTgc; 
      std::string         m_keyTgcHits;
  
      /// Default constructor: 
      ClusterSegmentCombinationFinder();

      void getTruth();
      bool matchTruth(const PRD_MultiTruthCollection& truthCol, const Identifier& id, int& barcode);
      Amg::Vector3D intersectPlane( const Trk::PlaneSurface* surf, const Amg::Vector3D& pos, const Amg::Vector3D& dir ) const;
      Trk::Track* fit( const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar ) const; 

    }; 

} //end of namespace

#endif //> !CLUSTERSEGMENTCOMBINATIONFINDER_H
