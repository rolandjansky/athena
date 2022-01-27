/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @file	 AFPSiDLinRegTool.cxx
/// @author Petr Balek <petr.balek@cern.ch>
/// @date	 2021-12-14
/// 
/// @brief  Implementation file for AFPSiDLinRegTool used in tracks reconstruction.


#include "AFP_LocReco/AFPSiDLinRegTool.h"


AFPSiDLinRegTool::AFPSiDLinRegTool( const std::string& type, 
									const std::string& name, 
									const IInterface* parent ) : 
  base_class( type, name, parent )
{
}


StatusCode AFPSiDLinRegTool::initialize()
{
	if(m_trackSelection=="loose" || m_trackSelection=="Loose")
	{
		m_selectionFunction = [](const std::vector<const xAOD::AFPSiHitsCluster*>&) { return true; };
	}
	else if(m_trackSelection=="medium" || m_trackSelection=="Medium")
	{
		m_selectionFunction = [this](const std::vector<const xAOD::AFPSiHitsCluster*>& t) {
			std::vector<int> plane{0, 0, 0, 0};

			for (auto&& cluster : t) {
				const int p = cluster->pixelLayerID();
				plane[p]++;
			}

			auto HasAnyClusters = [](int n) { return n > 0; };

			if (std::count_if(plane.begin(), plane.end(), HasAnyClusters) < m_minimalNumberOfPlanesInTrack) { return false; }

			return true;
		};
	}
	else if(m_trackSelection=="tight" || m_trackSelection=="Tight")
	{
		m_selectionFunction = [this](const std::vector<const xAOD::AFPSiHitsCluster*>& t) {
			std::vector<int> plane{0, 0, 0, 0};

			for (auto&& cluster : t) {
				const int p = cluster->pixelLayerID();
				plane[p]++;
			}

			auto HasMultipleClusters = [](int n) { return n > 1; };

			if (std::any_of(plane.begin(), plane.end(), HasMultipleClusters)) { return false; }

			// Check number of planes (cluster/plane == 1)
			if (t.size() < m_minimalNumberOfPlanesInTrack) { return false; }

			return true;
		};
	}
	else
	{
		ATH_MSG_ERROR("unknown trackSelection "<<m_trackSelection<<", allowed values are \"Loose\", \"Medium\", or \"Tight\" ");
		return StatusCode::FAILURE;
	}
	
	CHECK( m_hitsClusterContainerKey.initialize() );
	
	return StatusCode::SUCCESS;
}


bool AFPSiDLinRegTool::areNeighbours(const xAOD::AFPSiHitsCluster* a, const xAOD::AFPSiHitsCluster* b) const
{
	// All clusters should be in same station, so no need for checking

	const double xa = a->xLocal();
	const double ya = a->yLocal();

	const double xb = b->xLocal();
	const double yb = b->yLocal();

	const double dx = xa - xb;
	const double dy = ya - yb;
	const double maxDistanceSq = m_allowedDistanceBetweenClustersInTrack * m_allowedDistanceBetweenClustersInTrack;
	if (dx * dx + dy * dy > maxDistanceSq) { return false; }

	return true;
}


std::vector<const xAOD::AFPSiHitsCluster*> AFPSiDLinRegTool::findClusterAroundElement(std::list<const xAOD::AFPSiHitsCluster*>& toJoin, const xAOD::AFPSiHitsCluster* init) const 
{
	// Push initial cluster
	std::vector<const xAOD::AFPSiHitsCluster*> track;
	track.push_back(init);

	std::vector<const xAOD::AFPSiHitsCluster*> newNeighbours;

	// Repeat while new neighbours are found
	do {
	  newNeighbours.clear();
	  for (auto&& b : toJoin) {
		bool isNew = false;
		for (auto&& a : track) {
		  if (areNeighbours(a, b)) { isNew = true; }
		}

		if (isNew) {
		  newNeighbours.push_back(b);
		  track.push_back(b);
		}
	  }

	  // Clustered clusters are taken out from "clusters" container
	  for (auto& t : newNeighbours) {
		toJoin.remove(t);
	  }

	} while (newNeighbours.size() > 0);

	return track;
}
  

std::pair<double, double> AFPSiDLinRegTool::linearRegression(std::vector<std::pair<double, double>> YX) const
{
	// here, "x" is Z axis and "y" is X or Y axis
	
	double meanx = 0, meany = 0;
	for (const auto& [y, x] : YX) {
		meany += y;
		meanx += x;
	}
	meanx /= YX.size();
	meany /= YX.size();
	
	double numerator = 0, denominator = 0;
	for (const auto& [y, x] : YX) {
		const double dy = y - meany;
		const double dx = x - meanx;
				
		numerator += dx * dy;
		denominator += dx * dx;
	}
	
	const double slope = numerator / denominator;
	const double position = meany - slope * meanx;

	return {position, slope};
}


