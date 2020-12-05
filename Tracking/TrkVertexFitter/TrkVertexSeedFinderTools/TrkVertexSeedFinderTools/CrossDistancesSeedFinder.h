/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_CROSSDISTANCESSEEDFINDER_H
#define TRKVERTEXSEEDFINDERTOOLS_CROSSDISTANCESSEEDFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk
{
  class Track;
  class IMode3dFinder;
  class ITrkDistanceFinder;

  // @author N. Giacinto Piacquadio (Albert-Ludwigs-Universitaet Freiburg - Germany)
  //
  // @ATLAS software
  //
  // This class implements a seed finder for the vertexing algorithm 
  // which is based on the mean points of the distances between all tracks 
  // (taken two by two). There's actually no selection on the tracks.
  // 
  // 
  // A very similar point finder was implemented for the first time by Waltenberger 
  // in CMS. I suggest his PhD thesis (2005) as a reference for the different concepts 
  // of point finders.
  //
  // ---------------------------------------
  // Changes:
  //
  // David Shope <david.richard.shope@cern.ch> (2016-04-19)
  //
  // EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
  //                         from Trk::RecVertex   to xAOD::Vertex,
  //                         from Trk::Vertex      to Amg::Vector3D


  class CrossDistancesSeedFinder : public extends<AthAlgTool, IVertexSeedFinder>
  {
  public:
    // Standard Gaudi constructor.
    CrossDistancesSeedFinder (const std::string& t,
                              const std::string& n,
                              const IInterface*  p);

    
    virtual ~CrossDistancesSeedFinder();

    virtual StatusCode initialize() override;


    using IVertexSeedFinder::findSeed;

    /**
     *  Finds a linearization point out of a vector of tracks
     *  and returns it as an Amg::Vector3D object. If you want an 
     *  additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::Track*> & vectorTrk,
              const xAOD::Vertex * constraint=0) const override;
    

    /** 
     * Finds a linearization point out of a vector of TrackParameters
     *  and returns it as an Amg::Vector3D object. If you want an 
     * additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::TrackParameters*> & perigeeList,
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
    findMultiSeeds (const std::vector<const Trk::TrackParameters*>& perigeeList,
                    const xAOD::Vertex * constraint=0) const override;


  private:

    bool m_useweights;
    float m_trackdistcutoff;
    int m_trackdistexppower;
    float m_constraintcutoff;
    float m_constrainttemp;
    ToolHandle<IMode3dFinder> m_mode3dfinder;
    ToolHandle<ITrkDistanceFinder> m_distancefinder;
    unsigned int m_maximumTracksNoCut;
    double m_maximumDistanceCut;

  };
}
#endif
