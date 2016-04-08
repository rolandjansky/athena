/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOR_RELDIS_I_H
#define GENERATOR_RELDIS_I_H

#include "GeneratorModules/GenModule.h"
#include "Reldis_i/HepMCInterface.h"
#include "Reldis_i/ReldisTree.h"

using namespace HepMC;

class Reldis_i: public GenModule {

 public:
  
  Reldis_i(const std::string& name, ISvcLocator* pSvcLocator);
  ~Reldis_i();
  
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode fillEvt(GenEvent*);
  virtual StatusCode genFinalize();
  
 private:
  
  std::string m_reldisRootFileName;
  std::string m_reldisTreeName;
  ReldisTree  m_reldisTree;
  I_Reldis    m_reldisToHepMC;

  int m_eventCounter;  
};

#endif
