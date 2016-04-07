/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2

#include "TrigTauEmulation/TriggerValidation.h"
#include "AthContainers/AuxElement.h"

TriggerValidation::TriggerValidation(const std::string & name)
{

  m_h1d["lb"] = new TH1F(("lb_" + name).c_str(), "lb", 3000, 0, 3000);
  m_h1d["tauclus"] = new TH1F(("tauclus_" + name).c_str(), "tauclus", 100, 0, 100000);
  m_h1d["l1_taueta"] = new TH1F(("taueta_" + name).c_str(), "taueta", 50, -2.5, 2.5);
  m_h1d["l1_tauphi"] = new TH1F(("tauphi_" + name).c_str(), "tauphi", 50, -3.15, 3.15);

  m_h2d["l1_tau_etaphi"] = new TH2F(("l1_tau_etaphi" + name).c_str() , "etaphi", 50, -2.5, 2.5, 50, -3.15, 3.15);
}



void TriggerValidation::fill_histograms(const xAOD::EventInfo * ei, 
    const xAOD::EmTauRoIContainer * l1taus,
    const std::string & decor)
{

  static SG::AuxElement::Accessor< bool > acc(decor);

  // l1 taus
  for (const auto l1tau: *l1taus) {
    if (decor == "")
      fill_l1tau(l1tau);
    else {
      if (acc.isAvailable(*l1tau))
        if (l1tau->auxdataConst<bool>(decor))
          fill_l1tau(l1tau);
    }
  }


  m_h1d["lb"]->Fill(ei->lumiBlock());

}

void TriggerValidation::fill_l1tau(const xAOD::EmTauRoI * l1tau)
{

  m_h1d["tauclus"]->Fill(l1tau->tauClus());
  m_h1d["l1_taueta"]->Fill(l1tau->eta());
  m_h1d["l1_tauphi"]->Fill(l1tau->phi());
  m_h2d["l1_tau_etaphi"]->Fill(l1tau->eta(), l1tau->phi());
}
