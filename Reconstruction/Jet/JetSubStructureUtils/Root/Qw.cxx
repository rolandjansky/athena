/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/Qw.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include <math.h>

using namespace std;
using namespace JetSubStructureUtils;

Qw::Qw(QwVariant mode, double p, fastjet::JetAlgorithm jetalg) : m_vMode(mode), m_jetdef(), m_massCut(-1) {

  switch (mode ) {
  case Normal:
  case MassCut:
    m_jetdef = fastjet::JetDefinition(jetalg, 1.5) ;
    m_massCut = p;
    break;
  case SmallSubjets:
    if(p==-1) p = 0.2; 
    m_jetdef = fastjet::JetDefinition(jetalg, p) ;
    break;
  }
}

double Qw::result(const fastjet::PseudoJet &jet) const
{
  std::vector<fastjet::PseudoJet> constituents = jet.constituents();
  return result(constituents);
}


namespace {
  void decomposeToJetMass(fastjet::PseudoJet & jet, double massCut, std::vector<fastjet::PseudoJet> & outSubJets){

    if(jet.m() < massCut ) {
      outSubJets.push_back(jet);
      return;
    }
  
    // else decompose and apply procedure to parent 
    fastjet::PseudoJet p1, p2;
    if( ! jet.has_parents (p1, p2) ) return ;
    decomposeToJetMass(p1, massCut, outSubJets);
    decomposeToJetMass(p2, massCut, outSubJets);

  }

}

double Qw::result(std::vector<fastjet::PseudoJet> &constituents) const {

  // deal with extreme cases first.
  // early exit in this cases.
  size_t nconst = constituents.size();
  if( nconst < 3 ) return 0;

  // Build the subjets 
  fastjet::ClusterSequence cs(constituents, m_jetdef);

  double scaleF = 1.;

  std::vector<fastjet::PseudoJet> outjets; 
  
  switch( m_vMode) {
  case Normal :
    outjets= cs.exclusive_jets(3);
    //std::cout << " Normal mode :  scaleF " << scaleF << "  m1="<< outjets[0].m() << std::endl;

    break;

  case MassCut: {
    outjets= cs.exclusive_jets(3);
    fastjet::PseudoJet sumTot(0,0,0,0) ;
    sumTot = outjets[0] + outjets[1] + outjets[2];

    // make sure we deal only with subjets with mass < m_massCut
    std::vector<fastjet::PseudoJet> tmp_subjets; tmp_subjets.reserve(4);
    decomposeToJetMass(outjets[0], m_massCut , tmp_subjets);
    decomposeToJetMass(outjets[1], m_massCut , tmp_subjets);
    decomposeToJetMass(outjets[2], m_massCut , tmp_subjets);
    // Take the 3 highest pt subjets.
    outjets = fastjet::sorted_by_pt( tmp_subjets) ;
    outjets.resize(3);


    // resum the 3 highest pt subjets to compute the scale factor.
    fastjet::PseudoJet  sumSubJ(0,0,0,0);
    sumSubJ = outjets[0] + outjets[1] + outjets[2];
    scaleF = sumTot.pt()/sumSubJ.pt();    
    //    std::cout << " MassCut mode :  scaleF " << scaleF << "  m1="<< outjets[0].m() << std::endl;

  }
    break;
  case SmallSubjets: {
    outjets = fastjet::sorted_by_pt(cs.inclusive_jets());
    
    // deal with extreme case first.
    // early exit in this case.
    size_t nsubjets = outjets.size();
    if( nsubjets < 3 ) return 0;

    // compute the scale factor : as we choose only the 1st 3 subjets, 
    // we scale back the contribution of these 3 subjets to the total Pt.
    fastjet::PseudoJet sumTot(0,0,0,0), sumSubJ(0,0,0,0);
    for( fastjet::PseudoJet& c: constituents) sumTot+=c;

    // take the 3 highest subjets only
    outjets.resize(3);

    for( fastjet::PseudoJet& c: outjets ) sumSubJ+=c;
    scaleF = sumTot.pt()/sumSubJ.pt();
    //    std::cout << " SmallSubjets mode :  scaleF " << scaleF << "  m1="<< outjets[0].m() << std::endl;
  }
    break;
  }

  double m12 = (outjets[0]+outjets[1]).m();
  double m23 = (outjets[2]+outjets[1]).m();
  double m13 = (outjets[2]+outjets[0]).m();

  //  std::cout << " ---------> qw="<< scaleF*std::min(m12, std::min(m23,m13) ) << std::endl;
  return scaleF*std::min(m12, std::min(m23,m13) );
}


