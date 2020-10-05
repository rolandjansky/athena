/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <set>

#include "TruthIO/ReadHepEvtFromAscii.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/IO_HEPEVT.h"
#include "AtlasHepMC/HEPEVT_Wrapper.h"

#include "GaudiKernel/DataSvc.h"

#include "StoreGate/StoreGateSvc.h"



bool ReadHepEvtFromAscii::read_hepevt_event_header()
{
    const size_t       max_e_buffer_size=512;
    char buf_e[max_e_buffer_size];
    bool eventline=false;
    int im=0, pm=0;
    while(!eventline)
    {
        m_file.getline(buf_e,max_e_buffer_size);
        if( strlen(buf_e) == 0 ) return false;
        std::stringstream st_e(buf_e);
        char attr=' ';
        eventline=false;
        while (!eventline)
        {
            if (!(st_e>>attr)) break;
            if (attr==' ') continue;
            else eventline=false;
            if (attr=='E')
            {
                eventline=static_cast<bool>(st_e>>im>>pm);
            }
        }
    }
    HepMC::HEPEVT_Wrapper::set_event_number(im);
    HepMC::HEPEVT_Wrapper::set_number_entries(pm);
    return eventline;
}
bool ReadHepEvtFromAscii::read_hepevt_particle( int i)
{
    const size_t       max_p_buffer_size=512;
    const size_t       max_v_buffer_size=512;
    char buf_p[max_p_buffer_size];
    char buf_v[max_v_buffer_size];
    int   intcodes[6];
    double fltcodes1[5];
    double fltcodes2[4];
    m_file.getline(buf_p,max_p_buffer_size);
    if( strlen(buf_p) == 0 ) return false;
    m_file.getline(buf_v,max_v_buffer_size);
    if( strlen(buf_v) == 0 ) return false;
    std::stringstream st_p(buf_p);
    std::stringstream st_v(buf_v);
    if (!static_cast<bool>(st_p>>intcodes[0]>>intcodes[1]>>intcodes[2]>>intcodes[3]>>intcodes[4]>>intcodes[5]>>fltcodes1[0]>>fltcodes1[1]>>fltcodes1[2]>>fltcodes1[3]>>fltcodes1[4])) {     return false;}
    if (!static_cast<bool>(st_v>>fltcodes2[0]>>fltcodes2[1]>>fltcodes2[2]>>fltcodes2[3])) { return false;}
    HepMC::HEPEVT_Wrapper::set_status(i,intcodes[0]);
    HepMC::HEPEVT_Wrapper::set_id(i,intcodes[1]);
    HepMC::HEPEVT_Wrapper::set_parents(i,intcodes[2],std::max(intcodes[2],intcodes[3]));/* Pythia writes second mother 0*/
    HepMC::HEPEVT_Wrapper::set_children(i,intcodes[4],intcodes[5]);
    HepMC::HEPEVT_Wrapper::set_momentum(i,fltcodes1[0],fltcodes1[1],fltcodes1[2],fltcodes1[3]);
    HepMC::HEPEVT_Wrapper::set_mass(i,fltcodes1[4]);
    HepMC::HEPEVT_Wrapper::set_position(i,fltcodes2[0],fltcodes2[1],fltcodes2[2],fltcodes2[3]);
    return true;
}

ReadHepEvtFromAscii::ReadHepEvtFromAscii(const std::string& name, ISvcLocator* pSvcLocator) 
  :  AthAlgorithm(name, pSvcLocator),
     m_sgSvc(0)
{
  // Set users' request
  declareProperty("McEventKey",  m_key        = "GEN_EVENT");
  declareProperty("HepEvtFile",   m_input_file = "events.ascii");
}

StatusCode ReadHepEvtFromAscii::initialize(){

  msg(MSG::INFO) << ">>> ReadHepEvtFromAscii from Initialize" << endmsg;

  StatusCode sc = service("StoreGateSvc", m_sgSvc);
    
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not find StoreGateSvc" << endmsg;
    return sc;
  }
#ifdef HEPMC3
//These things are not present
#else

  HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);
  
#endif
  // Initialize input file
  m_file=std::ifstream(m_input_file.c_str());
  if( !m_file.is_open() ) return StatusCode::FAILURE;
  // Initialization terminated
  return StatusCode::SUCCESS;
}
StatusCode ReadHepEvtFromAscii::execute() {

  msg(MSG::INFO) << ">>> ReadHepEvtFromAscii from execute" << endmsg;
  
  McEventCollection* mcEvtColl;

  if ( m_sgSvc->contains<McEventCollection>(m_key) && m_sgSvc->retrieve(mcEvtColl, m_key).isSuccess() ) {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "found an McEventCollecion in store" << endmsg;
  } else {
    // McCollection doesn't exist. Create it (empty)
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "create new McEventCollecion in store" << endmsg;
    mcEvtColl = new McEventCollection;
    StatusCode status = m_sgSvc->record( mcEvtColl, m_key );
    if (status.isFailure()) {
      msg(MSG::ERROR) << "Could not record McEventCollection" << endmsg;
      return status;
    }
  }

  HepMC::GenEvent* evt = new HepMC::GenEvent(); 
  HepMC::HEPEVT_Wrapper::zero_everything();
  bool fileok=read_hepevt_event_header();
  for (int i=1; (i<=HepMC::HEPEVT_Wrapper::number_entries())&&fileok; i++) fileok=read_hepevt_particle(i);
  if (!fileok) return StatusCode::FAILURE;

#ifdef HEPMC3
  HepMC::HEPEVT_Wrapper::HEPEVT_to_GenEvent(evt);
#else
  HepMC::IO_HEPEVT hepio;
  hepio.set_print_inconsistency_errors(0);
  hepio.fill_next_event(evt);
#endif
 
  // Convert GeV to MeV 
#ifdef HEPMC3
  for ( auto p: evt->particles() ){
#else
  for ( auto ip = evt->particles_begin(); ip != evt->particles_end(); ++ip ){
      auto p=*ip;
#endif
	  HepMC::FourVector newMomentum(0.,0.,0.,0.);
	  newMomentum.setPx( p->momentum().px() * 1000. );
	  newMomentum.setPy( p->momentum().py() * 1000. );
	  newMomentum.setPz( p->momentum().pz() * 1000. );
	  newMomentum.setE( p->momentum().e() * 1000. );
	  p->set_momentum(newMomentum);

      }
  mcEvtColl->push_back(evt);
  
  // End of execution for each event
  return StatusCode::SUCCESS;
}
StatusCode ReadHepEvtFromAscii::finalize() {

  msg(MSG::INFO) << ">>> ReadHepEvtFromAscii from finalize" << endmsg;

  if(m_file.is_open()) m_file.close();
  // End of finalization step
  return StatusCode::SUCCESS;

}
