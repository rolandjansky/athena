/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_IPARTICLESELECTORETAPT_H
#define TRIGHLTJETREC_IPARTICLESELECTORETAPT_H

#include "TrigHLTJetRec/IIParticleSelector.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHLTJetRec/IParticleVector.h"

class StatusCode;

class IParticleSelectorEtaPt:
virtual public IIParticleSelector, public AthAlgTool {

public:

  IParticleSelectorEtaPt(const std::string&,
                         const std::string&,
                         const IInterface*);

  ~IParticleSelectorEtaPt(){};
  
  StatusCode initialize() override;
  StatusCode select(IParticleVector&) const override;
 private:
  double m_etaMax;
  double m_ptMin;
};

#endif
