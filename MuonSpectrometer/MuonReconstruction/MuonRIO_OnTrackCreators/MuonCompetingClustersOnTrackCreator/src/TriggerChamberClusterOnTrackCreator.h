/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TRIGGERCHAMBERCLUSTERONTRACKCREATOR_H
#define MUON_TRIGGERCHAMBERCLUSTERONTRACKCREATOR_H

#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

#include <list>
#include <vector>
#include <string>

namespace Track
{
    class LocalParameters;
    class Surface;
}
namespace Muon
{
    class MuonClusterOnTrack;

/**
   @brief Tool to cluster several trigger measurements in different gas-gaps of the same detector module
*/
class TriggerChamberClusterOnTrackCreator: public AthAlgTool, virtual public IMuonCompetingClustersOnTrackCreator
{
public:

    TriggerChamberClusterOnTrackCreator(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TriggerChamberClusterOnTrackCreator()=default;

    StatusCode initialize();
    
    /** method to create a CompetingMuonClustersOnTrack using the PrepRawData hits and a scaled factor for the errors */
    std::unique_ptr<const CompetingMuonClustersOnTrack>
    createBroadCluster(const std::list<const Trk::PrepRawData*>&,
							   const double) const;


private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    ToolHandle<Muon::IMuonClusterOnTrackCreator> m_clusterCreator{this,"ClusterCreator","Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"};

    // configuration
    bool						m_chooseBroadestCluster;
    
    // private methods 
    void						applyClusterConsistency(
	std::list<int>&						limitingChannels,
	std::list<const Muon::MuonClusterOnTrack*>&		limitingRots) const;

    std::vector<const Muon::MuonClusterOnTrack*>*	createPrdRots(
	const std::list<const Trk::PrepRawData*>&		prds) const;
  
    void						makeClustersBySurface(
	std::list<int>&						limitingChannels,
	std::list<const Muon::MuonClusterOnTrack*>&		limitingRots,
	const std::list<const Trk::PrepRawData*>&		prds,
	const std::vector<const Muon::MuonClusterOnTrack*>&	rots) const;

    void						makeOverallParameters(
	Trk::LocalParameters*&					parameters,
	Amg::MatrixX*&						errorMatrix,
	Trk::Surface*&						surface,
	std::list<int>&						limitingChannels,
	std::list<const Muon::MuonClusterOnTrack*>&		limitingRots) const; 
};

}	// end of namespace

#endif	// MUON_TRIGGERCHAMBERCLUSTERONTRACKCREATOR_H