StatusCode AFPSiDLinRegTool::reconstructTracks(std::unique_ptr<xAOD::AFPTrackContainer>& outputContainer, const EventContext& ctx) const
{
	SG::ReadHandle<xAOD::AFPSiHitsClusterContainer> clusters( m_hitsClusterContainerKey, ctx );
	if(!clusters.isValid())
	{
		// this is allowed, there might be no AFP data in the input
		return StatusCode::SUCCESS;
	}
	
	// Select clusters in given station
	std::list<const xAOD::AFPSiHitsCluster*> clustersInStation;
	std::copy_if(clusters->begin(), clusters->end(), std::back_inserter(clustersInStation),
							[this](auto cluster) { return cluster->stationID() == m_stationID; });

	ATH_MSG_DEBUG("clusters in " << m_stationID << " size: " << clustersInStation.size());


	// Loop until all clusters are used
	while (clustersInStation.size() > 0)
	{
		// Take first element from the list...
		const auto initialCluster = clustersInStation.front();
		clustersInStation.pop_front();

		// ...and find other clusters around it
		const auto clusterOfClusters = findClusterAroundElement(clustersInStation, initialCluster);

		// If track candidate does not pass chosen selection, skip it
		if (!m_selectionFunction(clusterOfClusters)) {
			ATH_MSG_DEBUG("track candidate of size " << clusterOfClusters.size() << " did not pass selection.");
			continue;
		}
		
		auto track = outputContainer->push_back(std::make_unique<xAOD::AFPTrack>());

		// Set stationID
		track->setStationID(m_stationID);

		// Add links to clusters
		for (const auto cluster : clusterOfClusters) {
			ElementLink<xAOD::AFPSiHitsClusterContainer> clusterLink;
			clusterLink.toContainedElement(*clusters, cluster);
			track->addCluster(clusterLink);
		}

		const int nClusters = track->clusters().size();
		
		if(nClusters>0)
		{
			track->setNClusters(nClusters);
		}
		else
		{
			ATH_MSG_DEBUG("track candidate has 0 clusters, erasing ");
			outputContainer->pop_back();
			continue;
		}
		
		// Fit the clusters with a line in X and Y direction along Z
		double x = 0.;
		double y = 0.;
		double xSlope = 0.;
		double ySlope = 0.;
		double chi2	 = 0.;

		if (nClusters == 1) {
			x = (*(track->clusters().at(0)))->xLocal();
			y = (*(track->clusters().at(0)))->yLocal();
		} 
		else
		{			
			std::vector<std::pair<double, double>> XZ;
			std::vector<std::pair<double, double>> YZ;

			for (int i = 0; i < nClusters; i++) {
				const xAOD::AFPSiHitsCluster* cluster = *(track->clusters().at(i));
								
				XZ.push_back({cluster->xLocal(), cluster->zLocal()});
				YZ.push_back({cluster->yLocal(), cluster->zLocal()});
			}

			auto linregx = linearRegression(XZ);
			auto linregy = linearRegression(YZ);

			std::tie(x, xSlope) = linregx;
			std::tie(y, ySlope) = linregy;

			for (int i = 0; i < nClusters; i++) {
				const xAOD::AFPSiHitsCluster* cluster = *(track->clusters().at(i));

				const double z	= cluster->zLocal();
				const double ex = cluster->xLocalErr();
				const double ey = cluster->yLocalErr();

				const double dx = cluster->xLocal() - (x + xSlope * z);
				const double dy = cluster->yLocal() - (y + ySlope * z);
								
				chi2 += dx * dx / (ex * ex);
				chi2 += dy * dy / (ey * ey);
			}
		}

		// Set track properties
		track->setChi2(chi2);
		track->setXLocal(x);
		track->setXSlope(xSlope);
		track->setYLocal(y);
		track->setYSlope(ySlope);
		track->setZLocal(0.); // because positions of x/y are calculated at z=0
		
		track->setAlgID(xAOD::AFPTrackRecoAlgID::linReg);
		
		std::vector<int> clustersPlane{0, 0, 0, 0};
		for (auto cl : track->clusters()) {
			++clustersPlane[(*cl)->pixelLayerID()];
		}
		auto HasNoClusters = [](int n) { return n == 0; };
		track->setNHoles(std::count_if(clustersPlane.begin(), clustersPlane.end(), HasNoClusters));
		

		ATH_MSG_DEBUG("new track with " << track->clusters().size() << " clusters, x: " << track->xLocal()
						<< ", y: " << track->yLocal() << ", sx: " << track->xSlope()
						<< ", sy: " << track->ySlope() << ", chi2: " << track->chi2());
		for (auto&& cluster : track->clusters())
		{
			ATH_MSG_DEBUG('\t' << (*cluster)->xLocal() << " " << (*cluster)->yLocal() << " " << (*cluster)->zLocal());
		}
	}
	
	return StatusCode::SUCCESS;
}
