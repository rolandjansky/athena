//Dear emacs, this is -*-c++-*-*

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef InDetTrackPRD_Association_H
#define InDetTrackPRD_Association_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

namespace InDet {


  // Class-algorithm for track accotiation with PRD
  //
     class InDetTrackPRD_Association : public AthReentrantAlgorithm
    {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

    public:

      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      InDetTrackPRD_Association(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~InDetTrackPRD_Association() {}
      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& ctx) const override;
      virtual StatusCode finalize() override;

   private:

      ///////////////////////////////////////////////////////////////////
      // Protected data
      ///////////////////////////////////////////////////////////////////


      SG::ReadHandleKeyArray<TrackCollection> m_tracksName
         {this,"TracksName",{}}; ///< Name of track collections

      ToolHandle<Trk::IPRDtoTrackMapTool>   m_assoTool
         {this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };


      SG::WriteHandleKey<Trk::PRDtoTrackMap>  m_assoMapName
         {this,"AssociationMapName",""};  ///< the key given to the newly created association map

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

    };
}
#endif // InDetTrackPRD_Association_H
