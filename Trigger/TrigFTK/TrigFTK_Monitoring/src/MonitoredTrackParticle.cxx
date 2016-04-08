/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_Monitoring/MonitoredTrackParticle.h"

using namespace TrigFTK;

//-----------------------------------------------------------------
// MonitoredTrackParticle
//-----------------------------------------------------------------
MonitoredTrackParticle::MonitoredTrackParticle(void) :
  m_2d_n_eta(1)
  , m_2d_eta_min(-3)
  , m_2d_eta_max(3)
  , m_2d_z0_min(-300)
  , m_2d_z0_max(300)
{}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void MonitoredTrackParticle::initialize(
    int n_eta,
    double eta_min,
    double eta_max,
    double z0_min,
    double z0_max)
{
  m_2d_n_eta    = n_eta;
  m_2d_eta_min  = eta_min;
  m_2d_eta_max  = eta_max;
  m_2d_z0_min   = z0_min;
  m_2d_z0_max   = z0_max;
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void MonitoredTrackParticle::declareForMonitoring( IMonitoredAlgo* p, std::string prefix, std::string suffix )
{
  const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear;
  p->declareMonitoredStdContainer( prefix + "eta"       + suffix, m_eta_vec       , reset );
  p->declareMonitoredStdContainer( prefix + "phi"       + suffix, m_phi_vec       , reset );
  p->declareMonitoredStdContainer( prefix + "pt"        + suffix, m_pt_vec        , reset );
  p->declareMonitoredStdContainer( prefix + "d0"        + suffix, m_d0_vec        , reset );
  p->declareMonitoredStdContainer( prefix + "z0"        + suffix, m_z0_vec        , reset );
  p->declareMonitoredStdContainer( prefix + "chi2Ndf"   + suffix, m_chi2Ndf_vec   , reset );
  p->declareMonitoredStdContainer( prefix + "nPix"      + suffix, m_nPix_vec      , reset );
  p->declareMonitoredStdContainer( prefix + "nSct"      + suffix, m_nSct_vec      , reset );
  p->declareMonitoredStdContainer( prefix + "nSi"       + suffix, m_nSi_vec       , reset );
  p->declareMonitoredStdContainer( prefix + "eta_phi"   + suffix, m_eta_phi_vec   , reset );
  p->declareMonitoredStdContainer( prefix + "eta_z0"    + suffix, m_eta_z0_vec    , reset );
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void MonitoredTrackParticle::push_back(const xAOD::TrackParticle* track)
{
  double eta     = track->eta();
  double phi     = track->phi();
  double pt      = track->pt() * 0.001;
  double d0      = track->d0();
  double z0      = track->z0();
  double chi2Ndf = track->chiSquared() / track->numberDoF();
  uint8_t nPix;
  uint8_t nSct;
  track->summaryValue( nPix, xAOD::numberOfPixelHits);
  track->summaryValue( nSct, xAOD::numberOfSCTHits);

  m_eta_vec.push_back( eta );
  m_phi_vec.push_back( phi );
  m_pt_vec.push_back( pt );
  m_d0_vec.push_back( d0 );
  m_z0_vec.push_back( z0 );
  m_chi2Ndf_vec.push_back( chi2Ndf );

  m_nPix_vec.push_back( nPix );
  m_nSct_vec.push_back( nSct );
  m_nSi_vec.push_back( nSct+nPix );
  
  m_eta_phi_vec.push_back( getEtaPhi(eta,phi) );
  m_eta_z0_vec.push_back( getEtaZ0(eta,z0) );
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void MonitoredTrackParticle::reserve(int size)
{
  m_eta_vec.reserve(size);
  m_phi_vec.reserve(size);
  m_pt_vec.reserve(size);
  m_d0_vec.reserve(size);
  m_z0_vec.reserve(size);
  m_nPix_vec.reserve(size);
  m_nSct_vec.reserve(size);
  m_chi2Ndf_vec.reserve(size);

  m_nSi_vec.reserve(size);    
  m_eta_phi_vec.reserve(size);
  m_eta_z0_vec.reserve(size);
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void MonitoredTrackParticle::clear(void)
{
  m_eta_vec.clear();
  m_phi_vec.clear();
  m_pt_vec.clear();
  m_z0_vec.clear();
  m_d0_vec.clear();
  m_nPix_vec.clear();
  m_nSct_vec.clear();
  m_chi2Ndf_vec.clear();

  m_nSi_vec.clear();    
  m_eta_phi_vec.clear();
  m_eta_z0_vec.clear();
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
int MonitoredTrackParticle::getEtaBin(const double &eta)
{
  return (int) ( (eta-m_2d_eta_min)*m_2d_n_eta / (m_2d_eta_max - m_2d_eta_min) );
}
double MonitoredTrackParticle::getEtaPhi(const double &eta, const double &phi)
{
  return phi + getEtaBin(eta) * 2 * M_PI;
}
double MonitoredTrackParticle::getEtaZ0(const double &eta, const double &z0)
{
  int bin_eta( getEtaBin(eta) );
  // fill overflow/underflow bins if z0 out of range
  if(z0 < m_2d_z0_min) bin_eta = -1;
  else if(z0 > m_2d_z0_max) bin_eta = m_2d_n_eta;
  return z0 + bin_eta * (m_2d_z0_max - m_2d_z0_min);
}


//-----------------------------------------------------------------
// MonitoredTrackParticlePair
//-----------------------------------------------------------------
void MonitoredTrackParticlePair::declareForMonitoring( IMonitoredAlgo* p, std::string prefix, std::string suffix)
{
   const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear;
  p->declareMonitoredStdContainer( prefix + "delta_eta"       + suffix, m_delta_eta_vec       , reset );
  p->declareMonitoredStdContainer( prefix + "delta_phi"       + suffix, m_delta_phi_vec       , reset );
  p->declareMonitoredStdContainer( prefix + "delta_pt"        + suffix, m_delta_pt_vec        , reset );
  p->declareMonitoredStdContainer( prefix + "delta_z0"        + suffix, m_delta_z0_vec        , reset );
  p->declareMonitoredStdContainer( prefix + "delta_d0"        + suffix, m_delta_d0_vec        , reset );
  p->declareMonitoredStdContainer( prefix + "delta_R"         + suffix, m_delta_R_vec         , reset );
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void MonitoredTrackParticlePair::push_back(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2)
{
  double delta_eta     = track1->eta() -  track2->eta();
  double delta_phi     = std::remainder(track1->phi()-track2->phi(), 2*M_PI);

  m_delta_eta_vec.push_back(delta_eta);
  m_delta_phi_vec.push_back(delta_phi);
  m_delta_pt_vec.push_back( (track1->pt() - track2->pt())*0.001 );
  m_delta_z0_vec.push_back( track1->z0() - track2->z0());
  m_delta_d0_vec.push_back( track1->d0() - track2->d0());
  m_delta_R_vec.push_back( std::sqrt( delta_phi*delta_phi + delta_eta*delta_eta ) );
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void MonitoredTrackParticlePair::reserve(int size)
{
  m_delta_eta_vec.reserve(size);
  m_delta_phi_vec.reserve(size);
  m_delta_pt_vec.reserve(size);
  m_delta_z0_vec.reserve(size);
  m_delta_d0_vec.reserve(size);
  m_delta_R_vec.reserve(size);
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void MonitoredTrackParticlePair::clear(void)
{
  m_delta_eta_vec.clear();
  m_delta_phi_vec.clear();
  m_delta_pt_vec.clear();
  m_delta_z0_vec.clear();
  m_delta_d0_vec.clear();
  m_delta_R_vec.clear();
}





