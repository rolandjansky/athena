/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetModifiedMassDrop.cxx

#include "JetRec/JetModifiedMassDrop.h"
#include <iomanip>
#include <algorithm>
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Filter.hh"
#include "JetEDM/PseudoJetVector.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/RecursiveSymmetryCutBase.hh"
#include "fastjet/contrib/ModifiedMassDropTagger.hh"

using std::setw;
using fastjet::PseudoJet;
using fastjet::contrib::RecursiveSymmetryCutBase;
using xAOD::JetContainer;

//**********************************************************************

JetModifiedMassDrop::JetModifiedMassDrop(std::string name)
  : AsgTool(name), m_bld("",this) {
  declareProperty("ZCut",            m_zcut = 0.1);
  declareProperty("SymmetryMeasure", m_sym = scalar_z);
  declareProperty("MassDrop",        m_mu = std::numeric_limits<double>::infinity() );
  declareProperty("RecursionChoice", m_recursion = larger_pt);
  declareProperty("doFilter",        m_doFilt = true);
  declareProperty("FilterNsubjets",  m_filtNSub = 3);
  declareProperty("FilterR",         m_filtR = 0.3);
  declareProperty("JetBuilder",      m_bld);
}

//**********************************************************************

JetModifiedMassDrop::~JetModifiedMassDrop() {
}

//**********************************************************************

StatusCode JetModifiedMassDrop::initialize() {
  if ( m_zcut < 0.0 || m_zcut > 10.0 ) {
    ATH_MSG_ERROR("Invalid value for ZCut " << m_zcut);
    return StatusCode::FAILURE;
  }
  if ( m_mu < 0.0 ) {
    ATH_MSG_ERROR("Invalid value for MassDrop " << m_mu);
  }
  if ( m_sym != y && m_sym != scalar_z && m_sym != vector_z ) {
    ATH_MSG_ERROR("Invalid value for SymmetryMeasure " << m_sym);
    return StatusCode::FAILURE;
  }
  if ( m_recursion != larger_pt && m_recursion != larger_mt && m_recursion != larger_m ) {
    ATH_MSG_ERROR("Invalid value for RecursionChoice " << m_recursion);
    return StatusCode::FAILURE;
  }
  if ( m_bld.empty() ) {
    ATH_MSG_ERROR("Unable to retrieve jet builder.");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetModifiedMassDrop::groom(const xAOD::Jet& jin,
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

  ////////////////////////
  //configure modified mass drop tagger tool
  //http://fastjet.hepforge.org/svn/contrib/contribs/RecursiveTools/tags/1.0.0/ModifiedMassDropTagger.hh
  ////////////////////////

  // Could do this casting in the call to the ModifiedMassDropTagger constructor, doing it here for readability
  RecursiveSymmetryCutBase::SymmetryMeasure sym       = static_cast<RecursiveSymmetryCutBase::SymmetryMeasure>(m_sym);
  RecursiveSymmetryCutBase::RecursionChoice recursion = static_cast<RecursiveSymmetryCutBase::RecursionChoice>(m_recursion);

  //create the modified mass drop tagger, apply it to the input pseudojet
  fastjet::contrib::ModifiedMassDropTagger MMDT(m_zcut,sym,m_mu,recursion);
  // Use grooming mode to avoid getting empty pseudojets
  MMDT.set_grooming_mode();
  PseudoJet pjMMDT = MMDT(*ppjin);
  
  bool didMMDT = pjMMDT.has_structure_of<fastjet::contrib::ModifiedMassDropTagger>();

  // then filter the jet (useful for studies at moderate pt) if requested and if grooming succeeded
  // with a dynamic Rfilt choice (as in arXiv:0802.2470)
  PseudoJet filtpjMMDT = pjMMDT;
  double Rfilt = m_filtR;
  if ( m_doFilt && didMMDT ) {
    Rfilt = std::min(m_filtR, pjMMDT.structure_of<fastjet::contrib::ModifiedMassDropTagger>().delta_R()*0.5);
    fastjet::Filter filter(Rfilt, fastjet::SelectorNHardest(m_filtNSub));
    filtpjMMDT = filter(pjMMDT);
  }

  int npMMDT = filtpjMMDT.pieces().size();
  xAOD::Jet* pjet = m_bld->add(filtpjMMDT, pjContainer, jets, &jin);
  if ( pjet == nullptr ) {
    ATH_MSG_ERROR("Unable to add jet to container");
    return 1;
  }
  //pjet->SetAttribute<int>("TransformType", xAOD::JetTransform::MMDT); // Possibly not needed
  pjet->setAttribute("didMMDT", didMMDT);
  pjet->setAttribute("ZCut", m_zcut);
  pjet->setAttribute("mMDTSymmetryMeasure", m_sym);
  pjet->setAttribute("mMDTRecursionChoice", m_recursion);
  pjet->setAttribute("mMDTMu", m_mu);
  pjet->setAttribute("FilterNSubjects", m_filtNSub);
  pjet->setAttribute("FilterR", Rfilt);
  pjet->setAttribute<int>("NmMDTSubjets", npMMDT);

  ATH_MSG_DEBUG("Properties after softdrop:");
  ATH_MSG_DEBUG("   ncon: " << pjMMDT.constituents().size() << "/"
                            << ppjin->constituents().size());
  ATH_MSG_DEBUG("   nsub: " << npMMDT);
  
  ATH_MSG_DEBUG("Added jet to container.");
  
  return 0;
}

//**********************************************************************

void JetModifiedMassDrop::print() const {
  //ATH_MSG_INFO("  Angular fraction min: " << m_zcut);
  //ATH_MSG_INFO("  Angular exponent: " << m_beta);
  ATH_MSG_INFO("  Jet builder: " << m_bld.name());
}

//**********************************************************************
