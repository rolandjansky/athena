/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolFactory.h"
#include "TrigHLTJetRec/IParticleSelectorAll.h"

IParticleSelectorAll::IParticleSelectorAll(const std::string& s,
                                             const std::string& name,
                                             const IInterface* i):
  AthAlgTool(s, name, i){
  declareInterface<IIParticleSelector>(this);
}

StatusCode IParticleSelectorAll::select(IParticleVector&) const {
  return StatusCode::SUCCESS;}
  


