/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/PhysicalConstants.h"
#include "GeneratorFilters/JetIntervalFilter.h"
#include "xAODJet/JetContainer.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <cmath>
#include "TRandom3.h"

// Pt High --> Low
/// @todo Move to a generator utils library
class High2LowByJetClassPt {
public:
  bool operator () (const xAOD::Jet *t1, const xAOD::Jet *t2) const {
    return (t1->pt() > t2->pt());
  }
};

// Et High --> Low
/// @todo Move to a generator utils library
class High2LowByJetClassEt {
public:
  bool operator () (const xAOD::Jet *t1, const xAOD::Jet *t2) const {
    return ((t1->p4()).Et() > (t2->p4()).Et());
  }
};


JetIntervalFilter::JetIntervalFilter(const std::string & name, ISvcLocator * pSvcLocator)
  : GenFilter (name, pSvcLocator)
{
  // General Jets
  declareProperty("JetContainer", m_jetContainer = "AntiKt4TruthJets");
  declareProperty("JetNumber", m_jetNumber = 2);
  declareProperty("OppositeSideJets", m_jetOppositeSide = false);

  // Jet Energy Cuts (give choice of et or pt cutting)
  declareProperty("JetCutByPt", m_jetCutByPt = true);
  declareProperty("Jet1MinEt", m_jet1MinEt = 10.0*Gaudi::Units::GeV);
  declareProperty("Jet1MaxEt", m_jet1MaxEt = 1000.0*Gaudi::Units::GeV);
  declareProperty("Jet1MinPt", m_jet1MinPt = 10.0*Gaudi::Units::GeV);
  declareProperty("Jet1MaxPt", m_jet1MaxPt = 1000.0*Gaudi::Units::GeV);
  declareProperty("Jet2MinEt", m_jet2MinEt = 10.0*Gaudi::Units::GeV);
  declareProperty("Jet2MaxEt", m_jet2MaxEt = 1000.0*Gaudi::Units::GeV);
  declareProperty("Jet2MinPt", m_jet2MinPt = 10.0*Gaudi::Units::GeV);
  declareProperty("Jet2MaxPt", m_jet2MaxPt = 1000.0*Gaudi::Units::GeV);

  // Jet Position Cuts (absolute)
  declareProperty("Jet1MaxEta", m_jet1MaxEta = 100.0);
  declareProperty("Jet1MinEta", m_jet1MinEta = 0.0);
  declareProperty("Jet2MaxEta", m_jet2MaxEta = 100.0);
  declareProperty("Jet2MinEta", m_jet2MinEta = 0.0);

  // Jet delta eta cuts
  declareProperty("MinDeltaEta", m_minDeltaEta = 0.0);
  declareProperty("MaxDeltaEta", m_maxDeltaEta = 100.0);

  // Allow filtering with weight storage
  // Default only good for J2 sample
  declareProperty("WeightEvents", m_weightingEvents = false );
  declareProperty("UniformMaxWeightBelowGausMean", m_flatWeightBelowMean = false );
  declareProperty("GausMean", m_gausMean=6.11*std::pow(10.0,-2.0) );
  declareProperty("GausSigma", m_gausSigma=2.33*std::pow(10.0,0.0) );
  declareProperty("AbsDEtaCutOff", m_absDEtaCutOff=8.5); //Above which keep all which are generated

  m_myRandGen = 0;
}


