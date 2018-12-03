/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetGetter.cxx

#include "JetRec/PseudoJetGetter.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetRec/PseudoJetGetterRegistry.h"

using std::string;
using jet::LabelIndex;
using jet::PseudoJetVector;

//**********************************************************************

PseudoJetGetter::PseudoJetGetter(const std::string& name)
: AsgTool(name), m_emtopo(false) {
  declareProperty("InputContainer", m_incoll);
  declareProperty("OutputContainer", m_outcoll);
  declareProperty("Label", m_label);
  declareProperty("SkipNegativeEnergy", m_skipNegativeEnergy =false);
  declareProperty("GhostScale", m_ghostscale =0.0);
  declareProperty("TreatNegativeEnergyAsGhost", m_negEnergyAsGhosts =false);

}

//**********************************************************************

StatusCode PseudoJetGetter::initialize() {
  ATH_MSG_DEBUG("Initializing...");
  // PJG needs to know if this is the basic EMTopo cluster collection
  // in order to change the cluster signal state.
  if ( m_label == "EMTopo") m_emtopo = true;
  PseudoJetGetterRegistry::add(this);
  print();
  return StatusCode::SUCCESS;
}

//**********************************************************************

const PseudoJetVector* PseudoJetGetter::get() const {
  ATH_MSG_DEBUG("Getting pseudojets...");
  PseudoJetVector* ppsjs = 0;
  // Check if output container is already present.
  if ( evtStore()->contains<PseudoJetVector>(m_outcoll) ) {
    ATH_MSG_DEBUG("Fetching existing pseudojets." << m_outcoll);
    ppsjs = evtStore()->retrieve<PseudoJetVector>(m_outcoll);
  }
  // Create and fill output container.
  if ( ppsjs == 0 ) {
    // Fetch label index map.
    LabelIndex* pli = 0;
    string liname = "PseudoJetLabelMap";
    if ( evtStore()->contains<LabelIndex>(liname) ) {
      ATH_MSG_DEBUG("Fetching existing pseudojet label map." << m_outcoll);
      pli = evtStore()->retrieve<LabelIndex>(liname);
      if ( pli == 0 ) {
        ATH_MSG_ERROR("Unable to fetch pseudojet label map.");
        return 0;
      }
    } else {
      ATH_MSG_DEBUG("Creating new pseudojet label map." << m_outcoll);
      pli = new LabelIndex(liname);
      if ( evtStore()->record(pli, liname).isFailure() ) {
        ATH_MSG_ERROR("Unable to store pseudojet label map.");
        return 0;
      }
    }
    // Add this label to the map.
    if ( pli->addLabel(m_label) == 0 ) {
      ATH_MSG_ERROR("Unable to store pseudojet label map.");
      return 0;
    }
    // Create pseudojet collection.
    ATH_MSG_DEBUG("Creating new pseudojet collection");
    ppsjs = new PseudoJetVector;
    // Fill pseudojet collection.
    if ( appendTo(*ppsjs, pli) ) {
      ATH_MSG_ERROR("Error creating pseudojets.");
      delete ppsjs;
      return 0;
    }
    StatusCode sc = evtStore()->record(ppsjs, m_outcoll);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR("Unable to write new pseudojet collection to event store: "
                    << m_outcoll);
    } else {
      ATH_MSG_DEBUG("Created new pseudojet collection in event store: "
                    << m_outcoll);
    }
  } else {
    ATH_MSG_DEBUG("Using existing PseudoJet collection");
  }
  return ppsjs;
}

//**********************************************************************

int PseudoJetGetter::appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const {
  ATH_MSG_DEBUG("Entering appendTo(PseudoJetVector)...");

  if ( evtStore()->contains<xAOD::IParticleContainer>(m_incoll) ) {
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_incoll << ", ghost scale="
                  << m_ghostscale  <<  ", isGhost=" << bool(m_ghostscale));
    const xAOD::IParticleContainer* ppars = 0;
    ppars = evtStore()->retrieve<const xAOD::IParticleContainer>(m_incoll);
    if ( ppars != 0 ) return append(*ppars, psjs, pli);
  }

  // if ( evtStore()->contains<xAOD::CaloClusterContainer>(m_incoll) ) {
  //   ATH_MSG_DEBUG("Retrieving xAOD clusters " << m_incoll);
  //   const xAOD::CaloClusterContainer* ppars = 0;
  //   ppars = evtStore()->retrieve<const xAOD::CaloClusterContainer>(m_incoll);
  //   if ( ppars != 0 ) return append(*ppars, psjs, pli);
  // } else if ( evtStore()->contains<xAOD::JetContainer>(m_incoll) ) {
  //   ATH_MSG_DEBUG("Retrieving xAOD jets " << m_incoll);
  //   const xAOD::JetContainer* ppars = 0;
  //   //StatusCode sc = evtStore()->retrieve(ppars, m_incoll);
  //   ppars = evtStore()->retrieve<const xAOD::JetContainer>(m_incoll);
  //   if ( ppars != 0 ) return append(*ppars, psjs, pli);
  // }
  ATH_MSG_ERROR("Unable to find input collection: " << m_incoll);
  return 1;
}

//**********************************************************************

std::string PseudoJetGetter::label() const{
  return m_label;
}

//**********************************************************************

void PseudoJetGetter::print() const {
  string sskip = m_skipNegativeEnergy ? "true" : "false";
  ATH_MSG_INFO("Properties for PseudoJetGetter " << name());
  ATH_MSG_INFO("             Label: " << m_label);
  ATH_MSG_INFO("   Input container: " << m_incoll);
  ATH_MSG_INFO("  Output container: " << m_outcoll);
  ATH_MSG_INFO("   Skip negative E: " << sskip);
  ATH_MSG_INFO("         Is EMTopo: " << m_emtopo);
  ATH_MSG_INFO(" Treat negative E as ghost: " << m_negEnergyAsGhosts);
}

//**********************************************************************

jet::IConstituentUserInfo* PseudoJetGetter::
buildCUI(const xAOD::IParticle* ppar, jet::IConstituentUserInfo::Index idx,
         const LabelIndex* pli) const {
  return new jet::IndexedConstituentUserInfo(*ppar, idx, pli);
}

//**********************************************************************

int PseudoJetGetter::inputContainerNames(std::vector<std::string>& connames) {
  if ( m_incoll.size() ) connames.push_back(m_outcoll);
  if ( m_incoll.size() ) connames.push_back(m_incoll);
  return 0;
}

//**********************************************************************

int PseudoJetGetter::outputContainerNames(std::vector<std::string>& connames) {
  if ( m_outcoll.size() ) connames.push_back(m_outcoll);
  return 0;
}

//**********************************************************************
