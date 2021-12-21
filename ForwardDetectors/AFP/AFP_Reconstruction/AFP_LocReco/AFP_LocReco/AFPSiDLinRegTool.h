/*
	Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @file	AFPSiDLinRegTool.h
/// @author Petr Balek <petr.balek@cern.ch>
/// @date	2021-12-14
/// 
/// @brief	Header file for AFPSiDLinRegTool used in tracks reconstruction. Adapted from AfpAnalysisToolbox.

#ifndef AFP_LOCRECO_AFPSIDLINREGTOOL_H
#define AFP_LOCRECO_AFPSIDLINREGTOOL_H



// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// xAOD includes
#include "xAODForward/AFPSiHitsCluster.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrack.h"
#include "xAODForward/AFPTrackRecoAlgID.h"

// AFP_LocReco includes
#include "AFP_LocReco/IAFPSiDLocRecoTrackAlgTool.h"

// STL includes
#include <string>
#include <list>
#include <vector>
#include <tuple>
#include <memory>
#include <utility>


/// Tool for reconstructing AFPTrack parameters from track candidates.
class AFPSiDLinRegTool
: public extends<AthAlgTool, IAFPSiDLocRecoTrackAlgTool>
{

public:
	AFPSiDLinRegTool(const std::string& type, const std::string& name, const IInterface* parent);

	virtual ~AFPSiDLinRegTool() override {}

	/// Read parameters from job options and print tool configuration
	virtual StatusCode initialize() override;


	/// @brief Reconstructs tracks properties from track candidates
	///
	/// * Loops over all tracks candidates
	/// * Calcutates and sets track properties
	StatusCode reconstructTracks(std::unique_ptr<xAOD::AFPTrackContainer>& outputContainer, const EventContext& ctx) const override;

	/// @copydoc IAFPSiDLocRecoTrackAlgTool::outputContainerName(xAOD::AFPTrackContainer*)
	const std::string& outputContainerName () const override {return m_tracksContainerName;}

private:
	/// Name of the xAOD container to which tracks will be saved; actual saving is done in AFPSIDLocRecoTool
	Gaudi::Property<std::string> m_tracksContainerName{this, "tracksContainerName", "AFPTrackContainer", "Name of the container in which tracks are saved"};

	/// AFP station ID for which tracks will be reconstructed
	Gaudi::Property<int> m_stationID{this, "stationID", 0, "ID number of station for which tracks should be reconstructed"};
	
	/// Maximum allowed distance between clusters to be considered coming from the same proton
	Gaudi::Property<double> m_allowedDistanceBetweenClustersInTrack{this, "allowedDistanceBetweenClustersInTrack", 0.5, "Maximum allowed distance between clusters to be considered coming from the same proton"};
	
	/// Track selection type:
    /// * Loose: accept all track candidates
    /// * Medium: accept only tracks with clusters in at least #m_minimalNumberOfPlanesInTrack planes
    /// * Tight: reject all with more than a single cluster per plane and less than #m_minimalNumberOfPlanesInTrack planes
	Gaudi::Property<std::string> m_trackSelection{this, "trackSelection", "Medium", "Track selection, allowed values are \"Loose\" (accept all track candidates), \"Medium\" (accept only tracks with clusters in at least #m_minimalNumberOfPlanesInTrack planes), or \"Tight\" (reject all with more than a single cluster per plane and less than #m_minimalNumberOfPlanesInTrack planes)"};
	
	/// Minimal numer of planes required for track candidate
	Gaudi::Property<unsigned int>  m_minimalNumberOfPlanesInTrack{this, "minimalNumberOfPlanesInTrack", 2, "Minimal numer of planes required for track candidate"};
	
	// function to select tracks
	std::function<bool(const std::vector<const xAOD::AFPSiHitsCluster*>&)> m_selectionFunction;
	
	/// Name of the xAOD container with clusters to be used in track reconstruction
	SG::ReadHandleKey<xAOD::AFPSiHitsClusterContainer> m_hitsClusterContainerKey{this, "AFPSiHitsClusterContainerKey", "AFPSiHitsClusterContainer", "Name of the container with clusters of hits from which tracks are to be reconstructed"};
	
	/// @brief Looks for cluster around init cluster
    ///
    /// * Loops over clusters in the list toJoin
    /// * Checks if those clusters are neighbours of init cluster
    /// * Adds neighbouring cluster to cluster vector and removes them from the list
    ///
    /// @return Vector of clusters - track candiate
	std::vector<const xAOD::AFPSiHitsCluster*> findClusterAroundElement(std::list<const xAOD::AFPSiHitsCluster*>& toJoin, const xAOD::AFPSiHitsCluster* init) const;
	
	/// Checks if clusters are neighbours
	/// Compares distance between them to #m_allowedDistanceBetweenClustersInTrack
	bool areNeighbours(const xAOD::AFPSiHitsCluster* a, const xAOD::AFPSiHitsCluster* b) const;
    
	/// Simple linear regression
	/// Used to calculate track postions and slopes
	std::pair<double, double> linearRegression(std::vector<std::pair<double, double>> YX) const;
	};


#endif
