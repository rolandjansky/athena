/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_TrackDensitySeedFinder_H
#define TRKVERTEXSEEDFINDERTOOLS_TrackDensitySeedFinder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexFitterInterfaces/IVertexTrackDensityEstimator.h"

namespace Trk
{

  class Track;
  class IMode1dFinder;
  class ITrackToVertexIPEstimator;

  // @author D. Casper
  //
  // @ATLAS software
  //
  // This class implements a seed finder for the primary vertex finding 
  // which is based on the use of a density function along the beam line 
  // 
  // -------------------------------------------

  class TrackDensitySeedFinder : public AthAlgTool, virtual public IVertexSeedFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    TrackDensitySeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~TrackDensitySeedFinder();

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

    virtual int perigeesAtSeed( std::vector<const Trk::TrackParameters*> * ,
                              const std::vector<const Trk::TrackParameters*> & ) const;

    virtual int getModes1d(std::vector<float>&, std::vector<float>&, 
			   std::vector<float>&, std::vector<float>&  ) const;
    virtual void getCorrelationDistance( double &cXY, double &cZ );

  private:
    ToolHandle< IVertexTrackDensityEstimator > m_densityEstimator { this, 
	                                                            "DensityEstimator", 
	                                                            "Trk::GaussianTrackDensity",
                                                                    "Track density tool" } ;

  };
}
#endif
