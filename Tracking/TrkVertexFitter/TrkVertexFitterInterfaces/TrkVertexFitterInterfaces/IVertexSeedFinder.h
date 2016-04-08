/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertSeedFinder.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXSEEDFINDER_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXSEEDFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

/**
 * @class Trk::IVertexSeedFinder
 *
 * An abstract base class for implementation of
 * Linearization point finders. Calculates the 
 * the starting point of the vertex fit given
 * with a set of tracks. 
 *
 * The correct choise of a starting linbearization point
 * does influence the precision of the adaptive fits and
 * may influence the convergence time of the Kalman filter.
 * See concrete implementations for more details.
 *
 *@author Giacinto.Piacquadio@physik.uni-freiburg.de 
 */


// AW how come a forward declare of Vertex is enough in this header even though Vertex is not used as pointer nor reference but as concrete type?
namespace Trk
{
  class Vertex;
  class RecVertex;
  class Track;

  static const InterfaceID IID_IVertexSeedFinder("IVertexSeedFinder", 1, 0);

  class IVertexSeedFinder : virtual public IAlgTool {

     public:
       /**
        * Virtual destructor 
	*/
       virtual ~IVertexSeedFinder() {};

       /** 
        * AlgTool interface methods 
	*/
       static const InterfaceID& interfaceID() { return IID_IVertexSeedFinder; };

       /**
        *  Finds a linearization point out of a vector of tracks
	*  and returns it as a Trk::Vertex object. If you want an 
	*  additional constraint can be taken into account.
        */
       virtual Vertex findSeed(const std::vector<const Trk::Track*>& vectorTrk,const RecVertex * constraint=0) = 0;


       /** 
        * Finds a linearization point out of a vector of TrackParameters
        *  and returns it as a Trk::Vertex object. If you want an 
	* additional constraint can be taken into account.
	*/
       virtual Vertex findSeed(const std::vector<const Trk::TrackParameters*>& perigeeList,const RecVertex * constraint=0) = 0;

       /**
        * Finds full vector of linearization points from a vector of tracks
        *  and returns it as a Trk::Vertex object.  Intended for seed finders that produce all at once.
	*  If you want an additional constraint can be taken into account.
        */
       virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,const RecVertex * constraint=0) = 0;

       /**
        * Finds full vector of linearization points from a vector of TrackParameters
        *  and returns it as a Trk::Vertex object.  Intended for seed finders that produce all at once.
	*  If you want an additional constraint can be taken into account.
        */
       virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,const RecVertex * constraint=0) = 0;



  };
}

#endif
