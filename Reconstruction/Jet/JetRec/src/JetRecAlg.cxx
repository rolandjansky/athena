
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

StatusCode JetRecAlg::execute(const EventContext& ctx) const {
  // Get JetContainer by running fastjet, grooming or copying existing jets...
  // Pass in the WriteHandle so that the provider can do the record, and we
  // needn't know the type of the jet aux container
  // We can subsequently access the jets from the handle and don't have to
  // worry about memory management.

#if !defined (GENERATIONBASE) && !defined (XAOD_ANALYSIS)
  auto t_total = Monitored::Timer<std::chrono::milliseconds>( "TIME_total" );

  SG::WriteHandle<xAOD::JetContainer> jetContHandle(m_output,ctx);

  auto t_jpv = Monitored::Timer<std::chrono::microseconds>( "TIME_jetprovider" );
  ATH_CHECK( m_jetprovider->getAndRecordJets(jetContHandle) );
  auto mon_jpv = Monitored::Group(m_monTool, t_jpv);

  ATH_MSG_DEBUG("Created jet container of size "<< jetContHandle->size() << "  | writing to "<< m_output.key() );

  ATH_MSG_DEBUG("Applying jet modifiers to " << m_output.key());

  // Calculate moments, calibrate, sort, filter...  -----------
  auto t_mod = Monitored::Timer<std::chrono::milliseconds>( "TIME_modifiers_total" );
  for(const ToolHandle<IJetModifier>& t : m_modifiers){
    std::string modname = t.name();
    auto t_mods = Monitored::Timer<std::chrono::microseconds>( Form("TIME_modifier_%s",modname.c_str()) );
    ATH_MSG_DEBUG("Running " << modname);
    ATH_CHECK(t->modify(*jetContHandle));
    auto mon_mods = Monitored::Group(m_monTool, t_mods);
  }
  auto mon_mod_total = Monitored::Group(m_monTool, t_mod);

  // monitor jet multiplicity and basic jet kinematics
  auto njets = Monitored::Scalar<int>("nJets");
  auto pt    = Monitored::Collection("pt",  *jetContHandle, [c=0.001]( const xAOD::Jet* jet ) { return jet->pt()*c; });
  auto et    = Monitored::Collection("et",  *jetContHandle, [c=0.001]( const xAOD::Jet* jet ) { return jet->p4().Et()*c; });
  auto eta   = Monitored::Collection("eta", *jetContHandle, []( const xAOD::Jet* jet ) { return jet->eta(); });
  auto phi   = Monitored::Collection("phi", *jetContHandle, []( const xAOD::Jet* jet ) { return jet->phi(); });
  auto mon   = Monitored::Group(m_monTool,njets,pt,et,eta,phi);
  njets      = jetContHandle->size();

  auto mon_total = Monitored::Group(m_monTool, t_total);

#else

  SG::WriteHandle<xAOD::JetContainer> jetContHandle(m_output,ctx);

  ATH_CHECK( m_jetprovider->getAndRecordJets(jetContHandle) );
  ATH_MSG_DEBUG("Created jet container of size "<< jetContHandle->size() << "  | writing to "<< m_output.key() );

  ATH_MSG_DEBUG("Applying jet modifiers to " << m_output.key());

  // Calculate moments, calibrate, sort, filter...  -----------
  for(const ToolHandle<IJetModifier>& t : m_modifiers){
    ATH_MSG_DEBUG("Running " << t.name());
    ATH_CHECK(t->modify(*jetContHandle));
  }

#endif

  return StatusCode::SUCCESS;

}

//**********************************************************************
