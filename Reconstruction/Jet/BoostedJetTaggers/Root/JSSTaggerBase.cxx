/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JSSTaggerBase.h"

JSSTaggerBase::JSSTaggerBase(const std::string &name) :
  asg::AsgTool(name),
  m_jetPtMin(200000.),
  m_jetPtMax(3000000.),
  m_jetEtaMax(2.0),
  m_decorate(true),
  m_calibarea(""){}

void JSSTaggerBase::showCuts() const{
  int nCuts = m_accept.getNCuts();
  for(int iCut=0; iCut<nCuts; iCut++){
    std::string cut_string = "";
    cut_string += "  (";
    cut_string += std::to_string(iCut);
    cut_string += ")  ";
    cut_string += m_accept.getCutName(iCut).Data();
    cut_string += " : ";
    cut_string += m_accept.getCutDescription(iCut).Data();
    ATH_MSG_INFO( cut_string );
  }
}

WTopLabel JSSTaggerBase::getWTopContainment(const xAOD::Jet& jet, const xAOD::TruthParticleContainer* truthPartsW, const xAOD::TruthParticleContainer* truthPartsZ, const xAOD::TruthParticleContainer* truthPartsTop, bool isSherpa, double dRmax,  double mLowTop, double mHighTop, double mLowW, double mHighW, double mLowZ, double mHighZ) const {

    bool isMatchW=false;
    bool isMatchZ=false;
    bool isMatchTop=false;

    // find W/Z boson matched to the jet with dR<dRmax
    if ( isSherpa ) {
      int isMatchWZ=matchToWZ_Sherpa(jet, truthPartsW, dRmax);
      isMatchW=(isMatchWZ==1);
      isMatchZ=(isMatchWZ==2);
    } else {
      for( auto part : *truthPartsW ){
	if ( fabs(part->pdgId()) != 24  ) continue; // W
	if ( part->nChildren() < 2 ) continue; // skip self-decay
	
	if( part->p4().DeltaR(jet.p4()) < dRmax ) {	
	  isMatchW=true;
	  break;
	}
      }

      for( auto part : *truthPartsZ ){
	if ( fabs(part->pdgId()) != 23  ) continue; // Z
	if ( part->nChildren() < 2 ) continue; // skip self-decay
	
	if( part->p4().DeltaR(jet.p4()) < dRmax ) {	
	isMatchZ=true;
	break;
	}
      }
    }

    // find top quark matched to the jet with dR<dRmax
    for( auto part : *truthPartsTop ){
      if ( fabs(part->pdgId()) != 6 ) continue; // top
      if ( part->nChildren() < 2 ) continue; // skip self-decay

      if( part->p4().DeltaR(jet.p4()) < dRmax ) {
	isMatchTop=true;
	break;
      }
    }

    // find B-hadron matched to the jet with dR<dRmaxB
    int nMatchB=0;
    if( !jet.getAttribute<int>( "GhostBHadronsFinalCount", nMatchB ) ){

      std::vector<const xAOD::TruthParticle*> ghostB; // Ghost associated B hadrons after FSR
      if( !jet.getAssociatedObjects<xAOD::TruthParticle>( "GhostBHadronsFinal", ghostB ) ){      
	ATH_MSG_ERROR("GhostBHadronsFinal cannot be retrieved! Truth label definition of W/top tagger might be wrong");
      } 
      nMatchB=ghostB.size();
    }

    if( !isMatchTop && !isMatchW && !isMatchZ ){
      return WTopLabel::unknown;
    }

    if( isMatchTop && nMatchB>0 &&
	mLowTop < jet.m()*0.001 && jet.m()*0.001 < mHighTop ) {
      return WTopLabel::t; 
    }else if( isMatchW && nMatchB==0 &&
	      mLowW < jet.m()*0.001 && jet.m()*0.001 < mHighW ){
      return WTopLabel::W; 
    }else if( isMatchZ && 
	      mLowZ < jet.m()*0.001 && jet.m()*0.001 < mHighZ ){
      return WTopLabel::Z;
    }else{
      return WTopLabel::other; 
    }
    
    return WTopLabel::unknown;
}

