// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>

// Local include(s):
#include "../TrigCostRootAnalysis/EnergyExtrapolation.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"

// ROOT include(s):
#include <TMath.h>

namespace TrigCostRootAnalysis {
  /**
   * Construct singleton,
   */
  EnergyExtrapolation::EnergyExtrapolation()
    : m_13To5(kFALSE) {
    // We currently only have one possible mode so...

    if (Config::config().getInt(kExtrapolate8To13) == kTRUE || Config::config().getInt(kExtrapolate13To5) == kTRUE) {
      load8To13();
      loadMenuV5();
      m_enabled = kTRUE;
      if (Config::config().getInt(kExtrapolate13To5)) m_13To5 = kTRUE;
    } else {
      m_enabled = kFALSE;
      m_13To5 = kFALSE;
    }
  }

  /**
   * Singleton class retrieval call.
   * @return Reference to the singleton class object.
   */
  EnergyExtrapolation& EnergyExtrapolation::energyExtrapolation() { // Instance getter
    static EnergyExtrapolation instance; // Guaranteed to be destroyed.

    return instance;        // Instantiated on first use.
  }

  void EnergyExtrapolation::load8To13() {
    // High order polynominal fit to J. Sterling
    // gluonic parton luminosity ratio 13/8 TeV
    // in GeV. Neglects gq and qq, however this should
    // be less important for rates at moderate impact paramters
    m_param[0] = 1.2; // was 1.55381, reduced to make less impact for minbias
    m_param[1] = 0.00808749;
    m_param[2] = -2.06448e-5;
    m_param[3] = 4.28319e-8;
    m_param[4] = -4.64101e-11;
    m_param[5] = 2.86482e-14;
    m_param[6] = -9.87409e-18;
    m_param[7] = 1.77766e-21;
    m_param[8] = -1.28233e-25;
    Info("EnergyExtrapolation::load8To13",
         "Energy extrapolation set to go from sqrt(s) 8 to 13 TeV (gluonic parton evolution).");
    Info("EnergyExtrapolation::load8To13",
         "Scaling M_X(GeV) 0->%.2f, 100->%.2f, 200->%.2f, 300->%.2f, 400->%.2f, 500->%.2f",
         eval(0), eval(100), eval(200), eval(300), eval(400), eval(500));
  }

  void EnergyExtrapolation::loadMenuV5() {
    m_jettyItems["HLT_ht1000_L1J100"] = 1000.;
    m_jettyItems["HLT_ht850_L1J100"] = 850.;
    m_jettyItems["HLT_ht700_L1J100"] = 700.;
    m_jettyItems["HLT_3j175"] = 525.;
    m_jettyItems["HLT_j460"] = 460.;
    m_jettyItems["HLT_j440"] = 440.;
    m_jettyItems["HLT_j420"] = 420.;
    m_jettyItems["HLT_5j85"] = 425.;
    m_jettyItems["HLT_4j100"] = 400.;
    m_jettyItems["HLT_j400"] = 400.;
    m_jettyItems["HLT_j380"] = 380.;
    m_jettyItems["HLT_j360"] = 360.;
    m_jettyItems["HLT_5j70"] = 350.;
    m_jettyItems["HLT_4j85"] = 340;
    m_jettyItems["HLT_j320"] = 320.;
    m_jettyItems["HLT_7j45"] = 315.;
    m_jettyItems["HLT_5j60"] = 300.;
    m_jettyItems["HLT_j300"] = 300.;
    m_jettyItems["HLT_5j55"] = 275.;
    m_jettyItems["HLT_j260"] = 260.;
    m_jettyItems["HLT_5j45"] = 225.;
    m_jettyItems["HLT_j200"] = 200.;
    m_jettyItems["HLT_4j45"] = 180.;
    m_jettyItems["HLT_j175"] = 175.;
    m_jettyItems["HLT_j150"] = 150.;
    m_jettyItems["HLT_j110"] = 110.;
    m_jettyItems["HLT_j100"] = 100.;
    m_jettyItems["HLT_j55"] = 55.;
    m_jettyItems["HLT_j60"] = 60.;
    m_jettyItems["HLT_j85"] = 85.;

    m_missingItems["HLT_xe100"] = 100.;
    m_missingItems["HLT_xe80"] = 80.;
    m_missingItems["HLT_xe70"] = 70.;
    m_missingItems["HLT_xe60"] = 60.;
    m_missingItems["HLT_xe35"] = 35.;

    m_muonItems["HLT_mu50"] = 50;
    m_muonItems["HLT_2mu14"] = 28;
    m_muonItems["HLT_mu26"] = 26;
    m_muonItems["HLT_mu24"] = 24;
    m_muonItems["HLT_mu22"] = 22;
    m_muonItems["HLT_2mu10"] = 20;
    m_muonItems["HLT_mu20"] = 20;
    m_muonItems["HLT_3mu6"] = 18;
    m_muonItems["HLT_mu18"] = 18;
    m_muonItems["HLT_mu14"] = 14;
    m_muonItems["HLT_2mu6"] = 12;
    m_muonItems["HLT_mu10"] = 10;
    m_muonItems["HLT_2mu4"] = 8;
    m_muonItems["HLT_mu6"] = 6;
    m_muonItems["HLT_mu4"] = 4;
    Info("EnergyExtrapolation::loadMenuV5", "Energy extrapolation factor based on result of HLT v5 menu.");
  }

  Float_t EnergyExtrapolation::eval(Float_t x) {
    return(m_param[0] +
           m_param[1] * TMath::Power(x, 1.) +
           m_param[2] * TMath::Power(x, 2.) +
           m_param[3] * TMath::Power(x, 3.) +
           m_param[4] * TMath::Power(x, 4.) +
           m_param[5] * TMath::Power(x, 5.) +
           m_param[6] * TMath::Power(x, 6.) +
           m_param[7] * TMath::Power(x, 7.) +
           m_param[8] * TMath::Power(x, 8.));
  }

  Float_t EnergyExtrapolation::getEventWeight(const TrigCostData* costData) {
    if (m_enabled == kFALSE) return 1.;

    Float_t jettyE = 0., missingE = 0., muonE = 0.;
    // Loop over all chains
    for (UInt_t c = 0; c < costData->getNChains(); ++c) {
      // Get the name of the chain (Supplying L2 or EF helps, but is not needed)
      Int_t chainID = costData->getChainID(c);
      const std::string chainName = TrigConfInterface::getHLTNameFromChainID(chainID, costData->getChainLevel(c));

      if (m_jettyItems.count(chainName) && m_jettyItems[chainName] > jettyE) {
        jettyE = m_jettyItems[chainName];
      } else if (m_muonItems.count(chainName) && m_muonItems[chainName] > muonE) {
        muonE = m_muonItems[chainName];
      } else if (m_missingItems.count(chainName) && m_missingItems[chainName] > missingE) {
        missingE = m_missingItems[chainName];
      }
    }

    Float_t eventMass = jettyE + missingE + muonE;
    Float_t extrapolationWeight = eval(eventMass);
    if (m_13To5) {
      // This is simply wrong, but no time to approx anything better
      extrapolationWeight *= 1.07; // 5->8 (minbias)
      extrapolationWeight = 1. / extrapolationWeight; // Invert
    }
    if (Config::config().debug()) {
      Info("EnergyExtrapolation::getEventWeight",
           "Event has J:%.0f, M:%.0f, X:%.0f.\tTotal:%.0f -> Extrap. Weight:%.4f",
           jettyE, muonE, missingE, eventMass, extrapolationWeight);
    }
    return extrapolationWeight;
  }
} // namespace TrigCostRootAnalysis
