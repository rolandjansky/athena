/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_LOCALMAX1DCLUSTERFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_LOCALMAX1DCLUSTERFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexSeedFinderUtils/IVertexClusterFinder.h"


namespace Trk
{

  /**
   @class LocalMax1DClusterFinder

   Projection implementation of a cluster finder with interface IVertexClusterFinder for use with the imaging seed finder.  Adds all neighboring bins above threshold to cluster.

   @author Matt Rudolph <matthew.scott.rudolph@cern.ch

   ------------------------------------
   Changes:

   David Shope <david.richard.shope@cern.ch> (2016-04-19)

   EDM Migration to xAOD - from Trk::Vertex to Amg::Vector3D

   */

  //Forward declarations.
  class Vertex;
  class VertexImage;

  class LocalMax1DClusterFinder : public AthAlgTool, IVertexClusterFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    LocalMax1DClusterFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~LocalMax1DClusterFinder();

    virtual std::vector<Amg::Vector3D> findVertexClusters( const VertexImage & image );

    typedef std::pair<int, float> Projection;

  private:

    float m_weightThreshold;
    float m_mergeParam;
    float m_clusterWindowXY;
    bool  m_refineZ;
    bool  m_gaussianWindow;
  };
}
#endif
