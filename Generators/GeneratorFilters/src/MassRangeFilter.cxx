/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/MassRangeFilter.h"

MassRangeFilter::MassRangeFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("PtCut",m_Ptmin = 10000.);
  declareProperty("PtCut2",m_Ptmin2 = 10000.);
  declareProperty("EtaCut",m_EtaRange = 10.);
  declareProperty("EtaCut2",m_EtaRange2 = 10.);
  declareProperty("InvMassMin",m_InvMassMin=0.);
  declareProperty("InvMassMax",m_InvMassMax=14000000.);
  declareProperty("PartId",m_PartId=13);
  declareProperty("PartId2",m_PartId2=13);
  declareProperty("PartStatus",m_PartStatus=1);
}


StatusCode MassRangeFilter::filterInitialize() {
  ATH_MSG_INFO(" PtCut      " << m_Ptmin <<
               " PtCut2     " << m_Ptmin2 <<
               " EtaCut		" << m_EtaRange <<
               " EtaCut2	" << m_EtaRange2 <<
               " InvMassMin " << m_InvMassMin <<
               " InvMassMax " << m_InvMassMax <<
               " PartId		" << m_PartId <<
               " PartId2	" << m_PartId2 <<
               " PartStatus " << m_PartStatus);
  return StatusCode::SUCCESS;
}


StatusCode MassRangeFilter::filterEvent() {
  double invMassMax = 0.;
  bool samePDGID = (std::abs(m_PartId) == std::abs(m_PartId2));
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;
    int n = 0;
    auto genEvt_particles_begin=HepMC::begin(*genEvt);
    auto genEvt_particles_end=HepMC::end(*genEvt);
    for ( auto pitr1 = genEvt_particles_begin; pitr1 != genEvt_particles_end; ++pitr1) {
      n++;
      if ((*pitr1)->status() != m_PartStatus ) continue; //status of the particle
      if (std::abs((*pitr1)->pdg_id()) != std::abs(m_PartId) ) continue; //PDG ID selection
      if ((*pitr1)->momentum().perp() < m_Ptmin ) continue; // pT cut
      if (std::abs((*pitr1)->momentum().pseudoRapidity()) > m_EtaRange) continue; //eta cut
      auto pitr2 = genEvt_particles_begin;
      if (samePDGID) {
        pitr2 = pitr1;
        pitr2++;
      }
      for (; pitr2 != genEvt_particles_end; ++pitr2) {
        if (pitr1 == pitr2) continue; //if the pointers are the same
        if ((*pitr2)->status() != m_PartStatus) continue;  //status of the particle
        if (std::abs((*pitr2)->pdg_id()) != std::abs(m_PartId2)) continue; //PDG ID selection
        if ((*pitr2)->momentum().perp() < m_Ptmin2) continue; // pT cut
        if (std::abs((*pitr2)->momentum().pseudoRapidity()) > m_EtaRange2) continue;//eta cut

        HepMC::FourVector vec((*pitr1)->momentum().px() + (*pitr2)->momentum().px(),
                              (*pitr1)->momentum().py() + (*pitr2)->momentum().py(),
                              (*pitr1)->momentum().pz() + (*pitr2)->momentum().pz(),
                              (*pitr1)->momentum().e()  + (*pitr2)->momentum().e());
        double invMass = vec.m();
        invMassMax = std::max(invMass, invMassMax);
      } //pitr2
    } //pitr1
  } //McEventCollection

  setFilterPassed(m_InvMassMin <= invMassMax && invMassMax < m_InvMassMax);
  return StatusCode::SUCCESS;
}