StatusCode JetIntervalFilter::filterInitialize() {
  // Output settings to screen
  ATH_MSG_INFO("xAOD::JetContainer: " <<  m_jetContainer);
  ATH_MSG_INFO("Jet Number: " <<  m_jetNumber);
  ATH_MSG_INFO("Opposite Side Jets: " <<  m_jetOppositeSide);

  // Jet Energy Cuts (give choice of et or pt cutting)
  ATH_MSG_INFO("Jets Cut By Pt: " <<  m_jetCutByPt);
  ATH_MSG_INFO("Jet 1 Min Et: " <<  m_jet1MinEt);
  ATH_MSG_INFO("Jet 1 Max Et: " <<  m_jet1MaxEt);
  ATH_MSG_INFO("Jet 1 Min Pt: " <<  m_jet1MinPt);
  ATH_MSG_INFO("Jet 1 Max Pt: " <<  m_jet1MaxPt);
  ATH_MSG_INFO("Jet 2 Min Et: " <<  m_jet2MinEt);
  ATH_MSG_INFO("Jet 2 Max Et: " <<  m_jet2MaxEt);
  ATH_MSG_INFO("Jet 2 Min Pt: " <<  m_jet2MinPt);
  ATH_MSG_INFO("Jet 2 Max Pt: " <<  m_jet2MaxPt);

  // Jet Position Cuts (absolute)
  ATH_MSG_INFO("Jet 1 Max Eta: " <<  m_jet1MaxEta);
  ATH_MSG_INFO("Jet 1 Min Eta: " <<  m_jet1MinEta);
  ATH_MSG_INFO("Jet 2 Max Eta: " <<  m_jet2MaxEta);
  ATH_MSG_INFO("Jet 2 Min Eta: " <<  m_jet2MinEta);

  // Jet delta eta cuts
  ATH_MSG_INFO("Min Delta Eta: " <<  m_minDeltaEta);
  ATH_MSG_INFO("Max Delta Eta: " <<  m_maxDeltaEta);

  // Event Weighting
  ATH_MSG_INFO("Weighting events: " << m_weightingEvents);
  ATH_MSG_INFO("Uniform maximum weight below mean: " << m_flatWeightBelowMean);
  ATH_MSG_INFO("GausMean: " << m_gausMean);
  ATH_MSG_INFO("GausSigma: " << m_gausSigma);
  ATH_MSG_INFO("AbsDEtaCutOff: " << m_absDEtaCutOff);

  // Set up the random number generator for weighting
  if (m_weightingEvents) {
    m_myRandGen = new TRandom3();
    m_myRandGen->SetSeed(0); // completely random!
  }

  return StatusCode::SUCCESS;
}


StatusCode JetIntervalFilter::filterFinalize() {
  // Get rid of the random number generator
  delete m_myRandGen;
  return StatusCode::SUCCESS;
}


