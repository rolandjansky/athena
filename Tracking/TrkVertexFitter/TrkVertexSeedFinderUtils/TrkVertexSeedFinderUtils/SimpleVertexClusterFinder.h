/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_SIMPLEVERTEXCLUSTERFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_SIMPLEVERTEXCLUSTERFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexSeedFinderUtils/IVertexClusterFinder.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"

namespace Trk
{

  /**
   @class SimpleVertexClusterFinder

   Simple implementation of a cluster finder with interface IVertexClusterFinder for use with the imaging seed finder.  Adds all neighboring bins above threshold to cluster.

   @author Matt Rudolph <matthew.scott.rudolph@cern.ch
   @author Lars ...

   */

  //Forward declarations.
  class Vertex;

  class SimpleVertexClusterFinder : public AthAlgTool, IVertexClusterFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    SimpleVertexClusterFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~SimpleVertexClusterFinder();

    virtual std::vector<Trk::Vertex> findVertexClusters( const float *hist_rs , const int nbinsx_in, const int nbinsy_in, const int nbinsz_in );

  private:

    float m_weightThreshold;
    float m_seedCutScale;

    int m_nbinsx;
    int m_nbinsy;
    int m_nbinsz;
    int m_binstot;

    typedef std::pair<int, float> binContent;
    typedef std::vector<binContent> Cluster;

    Cluster * highestCluster( std::map<int, float> *binsRemaining );
    void addBinsToCluster( Cluster *clust, Cluster::size_type oldEdge, std::map<int, float> * binsRemaining );

    void getCenter( Cluster * cl, float & xpos, float & ypos, float & zpos);
    std::vector<int> getAdjacentBins( int index );

    int getRMBin( const int xbin_in, const int ybin_in, const int zbin_in );
    void getInvRMBin( const int rmbin, int &xbin_out, int &ybin_out, int &zbin_out);

  };
}
#endif
