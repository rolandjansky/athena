/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "AthLinks/ElementLink.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMCTruthReader.h"

using namespace std;

HepMCTruthReader::HepMCTruthReader(const string& name, ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc)
{
  /// @todo Provide these names centrally in a Python module and remove these hard-coded versions?
  declareProperty( "HepMCContainerName", m_hepMCContainerName="GEN_EVENT" );
}


StatusCode HepMCTruthReader::initialize() {
  ATH_MSG_INFO("Initializing; package version = " << PACKAGE_VERSION );
  ATH_MSG_INFO("HepMC container name = " << m_hepMCContainerName );
  return StatusCode::SUCCESS;
}


StatusCode HepMCTruthReader::execute() {

  // Retrieve the HepMC truth:
  const McEventCollection* mcColl = 0;
  CHECK( evtStore()->retrieve( mcColl, m_hepMCContainerName ) );
  ATH_MSG_INFO("Number of pile-up events in this Athena event: " << mcColl->size()-1);

  // Loop over events
  for (unsigned int cntr = 0; cntr < mcColl->size(); ++cntr) {
    const HepMC::GenEvent* genEvt = (*mcColl)[cntr]; 

    // Print PDF info if found
    /*xAOD::TruthEvent::PdfInfo pdfi = evt->pdfInfo();
    if (pdfi.valid()) {
      cout << "PDF info: PIDs " << pdfi.pdgId1 << ", " << pdfi.pdgId2 << " with x = "
           << pdfi.x1 << ", " << pdfi.x2 << " & Q = " << pdfi.Q << " => xf = "
           << pdfi.xf1 << ", " << pdfi.xf2 << " with PDFs "
           << pdfi.pdfId1 << " and " << pdfi.pdfId2 << endl;
    }*/

    // Print the event particle/vtx contents
    if (cntr==0) ATH_MSG_INFO("Printing signal event...");
    if (cntr>0) ATH_MSG_INFO("Printing pileup events...");  

    if (cntr==0) {
      HepMC::GenVertex* signalProcessVtx = genEvt->signal_process_vertex();
      ATH_MSG_INFO("Signal process vertex position: (" << signalProcessVtx->position().x() 
		   << ", " << signalProcessVtx->position().y() << ", " << signalProcessVtx->position().z() << "). Pointer: " 
		   << signalProcessVtx);
    }

    printEvent(genEvt);

  }

  return StatusCode::SUCCESS;
}


// Print method for event - mimics the HepMC dump.
// Vertex print method called within here
void HepMCTruthReader::printEvent(const HepMC::GenEvent* event) {
  cout << "--------------------------------------------------------------------------------\n";
  cout << "GenEvent: #" << "NNN" << "\n";
  cout << " Entries this event: " << event->vertices_size() << " vertices, " << event->particles_size() << " particles.\n";
  // Particles and vertices
  cout << "                                    GenParticle Legend\n";
  cout << "        Barcode   PDG ID      ( Px,       Py,       Pz,     E ) Stat  DecayVtx\n";
  cout << "--------------------------------------------------------------------------------\n";
  for (HepMC::GenEvent::vertex_const_iterator iv = event->vertices_begin(); iv != event->vertices_end(); ++iv) {
    printVertex(*iv);
  }
  cout << "--------------------------------------------------------------------------------\n";
}

