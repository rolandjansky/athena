/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyTruthJetParticles.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "TruthUtils/PIDUtils.h"

using namespace std;


CopyTruthJetParticles::CopyTruthJetParticles(const std::string& name)
  : CopyTruthParticles(name) ,
    m_maxAbsEta(5)
{
  declareProperty("IncludeNeutrinos",  m_includeNu=false, "Include neutrinos in the output collection");
  declareProperty("IncludeMuons",      m_includeMu=false, "Include muons in the output collection");
  declareProperty("IncludeWZLeptons",  m_includeWZ=true,  "Include leptons from W/Z decays in the output collection");
  declareProperty("IncludeTauLeptons", m_includeTau=true, "Include leptons from tau decays in the output collection");
  declareProperty("MaxAbsEta", m_maxAbsEta);
}

bool CopyTruthJetParticles::classify(const xAOD::TruthParticle* tp) const {

  // Check if this thing is a candidate to be in a truth jet
  //  First block is largely copied from isGenStable, which works on HepMC only
  if (tp->barcode()>=200000) return false; // Particle is from G4
  if (tp->pdgId()==21 && tp->e()==0) return false; // Work around for an old generator bug
  if ( tp->status()%1000!=1 ) return false; // Stable!
  
  // Easy classifiers by PDG ID
  if (!m_includeNu && MC::PID::isNeutrino(tp->pdgId())) return false;
  if (!m_includeMu && abs(tp->pdgId())==13) return false;

  // Cannot use the truth helper functions; they're written for HepMC
  // Last two switches only apply if the thing is a lepton and not a tau
  if (MC::PID::isLepton(tp->pdgId()) && abs(tp->pdgId())!=15 && tp->hasProdVtx()){
    if (!m_includeWZ && fromWZ( tp )) return false;
    if (!m_includeTau && fromTau( tp )) return false;
  }

  if(fabs(tp->eta())>m_maxAbsEta) return false;
  // Made it! 
  return true;
}

bool CopyTruthJetParticles::fromWZ( const xAOD::TruthParticle* tp ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function 
  //  it's used in place of code in MCTruthClassifier as long as this packagage is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Hit a hadron -> return false
  // Hit a parton -> return true
  //   This catch is important - we *cannot* look explicitly for the W or Z, because some
  //    generators do not include the W or Z in the truth record (like Sherpa)
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!tp->hasProdVtx()) return false;
  for (size_t i=0;i<tp->prodVtx()->nIncomingParticles();++i){
    if (MC::PID::isHadron( tp->prodVtx()->incomingParticle(i)->pdgId() ) ) return false;
    if ( abs( tp->prodVtx()->incomingParticle(i)->pdgId() ) < 9 ) return true;
    if ( tp->prodVtx()->incomingParticle(i)->pdgId() == tp->pdgId() ) return fromWZ( tp->prodVtx()->incomingParticle(i) );
  }
  return false;
}

bool CopyTruthJetParticles::fromTau( const xAOD::TruthParticle* tp ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function 
  //  it's used in place of code in MCTruthClassifier as long as this packagage is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Find a tau -> return true
  // Find a hadron or parton -> return false
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!tp->hasProdVtx()) return false;
  for (size_t i=0;i<tp->prodVtx()->nIncomingParticles();++i){
    if ( abs( tp->prodVtx()->incomingParticle(i)->pdgId() ) == 15 ) return true;
    if (MC::PID::isHadron( tp->prodVtx()->incomingParticle(i)->pdgId() ) ||
        abs( tp->prodVtx()->incomingParticle(i)->pdgId() ) < 9 ) return false;
    if ( tp->prodVtx()->incomingParticle(i)->pdgId() == tp->pdgId() ) return fromWZ( tp->prodVtx()->incomingParticle(i) );
  }
  return false;
}
