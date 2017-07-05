/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#include "TopObjectSelectionTools/OverlapRemovalASG.h"
#include "TopEvent/EventTools.h"

#include <list>

namespace top {

  OverlapRemovalASG::OverlapRemovalASG(bool dolargeJet) :
    m_overlapRemovalTool("OverlapRemovalTool"),
    m_overlapRemovalToolLoose("OverlapRemovalToolLoose"),
    m_passPreORSelection("passPreORSelection"),
    m_passPreORSelectionLoose("passPreORSelectionLoose"),
    m_overlaps("overlaps"),
    m_doLargeJet(dolargeJet)
  {

    top::check( m_overlapRemovalTool.retrieve() , "Failed to retrieve overlap removal tool" );
    top::check( m_overlapRemovalToolLoose.retrieve() , "Failed to retrieve (loose) overlap removal tool" );

  }

  OverlapRemovalASG::~OverlapRemovalASG()
  {
  }

  void OverlapRemovalASG::overlapremoval(const xAOD::PhotonContainer*   photon ,
                                         const xAOD::ElectronContainer* el  ,
                                         const xAOD::MuonContainer*     mu  ,
                                         const xAOD::TauJetContainer*   tau ,
                                         const xAOD::JetContainer*      jet ,
                                         const xAOD::JetContainer*      ljet,
                                         std::vector<unsigned int>& goodPhotons,
                                         std::vector<unsigned int>& goodElectrons,
                                         std::vector<unsigned int>& goodMuons,
                                         std::vector<unsigned int>& goodTaus,
                                         std::vector<unsigned int>& goodJets,
                                         std::vector<unsigned int>& goodLargeRJets,
                                         const bool isLoose )
  {
    std::string passTopCuts("");
    if (!isLoose) {
      if (m_doLargeJet)
        top::check( m_overlapRemovalTool->removeOverlaps( el, mu, jet, tau, photon, ljet ) , "Failed to remove overlaps" );
      else
        top::check( m_overlapRemovalTool->removeOverlaps( el, mu, jet, tau, photon ) , "Failed to remove overlaps" );
      passTopCuts = m_passPreORSelection;
    }
    if (isLoose) {
      if (m_doLargeJet)
        top::check( m_overlapRemovalToolLoose->removeOverlaps( el, mu, jet, tau, photon, ljet ) , "Failed to remove overlaps" );
      else
        top::check( m_overlapRemovalToolLoose->removeOverlaps( el, mu, jet, tau, photon ) , "Failed to remove overlaps" );
      passTopCuts = m_passPreORSelectionLoose;
    }

    removeObjectOverlap( photon, goodPhotons, passTopCuts );
    removeObjectOverlap( el, goodElectrons, passTopCuts );
    removeObjectOverlap( mu, goodMuons, passTopCuts );
    removeObjectOverlap( tau, goodTaus, passTopCuts );
    removeObjectOverlap( jet, goodJets, passTopCuts );
    if (m_doLargeJet)
      removeObjectOverlap( ljet, goodLargeRJets, passTopCuts );
    else
      removeObject( ljet, goodLargeRJets, passTopCuts );


  }

  void OverlapRemovalASG::removeObjectOverlap( const xAOD::IParticleContainer* xaod,
					       std::vector<unsigned int>& OUT_vec,
					       const std::string passTopCuts) {

    OUT_vec.clear();

    unsigned int index(0);

    if (xaod) {
      for (auto x : *xaod) {
        if (x->auxdataConst< char >(passTopCuts) == 1 && x->auxdataConst< char >(m_overlaps) == 0) {
          OUT_vec.push_back(index);
        }
        ++index;
      }
    }
  }

  void OverlapRemovalASG::removeObject( const xAOD::IParticleContainer* xaod,
                                        std::vector<unsigned int>& OUT_vec,
                                        const std::string passTopCuts) {

    OUT_vec.clear();

    unsigned int index(0);

    if (xaod) {
      for (auto x : *xaod) {
        if (x->auxdataConst< char >(passTopCuts) == 1) {
          OUT_vec.push_back(index);
        }
        ++index;
      }
    }
  }

void OverlapRemovalASG::print(std::ostream& os) const {
    os << "OverlapRemovalASG\n";
    os << "  (1) remove electron that sharing track with muon\n";
    os << "  (2) remove single jet closest to an electron (within dR < 0.2)\n";
    os << "  (3) remove electron with dR < 0.4 of jet\n";
    os << "  (4) remove muon within dR < 0.4 of any jet\n";
}

}
