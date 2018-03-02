/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILIS_GAUSSIANTRACKDENSITY_H
#define TRKVERTEXSEEDFINDERUTILIS_GAUSSIANTRACKDENSITY_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexTrackDensityEstimator.h"

#include <unordered_map>

namespace Trk
{

  class Track;

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
    virtual double trackDensity(double z) const;

    /**
     *  Evaluate the density and its first two derivatives at the specified coordinate.
     */
    virtual void trackDensity(double z, double& density, double& firstDerivative, double& secondDerivativec) const;

    /**
     *  Resets the internal state of the tool, forgetting all tracks previously added.
     */
    virtual void reset();


  private:
    struct TrackEntry
    {
      TrackEntry(double c0, double c1, double c2);
      // Cached information for a single track
      double constant; // z-independent factor
      double c_1;      // linear coefficient in exponent
      double c_2;      // quadratic coefficient in exponent
    };

    // functor to hash key for unordered_map
    struct hash_perigee {
      size_t operator()(const Trk::Perigee& perigee) const
      {
	return 
	  std::hash<double>()(perigee.parameters()[Trk::d0]) ^
	  std::hash<double>()(perigee.parameters()[Trk::z0]) ^
	  std::hash<double>()(perigee.parameters()[Trk::phi]) ^
	  std::hash<double>()(perigee.parameters()[Trk::theta]) ^
	  std::hash<double>()(perigee.parameters()[Trk::qOverP]); 
      }
    };

    // functor to compare two unordered_map Key values for equality
    struct pred_perigee {
      bool operator()(const Trk::Perigee& left, const Trk::Perigee& right) const
      {
	return 
	  (left.parameters()[Trk::d0] == right.parameters()[Trk::d0]) &&
	  (left.parameters()[Trk::z0] == right.parameters()[Trk::z0]) &&
	  (left.parameters()[Trk::phi] == right.parameters()[Trk::phi]) &&
	  (left.parameters()[Trk::theta] == right.parameters()[Trk::theta]) &&
	  (left.parameters()[Trk::qOverP] == right.parameters()[Trk::qOverP]);
      }
    };

    std::unordered_map< Trk::Perigee, Trk::GaussianTrackDensity::TrackEntry, hash_perigee, pred_perigee> m_trackMap;


  };
}
#endif
