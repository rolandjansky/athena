/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include "DerivationFrameworkExotics/TruthHelper.h"
#include <algorithm>

bool DerivationFramework::passTruthFilter(const xAOD::TruthEventContainer* xTruthEventContainer, const xAOD::JetContainer *truthJets, double JetEtaFilter, double JetpTFilter, double MjjFilter, double PhijjFilter){

   bool passFilter=false;
   std::vector<TLorentzVector> jets, ele_taus;
   TLorentzVector tmp;

   // load the electrona and taus if requested
   if(xTruthEventContainer){
     static SG::AuxElement::Accessor<unsigned int> acc_classifierParticleOrigin("classifierParticleOrigin");
     xAOD::TruthEventContainer::const_iterator itr;
     for (itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {
       unsigned nPart = (*itr)->nTruthParticles();
       for(unsigned iPart=0; iPart<nPart;++iPart){
	 const xAOD::TruthParticle* particle = (*itr)->truthParticle(iPart);
	 if(particle && 
	    ((particle->status()==1 && fabs(particle->pdgId())==11) || fabs(particle->pdgId())==15) && // require status 1 for the electrons. taus do NOT require status
	    particle->pt()>20.0e3 && fabs(particle->eta())<5.0 &&
	    (acc_classifierParticleOrigin(*particle)==12 || acc_classifierParticleOrigin(*particle)==13)) // from a truth W or Z
	   ele_taus.push_back(particle->p4());
       }
     }
   }
   // Load the truth jets
   for (const auto &jet : *truthJets) {
     tmp.SetPtEtaPhiM(jet->pt(),jet->eta(),jet->phi(),jet->m() );
     if(fabs(jet->eta() )<JetEtaFilter && jet->pt()>JetpTFilter){
       bool passOR=true;
       for(unsigned ell=0; ell<ele_taus.size(); ++ell){
	 if(ele_taus.at(ell).DeltaR(tmp)<0.3) passOR=false;
       }
       
       if(passOR) jets.push_back(tmp);
     }
   }
   // Compute the jets
   //if(jets.size()>1) GreaterPt(jets.at(0),jets.at(1));
   //std::sort(jets.begin(),jets.end(), DerivationFramework::GreaterPt);
   if(jets.size()>=2){
     if(jets.at(0).Pt() < jets.at(1).Pt()) std::cout << "jets are not ordered: " << jets.at(0).Pt() << " " << jets.at(1).Pt() << std::endl;
     double e_DiJetMass = (jets.at(0) + jets.at(1)).M();
     double e_JetsDPhi = fabs(jets.at(0).DeltaPhi(jets.at(1)));
     if (e_DiJetMass > MjjFilter && e_JetsDPhi < PhijjFilter) passFilter=true;
   }

   return passFilter;
}

const std::vector<float> DerivationFramework::computeTruthInfo(const xAOD::TruthEventContainer* xTruthEventContainer){

  float flav=0;
  float ptv62=-1.0;
  TLorentzVector lep23,lep3;
  bool isCharm=false;
  bool isBottom=false;
  xAOD::TruthEventContainer::const_iterator itr;
  for (itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {
    unsigned nPart = (*itr)->nTruthParticles();
    for(unsigned iPart=0; iPart<nPart;++iPart){
      const xAOD::TruthParticle* particle = (*itr)->truthParticle(iPart);

      if(particle && ((fabs(particle->pdgId())>=11 && fabs(particle->pdgId())<=18))){ 
	if(particle->status()==23) lep23+=particle->p4();// these are the events without a status 62 W or Z
	if(particle->status()==3) lep3+=particle->p4();// sherpa these are status 3 for the ME
      }
      if(particle && ((fabs(particle->pdgId())>=11 && fabs(particle->pdgId())<=18) || fabs(particle->pdgId())==24 || fabs(particle->pdgId())==23) && fabs(particle->eta())<10.0){
	if(particle->status()==62 && (particle->pdgId()==23 || fabs(particle->pdgId())==24)){ ptv62 = particle->pt(); }	
      }

      if(particle && (isBwithWeakDK(particle->pdgId())) && particle->pt()>0.0 && fabs(particle->eta())<4.0){ isBottom=true; }
      if(particle && (isDwithWeakDK(particle->pdgId())) && particle->pt()>4.0e3 && fabs(particle->eta())<3.0) isCharm=true;

    }// end particle loop
  }

  if(isCharm) flav=2;
  if(isBottom) flav=1;

   float ptvFilter=ptv62;
   if(ptv62<0.0) ptvFilter = lep23.Pt();

  std::vector<float> out;
  out.push_back(flav); // flavour filter decision
  out.push_back(ptvFilter); // madgraph ptV
  out.push_back(lep3.Pt()); // sherpa ptV
  return out;
}

bool DerivationFramework::isBwithWeakDK(const int pID){
  int id = abs(pID);
  return ( id == 511   || // B+
	   id == 521   || // B0
	   id == 531   || // Bs
	   id == 541   || // Bc
	   id == 5122  || // Lambda_B
	   id == 5132  || // Xi_b-
	   id == 5232  || // X_b0
	   id == 5112  || // Sigma_b-
	   id == 5212  || // Sigma_b0
	   id == 5222  || // Sigma_b+
	   id == 5332 );  // Omega_B
}
	
	
bool DerivationFramework::isDwithWeakDK(const int pID){
  int id = abs(pID);
  return ( id == 411   || // D+
	   id == 421   || // D0
	   id == 431   || // Ds
	   id == 4122  || // Lambda_C
	   id == 4132  || // Xi_C^0
	   id == 4232  || // Xi_C^+
	   id == 4212  || // Xi_C^0
	   id == 4322  || // Xi'_C+  This is in fact EM not weak
	   id == 4332); // Omega_C
  
}

