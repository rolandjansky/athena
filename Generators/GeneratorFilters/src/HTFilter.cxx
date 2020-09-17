/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header for this module
#include "GeneratorFilters/HTFilter.h"

// Framework Related Headers
#include "GaudiKernel/MsgStream.h"

// Used for retrieving the collection
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

// Other classes used by this class
#include "TruthUtils/PIDHelpers.h"
#include "AtlasHepMC/GenEvent.h"
// #include "GeneratorObjects/McEventCollection.h"
#include "TruthUtils/HepMCHelpers.h"
#include "CLHEP/Units/SystemOfUnits.h"

//--------------------------------------------------------------------------

HTFilter::HTFilter(const std::string& name, ISvcLocator* pSvcLocator)
   : GenFilter(name,pSvcLocator) 
   , m_total(0)
   , m_passed(0)
   , m_ptfailed(0)
{
  declareProperty("MinJetPt",m_MinJetPt = 0*CLHEP::GeV);  
  declareProperty("MaxJetEta",m_MaxJetEta = 10.0);
  declareProperty("TruthJetContainer", m_TruthJetContainerName = "AntiKt4TruthWZJets");
  declareProperty("MinHT",m_MinHT = 20.*CLHEP::GeV);
  declareProperty("MaxHT",m_MaxHT = 14000.*CLHEP::GeV);
  declareProperty("UseNeutrinosFromWZTau",m_UseNu = false, "Include neutrinos from W/Z/tau decays in the calculation of HT");
  declareProperty("UseLeptonsFromWZTau",m_UseLep = false, "Include e/mu from W/Z/tau decays in the HT");
  declareProperty("MinLeptonPt",m_MinLepPt = 0*CLHEP::GeV);
  declareProperty("MaxLeptonEta",m_MaxLepEta = 10.0);
}

//--------------------------------------------------------------------------

HTFilter::~HTFilter() {
}

//---------------------------------------------------------------------------

