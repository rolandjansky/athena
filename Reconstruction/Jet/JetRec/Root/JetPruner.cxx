/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetPruner.cxx

#include "JetRec/JetPruner.h"
#include <iomanip>
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/tools/Pruner.hh"
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/FastJetUtils.h"
#include "JetRec/JetDumper.h"

using std::setw;
using fastjet::PseudoJet;
using xAOD::JetContainer;

//**********************************************************************

JetPruner::JetPruner(std::string name)
: AsgTool(name), m_bld("") {
  declareProperty("JetAlgorithm", m_jetalg ="CamKt");
  declareProperty("RCut", m_rcut =0.3);
  declareProperty("ZCut", m_zcut =0.10);
  declareProperty("JetBuilder", m_bld);
}

//**********************************************************************

JetPruner::~JetPruner() {
}

//**********************************************************************

StatusCode JetPruner::initialize() {
  xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_jetalg);
  m_fjalg = xAOD::JetAlgorithmType::fastJetDef(ialg);
  if ( m_fjalg == fastjet::undefined_jet_algorithm ) {
    ATH_MSG_ERROR("Invalid jet algorithm name: " << m_jetalg);
    ATH_MSG_ERROR("Allowed values are Kt, CamKt, AntiKt, etc.");
    return StatusCode::FAILURE;
  }
  if ( m_rcut < 0.0 || m_rcut > 10.0 ) {
    ATH_MSG_WARNING("Invalid value for RCut " << m_rcut);
  }
  if ( m_zcut < 0.0 || m_zcut > 1.0 ) {
    ATH_MSG_WARNING("Invalid value for ZCut " << m_zcut);
  }
  if ( m_bld.empty() ) {
    ATH_MSG_ERROR("Unable top retrieve jet builder.");
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetPruner::groom(const xAOD::Jet& jin, xAOD::JetContainer& jets) const {
  const PseudoJet* ppjin = jin.getPseudoJet();
  if ( ppjin == 0 ) {
    ATH_MSG_ERROR("Jet does not have a pseudojet.");
  }
  // Prune.
  fastjet::Pruner pruner(m_fjalg, m_zcut, m_rcut);
  PseudoJet pjprun = pruner(*ppjin);
  ATH_MSG_VERBOSE("   Input cluster sequence: " << ppjin->associated_cluster_sequence());
  ATH_MSG_VERBOSE(" Pruned cluster sequence: " << pjprun.associated_cluster_sequence());
  // Add jet to collection.
  xAOD::Jet* pjet = m_bld->add(pjprun, jets, &jin);
  pjet->setAttribute("RCut", m_rcut);
  pjet->setAttribute("ZCut", m_zcut);
  pjet->setAttribute<int>("TransformType", xAOD::JetTransform::Prune);
  ATH_MSG_DEBUG("Properties after pruning:");
  ATH_MSG_DEBUG("   ncon: " << pjprun.constituents().size() << "/"
                            << ppjin->constituents().size());
  ATH_MSG_DEBUG("   nsub: " << pjprun.pieces().size());
  return 0;
}

//**********************************************************************

void JetPruner::print() const {
  ATH_MSG_INFO("  Jet algorithm: " << m_jetalg);
  ATH_MSG_INFO("   R cut factor: " << m_rcut);
  ATH_MSG_INFO("          Z cut: " << m_zcut);
  ATH_MSG_INFO("  Jet builder: " << m_bld.name());
}

//**********************************************************************