StatusCode JetIntervalFilter::filterEvent() {
  // Get TruthJets
  ATH_MSG_DEBUG("Get xAOD::JetContainer");
  const xAOD::JetContainer* truthjetTES;
  CHECK(evtStore()->retrieve(truthjetTES, m_jetContainer));
  ATH_MSG_INFO("xAOD::JetContainer Size = " << truthjetTES->size());

  // Get a list of all the truth jets
  std::vector<const xAOD::Jet*> jetList(truthjetTES->begin(), truthjetTES->end());

  // Sort jets by Pt or Et
  if (m_jetCutByPt) {
    std::sort(jetList.begin(), jetList.end(), High2LowByJetClassPt());
  } else {
    std::sort(jetList.begin(), jetList.end(), High2LowByJetClassEt());
  }

  // Number of jets
  int flagNJets = -1;
  if (m_jetNumber >= 1) {
    flagNJets = int(jetList.size()) >= m_jetNumber ? 1 : 0;
  }

  // Leading 1st jet
  int flag1stJet = -1;
  if (jetList.size() >= 1) {
    const xAOD::Jet *j1 = jetList[0];
    flag1stJet = 1;
    if (m_jetCutByPt) {
      if (j1->pt() < m_jet1MinPt) {
        flag1stJet = 0;
      }
      if (j1->pt() > m_jet1MaxPt) {
        flag1stJet = 0;
      }
    } else {
      if (j1->p4().Et() < m_jet1MinEt) {
        flag1stJet = 0;
      }
      if (j1->p4().Et() > m_jet1MaxEt) {
        flag1stJet = 0;
      }
    }

    if ( std::fabs(j1->eta()) < m_jet1MinEta ) {
      flag1stJet = 0;
    }
    if ( std::fabs(j1->eta()) > m_jet1MaxEta ) {
      flag1stJet = 0;
    }
  }

  // 2nd jet
  int flag2ndJet = -1;
  if (jetList.size() >= 2) {
    const xAOD::Jet *j2 = jetList[1];
    flag2ndJet = 1;
    if (m_jetCutByPt) {
      if (j2->pt() < m_jet2MinPt) {
        flag2ndJet = 0;
      }
      if (j2->pt() > m_jet2MaxPt) {
        flag2ndJet = 0;
      }
    } else {
      if (j2->p4().Et() < m_jet2MinEt) {
        flag2ndJet = 0;
      }
      if (j2->p4().Et() > m_jet2MaxEt) {
        flag2ndJet = 0;
      }
    }

    if ( std::fabs(j2->eta()) < m_jet2MinEta ) {
      flag2ndJet = 0;
    }
    if ( std::fabs(j2->eta()) > m_jet2MaxEta ) {
      flag2ndJet = 0;
    }
  }


  // Sign of Eta (requires at least two jets as well)
  int flagSign = -1;
  if (m_jetOppositeSide) {
    flagSign = 0;
    if (jetList.size() >= 2) {
      const xAOD::Jet *j1 = jetList[0];
      const xAOD::Jet *j2 = jetList[1];
      if (j1->eta()*j2->eta() < 0.) flagSign = 1;
    }
  }


  // DeltaEta
  int flagJJ = -1;
  double intervalSize = -100.0;
  if (jetList.size() >= 2) {
    const xAOD::Jet *j1 = jetList[0];
    const xAOD::Jet *j2 = jetList[1];
    flagJJ = 1;
    intervalSize = std::fabs(j1->eta()-j2->eta());
    if (intervalSize < m_minDeltaEta) {
      flagJJ = 0;
    }
    if (intervalSize > m_maxDeltaEta) {
      flagJJ = 0;
    }

  }

  //If interval size greater than 0 then calculate a proability for it to pass
  double weighting = 1.0;
  int flagW = -1;
  if (m_weightingEvents && intervalSize > 0.0) {
    weighting = calculateProbToKeep(intervalSize);
    flagW = 0;
    if (weighting > m_myRandGen->Rndm()) {
      flagW = 1;
    }
  }

  ATH_MSG_INFO("NJets  OK? : " << flagNJets);
  ATH_MSG_INFO("1stJet OK? : " << flag1stJet);
  ATH_MSG_INFO("2ndJet OK? : " << flag2ndJet);
  ATH_MSG_INFO("Sign   OK? : " << flagSign);
  ATH_MSG_INFO("JJ     OK? : " << flagJJ);
  ATH_MSG_INFO("Weight OK? : " << flagW);

  if (flagNJets != 0 && flag1stJet != 0 && flag2ndJet != 0 && flagSign != 0 && flagJJ != 0 && flagW != 0) {
    ATH_MSG_DEBUG("Pass filter");
    // If passed then calculate (and add) a weight!
    if (m_weightingEvents) {
      for (HepMC::GenEvent* genEvt : *events()) {
        // Simple test to see if it is possible to put extra weights into Monte Carlo!
        // Store the inverse of the weighting
        genEvt->weights().push_back(1.0/weighting);
      }
    }
    return StatusCode::SUCCESS;
  }

  setFilterPassed(false);
  ATH_MSG_DEBUG("Drop event");
  return StatusCode::SUCCESS;
}


double JetIntervalFilter::calculateProbToKeep(double absoluteDeltaEta) {
  // If above cut off return 1.0
  if (absoluteDeltaEta >= m_absDEtaCutOff) return 1.0;

  // If below the mean, then treat as equal to the mean
  if (absoluteDeltaEta < m_gausMean && m_flatWeightBelowMean) {
    absoluteDeltaEta = m_gausMean;
  }

  // Calculate the constant C and probability
  const double myConstantC = std::exp(-0.5*std::pow( (m_absDEtaCutOff-m_gausMean)/m_gausSigma, 2.0) );
  const double keepProbabilityDenominator = std::exp(-0.5*std::pow( (absoluteDeltaEta-m_gausMean)/m_gausSigma, 2.0) );
  return myConstantC/keepProbabilityDenominator;
}
