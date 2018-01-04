/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthIO/DumpMC.h"
#include "GaudiKernel/PhysicalConstants.h"
using namespace Gaudi::Units;
using namespace std;


DumpMC::DumpMC(const string& name, ISvcLocator* pSvcLocator)
  : GenBase(name, pSvcLocator)
{
  declareProperty("McEventOutputKey", m_keyout="GEN_EVENT");
  declareProperty("VerboseOutput", m_VerboseOutput=true);
  declareProperty("DeepCopy", m_DeepCopy=false);
  declareProperty("EtaPhi", m_EtaPhi=false);
}


StatusCode DumpMC::initialize() {
  CHECK(GenBase::initialize());
  if (m_mcEventKey == m_keyout && m_DeepCopy) {
    ATH_MSG_FATAL("Input and output MC event keys cannot be the same (" << m_mcEventKey << ")");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode DumpMC::execute() {
  if (m_DeepCopy) {
    McEventCollection* mcCollptra = new McEventCollection();
    // Fill the new McEventCollection with a copy of the initial HepMC::GenEvent
    for (McEventCollection::const_iterator evt = events_const()->begin(); evt != events_const()->end(); ++evt) {
      mcCollptra->push_back(new HepMC::GenEvent(*(*evt)));
    }
    // Loop over all events in McEventCollection
    for (McEventCollection::iterator evt = mcCollptra->begin(); evt != mcCollptra->end(); ++evt) {
      // Loop over the vertices of the event
      set<int> Lambdas;
      set<int> vtx_to_delete;
      for (HepMC::GenEvent::vertex_const_iterator vtx = (*evt)->vertices_begin(); vtx != (*evt)->vertices_end(); ++vtx) {
        // Loop over the particles that produced the vertex
        if ( vtx_to_delete.find((*vtx)->barcode()) == vtx_to_delete.end() &&
             (*vtx)->particles_in_const_begin() != (*vtx)->particles_in_const_end() ) {
          HepMC::GenVertex::particles_in_const_iterator part = (*vtx)->particles_in_const_begin();
          bool lambda_not_found = true;
          const int abspid = abs((*part)->pdg_id());
          do {
            if (abspid == 310 || abspid == 3122 || abspid == 3222 || abspid == 3112 ||
                abspid == 3322 || abspid == 3312 || abspid == 3334 ) {
              lambda_not_found = false;
              Lambdas.insert((*part)->barcode());
              vtx_to_delete.insert((*vtx)->barcode());
              // In case a lambda was found, store in vertices to be deleted all the vertices
              // that the products of Lambda created
              for (HepMC::GenVertex::vertex_iterator desc = (*vtx)->vertices_begin(HepMC::descendants);
                   desc != (*vtx)->vertices_end(HepMC::descendants); ++desc) {
                vtx_to_delete.insert((*desc)->barcode());
              }
            }
            ++part;
          } while (part != (*vtx)->particles_in_const_end() && lambda_not_found);
        }
      }

      // Set Lambda's status to stable
      for (set<int>::iterator l = Lambdas.begin(); l != Lambdas.end(); ++l) {
        HepMC::GenParticle* lam = (*evt)->barcode_to_particle(*l);
        lam->set_status(1);
      }
      // Delete all Lambda vertices from the event
      for (set<int>::iterator v = vtx_to_delete.begin(); v != vtx_to_delete.end(); ++v) {
        HepMC::GenVertex* vdel = (*evt)->barcode_to_vertex(*v);
        (*evt)->remove_vertex(vdel);
        delete vdel;
      }
    }

    if (evtStore()->record(mcCollptra, m_keyout).isFailure()){
      ATH_MSG_ERROR("Could not record McEventCollection");
      return StatusCode::FAILURE;
    }
  }

  // Loop over all events in McEventCollection
  for (McEventCollection::const_iterator itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
    //int g_id = (*itr)->signal_process_id();
    //GeneratorName_print(g_id);
    const HepMC::PdfInfo *pdfinfo = (*itr)->pdf_info();
    if (pdfinfo) {
      cout << "PdfInfo: "
                << pdfinfo->id1() << ", "
                << pdfinfo->id2() << ", "
                << pdfinfo->x1() << ", "
                << pdfinfo->x2() << ", "
                << pdfinfo->scalePDF() << ", "
                << pdfinfo->pdf1() << ", "
                << pdfinfo->pdf2() << ", "
                << pdfinfo->pdf_id1() << ", "
                << pdfinfo->pdf_id2()
                <<      endl;
    }

   const HepMC::HeavyIon *ion = (*itr)->heavy_ion();
    if (ion) {
      std::cout << std::endl;
      std::cout << "Heavy Ion: "
                << ion->Ncoll_hard() <<", "
                << ion->Npart_proj() <<" , "
                << ion->Npart_targ()<< ", "
                << ion->Ncoll()<< ", "
                << ion->spectator_neutrons() << ", "
                << ion->spectator_protons() << ", " 
                << ion->N_Nwounded_collisions() << ", "
                << ion->Nwounded_N_collisions() << ", "
                << ion->Nwounded_Nwounded_collisions() << ", "
                << ion->impact_parameter() << ", "
                << ion->event_plane_angle() << ", "
                << ion->eccentricity() << ", "
                << ion->sigma_inel_NN() 
                << std::endl;
                                               }

    if (m_VerboseOutput) {
      if (!m_EtaPhi) {
        (*itr)->print(); // standard HepMc dump
      } else { // sort particles by rapidity and then dump
        // Loop over all particles in the event and build up the grid
        const HepMC::GenEvent* genEvt = (*itr);
        for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr) {
          // if( (*pitr)->status() == 1 ){// stables only
          double rapid =(*pitr)->momentum().pseudoRapidity();
          double phi =  (*pitr)->momentum().phi(); //phi is in range -pi to pi
          double et=(*pitr)->momentum().perp();
          int p_stat=(*pitr)->status();
          int p_id = (*pitr)->pdg_id();
          cout << " eta = " << rapid<< "  Phi = " << phi << "   Et = " <<et/GeV << "  Status= " << p_stat << " PDG ID= "<< p_id << endl;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
