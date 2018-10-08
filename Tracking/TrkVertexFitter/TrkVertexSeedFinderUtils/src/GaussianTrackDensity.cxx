/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexSeedFinderUtils/GaussianTrackDensity.h"

#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <algorithm>

namespace Trk
{
  GaussianTrackDensity::GaussianTrackDensity(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t, n, p), m_maxRange(0.0)
  {
    declareInterface<IVertexTrackDensityEstimator>(this);
    declareProperty( "MaxD0Significance", m_d0MaxSignificance );
    declareProperty( "MaxZ0Significance", m_z0MaxSignificance );
    declareProperty( "GaussianStep", m_gaussStep );
  }

  StatusCode GaussianTrackDensity::initialize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode GaussianTrackDensity::finalize()
  {
    return StatusCode::SUCCESS;
  }

  void GaussianTrackDensity::trackDensity(double z, double& density, double& firstDerivative, double& secondDerivative) const
  {
    density = 0.0;
    firstDerivative = 0.0;
    secondDerivative = 0.0;
    TrackEntry target(z);
    trackMap overlaps;
    lowerMapIterator left = m_lowerMap.lower_bound(target);  // first track whose UPPER bound is not less than z
    if (left == m_lowerMap.end()) return;                    // z is to the right of every track's range
    upperMapIterator right = m_upperMap.upper_bound(target); // first track whose LOWER bound is greater than z
    if (right == m_upperMap.begin()) return;                 // z is to the left of every track's range
    for (auto itrk = left; itrk != m_lowerMap.end(); itrk++)
    {
      if ( itrk->first.upperBound > z + m_maxRange ) break;
      if ( z >= itrk->first.lowerBound && z <= itrk->first.upperBound ) overlaps[itrk->second] = itrk->first;
    }
    for (auto itrk = right; itrk-- != m_upperMap.begin(); )
    {
      if ( itrk->first.lowerBound < z - m_maxRange ) break;
      if (z >= itrk->first.lowerBound && z <= itrk->first.upperBound ) overlaps[itrk->second] = itrk->first;
    }
    for (const auto& entry : overlaps)
    {
      if (entry.second.lowerBound > z || entry.second.upperBound < z) continue;
      double delta = exp(entry.second.c_0+z*(entry.second.c_1 + z*entry.second.c_2));
      density += delta;
      double qPrime = entry.second.c_1 + 2*z*entry.second.c_2;
      double deltaPrime = delta * qPrime;
      firstDerivative += deltaPrime;
      secondDerivative += 2*entry.second.c_2*delta + qPrime*deltaPrime;
    }
  }

  double GaussianTrackDensity::trackDensity(double z) const
  {
    ATH_MSG_VERBOSE("Inside trackDensity function; z=" << z);
    double sum = 0.0;
    TrackEntry target(z);
    trackMap overlaps;
    lowerMapIterator left = m_lowerMap.lower_bound(target);  // first track whose UPPER bound is not less than z
    if (left == m_lowerMap.end()) return sum;                // z is to the right of every track's range
    upperMapIterator right = m_upperMap.upper_bound(target); // first track whose LOWER bound is greater than z
    if (right == m_upperMap.begin()) return sum;             // z is to the left of every track's range
    for (auto itrk = left; itrk != m_lowerMap.end(); itrk++)
    {
      if ( itrk->first.upperBound > z + m_maxRange ) break;
      if ( z >= itrk->first.lowerBound && z <= itrk->first.upperBound ) overlaps[itrk->second] = itrk->first;
    }
    for (auto itrk = right; itrk-- != m_upperMap.begin(); )
    {
      if ( itrk->first.lowerBound < z - m_maxRange ) break;
      if (z >= itrk->first.lowerBound && z <= itrk->first.upperBound ) overlaps[itrk->second] = itrk->first;
    }
    for (const auto& entry : overlaps)
    {
      sum += exp(entry.second.c_0+z*(entry.second.c_1 + z*entry.second.c_2));
    }
    return sum;
  }

  double GaussianTrackDensity::globalMaximum() const
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
    
