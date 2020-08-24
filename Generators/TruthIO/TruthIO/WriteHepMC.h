/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHIO_WRITEHEPMC_H
#define TRUTHIO_WRITEHEPMC_H

#include "GeneratorModules/GenBase.h"
#include "AtlasHepMC/IO_GenEvent.h"
#include <memory>


/// Write the MC event record to file in IO_GenEvent text format
class WriteHepMC : public GenBase {
public:

  WriteHepMC(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

  std::string m_outfile;
  int m_precision;

#ifdef HEPMC3
  std::unique_ptr<HepMC3::WriterAsciiHepMC2> m_hepmcio;
#else
  std::unique_ptr<HepMC::IO_GenEvent> m_hepmcio;

#endif
};

#endif
