/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/
#include "GeneratorFilters/QCDTruthJetFilter.h"
#include "xAODJet/JetContainer.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "TRandom3.h"
#include "AthenaKernel/IAtRndmGenSvc.h" // For random numbers...
#include "CLHEP/Random/RandomEngine.h"
#include "EventInfo/EventInfo.h" // For setting the weight
#include "EventInfo/EventType.h" // From event info - the real holder of the event weight
#include "TF1.h" // For holding the weighting function

QCDTruthJetFilter::QCDTruthJetFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
  , m_rand("AtRndmGenSvc",name)
  , m_total(0)
  , m_passed(0)
  , m_ptfailed(0)
  , m_norm(1.)
  , m_high(1.)
{
  m_StartMinEta=-10;
  declareProperty("MinPt",m_MinPt = -1.*Gaudi::Units::GeV);
  declareProperty("MaxPt",m_MaxPt = 7000*Gaudi::Units::GeV); // LHC kinematic limit...
  declareProperty("SymEta",m_SymEta = false);
  declareProperty("MaxEta",m_MaxEta = 999.0);
  declareProperty("MinEta",m_MinEta = m_StartMinEta);
  declareProperty("TruthJetContainer", m_TruthJetContainerName = "AntiKt6TruthJets");
  declareProperty("DoShape",m_doShape = false);
  declareProperty("MinPhi",m_MinPhi = -999.0);
  declareProperty("MaxPhi",m_MaxPhi = 999.0);
}


StatusCode QCDTruthJetFilter::filterInitialize() {
  m_MinPt /= Gaudi::Units::GeV;
  m_MaxPt /= Gaudi::Units::GeV;
  if (m_MinEta == m_StartMinEta && m_MaxEta>0) m_MinEta = -1.*m_MaxEta;
  //  ATH_MSG_INFO("Configured with " << m_MinPt << "<p_T<" << m_MaxPt << " GeV and" << m_MinEta << " <|eta|<" << m_MaxEta << " for jets in " << m_TruthJetContainerName);
  if (m_SymEta){
    ATH_MSG_INFO("Configured with " << m_MinPt << "<p_T<" << m_MaxPt << " GeV and " << m_MinEta << "<abs(eta)<" << m_MaxEta << " for jets in " << m_TruthJetContainerName);
  } else {
    ATH_MSG_INFO("Configured with " << m_MinPt << "<p_T<" << m_MaxPt << " GeV and " << m_MinEta << "<eta and abs(eta)<" << m_MaxEta << " for jets in " << m_TruthJetContainerName);
  }

  // Special cases: min pT is above 2 TeV or max pT is below 20 GeV, use no weighting - hard-coded range of the fit in this filter
  if (m_MinPt < 1999. && m_MaxPt > 21. && m_doShape) {
    // Use the fit in all its glory
    m_high = fitFn(m_MaxPt);
    m_norm = 1./(m_high==0?1.:m_high); //(m_MaxPt-m_MinPt) / (m_norm!=0?m_norm:1.);
    ATH_MSG_INFO("Initialized and set high to " << m_high << " and norm to " << m_norm);
  } else if (m_doShape) {
    ATH_MSG_INFO("Requested shaping with bounds of " << m_MinPt << " , " << m_MaxPt << " which cannot be done.  Turning off shaping (sorry).");
    m_doShape=false;
  }

  if (m_MinPhi > -998.0 || m_MaxPhi < 998.0) {
    ATH_MSG_INFO("Configured phi as well with min = " << m_MinPhi << " and max = " << m_MaxPhi);
  }

  return StatusCode::SUCCESS;
}


StatusCode QCDTruthJetFilter::filterFinalize() {
  ATH_MSG_INFO("Total efficiency: " << 100.*double(m_passed)/double(m_total) << "% (" << 100.*double(m_ptfailed)/double(m_total) << "% failed pT cuts)");
  return StatusCode::SUCCESS;
}


