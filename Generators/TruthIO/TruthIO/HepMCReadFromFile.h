/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHIO_HEPMCREADFROMFILE_H
#define TRUTHIO_HEPMCREADFROMFILE_H

#include "GeneratorModules/GenBase.h"
#include "AtlasHepMC/IO_GenEvent.h"
#include <memory>

class StoreGateSvc;

class HepMCReadFromFile : public GenBase {
public:

  HepMCReadFromFile(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:

  std::string m_input_file;
  int m_event_number;
  
  StoreGateSvc* m_sgSvc;

#ifdef HEPMC3
  std::unique_ptr<HepMC3::ReaderAsciiHepMC2> m_hepmcio;
#else
  std::unique_ptr<HepMC::IO_GenEvent> m_hepmcio;
#endif
};

#endif
