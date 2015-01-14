/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_IPARTICLESELECTORALL_H
#define TRIGHLTJETREC_IPARTICLESELECTORALL_H

#include "TrigHLTJetRec/IIParticleSelector.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHLTJetRec/IParticleVector.h"

class StatusCode;

class IParticleSelectorAll: virtual public IIParticleSelector, 
  public AthAlgTool {

public:

  IParticleSelectorAll(const std::string&,
                       const std::string&,
                       const IInterface*);

  ~IParticleSelectorAll(){};
  
  StatusCode select(IParticleVector&) const override;
};

#endif
