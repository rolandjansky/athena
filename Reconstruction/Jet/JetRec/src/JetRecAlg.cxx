
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetRecAlg.cxx

#include <memory>
#include "JetRecAlg.h"
#include "JetInterface/IJetExecuteTool.h"
#include "xAODJet/JetAuxContainer.h"

#if !defined (GENERATIONBASE) && !defined (XAOD_ANALYSIS)
#include "AthenaMonitoringKernel/Monitored.h"
#include "GaudiKernel/SystemOfUnits.h"
#endif

using std::string;

//**********************************************************************

StatusCode JetRecAlg::initialize() {

  ATH_CHECK(m_output.initialize());

  ATH_CHECK(m_jetprovider.retrieve());
  // Some providers (e.g. copy) need the output WriteHandle
  // to be provided during initialisation
  ATH_CHECK(m_jetprovider->initWithOutput(m_output));

  ATH_MSG_INFO(" Initialized  IJetProvider : "<< m_jetprovider->name());

  ATH_MSG_INFO(" Initialize .... List of modifiers: ");
  ATH_CHECK(m_modifiers.retrieve());
  for(ToolHandle<IJetModifier> t : m_modifiers){
    ATH_MSG_INFO("    --> : "<< t->name());
  }

  ATH_CHECK(m_output.initialize());

#if !defined (GENERATIONBASE) && !defined (XAOD_ANALYSIS)
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
#endif

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetRecAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

#if !defined (GENERATIONBASE) && !defined (XAOD_ANALYSIS)
#define DOMONITORING
#endif

StatusCode JetRecAlg::execute(const EventContext& ctx) const {
  // Get JetContainer by running fastjet, grooming or copying existing jets...
  // Pass in the WriteHandle so that the provider can do the record, and we
  // needn't know the type of the jet aux container
  // We can subsequently access the jets from the handle and don't have to
  // worry about memory management.

#ifdef DOMONITORING
  auto t_total = Monitored::Timer<std::chrono::milliseconds>( "TIME_jetreco" );
  auto t_jpv = Monitored::Timer<std::chrono::milliseconds>( "TIME_jetprovider" );
  auto t_mod = Monitored::Timer<std::chrono::milliseconds>( "TIME_jetmod" );
#endif

  SG::WriteHandle<xAOD::JetContainer> jetContHandle(m_output,ctx);

  // Define a scope to ease monitoring of the JetProvider action
  {
#ifdef DOMONITORING
    Monitored::ScopedTimer time_provider(t_jpv);
#endif
    ATH_CHECK( m_jetprovider->getAndRecordJets(jetContHandle) );
    ATH_MSG_DEBUG("Created jet container of size "<< jetContHandle->size() << "  | writing to "<< m_output.key() );
  }

  // Define a scope to ease monitoring of the JetModifier action
  {
#ifdef DOMONITORING
    Monitored::ScopedTimer time_modifiers(t_mod);
#endif
    ATH_MSG_DEBUG("Applying jet modifiers to " << m_output.key());
    // Calculate moments, calibrate, sort, filter...  -----------
    for(const ToolHandle<IJetModifier>& t : m_modifiers){
    ATH_MSG_DEBUG("Running " << t.name());
      ATH_CHECK(t->modify(*jetContHandle));
    }
  }

#if !defined (GENERATIONBASE) && !defined (XAOD_ANALYSIS)
  // monitor jet multiplicity and basic jet kinematics
  auto njets = Monitored::Scalar<int>("JET_n");
  njets      = jetContHandle->size();
  std::vector<float> jetpt, jetet, jeteta, jetphi, jetmass;
  for (const xAOD::Jet* j : *jetContHandle) {
    constexpr float invGeV = 1./Gaudi::Units::GeV;
    jetpt.push_back(j->pt() * invGeV);
    jetet.push_back(j->p4().Et() * invGeV);
    jetmass.push_back(j->m() * invGeV);
    jeteta.push_back(j->eta());
    jetphi.push_back(j->phi());
  }
  auto pt    = Monitored::Collection("JET_pt",  jetpt);
  auto et    = Monitored::Collection("JET_et",  jetet);
  auto eta   = Monitored::Collection("JET_eta", jeteta);
  auto mass  = Monitored::Collection("JET_m",   jetmass);
  auto phi   = Monitored::Collection("JET_phi", jetphi);
  
  auto mon   = Monitored::Group(m_monTool,t_total,t_jpv,t_mod,njets,pt,et,mass,eta,phi);
#endif


  return StatusCode::SUCCESS;

}

//**********************************************************************
