/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_DUMMYSEEDFINDER_H
#define TRKVERTEXSEEDFINDERTOOLS_DUMMYSEEDFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"

namespace Trk
{
  // @author N. Giacinto Piacquadio (Albert-Ludwigs-Universitaet Freiburg - Germany)
  //
  // @ATLAS software
  //
  // This class implements a dummy seed finder
  //
  // -----------------------------------------
  // Changes:
  //
  // David Shope <david.richard.shope@cern.ch> (2016-04-19)
  //
  // EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
  //                         from Trk::RecVertex   to xAOD::Vertex,
  //                         from Trk::Vertex      to Amg::Vector3D

  class DummySeedFinder final: public extends<AthAlgTool, IVertexSeedFinder>
  {
  public:
    // Standard Athena constructor.
    DummySeedFinder (const std::string& t,
                     const std::string& n,
                     const IInterface*  p);
    

    virtual ~DummySeedFinder();


    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;


    using IVertexSeedFinder::findSeed;

    // Interface for Tracks with starting seed/linearization point
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::Track*> & vectorTrk,
              const xAOD::Vertex * constraint=0) const override final;
    

    /** Interface for MeasuredPerigee with starting point */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::TrackParameters*> & perigeeList,
              const xAOD::Vertex * constraint=0) const override final;


    // Interface for finding vector of seeds from tracks
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds (const std::vector<const Trk::Track*>& vectorTrk,
                    const xAOD::Vertex * constraint=0) const override final;


    // Interface for finding vector of seeds from track parameters
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,
                   const xAOD::Vertex * constraint=0) const override final;

  };
}
#endif
