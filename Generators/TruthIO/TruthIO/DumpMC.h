/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHIO_DUMPMC_H
#define TRUTHIO_DUMPMC_H

#include "GeneratorModules/GenBase.h"


/// Dump MC event record
class DumpMC : public GenBase {
public:

  DumpMC(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

  std::string m_keyout;
  bool m_VerboseOutput;
  bool m_DeepCopy;
  bool m_EtaPhi;
  bool m_PrintQuasiStableParticles;
};

#endif
