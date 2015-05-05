/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <set>

#include "TruthIO/ReadHepEvtFromAscii.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include "GaudiKernel/DataSvc.h"

#include "StoreGate/StoreGateSvc.h"

extern "C" {
  void openfile_(int & ifile, const char* filename, int);
  void readevt_(int& ifile);
  void closefile_(int& ifile);
}


ReadHepEvtFromAscii::ReadHepEvtFromAscii(const std::string& name, ISvcLocator* pSvcLocator) 
  :  AthAlgorithm(name, pSvcLocator),
     m_sgSvc(0), m_ascii_in(0)
{
  // Set users' request
  declareProperty("McEventKey",  m_key        = "GEN_EVENT");
  declareProperty("HepEvtFile",   m_input_file = "events.ascii");
}

StatusCode ReadHepEvtFromAscii::initialize(){

  msg(MSG::INFO) << ">>> ReadHepEvtFromAscii from Initialize" << endreq;

  StatusCode sc = service("StoreGateSvc", m_sgSvc);
    
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not find StoreGateSvc" << endreq;
    return sc;
  }

  HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);
  
  // Initialize input file
//  m_ascii_in = new HepMC::IO_Ascii(m_input_file.c_str(), std::ios::in);
  int ifile=5;
  closefile_(ifile);
  openfile_(ifile,m_input_file.c_str(),m_input_file.size());

  // Initialization terminated
  return StatusCode::SUCCESS;
}
StatusCode ReadHepEvtFromAscii::execute() {

  msg(MSG::INFO) << ">>> ReadHepEvtFromAscii from execute" << endreq;
  
  McEventCollection* mcEvtColl;

  if ( m_sgSvc->contains<McEventCollection>(m_key) && m_sgSvc->retrieve(mcEvtColl, m_key).isSuccess() ) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "found an McEventCollecion in store" << endreq;
  } else {
    // McCollection doesn't exist. Create it (empty)
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "create new McEventCollecion in store" << endreq;
    mcEvtColl = new McEventCollection;
    StatusCode status = m_sgSvc->record( mcEvtColl, m_key );
    if (status.isFailure()) {
      msg(MSG::ERROR) << "Could not record McEventCollection" << endreq;
      return status;
    }
  }

  HepMC::GenEvent* evt = new HepMC::GenEvent(); 
  int ifile=5; 
  readevt_(ifile);

  HepMC::IO_HEPEVT hepio;
  hepio.set_print_inconsistency_errors(0);
  hepio.fill_next_event(evt);
 
  // Convert GeV to MeV 
  for ( HepMC::GenEvent::particle_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p )
      {
	  HepMC::FourVector newMomentum(0.,0.,0.,0.);
	  newMomentum.setPx( (*p)->momentum().px() * 1000. );
	  newMomentum.setPy( (*p)->momentum().py() * 1000. );
	  newMomentum.setPz( (*p)->momentum().pz() * 1000. );
	  newMomentum.setE( (*p)->momentum().e() * 1000. );
	  (*p)->set_momentum(newMomentum);

      }
  mcEvtColl->push_back(evt);
  
  // End of execution for each event
  return StatusCode::SUCCESS;
}
StatusCode ReadHepEvtFromAscii::finalize() {

  msg(MSG::INFO) << ">>> ReadHepEvtFromAscii from finalize" << endreq;

  int ifile=5;
  closefile_(ifile);
  // End of finalization step
  return StatusCode::SUCCESS;

}
