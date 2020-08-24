/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 *   *
 *   *
 *   *       AFPFastReco.cxx
 *   *
 *   *
 *   */

#include <Run3AFPMonitoring/AFPFastReco.h>

namespace AFPMon {

void AFPFastReco::reco() 
{
	recoClusters();
	recoTracks();
}

void AFPFastReco::recoClusters() 
{
	constexpr float dx   = 0.25;  // [mm]
    constexpr float dy   = 0.05;  // [mm]
    constexpr float dz   = 9.00;  // [mm]
    constexpr float tilt = 14. / 180. * 3.14159;

    std::list toCluster(m_hitContainer->begin(), m_hitContainer->end());

<<<<<<< HEAD
    while (toCluster.size() > 0) 
	{
		auto init = *(toCluster.begin());
		toCluster.pop_front();
		auto clusteredHits = findAround(init, toCluster);

		float sumX      = 0;
		float sumY      = 0;
		float sumCharge = 0;
		for (const xAOD::AFPSiHit* h : clusteredHits) 
		{
        	const float charge = h->depositedCharge();
			const float pixX   = dx * h->pixelColIDChip();
			const float pixY   = dy * h->pixelRowIDChip();
			sumX += charge * pixX;
			sumY += charge * pixY;
			sumCharge += charge;
		}

		const float xPlane = sumX / sumCharge;
		const float yPlane = sumY / sumCharge;

		const int stationID = init->stationID();
		const int layerID   = init->pixelLayerID();

		const float x = xPlane;
		const float y = yPlane * cos(tilt);
		const float z = yPlane * sin(tilt) + dz * layerID;

		m_clusters.emplace_back(x, y, z, stationID, layerID);
=======
    while (toCluster.size() > 0) {
      auto init = *(toCluster.begin());
      toCluster.pop_front();
      auto clusteredHits = findAround(init, toCluster);

      float sumX      = 0;
      float sumY      = 0;
      float sumCharge = 0;
      for (const xAOD::AFPSiHit* h : clusteredHits) {
        const float charge = h->depositedCharge();
        const float pixX   = dx * h->pixelColIDChip();
        const float pixY   = dy * h->pixelRowIDChip();
        sumX += charge * pixX;
        sumY += charge * pixY;
        sumCharge += charge;
      }

      const float xPlane = sumX / sumCharge;
      const float yPlane = sumY / sumCharge;

      const int stationID = init->stationID();
      const int layerID   = init->pixelLayerID();

      const float x = xPlane;
      const float y = yPlane * cos(tilt);
      const float z = yPlane * sin(tilt) + dz * layerID;

      m_clusters.emplace_back(x, y, z, stationID, layerID);
>>>>>>> upstream/master
    }
}

void AFPFastReco::recoTracks() 
{
	std::list toTrack(m_clusters.begin(), m_clusters.end());

	while (toTrack.size() > 0) 
	{
		auto init = *(toTrack.begin());
		toTrack.pop_front();
		auto trackCandidate = findAround(init, toTrack);

		if (trackCandidate.size() < s_trackSize) continue;

		std::array<int, 4> clusters {};

		std::vector<std::pair<double, double>> XZ;
		std::vector<std::pair<double, double>> YZ;

		for (const auto& cluster : trackCandidate) 
		{
			clusters[cluster.layer]++;

			XZ.emplace_back(cluster.x, cluster.z);
			YZ.emplace_back(cluster.y, cluster.z);
		}

		const auto [x, xSlope] = linReg(XZ);
		const auto [y, ySlope] = linReg(YZ);
		const int station      = trackCandidate[0].station;

		m_tracks.emplace_back(x, y, station, clusters);
    }
}

std::pair<double, double> AFPFastReco::linReg(std::vector<std::pair<double, double>> YX) const 
{
    double meanx = 0;
    double meany = 0;
    for (const auto& yx : YX) 
	{
		meany += yx.first;
		meanx += yx.second;
    }

	meanx /= YX.size();
    meany /= YX.size();

    double numerator   = 0;
    double denumerator = 0;
    for (const auto& yx : YX) 
	{
		const double dy = yx.first - meany;
		const double dx = yx.second - meanx;
		numerator += dx * dy;
		denumerator += dx * dx;
	}

	const double slope    = numerator / denumerator;
	const double position = meany - slope * meanx;

	return {position, slope};
}

bool AFPFastReco::areNeighbours(const xAOD::AFPSiHit* lhs, const xAOD::AFPSiHit* rhs) const 
{
	if (lhs->stationID() != rhs->stationID()) return false;
	if (lhs->pixelLayerID() != rhs->pixelLayerID()) return false;
	if (lhs->pixelColIDChip() != rhs->pixelColIDChip()) return false;
	if (abs(lhs->pixelRowIDChip() - rhs->pixelRowIDChip()) != 1) return false;

	return true;
}

bool AFPFastReco::areNeighbours(const AFPCluster& lhs, const AFPCluster& rhs) const 
{
	if (lhs.station != rhs.station) return false;

	const float dx = lhs.x - rhs.x;
	const float dy = lhs.y - rhs.y;
	if (dx * dx + dy * dy > s_clusterDistance) return false;

	return true;
}

}  // namespace AFPMon

