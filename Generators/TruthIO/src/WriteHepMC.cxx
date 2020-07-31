/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthIO/WriteHepMC.h"


WriteHepMC::WriteHepMC(const std::string& name, ISvcLocator* pSvcLocator)
  : GenBase(name, pSvcLocator)
{
  declareProperty("OutputFile", m_outfile="events.hepmc");
  declareProperty("Precision", m_precision=8);
}


StatusCode WriteHepMC::initialize() {
  CHECK(GenBase::initialize());
#ifdef HEPMC3
  m_hepmcio.reset(new HepMC3::WriterAsciiHepMC2(m_outfile) );
  m_hepmcio->set_precision(m_precision);
#else
  m_hepmcio.reset( new HepMC::IO_GenEvent(m_outfile) );
  m_hepmcio->precision(m_precision);
#endif
  return StatusCode::SUCCESS;
}


StatusCode WriteHepMC::execute() {
  // Just write out the first (i.e. signal) event in the collection
#ifdef HEPMC3
  m_hepmcio->write_event(*(event_const()));
#else
  m_hepmcio->write_event(event_const());
#endif
  return StatusCode::SUCCESS;
}
