/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  class IVertexTrackDensityEstimator : virtual public IAlgTool
  {
  public:
    DeclareInterfaceID (IVertexTrackDensityEstimator, 1, 0);


    /**
     * @brief Helper to hold density results.
     */
    class ITrackDensity
    {
    public:
      virtual ~ITrackDensity() = default;


      /**
       *  Evaluate the density function at the specified coordinate
       *  along the beamline.
       */
      virtual double trackDensity (double z) const = 0;


      /**
       *  Evaluate the density and its first two derivatives
       *  at the specified coordinate.
       */
      virtual void trackDensity (double z,
                                 double& density,
                                 double& firstDerivative,
                                 double& secondDerivative) const = 0;
    };


    /**
     *  Virtual destructor 
     */
    virtual ~IVertexTrackDensityEstimator() = default;


    /**
     * @brief Find position of global maximum for density function.
     * @param vectorTrk List of input tracks.
     */
    virtual double
    globalMaximum (const std::vector<const Track*>& vectorTrk) const = 0;


    /**
     * @brief Find position of global maximum for density function.
     * @param vectorTrk List of input tracks.
     * @param density[out] Helper to hold density results.
     */
    virtual double
    globalMaximum (const std::vector<const Track*>& vectorTrk,
                   std::unique_ptr<ITrackDensity>& density) const = 0;


    /**
     * @brief Find position of global maximum for density function.
     * @param perigeeList List of input tracks.
     */
    virtual double
    globalMaximum (const std::vector<const TrackParameters*>& perigeeList) const = 0;

    virtual std::pair<double,double>
    globalMaximumWithWidth (const std::vector<const TrackParameters*>& perigeeList) const = 0;

    /**
     * @brief Find position of global maximum for density function.
     * @param perigeeList List of input tracks.
     * @param density[out] Helper to hold density results.
     */
    virtual double
    globalMaximum (const std::vector<const TrackParameters*>& perigeeList,
                   std::unique_ptr<ITrackDensity>& density) const = 0;
  };




} // namespace Trk


#endif
