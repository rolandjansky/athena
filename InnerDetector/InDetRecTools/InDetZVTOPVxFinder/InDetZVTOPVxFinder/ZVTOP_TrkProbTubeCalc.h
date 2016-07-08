/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_TrkProbTubeCalc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETZVTOP_TRKPROBTUBECALC_H
#define INDETZVTOP_TRKPROBTUBECALC_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetZVTOPVxFinder/IZVTOP_TrkProbTubeCalc.h"
#include "TrkTrack/Track.h"
#include "VxVertex/RecVertex.h"
#include"Particle/TrackParticle.h"
namespace Trk
{
  class IExtrapolator;
}
namespace InDet 
{

  /** @class ZVTOP_TrkProbTubeCalc 

      ZVTOP helper class, calculates the Gaussian probability
      tube for the track trajectory.
      
      @author  Tatjana Lenz <tatjana.lenz@cern.ch>
  */  

  class ZVTOP_TrkProbTubeCalc : virtual public IZVTOP_TrkProbTubeCalc, public AthAlgTool
    {
    public:
      ZVTOP_TrkProbTubeCalc(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~ZVTOP_TrkProbTubeCalc ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      virtual double calcProbTube(const Trk::Track& trk, Trk::Vertex& vec);
      virtual double calcProbTube(const Rec::TrackParticle& trk, Trk::Vertex& vec);
      virtual double calcProbTube(const Trk::TrackParticleBase& trk, Trk::Vertex& vec);
      virtual double calcProbTube(const Trk::Perigee* trk, Trk::Vertex& vec);
      virtual double calcProbTube(const Trk::RecVertex& vtx, Trk::Vertex& vec);
    private:
      
      /** member variables for algorithm properties: */
     ToolHandle< Trk::IExtrapolator > m_extrapolator;
      
    }; 
} // end of namespace

#endif 
