/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILIS_GAUSSIANTRACKDENSITY_H
#define TRKVERTEXSEEDFINDERUTILIS_GAUSSIANTRACKDENSITY_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexTrackDensityEstimator.h"

#include <map>

namespace Trk
{

  class Track;
  class GaussianTrackDensity;

  /**
   @class GaussianTrackDensity

   Implementation of IVertexTrackDensityEstimator modeling reconstructed tracks as
   two-dimensional Gaussian distributions in (d0, z0) space and sampling
   the aggregate density distribution at user-requested points along the beam axis.

   @author Dave Casper <dcasper@uci.edu>

   ------------------------------------
   Changes:
  */

  class GaussianTrackDensity : public AthAlgTool, virtual public IVertexTrackDensityEstimator
  {

  public:
    StatusCode initialize();
    StatusCode finalize();

    GaussianTrackDensity(const std::string& t, const std::string& n, const IInterface* p);

    virtual ~GaussianTrackDensity() {}

    /**
     *   Adds a list of tracks, whose impact parameters will contribute to the density function.
     */
    virtual void addTracks(const std::vector<const Trk::Track*>& vectorTrk);

    /**
     *  Adds a list of track perigee parameters, whose impact parameters will contribute to 
     *  the density function.
     */
    virtual void addTracks(const std::vector<const Trk::TrackParameters*>& perigeeList);

    /**
     *  Removes a list of tracks, which will no longer contribute to the density function.
     */
    virtual void removeTracks(const std::vector<const Trk::Track*>& vectorTrk);

    /**
     *  Removes a list of track perigee parameters, which will no longer contribute to 
     *  the density function.
     */
    virtual void removeTracks(const std::vector<const Trk::TrackParameters*>& perigeeList);

    /**
     *  Evaluate the density function at the specified coordinate along the beam-line.
     */
    virtual double trackDensity(double z);

    /**
     *  Evaluate the density and its first two derivatives at the specified coordinate.
     */
    virtual void trackDensity(double z, double& density, double& firstDerivative, double& secondDerivativec);

    /**
     *  Resets the internal state of the tool, forgetting all tracks previously added.
     */
    virtual void reset();

    // functor to compare two unordered_map Key values for equality
    struct pred_perigee {
      bool operator()(const Trk::Perigee& left, const Trk::Perigee& right) const
      {
	return left.parameters()[Trk::z0] < right.parameters()[Trk::z0];
      }
    };

    struct TrackEntry
    {
      TrackEntry(double c0, double c1, double c2, double covz);
      // Cached information for a single track
      double constant; // z-independent factor
      double c_1;      // linear coefficient in exponent
      double c_2;      // quadratic coefficient in exponent
      double cov_z;    // z0 variance from track error matrix
      std::map< Perigee, TrackEntry, pred_perigee >::const_iterator start;  
      // will point to the left-most TrackEntry close enough to affect the weight at this one
      std::map< Perigee, TrackEntry, pred_perigee >::const_iterator finish; 
      // will point to the right-most TrackEntry close enough to affect the weight at this one
    };

    typedef std::map< Perigee, 
                      GaussianTrackDensity::TrackEntry, 
                      GaussianTrackDensity::pred_perigee > trackMap;
    typedef std::map< Perigee, 
                      GaussianTrackDensity::TrackEntry, 
                      GaussianTrackDensity::pred_perigee >::const_iterator trackMapIterator;

  private:

    trackMapIterator findStart(double z);

    trackMapIterator findFinish(double z);


    //  Cache for track information
    //std::unordered_map< Trk::Perigee, Trk::GaussianTrackDensity::TrackEntry, hash_perigee, pred_perigee> m_trackMap;
    trackMap m_trackMap;

    //  Indicates whether track data needs to be refreshed before evaluation
    bool m_dirty; 

    void prepareTracks();

    //  Cuts set by configurable properties
    
    //  Maximum allowed d0 significance to use (in sigma)
    Gaudi::Property<double> m_d0MaxSignificance { this, 
                                                  "MaxD0Significance", 
	                                          3.5, 
                                                  "Maximum radial impact parameter significance to use track" };
    Gaudi::Property<double> m_z0MaxSignificance { this,
	                                          "MaxZ0Significance",
	                                          5.0,
	                                          "Maximum longitudinal impact parameter significance to include track in weight"};

  };
}
#endif
