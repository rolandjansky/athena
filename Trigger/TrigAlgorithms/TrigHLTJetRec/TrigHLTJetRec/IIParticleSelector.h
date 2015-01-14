/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETREC_IIPARTICLESELECTOR_H
#define TRIGHLTJETREC_IIPARTICLESELECTOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigHLTJetRec/IParticleVector.h"

class StatusCode;

class IIParticleSelector: virtual public IAlgTool {

public:

  static const InterfaceID& interfaceID() {
    static const InterfaceID
      IID_IIParticleSelector("IIParticleSelector", 1, 0);
    return IID_IIParticleSelector;
  }

  virtual ~IIParticleSelector();

  virtual StatusCode select(IParticleVector&) const = 0;
  
};

#endif
