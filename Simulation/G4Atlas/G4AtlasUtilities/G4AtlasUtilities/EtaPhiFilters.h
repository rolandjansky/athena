/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EtaPhiFilters_H
#define EtaPhiFilters_H

#include "FadsKinematics/ParticleManipulator.h"

typedef std::pair<double, double> EtaRange;
typedef std::pair<double, double> PhiRange;


class EtaPhiFilters : public FADS::ParticleManipulator {
public:

  EtaPhiFilters(const std::string& s)
    : ParticleManipulator(s)
  {  }


  void AddEtaInterval(double a, double b) {
  	m_eLimits.push_back(std::make_pair(a,b));
  }

  const std::vector<EtaRange>& GetEtaIntervals() {
    return m_eLimits;
  }

  void ClearEtaIntervals() {
	m_eLimits.clear();
  }


  void AddPhiInterval(double a, double b) {
	m_pLimits.push_back(std::make_pair(a,b));
  }

  const std::vector<PhiRange>& GetPhiIntervals() {
    return m_pLimits;
  }

  void ClearPhiIntervals() {
	m_pLimits.clear();
  }

  // For the C++ layer only
  bool EditParticle(G4PrimaryParticle* p);

private:

  std::vector<EtaRange> m_eLimits;
  std::vector<PhiRange> m_pLimits;

};

#endif
