/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthIO/HepMCReadFromFile.h"


HepMCReadFromFile::HepMCReadFromFile(const std::string& name, ISvcLocator* pSvcLocator) :
  GenBase(name, pSvcLocator)
{
  declareProperty("InputFile", m_input_file="events.hepmc");
  m_event_number = 0;
}


StatusCode HepMCReadFromFile::initialize() {
  CHECK(GenBase::initialize());
  // Initialize input file and event number
  m_hepmcio.reset( new HepMC::IO_GenEvent(m_input_file.c_str(), std::ios::in) );
  m_event_number = 0;
  return StatusCode::SUCCESS;
}


StatusCode HepMCReadFromFile::execute() {
  /// @todo Should be a do-while until the read is successful or end of file?
  HepMC::GenEvent* evt = m_hepmcio->read_next_event();
  if (evt) {
    ++m_event_number;
    evt->set_event_number(m_event_number);
    GeVToMeV(evt);
    events()->push_back(evt);
  }
  return StatusCode::SUCCESS;
}
