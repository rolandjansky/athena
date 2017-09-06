/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OverlapRemovalIndices.cxx 661110 2015-04-17 00:03:39Z morrisj $
#include "TopObjectSelectionTools/OverlapRemovalIndices.h"
#include "TopEvent/EventTools.h"

#include <list>

namespace top {

OverlapRemovalIndices::OverlapRemovalIndices() {
}

OverlapRemovalIndices::~OverlapRemovalIndices() {
}

void OverlapRemovalIndices::overlapremoval(const xAOD::ElectronContainer* el,
                                           const xAOD::MuonContainer* mu,
                                           const xAOD::JetContainer* jet,
                                           const xAOD::JetContainer* ljet,
                                           std::vector<unsigned int>& OUT_el,
                                           std::vector<unsigned int>& OUT_mu,
                                           std::vector<unsigned int>& OUT_jet,
                                           std::vector<unsigned int>& OUT_ljet,
                                           const bool isLoose )
{
  std::string passTopCuts("");
  if (!isLoose) {
    passTopCuts = "passPreORSelection";
  }
  if (isLoose) {
    passTopCuts = "passPreORSelectionLoose";
  }

  
    // Work internally with std::list
    // What passed the pre-overlap removal selection?
    std::vector<unsigned int> IN_el,IN_mu,IN_jet,IN_ljet;
    std::list<unsigned int> l_el,l_mu,l_jet,l_ljet;
    unsigned int index_el(0),index_mu(0),index_jet(0),index_ljet(0);

    if (el) {
        for( auto x : *el ){
            if( x->auxdataConst< char >(passTopCuts) == 1 ){
                IN_el.push_back(index_el);
                l_el.push_back(index_el);
            }
            ++index_el;
        }
    }

    if (mu) {
        for( auto x : *mu ){
            if( x->auxdataConst< char >(passTopCuts) == 1 ){
                IN_mu.push_back(index_mu);
                l_mu.push_back(index_mu);
            }
            ++index_mu;
        }
    }

    if (jet) {
        for( auto x : *jet ){
            if( x->auxdataConst< char >(passTopCuts) == 1 ){
                IN_jet.push_back(index_jet);
                l_jet.push_back(index_jet);
            }
            ++index_jet;
        }
    }

    if (ljet) {
        for( auto x : *ljet ){
            if( x->auxdataConst< char >(passTopCuts) == 1 ){
                IN_ljet.push_back(index_ljet);
                l_ljet.push_back(index_ljet);
            }
            ++index_ljet;
        }
    }

    // Jets and Muons - remove muon with dR < 0.4
    for( auto j : IN_jet ){
        for( auto m : IN_mu ){
            if( top::deltaR( *(jet->at(j) ) , *(mu->at(m)) ) < 0.4 ){
                l_mu.remove( m );
            }
        }
    }

    // Jets and Electrons - remove single closest jet with dR < 0.2
    for (auto e : IN_el) {
        double closestdr = 100.;
        int closestJetIndex = -1;
        for (auto j : IN_jet) {
            const double ejetdr = top::deltaR(*(jet->at(j)) , *(el->at(e)));

            if (ejetdr < 0.2 && ejetdr < closestdr) {
                closestdr = ejetdr;
                closestJetIndex = j;
            }
        }

        if (closestJetIndex > -1)
            l_jet.remove(closestJetIndex);
    }

    // Electrons and Jets - remove electrons with dR < 0.4
    for( auto j : l_jet ){
        for( auto e : IN_el ){
            if( top::deltaR( *(el->at(e)) , *(jet->at(j)) ) < 0.4 ){
                l_el.remove( e );
            }
        }
    }

    // Save what's left of the std::lists into the OUT vectors;
    OUT_el.clear();
    OUT_mu.clear();
    OUT_jet.clear();
    OUT_ljet.clear();

    for( auto i : l_el  ){OUT_el.push_back(i); }
    for( auto i : l_mu  ){OUT_mu.push_back(i); }
    for( auto i : l_jet ){OUT_jet.push_back(i);}
    for( auto i : l_ljet ){OUT_ljet.push_back(i);}
}

void OverlapRemovalIndices::overlapremoval(const xAOD::PhotonContainer*   /*photon*/ ,
                                           const xAOD::ElectronContainer* el  ,
                                           const xAOD::MuonContainer*     mu  ,
                                           const xAOD::TauJetContainer*   /*tau*/ ,
                                           const xAOD::JetContainer*      jet ,
                                           const xAOD::JetContainer*      ljet,
                                           std::vector<unsigned int>& /*goodPhotons*/,
                                           std::vector<unsigned int>& goodElectrons,
                                           std::vector<unsigned int>& goodMuons,
                                           std::vector<unsigned int>& /*goodTaus*/,
                                           std::vector<unsigned int>& goodJets,
                                           std::vector<unsigned int>& goodLargeRJets,
                                           const bool isLoose )
{

  overlapremoval(el, mu, jet, ljet, goodElectrons, goodMuons, goodJets, goodLargeRJets, isLoose );

}

void OverlapRemovalIndices::print(std::ostream& os) const {
    os << "OverlapRemovalIndices\n";
    os << "  (1) remove muon within dR < 0.4 of any jet\n";
    os << "  (2) remove single jet closest to an electron (within dR < 0.2)\n";
    os << "  (3) remove electron with dR < 0.4 of jet\n";
}

}
