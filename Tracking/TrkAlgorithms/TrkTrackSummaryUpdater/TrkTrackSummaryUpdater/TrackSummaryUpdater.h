/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrackSummaryUpdater_H
#define TrackSummaryUpdater_H

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

  /** @brief Algorithm for updating the track summary of a track collection */
  class TrackSummaryUpdater : public AthAlgorithm{
    public:
      TrackSummaryUpdater(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TrackSummaryUpdater() {}
      StatusCode initialize() override;
      StatusCode execute() override;
      StatusCode finalize() override;
      
    protected:
      SG::ReadHandleKey<TrackCollection>           m_inputTracksStoregateKey  ;  /** Input track collection.   */
      SG::WriteHandleKey<TrackCollection>          m_updatedTracksStoregateKey  ;  /** Input track collection with updated summary.   */
      SG::WriteHandleKey<Trk::PRDtoTrackMap>       m_assoMapName{this,"AssociationMapName",""};  ///< the key given to the newly created association map
      ToolHandle<Trk::IPRDtoTrackMapTool>          m_assoTool{this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };
      ToolHandle< Trk::IExtendedTrackSummaryTool > m_trkSummaryTool    ;  /** summary tool with shared hits enabled */

    private:
      bool  m_updateSharedHits; //!< do not create the track summary again, but only update shared hits
      bool  m_updateAdditionalInfo;     //!< do not create the track summary again, but only update necessary things
    };
}
#endif // TrackSummaryUpdater_H

