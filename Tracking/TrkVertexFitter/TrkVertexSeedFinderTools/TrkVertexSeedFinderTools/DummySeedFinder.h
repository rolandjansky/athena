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

  class DummySeedFinder : public AthAlgTool, virtual public IVertexSeedFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    DummySeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~DummySeedFinder();

    // Interface for Tracks with starting seed/linearization point
    virtual Amg::Vector3D findSeed(const std::vector<const Trk::Track*> & vectorTrk,const xAOD::Vertex * constraint=0);
    
    /** Interface for MeasuredPerigee with starting point */
    virtual Amg::Vector3D findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const xAOD::Vertex * constraint=0);

    // Interface for finding vector of seeds from tracks
    virtual std::vector<Amg::Vector3D> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,const xAOD::Vertex * constraint=0);

    // Interface for finding vector of seeds from track parameters
    virtual std::vector<Amg::Vector3D> findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,const xAOD::Vertex * constraint=0);

    //The below four functions are dummy functions so that this compiles. The functions are needed in the interface IMode3dFinder.h for Mode3dFromFsmw1dFinder (the seed finder for the Inclusive Secondary Vertex Finder)

    virtual void setPriVtxPosition( double vx, double vy );
    
    virtual int perigeesAtSeed( std::vector<const Trk::TrackParameters*> * a,
                              const std::vector<const Trk::TrackParameters*> & b) const;

    virtual int getModes1d(std::vector<float>& a, std::vector<float>& b, 
			   std::vector<float>& c, std::vector<float>& d) const;
    virtual void getCorrelationDistance( double &cXY, double &cZ );

  };
}
#endif