StatusCode HTFilter::filterInitialize() {
  m_MinJetPt /= CLHEP::GeV;
  m_MinLepPt /= CLHEP::GeV;
  m_MinHT /= CLHEP::GeV;
  m_MaxHT /= CLHEP::GeV;
  if (m_MaxHT<0) m_MaxHT=9e9;

  ATH_MSG_INFO( "Configured with " << m_MinJetPt << "<p_T GeV and abs(eta)<" << m_MaxJetEta << " for jets in " << m_TruthJetContainerName );
  ATH_MSG_INFO( "Will require H_T in range " << m_MinHT << " < H_T < " << m_MaxHT );
  if (m_UseNu)  ATH_MSG_INFO( " including neutrinos" );
  if (m_UseLep)  ATH_MSG_INFO( " including W/Z/tau leptons in range "  << m_MinLepPt << "<p_T GeV and abs(eta)<" << m_MaxLepEta );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode HTFilter::filterFinalize() {
  ATH_MSG_INFO( "Total efficiency: " << 100.*double(m_passed)/double(m_total) << "% (" 
                << 100.*double(m_ptfailed)/double(m_total) << "% failed p_T cuts)"  );
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode HTFilter::filterEvent() {
  m_total++; // Book keeping

  // Get jet container out
  const DataHandle<xAOD::JetContainer> truthjetTES = 0;
  if ( !evtStore()->contains<xAOD::JetContainer>( m_TruthJetContainerName ) ||
        evtStore()->retrieve( truthjetTES, m_TruthJetContainerName).isFailure() || !truthjetTES ){
    ATH_MSG_INFO( "No xAOD::JetContainer found in StoreGate with key " << m_TruthJetContainerName ); 
    setFilterPassed(m_MinHT<1.);  
    return StatusCode::SUCCESS;
  }

  // Get HT
  double HT = -1;
  for (xAOD::JetContainer::const_iterator it_truth = (*truthjetTES).begin(); it_truth != (*truthjetTES).end() ; ++it_truth) {
    if (!(*it_truth)) continue;
    if ( (*it_truth)->pt()>m_MinJetPt*CLHEP::GeV && std::abs((*it_truth)->eta())<m_MaxJetEta ) {
      ATH_MSG_VERBOSE("Adding truth jet with pt " << (*it_truth)->pt()
		      << ", eta " << (*it_truth)->eta()
		      << ", phi " << (*it_truth)->phi()
		      << ", nconst = " << (*it_truth)->numConstituents());
      HT += (*it_truth)->pt();
    }
  }

  // If we are asked to include neutrinos or leptons...
  if (m_UseLep || m_UseNu){  
    // Get MC event collection
    const DataHandle<McEventCollection> mecc = 0;
    if ( evtStore()->retrieve(mecc).isFailure() || !mecc || mecc->size()<1 || !((*mecc)[0]) ){
      ATH_MSG_WARNING( "Could not retrieve MC Event Collection - weight might not work" );
      return StatusCode::SUCCESS;
    }

    std::vector<HepMC::GenParticlePtr> WZleptons;
    WZleptons.reserve(10);

    for (auto  iter: *((*mecc)[0])){
      if ( !iter ) continue;
      int pdgid = iter->pdg_id();
      if (m_UseNu && MC::PID::isNeutrino(pdgid) && MC::isGenStable(iter)) {
	if( fromWZ(iter) || fromTau(iter) ) {
	  HT += iter->momentum().perp();
	}
      }
      // pick muons and electrons specifically -- isLepton selects both charged leptons and neutrinos
      if (m_UseLep && (std::abs(pdgid)==11 || std::abs(pdgid)==13) && MC::isGenStable(iter)
	  && (iter)->momentum().perp()>m_MinLepPt*CLHEP::GeV && std::abs(iter->momentum().eta())<m_MaxLepEta) {
	bool isFromWZ = fromWZ(iter);
	if( isFromWZ || fromTau(iter) ) {
	  ATH_MSG_VERBOSE("Adding W/Z/tau lepton with pt " << iter->momentum().perp()
			  << ", eta " << iter->momentum().eta()
			  << ", phi " << iter->momentum().phi()
			  << ", status " << iter->status()
			  << ", pdgId " << pdgid);
	  HT += iter->momentum().perp();
	}
      }
    }
  } // End need to access MC Event

  HT /= CLHEP::GeV; // Make sure we're in GeV
  ATH_MSG_DEBUG( "HT: " << HT );

  if (HT<m_MinHT || HT>=m_MaxHT){
    ATH_MSG_DEBUG( "Failed filter on HT: " << HT << " is not between " << m_MinHT << " and " << m_MaxHT );
    setFilterPassed(false);
  } else {
    // Made it to the end - success!
    m_passed++;
    setFilterPassed(true);
  }

  return StatusCode::SUCCESS;
}

bool HTFilter::fromWZ(HepMC::ConstGenParticlePtr part ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function
  //  it's used in place of code in MCTruthClassifier as long as this package is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Hit a hadron -> return false
  // Hit a parton -> return true
  //   This catch is important - we *cannot* look explicitly for the W or Z, because some
  //    generators do not include the W or Z in the truth record (like Sherpa)
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!part->production_vertex()) return false;
#ifdef HEPMC3
  for (auto  iter: part->production_vertex()->particles_in()){
    int parent_pdgid = iter->pdg_id();
    if (MC::PID::isW(parent_pdgid) || MC::PID::isZ(parent_pdgid)) return true;
    if (MC::PID::isHadron( parent_pdgid ) ) return false;
    if ( std::abs( parent_pdgid ) < 9 ) return true;
    if ( parent_pdgid == part->pdg_id() ) return fromWZ( iter );
  }
#else
  for (HepMC::GenVertex::particles_in_const_iterator iter=part->production_vertex()->particles_in_const_begin(); 
       iter!=part->production_vertex()->particles_in_const_end();++iter){
    int parent_pdgid = (*iter)->pdg_id();
    if (MC::PID::isW(parent_pdgid) || MC::PID::isZ(parent_pdgid)) return true;
    if (MC::PID::isHadron( parent_pdgid ) ) return false;
    if ( std::abs( parent_pdgid ) < 9 ) return true;
    if ( parent_pdgid == part->pdg_id() ) return fromWZ( *iter );
  }
#endif  
  return false;
}

bool HTFilter::fromTau(HepMC::ConstGenParticlePtr part ) const
{
  // !!! IMPORTANT !!! This is a TEMPORARY function
  //  it's used in place of code in MCTruthClassifier as long as this package is not dual-use
  //  when MCTruthClassifier is made dual-use, this function should be discarded.
  // see ATLJETMET-26
  //
  // Loop through parents
  // Find a tau -> return true
  // Find a hadron or parton -> return false
  //   This code, like the code before it, really assumes one incoming particle per vertex...
  if (!part->production_vertex()) return false;
#ifdef HEPMC3
  for (auto iter: part->production_vertex()->particles_in()){
    int parent_pdgid = iter->pdg_id();
    if ( std::abs( parent_pdgid ) == 15 ) return true;
    if (MC::PID::isHadron( parent_pdgid ) || std::abs( parent_pdgid ) < 9 ) return false;
    if ( parent_pdgid == part->pdg_id() ) return fromTau( iter );
  }
#else
  for (HepMC::GenVertex::particles_in_const_iterator iter=part->production_vertex()->particles_in_const_begin(); 
       iter!=part->production_vertex()->particles_in_const_end();++iter){
    int parent_pdgid = (*iter)->pdg_id();
    if ( std::abs( parent_pdgid ) == 15 ) return true;
    if (MC::PID::isHadron( parent_pdgid ) || abs( parent_pdgid ) < 9 ) return false;
    if ( parent_pdgid == part->pdg_id() ) return fromTau( *iter );
  }
#endif
  return false;
}
