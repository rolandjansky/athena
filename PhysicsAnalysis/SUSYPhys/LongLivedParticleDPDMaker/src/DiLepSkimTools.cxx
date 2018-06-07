/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DiLepSkimTools.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Dominik Krauss (krauss@mpp.mpg.de)

// Athena
#include "LongLivedParticleDPDMaker/DiLepSkimTools.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"

DerivationFramework::DiLepSkim::DiLepSkim(const std::string& t, const std::string& n, const IInterface* p)
                                         : AthAlgTool(t, n, p), m_dlf("DerivationFramework::DiLepFilters/DiLepFilters"),
                                           m_filter_prop(0), m_filter(Filters::None)
{
  declareInterface<DerivationFramework::ISkimmingTool>(this);
  declareProperty("Filter", m_filter_prop);
}

StatusCode DerivationFramework::DiLepSkim::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  CHECK(m_dlf.retrieve());

  m_filter = static_cast<Filters>(m_filter_prop);
  if(m_filter == Filters::None)
  {
    ATH_MSG_ERROR("No DESD filter specified!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

bool DerivationFramework::DiLepSkim::eventPassesFilter() const
{
  // we check the filters only if at least one trigger is passed
  if(!m_dlf->GetTriggers()) return false;

  // retrieve particle containers
  const xAOD::ElectronContainer* elc = nullptr;
  CHECK(evtStore()->retrieve(elc, "Electrons"));
  const xAOD::MuonContainer* muc = nullptr;
  CHECK(evtStore()->retrieve(muc, "Muons"));
  const xAOD::PhotonContainer* phc = nullptr;
  CHECK(evtStore()->retrieve(phc, "Photons"));

  // loop over containers and check filters
  if(m_filter == Filters::SiEl)
  {
    for(const xAOD::Electron* el: *elc)
    {
      if(m_dlf->PassSiEl(*el)) return true;
    }
  }
  else if(m_filter == Filters::SiPhX)
  {
    for(auto ph1 = phc->cbegin(); ph1 != phc->cend(); ph1++)
    {
      for(const xAOD::Electron* el: *elc)
      {
        if(m_dlf->PassSiPhX(**ph1, *el)) return true;
      }
      for(auto ph2 = ph1+1; ph2 != phc->cend(); ph2++)
      {
        if(m_dlf->PassSiPhX(**ph1, **ph2)) return true;
      }
      for(const xAOD::Muon* mu: *muc)
      {
        if(m_dlf->PassSiPhX(**ph1, *mu)) return true;
      }
    }
  }
  else if(m_filter == Filters::SiMu)
  {
    for(const xAOD::Muon* mu: *muc)
    {
      if(m_dlf->PassSiMu(*mu)) return true;
    }
  }
  else if(m_filter == Filters::DiEl)
  {
    for(auto el1 = elc->cbegin(); el1 != elc->cend(); el1++)
    {
      for(auto el2 = el1+1; el2 != elc->cend(); el2++)
      {
        if(m_dlf->PassDiEl(**el1, **el2)) return true;
      }
    }
  }
  else if(m_filter == Filters::DiPh)
  {
    for(auto ph1 = phc->cbegin(); ph1 != phc->cend(); ph1++)
    {
      for(auto ph2 = ph1+1; ph2 != phc->cend(); ph2++)
      {
        if(m_dlf->PassDiPh(**ph1, **ph2)) return true;
      }
    }
  }
  else if(m_filter == Filters::DiElPh)
  {
    for(const xAOD::Electron* el: *elc)
    {
      for(const xAOD::Photon* ph: *phc)
      {
        if(m_dlf->PassDiElPh(*el, *ph)) return true;
      }
    }
  }
  else if(m_filter == Filters::DiLoElPh)
  {
    for(const xAOD::Electron* el: *elc)
    {
      for(const xAOD::Photon* ph: *phc)
      {
        if(m_dlf->PassDiLoElPh(*el, *ph)) return true;
      }
    }
  }

  return false;
}
