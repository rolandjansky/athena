/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_DSTD0K3PIFILTER_H
#define GENERATORFILTERS_DSTD0K3PIFILTER_H

#include "GeneratorModules/GenFilter.h"


/// Filters and looks for D*+ -> D0 pi+; D0 -> (6 intermediate states) -> K- CLHEP::pi+ CLHEP::pi- CLHEP::pi+
///
/// 1. K_1+ CLHEP::pi-   10323 211
///       K_1+ -> K*0 (K-CLHEP::pi+) CLHEP::pi+
///       K_1+ -> K+ rho0                  rho0-> CLHEP::pi CLHEP::pi 100% in decay.dec
///       K_1+ -> K- CLHEP::pi+ CLHEP::pi+
///
/// 2. K*0 rho0
///        K*0->K- CLHEP::pi+
///
/// 3. K+ a_1+    321 20213
///       a_1+-> rho0 pi+
///
/// 4. K+ CLHEP::pi- rho0
///
/// 5. K*0 pi - CLHEP::pi-
///        K*0->K- CLHEP::pi+
///
/// 6. K+ CLHEP::pi- CLHEP::pi+ CLHEP::pi-
///
/// @author Woochun Park, October 5, 2010
class DstD0K3piFilter:public GenFilter {
public:

  DstD0K3piFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  bool CheckChildLundId(HepMC::ConstGenParticlePtr mcpart, unsigned int nth, int chLundId); // Check lundId of Nth(0 or 1) children from mcpart...
  bool IsCandidate(std::vector<float>& lundIds, std::vector<HepMC::ConstGenParticlePtr>& genParticles); // Is Candidate among 6 intermediate state??

  double m_Ptmin;
  double m_EtaRange;

};

#endif
