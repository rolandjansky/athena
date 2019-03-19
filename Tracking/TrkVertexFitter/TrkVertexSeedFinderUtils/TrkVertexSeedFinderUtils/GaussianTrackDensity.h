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
    virtual void addTracks(const std::vector<const Track*>& vectorTrk);

    /**
     *  Adds a list of track perigee parameters, whose impact parameters will contribute to 
     *  the density function.
     */
    virtual void addTracks(const std::vector<const TrackParameters*>& perigeeList);

    /**
     *  Removes a list of tracks, which will no longer contribute to the density function.
     */
    virtual void removeTracks(const std::vector<const Track*>& vectorTrk);

    /**
     *  Removes a list of track perigee parameters, which will no longer contribute to 
     *  the density function.
     */
    virtual void removeTracks(const std::vector<const TrackParameters*>& perigeeList);

    /**
     *  Evaluate the density function at the specified coordinate along the beam-line.
     */
    virtual double trackDensity(double z) const;

    /**
     *  Evaluate the density and its first two derivatives at the specified coordinate.
     */
    virtual void trackDensity(double z, double& density, double& firstDerivative, double& secondDerivativec) const;

    /**
     *  Find location of global maximum for density function
     */
    virtual double globalMaximum() const;

    /**
     *  Resets the internal state of the tool, forgetting all tracks previously added.
     */
    virtual void reset();

    // functor to compare two Perigee values
    struct pred_perigee {
      bool operator()(const Perigee& left, const Perigee& right) const
      {
	return left.parameters()[Trk::z0] < right.parameters()[Trk::z0];
      }
    };

    struct TrackEntry
    {
      TrackEntry() { c_0 = 0; c_1 = 0; c_2 = 0; lowerBound = 0; upperBound = 0; }
      TrackEntry(double c0, double c1, double c2, double zMin, double zMax);
      TrackEntry(double zProbe);
      // Cached information for a single track
      double c_0;      // z-independent term in exponent
      double c_1;      // linear coefficient in exponent
      double c_2;      // quadratic coefficient in exponent
      double lowerBound;
      double upperBound;
    };

    // functor to compare two TrackEntry values based on their lower limits (low to high)
    struct pred_entry_by_min {
      bool operator()(const TrackEntry& left, const TrackEntry& right) const
      {
	return left.lowerBound < right.lowerBound;
      }
    };

    // functor to compare two TrackEntry values based on their upper limits (low to high)
    struct pred_entry_by_max {
      bool operator()(const TrackEntry& left, const TrackEntry& right) const
      {
	return left.upperBound < right.upperBound;
      }
    };

    typedef std::map< Perigee, 
                      GaussianTrackDensity::TrackEntry, 
                      GaussianTrackDensity::pred_perigee > trackMap;

    typedef std::map< Perigee, 
                      GaussianTrackDensity::TrackEntry, 
                      GaussianTrackDensity::pred_perigee >::const_iterator trackMapIterator;

    typedef std::map< GaussianTrackDensity::TrackEntry,
                      Perigee,
                      GaussianTrackDensity::pred_entry_by_max > lowerMap;

    typedef std::map< GaussianTrackDensity::TrackEntry,
                      Perigee,
                      GaussianTrackDensity::pred_entry_by_max >::const_iterator lowerMapIterator;

    typedef std::map< GaussianTrackDensity::TrackEntry,
                      Perigee,
                      GaussianTrackDensity::pred_entry_by_min > upperMap;

    typedef std::map< GaussianTrackDensity::TrackEntry,
                      Perigee,
                      GaussianTrackDensity::pred_entry_by_min >::const_iterator upperMapIterator;

  private:
    inline void updateMaximum(double trialZ, double trialValue, double& maxZ, double& maxValue) const
    { if (trialValue > maxValue) { maxZ = trialZ; maxValue = trialValue; } }

      
    inline double stepSize(double y, double dy, double ddy) const
    { return ( m_gaussStep ? (y * dy)/(dy * dy - y * ddy) : -dy/ddy ); }


    //  Cache for track information
    trackMap m_trackMap;
    lowerMap m_lowerMap;
    upperMap m_upperMap;

    double m_maxRange;

    //  Cuts set by configurable properties
    
    //  Maximum allowed d0 significance to use (in sigma)
    Gaudi::Property<double> m_d0MaxSignificance { this, 
                                                  "MaxD0Significance", 
	                                          3.5, 
                                                  "Maximum radial impact parameter significance to use track" };

    // Tracks within this many sigma(z) are added to weight; increasing cut trades CPU efficiency for improved smoothness in tails
    Gaudi::Property<double> m_z0MaxSignificance { this,
	                                          "MaxZ0Significance",
	                                          12.0,
	                                          "Maximum longitudinal impact parameter significance to include track in weight"};

    // Assumed shape of density function near peak; Gaussian (true) or parabolic (false)
    Gaudi::Property<bool> m_gaussStep           { this,
	                                          "GaussianStep",
	                                          true,
	                                          "Peak search: True means assume Gaussian behavior, False means Newton/parabolic" };
                            
  };
}
#endif
