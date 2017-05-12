/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////
// TruthParticle Utilities 
//////////////////////////

#include "CPAnalysisExamples/xExampleUtils.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

#include <vector>

// Long-lived B hadron (5x1,5xx2)
bool isBHadron(const xAOD::TruthParticle* p){
  int ida = TMath::Abs( p->pdgId() );
  if( ida > 10000 || ida < 100 ) return false;
  if( ida > 1000 ){
    if( ida==5112 || ida==5132 || ida==5232 || ida==5332 ){
      return true;
    } else {
      return false;
    }
  }
  if( (ida/100)%10==5 && ida%10==1 ) return true;
  return false;
}

// Implement HepMC isGenStable
bool isGenStable(const xAOD::TruthParticle* p){
  if( p->barcode() > 200000 ) return false;
  if( p->pdgId() == 21 && p->e() == 0 ) return false;
  if( p->status()%1000 == 1 ) return true;
  if( p->hasDecayVtx() ){
    const xAOD::TruthVertex* dvtx = p->decayVtx();
    if( dvtx ){
      if( p->status() == 2 && dvtx->barcode() < -200000 ) return true;
    }
  }
  if( p->status()%1000 == 2 && p->status() > 1000 ) return true;
  return false;
}

// Emulate HepMC descendant iterator
// Multiple particles can give same descendant (string/cluster)
// Remove Geant descendants
// MUST check ElementLink validity with isValid() for thinned samples

int getDescendants(const xAOD::TruthParticle* p,
                   std::vector< const xAOD::TruthParticle* >& dscd ){
  dscd.clear();
  if( ! (p->hasDecayVtx()) ) return 0;
  const xAOD::TruthVertex* dvtx = p->decayVtx();
  if( !dvtx ) return 0;

  const std::vector< ElementLink< xAOD::TruthParticleContainer > >& outPart =
    dvtx->outgoingParticleLinks();
  int nkid = outPart.size();
  for(int k=0; k<nkid; ++k){
    if( ! (outPart[k]).isValid() ) continue;
    const xAOD::TruthParticle* kid = *(outPart[k]);
    dscd.push_back(kid);
  }

  int nstart = 0;
  int nstop = dscd.size();

  while( nstop > nstart ){
    for(int i=nstart; i<nstop; ++i){
      const xAOD::TruthParticle* pp = dscd[i];
      if( ! (pp->hasDecayVtx()) ) continue;
      const xAOD::TruthVertex* vpp = pp->decayVtx();
      if( !vpp ) continue;
      if( vpp->barcode() < -200000 ) continue;
      const std::vector< ElementLink< xAOD::TruthParticleContainer > >&
        outPart2 = vpp->outgoingParticleLinks();
      unsigned int npp = outPart2.size();
      for(unsigned int k=0; k<npp; ++k){
        if( ! (outPart2[k]).isValid() ) continue;
        const xAOD::TruthParticle* kpp = *(outPart2[k]);
        if( kpp->barcode() > 200000 ) continue;
        bool isIn = false;
        for(unsigned int kk=0; kk<dscd.size(); ++kk) if(kpp==dscd[kk]) isIn = true;
        if( !isIn ) dscd.push_back(kpp);
      }
    }
    nstart = nstop;
    nstop = dscd.size();
  }

  return nstop;
}
