/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthIO/HepMCReadFromFile.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/IO_HEPEVT.h"
#include "AtlasHepMC/HEPEVT_Wrapper.h"

#include "GaudiKernel/DataSvc.h"

#include "StoreGate/StoreGateSvc.h"


HepMCReadFromFile::HepMCReadFromFile(const std::string& name, ISvcLocator* pSvcLocator) :
  GenBase(name, pSvcLocator),
  m_sgSvc(0)
{
  declareProperty("InputFile", m_input_file="events.hepmc");
  m_event_number = 0;
}


StatusCode HepMCReadFromFile::initialize() {
  CHECK(GenBase::initialize());
  
  StatusCode sc = service("StoreGateSvc", m_sgSvc);   
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not find StoreGateSvc" << endmsg;
    return sc;
  }
  
  // Initialize input file and event number
#ifdef HEPMC3
  m_hepmcio.reset( new HepMC3::ReaderAsciiHepMC2(m_input_file.c_str()) );
#else
  m_hepmcio.reset( new HepMC::IO_GenEvent(m_input_file.c_str(), std::ios::in) );
#endif
  m_event_number = 0;
  return StatusCode::SUCCESS;
}


StatusCode HepMCReadFromFile::execute() {

  McEventCollection* mcEvtColl;

  if ( m_sgSvc->contains<McEventCollection>(m_mcEventKey) && m_sgSvc->retrieve(mcEvtColl, m_mcEventKey).isSuccess() ) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "found an McEventCollecion in store" << endmsg;
  } else {
    // McCollection doesn't exist. Create it (empty)
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "create new McEventCollecion in store" << endmsg;
    mcEvtColl = new McEventCollection;
    StatusCode status = m_sgSvc->record( mcEvtColl, m_mcEventKey );
    if (status.isFailure()) {
      msg(MSG::ERROR) << "Could not record McEventCollection" << endmsg;
      return status;
    }
  }
#ifdef HEPMC3
  HepMC3::GenEvent* evt = new HepMC3::GenEvent();
  m_hepmcio->read_event(*evt);
  if (m_hepmcio) {
    ++m_event_number;
    evt->set_event_number(m_event_number);
    mcEvtColl->push_back(evt);
  }

#else
  /// @todo Should be a do-while until the read is successful or end of file?
  HepMC::GenEvent* evt = m_hepmcio->read_next_event();
  if (evt) {
    ++m_event_number;
    evt->set_event_number(m_event_number);
    GeVToMeV(evt);
    mcEvtColl->push_back(evt);
  }
#endif
  return StatusCode::SUCCESS;
}
