/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Interface for MuonDriftCircleOnTrack production
// (for MDT technology)
///////////////////////////////////////////////////////////////////

#ifndef MUON_MUONCOMPETINGCLUSTERSONTRACKCREATOR_H
#define MUON_MUONCOMPETINGCLUSTERSONTRACKCREATOR_H

#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "TrkEventPrimitives/LocalParameters.h"

namespace Muon {
  /**
     @brief Tool to create MuonCompetingClustersOnTrack objects 
  */
  class MuonCompetingClustersOnTrackCreator : public AthAlgTool, virtual public IMuonCompetingClustersOnTrackCreator
  {
  public:

    MuonCompetingClustersOnTrackCreator(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonCompetingClustersOnTrackCreator()=default;
    virtual StatusCode initialize();
    
    /** method to create a CompetingMuonClustersOnTrack using the PrepRawData hits and a scaled factor for the errors */
    std::unique_ptr<const CompetingMuonClustersOnTrack> 
    createBroadCluster(const std::list< const Trk::PrepRawData * > &,   const double errorScaleFactor ) const;


  private:
   ToolHandle<Muon::IMuonClusterOnTrackCreator> m_clusterCreator{this,"ClusterCreator","Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator","pointer to muon cluster rio ontrack creator"};
 
  };

}

#endif 
