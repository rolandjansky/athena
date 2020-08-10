/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DirectPhotonFilter.h"


DirectPhotonFilter::DirectPhotonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("Ptmin",m_Ptmin = 10000.);
  declareProperty("Ptmax",m_Ptmax = 100000000.);
  declareProperty("Etacut", m_EtaRange = 2.50);
  declareProperty("NPhotons", m_NPhotons = 1);
  declareProperty("AllowSUSYDecay",m_AllowSUSYDecay = false);

  // Backward compatibility aliases
  declareProperty("Ptcut", m_Ptmin = 10000.);
}


StatusCode DirectPhotonFilter::filterEvent() {
  int NPhotons = 0;
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    ATH_MSG_DEBUG("----->>> Process : " << HepMC::signal_process_id(genEvt));
#ifdef HEPMC3
    for (auto pitr: genEvt->particles()) {
      if (pitr->pdg_id() == 22) {
	if( (pitr->momentum().perp() >= m_Ptmin) && (pitr->momentum().perp() <= m_Ptmax) &&
	    std::abs(pitr->momentum().pseudoRapidity()) <= m_EtaRange){
	  ATH_MSG_DEBUG("Generic photon found with status = " << pitr->status() << " " << HepMC::barcode(pitr));
          auto CandProdVertex = pitr->production_vertex();
          ATH_MSG_DEBUG("Candidate production vertex ID      = " << CandProdVertex->id());
          ATH_MSG_DEBUG("Candidate production vertex barcode = " << HepMC::barcode(CandProdVertex));
          for (auto thisChild: pitr->production_vertex()->particles_out()) {
            ATH_MSG_DEBUG("Looping on Production (children) vertex : " << thisChild->pdg_id() << "  " << HepMC::barcode(thisChild));
          }
          for (auto thisChild1: pitr->production_vertex()->particles_in()) {
            ATH_MSG_DEBUG("Looping on Production (parents) vertex : " << thisChild1->pdg_id() << "  " << HepMC::barcode(thisChild1));
          }
        }
      }
      // Check for a photon with desired kinematics
      if (pitr->pdg_id() == 22) { 
	if( pitr->status() == 1 &&
	    (pitr->momentum().perp() >= m_Ptmin) && (pitr->momentum().perp() <= m_Ptmax) &&
	    std::abs(pitr->momentum().pseudoRapidity()) <= m_EtaRange){
          // The following lines are for cross checking purpose when using different generators
          auto CandProdVertex = pitr->production_vertex();
          ATH_MSG_DEBUG("Candidate production vertex ID      = " << CandProdVertex->id());
          ATH_MSG_DEBUG("Candidate production vertex barcode = " << HepMC::barcode(CandProdVertex));
          for(auto thisChild: pitr->production_vertex()->particles_out() ) {
            ATH_MSG_DEBUG("Looping on Production (children) vertex : " << thisChild->pdg_id() << "  " << HepMC::barcode(thisChild));
          }
          ///////////////////////////////////////////////////////////////////////////////////////////////
          //
          // 1) once a status = 1 photon is found check where it comes from : loop on incoming particle
          //
          // Pythia : - a photon from HP comes out with status=3 and then turned into a status = 1 photon
          //          - coming from the status-3 photon.
          //        : - a photon from brem comes out directly with a status=1
          // Herwig : - NOT CHECKED on HP !
          //            - a photon from brem comes out with different status but always turned into a
          //            status=1 photon at the end coming from a photon with a different status.
          //
          // So requiring a status=1 photon coming from q/CLHEP::g saves brem photons in Pythia. Requiring
          // a status=1 photon coming from a photon should save HP photons in Pythia and Brem in
          // Herwig
          //
          // 2) the second option is to ask for a status = 1 photon which doesn't come from a PDG>100
          // particle. In this way we should veto 'background photon'
          //
          //////////////////////////////////////////////////////////////////////////////////////////////

          bool fromHadron(false);
          for ( auto thisChild1:  pitr->production_vertex()->particles_in()) {
            int pdgindex =  std::abs(thisChild1->pdg_id());
            ATH_MSG_DEBUG("Looping on Production (parents) vertex : " << thisChild1->pdg_id() << "  " << HepMC::barcode(thisChild1));
            if (pdgindex > 100) {
              fromHadron = true;
              if (m_AllowSUSYDecay && ( (pdgindex > 1000000 && pdgindex < 1000040) || (pdgindex > 2000000 && pdgindex < 2000016) ) ) fromHadron = false;
              ATH_MSG_DEBUG("event kept");
            }
          }
          if (!fromHadron) NPhotons++;

        } // if( (*pitr)->status()==1 && ...
      } 
   }
#else
    for (HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr) {
      if (((*pitr)->pdg_id() == 22)) {
	if( ((*pitr)->momentum().perp() >= m_Ptmin) && ((*pitr)->momentum().perp() <= m_Ptmax) &&
	    std::abs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange){
	  ATH_MSG_DEBUG("Generic photon found with status = " << (*pitr)->status() << " " << (*pitr)->barcode());

          HepMC::GenVertex* CandProdVertex = (*pitr)->production_vertex();
          ATH_MSG_DEBUG("Candidate production vertex ID      = " << CandProdVertex->id());
          ATH_MSG_DEBUG("Candidate production vertex barcode = " << CandProdVertex->barcode());

          HepMC::GenVertex::particle_iterator firstChild = (*pitr)->production_vertex()->particles_begin(HepMC::children);
          HepMC::GenVertex::particle_iterator endChild   = (*pitr)->production_vertex()->particles_end(HepMC::children);
          HepMC::GenVertex::particle_iterator thisChild = firstChild;
          for (; thisChild != endChild; ++thisChild) {
            ATH_MSG_DEBUG("Looping on Production (children) vertex : " << (*thisChild)->pdg_id() << "  " << (*thisChild)->barcode());
          }

          HepMC::GenVertex::particle_iterator firstChild1 = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
          HepMC::GenVertex::particle_iterator endChild1   = (*pitr)->production_vertex()->particles_end(HepMC::parents);
          HepMC::GenVertex::particle_iterator thisChild1 = firstChild1;
          for (; thisChild1 != endChild1; ++thisChild1) {
            ATH_MSG_DEBUG("Looping on Production (parents) vertex : " << (*thisChild1)->pdg_id() << "  " << (*thisChild1)->barcode());
          }
        }
      }

      // Check for a photon with desired kinematics
      if ( ((*pitr)->pdg_id() == 22) ){ 
	if( (*pitr)->status() == 1 &&
	    ((*pitr)->momentum().perp() >= m_Ptmin) && ((*pitr)->momentum().perp() <= m_Ptmax) &&
	    std::abs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange){

          // The following lines are for cross checking purpose when using different generators
          HepMC::GenVertex* CandProdVertex = (*pitr)->production_vertex();
          ATH_MSG_DEBUG("Candidate production vertex ID      = " << CandProdVertex->id());
          ATH_MSG_DEBUG("Candidate production vertex barcode = " << CandProdVertex->barcode());

          HepMC::GenVertex::particle_iterator firstChild = (*pitr)->production_vertex()->particles_begin(HepMC::children);
          HepMC::GenVertex::particle_iterator endChild   = (*pitr)->production_vertex()->particles_end(HepMC::children);
          HepMC::GenVertex::particle_iterator thisChild = firstChild;
          for(; thisChild != endChild; ++thisChild) {
            ATH_MSG_DEBUG("Looping on Production (children) vertex : " << (*thisChild)->pdg_id() << "  " << (*thisChild)->barcode());
          }

          ///////////////////////////////////////////////////////////////////////////////////////////////
          //
          // 1) once a status = 1 photon is found check where it comes from : loop on incoming particle
          //
          // Pythia : - a photon from HP comes out with status=3 and then turned into a status = 1 photon
          //          - coming from the status-3 photon.
          //        : - a photon from brem comes out directly with a status=1
          // Herwig : - NOT CHECKED on HP !
          //            - a photon from brem comes out with different status but always turned into a
          //            status=1 photon at the end coming from a photon with a different status.
          //
          // So requiring a status=1 photon coming from q/CLHEP::g saves brem photons in Pythia. Requiring
          // a status=1 photon coming from a photon should save HP photons in Pythia and Brem in
          // Herwig
          //
          // 2) the second option is to ask for a status = 1 photon which doesn't come from a PDG>100
          // particle. In this way we should veto 'background photon'
          //
          //////////////////////////////////////////////////////////////////////////////////////////////

          HepMC::GenVertex::particle_iterator firstChild1 = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
          HepMC::GenVertex::particle_iterator endChild1   = (*pitr)->production_vertex()->particles_end(HepMC::parents);
          HepMC::GenVertex::particle_iterator thisChild1 = firstChild1;

          bool fromHadron(false);
          for (; thisChild1 != endChild1; ++thisChild1) {
            int pdgindex =  abs((*thisChild1)->pdg_id());
            ATH_MSG_DEBUG("Looping on Production (parents) vertex : " << (*thisChild1)->pdg_id() << "  " << (*thisChild1)->barcode());
            if (pdgindex > 100) {
              fromHadron = true;
              if (m_AllowSUSYDecay && ( (pdgindex > 1000000 && pdgindex < 1000040) || (pdgindex > 2000000 && pdgindex < 2000016) ) ) fromHadron = false;
              ATH_MSG_DEBUG("event kept");
            }
          }
          if (!fromHadron) NPhotons++;

        } // if( (*pitr)->status()==1 && ...
      } // if( ((*pitr)->pdg_id() == 22) )
    }
#endif
  }

  if (NPhotons >= m_NPhotons) return StatusCode::SUCCESS;
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
