/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTOND3PDMAKER_ITRUTHPARTICLEFILTERCUTTOOL_H
#define PHOTOND3PDMAKER_ITRUTHPARTICLEFILTERCUTTOOL_H
 
 
// Includes for Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IAlgTool.h"

namespace HepMC{
  class GenParticle;
}
 
static const InterfaceID IID_ITruthParticleFilterCutTool("ITruthParticleFilterCutTool", 1 , 0);
 
class ITruthParticleFilterCutTool: virtual public IAlgTool {
 
 public:
  // Athena algorithm's Hooks
  virtual StatusCode  initialize() = 0;
  virtual StatusCode  execute()    = 0;
  virtual StatusCode  finalize()   = 0;
  virtual bool passesCut( const HepMC::GenParticle* tp)=0 ;

  static const InterfaceID& interfaceID() { return IID_ITruthParticleFilterCutTool;}
};

#endif 
