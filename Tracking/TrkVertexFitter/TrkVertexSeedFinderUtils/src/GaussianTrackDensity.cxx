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

  double GaussianTrackDensity::trackDensity(double z) const
  {
    double sum = 0.0;
    for (auto& entry : m_trackMap)
    {
      sum += entry.second.constant * exp(z*(entry.second.c_1 + z*entry.second.c_2));
    }
    return sum;
  }

  void GaussianTrackDensity::trackDensity(double z, double& density, double& firstDerivative, double& secondDerivative) const
  {
    density = 0.0;
    firstDerivative = 0.0;
    secondDerivative = 0.0;
    for (auto& entry : m_trackMap)
    {
      double delta = entry.second.constant * exp(z*(entry.second.c_1 + z*entry.second.c_2));
      density += delta;
      double qPrime = entry.second.c_1 + 2*z*entry.second.c_2;
      double deltaPrime = delta * qPrime;
      firstDerivative += deltaPrime;
      secondDerivative += 2*entry.second.c_2*delta + qPrime*deltaPrime;
    }
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
	double cov_dz = perigeeCov(Trk::d0, Trk::z0);
	double covDeterminant = cov_dd*cov_zz - cov_dz*cov_dz;
	if ( covDeterminant <= 0 ) continue;
	double constantArg = (d0*d0*cov_zz + z0*z0*cov_dd + 2*d0*z0*cov_dz) / (2*covDeterminant);
	double constantFactor =  exp(-constantArg) / (2*Gaudi::Units::pi*covDeterminant);
	double linearTerm = (d0*cov_dz + z0*cov_dd) / covDeterminant ; // minus signs and factors of 2 cancel...
	double quadraticTerm = -cov_dd / (2*covDeterminant);
	m_trackMap.emplace(std::piecewise_construct,
                           std::forward_as_tuple(*itrk),
			   std::forward_as_tuple(constantFactor, linearTerm, quadraticTerm));
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
      }
    }
  }

  void GaussianTrackDensity::reset()
  {
    m_trackMap.clear();
  }

  GaussianTrackDensity::TrackEntry::TrackEntry(double c0, double c1, double c2) : constant(c0), c_1(c1), c_2(c2) { }

}
