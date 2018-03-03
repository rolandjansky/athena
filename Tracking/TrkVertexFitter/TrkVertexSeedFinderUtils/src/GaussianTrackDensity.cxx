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
    AthAlgTool(t, n, p),
    m_dirty(false)
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

  GaussianTrackDensity::trackMapIterator GaussianTrackDensity::findStart(double z)
  {
    trackMapIterator result = m_trackMap.cbegin();
    while (result != m_trackMap.cend() && result->first.parameters()[Trk::z0] <= z) 
    {
	result++;
    }
    // always goes one past the track immediately to left of z
    return (--result)->second.start;
  }

  GaussianTrackDensity::trackMapIterator GaussianTrackDensity::findFinish(double z) 
  {
    trackMapIterator result = m_trackMap.cend();
    result--;
    while (result != m_trackMap.cbegin() && result->first.parameters()[Trk::z0] > z)
    {
      result--;
    }
    if (result->first.parameters()[Trk::z0] < z) result++;
    return result->second.finish;
  }

  void GaussianTrackDensity::trackDensity(double z, double& density, double& firstDerivative, double& secondDerivative)
  {
    if (m_dirty) prepareTracks();
    density = 0.0;
    firstDerivative = 0.0;
    secondDerivative = 0.0;
    for (auto it = findStart(z); it != findFinish(z); ++it)
    {
      double delta = it->second.constant * exp(z*(it->second.c_1 + z*it->second.c_2));
      density += delta;
      double qPrime = it->second.c_1 + 2*z*it->second.c_2;
      double deltaPrime = delta * qPrime;
      firstDerivative += deltaPrime;
      secondDerivative += 2*it->second.c_2*delta + qPrime*deltaPrime;
    }
  }

  double GaussianTrackDensity::trackDensity(double z)
  {
    if (m_dirty) prepareTracks();
    double sum = 0.0;
    for (auto it = findStart(z); it != findFinish(z); ++it)
    {
      sum += it->second.constant * exp(z*(it->second.c_1 + z*it->second.c_2));
    }
    return sum;
  }

  void GaussianTrackDensity::addTracks(const std::vector<const Trk::Track*>& vectorTrk)
  {
    std::vector<const TrackParameters*> perigeeList;
    for (auto itrk : vectorTrk)
    {
      perigeeList.push_back(itrk->perigeeParameters());
    }

    addTracks(perigeeList);
  }

  void GaussianTrackDensity::removeTracks(const std::vector<const Trk::Track*>& vectorTrk)
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
    double significanceCut = m_d0MaxSignificance * m_d0MaxSignificance;
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
	if ( d0*d0/cov_dd > significanceCut ) continue;
	double cov_zz = perigeeCov(Trk::z0, Trk::z0);
	if (cov_zz <= 0 ) continue;
	double cov_dz = perigeeCov(Trk::d0, Trk::z0);
	double covDeterminant = cov_dd*cov_zz - cov_dz*cov_dz;
	if ( covDeterminant <= 0 ) continue;
	double constantArg = (d0*d0*cov_zz + z0*z0*cov_dd + 2*d0*z0*cov_dz) / (2*covDeterminant);
	double constantFactor =  exp(-constantArg) / (2*Gaudi::Units::pi*covDeterminant);
	double linearTerm = (d0*cov_dz + z0*cov_dd) / covDeterminant ; // minus signs and factors of 2 cancel...
	double quadraticTerm = -cov_dd / (2*covDeterminant);
	m_trackMap.emplace(std::piecewise_construct,
                           std::forward_as_tuple(*itrk),
			   std::forward_as_tuple(constantFactor, linearTerm, quadraticTerm, cov_zz));
	m_dirty = true;
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
	m_trackMap.erase(*itrk);
	m_dirty = true;
      }
    }
  }

  void GaussianTrackDensity::prepareTracks()
  {
    trackMapIterator left = m_trackMap.cbegin();
    double z0Cut = m_z0MaxSignificance * m_z0MaxSignificance;
    for (auto& entry : m_trackMap)
    {
      double myZ0 = entry.first.parameters()[Trk::z0];
      // left should advance to the left-most track to the left of "entry" that is within n sigma of it,
      // or no track to the left of "entry" passes, it should stop at "entry" itself
      while (pow(myZ0 - left->first.parameters()[Trk::z0],2) > z0Cut*left->second.cov_z && 
	     left->first.parameters()[Trk::z0] < myZ0) 
	left++;
      // right should advance to the element after the right-most element within n-sigma of "entry"
      trackMapIterator right = m_trackMap.cend();
      while((--right)->first.parameters()[Trk::z0] > myZ0)
      {
	if (pow(myZ0 - right->first.parameters()[Trk::z0],2) < z0Cut*right->second.cov_z) break;
      }
      entry.second.start = left;
      entry.second.finish = ++right;
    }
    m_dirty = false;
  }

  void GaussianTrackDensity::reset()
  {
    m_trackMap.clear();
    m_dirty = false;
  }

  GaussianTrackDensity::TrackEntry::TrackEntry(double c0, double c1, double c2, double covz) 
    : constant(c0), c_1(c1), c_2(c2), cov_z(covz), start(), finish() 
{ }

}
