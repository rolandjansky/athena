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
    AthAlgTool(t, n, p)
  {
    declareInterface<IVertexTrackDensityEstimator>(this);
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
    TrackEntry target(z);
    lowerMapIterator first = m_lowerMap.lower_bound(target);
    upperMapIterator final = m_upperMap.upper_bound(target);
    trackMapIterator firstLoop = m_trackMap.find(first->second);
    trackMapIterator finalLoop = m_trackMap.find(final->second);
    density = 0.0;
    firstDerivative = 0.0;
    secondDerivative = 0.0;
    for (auto itrk = firstLoop; itrk != finalLoop && itrk != m_trackMap.end(); itrk++)
    {
      double delta = exp(itrk->second.c_0+z*(itrk->second.c_1 + z*itrk->second.c_2));
      density += delta;
      double qPrime = itrk->second.c_1 + 2*z*itrk->second.c_2;
      double deltaPrime = delta * qPrime;
      firstDerivative += deltaPrime;
      secondDerivative += 2*itrk->second.c_2*delta + qPrime*deltaPrime;
    }
  }

  double GaussianTrackDensity::trackDensity(double z) const
  {
    TrackEntry target(z);
    lowerMapIterator first = m_lowerMap.lower_bound(target);
    upperMapIterator final = m_upperMap.upper_bound(target);
    trackMapIterator firstLoop = m_trackMap.find(first->second);
    trackMapIterator finalLoop = m_trackMap.find(final->second);
    double sum = 0.0;
    for (auto itrk = firstLoop; itrk != finalLoop && itrk != m_trackMap.end(); itrk++)
    {
      sum += exp(itrk->second.c_0+z*(itrk->second.c_1 + itrk->second.c_2));
    }
    return sum;
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
	double discriminant = linearTerm*linearTerm - 4*quadraticTerm*(constantTerm + 2*z0SignificanceCut*cov_zz);
	if (discriminant < 0) continue;
	discriminant = sqrt(discriminant);
	double zMin = (-linearTerm - discriminant)/(2*quadraticTerm);
	double zMax = (-linearTerm + discriminant)/(2*quadraticTerm);
	constantTerm -= log(2*Gaudi::Units::pi*covDeterminant);
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
  }

  GaussianTrackDensity::TrackEntry::TrackEntry(double c0, double c1, double c2, double zMin, double zMax) 
    : c_0(c0), c_1(c1), c_2(c2), lowerBound(zMin), upperBound(zMax)
{ }

  // Dummy constructor for binary search
  GaussianTrackDensity::TrackEntry::TrackEntry(double z) 
    : c_0(0), c_1(0), c_2(0), lowerBound(z), upperBound(z)
{ }

}
