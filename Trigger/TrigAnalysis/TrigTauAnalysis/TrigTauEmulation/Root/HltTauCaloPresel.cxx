/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauEmulation/HltTauSelection.h"
#include "TrigTauEmulation/Utils.h"
#include <cmath>

// Default constructor
HltTauCaloPresel::HltTauCaloPresel() :
  m_use_presel(true),
  m_use_calo_presel(true),
  m_pt(25000.),
  m_centfrac(0.9),
  m_centfrac_strategy("pt_dependent")

{
  m_accept.addCut("CaloPreSel", "CaloPreSel");
}


// Accept method
const Root::TAccept& HltTauCaloPresel::accept(const xAOD::TauJet * hltau) const

{
  m_accept.clear();
  m_accept.setCutResult("CaloPreSel", false);

  if (not m_use_presel) {
    // passthrough mode
    m_accept.setCutResult("CaloPreSel", true);
    return m_accept;
  }

  if (topo_cluster_pt(hltau) < m_pt)
    return m_accept;

  if (not m_use_calo_presel) {
    // passthrough mode
    m_accept.setCutResult("CaloPreSel", true);
    return m_accept;
  }

  if (!centfrac_cut(hltau, m_centfrac_strategy))
    return m_accept;

  m_accept.setCutResult("CaloPreSel", true);
  return m_accept;
}


double HltTauCaloPresel::topo_cluster_pt(const xAOD::TauJet * hltau) const
{
  // Simple return of the HLT tau pt()
  // Note that this is not exactly the same
  // as the pT used online. A ntracks-dependent
  // correction is applied while online
  // all taus are assumed being 1p taus.
  // This happens because we run this correction
  // before the fasttracking reconstruction.
  return hltau->pt();

}

double HltTauCaloPresel::centfrac_cut_ptdep(double pt_mev) const
{
  if (pt_mev < 30000.) {
    return 0.68;
  } else if (pt_mev > 30000 && pt_mev < 35000) {
    return 0.64;
  } else if (pt_mev > 35000 && pt_mev < 40000) {
    return 0.62;
  } else if (pt_mev > 40000 && pt_mev < 50000) {
    return 0.60;
  } else {
    return 0.57;
  }
}

bool HltTauCaloPresel::centfrac_cut(const xAOD::TauJet * hltau, std::string strat) const
{
  // Compute the pt-dependent cut with the hlt tau pT and not the topocluster one
  if (strat == "pt_dependent")
    return (hltau->detail<float>(xAOD::TauJetParameters::centFrac) > centfrac_cut_ptdep(hltau->pt()));
  else
    return (hltau->detail<float>(xAOD::TauJetParameters::centFrac) > m_centfrac);
}
     
