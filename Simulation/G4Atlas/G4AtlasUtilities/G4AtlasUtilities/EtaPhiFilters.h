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
  	eLimits.push_back(std::make_pair(a,b));
  }

  const std::vector<EtaRange>& GetEtaIntervals() {
    return eLimits;
  }

  void ClearEtaIntervals() {
	eLimits.clear();
  }


  void AddPhiInterval(double a, double b) {
	pLimits.push_back(std::make_pair(a,b));
  }

  const std::vector<PhiRange>& GetPhiIntervals() {
    return pLimits;
  }

  void ClearPhiIntervals() {
	pLimits.clear();
  }

  // For the C++ layer only
  bool EditParticle(G4PrimaryParticle* p);

private:

  std::vector<EtaRange> eLimits;
  std::vector<PhiRange> pLimits;

};

#endif
