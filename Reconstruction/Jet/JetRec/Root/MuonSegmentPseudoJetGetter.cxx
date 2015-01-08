/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonSegmentPseudoJetGetter.cxx

#include "JetRec/MuonSegmentPseudoJetGetter.h"
#ifdef USE_BOOST_AUTO
#include <boost/typeof/typeof.hpp>
#endif
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/IndexedTConstituentUserInfo.h"

using std::string;
using jet::LabelIndex;
using jet::PseudoJetVector;

//**********************************************************************

MuonSegmentPseudoJetGetter::MuonSegmentPseudoJetGetter(const std::string& name)
: AsgTool(name) {
  declareProperty("InputContainer", m_incoll);
  declareProperty("OutputContainer", m_outcoll);
  declareProperty("Label", m_label);
  declareProperty("Pt", m_pt =1.e-10);
}

//**********************************************************************

StatusCode MuonSegmentPseudoJetGetter::initialize() {
  ATH_MSG_DEBUG("Initializing...");
  print();
  return StatusCode::SUCCESS;
}

//**********************************************************************

const PseudoJetVector* MuonSegmentPseudoJetGetter::get() const {
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

int MuonSegmentPseudoJetGetter::appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const {
  ATH_MSG_DEBUG("Entering appendTo(PseudoJetVector)...");
  if ( evtStore()->contains<xAOD::MuonSegmentContainer>(m_incoll) ) {
    ATH_MSG_DEBUG("Retrieving xAOD muon segment container " << m_incoll << ", pT="
                  << m_pt  <<  ", isGhost");
    const xAOD::MuonSegmentContainer* ppars = 0;
    ppars = evtStore()->retrieve<const xAOD::MuonSegmentContainer>(m_incoll);
    if ( ppars != 0 ) return append(*ppars, psjs, pli);
  }
  ATH_MSG_ERROR("Unable to find input collection: " << m_incoll);
  return 1;
}

//**********************************************************************

int MuonSegmentPseudoJetGetter::
append(const xAOD::MuonSegmentContainer& inputs, PseudoJetVector& psjs, const LabelIndex* pli) const {
  jet::IConstituentUserInfo::Index labidx = 0;
  if ( pli != 0 ) labidx = pli->index(m_label);
  else ATH_MSG_WARNING("Index-to-label map is not supplied.");
  labidx = -labidx;

  /// Loop over input, buid CUI and PseudoJets
#ifdef USE_BOOST_AUTO
  for ( BOOST_AUTO(iinp, inputs.begin()); iinp!=inputs.end(); ++iinp ) {
    BOOST_AUTO(ppar, *iinp); // IParticle pointer
#else
  for ( auto iinp=inputs.begin(); iinp!=inputs.end(); ++iinp ) {
    auto ppar = *iinp; // IParticle pointer
#endif
    double pt = m_pt;
    double x = ppar->x();
    double y = ppar->y();
    double z = ppar->z();
    double xy = sqrt(x*x + y*y);
    double r = sqrt(xy*xy + z*z);
    double pfac = pt/xy;
    double px = pfac*x;
    double py = pfac*y;
    double pz = pfac*z;
    double  e = pfac*r;
    fastjet::PseudoJet psj(px, py, pz, e);
    ATH_MSG_VERBOSE("Muon segment pseuojet y: " << psj.rap());
    if ( pli != 0 ) {
      jet::IConstituentUserInfo* pcui =
        new jet::IndexedTConstituentUserInfo<xAOD::MuonSegment>(*ppar,labidx, pli);
      psj.set_user_info(pcui);
    }
    psjs.push_back(psj);
  } // end loop over input 
  ATH_MSG_DEBUG("After append, PseudoJet count is " << psjs.size());
  return 0;
}

//**********************************************************************


std::string MuonSegmentPseudoJetGetter::label() const{
  return m_label;
}

//**********************************************************************

void MuonSegmentPseudoJetGetter::print() const {
  ATH_MSG_INFO("Properties for PseudoJetGetter " << name());
  ATH_MSG_INFO("             Label: " << m_label);
  ATH_MSG_INFO("   Input container: " << m_incoll);
  ATH_MSG_INFO("  Output container: " << m_outcoll);
  ATH_MSG_INFO("      Pseudojet pT: " << m_pt);

}

//**********************************************************************
