/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_TrackSegmentsFinder_H
#define TRT_TrackSegmentsFinder_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"
#include "InDetRecToolInterfaces/ITRT_DetElementsRoadMaker.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
namespace InDet {


  // Class-algorithm for track finding in TRT
  //
  class TRT_TrackSegmentsFinder : public AthReentrantAlgorithm
    {
    public:

      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      TRT_TrackSegmentsFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TRT_TrackSegmentsFinder() {};
      StatusCode initialize();
      StatusCode execute(const EventContext &ctx) const;
      StatusCode finalize();

    protected:

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out, int nsegments) const;

    /// @name Flags to configure SiSPSeededTrackFinder
    //@{
    BooleanProperty m_useZvertexTool{this, "useZvertexTool", true}; 

      Gaudi::Property<bool>                         m_useCaloSeeds
       {this, "useCaloSeeds",          false,  "Use calo seeds to find TRT segments"};

      Gaudi::Property<int>                          m_minNumberDCs
       {this, "MinNumberDriftCircles", 15,      "Minimum number of DriftCircles for a TRT segment."};

      Gaudi::Property<double>                       m_ClusterEt
       {this, "CaloClusterEt",         6000.0, "Minimum ET of calo clusters in MeV too seed the TRT segment finder."};

      SG::ReadHandleKey<CaloClusterROI_Collection>  m_caloKey
       {this, "InputClusterContainerName", "InDetCaloClusterROIs", "Location of the optional Calo cluster seeds."};

      SG::WriteHandleKey<Trk::SegmentCollection>    m_foundSegmentsKey
       {this, "SegmentsLocation", "TRTSegments", "Storegate key of the found TRT segments."};

      ToolHandle<ITRT_TrackSegmentsMaker>           m_segmentsMakerTool
       {this, "SegmentsMakerTool", "InDet::TRT_TrackSegmentsMaker_ATLxk", "TRT segments maker tool." };

      ToolHandle<ITRT_DetElementsRoadMaker>         m_roadtool
       {this, "RoadTool", "InDet::TRT_DetElementsRoadMaker_xk", "Tool to build roads in the TRT."};

      SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
        
      mutable std::atomic<int>                      m_nsegmentsTotal {}  ; // statistics about number of segments

    };
}
#endif // TRT_TrackSegmentsFinder_H
