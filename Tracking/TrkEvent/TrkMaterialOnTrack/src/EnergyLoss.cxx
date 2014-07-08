/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLoss.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "GaudiKernel/MsgStream.h"
#include <string>

Trk::EnergyLoss::EnergyLoss() :
  m_deltaE(0.),
  m_sigmaMinusDeltaE(0.),
  m_sigmaPlusDeltaE(0.),
  m_mean_ioni(0.),
  m_sig_ioni(0.),
  m_mean_rad(0.),
  m_sig_rad(0.)
{
}

Trk::EnergyLoss::EnergyLoss(double deltaE, double sigmaDeltaE,
                            double sMinusDeltaE, double sPlusDeltaE)  :
  m_deltaE(deltaE),
  m_sigmaDeltaE(sigmaDeltaE),
  m_sigmaMinusDeltaE(sMinusDeltaE>0.0?sMinusDeltaE:sigmaDeltaE),
  m_sigmaPlusDeltaE(sPlusDeltaE>0.0?sPlusDeltaE:sigmaDeltaE),
  m_mean_ioni(0.),
  m_sig_ioni(0.),
  m_mean_rad(0.),
  m_sig_rad(0.)
{ }

Trk::EnergyLoss::EnergyLoss(double deltaE, double sigmaDeltaE,
                            double mean_ioni, double sig_ioni,
			    double mean_rad, double sig_rad)  :
  m_deltaE(deltaE),
  m_sigmaDeltaE(sigmaDeltaE),
  m_sigmaMinusDeltaE(0.),
  m_sigmaPlusDeltaE(0.),
  m_mean_ioni(mean_ioni),
  m_sig_ioni(sig_ioni),
  m_mean_rad(mean_rad),
  m_sig_rad(sig_rad)
{ }

Trk::EnergyLoss::EnergyLoss(const Trk::EnergyLoss& eloss) :
  m_deltaE(eloss.m_deltaE),
  m_sigmaDeltaE(eloss.m_sigmaDeltaE),
  m_sigmaMinusDeltaE(eloss.m_sigmaMinusDeltaE),
  m_sigmaPlusDeltaE(eloss.m_sigmaPlusDeltaE),
  m_mean_ioni(eloss.m_mean_ioni),
  m_sig_ioni(eloss.m_sig_ioni),
  m_mean_rad(eloss.m_mean_rad),
  m_sig_rad(eloss.m_sig_rad)
{ }

// Overload of << operator for MsgStream and std::ostream for debug output 
MsgStream& Trk::EnergyLoss::dump (MsgStream& sl) const
{ 
  sl << "EnergyLoss :   ( delta(E), sigma(dE) ) = \t"
      << "("<< deltaE()<<", \t"<< sigmaDeltaE() << ")";
  return sl;
}

std::ostream& Trk::EnergyLoss::dump (std::ostream& sl) const
{ 
  sl << "EnergyLoss :   ( delta(E), sigma(dE) ) = \t"
     << "("<< deltaE()<<", \t"<< sigmaDeltaE() << ")";
  return sl; 
}