int JSSTaggerBase::matchToWZ_Sherpa(const xAOD::Jet& jet,
				     const xAOD::TruthParticleContainer* truthParts,
				     double dRmax) const {
  // 1: W, 2: Z, 0: others
  int countStatus3=0;
  TLorentzVector p_1(0,0,0,0);
  TLorentzVector p_2(0,0,0,0);
  for ( unsigned int ipart = 0; ipart < truthParts->size(); ipart++ ){
    const xAOD::TruthParticle* part1=truthParts->at(ipart);
    if ( part1->status()!=3 ) continue;
    countStatus3++;
    //if ( countStatus3 < 2 ) continue; // skip incoming beam particles
    p_1=part1->p4();    
    
    // Find the next particle in the list with status==3.
    bool isWcand=false;
    bool isZcand=false;
    for ( unsigned int jpart = ipart+1; jpart < truthParts->size(); jpart++ ){
      const xAOD::TruthParticle* part2=truthParts->at(jpart);
      if ( part2->status()!=3 ) continue;
      p_2=part2->p4();
      if ( part1->pdgId() + part2->pdgId() == 0 ) {
	isZcand=true; // daughters of Z decay should be same-flavor but opposite charge
      }else{
	isWcand=true;
      }
      break; // if p_1 is a daughter of W/Z decay, the next one is the other daughter
    }
    TLorentzVector truthWZcand=p_1+p_2;
    if ( truthWZcand.M() < 60000. || truthWZcand.M() > 140000. ) continue; // ~98% efficiency to W/Z signals. (95% if it is changed to [65, 105]GeV and 90% if [70,100]GeV)
    if ( truthWZcand.DeltaR(jet.p4()) < dRmax ) {
      if ( isWcand ) return 1;
      if ( isZcand ) return 2;
    }
  }
  return 0;
}
void JSSTaggerBase::DecorateMatchedTruthJet(const xAOD::Jet& jet,
					    const xAOD::JetContainer* truthJets,
					    double dRmax,
					    std::string decorName) const {
  jet.auxdecor<const xAOD::Jet*>(decorName) = 0;
  double dRmin=9999;
  for ( const xAOD::Jet* truthjet : *truthJets ) {
    double dR=jet.p4().DeltaR( truthjet->p4() );
    if ( dRmax<0 || dR < dRmax ) { // if dRmax<0, the closest truth jet is used as matched jet. Otherwise, 
      if( dR < dRmin ){
	dRmin=dR;
	jet.auxdecor<const xAOD::Jet*>(decorName) = truthjet;
      }
    }
  }
}

StatusCode JSSTaggerBase::decorateTruthLabel(const xAOD::Jet& jet, std::string decorName, double dR_truthJet, double dR_truthPart, double mLowTop, double mHighTop, double mLowW, double mHighW, double mLowZ, double mHighZ) const {

  const xAOD::JetContainer* truthJet=nullptr;
  evtStore()->retrieve(truthJet, m_truthJetContainerName);
  if( evtStore()->contains<xAOD::TruthParticleContainer>( m_truthWBosonContainerName ) ){
    //std::cout << "isTRUTH3!!" << std::endl;
    const xAOD::TruthParticleContainer* truthPartsW=nullptr;
    evtStore()->retrieve(truthPartsW, m_truthWBosonContainerName);
    const xAOD::TruthParticleContainer* truthPartsZ=nullptr;
    evtStore()->retrieve(truthPartsZ, m_truthZBosonContainerName);
    const xAOD::TruthParticleContainer* truthPartsTop=nullptr;
    evtStore()->retrieve(truthPartsTop, m_truthTopQuarkContainerName);
    return decorateTruthLabel(jet, truthPartsW, truthPartsZ, truthPartsTop, truthJet, decorName,
			      dR_truthJet, dR_truthPart, mLowTop, mHighTop, mLowW, mHighW, mLowZ, mHighZ);
  }else if( evtStore()->contains<xAOD::TruthParticleContainer>( m_truthParticleContainerName ) ){    
    const xAOD::TruthParticleContainer* truthParts=nullptr;
    evtStore()->retrieve(truthParts, m_truthParticleContainerName);
    return decorateTruthLabel(jet, truthParts, truthParts, truthParts, truthJet, decorName,
			      dR_truthJet, dR_truthPart, mLowTop, mHighTop, mLowW, mHighW, mLowZ, mHighZ);
  }

  return StatusCode::FAILURE;
}

StatusCode JSSTaggerBase::decorateTruthLabel(const xAOD::Jet& jet, const xAOD::TruthParticleContainer* truthPartsW, const xAOD::TruthParticleContainer* truthPartsZ, const xAOD::TruthParticleContainer* truthPartsTop, const xAOD::JetContainer* truthJets, std::string decorName, double dRmax_truthJet, double dR_truthPart, double mLowTop, double mHighTop, double mLowW, double mHighW, double mLowZ, double mHighZ ) const {
  DecorateMatchedTruthJet(jet, truthJets, /*dR*/dRmax_truthJet, "dRMatchedTruthJet");
  const xAOD::Jet* truthjet=jet.auxdata<const xAOD::Jet*>("dRMatchedTruthJet");
  WTopLabel jetContainment=WTopLabel::notruth;
  bool isSherpa=getIsSherpa(m_DSID);
  if ( truthjet ) {
    jetContainment=getWTopContainment(*truthjet, truthPartsW, truthPartsZ, truthPartsTop, isSherpa, /*dR for W/Z/top matching*/dR_truthPart, mLowTop, mHighTop, mLowW, mHighW, mLowZ, mHighZ);
  }
  jet.auxdecor<WTopLabel>(decorName) = jetContainment;

  return StatusCode::SUCCESS;
}
