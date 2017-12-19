/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filter to suppress events in the low pt/deltaY region.
#include "GeneratorFilters/JetForwardFilter.h"
#include "xAODJet/JetContainer.h"
#include "TRandom3.h"

// Pt  High --> Low
/// @todo Move to a util library
class High2LowByJetClassPt {
public:
  bool operator () (const xAOD::Jet *t1, const xAOD::Jet *t2) const {
    return (t1->pt() > t2->pt());
  }
};


JetForwardFilter::JetForwardFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter (name, pSvcLocator)
{
  std::vector<double> empty;

  // General Jets
  declareProperty("JetContainer", m_jetContainer = "AntiKt6TruthJets");

  // Allow a hard cut on the jet system
  declareProperty("MinDeltaY", m_minDeltaY = 0.0); //absolute deltaY!
  declareProperty("MaxDeltaY", m_maxDeltaY = 100.0);
  declareProperty("MinPt1", m_minPt1 = 12.0);//In CLHEP::GeV
  declareProperty("MaxPt1", m_maxPt1 = 70000.0);//In CLHEP::GeV
  declareProperty("MinPt2", m_minPt2 = 12.0);//In CLHEP::GeV
  declareProperty("MaxPt2", m_maxPt2 = 70000.0);//In CLHEP::GeV

  // Allow an arbitrary number of Gaussian distributions to describe the delta(y)-pt distribution
  declareProperty("muXs", m_muXs = empty );
  declareProperty("muYs", m_muYs = empty );
  declareProperty("sigmaXs", m_sigmaXs = empty );
  declareProperty("sigmaYs", m_sigmaYs = empty );
  declareProperty("rhos", m_rhos = empty );
  declareProperty("weights", m_weights = empty );

  declareProperty("SuppressionFactor", m_suppressionFactor = 1.0);

  m_myRandGen = 0;
}


StatusCode JetForwardFilter::filterInitialize() {
  ATH_MSG_INFO("xAOD::JetContainer: " <<  m_jetContainer);
  ATH_MSG_INFO("Jet 1 Min Pt: " <<  m_minPt1);
  ATH_MSG_INFO("Jet 1 Max Pt: " <<  m_maxPt1);
  ATH_MSG_INFO("Jet 2 Min Pt: " <<  m_minPt2);
  ATH_MSG_INFO("Jet 2 Max Pt: " <<  m_maxPt2);
  ATH_MSG_INFO("Min |Delta Y|: " <<  m_minDeltaY);
  ATH_MSG_INFO("Max |Delta Y|: " <<  m_maxDeltaY);

  // Output the setup for each of the gaussian distributions
  if (m_muXs.size() != m_muYs.size() ||
      m_muYs.size() != m_sigmaXs.size() ||
      m_sigmaXs.size() != m_sigmaYs.size() ||
      m_sigmaYs.size() != m_rhos.size() || m_rhos.size() != m_weights.size() ) {
    ATH_MSG_ERROR("Mismatch in size of gaussian parameter arrays");
    return StatusCode::FAILURE;
  }

  for (unsigned int i = 0; i < m_muXs.size(); ++i) {
    ATH_MSG_INFO("For gaus[" << i << "]: -");
    ATH_MSG_INFO("  Mean x = " << m_muXs[i]);
    ATH_MSG_INFO("  Mean y = " << m_muYs[i]);
    ATH_MSG_INFO("  Sigma x = " << m_sigmaXs[i]);
    ATH_MSG_INFO("  Sigma y = " << m_sigmaYs[i]);
    ATH_MSG_INFO("  Rho = " << m_rhos[i]);
    ATH_MSG_INFO("  Weight = " << m_weights[i]);
  }
  ATH_MSG_INFO("  Suppression factor = " << m_suppressionFactor);

  // Setup the random number generator for weighting
  m_myRandGen = new TRandom3();
  m_myRandGen->SetSeed(0); // completely random!

  return StatusCode::SUCCESS;
}


StatusCode JetForwardFilter::filterFinalize() {
  delete m_myRandGen;
  return StatusCode::SUCCESS;
}