// Print method for vertex - mimics the HepMC dump.
// Particle print method called within here
void HepMCTruthReader::printVertex(const HepMC::GenVertex* vertex) {
  std::ios::fmtflags f( cout.flags() ); 
  cout << "GenVertex (" << vertex << "):";
  if (vertex->barcode() != 0) {
    if (vertex->position().x() != 0.0 && vertex->position().y() != 0.0 && vertex->position().z() != 0.0) {
      cout.width(9);
      cout << vertex->barcode();
      cout << " ID:";
      cout.width(5);
      cout << vertex->id();
      cout << " (X,cT)=";
      cout.width(9);
      cout.precision(2);
      cout.setf(ios::scientific, ios::floatfield);
      cout.setf(ios_base::showpos);
      cout << vertex->position().x() << ",";
      cout.width(9);
      cout.precision(2);
      cout << vertex->position().y() << ",";
      cout.width(9);
      cout.precision(2);
      cout << vertex->position().z() << ",";
      cout.width(9);
      cout.precision(2);
      cout << vertex->position().t();
      cout.setf(ios::fmtflags(0), ios::floatfield);
      cout.unsetf(ios_base::showpos);
      cout << endl;
    } else {
      cout.width(9);
      cout << vertex->barcode();
      cout << " ID:";
      cout.width(5);
      cout << vertex->id();
      cout << " (X,cT): 0";
      cout << endl;
    }
  } else {
    // If the vertex doesn't have a unique barcode assigned, then
    //  we print its memory address instead... so that the
    //  print out gives us a unique tag for the particle.
    if (vertex->position().x() != 0.0 && vertex->position().y() != 0.0 && vertex->position().z() != 0.0) {
      cout.width(9);
      cout << (void*)vertex;
      cout << " ID:";
      cout.width(5);
      cout << vertex->id();
      cout << " (X,cT)=";
      cout.width(9);
      cout.precision(2);
      cout.setf(ios::scientific, ios::floatfield);
      cout.setf(ios_base::showpos);
      cout << vertex->position().x();
      cout.width(9);
      cout.precision(2);
      cout << vertex->position().y();
      cout.width(9);
      cout.precision(2);
      cout << vertex->position().z();
      cout.width(9);
      cout.precision(2);
      cout << vertex->position().t();
      cout.setf(ios::fmtflags(0), ios::floatfield);
      cout.unsetf(ios_base::showpos);
      cout << endl;
    } else {
      cout.width(9);
      cout << (void*)vertex;
      cout << " ID:";
      cout.width(5);
      cout << vertex->id();
      cout << " (X,cT):0";
      cout << endl;
    }
  }
  // // Print the weights if there are any
  // if (vertex->weights().size() != 0) {
  //   cout << vertex->weights().size() << " weights =";
  //   for (vector<float>::const_iterator wgt = vertex->weights().begin();
  //        wgt != vertex->weights().end(); ++wgt) { cout << *wgt << " "; }
  //   cout << endl;
  // }
  // Print out all the incoming, then outgoing particles
  for (HepMC::GenVertex::particles_in_const_iterator iPIn = vertex->particles_in_const_begin();
       iPIn != vertex->particles_in_const_end(); ++iPIn) {       
    if ( iPIn == vertex->particles_in_const_begin() ) {
      cout << " I: ";
      cout.width(2);
      cout << vertex->particles_in_size();
    } else cout << "      ";
    printParticle(*iPIn);
  }
  for (HepMC::GenVertex::particles_out_const_iterator iPOut = vertex->particles_out_const_begin();
       iPOut != vertex->particles_out_const_end(); ++iPOut) {
    if ( iPOut == vertex->particles_out_const_begin() ) {
      cout << " O: ";
      cout.width(2);
      cout << vertex->particles_out_size();
    } else cout << "      ";
    printParticle(*iPOut);
  }

  cout.flags(f); 
}


// Print method for particle - mimics the HepMC dump.
void HepMCTruthReader::printParticle(const HepMC::GenParticle* particle) {
  std::ios::fmtflags f( cout.flags() ); 
  cout << " ";
  cout.width(9);
  cout << particle->barcode();
  cout.width(9);
  cout << particle->pdg_id() << " ";
  cout.width(9);
  cout.precision(2);
  cout.setf(ios::scientific, ios::floatfield);
  cout.setf(ios_base::showpos);
  cout << particle->momentum().px() << ",";
  cout.width(9);
  cout.precision(2);
  cout << particle->momentum().py() << ",";
  cout.width(9);
  cout.precision(2);
  cout << particle->momentum().pz() << ",";
  cout.width(9);
  cout.precision(2);
  cout << particle->momentum().e() << " ";
  cout.setf(ios::fmtflags(0), ios::floatfield);
  cout.unsetf(ios_base::showpos);
  if ( particle->has_decayed() ) {
    if ( particle->end_vertex()->barcode()!=0 ) {
      cout.width(3);
      cout << particle->status() << " ";
      cout.width(9);
      cout << particle->end_vertex()->barcode();
    }
  } else {
    cout.width(3);
    cout << particle->status();
  }
  cout << endl;
  cout.flags(f); 
}


