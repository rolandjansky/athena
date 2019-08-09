/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_IMAGINGSEEDFINDER_H
#define TRKVERTEXSEEDFINDERTOOLS_IMAGINGSEEDFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexSeedFinderUtils/VertexImage.h"
#include "xAODEventInfo/EventInfo.h"

//class IBeamCondSvc; //Beam spot constraint from here

namespace Trk
{

  class Track;
  class IVertexImageMaker;
  class IVertexClusterFinder;
  class VertexCluster;

  // @author Matt Rudolph (matthew.scott.rudolph@cern.ch) Lars Egholm Pedersen (egholm...)
  // This class implements a multiseed finder using a filtering technique inspired by imaging algorithms
  // As a backup, the single findSeed methods are implemented using a cache of the result from findMultiSeed
  // @ATLAS software
  // 
  // ----------------------------------------
  // Changes:
  //
  // David Shope <david.richard.shope@cern.ch> (2016-04-19)
  //
  // EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
  //                         from Trk::RecVertex   to xAOD::Vertex,
  //                         from Trk::Vertex      to Amg::Vector3D

  class ImagingSeedFinder : public extends<AthAlgTool, IVertexSeedFinder>
  {
  public:
    // Standard Gaudi constructor.
    ImagingSeedFinder (const std::string& t,
                       const std::string& n,
                       const IInterface*  p);


    virtual ~ImagingSeedFinder();


    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;


    using IVertexSeedFinder::findSeed;

    /**
     *  Finds a linearization point out of a vector of tracks
     *  and returns it as an Amg::Vector3D object. If you want an 
     *  additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed(const std::vector<const Trk::Track*> & vectorTrk,
             const xAOD::Vertex * constraint=0) const override;
    

    /** 
     * Finds a linearization point out of a vector of TrackParameters
     *  and returns it as an Amg::Vector3D object. If you want an 
     * additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::TrackParameters*> &parametersList,
              const xAOD::Vertex * constraint=0) const override;


    /**
     * Finds full vector of linearization points from a vector of tracks
     *  and returns it as an Amg::Vector3D object.  Intended for seed finders that produce all at once.
     *  If you want an additional constraint can be taken into account.
     */
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds (const std::vector<const Trk::Track*>& vectorTrk,
                    const xAOD::Vertex * constraint=0) const override;


    /**
     * Finds full vector of linearization points from a vector
     * of TrackParameters and returns it as an Amg::Vector3D object.
     * Intended for seed finders that produce all at once.
     * If you want an additional constraint can be taken into account.
     */
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds(const std::vector<const Trk::TrackParameters*>& parametersList,
                   const xAOD::Vertex * constraint=0) const override;


  private:
    //Tool that actually makes the image to process
    ToolHandle< Trk::IVertexImageMaker > m_vertexImageMaker;

    //Clustering util
    ToolHandle< Trk::IVertexClusterFinder > m_VertexClusterFinder;
  };
}
#endif
