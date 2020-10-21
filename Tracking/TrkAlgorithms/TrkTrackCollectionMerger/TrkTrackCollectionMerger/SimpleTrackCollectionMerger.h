/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimpleTrackCollectionMerger.h
///////////////////////////////////////////////////////////////////
#ifndef SimpleTrackCollectionMerger_H
#define SimpleTrackCollectionMerger_H

#include <string>
#include <map>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/TrackCollection.h"

#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

namespace Trk {
  /** @brief algorithm for track collection merging and removal of potential duplicate tracks. */
  class SimpleTrackCollectionMerger : public AthAlgorithm{
      
    public:
      ///////////////////////////////////////////////////////////////////
      /** @brief Standard Algorithm methods:                           */
      ///////////////////////////////////////////////////////////////////
      SimpleTrackCollectionMerger(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~SimpleTrackCollectionMerger() {}
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

    protected:
      ///////////////////////////////////////////////////////////////////
      /** @brief Protected data:                                       */
      ///////////////////////////////////////////////////////////////////
      SG::ReadHandleKeyArray<TrackCollection>      m_inputTracksStoregateKeys; /** Vector of track collections to be merged. */
      SG::WriteHandleKey<TrackCollection>          m_combinedTracksStoregateKey  ;  /** Combined track collection.   */
      SG::WriteHandleKey<Trk::PRDtoTrackMap>       m_associationMapKey{this,"AssociationMapSG_Key",""};  ///< the key given to the newly created association map
      ToolHandle<Trk::IPRDtoTrackMapTool>          m_assoTool{this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected methods:                                    */
      ///////////////////////////////////////////////////////////////////
      /** @brief A routine that merges the track collections. */
      StatusCode    mergeTrack(const TrackCollection* trackCol, Trk::PRDtoTrackMap &prd_to_track_map, TrackCollection* outputCol);
      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;
    };
    MsgStream&    operator << (MsgStream&   , const SimpleTrackCollectionMerger&);
    std::ostream& operator << (std::ostream&, const SimpleTrackCollectionMerger&); 
}
#endif // SimpleTrackCollectionMerger_H
