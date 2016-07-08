/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_SlowSpatialPointFinder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETZVTOP_SLOWSPATIALPOINTFINDER_H
#define INDETZVTOP_SLOWSPATIALPOINTFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetZVTOPVxFinder/IZVTOP_SpatialPointFinder.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h"
namespace Trk
{
  class Track;
  class RecVertex;
  class TrackParticleBase;
  class IVertexLinearizedTrackFactory;
  class Vertex;
}

namespace Rec 
{
  class TrackParticle;
}

namespace InDet 
{

  /** @class ZVTOP_SlowSpatialPointFinder 


      HelperClass for Topological Vertex Finder -ZVTOP-
      Calculates the spatial point, which define the region of interest,
      the region where we later look for the maxima of the vertex probability
      function. Since at least two of the tracks (or RecVertex and Trk::Track) 
      must contribute to a maximum in the vertex probability function, 
      we calculate for each track pair the spatial point.
      The found spatial point is rejected if chi² of this point is larger as chi²_cut.
      
      @author  Tatjana Lenz <tatjana.lenz@cern.ch>
  */  

  class ZVTOP_SlowSpatialPointFinder : virtual public IZVTOP_SpatialPointFinder, public AthAlgTool
    {
    public:
      ZVTOP_SlowSpatialPointFinder(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~ZVTOP_SlowSpatialPointFinder ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      Trk::Vertex* findSpatialPoint(const Trk::Track* trk_1, const Trk::Track* trk_2);
      Trk::Vertex* findSpatialPoint(const Trk::RecVertex vtx, const Trk::Track* trk_1);
      Trk::Vertex* findSpatialPoint(const Rec::TrackParticle* trk_1, const Rec::TrackParticle* trk_2);
      Trk::Vertex* findSpatialPoint(const Trk::RecVertex vtx, const Rec::TrackParticle* trk_1);
      Trk::Vertex* findSpatialPoint(const Trk::TrackParticleBase* trk_1, const Trk::TrackParticleBase* trk_2);
      Trk::Vertex* findSpatialPoint(const Trk::RecVertex vtx, const Trk::TrackParticleBase* trk_1);
      Trk::Vertex* findSpatialPoint(const Trk::TrackParameters* perigee_1, const Trk::TrackParameters* perigee_2);
      Trk::Vertex* findSpatialPoint(const Trk::RecVertex vtx, const Trk::TrackParameters* perigee_1);

    private:
      
    	ToolHandle< Trk::IVertexLinearizedTrackFactory > m_linFactory;
      double				m_chi2;
      
    }; 
} // end of namespace

#endif 
 
