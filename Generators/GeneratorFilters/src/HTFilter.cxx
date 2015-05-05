/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header for this module
#include "GeneratorFilters/HTFilter.h"

// Framework Related Headers
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// Used for retrieving the collection
#include "xAODJet/JetContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

// Other classes used by this class
#include "TruthUtils/PIDUtils.h"
#include "HepMC/GenEvent.h"
// #include "GeneratorObjects/McEventCollection.h"
#include "TruthUtils/HepMCHelpers.h"

//--------------------------------------------------------------------------

HTFilter::HTFilter(const std::string& name, ISvcLocator* pSvcLocator)
   : GenFilter(name,pSvcLocator) 
   , m_storeGate("StoreGateSvc",name)
   , m_total(0)
   , m_passed(0)
   , m_ptfailed(0)
{
  declareProperty("MinJetPt",m_MinJetPt = 0*CLHEP::GeV);  
  declareProperty("MaxJetEta",m_MaxJetEta = 10.0);
  declareProperty("TruthJetContainer", m_TruthJetContainerName = "AntiKt4TruthJets");
  declareProperty("MinHT",m_MinHT = 20.*CLHEP::GeV);
  declareProperty("MaxHT",m_MaxHT = 14000.*CLHEP::GeV);
  declareProperty("UseNeutrinos",m_UseNu = false, "Include neutrinos in the calculation of HT");
  // electrons and taus are in truth jets!
  declareProperty("UseMuons",m_UseMu = false, "Include muons from the MC event in the HT");
  declareProperty("MinMuonPt",m_MinMuPt = 0*CLHEP::GeV);
  declareProperty("MaxMuonEta",m_MaxMuEta = 10.0);

  m_log = new MsgStream(messageService(),name);
}

//--------------------------------------------------------------------------

HTFilter::~HTFilter() {
  if (m_log) delete m_log;
}

//---------------------------------------------------------------------------

StatusCode HTFilter::filterInitialize() {
  m_MinJetPt /= CLHEP::GeV;
  m_MinMuPt /= CLHEP::GeV;
  m_MinHT /= CLHEP::GeV;
  m_MaxHT /= CLHEP::GeV;
  if (m_MaxHT<0) m_MaxHT=9e9;

  ATH_MSG_INFO( "Configured with " << m_MinJetPt << "<p_T GeV and abs(eta)<" << m_MaxJetEta << " for jets in " << m_TruthJetContainerName );
  ATH_MSG_INFO( "Will require H_T in range " << m_MinHT << " < H_T < " << m_MaxHT );
  if (m_UseNu)  ATH_MSG_INFO( " including neutrinos" );
  if (m_UseMu)  ATH_MSG_INFO( " including muons in range "  << m_MinMuPt << "<p_T GeV and abs(eta)<" << m_MaxMuEta );

  if (m_storeGate.retrieve().isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve pointer to StoreGateSvc " << m_storeGate );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode HTFilter::filterFinalize() {
  (*m_log) << MSG::INFO << "Total efficiency: " << 100.*double(m_passed)/double(m_total) << "% (" 
           << 100.*double(m_ptfailed)/double(m_total) << "% failed p_T cuts)" << endreq;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode HTFilter::filterEvent() {
  m_total++; // Book keeping

  // Get jet container out
  const DataHandle<xAOD::JetContainer> truthjetTES = 0;
  if ( !m_storeGate->contains<xAOD::JetContainer>( m_TruthJetContainerName ) ||
        m_storeGate->retrieve( truthjetTES, m_TruthJetContainerName).isFailure() || !truthjetTES ){
    ATH_MSG_INFO( "No xAOD::JetContainer found in StoreGate with key " << m_TruthJetContainerName ); 
    setFilterPassed(m_MinHT<1.);  
    return StatusCode::SUCCESS;
  }

  // Get HT
  double HT = -1;
  for (xAOD::JetContainer::const_iterator it_truth = (*truthjetTES).begin(); it_truth != (*truthjetTES).end() ; ++it_truth) {
    if (!(*it_truth)) continue;
    if ( (*it_truth)->pt()>m_MinJetPt*CLHEP::GeV && fabs((*it_truth)->eta())<m_MaxJetEta ) {
      ATH_MSG_VERBOSE("Adding truth jet with pt " << (*it_truth)->pt()
		      << ", eta " << (*it_truth)->eta()
		      << ", phi " << (*it_truth)->phi());
      HT += (*it_truth)->pt();
    }
  }

  // If we are asked to include neutrinos or leptons...
  if (m_UseMu || m_UseNu){  
    // Get MC event collection
    const DataHandle<McEventCollection> mecc = 0;
    if ( m_storeGate->retrieve(mecc).isFailure() || !mecc || mecc->size()<1 || !((*mecc)[0]) ){
      ATH_MSG_WARNING( "Could not retrieve MC Event Collection - weight might not work" );
      return StatusCode::SUCCESS;
    }

    for (HepMC::GenEvent::particle_const_iterator iter=(*mecc)[0]->particles_begin(); iter!=(*mecc)[0]->particles_end();++iter){
      if ( !(*iter) ) continue;
      if (m_UseNu && MC::PID::isNeutrino((*iter)->pdg_id()) && MC::isGenStable(*iter))
	HT += (*iter)->momentum().perp();
      if (MC::PID::isLepton((*iter)->pdg_id()) && !MC::PID::isNeutrino((*iter)->pdg_id()) && MC::isGenStable(*iter)) {
	ATH_MSG_VERBOSE("Lepton with pdgId " << (*iter)->pdg_id() << ", pt " << (*iter)->momentum().perp()
			<< ", eta " << (*iter)->momentum().eta()
			<< ", phi " << (*iter)->momentum().phi());
      }
      if (m_UseMu  && fabs((*iter)->pdg_id())==13 && MC::isGenStable(*iter)
	  && (*iter)->momentum().perp()>m_MinMuPt*CLHEP::GeV && fabs((*iter)->momentum().eta())<m_MaxMuEta) {
	ATH_MSG_VERBOSE("Adding muon with pt " << (*iter)->momentum().perp()
			<< ", eta " << (*iter)->momentum().eta()
			<< ", phi " << (*iter)->momentum().phi());
	HT += (*iter)->momentum().perp();
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

