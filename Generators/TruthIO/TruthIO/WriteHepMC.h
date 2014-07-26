/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHIO_WRITEHEPMC_H
#define TRUTHIO_WRITEHEPMC_H

#include "GeneratorModules/GenBase.h"
#include "HepMC/IO_GenEvent.h"
#include <memory>


/// Write the MC event record to file in IO_GenEvent text format
class WriteHepMC : public GenBase {
public:

  WriteHepMC(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

  std::string m_outfile;
  int m_precision;

  std::auto_ptr<HepMC::IO_GenEvent> m_hepmcio;

};

#endif