    for (const auto& entry : m_trackMap)
    {
      double trialZ = entry.first.parameters()[Trk::z0];
      double density   = 0.0;
      double slope     = 0.0;
      double curvature = 0.0;
      trackDensity( trialZ, density, slope, curvature );
      if ( curvature >= 0.0 || density <= 0.0 ) continue; 
      updateMaximum( trialZ, density, maximumPosition, maximumDensity );
      trialZ += stepSize( density, slope, curvature );
      trackDensity( trialZ, density, slope, curvature );
      if ( curvature >= 0.0 || density <= 0.0 ) continue;
      updateMaximum( trialZ, density, maximumPosition, maximumDensity );
      trialZ += stepSize( density, slope, curvature );
      trackDensity( trialZ, density, slope, curvature );
      if ( curvature >= 0.0 || density <= 0.0) continue;
      updateMaximum( trialZ, density, maximumPosition, maximumDensity );
    }
    if ( maximumDensity <= 0 ) ATH_MSG_DEBUG("Global maximum at density of 0; track map contains " << 
					       m_trackMap.size() << " tracks");
    return maximumPosition;
  }

  void GaussianTrackDensity::addTracks(const std::vector<const Track*>& vectorTrk)
  {
    std::vector<const TrackParameters*> perigeeList;
    for (auto itrk : vectorTrk)
    {
      perigeeList.push_back(itrk->perigeeParameters());
    }

    addTracks(perigeeList);
  }

  void GaussianTrackDensity::removeTracks(const std::vector<const Track*>& vectorTrk)
  {
    std::vector<const TrackParameters*> perigeeList;
    for (auto itrk : vectorTrk)
    {
      perigeeList.push_back(itrk->perigeeParameters());
    }

    removeTracks(perigeeList);
  }

  void GaussianTrackDensity::addTracks(const std::vector<const TrackParameters*>& perigeeList)
  {
    double d0SignificanceCut = m_d0MaxSignificance * m_d0MaxSignificance;
    double z0SignificanceCut = m_z0MaxSignificance * m_z0MaxSignificance;
    for (auto iparam : perigeeList)
    {
      const Perigee* itrk = dynamic_cast<const Perigee*>(iparam);
      if (itrk != nullptr)
      {
	if (m_trackMap.count(*itrk) != 0) continue;
	double d0 = itrk->parameters()[Trk::d0];
	double z0 = itrk->parameters()[Trk::z0];
	const AmgSymMatrix(5) & perigeeCov = *(itrk->covariance());
	double cov_dd = perigeeCov(Trk::d0, Trk::d0);
	if ( cov_dd <= 0 ) continue;
	if ( d0*d0/cov_dd > d0SignificanceCut ) continue;
	double cov_zz = perigeeCov(Trk::z0, Trk::z0);
	if (cov_zz <= 0 ) continue;
	double cov_dz = perigeeCov(Trk::d0, Trk::z0);
	double covDeterminant = cov_dd*cov_zz - cov_dz*cov_dz;
	if ( covDeterminant <= 0 ) continue;
	double constantTerm = -(d0*d0*cov_zz + z0*z0*cov_dd + 2*d0*z0*cov_dz) / (2*covDeterminant);
	double linearTerm = (d0*cov_dz + z0*cov_dd) / covDeterminant ; // minus signs and factors of 2 cancel...
	double quadraticTerm = -cov_dd / (2*covDeterminant);
	double discriminant = linearTerm*linearTerm - 4*quadraticTerm*(constantTerm + 2*z0SignificanceCut);
	if (discriminant < 0) continue;
	discriminant = sqrt(discriminant);
	double zMax = (-linearTerm - discriminant)/(2*quadraticTerm);
	double zMin = (-linearTerm + discriminant)/(2*quadraticTerm);
	m_maxRange = std::max(m_maxRange, std::max(zMax-z0, z0-zMin));
	constantTerm -= log(2*Gaudi::Units::pi*sqrt(covDeterminant));
	m_trackMap.emplace(std::piecewise_construct,
                           std::forward_as_tuple(*itrk),
			   std::forward_as_tuple(constantTerm, linearTerm, quadraticTerm, zMin, zMax));
	m_lowerMap.emplace(std::piecewise_construct,
			   std::forward_as_tuple(constantTerm, linearTerm, quadraticTerm, zMin, zMax),
			   std::forward_as_tuple(*itrk));
	m_upperMap.emplace(std::piecewise_construct,
			   std::forward_as_tuple(constantTerm, linearTerm, quadraticTerm, zMin, zMax),
			   std::forward_as_tuple(*itrk));
      }
    }
  }

  void GaussianTrackDensity::removeTracks(const std::vector<const TrackParameters*>& perigeeList)
  {
    for (auto iparam : perigeeList)
    {
      const Perigee* itrk = dynamic_cast<const Perigee*>(iparam);
      if (itrk != nullptr)
      {
	if (m_trackMap.count(*itrk) == 0) continue;
	const auto& entry = m_trackMap[*itrk];
	if (m_lowerMap.count(entry) > 0) m_lowerMap.erase(entry);
	if (m_upperMap.count(entry) > 0) m_upperMap.erase(entry);
	m_trackMap.erase(*itrk);
      }
    }
  }

  void GaussianTrackDensity::reset()
  {
    m_trackMap.clear();
    m_lowerMap.clear();
    m_upperMap.clear();
    m_maxRange = 0.0;
  }

  GaussianTrackDensity::TrackEntry::TrackEntry(double c0, double c1, double c2, double zMin, double zMax) 
    : c_0(c0), c_1(c1), c_2(c2), lowerBound(zMin), upperBound(zMax)
{ }

  // Dummy constructor for binary search
  GaussianTrackDensity::TrackEntry::TrackEntry(double z) 
    : c_0(0), c_1(0), c_2(0), lowerBound(z), upperBound(z)
{ }

}
