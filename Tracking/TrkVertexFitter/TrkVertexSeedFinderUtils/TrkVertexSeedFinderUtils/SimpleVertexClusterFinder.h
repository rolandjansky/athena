/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  class SimpleVertexClusterFinder : public AthAlgTool, IVertexClusterFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    SimpleVertexClusterFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~SimpleVertexClusterFinder();

    virtual std::vector<Amg::Vector3D> findVertexClusters( const VertexImage & image );

  private:

    float m_weightThreshold;
    float m_seedCutScale;

    VertexImage m_image;

    typedef std::pair<int, float> binContent;
    typedef std::vector<binContent> Cluster;

    Cluster * highestCluster( std::map<int, float> *binsRemaining );
    void addBinsToCluster( Cluster *clust, Cluster::size_type oldEdge, std::map<int, float> * binsRemaining );

    void getCenter( Cluster * cl, float & xpos, float & ypos, float & zpos);

  };
}
#endif
