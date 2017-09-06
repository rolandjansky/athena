/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"
#include "AthLinks/ElementLink.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEventAuxContainer.h"

#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthPileupEventAuxContainer.h"


#include "xAODTruthReader.h"

using namespace std;

namespace xAODReader {


  xAODTruthReader::xAODTruthReader(const string& name, ISvcLocator* svcLoc)
    : AthAlgorithm(name, svcLoc)
  {
    /// @todo Provide these names centrally in a Python module and remove these hard-coded versions?
    declareProperty( "xAODTruthEventContainerName", m_xaodTruthEventContainerName="TruthEvents" );
    declareProperty( "xAODTruthPileupEventContainerName", m_xaodTruthPUEventContainerName="TruthPileupEvents" );
  }


  StatusCode xAODTruthReader::initialize() {
    ATH_MSG_INFO("Initializing; package version = " << PACKAGE_VERSION );
    ATH_MSG_INFO("xAOD TruthEventContainer name = " << m_xaodTruthEventContainerName );
    ATH_MSG_INFO("xAOD TruthPileupEventContainer name = " << m_xaodTruthPUEventContainerName );
    return StatusCode::SUCCESS;
  }


  StatusCode xAODTruthReader::execute() {
    // Retrieve the xAOD truth:
    /// @todo Can the main truth event be a singleton, not a container?
    const xAOD::TruthEventContainer* xTruthEventContainer = NULL;
    CHECK( evtStore()->retrieve( xTruthEventContainer, m_xaodTruthEventContainerName));
    const xAOD::TruthPileupEventContainer* xTruthPUEventContainer = NULL;
    //CHECK( evtStore()->retrieve( xTruthPUEventContainer, m_xaodTruthPUEventContainerName));

    ATH_MSG_INFO("Number of signal events in this Athena event: " << xTruthEventContainer->size());
    if (xTruthPUEventContainer) ATH_MSG_INFO("Number of pile-up events in this Athena event: " << xTruthPUEventContainer->size());

    // Signal process loop
    ATH_MSG_INFO("Printing signal event...");
    for (const xAOD::TruthEvent* evt : *xTruthEventContainer) {
      cout << endl << endl;

      // Print PDF info if found
      /*xAOD::TruthEvent::PdfInfo pdfi = evt->pdfInfo();
      if (pdfi.valid()) {
        cout << "PDF info: PIDs " << pdfi.pdgId1 << ", " << pdfi.pdgId2 << " with x = "
             << pdfi.x1 << ", " << pdfi.x2 << " & Q = " << pdfi.Q << " => xf = "
             << pdfi.xf1 << ", " << pdfi.xf2 << " with PDFs "
             << pdfi.pdfId1 << " and " << pdfi.pdfId2 << endl;
      }*/

      // Print hard-scattering info
      const xAOD::TruthVertex* vtx = evt->signalProcessVertex();
      ATH_MSG_INFO("Signal process vertex: " << vtx);
      if (vtx)
	ATH_MSG_INFO("Poistion = (" << vtx->x() << ", " << vtx->y() << ", " << vtx->z() << ")");
      else
	ATH_MSG_INFO("Position n.a.");
      // Print the event particle/vtx contents
      printEvent(evt);

    }

    // Pile-up loop
    ATH_MSG_INFO("Printing pileup events...");
    if (xTruthPUEventContainer) {
      for (const xAOD::TruthPileupEvent* evt : *xTruthPUEventContainer) {
	cout << endl << endl;
	printEvent(evt);
      } 
    }

    return StatusCode::SUCCESS;
  }


