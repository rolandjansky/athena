/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertSeedFinder.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXSEEDFINDER_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXSEEDFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/Vertex.h"

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
 *
 * --------------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-19)
 *
 * EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
 *                         from Trk::RecVertex   to xAOD::Vertex,
 *                         from Trk::Vertex      to Amg::Vector3D
 */

namespace Trk
{
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

       virtual void setPriVtxPosition( double vx, double vy ) = 0 ;

       /**
        *  Finds a linearization point out of a vector of tracks
	*  and returns it as an Amg::Vector3D object. If you want an 
	*  additional constraint can be taken into account.
        */
       virtual Amg::Vector3D findSeed(const std::vector<const Trk::Track*>& vectorTrk,const xAOD::Vertex * constraint=0) = 0;


       /** 
        * Finds a linearization point out of a vector of TrackParameters
        *  and returns it as an Amg::Vector3D object. If you want an 
	* additional constraint can be taken into account.
	*/
       virtual Amg::Vector3D findSeed(const std::vector<const Trk::TrackParameters*>& perigeeList,const xAOD::Vertex * constraint=0) = 0;

       /**
        * Finds full vector of linearization points from a vector of tracks
        *  and returns it as an Amg::Vector3D object.  Intended for seed finders that produce all at once.
	*  If you want an additional constraint can be taken into account.
        */
       virtual std::vector<Amg::Vector3D> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,const xAOD::Vertex * constraint=0) = 0;

       /**
        * Finds full vector of linearization points from a vector of TrackParameters
        *  and returns it as an Amg::Vector3D object.  Intended for seed finders that produce all at once.
	*  If you want an additional constraint can be taken into account.
        */
       virtual std::vector<Amg::Vector3D> findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,const xAOD::Vertex * constraint=0) = 0;

      /** 
        * Return the perigees of tracks that are directly used in the crossing points
       */

       virtual int perigeesAtSeed( std::vector<const Trk::TrackParameters*> * ,
                              const std::vector<const Trk::TrackParameters*> & ) const = 0 ;

       virtual int getModes1d(std::vector<float>&, std::vector<float>&, 
                              std::vector<float>&, std::vector<float>&  ) const = 0 ;
       virtual void getCorrelationDistance( double &cXY, double &cZ ) = 0 ;

  };
}

#endif
