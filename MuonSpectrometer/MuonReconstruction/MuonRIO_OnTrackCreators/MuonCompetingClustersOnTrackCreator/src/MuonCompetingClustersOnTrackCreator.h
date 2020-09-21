/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

    /** method for the creation of a single 
	Trk::CompetingRIOsOnTrack:
        It takes a list of RIOs (PrepRawData) and the given Track 
	Parameter, cutValue defines
        the cut used for calculating the initial assignment probabilities 
	(see Trk::IWeightCalculator). Not implemented for now!! 
    */
    const Trk::CompetingRIOsOnTrack* createCompetingROT(const std::list< const Trk::PrepRawData * > &,
							const Trk::TrackParameters&,
							const Trk::IWeightCalculator::AnnealingFactor ) const
      {
	return 0;
      }
    
    /** method for the update of the assignment 
	probabilities and effective measurements
        of an Muon::CompetingMuonClustersOnTrack using a new track prediction. Not implemented for now!! */
    void updateCompetingROT( Trk::CompetingRIOsOnTrack&,
			     const Trk::TrackParameters&,
			     const Trk::IWeightCalculator::AnnealingFactor ) const
    {
    }
    
    /** method to create a CompetingMuonClustersOnTrack using the PrepRawData hits and a scaled factor for the errors */
    const CompetingMuonClustersOnTrack* createBroadCluster(const std::list< const Trk::PrepRawData * > &,   const double errorScaleFactor ) const;


  private:
   ToolHandle<Muon::IMuonClusterOnTrackCreator> m_clusterCreator{this,"ClusterCreator","Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator","pointer to muon cluster rio ontrack creator"};
 
  };

}

#endif 
