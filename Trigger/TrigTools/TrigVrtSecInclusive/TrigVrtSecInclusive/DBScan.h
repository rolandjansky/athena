/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTOOLS_TRIG_VSI_DBSCAN
#define TRIGTOOLS_TRIG_VSI_DBSCAN

/**
* @file   DBScan.h
* @brief  Clustering algorithm for vertex merging based on DBSCAN
* @author Takane Sano <takane.sano@cern.ch>
* @date   July 15th, 2021
*/
#include "TrigVrtSecInclusive/HashTools.h"

#include <vector>
#include <functional>
#include <unordered_set>
#include <unordered_map>

namespace TrigVSI {

//! Base class for clusters.
template<typename T>
class Cluster {
   public:
      Cluster(){ m_points.clear(); };
      Cluster( const std::vector<T>&  points ) : m_points( points ){};
      Cluster( std::vector<T>&&       points ) : m_points( std::move(points) ){};

      inline T getPoint(size_t ipt) const { return m_points[ipt]; };
      inline size_t nPoints() const { return m_points.size(); };

      inline std::vector<T>&  Points() { return m_points; };
      inline const std::vector<T>& getPoints() const { return m_points; };

   protected :
      std::vector<T> m_points;
};


//! Class for operating DBSCAN clustering
template<typename pointType>
class DBScan {
   public :
      using RegionFunc = std::function<std::vector<pointType>(const pointType&, double)>; //!< Function type for region query function
      DBScan( const std::unordered_set<pointType>&, RegionFunc);
      DBScan();
      ~DBScan(){};

      DBScan& operator= (DBScan&&) noexcept = default;  //!<  Move assignment operator
      DBScan(DBScan&&) noexcept = default;              //!<  Move constructor

      inline std::vector<Cluster<pointType>> getClusters() const;
      inline size_t nClusters() const { return m_clusters.size() + m_noisesCluster.size(); };
      inline const Cluster<pointType>& getCluster(size_t) const;

      void clusterize(double, size_t);

   private :
      RegionFunc  m_regionQuery;
      std::unordered_map<pointType, bool>   m_pointsVisited;
      std::vector<Cluster<pointType>>       m_clusters;
      std::unordered_set<pointType>         m_noises;
      std::vector<Cluster<pointType>>       m_noisesCluster;
};


//============================================================================
/// @class DBScan
/// @brief  Class for operating DBSCAN clustering
///
/// Pass set of points on construction, then execute clusterize() once.
/// Set of points can only be read on construction so you need to make new DBSCAN instance when you want to add new points.
///

/**
* Normal Constructor.
*
* @param[in]  set         Set of points to be clustered.
* @param[in]  regionQuery Region query function, which defines the algorithm to pick points nearby.
*/
template<typename pointType>
DBScan<pointType>::DBScan( const std::unordered_set<pointType>& set, RegionFunc regionQuery):
m_regionQuery(regionQuery)
{
   m_pointsVisited.clear();
   for (const auto& point : set) {
      m_pointsVisited.emplace(point, false);
   }
}


/**
* Default constructor
*/
template<typename pointType>
DBScan<pointType>::DBScan()
{

}


/**
* Retrun the list of clusters along with noise clusters.
*/
template<typename pointType>
std::vector<Cluster<pointType>> DBScan<pointType>::getClusters() const
{
   auto tmp = m_clusters;
   const auto& tmp_noise = m_noisesCluster;
   tmp.insert( tmp.end(), tmp_noise.begin(), tmp_noise.end() );
   return tmp;
}


/**
* Retrieve cluster.
*
* @param[in]  icls  Index of the cluster.
*
* Return different type of clusters depending on icls.
*   - icls < size of cluster list;
*     Return cluster
*   - size of cluster list <= icls < (size of cluster list) + (size of noise list);
*     Return cluster made from (icls - size of cluster list)th noise.
*/
template<typename pointType>
const Cluster<pointType>& DBScan<pointType>::getCluster(size_t icls) const
{
   const Cluster<pointType>& cls_tmp = ( icls < m_clusters.size() )? m_clusters[icls] : m_noisesCluster[icls-m_clusters.size()];
   return cls_tmp;
}


/**
* Generate clusters with DBSCAN algorithim.
*
* @param[in]  eps   Radius of region to find neighbor points.
* @param[in]  minN  Threshold to determine point as core.
*/
template<typename pointType>
void DBScan<pointType>::clusterize( double eps, size_t minN )
{
   m_clusters.clear();
   m_noisesCluster.clear();
   for (auto& pair : m_pointsVisited) {
      const pointType& point = pair.first;

      if ( pair.second ) continue;  // Continue when the point is visited
      pair.second = true;           // Set the point as visited
      std::vector<pointType> neighbor_points = m_regionQuery(point, eps);
      if ( neighbor_points.size() < minN ) {
         m_noises.emplace(point);
         continue;
      }

      m_clusters.emplace_back( std::vector<pointType>{point} );
      Cluster<pointType>& cls = *(--m_clusters.end());

      for (size_t i = 0; i < neighbor_points.size(); i++) {
         const auto& q = neighbor_points[i];

         // Check if q is labeled as noise
         // If so, remove q from noise lists and add q to cluster
         auto q_itr_noise = m_noises.find(q);
         if ( q_itr_noise != m_noises.end() ) {
            m_noises.erase(q_itr_noise);
            cls.Points().emplace_back(q);
         }

         // Check if q had been visited before
         // If visited, q is already labeled as a noise or added to other cluster so skip it
         // If not visited, label q as visited and add it to cluster
         auto q_itr = m_pointsVisited.find(q);
         if ( q_itr == m_pointsVisited.end() ) continue;
         if ( q_itr->second ) continue;
         q_itr->second = true;
         cls.Points().emplace_back(q);

         std::vector<pointType> q_neighbor_points = m_regionQuery(q, eps);
         if ( q_neighbor_points.size() >= minN ) neighbor_points.insert( neighbor_points.end(), q_neighbor_points.begin(), q_neighbor_points.end() );
      }

   }

   // Convert noises to one point clusters
   for (const auto& q : m_noises) {
      m_noisesCluster.emplace_back( std::vector<pointType>{q} );
   }
}


} // end of namespace TrigVSI

#endif
