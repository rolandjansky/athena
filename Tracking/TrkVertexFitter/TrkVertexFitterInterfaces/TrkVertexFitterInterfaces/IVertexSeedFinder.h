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
 * The correct choice of a starting linearization point
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
  class IMode3dInfo;

  class IVertexSeedFinder : virtual public IAlgTool
  {
  public:
    DeclareInterfaceID( IVertexSeedFinder, 1, 0 );


    /**
     *  Finds a linearization point out of a vector of tracks
     *  and returns it as an Amg::Vector3D object. If you want an 
     *  additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::Track*>& vectorTrk,
              const xAOD::Vertex * constraint=0) const = 0;


    /** 
     * Finds a linearization point out of a vector of TrackParameters
     *  and returns it as an Amg::Vector3D object. If you want an 
     * additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::TrackParameters*>& perigeeList,
              const xAOD::Vertex * constraint=0) const = 0;

    
    /** 
     * Finds a linearization point out of a vector of TrackParameters
     *  and returns it as an Amg::Vector3D object. If you want an 
     * additional constraint can be taken into account.
     * Must specify the primary vertex position.
     */
    virtual Amg::Vector3D
    findSeed(const double /*vx*/,
             const double /*vy*/,
             const std::vector<const Trk::TrackParameters*>& perigeeList,
             const xAOD::Vertex * constraint=0) const
    {
      return findSeed (perigeeList, constraint);
    }

    
    /** 
     * Finds a linearization point out of a vector of TrackParameters
     *  and returns it as an Amg::Vector3D object. If you want an 
     * additional constraint can be taken into account.
     * Must specify the primary vertex position.
     * Allows returning additional diagnostic information.
     */
    virtual Amg::Vector3D
    findSeed(const double /*vx*/,
             const double /*vy*/,
             std::unique_ptr<Trk::IMode3dInfo>& /*info*/,
             const std::vector<const Trk::TrackParameters*>& perigeeList,
             const xAOD::Vertex * constraint=0) const
    {
      return findSeed (perigeeList, constraint);
    }


    /**
     * Finds full vector of linearization points from a vector of tracks
     *  and returns it as an Amg::Vector3D object.  Intended for seed finders that produce all at once.
     *  If you want an additional constraint can be taken into account.
     */
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds (const std::vector<const Trk::Track*>& vectorTrk,
                    const xAOD::Vertex * constraint=0) const = 0;


    /**
     * Finds full vector of linearization points from a vector
     * of TrackParameters and returns it as an Amg::Vector3D object.
     * Intended for seed finders that produce all at once.
     * If you want an additional constraint can be taken into account.
     */
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,
                   const xAOD::Vertex * constraint=0) const = 0;

  };
}

#endif
