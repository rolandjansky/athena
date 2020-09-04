#ifndef AFP_MONITORING_AFPFASTRECO_H
#define AFP_MONITORING_AFPFASTRECO_H

#include <array>
#include <list>
#include <utility>
#include <vector>

#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"

namespace AFPMon {

struct AFPCluster 
{
	AFPCluster(float x_, float y_, float z_, int s, int l)
        : x {x_}, y {y_}, z {z_}, station {s}, layer {l} {}

	float x;
	float y;
	float z;
	int station;
	int layer;
 };

inline bool operator==(const AFPCluster& lhs, const AFPCluster& rhs) 
{
	if (lhs.x != rhs.x) return false;
	if (lhs.y != rhs.y) return false;
	if (lhs.z != rhs.z) return false;
	if (lhs.station != rhs.station) return false;
	if (lhs.layer != rhs.layer) return false;

	return true;
}

struct AFPTrack 
{
	AFPTrack(float x_, float y_, int s, std::array<int, 4> a)
        : x {x_}, y {y_}, station {s}, layerClusters {std::move(a)} {}

	float x;
	float y;
	int station;
	std::array<int, 4> layerClusters;
};

class AFPFastReco 
{
	public:
	/// Constructor. Sets input hit container
	AFPFastReco(const xAOD::AFPSiHitContainer* hits) : m_hitContainer {hits} {}

	/// Performs fast reconstruction of clusters and tracks
	void reco();

	/// Returns vector of clusters
	std::vector<AFPCluster> clusters() const { return m_clusters; }

	/// Returns vector of tracks
	std::vector<AFPTrack> tracks() const { return m_tracks; }

  	private:
	/// Performs fast cluster reconstruction
	void recoClusters();

	/// Performs fast track reconstruction
	void recoTracks();

	/// Returns parameters of fitted line
	std::pair<double, double> linReg(std::vector<std::pair<double, double>> YX) const;

	/// Finds hits/clusters around given init element
	template <class T>
	std::vector<T> findAround(T init, std::list<T>& toJoin) const;

	/// Checks if given hits are neighbours
	bool areNeighbours(const xAOD::AFPSiHit* lhs, const xAOD::AFPSiHit* rhs) const;

	/// Checks if given clusters are neighbours
	bool areNeighbours(const AFPCluster& lhs, const AFPCluster& rhs) const;

	/// Pointer to hit container
	const xAOD::AFPSiHitContainer* m_hitContainer;

	/// Vector of clusters
	std::vector<AFPCluster> m_clusters;

	/// Vector of tracks
	std::vector<AFPTrack> m_tracks;

	/// Number of AFP stations
	static constexpr int s_afpStations = 4;

	/// Number of layers in each station
	static constexpr int s_afpLayers = 4;

	/// Minimum number of clusters in track
	static constexpr int s_trackSize = 3;

	/// Maximum distance between cluster
	static constexpr float s_clusterDistance = 0.4;
 };

template <class T> std::vector<T> AFPFastReco::findAround(T init, std::list<T>& toJoin) const 
{
	std::vector<T> element;
	element.push_back(init);

	std::vector<T> newNeighbours;
	do 
	{
		newNeighbours.clear();
		for (auto& b : toJoin) 
		{
			bool isNew = false;
			for (auto& a : element)
			if (areNeighbours(a, b)) 
			{
				isNew = true;
				break;
 			}

			if (isNew) 
			{
				newNeighbours.push_back(b);
				element.push_back(b);
			}
		}

		for (auto& t : newNeighbours)
			toJoin.remove(t);

	} while (newNeighbours.size() > 0);

	return element;
  }

}  // namespace AFPMon

#endif /* AFP_MONITORING_AFPFASTRECO_H */