StatusCode JetForwardFilter::filterEvent() {
  // Get TruthJets
  const xAOD::JetContainer* truthjetTES;
  CHECK(evtStore()->retrieve(truthjetTES, m_jetContainer));
  ATH_MSG_INFO("xAOD::JetContainer Size = " << truthjetTES->size());

  // Get a list of all the truth jets, sorted by pT
  std::vector<const xAOD::Jet*> jetList(truthjetTES->begin(), truthjetTES->end());
  std::sort(jetList.begin(), jetList.end(), High2LowByJetClassPt());

  // Number of jets (need at least two)
  int flagNJets = (int(jetList.size()) < 2) ? 0 : 1;

  // Leading 1st jet
  int flag1stJet = -1;
  double jetPt1 = -1.0;
  if (jetList.size() >= 1) {
    const xAOD::Jet* j1 = jetList[0];
    jetPt1 = j1->pt()/1000.0;
    flag1stJet = 1;
    if (j1->pt()/1000.0 < m_minPt1) flag1stJet = 0;
    if (j1->pt()/1000.0 > m_maxPt1) flag1stJet = 0;
  }

  // Leading 2nd jet
  int flag2ndJet = -1;
  if (jetList.size() >= 2) {
    const xAOD::Jet* j2 = jetList[1];
    flag2ndJet = 1;
    if (j2->pt()/1000.0 < m_minPt2) flag2ndJet = 0;
    if (j2->pt()/1000.0 > m_maxPt2) flag2ndJet = 0;
  }

  // DeltaY
  int flagJJ = -1;
  double intervalSize = -100.0;
  if (jetList.size() >= 2) {
    const xAOD::Jet *j1 = jetList[0];
    const xAOD::Jet *j2 = jetList[1];
    flagJJ = 1;
    intervalSize = std::fabs(j1->rapidity()-j2->rapidity());
    if (intervalSize < m_minDeltaY) flagJJ = 0;
    if (intervalSize > m_maxDeltaY) flagJJ = 0;
  }

  // If two jets are present calculate the weight (and test for rejection)
  double weighting = 1.0;
  int flagW = -1;
  if (intervalSize > 0.0 && jetPt1 > 0.0) {
    weighting = calculateProbToKeep(intervalSize, jetPt1);
    flagW = 0;
    if (weighting > m_myRandGen->Rndm()) flagW = 1;
  }

  ATH_MSG_INFO("NJets  OK? : " << flagNJets);
  ATH_MSG_INFO("1stJet OK? : " << flag1stJet);
  ATH_MSG_INFO("2ndJet OK? : " << flag2ndJet);
  ATH_MSG_INFO("DeltaY OK? : " << flagJJ);
  ATH_MSG_INFO("Weight OK? : " << flagW);

  if (flagNJets != 0 && flag1stJet != 0 && flag2ndJet != 0 && flagJJ != 0 && flagW != 0) {
    ATH_MSG_DEBUG("Pass filter");
    // If passed then add a weight!
    for (HepMC::GenEvent* genEvt : *events()) {
      // Store the inverse of the weighting for conventions sake
      genEvt->weights().push_back(1.0/weighting);
    }
    return StatusCode::SUCCESS;
  }

  setFilterPassed(false);
  ATH_MSG_INFO("Drop event");
  return StatusCode::SUCCESS;
}


double JetForwardFilter::evaluatePDF(double x, double y, int gausIndex) {
  double muX = m_muXs[gausIndex];
  double muY = m_muYs[gausIndex];
  double sigmaX = m_sigmaXs[gausIndex];
  double sigmaY = m_sigmaYs[gausIndex];
  double rho = m_rhos[gausIndex];
  double normFactor = 1.0/(2.0*M_PI*sigmaX*sigmaY*sqrt(1-rho*rho));

  // Split up bits of the exponential exp((-1/valA)*(valB+valC-valD))
  double valA = 2.0*(1-rho*rho);
  double valB = pow(x-muX, 2)/pow(sigmaX, 2);
  double valC = pow(y-muY, 2)/pow(sigmaY, 2);
  double valD = 2.0*rho*(x-muX)*(y-muY)/(sigmaX*sigmaY);

  double pdf = normFactor*exp( (-1.0/valA)*(valB+valC-valD)  );
  return pdf;
}


double JetForwardFilter::calculateProbToKeep(double absoluteDeltaY, double leadJetPt) {
  // Sum up the pdf for each gaussian distribution
  double pdfSum = 0.0;
  for (size_t g = 0; g < m_muXs.size(); g++) {
    pdfSum += m_weights[g]*evaluatePDF(leadJetPt, absoluteDeltaY, g);
  }

  // Return the ratio to suppression point to define the event weight
  return (m_suppressionFactor >= pdfSum) ? 1.0 : m_suppressionFactor/pdfSum; // 0.0->1.0
}
