/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/QCDTruthMultiJetFilter.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "xAODJet/JetContainer.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h" // For random numbers...
#include "EventInfo/EventInfo.h" // For setting the weight
#include "EventInfo/EventType.h" // From event info - the real holder of the event weight
#include "TF1.h" // For holding the weighting function

QCDTruthMultiJetFilter::QCDTruthMultiJetFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
  , m_rand("AtRndmGenSvc",name)
  , m_total(0)
  , m_passed(0)
  , m_ptfailed(0)
  , m_nJetsFailed(0)
  , m_norm(1.)
  , m_high(1.)
{
  declareProperty("Njet",m_Njet = -1.);
  declareProperty("NjetMinPt",m_NjetMinPt = 0*Gaudi::Units::GeV);
  declareProperty("MinLeadJetPt",m_MinLeadJetPt = 0*Gaudi::Units::GeV);
  declareProperty("MaxLeadJetPt",m_MaxLeadJetPt = 7000*Gaudi::Units::GeV); // LHC kinematic limit...
  declareProperty("MaxEta",m_MaxEta = 10.0);
  declareProperty("TruthJetContainer", m_TruthJetContainerName = "AntiKt6TruthJets");
  declareProperty("DoShape",m_doShape = true);
}


StatusCode QCDTruthMultiJetFilter::filterInitialize() {
  m_NjetMinPt /= Gaudi::Units::GeV;
  m_MinLeadJetPt /= Gaudi::Units::GeV;
  m_MaxLeadJetPt /= Gaudi::Units::GeV;

  ATH_MSG_INFO("Configured with " << m_Njet << " jets with p_T>" << m_NjetMinPt << " GeV, " << "|eta|<" << m_MaxEta << " and " << m_MinLeadJetPt << "<lead jet p_T<" << m_MaxLeadJetPt << " for jets in " << m_TruthJetContainerName);

  // Special cases: min pT is above 2 TeV or max pT is below 20 GeV, use no weighting - hard-coded range of the fit in this filter
  if (m_MinLeadJetPt < 1999. && m_MaxLeadJetPt > 21. && m_doShape) {
    // Use the fit in all its glory
    m_high = fitFn(m_MaxLeadJetPt);
    m_norm = 1./(m_high==0?1.:m_high); //(m_MaxLeadJetPt-m_MinLeadJetPt) / (m_norm!=0?m_norm:1.);
    ATH_MSG_INFO("Initialized and set high to " << m_high << " and norm to " << m_norm);
  } else if (m_doShape) {
    ATH_MSG_INFO("Requested shaping with bounds of " << m_MinLeadJetPt << " , " << m_MaxLeadJetPt << " which cannot be done.  Turning off shaping (sorry).");
    m_doShape=false;
  }
  return StatusCode::SUCCESS;
}


StatusCode QCDTruthMultiJetFilter::filterFinalize() {
  ATH_MSG_INFO("Total efficiency: " << 100.*double(m_passed)/double(m_total) << "% (" << 100.*double(m_ptfailed)/double(m_total) << "% failed lead pT cut, " << 100.*double(m_nJetsFailed)/double(m_total) << "% failed nJets cut)");
  return StatusCode::SUCCESS;
}


StatusCode QCDTruthMultiJetFilter::filterEvent() {
  m_total++; // Bookkeeping

  // Grab random number engine - this is kept with the QCDTruthJetFilter engine
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
    setFilterPassed(m_MinLeadJetPt < 1);
    return StatusCode::SUCCESS;
  }

  // Count jets above m_NjetMinPt and find pT of leading jet
  int Njet=0;
  double pt_lead = -1;
  for (xAOD::JetContainer::const_iterator it_truth = (*truthjetTES).begin(); it_truth != (*truthjetTES).end() ; ++it_truth) {
    if (!(*it_truth)) continue;
    if (std::abs( (*it_truth)->eta() ) > m_MaxEta) continue;
    if ((*it_truth)->pt() > m_NjetMinPt*Gaudi::Units::GeV) Njet++;
    if (pt_lead < (*it_truth)->pt()) pt_lead = (*it_truth)->pt();
  }
  pt_lead /= Gaudi::Units::GeV; // Make sure we're in GeV

  // See if the leading jet is in the right range
  if (Njet < m_Njet && m_Njet > 0) {
    m_nJetsFailed++;
    setFilterPassed(false);
    ATH_MSG_DEBUG("Failed filter on: " << Njet << " jets found with pT>" << m_NjetMinPt << " GeV -> less than required " << m_Njet << " jets");
    return StatusCode::SUCCESS;
  }

  // See if the leading jet is in the right range
  if ((pt_lead<=m_MinLeadJetPt || (pt_lead>m_MaxLeadJetPt && m_MaxLeadJetPt>0)) && !(pt_lead<=m_MinLeadJetPt && m_MinLeadJetPt<1)) {
    m_ptfailed++;
    setFilterPassed(false);
    ATH_MSG_DEBUG("Failed filter on jet pT: " << pt_lead << " is not between " << m_MinLeadJetPt << " and " << m_MaxLeadJetPt);
    return StatusCode::SUCCESS;
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
  CHECK(evtStore()->retrieve(mecc));
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
