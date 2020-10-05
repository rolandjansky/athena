/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/ParentTwoChildrenFilter.h"

ParentTwoChildrenFilter::ParentTwoChildrenFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("PDGParent",m_PDGParent);
  declareProperty("PtMinParent",m_PtMinParent = 0.);
  declareProperty("EtaRangeParent",m_EtaRangeParent = 10.0);
  declareProperty("PDGChild",m_PDGChild);
  declareProperty("PtMinChild",m_PtMinChild = 0.);
  declareProperty("EtaRangeChild",m_EtaRangeChild = 10.0);
}


StatusCode ParentTwoChildrenFilter::filterInitialize() {
 if (m_PDGParent.size() == 0) ATH_MSG_ERROR("PDGParent[] not set ");
 if (m_PDGChild.size() == 0) ATH_MSG_ERROR("PDGChild[] not set ");
 for (int i=0; i < int(m_PDGParent.size()); i++) ATH_MSG_DEBUG("PDGParent["<<i<<"] = " << m_PDGParent[i]);
 ATH_MSG_DEBUG("PtMinParent = " << m_PtMinParent);
 ATH_MSG_DEBUG("EtaRangeParent = " << m_EtaRangeParent);
 for (int i=0; i < int(m_PDGChild.size()); i++) ATH_MSG_DEBUG("PDGChild["<<i<<"] = " << m_PDGChild[i]);
 ATH_MSG_DEBUG("PtMinChild = " << m_PtMinChild);
 ATH_MSG_DEBUG("EtaRangeChild = " << m_EtaRangeChild);
 return StatusCode::SUCCESS;
}


StatusCode ParentTwoChildrenFilter::filterEvent() {
  ATH_MSG_DEBUG(" ParentTwoChildrenFilter filtering for: Parent --> " << m_PDGParent[0]
                << ", Child(1) --> " << m_PDGChild[0] << " and Child(2) --> " << m_PDGChild[1]);
  int n_parents = 0;
  int N_Child[2][2];
  for (int i = 0; i < 2; i++) {
    N_Child[i][0] = 0;
    N_Child[i][1] = 0;
  }
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto  pitr:  *genEvt) {
      int id = pitr->pdg_id();
      if (std::abs(id) != m_PDGParent[0]) continue;
      if (pitr->momentum().perp() < m_PtMinParent) continue;
      n_parents++;
      HepMC::ConstGenVertexPtr decayVtx = pitr->end_vertex();
      // Verify if we got a valid pointer and retrieve the number of daughters
      if (!decayVtx) continue; 
#ifdef HEPMC3
      int n_daughters = decayVtx->particles_out().size();
#else
      int n_daughters = decayVtx->particles_out_size();
#endif
      if (n_daughters < 2) continue;
        for( auto thisChild: *decayVtx ) {
          ATH_MSG_DEBUG(" ParentTwoChildrenFilter: parent ==> " <<pitr->pdg_id() << " child ===> "  <<thisChild->pdg_id());
          for (int i = 0; i < 2; i++) {
            if ( std::abs(thisChild->pdg_id()) == m_PDGChild[i] ){
              if ( thisChild->pdg_id()    == m_PDGChild[i] ){
                if (thisChild->momentum().perp() >= m_PtMinChild) N_Child[i][0]++;
              }
              if ( thisChild->pdg_id()   == -m_PDGChild[i] ){
                if (thisChild->momentum().perp() >= m_PtMinChild) N_Child[i][1]++;
              }
            }
          }
        }
    }
  }
  setFilterPassed(N_Child[0][0] >= 1 && N_Child[0][1] >= 1 && N_Child[1][0] >= 1 && N_Child[1][1] >= 1);
  return StatusCode::SUCCESS;
}
