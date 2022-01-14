/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef GENERATORFILTERSXAODPARENTCHILDFILTER_H
#define GENERATORFILTERSXAODPARENTCHILDFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

/// Allows the user to search for any given decay  Parent -> Child + X
/// @author B K Gjelsten, March 2006
class xAODParentChildFilter : public GenFilter {
public:

  xAODParentChildFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();

private:

  Gaudi::Property<double> m_PtMinParent{this,"PtMinParent",0.,"Min parent Pt"};
  Gaudi::Property<double> m_PtMaxParent{this,"PtMaxParent",1e9,"Max parent Pt"};
  Gaudi::Property<double> m_MassMinParent{this,"MassMinParent",-1e9,"Min parent mass"};
  Gaudi::Property<double> m_MassMaxParent{this,"MassMaxParent",-1e9,"Max parent mass"};
  Gaudi::Property<std::vector<int>> m_PDGParent{this,"PDGParent",0,"PDG ID of parents"};
  Gaudi::Property<double> m_EtaRangeParent{this,"EtaRangeParent",10.0,"Parent eta range"};
  Gaudi::Property<std::vector<int>> m_PDGChild{this,"PDGChild",0,"PDG ID of child"};
  Gaudi::Property<double> m_PtMinChild{this,"PtMinChild",0.,"Min child Pt"};
  Gaudi::Property<double> m_EtaRangeChild{this,"EtaRangeChild",0.,"Child eta range"};


  /** calculate the rapidity of a particle */
  inline double getRapidity(HepMC::GenParticle*& p) {
    double e = (p)->momentum().e();
    double pz = (p)->momentum().pz();
    if (e == pz) return 9999.;
    if (e == -pz) return -9999.;
    double rapidity = 0.5 * log((e + pz) / (e - pz));
    return rapidity;
  };
};

#endif
