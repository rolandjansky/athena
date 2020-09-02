/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetTrimmer.cxx

#include "JetRec/JetTrimmer.h"
#include <iomanip>
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Filter.hh"
#include "JetEDM/PseudoJetVector.h"

using std::setw;
using fastjet::PseudoJet;
using xAOD::JetContainer;

//**********************************************************************

JetTrimmer::JetTrimmer(std::string name)
  : AsgTool(name), m_bld("",this) {
  declareProperty("RClus", m_rclus =0.3);
  declareProperty("PtFrac", m_ptfrac =0.03);
  declareProperty("JetBuilder", m_bld);
}

//**********************************************************************

JetTrimmer::~JetTrimmer() {
}

//**********************************************************************

StatusCode JetTrimmer::initialize() {
  if ( m_rclus < 0.0 || m_rclus > 10.0 ) {
    ATH_MSG_ERROR("Invalid value for RClus " << m_rclus);
    return StatusCode::FAILURE;
  }
  if ( m_ptfrac < 0.0 || m_ptfrac > 1.0 ) {
    ATH_MSG_ERROR("Invalid value for PtFrac " << m_ptfrac);
    return StatusCode::FAILURE;
  }
  if ( m_bld.empty() ) {
    ATH_MSG_ERROR("Unable to retrieve jet builder.");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_bld.retrieve() );
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetTrimmer::groom(const xAOD::Jet& jin,
                      const PseudoJetContainer& pjContainer,
                      xAOD::JetContainer& jets) const {
  if ( pseudojetRetriever() == nullptr ) {
    ATH_MSG_WARNING("Pseudojet retriever is null.");
    return 1;
  }
  const PseudoJet* ppjin = pseudojetRetriever()->pseudojet(jin);
  if ( ppjin == 0 ) {
    ATH_MSG_WARNING("Jet does not have a pseudojet.");
    return 1;
  }
  // Trim.
  fastjet::Filter trimmer(fastjet::JetDefinition(fastjet::kt_algorithm, m_rclus),
                          fastjet::SelectorPtFractionMin(m_ptfrac));
  PseudoJet pjtrim = trimmer(*ppjin);
  ATH_MSG_VERBOSE("   Input cluster sequence: " << ppjin->associated_cluster_sequence());
  ATH_MSG_VERBOSE(" Trimmed cluster sequence: " << pjtrim.associated_cluster_sequence());
  int nptrim = pjtrim.pieces().size();
  // Add jet to collection.
  xAOD::Jet* pjet = m_bld->add(pjtrim, pjContainer, jets, &jin);
  pjet->setAttribute<int>("TransformType", xAOD::JetTransform::Trim);
  pjet->setAttribute("RClus", float(m_rclus));
  pjet->setAttribute("PtFrac", float(m_ptfrac));
  pjet->setAttribute<int>("NTrimSubjets", nptrim);
  ATH_MSG_DEBUG("Properties after trimming:");
  ATH_MSG_DEBUG("   ncon: " << pjtrim.constituents().size() << "/"
                            << ppjin->constituents().size());
  ATH_MSG_DEBUG("   nsub: " << nptrim);
  if ( pjet == 0 ) {
    ATH_MSG_ERROR("Unable to add jet to container");
  } else {
    ATH_MSG_DEBUG("Added jet to container.");
  }
  return 0;
}

//**********************************************************************

void JetTrimmer::print() const {
  ATH_MSG_INFO("     Recluster R: " << m_rclus);
  ATH_MSG_INFO("  pT faction min: " << m_ptfrac);
  ATH_MSG_INFO("  Jet builder: " << m_bld.name());
}

//**********************************************************************
