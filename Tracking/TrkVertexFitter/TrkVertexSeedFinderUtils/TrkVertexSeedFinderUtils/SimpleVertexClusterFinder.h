/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_SIMPLEVERTEXCLUSTERFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_SIMPLEVERTEXCLUSTERFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexSeedFinderUtils/IVertexClusterFinder.h"
#include "TrkVertexSeedFinderUtils/VertexImage.h"

namespace Trk
{

  /**
   @class SimpleVertexClusterFinder

   Simple implementation of a cluster finder with interface IVertexClusterFinder for use with the imaging seed finder.  Adds all neighboring bins above threshold to cluster.

   @author Matt Rudolph <matthew.scott.rudolph@cern.ch
   @author Lars ...

   ------------------------------------
   Changes:

   David Shope <david.richard.shope@cern.ch> (2016-04-19)

   EDM Migration to xAOD - from Trk::Vertex to Amg::Vector3D

   */

  class SimpleVertexClusterFinder : public extends<AthAlgTool, IVertexClusterFinder>
  {
  public:
    //default constructor due to Athena interface
    SimpleVertexClusterFinder(const std::string& t, const std::string& n, const IInterface*  p);
    

    //destructor
    virtual ~SimpleVertexClusterFinder() = default;

    
    virtual std::vector<Amg::Vector3D> findVertexClusters( const VertexImage & image ) const override;

  private:
    float m_weightThreshold;
    float m_seedCutScale;

    typedef std::pair<int, float> binContent;
    typedef std::vector<binContent> Cluster;

    std::unique_ptr<Cluster>
    highestCluster( const VertexImage& image,
                    std::map<int, float>& binsRemaining ) const;

    void addBinsToCluster( const VertexImage& image,
                           Cluster& clust,
                           Cluster::size_type oldEdge,
                           std::map<int, float>& binsRemaining ) const;

    void getCenter( const VertexImage& image,
                    const Cluster& cl,
                    float & xpos, float & ypos, float & zpos) const;

  };
}
#endif
