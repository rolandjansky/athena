/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackCollectionMerger.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TrackCollectionMerger_H
#define TrackCollectionMerger_H

#include <string>
#include <map>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/TrackCollection.h"

#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"

namespace Trk {

  /** @brief Class-algorithm for track collection merging and removalof potential duplicate tracks. */
  class TrackCollectionMerger : public AthReentrantAlgorithm
    {
    
      ///////////////////////////////////////////////////////////////////
      /** @brief Public methods:                                       */
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** @brief Standard Algotithm methods:                           */
      ///////////////////////////////////////////////////////////////////

      TrackCollectionMerger(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TrackCollectionMerger() {}
      StatusCode initialize();
      StatusCode execute_r (const EventContext& ctx) const;
      StatusCode finalize();

      ///////////////////////////////////////////////////////////////////
      /** @brief Print internal tool parameters and status.            */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected data:                                       */
      ///////////////////////////////////////////////////////////////////
      SG::ReadHandleKeyArray<TrackCollection>  m_tracklocation; /** Vector of track collections to be merged. */
      SG::WriteHandleKey<TrackCollection>  m_outtracklocation  ;  /** Combined track collection.   */

      ToolHandle< Trk::IPRD_AssociationTool >                      m_assoTool          ;  /** association tool for PRDs */ 
      ToolHandle< Trk::ITrackSummaryTool >                         m_trkSummaryTool    ;  /** summary tool with shared hits enabled */

      ///////////////////////////////////////////////////////////////////
      /** @brief Protected methods:                                    */
      ///////////////////////////////////////////////////////////////////

      /** @brief A routine that merges the track collections. */
      StatusCode    mergeTrack(const TrackCollection* trackCol, TrackCollection* outputCol) const;

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    private:
      bool  m_createViewCollection;     //!< option to create a view collection and not deep-copy tracks
     
      bool m_updateSharedHitsOnly; //!< do not create the track summary again, but only update shared hits

      bool  m_updateAdditionalInfo;     //!< do not create the track summary again, but only update necessary things


    };
    
    MsgStream&    operator << (MsgStream&   ,const TrackCollectionMerger&);
    std::ostream& operator << (std::ostream&,const TrackCollectionMerger&); 
}
#endif // TrackCollectionMerger_H