  // Print method for event - mimics the HepMC dump.
  // Vertex print method called within here
  void xAODTruthReader::printEvent(const xAOD::TruthEventBase* event) {
    cout << "--------------------------------------------------------------------------------\n";
    cout << "GenEvent: #" << "NNN" << "\n";
    /*<< " ID=" << signal_process_id() << " SignalProcessGenVertex Barcode: "
      << ( signal_process_vertex() ? signal_process_vertex()->barcode() : 0 ) << "\n";*/
    cout << " Entries this event: " << event->nTruthVertices() << " vertices, " << event->nTruthParticles() << " particles.\n";
    // Beam particles
    /*pair<const xAOD::TruthParticle*,const xAOD::TruthParticle*> beamParticles = event->beamParticles();
    if ( beamParticles.first && beamParticles.second ) {
      cout << " Beam particle barcodes: " << beamParticles.first->barcode() << " " << beamParticles.second->barcode() << " \n";
    } else {
      cout << " Beam particles are not defined.\n";
    }
    // Weights
    const vector<float> weights = event->weights();
    cout << weights.size() << " weights = ";
    for (const float& w : weights) cout << w << " ";
    cout << "\n";*/
    // Particles and vertices
    cout << "                                    GenParticle Legend\n";
    cout << "        Barcode   PDG ID      ( Px,       Py,       Pz,     E ) Stat  DecayVtx\n";
    cout << "--------------------------------------------------------------------------------\n";
    for (unsigned int iv = 0; iv < event->nTruthVertices(); ++iv) {
      printVertex(event->truthVertex(iv));
    }
    cout << "--------------------------------------------------------------------------------\n";
  }

  // Print method for vertex - mimics the HepMC dump.
  // Particle print method called within here
  void xAODTruthReader::printVertex(const xAOD::TruthVertex* vertex) {
  std::ios::fmtflags f( cout.flags() ); 
    cout << "TruthVertex:";
    if (vertex->barcode() != 0) {
      if (vertex->x() != 0.0 && vertex->y() != 0.0 && vertex->z() != 0.0) {
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
        cout << vertex->x() << ",";
        cout.width(9);
        cout.precision(2);
        cout << vertex->y() << ",";
        cout.width(9);
        cout.precision(2);
        cout << vertex->z() << ",";
        cout.width(9);
        cout.precision(2);
        cout << vertex->t();
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
      if (vertex->x() != 0.0 && vertex->y() != 0.0 && vertex->z() != 0.0) {
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
        cout << vertex->x();
        cout.width(9);
        cout.precision(2);
        cout << vertex->y();
        cout.width(9);
        cout.precision(2);
        cout << vertex->z();
        cout.width(9);
        cout.precision(2);
        cout << vertex->t();
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
    for (unsigned int iPIn = 0; iPIn<vertex->nIncomingParticles(); ++iPIn) {
      if ( iPIn == 0 ) {
        cout << " I: ";
        cout.width(2);
        cout << vertex->nIncomingParticles();
      } else cout << "      ";
      printParticle(vertex->incomingParticle(iPIn));
    }
    for (unsigned int iPOut = 0; iPOut<vertex->nOutgoingParticles(); ++iPOut) {
      if ( iPOut == 0 ) {
        cout << " O: ";
        cout.width(2);
        cout << vertex->nOutgoingParticles();
      } else cout << "      ";
      printParticle(vertex->outgoingParticle(iPOut));
    }
  
    cout.flags(f); 
  }


  // Print method for particle - mimics the HepMC dump.
  void xAODTruthReader::printParticle(const xAOD::TruthParticle* particle) {
    std::ios::fmtflags f( cout.flags() ); 
    cout << " ";
    cout.width(9);
    cout << particle->barcode();
    cout.width(9);
    cout << particle->pdgId() << " ";
    cout.width(9);
    cout.precision(2);
    cout.setf(ios::scientific, ios::floatfield);
    cout.setf(ios_base::showpos);
    cout << particle->px() << ",";
    cout.width(9);
    cout.precision(2);
    cout << particle->py() << ",";
    cout.width(9);
    cout.precision(2);
    cout << particle->pz() << ",";
    cout.width(9);
    cout.precision(2);
    cout << particle->e() << " ";
    cout.setf(ios::fmtflags(0), ios::floatfield);
    cout.unsetf(ios_base::showpos);
    if ( particle->hasDecayVtx() ) {
      if ( particle->decayVtx()->barcode()!=0 ) {
        cout.width(3);
        cout << particle->status() << " ";
        cout.width(9);
        cout << particle->decayVtx()->barcode();
      }
    } else {
      cout.width(3);
      cout << particle->status();
    }
    cout << endl;
    cout.flags(f); 
  }


} // namespace xAODReader
