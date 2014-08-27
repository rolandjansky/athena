/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header for this module
#include "GeneratorFilters/HTFilter.h"

// Framework Related Headers
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// Used for retrieving the collection
#include "JetEvent/JetCollection.h"
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
  declareProperty("MinJetPt",m_MinPt = 0*CLHEP::GeV);  
  declareProperty("MaxJetEta",m_MaxEta = 10.0);
  declareProperty("TruthJetContainer", m_TruthJetContainerName = "AntiKt4TruthJets");
  declareProperty("MinHT",m_MinHT = 20.*CLHEP::GeV);
  declareProperty("MaxHT",m_MaxHT = 14000.*CLHEP::GeV);
  declareProperty("UseNeutrinos",m_UseNu = false, "Include neutrinos in the calculation of HT");
  declareProperty("UseLeptons",m_UseL = false, "Include leptons from the MC event in the HT");

  m_log = new MsgStream(messageService(),name);
}

//--------------------------------------------------------------------------

HTFilter::~HTFilter() {
  if (m_log) delete m_log;
}

//---------------------------------------------------------------------------

StatusCode HTFilter::filterInitialize() {
  m_MinPt /= CLHEP::GeV;
  m_MinHT /= CLHEP::GeV;
  m_MaxHT /= CLHEP::GeV;
  if (m_MaxHT<0) m_MaxHT=9e9;

  (*m_log) << MSG::INFO << "Configured with " << m_MinPt << "<p_T GeV and abs(eta)<" << m_MaxEta << " for jets in " << m_TruthJetContainerName << endreq;
  (*m_log) << MSG::INFO << "Will require H_T in range " << m_MinHT << " < H_T < " << m_MaxHT;
  if (m_UseNu) (*m_log) << MSG::INFO << " including neutrinos";
  if (m_UseL)  (*m_log) << MSG::INFO << " including e/mu" << endreq;

  if (m_storeGate.retrieve().isFailure()) {
    (*m_log) << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc " << m_storeGate << endreq;
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
  const DataHandle<JetCollection> truthjetTES = 0;
  if ( !m_storeGate->contains<JetCollection>( m_TruthJetContainerName ) ||
        m_storeGate->retrieve( truthjetTES, m_TruthJetContainerName).isFailure() || !truthjetTES ){
    (*m_log) << MSG::INFO << "No JetCollection found in StoreGate with key " << m_TruthJetContainerName << endreq; 
    setFilterPassed(m_MinHT<1.);  
    return StatusCode::SUCCESS;
  }

  // Get HT
  double HT = -1;
  for (JetCollection::const_iterator it_truth = (*truthjetTES).begin(); it_truth != (*truthjetTES).end() ; ++it_truth) {
    if (!(*it_truth)) continue;
    if ( (*it_truth)->pt()>m_MinPt*CLHEP::GeV && abs((*it_truth)->eta())<m_MaxEta ) HT += (*it_truth)->pt();
  }

  // If we are asked to include neutrinos or leptons...
  if (m_UseL || m_UseNu){  
    // Get MC event collection
    const DataHandle<McEventCollection> mecc = 0;
    if ( m_storeGate->retrieve( mecc).isFailure() || !mecc || mecc->size()<1 || !((*mecc)[0]) ){
      (*m_log) << MSG::WARNING << "Could not retrieve MC Event Collection - weight might not work" << endreq;
      return StatusCode::SUCCESS;
    }

    for (HepMC::GenEvent::particle_const_iterator iter=(*mecc)[0]->particles_begin(); iter!=(*mecc)[0]->particles_end();++iter){
      if ( !(*iter) ) continue;
      if (m_UseNu && MC::PID::isNeutrino((*iter)->pdg_id()) && MC::isGenStable(*iter)) HT += (*iter)->momentum().perp();
      if (m_UseL  && MC::PID::isLepton((*iter)->pdg_id()) && !MC::PID::isNeutrino((*iter)->pdg_id()) && MC::isGenStable(*iter)) HT += (*iter)->momentum().perp();
    }
  } // End need to access MC Event

  HT /= CLHEP::GeV; // Make sure we're in GeV

  if (HT<m_MinHT || HT>=m_MaxHT){
    if (m_log->level()<=MSG::DEBUG) (*m_log) << MSG::DEBUG << "Failed filter on HT: " << HT << " is not between " << m_MinHT << " and " << m_MaxHT << endreq;
    setFilterPassed(false);
} else {
    // Made it to the end - success!
    m_passed++;
    setFilterPassed(true);
  }

  return StatusCode::SUCCESS;
}

