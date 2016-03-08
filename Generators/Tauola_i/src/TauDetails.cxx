//--------------------------------------------------------------------------
// File and Version Information:
//      TauDetails.cxx
//
// Description:
//      Interface to Tauola decay details for use in Tauola_i and Herwig_i
//
// Author List:
// Eric Torrence (ET), September 2009
//
// Copyright Information:
//
//------------------------------------------------------------------------

// Header
#include "Tauola_i/TauDetails.h"

// Other classes used
#include <iostream>
#include <vector>
#include "CLHEP/Geometry/Normal3D.h"

//--------------------------------------------------------------------------
TauDetails::TauDetails() : m_verbose(false) {
//TauDetails::TauDetails() : m_verbose(true) {
}

TauDetails::~TauDetails() {
}

//--------------------------------------------------------------------------
Fftaudet& TauDetails::fftaudet() {
  return m_fftaudet;
}

//--------------------------------------------------------------------------
// Clear details, but only if this is a new event.
// Workaround to allow HERWIG to call Tauola directly
bool
TauDetails::clearNewEvent(int eventNumber) {

  if (m_verbose) std::cout << "TauDetails::clearNewEvent called" << std::endl;

  int evtnum = this->fftaudet().evtnum();
  if (eventNumber == evtnum) {
    if (m_verbose) std::cout << "TauDetails::clearNewEvent has already seen event " 
			    << evtnum << std::endl;
    return false;
  }

  if (m_verbose) std::cout << "TauDetails::clearNewEvent called for " << eventNumber 
			  << " != " << evtnum << std::endl;
  this->clear();
  this->setEvent(eventNumber);
  return true;
}

int
TauDetails::setEvent(int eventNumber) {
  if (m_verbose) std::cout << "Taudetails::setEvent Event number now " 
			  << eventNumber << std::endl;
  return(this->fftaudet().evtnum() = eventNumber);
}

//--------------------------------------------------------------------------
// Conversion of Pythia HEPEVT -> HepMC preserves particle numbering, so we
// can use barcode to match taus.
//
void
TauDetails::savePythia(MsgStream& log, HepMC::GenEvent* evt) {

  log << MSG::DEBUG << " TauDetails saving..."  << endreq;

  // Protect against null pointer
  if (evt == 0) return;

  // Fortran counts from 1...
  for (size_t i=1; i<= this->size(); i++) {
    if (m_verbose)
    std::cout << ">>> " << i << " " 
    	      << this->fftaudet().itnp(i) << " " 
    	      << this->fftaudet().itjak(i) << " "
    	      << this->fftaudet().ftpolz(i)
    	      << std::endl;

    int barcode = this->fftaudet().itnp(i);
    HepMC::GenParticle* p = evt->barcode_to_particle(barcode);
    if (p == 0) {
      log << MSG::WARNING << " Couldn't find particle with barcode " << barcode << "!" << endreq;
    } else if (abs(p->pdg_id()) != 15) {
      log << MSG::WARNING << " Particle with barcode " << barcode << " has pdg_id = " 
	  << p->pdg_id() << " which is not a tau (=15)!" << endreq;
    } else {
      
      // OK, everything checks out, set polarization
      HepGeom::Normal3D<double> polv(this->fftaudet().ftpolx(i), 
		       this->fftaudet().ftpoly(i), 
		       this->fftaudet().ftpolz(i) );
      HepMC::Polarization pol(polv);
      p->set_polarization(pol);
      
      // Set decay vertex id (IJAK - decay mode)
      HepMC::GenVertex* v = p->end_vertex();
      if (v == 0) {
	log << MSG::WARNING << ">>> Couldn't find ending vertex for tau with barcode " 
	    << barcode << "!" << endreq;
      } else {
	if (m_verbose)
	std::cout << ">>> Set vertex type to " << this->fftaudet().itjak(i) << std::endl;
	v->set_id(this->fftaudet().itjak(i));
      }
    }
  }	

  //  this->clear();

}

//--------------------------------------------------------------------------
// Conversion of Herwig HEPEVT -> HepMC does not preserve particle numbering, 
// so we can not use barcode to match taus.  Use order in listing instead.
//
void
TauDetails::saveHerwig(MsgStream& log, HepMC::GenEvent* evt) {

  log << MSG::DEBUG << " TauDetails saving..."  << endreq;

  // Protect against null pointer
  if (evt == 0) return;

  // Find all taus in HepMC
  std::vector<int> tauBarcode;
  tauBarcode.clear();

  for ( HepMC::GenEvent::particle_const_iterator ip = evt->particles_begin();
	ip != evt->particles_end(); ++ip ) {

    // Find taus
    if (abs((*ip)->pdg_id()) != 15) continue;

    // Skip taus which don't decay, or decay to other taus (1 output particle)
    HepMC::GenVertex* dVertex = (*ip)->end_vertex();
    if (dVertex == 0) continue;

    if (dVertex->particles_out_size() <= 1) continue;
      
    if (m_verbose) (*ip)->print();
    tauBarcode.push_back((*ip)->barcode());
 
  }

  if (m_verbose) std::cout << "TauDetails::saveHerwig found " << tauBarcode.size()
			  << " taus in HepMC" << std::endl;

  // Now, check that our counts match
  if (this->size() != tauBarcode.size()) {
    log << MSG::WARNING << " TauDetails found " << tauBarcode.size()
	<< " taus in HepMC but expected " << this->size()
	<< " from HEPEVT!" << endmsg;
    return;
  }

  // Fortran counts from 1...
  for (size_t i=1; i<= this->size(); i++) {
    if (m_verbose)
    std::cout << ">>> " << i << " " 
    	      << this->fftaudet().itnp(i) << " " 
    	      << this->fftaudet().itjak(i) << " "
    	      << this->fftaudet().ftpolz(i)
    	      << std::endl;

    int barcode = tauBarcode[i-1];
    if (barcode == 0) {
      log << MSG::WARNING << " Couldn't find particle with barcode " << barcode << "!" << endreq;
      return;
    }

    HepMC::GenParticle* p = evt->barcode_to_particle(barcode);
    if (p == 0) {
      log << MSG::WARNING << " Couldn't find particle with barcode " 
	  << barcode << "!" << endreq;
    } else if (abs(p->pdg_id()) != 15) {
      log << MSG::WARNING << " Particle with barcode " << barcode
	  << " has pdg_id = " << p->pdg_id() << " which is not a tau (=15)!" << endreq;
    } else {
      
      // OK, everything checks out, set polarization
      HepGeom::Normal3D<double> polv(this->fftaudet().ftpolx(i), 
		       this->fftaudet().ftpoly(i), 
		       this->fftaudet().ftpolz(i) );
      HepMC::Polarization pol(polv);
      p->set_polarization(pol);
      
      // Set decay vertex id (IJAK - decay mode)
      HepMC::GenVertex* v = p->end_vertex();
      if (v == 0) {
	log << MSG::WARNING << ">>> Couldn't find ending vertex for tau with barcode " 
	    << barcode << "!" << endreq;
      } else {
	if (m_verbose)
	std::cout << ">>> Set vertex type to " << this->fftaudet().itjak(i) << std::endl;
	v->set_id(this->fftaudet().itjak(i));
      }

      if (m_verbose) {
	p->print();
	v->print();
      }

    }
  }

  //  this->clear();  

}
