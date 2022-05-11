/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackCollectionMerger.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TrackCollectionMerger_H
#define TrackCollectionMerger_H

#include <string>
#include <map>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/TrackCollection.h"

#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

namespace Trk {

  /** @brief Class-algorithm for track collection merging and removalof potential duplicate tracks. */
  class TrackCollectionMerger : public AthAlgorithm
    {
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** @brief Standard Algotithm methods:                           */
      ///////////////////////////////////////////////////////////////////

      TrackCollectionMerger(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TrackCollectionMerger() {}
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

    protected:

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected data:                                       */
      ///////////////////////////////////////////////////////////////////
      SG::ReadHandleKeyArray<TrackCollection>      m_tracklocation; /** Vector of track collections to be merged. */
      SG::ReadHandleKey<TrackCollection> m_overlayTrackLocation{this,"OverlayTracksLocation","","Pileup Track Collection"}; /**overlay track collection, if track overlay is on; default is empty string, i.e. track overlay is off */
      //note that these handles are not used directly, they are needed here just to ensure that these collections get loaded into SG
      SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_pileupTRT{this,"OverlayTRTClusters","Bkg_TRT_DriftCircles","Pileup Drift Circles"}; /** pileup TRT PRDs, only retrieved if track overlay is on */
      SG::ReadHandleKey<InDet::PixelClusterContainer> m_pileupPixel{this,"OverlayPixelClusters","Bkg_PixelClusters","Pileup Pixel Clusters"}; /** pileup pixel PRDs, only retrieved if track overlay is on*/
      SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_pileupSCT{this,"OverlaySCTClusters","Bkg_SCT_Clusters","Pileup SCT Clusters"}; /** pileup SCT PRDs, only retrieved if track overlay is on */
      SG::WriteHandleKey<TrackCollection>          m_outtracklocation  ;  /** Combined track collection.   */
      SG::WriteHandleKey<Trk::PRDtoTrackMap>       m_assoMapName
         {this,"AssociationMapName",""};  ///< the key given to the newly created association map

      ToolHandle<Trk::IPRDtoTrackMapTool>          m_assoTool
         {this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };

      ToolHandle< Trk::IExtendedTrackSummaryTool > m_trkSummaryTool    ;  /** summary tool with shared hits enabled */

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected methods:                                    */
      ///////////////////////////////////////////////////////////////////

      /** @brief A routine that merges the track collections. */
      StatusCode    mergeTrack(const TrackCollection* trackCol,
                               Trk::PRDtoTrackMap *pPrdToTrackMap,
                               TrackCollection* outputCol);

    private:
      bool  m_createViewCollection;     //!< option to create a view collection and not deep-copy tracks
     
      bool m_updateSharedHits; //!< do not create the track summary again, but only update shared hits

      bool  m_updateAdditionalInfo;     //!< do not create the track summary again, but only update necessary things

      bool m_doTrackOverlay; //doing track overlay: needed to initialize the background PRD containers


    };
    
}
#endif // TrackCollectionMerger_H
