/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TopCKMFilter.h"

TopCKMFilter::TopCKMFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("PDGChild",m_PDGChild);
  declareProperty("PtMinChild",m_PtMinChild = 0.);
  declareProperty("EtaRangeChild",m_EtaRangeChild = 10.0);
}


StatusCode TopCKMFilter::filterInitialize() {
  if (m_PDGChild.size()==0) ATH_MSG_ERROR("PDGChild[] not set ");
  for (int i = 0; i < int(m_PDGChild.size()); i++) {
    ATH_MSG_INFO("PDGChild["<<i<<"] = " << m_PDGChild[i]);
  }
  ATH_MSG_INFO("PtMinChild = " << m_PtMinChild);
  ATH_MSG_INFO("EtaRangeChild = " << m_EtaRangeChild);
  return StatusCode::SUCCESS;
}


StatusCode TopCKMFilter::filterEvent() {
  ATH_MSG_INFO(" TopCKMFilter filtering ");
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent * genEvt = (*itr);
#ifdef HEPMC3
    // Get the number of parents on the event (ttbar --> 2 tops!)
    std::vector<HepMC::ConstGenParticlePtr> Parents;
    for (auto partItr: *genEvt) if (std::abs(partItr->pdg_id()) == 6) Parents.push_back(partItr);
    if (Parents.size() != 2) continue;
      // Check that the first and second parents have the corresponding childs
      if (!Parents.at(0) || !Parents.at(1)) continue; 
      int Wbosons[2]={0,0};
      int quarks[2]={0,0};
      bool isTau[2] ={false,false};
      for (int i=0;i<2;i++)
      {
      for (auto Child: *(Parents.at(i)->end_vertex()))
      {
      if ((std::abs(Child->pdg_id()) == m_PDGChild[0] || std::abs(Child->pdg_id()) == m_PDGChild[1])
         && (Child->momentum().perp() > m_PtMinChild)
         && (std::abs(Child->momentum().eta()) < m_EtaRangeChild)) 
         quarks[i]=Child->pdg_id();
      if (std::abs(Child->pdg_id()) == 24) 
      {
       Wbosons[i]=Child->pdg_id();
       if (!Child->end_vertex()) continue;
       for (auto gChild: *(Child->end_vertex())) if (std::abs(gChild->pdg_id()) == 15) isTau[i] = true;
       }
       }
       }
       if ( quarks[0]!=0 && quarks[1]!=0  && Wbosons[0]!=0 && Wbosons[1]!=0 && !isTau[0] && !isTau[1] &&  (std::abs(quarks[0]) != std::abs(quarks[1])) ) 
       {
          return StatusCode::SUCCESS;
       }
#else
    HepMC::GenEvent::particle_const_iterator part1 = genEvt->particles_begin();
    HepMC::GenEvent::particle_const_iterator partItr = part1;
    HepMC::GenEvent::particle_const_iterator partE = genEvt->particles_end();

    // Get the number of parents on the event (ttbar --> 2 tops!)
    int nParents = 0;
    for (partItr = part1; partItr != partE; ++partItr) {
      if (std::abs((*partItr)->pdg_id()) == 6) ++nParents;
    }

    if (nParents == 2) {
      const HepMC::GenParticle * firstParent = 0;
      const HepMC::GenParticle * secondParent = 0;
      for (partItr = part1; partItr != partE; ++partItr) {
        if (std::abs((*partItr)->pdg_id()) == 6) firstParent = (*partItr);
      }
      for (partItr = part1; partItr != partE; ++partItr) {
        if (std::abs((*partItr)->pdg_id()) == 6 && (*partItr) != firstParent) secondParent = (*partItr);
      }

      // Check that the first and second parents have the corresponding childs
      if (firstParent && secondParent) {
        HepMC::GenVertex * firstVertex = firstParent->end_vertex();
        HepMC::GenVertex * secondVertex = secondParent->end_vertex();

        HepMC::GenVertex::particle_iterator firstChild1 = firstVertex->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator firstChildItr = firstChild1;
        HepMC::GenVertex::particle_iterator firstChildE = firstVertex->particles_end(HepMC::children);

        HepMC::GenVertex::particle_iterator secondChild1 = secondVertex->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator secondChildItr = secondChild1;
        HepMC::GenVertex::particle_iterator secondChildE = secondVertex->particles_end(HepMC::children);

        // Check the children
        bool isOK1 = 0;
        bool isOK2 = 0;
        HepMC::GenParticle * quark1 = 0;
        HepMC::GenParticle * quark2 = 0;
        HepMC::GenParticle * wBoson1 = 0;
        HepMC::GenParticle * wBoson2 = 0;

        for (firstChildItr = firstChild1; firstChildItr != firstChildE; ++firstChildItr) {
          if ((std::abs((*firstChildItr)->pdg_id()) == m_PDGChild[0] || std::abs((*firstChildItr)->pdg_id()) == m_PDGChild[1])
              && (*firstChildItr)->momentum().perp() > m_PtMinChild
              && std::abs((*firstChildItr)->momentum().eta()) < m_EtaRangeChild) {
            isOK1 = 1;
            quark1 = (*firstChildItr);
          }
          if (std::abs((*firstChildItr)->pdg_id()) == 24) wBoson1 = (*firstChildItr);
        }

        for (secondChildItr = secondChild1; secondChildItr != secondChildE; ++secondChildItr) {
          if ((std::abs((*secondChildItr)->pdg_id()) == m_PDGChild[0] || std::abs((*secondChildItr)->pdg_id()) == m_PDGChild[1])
              && (*secondChildItr)->momentum().perp() > m_PtMinChild
              && std::abs((*secondChildItr)->momentum().eta()) < m_EtaRangeChild){
            isOK2 = 1;
            quark2 = (*secondChildItr);
          }
          if (std::abs((*secondChildItr)->pdg_id()) == 24) wBoson2 = (*secondChildItr);
        }

        // Now avoid the decay W --> tau nu
        bool isTau1 = 0;
        bool isTau2 = 0;
        HepMC::GenVertex * wVertex1 = wBoson1->end_vertex();
        HepMC::GenVertex * wVertex2 = wBoson2->end_vertex();

        HepMC::GenVertex::particle_iterator w1Son1, w1SonItr, w1SonE;
        HepMC::GenVertex::particle_iterator w2Son1, w2SonItr, w2SonE;

        w1Son1 = wVertex1->particles_begin(HepMC::children);
        w1SonE = wVertex1->particles_end(HepMC::children);
        w2Son1 = wVertex2->particles_begin(HepMC::children);
        w2SonE = wVertex2->particles_end(HepMC::children);

        for (w1SonItr = w1Son1; w1SonItr != w1SonE; ++w1SonItr) {
          if (std::abs((*w1SonItr)->pdg_id()) == 15) isTau1 = 1;
        }
        for (w2SonItr = w2Son1; w2SonItr != w2SonE; ++w2SonItr) {
          if (std::abs((*w2SonItr)->pdg_id()) == 15) isTau2 = 1;
        }
        if (isOK1 && isOK2 && !isTau1 && !isTau2 && std::abs(quark1->pdg_id()) != std::abs(quark2->pdg_id())) {
          return StatusCode::SUCCESS;
        }
      }
    }
#endif
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
