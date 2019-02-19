/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//  Header file for class  TriggerChamberClusterOnTrackCreator
//
// (c) ATLAS muon software
/////////////////////////////////////////////////////////////////////

#ifndef MUON_TRIGGERCHAMBERCLUSTERONTRACKCREATOR_H
#define MUON_TRIGGERCHAMBERCLUSTERONTRACKCREATOR_H

#include <list>
#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"

class Identifier;
class RpcIdHelper;
class TgcIdHelper;

namespace MuonGM
{
    class MuonDetectorManager;
}
namespace Track
{
    class LocalParameters;
    class Surface;
}
namespace Muon
{
    class IMuonClusterOnTrackCreator;
    class MuonClusterOnTrack;

/**
   @brief Tool to cluster several trigger measurements in different gas-gaps of the same detector module
*/
class TriggerChamberClusterOnTrackCreator: public AthAlgTool,
					   virtual public IMuonCompetingClustersOnTrackCreator
{
public:

    TriggerChamberClusterOnTrackCreator		(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent);
    virtual ~TriggerChamberClusterOnTrackCreator	 (void); 	// destructor

    StatusCode					initialize();
    StatusCode 					finalize  ();

    /** method for the creation of a single 
	Trk::CompetingRIOsOnTrack:
	It takes a list of RIOs (PrepRawData) and the given Track 
	Parameter, cutValue defines
	the cut used for calculating the initial assignment probabilities 
	(see Trk::IWeightCalculator). Not implemented for now!! 
    */
    const Trk::CompetingRIOsOnTrack* createCompetingROT(const std::list< const Trk::PrepRawData* > &,
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
    const CompetingMuonClustersOnTrack* createBroadCluster(const std::list<const Trk::PrepRawData*>&,
							   const double) const;


private:
    // helpers, managers, tools
    const RpcIdHelper*					m_rpcIdHelper;
    const TgcIdHelper*					m_tgcIdHelper;
    ToolHandle<Muon::IMuonClusterOnTrackCreator>	m_clusterCreator;

    // configuration
    bool						m_chooseBroadestCluster;
    
    // internal communication
    mutable bool					m_isRpc;
    
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
