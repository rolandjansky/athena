/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexFitter.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXFITTER_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXFITTER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include <vector>

//xAOD includes
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

 /**
   * @class Trk::IVertexFitter
   *
   * Interface class for vertex fitting AlgTools,
   * for more detailed information, take a look at the header file of the actual
   * implementation file. In general, the vertex fitters is an AlgTool,
   * taking a list of tracks or their parameters as an argument and returning a 
   * vertex candidate.
   * 
   * <br>
   * Modification for the use of VertexSeedFinders (algorithms
   * providing initial point of linearization); February 2006
   *
   * @author Kirill.Prokofiev@cern.ch, Andreas.Wildauer@cern.ch, Giacinto.Piacquadio@physik.uni-freiburg.de
   */



namespace Trk
{
  class Vertex;
  class RecVertex;
  class VxCandidate;
  class Track;
  class TrackParticleBase;
  
  static const InterfaceID IID_IVertexFitter("IVertexFitter", 1, 0);

  class IVertexFitter : virtual public IAlgTool {

     public:
       /** 
        * Virtual destructor 
	*/
       virtual ~IVertexFitter(){};

       /** 
        * AlgTool interface methods 
	*/
       static const InterfaceID& interfaceID() { return IID_IVertexFitter; };


       /** 
        *Interface for Track with starting point 
        */
       virtual VxCandidate * fit(const std::vector<const Trk::Track*>& vectorTrk,
                                 const Vertex& startingPoint) = 0;

       /** 
        * Interface for Track with vertex constraint 
        * the position of the constraint is ALWAYS the starting point 
        */
       virtual VxCandidate * fit(const std::vector<const Trk::Track*>& vectorTrk,
                                 const RecVertex& constraint) = 0;

       /** 
        *Interface for TrackParticleBase with starting point 
        */
       virtual VxCandidate * fit(const std::vector<const Trk::TrackParticleBase*>& vectorTrk,
                                 const Vertex& startingPoint) = 0;

       /** 
        * Interface for TrackParticleBase with vertex constraint 
        * the position of the constraint is ALWAYS the starting point 
        */
       virtual VxCandidate * fit(const std::vector<const Trk::TrackParticleBase*>& vectorTrk,
                                 const RecVertex& constraint) = 0;


       /** 
        *Interface for xAOD::TrackParticle with starting point 
        */
       virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                                 const Vertex& startingPoint) = 0;

       /** 
        * Interface for xAOD::TrackParticle with vertex constraint 
        * the position of the constraint is ALWAYS the starting point 
        */
       virtual xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                                 const RecVertex& constraint) = 0;

       /** 
        * Interface for TrackParameters with starting point 
	*/
       virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*>& perigeeList,
                                 const Vertex& startingPoint) = 0;

       /** 
        * Interface for TrackParameters with vertex constraint 
        * the position of the constraint is ALWAYS the starting point 
	*/
       virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*>& perigeeList,
                                 const RecVertex& constraint) = 0;

       /**
        * Fit method using the VertexSeedFinder to estimate initial 
	* position of the vertex and taking it as a first linearization point
	* (in iterative fitters).
        */
       virtual VxCandidate * fit(const std::vector<const Trk::TrackParameters*>& perigeeList) = 0;
       
       /**
        * Fit method using the VertexSeedFinder to estimate initial 
	* position of the vertex and taking it as a first linearization point
	* (in iterative fitters).
        */
       virtual VxCandidate * fit(const std::vector<const Trk::Track*>& vectorTrk) = 0;
  };
}

#endif

