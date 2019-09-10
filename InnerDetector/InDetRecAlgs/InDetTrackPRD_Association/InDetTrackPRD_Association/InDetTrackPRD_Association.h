//Dear emacs, this is -*-c++-*-*

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef InDetTrackPRD_Association_H
#define InDetTrackPRD_Association_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkToolInterfaces/IPRD_AssociationTool.h"

namespace InDet {


  // Class-algorithm for track accotiation with PRD 
  // 
     class InDetTrackPRD_Association : public AthAlgorithm 
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
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

   private:

      ///////////////////////////////////////////////////////////////////
      // Protected data 
      ///////////////////////////////////////////////////////////////////
     

      SG::ReadHandleKeyArray<TrackCollection> m_tracksName    ; // Name of track collections       
      ToolHandle<Trk::IPRD_AssociationTool> m_assoTool      ; // Assotiation tool

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

    };
}
#endif // InDetTrackPRD_Association_H
