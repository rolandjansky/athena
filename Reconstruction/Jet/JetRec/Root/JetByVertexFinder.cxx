/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetByVertexFinder.cxx

#include "JetRec/JetByVertexFinder.h"
#include <iomanip>
#include <sstream>
#ifdef USE_BOOST_AUTO
#include <boost/typeof/typeof.hpp>
#endif
#ifdef USE_BOOST_FOREACH
#include <boost/foreach.hpp>
#endif
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/IConstituentUserInfo.h"
#include "JetEDM/VertexIndexedConstituentUserInfo.h"
#include "xAODJet/JetContainerInfo.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"     // Needed for ElementLink

using std::string;
using xAOD::Jet;
using xAOD::JetContainer;
using xAOD::Vertex;
using xAOD::VertexContainer;
using jet::IConstituentUserInfo;
using jet::VertexIndexedConstituentUserInfo;

//**********************************************************************

JetByVertexFinder::JetByVertexFinder(string name)
: AsgTool(name),
  m_finder("") {
  declareProperty("JetFinder", m_finder);
  declareProperty("Vertex", m_ivtx);
}

//**********************************************************************

StatusCode JetByVertexFinder::initialize() {
  ATH_MSG_DEBUG("Initializing...");
  if ( m_finder.empty() ) {
    ATH_MSG_ERROR("Unable to retrieve jet finder.");
  } else {
    if ( m_finder->initialize() == StatusCode::SUCCESS ) {
      ATH_MSG_INFO("Finder initialization succeeded.");
    } else {
      ATH_MSG_ERROR("Finder initialization failed.");
    }
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetByVertexFinder::
find(const PseudoJetVector& inps, xAOD::JetContainer& jets,
     xAOD::JetInput::Type inputtype, const NameList& ghostlabs) const {
  ATH_MSG_DEBUG("Begin finding.");
  string sinp0 = xAOD::JetInput::typeName(inputtype);
  ATH_MSG_DEBUG("  Input type: " << sinp0);
  std::vector<PseudoJetVector> psjvectors;
  std::vector<const Vertex*> vertices;
  PseudoJetVector gpsjvector;
  ATH_MSG_VERBOSE("  Loop over " << inps.size() << " input pseudojets:");
#ifdef USE_BOOST_FOREACH
  BOOST_FOREACH(const fastjet::PseudoJet& psj, inps) {
#else
  for (const auto& psj : inps) {
#endif
    int ivtx = -11;
    if ( psj.has_user_info<IConstituentUserInfo>() ) {
      ivtx = -12;
      const IConstituentUserInfo& cui = psj.user_info<IConstituentUserInfo>();
      if ( cui.label() == sinp0 ) {
        ivtx = -13;
        const VertexIndexedConstituentUserInfo* pvcui =
          dynamic_cast<const VertexIndexedConstituentUserInfo*>(&cui);
        if ( pvcui != nullptr ) {
          ivtx = -14;
          const Vertex* pvtx = pvcui->vertex();
          if ( pvtx != 0 ) {
            ivtx = pvtx->index();
            unsigned int size = ivtx + 1;
            if ( psjvectors.size() < size ) psjvectors.resize(size);
            psjvectors[ivtx].push_back(psj);
            if ( vertices.size() < size ) vertices.resize(size);
            vertices[ivtx] = pvtx;
          }
        }
      }
      ATH_MSG_VERBOSE("    Pseudojet label=" << cui.label() << ", Vertex=" << ivtx);
      if ( ivtx == -13 ) ATH_MSG_ERROR("      Pseudojet has unexpected CUI type.");
      if ( ivtx == -14 ) ATH_MSG_VERBOSE("      Pseudojet does not have a vertex.");
      if ( ivtx < 0 && ivtx > -13 )  gpsjvector.push_back(psj);
    }
  }
  // Loop over vertices and find jets.
  ATH_MSG_VERBOSE("  Loop over " << psjvectors.size() << " vertices.");
  for ( unsigned int ivtx=0; ivtx<psjvectors.size(); ++ivtx ) {
    if ( m_ivtx < 0 || int(ivtx) == m_ivtx ) {
#ifdef USE_BOOST_AUTO
      BOOST_AUTO(psjvector, psjvectors[ivtx]);
#else
      auto& psjvector = psjvectors[ivtx];
#endif
      if ( psjvector.size() ) {
        ATH_MSG_VERBOSE("    Adding jets from vertex " << ivtx);
        psjvector.insert(psjvector.end(), gpsjvector.begin(), gpsjvector.end());
        unsigned int ijet0 = jets.size();
        m_finder->find(psjvector, jets, inputtype, ghostlabs);
#ifdef USE_BOOST_AUTO
        for (BOOST_AUTO(ijet, ijet0); ijet<jets.size(); ++ijet) {
#else
        for (auto ijet = ijet0; ijet<jets.size(); ++ijet) {
#endif
          Jet* pjet = jets[ijet];
          pjet->setAssociatedObject("OriginVertex", vertices[ivtx]);
        }
        ATH_MSG_VERBOSE("    Jet count after vertex " << ivtx << ": " << jets.size());
      } else {
        ATH_MSG_VERBOSE("    Skipping no-track vertex " << ivtx);
      }
    } else {
      ATH_MSG_VERBOSE("    Skipping wrong vertex " << ivtx);
    }
  }
  ATH_MSG_DEBUG("End finding. Jet count: " << jets.size());
  return 0;
}

//**********************************************************************

void JetByVertexFinder::print() const {
  ATH_MSG_INFO("      Jet finder: " << m_finder.name());
  ATH_MSG_INFO("    Output level: " << MSG::name(msg().level()));
}

//**********************************************************************