StatusCode QCDTruthJetFilter::filterEvent() {
  m_total++; // Bookkeeping


  // Grab random number engine
  CLHEP::HepRandomEngine* rndm = m_rand->GetEngine("QCDTruthJetFilter");
  if (!rndm) {
    ATH_MSG_WARNING("Failed to retrieve random number engine QCDTruthJetFilter");
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // Get jet container out
  const DataHandle<xAOD::JetContainer> truthjetTES = 0;
  if (!evtStore()->contains<xAOD::JetContainer>(m_TruthJetContainerName) ||
      evtStore()->retrieve(truthjetTES, m_TruthJetContainerName).isFailure() || !truthjetTES) {
    ATH_MSG_ERROR("No xAOD::JetContainer found in StoreGate with key " << m_TruthJetContainerName);
    setFilterPassed(m_MinPt < 1);
    return StatusCode::SUCCESS;
  }

  // Get pT and phi of leading jet
  double pt_lead = -1, phi_lead = 0;
  for (xAOD::JetContainer::const_iterator it_truth = (*truthjetTES).begin(); it_truth != (*truthjetTES).end() ; ++it_truth) {
    if (!(*it_truth)) continue;
//    if (fabs( (*it_truth)->eta() ) > m_MaxEta) continue;
//    if ((*it_truth)->eta()  > m_MaxEta || (*it_truth)->eta()  <= m_MinEta) continue;
    if ((*it_truth)->eta()>m_MaxEta || ( (*it_truth)->eta()<=m_MinEta && !m_SymEta) || ((*it_truth)->eta()<=m_MinEta && (*it_truth)->eta()>=-m_MinEta && m_SymEta)) continue;
    if (pt_lead < (*it_truth)->pt()) {
      pt_lead = (*it_truth)->pt();
      phi_lead = (*it_truth)->phi();
    }
  }
  pt_lead /= Gaudi::Units::GeV; // Make sure we're in GeV

  // See if the leading jet is in the right range
  if ((pt_lead<=m_MinPt || (pt_lead>m_MaxPt && m_MaxPt>0)) && !(pt_lead<=m_MinPt && m_MinPt<1)) {
    m_ptfailed++;
    setFilterPassed(false);
    ATH_MSG_DEBUG("Failed filter on jet pT: " << pt_lead << " is not between " << m_MinPt << " and " << m_MaxPt);
    return StatusCode::SUCCESS;
  }

  // If appropriate, check the phi of the lead jet as well
  if (m_MinPhi > -999.0 || m_MaxPhi < 999.0) {
    setFilterPassed(false);

    if (phi_lead < m_MinPhi || phi_lead > m_MaxPhi) {
      ATH_MSG_DEBUG("Failed filter on jet phi: " << phi_lead << " not between " << m_MinPhi << " and " << m_MaxPhi);
      return StatusCode::SUCCESS;
    }
    else {
      setFilterPassed(true);
    }
  }



  // Unweight the pT spectrum
  double w = 1.;
  if (m_doShape) w = fitFn(pt_lead);
  double rnd = rndm->flat();
  if (m_high/w < rnd) {
    setFilterPassed(false);
    ATH_MSG_DEBUG("Event failed weighting cut. Weight is " << w << " for pt_lead of " << pt_lead << " high end is " << m_high << " rnd is " << rnd);
    return StatusCode::SUCCESS;
  }

  
  // Made it to the end - success!
  m_passed++;
  setFilterPassed(true);

  // Get MC event collection for setting weight
  const DataHandle<McEventCollection> mecc = 0;
  CHECK(evtStore()->retrieve(mecc, m_mcEventKey));
  ATH_MSG_DEBUG("Event passed.  Will mod event weights by " << w*m_norm << " for pt_lead of " << pt_lead << " norm " << m_norm << " w " << w << " high " << m_high << " rnd " << rnd);
  double orig = 1.;
  McEventCollection* mec = const_cast<McEventCollection*> (&(*mecc));
  for (unsigned int i=0;i<mec->size();++i) {
    if ( !(*mec)[i] ) continue;
    orig = (*mec)[i]->weights().size()>0?(*mec)[i]->weights()[0]:1.;
    if ((*mec)[i]->weights().size()>0) (*mec)[i]->weights()[0] = orig*w*m_norm;
    else (*mec)[i]->weights().push_back( w*m_norm*orig );
  }
  return StatusCode::SUCCESS;
}
