/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexSeedFinderUtils/GaussianTrackDensity.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkTrack/Track.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace Trk
{
  /**
   * @brief Find position of global maximum for density function.
   * @param vectorTrk List of input tracks.
   */
  double GaussianTrackDensity::globalMaximum (const std::vector<const Track*>& vectorTrk) const
  {
    std::vector<const TrackParameters*> perigeeList;
    for (const Track* itrk : vectorTrk)
    {
      perigeeList.push_back(itrk->perigeeParameters());
    }

    TrackDensity d (m_gaussStep);
    return globalMaximumImpl (perigeeList, d);
  }


  /**
   * @brief Find position of global maximum for density function.
   * @param vectorTrk List of input tracks.
   * @param density[out] Helper to hold density results.
   */
  double GaussianTrackDensity::globalMaximum (const std::vector<const Track*>& vectorTrk,
                                              std::unique_ptr<ITrackDensity>& density) const
  {
    std::vector<const TrackParameters*> perigeeList;
    for (const Track* itrk : vectorTrk)
    {
      perigeeList.push_back(itrk->perigeeParameters());
    }
    return globalMaximum (perigeeList, density);
  }


  /**
   * @brief Find position of global maximum for density function.
   * @param vectorTrk List of input tracks.
   */
  double
  GaussianTrackDensity::globalMaximum (const std::vector<const TrackParameters*>& perigeeList) const
  {
    TrackDensity d (m_gaussStep);
    return globalMaximumImpl (perigeeList, d);
  }


  /**
   * @brief Find position of global maximum for density function.
   * @param perigeeList List of input tracks.
   * @param density[out] Helper to hold density results.
   */
  double
  GaussianTrackDensity::globalMaximum (const std::vector<const TrackParameters*>& perigeeList,
                                       std::unique_ptr<ITrackDensity>& density) const
  {
    auto d = std::make_unique<TrackDensity> (m_gaussStep);
    TrackDensity* dp = d.get();
    density = std::move(d);
    return globalMaximumImpl (perigeeList, *dp);
  }

  std::pair<double,double> GaussianTrackDensity::globalMaximumWithWidth (const std::vector<const TrackParameters*>& perigeeList/*,
                                                  std::unique_ptr<ITrackDensity>& density*/) const
  {
    TrackDensity d (m_gaussStep);
    return globalMaximumWithWidthImpl (perigeeList, d);
  }

  /**
   * @brief Find position of global maximum for density function.
   * @param pergigeeList List of input tracks.
   * @param density Helper density object.
   */
  double GaussianTrackDensity::globalMaximumImpl (const std::vector<const TrackParameters*>& perigeeList,
                                                  TrackDensity& density) const
  {
    addTracks (perigeeList, density);
    return density.globalMaximum (msg());
  }

  std::pair<double,double> GaussianTrackDensity::globalMaximumWithWidthImpl (const std::vector<const TrackParameters*>& perigeeList,
                                                  TrackDensity& density) const
  {
    addTracks (perigeeList, density);
    return density.globalMaximumWithWidth (msg());
  }


  /**
   * @brief Add a set of tracks to a density object.
   * @param perigeeList Set of track parameters to add.
   * @param density Density object to which to add.
   */
  void GaussianTrackDensity::addTracks(const std::vector<const TrackParameters*>& perigeeList,
                                       TrackDensity& density) const
  {
    const double d0SignificanceCut = m_d0MaxSignificance * m_d0MaxSignificance;
    const double z0SignificanceCut = m_z0MaxSignificance * m_z0MaxSignificance;

    for (const TrackParameters* iparam : perigeeList) {
      if (iparam && iparam->surfaceType() == Trk::Surface::Perigee) {
        density.addTrack(*(static_cast<const Perigee*>(iparam)),
                         d0SignificanceCut,
                         z0SignificanceCut);
      }
    }
  }


  //***********************************************************************


  GaussianTrackDensity::TrackEntry::TrackEntry(double c0, double c1, double c2,
                                               double zMin, double zMax)
    : c_0(c0), c_1(c1), c_2(c2), lowerBound(zMin), upperBound(zMax)
  { }

  // Dummy constructor for binary search
  GaussianTrackDensity::TrackEntry::TrackEntry(double z)
    : c_0(0), c_1(0), c_2(0), lowerBound(z), upperBound(z)
  { }


  //***********************************************************************


  /**
   *  Evaluate the density function at the specified coordinate
   *  along the beamline.
   */
  double GaussianTrackDensity::TrackDensity::trackDensity (double z) const
  {
    double firstDeriv, secondDeriv = 0;  // unused in this case
    double density = 0;
    // use the existing trackDensity method to avoid duplication of logic
    trackDensity(z,density,firstDeriv,secondDeriv);
    return density;
  }


  /**
   *  Evaluate the density and its first two derivatives
   *  at the specified coordinate.
   */
  void
  GaussianTrackDensity::TrackDensity::trackDensity (double z,
                                                    double& density,
                                                    double& firstDerivative,
                                                    double& secondDerivative) const
  {
    TrackDensityEval densityResult(z);
    for (const auto & trackAndPerigeePair : m_lowerMap){
      densityResult.addTrack(trackAndPerigeePair.first);
    }
    density = densityResult.density();
    firstDerivative = densityResult.firstDerivative();
    secondDerivative = densityResult.secondDerivative();
  }

  std::pair<double,double>
  GaussianTrackDensity::TrackDensity::globalMaximumWithWidth (MsgStream& msg) const
  {
    // strategy:
    // the global maximum must be somewhere near a track...
    // since we can calculate the first and second derivatives, at each point we can determine
    // a) whether the function is curved up (minimum) or down (maximum)
    // b) the distance to nearest maximum, assuming either Newton (parabolic) or Gaussian local behavior
    //
    // For each track where the second derivative is negative, find step to nearest maximum
    // Take that step, and then do one final refinement
    // The largest density encountered in this procedure (after checking all tracks) is considered the maximum
    //
    double maximumPosition = 0.0;
    double maximumDensity = 0.0;
    double maxCurvature = 0. ;
    const std::pair<double,double> invalidResult(0.,std::numeric_limits<double>::quiet_NaN());
    if (m_trackMap.empty()) return invalidResult;
    for (const auto& entry : m_trackMap)
    {
      double trialZ = entry.first.parameters()[Trk::z0];
      double density   = 0.0;
      double slope     = 0.0;
      double curvature = 0.0;
      trackDensity( trialZ, density, slope, curvature );
      if ( curvature >= 0.0 || density <= 0.0 ) continue;
      updateMaximum( trialZ, density, curvature, maximumPosition, maximumDensity, maxCurvature);
      trialZ += stepSize( density, slope, curvature );
      trackDensity( trialZ, density, slope, curvature );
      if ( curvature >= 0.0 || density <= 0.0 ) continue;
      updateMaximum( trialZ, density, curvature, maximumPosition, maximumDensity, maxCurvature);
      trialZ += stepSize( density, slope, curvature );
      trackDensity( trialZ, density, slope, curvature );
      if ( curvature >= 0.0 || density <= 0.0) continue;
      updateMaximum( trialZ, density, curvature, maximumPosition, maximumDensity, maxCurvature);
    }
    if ( maximumDensity <= 0 &&  msg.level() <= MSG::DEBUG) {
      msg << MSG::DEBUG << "Global maximum at density of 0; track map contains "
          <<  m_trackMap.size() << " tracks" << endmsg;
    }
    if (maxCurvature == 0.) return invalidResult;
    return {maximumPosition,std::sqrt(-maximumDensity/maxCurvature)};
  }

  /**
   * @brief Return position of global maximum for density function.
   * @param msg Message stream.
   */
  double
  GaussianTrackDensity::TrackDensity::globalMaximum (MsgStream& msg) const
  {
    return GaussianTrackDensity::TrackDensity::globalMaximumWithWidth(msg).first;
  }


  /**
   * @brief Add a track to the set being considered.
   * @param itrk Track parameters.
   * @param d0SignificanceCut Significance cut on d0.
   * @param z0SignificanceCut Significance cut on z0.
   */
  void GaussianTrackDensity::TrackDensity::addTrack (const Perigee& itrk,
                                                     const double d0SignificanceCut,
                                                     const double z0SignificanceCut)
  {
    if (m_trackMap.count(itrk) != 0) return;
    const double d0 = itrk.parameters()[Trk::d0];
    const double z0 = itrk.parameters()[Trk::z0];
    const AmgSymMatrix(5) & perigeeCov = *(itrk.covariance());
    const double cov_dd = perigeeCov(Trk::d0, Trk::d0);
    if ( cov_dd <= 0 ) return;
    if ( d0*d0/cov_dd > d0SignificanceCut ) return;
    const double cov_zz = perigeeCov(Trk::z0, Trk::z0);
    if (cov_zz <= 0 ) return;
    const double cov_dz = perigeeCov(Trk::d0, Trk::z0);
    const double covDeterminant = cov_dd*cov_zz - cov_dz*cov_dz;
    if ( covDeterminant <= 0 ) return;
    double constantTerm = -(d0*d0*cov_zz + z0*z0*cov_dd + 2*d0*z0*cov_dz) / (2*covDeterminant);
    const double linearTerm = (d0*cov_dz + z0*cov_dd) / covDeterminant ; // minus signs and factors of 2 cancel...
    const double quadraticTerm = -cov_dd / (2*covDeterminant);
    double discriminant = linearTerm*linearTerm - 4*quadraticTerm*(constantTerm + 2*z0SignificanceCut);
    if (discriminant < 0) return;
    discriminant = std::sqrt(discriminant);
    const double zMax = (-linearTerm - discriminant)/(2*quadraticTerm);
    const double zMin = (-linearTerm + discriminant)/(2*quadraticTerm);
    m_maxRange = std::max(m_maxRange, std::max(zMax-z0, z0-zMin));
    constantTerm -= std::log(2*M_PI*std::sqrt(covDeterminant));
    m_trackMap.emplace(std::piecewise_construct,
                       std::forward_as_tuple(itrk),
                       std::forward_as_tuple(constantTerm, linearTerm, quadraticTerm, zMin, zMax));
    m_lowerMap.emplace(std::piecewise_construct,
                       std::forward_as_tuple(constantTerm, linearTerm, quadraticTerm, zMin, zMax),
                       std::forward_as_tuple(itrk));
    m_upperMap.emplace(std::piecewise_construct,
                       std::forward_as_tuple(constantTerm, linearTerm, quadraticTerm, zMin, zMax),
                       std::forward_as_tuple(itrk));
  }


  void GaussianTrackDensity::TrackDensityEval::addTrack (const TrackEntry & entry){
    if (entry.lowerBound < m_z && entry.upperBound > m_z) {
      double delta = std::exp(entry.c_0+m_z*(entry.c_1 + m_z*entry.c_2));
      double qPrime = entry.c_1 + 2*m_z*entry.c_2;
      double deltaPrime = delta * qPrime;
      m_density += delta;
      m_firstDerivative += deltaPrime;
      m_secondDerivative += 2*entry.c_2*delta + qPrime*deltaPrime;
    }
  }

}
