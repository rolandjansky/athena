/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexTrackDensityEstimator.h, (c) ATLAS Detector software 2018
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXTRACKDENSITYESTIMATOR_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXTRACKDENSITYESTIMATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

/**
 * @class Trk::IVertexTrackDensityEstimator
 *
 * An abstract base class for implementation of
 * weight functions for seeding. Calculates the 
 * the density of tracks at a given point along
 * the beamline given a set of tracks. 
 *
 *@author Dave Casper (dcasper@uci.edu) 
 *
 * --------------------------------------------------------
 */

namespace Trk
{
  class Track;

  static const InterfaceID IID_IVertexTrackDensityEstimator("IVertexTrackDensityEstimator", 1, 0);

  class IVertexTrackDensityEstimator : virtual public IAlgTool {

     public:
       /**
        *  Virtual destructor 
	*/
       virtual ~IVertexTrackDensityEstimator() {};

       /** 
        * AlgTool interface methods 
	*/
       static const InterfaceID& interfaceID() { return IID_IVertexTrackDensityEstimator; };

       /**
        *   Adds a list of tracks, whose impact parameters will contribute to the density function.
        */
       virtual void addTracks(const std::vector<const Trk::Track*>& vectorTrk) = 0;

       /**
	*  Adds a list of track perigee parameters, whose impact parameters will contribute to 
        *  the density function.
        */
       virtual void addTracks(const std::vector<const Trk::TrackParameters*>& perigeeList) = 0;

       /**
        *  Removes a list of tracks, which will no longer contribute to the density function.
        */
       virtual void removeTracks(const std::vector<const Trk::Track*>& vectorTrk) = 0;

       /**
        *  Removes a list of track perigee parameters, which will no longer contribute to 
        *  the density function.
        */
       virtual void removeTracks(const std::vector<const Trk::TrackParameters*>& perigeeList) = 0;

       /**
        *  Evaluate the density function at the specified coordinate along the beam-line.
        */
       virtual double trackDensity(double z) const = 0;

       /*
        *  Evaluate the density function and its first two derivatives at the specified coordinate.
        */
       virtual void trackDensity(double z, double& density, double& firstDerivative, double& secondDerivative) const = 0;

       /*
	*  Find position of global maximum for density function
	*/
       virtual double globalMaximum() const = 0;

       /**
        *  Resets the internal state of the tool, forgetting all tracks previously added.
        */
       virtual void reset() = 0;

  };
}

#endif
