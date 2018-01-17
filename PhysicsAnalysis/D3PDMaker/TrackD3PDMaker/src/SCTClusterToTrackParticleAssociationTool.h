/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_SCTCLUSTERTOTRACKPARTICLEASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_SCTCLUSTERTOTRACKPARTICLEASSOCIATIONTOOL_H
 
#include "D3PDMakerUtils/MultiAssociationTool.h"
 
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>
#include <map>

#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
#include "Identifier/Identifier.h"
#include "AthContainers/DataVector.h"

#include "Particle/TrackParticleContainer.h"


namespace Rec{
  class TrackParticle;
}


namespace Trk{
  class Track;
  class TrackStateOnSurface;
}

namespace InDet {
  class SCT_Cluster;
  class SiCluster;
}



namespace D3PD {
 
  class SCTClusterToTrackParticleAssociationTool
    : public MultiAssociationTool<InDet::SCT_Cluster, Rec::TrackParticle>
    {
    public:
      typedef MultiAssociationTool<InDet::SCT_Cluster, Rec::TrackParticle> Base;
 
      SCTClusterToTrackParticleAssociationTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent);
 
      virtual StatusCode initialize();
      virtual StatusCode book();

      virtual StatusCode reset(const InDet::SCT_Cluster& cluster);

      virtual const Rec::TrackParticle* next();

    private:

      const Rec::TrackParticleContainer* m_tr;
      int m_eventNumber;
      std::map< const Rec::TrackParticle*, std::vector<Identifier> > m_trackParticleMap;

      // StoreGate keys
      std::string m_TrackParticleContainer;
 

      std::vector<const Rec::TrackParticle* >::const_iterator m_pItr;
      std::vector<const Rec::TrackParticle* >::const_iterator m_pEnd;
      std::vector<const Rec::TrackParticle* > m_trackParticleForAssoc;


      // NTuple variables 
 
 
    }; // class SCTClusterToTrackParticleAssociationTool
 
} // namespace D3PD
 
#endif // not TRACKD3PDMAKER_SCTCLUSTERTOTRACKPARTICLEASSOCIATIONTOOL_H
