// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <utility> //std::pair
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <sstream>
#include <cmath>
#include <stdlib.h>
#include <algorithm> //find

// Local include(s):
#include "../TrigCostRootAnalysis/TrigXMLService.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/CounterBaseRates.h"

// ROOT includes
#include <TSystem.h>
#include <TXMLEngine.h>
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>
#include <TError.h>

// ATHENA Includes
#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif // not ROOTCORE

namespace TrigCostRootAnalysis {
  /**
   * Construct singleton,
   * This class handles all loading of custom prescale sets to be applied at rate/cost post-processing time
   * It also handles the distribution of these data around the application
   */
  TrigXMLService::TrigXMLService() :
    m_menuName(),
    m_prescalSetName(),
    m_serviceEnabled(kFALSE),
    m_hasExpressPrescaleInfo(kFALSE),
    m_hasComments(kFALSE),
    m_eventsPerBGCounter(),
    m_unbiasedPerBGCounter(),
    m_weightsServiceEnabled(kFALSE),
    m_parsedRunXML(kFALSE),
    m_minLB(INT_MAX),
    m_maxLB(INT_MIN),
    m_computerUnknownID(0),
    m_loadedDeadtime(0),
    m_loadedPairedBunches(0) {
  }

  /**
   * Singleton class retrieval call.
   * @return Reference to the singleton class object.
   */
  TrigXMLService& TrigXMLService::trigXMLService() { // Instance getter
    static TrigXMLService instance; // Guaranteed to be destroyed.

    return instance;        // Instantiated on first use.
  }

  void TrigXMLService::init() {
    m_ignoreGRL = Config::config().getInt(kIgnoreGRL);
    parseRunXML(Config::config().getInt(kRunNumber), kTRUE);
    // Is this a "multi run"? If so - load details from the LB of the other runs
    std::vector<Int_t> multiRun = Config::config().getIntVec(kMultiRun);
    for (Int_t mr : multiRun) {
      if (mr == Config::config().getInt(kRunNumber)) {
        Fatal("TrigXMLService::TrigXMLService",
              "If doing MultiRun, provide the 'primary' run's inputs first, followed by the additional runs whose run numbers were speificed to --multiRun");
        //Bool_t primaryRunOpenedFirstBeforeMultiRunInputFiles = kFALSE;
        //assert(primaryRunOpenedFirstBeforeMultiRunInputFiles);
        std::abort();
      }
      parseRunXML(mr, kFALSE);
    }

    if (Config::config().getIsSet(kPrescaleXMLPath1) == kTRUE) parseXML(1);
    if (Config::config().getIsSet(kPrescaleXMLPath2) == kTRUE) parseXML(2);
    if (m_serviceEnabled == kFALSE) {
      Warning("TrigXMLService::TrigXMLService",
              "User did not supply any prescale XML files. L1 PS=%f and HLT PS=%f will be used.",
              Config::config().getFloat(kRateFallbackPrescaleL1), Config::config().getFloat(kRateFallbackPrescaleHLT));
    }
  }

  Double_t TrigXMLService::getPrescale(const std::string& name) {
    if (m_serviceEnabled == kFALSE) {
      if (name.substr(0, 3) == "L1_") {
        return (Double_t) Config::config().getFloat(kRateFallbackPrescaleL1);
      } else {
        return (Double_t) Config::config().getFloat(kRateFallbackPrescaleHLT);
      }
    }

    StringDoubleMapIt_t it = m_chainPS.find(name);
    if (it == m_chainPS.end()) {
      if (name.substr(0, 3) == "L1_") {
        if (Config::config().getDisplayMsg(kMsgXMLPrescale) == kTRUE) {
          Warning("TrigXMLService::getPrescale",
                  "Cannot find rates prescale for chain %s in the supplied XML. Will return fallback L1 PS:%f.",
                  name.c_str(),
                  Config::config().getFloat(kRateFallbackPrescaleL1));
        }
        return (Double_t) Config::config().getFloat(kRateFallbackPrescaleL1);
      } else {
        if (Config::config().getDisplayMsg(kMsgXMLPrescale) == kTRUE) {
          Warning("TrigXMLService::getPrescale",
                  "Cannot find rates prescale for chain %s in the supplied XML. Will return fallback HLT PS:%f.",
                  name.c_str(),
                  Config::config().getFloat(kRateFallbackPrescaleHLT));
        }
        return (Double_t) Config::config().getFloat(kRateFallbackPrescaleHLT);
      }
    }

    return it->second;
  }

  /**
   * When running cost analysis in the HLT and looking at the effect of prescales the following should be taken into
   *account:
   * - All data should be scaled down by 1/PSL1 * 1/PSHLT
   * - If the HLT trigger or L1 trigger is disabled, the weight should be 0.
   * @param chainName Name of the chain to get the weighting factor for. Only makes sense to this fn for HLT chains
   * @return the calculated multiplicitive weighting factor
   */
  Double_t TrigXMLService::getHLTCostWeightingFactor(const std::string& chainName) {
    Double_t prescaleHLT = getPrescale(chainName);

    if (prescaleHLT < 0.) return 0.;

    const std::string myLowerChain = TrigConfInterface::getLowerChainName(chainName);
    // Check we're not multi-seeded
    if (myLowerChain.find(",") != std::string::npos) return 1. / prescaleHLT; //What else could we do here?

    Double_t lowerPS = getPrescale(myLowerChain);
    if (lowerPS < 0.) return 0.;

    return (1. / prescaleHLT) * (1. / lowerPS);
  }

  /**
   * Get the per-event weight needed to extrapolate to the target luminosity. This is based on the kRunLumi value,
   * the effective (i.e. time-averaged) instantaneous luminosity of the Enhanced Bias run being used for the prediction
   * and the targer kPredictionLumi. The former is supplied (for now) by the user, the latter can be supplied by the
   *user
   * or fetched from the prescale XML (hence this function being in the XML services, it's loaded in the constructor).
   * @return Ratio of (predictionLumi / runLumi), or 1 if insufficient/obviously wrong infomration present.
   */
  Float_t TrigXMLService::getLumiExtrapWeight() {
    // Do we know the effective L of the run? If not return 1.
    if (!Config::config().getIsSet(kRunLumi) && !Config::config().getIsSet(kRunLumiXML)) {
      Warning("TrigXMLService::getLumiExtrapWeight", "No run lumi set. No extrapolation.");
      return 1.;
    }
    Float_t runLumi;
    if (Config::config().getIsSet(kRunLumi)) runLumi = Config::config().getFloat(kRunLumi); // Prefer user supplied
    else runLumi = Config::config().getFloat(kRunLumiXML); // Fallbac on XML

    // If prediction lumi was not set by user or XML, then return 1.
    if (!Config::config().getIsSet(kPredictionLumi)
        && !Config::config().getIsSet(kPredictionLumiMenuXML)
        && !Config::config().getIsSet(kPredictionLumiRunXML)) {
      Warning("TrigXMLService::getLumiExtrapWeight", "No prediction lumi set. No extrapolation.");
      return 1.;
    }

    // If prediction lumi was not set by the user, and we are not in rates mode (i.e. we just happen to be processing a
    // run
    // which was an EB run) then return 1
    if (!Config::config().getIsSet(kPredictionLumi)
        && !Config::config().getIsSet(kPredictionLumiMenuXML)
        && Config::config().getInt(kDoRatesMonitor) == 0) {
      Warning("TrigXMLService::getLumiExtrapWeight",
              "We have a prediction lumi for this run from the EB XML (%e), but are not doing standard rates. To use this explicitly specify a --predictionLumi",
              Config::config().getFloat(kPredictionLumiRunXML));
      return 1.;
    }

    Float_t predictionLumi, onlineDeadtime = 0.;
    std::string predFrom;
    // Prefer the user CLI value, then the value from the prescale XML, then the default lumi point from the run XML
    if (Config::config().getIsSet(kPredictionLumi)) {
      predFrom = "command line";
      predictionLumi = Config::config().getFloat(kPredictionLumi);
    } else if (Config::config().getIsSet(kPredictionLumiMenuXML)) {
      predFrom = "prescale XML";
      predictionLumi = Config::config().getFloat(kPredictionLumiMenuXML);
    } else {
      predFrom = "run XML (the default target L point)";
      predictionLumi = Config::config().getFloat(kPredictionLumiRunXML);
    }

    if (Config::config().getIsSet(kOnlineDeadtime) && Config::config().getInt(kNoOnlineDeadtimeCorrection) == kFALSE) {
      onlineDeadtime = Config::config().getFloat(kOnlineDeadtime);
    }

    // This should not be zero, and if it is set then it's set explicitly. Still, check.
    if (isZero(predictionLumi) || isZero(runLumi)) {
      Warning("TrigXMLService::getLumiExtrapWeight", "The prediction/run lumi of this run has been set to zero. "
                                                     "This is obviously wrong. No scaling will be perfomed.");
      return 1.;
    }

    Float_t scalingFactor = predictionLumi / runLumi;
    scalingFactor *= 1 + onlineDeadtime;

    Double_t averageL = 0.;
    Int_t averageLN = 0;

    // Do we have a target mu?
    if (!isZero(Config::config().getFloat(kTargetPeakMuAverage)) && Config::config().getIsSet(kOnlinePeakMuAverage)) {
      Bool_t doExponentialMu = Config::config().getInt(kDoExponentialMu);

      Float_t lumiScaling = predictionLumi / runLumi;

      Float_t targetMu = Config::config().getFloat(kTargetPeakMuAverage);
      Float_t onlineMu = Config::config().getFloat(kOnlinePeakMuAverage);

      Float_t expoRateScaleModifierL1 = Config::config().getFloat(kExpoRateScaleModifierL1);
      Float_t expoRateScaleModifierHLT = Config::config().getFloat(kExpoRateScaleModifierHLT);

      Float_t lumiMuScaling = targetMu / onlineMu;
      Float_t lumiBunchScaling = lumiScaling / lumiMuScaling;

      Float_t lumiMuScalingExpoL1 = TMath::Exp((targetMu - onlineMu) * expoRateScaleModifierL1);
      Float_t lumiMuScalingExpoHLT = TMath::Exp((targetMu - onlineMu) * expoRateScaleModifierHLT);

      if (doExponentialMu == kFALSE) {
        lumiMuScalingExpoL1 = lumiMuScaling;
        lumiMuScalingExpoHLT = lumiMuScaling;
      }

      Float_t lumiScalingExpoL1 = lumiBunchScaling * lumiMuScalingExpoL1 * (1 + onlineDeadtime);
      Float_t lumiScalingExpoHLT = lumiBunchScaling * lumiMuScalingExpoHLT * (1 + onlineDeadtime);
      lumiBunchScaling *= (1 + onlineDeadtime);
      lumiMuScaling *= (1 + onlineDeadtime);

      Int_t maxBunches = Config::config().getInt(kMaxBunches);
      Int_t maxBCIDs = Config::config().getInt(kMaxBCIDs);
      Int_t targetBunches = (Int_t) std::round(m_bunchGroupXML[1].second * lumiBunchScaling);
      Info("TrigXMLService::getLumiExtrapWeight", "PredictionLumi taken from %s.", predFrom.c_str());
      Info("TrigXMLService::getLumiExtrapWeight",
           "Using targetMu setting %.2f. <mu> scaling factor: %.2f->%.2f = %.2f. Expo. L1 = %.2f. Expo. HLT = %.2f",
           targetMu, onlineMu, targetMu, lumiMuScaling, lumiMuScalingExpoL1, lumiMuScalingExpoHLT);
      Info("TrigXMLService::getLumiExtrapWeight", "Bunch scaling factor: %i->%i = %.2f",
           m_bunchGroupXML[1].second, targetBunches, lumiBunchScaling);
      Info("TrigXMLService::getLumiExtrapWeight",
           "Online deadtime was %.2f%%, including deadtime - the run averaged lumi scaling factors are: "
           "linear = %.2f, expo. in <mu> L1 = %.2f, expo. in <mu> HLT = %.2f, bunch only = %.2f, mu only = %.2f",
           onlineDeadtime * 100., scalingFactor, lumiScalingExpoL1, lumiScalingExpoHLT, lumiBunchScaling,
           lumiMuScaling);
      if (targetBunches > maxBunches + 15 /*allow wiggle room*/ || targetBunches < 1) {
        Warning("TrigXMLService::getLumiExtrapWeight",
                "To get to L=%.2e with a --targetMu of %.2f requires %i bunches. A full ring is %i!",
                predictionLumi, targetMu, targetBunches, maxBunches);
      }
      // Can request exponential scaling in <mu>

      // Some extra calculations for EMPTY bunchgroup scaling
      Int_t currentEmptyBunches = std::max(0, maxBCIDs - m_bunchGroupXML[1].second); // Filled
      Int_t targetEmptyBunches = std::max(0, maxBCIDs - targetBunches);
      Float_t emptyExtrap = 0.;
      if (currentEmptyBunches > 0) emptyExtrap = targetEmptyBunches / (Float_t) currentEmptyBunches;

      // Write info
      Config::config().set(kDoAdvancedLumiScaling, 1, "DoAdvancedLumiScaling");
      Config::config().setFloat(kPredictionLumiFinalMuComponent, lumiMuScaling, "PredictionLumiFinalMuComponent");
      Config::config().setFloat(kPredictionLumiFinalMuExpoL1Component, lumiMuScalingExpoL1,
                                "PredictionLumiFinalExponentialMuL1Component");
      Config::config().setFloat(kPredictionLumiFinalMuExpoHLTComponent, lumiMuScalingExpoHLT,
                                "PredictionLumiFinalExponentialMuHLTComponent");
      Config::config().setFloat(kPredictionLumiFinalBunchComponent, lumiBunchScaling,
                                "PredictionLumiFinalBunchComponent");
      Config::config().setFloat(kPredictionLumiFinalExpoL1, lumiScalingExpoL1, "PredictionLumiFinalExponentialMuL1");
      Config::config().setFloat(kPredictionLumiFinalExpoHLT, lumiScalingExpoHLT,
                                "PredictionLumiFinalExponentialMuHLT");
      Config::config().setFloat(kEmptyBunchgroupExtrapolaion, emptyExtrap, "EmptyBunchgroupExtrapolation");
      Config::config().set(kTargetPairedBunches, targetBunches, "TargetPairedBunches");

      // Even more fancy is to split these details per LB
      Float_t thisLBLumi = runLumi;
      Float_t thisLBMu = onlineMu;
      Float_t thisLBDeadtime = onlineDeadtime;
      Int_t thisLBEmptyBnch = currentEmptyBunches;
      for (Int_t lb = m_minLB; lb <= m_maxLB; ++lb) {
        // If we miss data - then we take the previous LB's data or the run default
        if (m_lumiPerLB.count(lb) == 1) thisLBLumi = m_lumiPerLB[lb];
        if (m_muPerLB.count(lb) == 1) thisLBMu = m_muPerLB[lb];
        if (m_deadtimePerLB.count(lb) == 1) thisLBDeadtime = m_deadtimePerLB[lb];
        if (m_pairedBunchesPerLB.count(lb) == 1) thisLBEmptyBnch = std::max(0, maxBCIDs - m_pairedBunchesPerLB[lb]);


        Float_t emptyExtrapPerLB = 0.;
        if (thisLBEmptyBnch > 0) emptyExtrapPerLB = targetEmptyBunches / (Float_t) thisLBEmptyBnch;

        Float_t lumiScalingPerLB = predictionLumi / thisLBLumi;

        Float_t lumiMuScalingPerLB = targetMu / thisLBMu;
        Float_t lumiBunchScalingPerLB = lumiScalingPerLB / lumiMuScalingPerLB;

        Float_t lumiMuScalingExpoL1PerLB = TMath::Exp((targetMu - thisLBMu) * expoRateScaleModifierL1);
        Float_t lumiMuScalingExpoHLTPerLB = TMath::Exp((targetMu - thisLBMu) * expoRateScaleModifierHLT);

        if (doExponentialMu == kFALSE) {
          lumiMuScalingExpoL1PerLB = lumiMuScalingPerLB;
          lumiMuScalingExpoHLTPerLB = lumiMuScalingPerLB;
        }

        Float_t lumiScalingExpoL1PerLB = lumiBunchScalingPerLB * lumiMuScalingExpoL1PerLB;
        Float_t lumiScalingExpoHLTPerLB = lumiBunchScalingPerLB * lumiMuScalingExpoHLTPerLB;

        // Protect inf
        if (isZero(thisLBLumi)) {
          lumiScalingPerLB = 0;
          lumiScalingExpoL1PerLB = 0;
          lumiScalingExpoHLTPerLB = 0;
          lumiBunchScalingPerLB = 0;
          lumiMuScalingPerLB = 0;
        } else {
          averageL += thisLBLumi;
          ++averageLN;
        }

        m_lumiScalingFactorExpoL1[lb] = lumiScalingExpoL1PerLB * (1 + thisLBDeadtime);
        m_lumiScalingFactorExpoHLT[lb] = lumiScalingExpoHLTPerLB * (1 + thisLBDeadtime);
        m_lumiScalingFactorLinear[lb] = lumiScalingPerLB * (1 + thisLBDeadtime);
        m_lumiScalingFactorBunchOnly[lb] = lumiBunchScalingPerLB * (1 + thisLBDeadtime);
        m_lumiScalingFactorMuOnly[lb] = lumiMuScalingPerLB * (1 + thisLBDeadtime);
        m_lumiScalingFactorDeadtimeOnly[lb] = 1. * (1 + thisLBDeadtime);
        m_lumiScalingFactorEmpty[lb] = emptyExtrapPerLB;
        m_lumiScalingFactorUnity[lb] = 1.;

        Info("TrigXMLService::getLumiExtrapWeight",
             "  Per-LB scaling factors LB %i: Linear = %.2f | ExpInMuL1 = %.2f | ExpInMuHLT = %.2f | "
             "BunchOnly = %.2f | MuOnly = %.2f | DeadtimeOnly = %.2f | Empty = %.2f |",
             lb, m_lumiScalingFactorLinear[lb], m_lumiScalingFactorExpoL1[lb], m_lumiScalingFactorExpoHLT[lb],
             m_lumiScalingFactorBunchOnly[lb], m_lumiScalingFactorMuOnly[lb], m_lumiScalingFactorDeadtimeOnly[lb],
             m_lumiScalingFactorEmpty[lb]);
      }
      if (averageLN) averageL /= averageLN;
    } else {
      // Build map per-LB
      Float_t thisLBLumi = runLumi;
      std::stringstream perLBstr;
      perLBstr.precision(5);

      for (Int_t lb = m_minLB; lb <= m_maxLB; ++lb) {
        // If we miss data - then we take the previous LB's data or the run default
        if (m_lumiPerLB.count(lb) == 1) thisLBLumi = m_lumiPerLB[lb];
        Float_t lumiScalingPerLB = predictionLumi / thisLBLumi;
        if (isZero(thisLBLumi)) {
          lumiScalingPerLB = 0;
        } else {
          averageL += thisLBLumi;
          ++averageLN;
        }
        m_lumiScalingFactorLinear[lb] = lumiScalingPerLB * (1 + onlineDeadtime);
        averageL += thisLBLumi;
        ++averageLN;
        perLBstr << "LB " << lb << "=" << m_lumiScalingFactorLinear[lb] << ", ";
      }
      if (averageLN) averageL /= averageLN;

      // This is the old-style message, only show it now if doing basic extrapolation mode
      Info("TrigXMLService::getLumiExtrapWeight", "Predictions will be scaled on average by %.4f from EB RunLumi %.2e to "
                                                  "PredictionLumi %.2e. Including a %.2f%% correction for online deadtime. PredictionLumi taken from %s.", 
                                                  scalingFactor, runLumi, predictionLumi, onlineDeadtime * 100., predFrom.c_str());
      Info("TrigXMLService::getLumiExtrapWeight", "Per-LB, the weights are: %s", perLBstr.str().c_str());
    }

    // Do we need to update the average lumi of this processing?
    if (!isZero(averageL)) {
      if (fabs(1. - (runLumi / averageL)) > 0.05) { // If these numbers are off by more than 5% (arbitrary threshold)
        Warning("TrigXMLService::getLumiExtrapWeight",
                "Discrepancy in XML EB average run Lumi (%.4e) and calculated (%.4e). Probably due to restricted LB range.",
                runLumi,
                averageL);
      }
      Config::config().setFloat(kRunLumiXML, averageL, "EnhancedBiasOnlineLumi", kLocked);
    }

    Config::config().setFloat(kLumiExtrapWeight, scalingFactor, "FinalLumiExtrapWeight", kLocked); // Keep a note of
                                                                                                    // this factor
    Config::config().setFloat(kPredictionLumiFinal, predictionLumi, "PredictionLumiFinal", kLocked);
    Config::config().setFloat(kDeadtimeScalingFinal, (1. + onlineDeadtime), "DeadtimeScalingFinal", kLocked);
    return scalingFactor;
  }

  /**
   * Read XML structure, decide on format using (usually) the head node and pass to parser function
   * @param xmlID This program can take in two XMLs, if the L1 and HLT PS are split over them. This int says which we
   *want to process.
   */
  void TrigXMLService::parseXML(UInt_t xmlID) {
    assert(xmlID == 1 || xmlID == 2);
    std::string psFilePath;
    std::string psFileName;
    if (xmlID == 1) {
      psFilePath = Config::config().getStr(kPrescaleXMLPath1);
      psFileName = Config::config().getStr(kPrescaleXMLName1);
    } else if (xmlID == 2) {
      psFilePath = Config::config().getStr(kPrescaleXMLPath2);
      psFileName = Config::config().getStr(kPrescaleXMLName2);
    }

    // Now try to parse xml file
    // Only file with restricted xml syntax are supported
    TXMLEngine* xml = new TXMLEngine();
    XMLDocPointer_t xmlDoc = xml->ParseFile(psFilePath.c_str());
    if (xmlDoc == 0) {
      Error("TrigXMLService::parseXML",
            "Unable to parse prescale XML %s!!! Fallback PS=%f L1, PS=%fi HLT will be used for rate calculations.",
            psFilePath.c_str(),
            Config::config().getFloat(kRateFallbackPrescaleL1),
            Config::config().getFloat(kRateFallbackPrescaleHLT));
      delete xml;
      return;
    }

    // Get access to main node
    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
    if (xml->GetNodeName(mainNode) == std::string("trigger")) {
      parsePrescaleXML(xml, xmlDoc);
    } else if (xml->GetNodeName(mainNode) == std::string("HLT_MENU")) {
      parseMenuXML(xml, xmlDoc);
    } else if (xml->GetNodeName(mainNode) == std::string("LVL1Config")) {
      parseL1MenuXML(xml, xmlDoc);
    } else {
      Error("TrigXMLService::parseXML",
            "Supplied a prescale XML with root node '%s'. I do not know how to decode this!!! Fallback PS=%f L1, PS=%fi HLT will be used for rate calculations.",
            xml->GetNodeName(mainNode),
            Config::config().getFloat(kRateFallbackPrescaleL1),
            Config::config().getFloat(kRateFallbackPrescaleHLT));
    }

    delete xml;
  }

  /**
   * Read details from a menu XML file. This allows the user to supply their own prescale set.
   * However this method does not supply L1 prescales.
   */
  void TrigXMLService::parseMenuXML(TXMLEngine* xml, XMLDocPointer_t xmlDoc) {
    // Get access to main node
    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);

    const Bool_t ignorePSGreaterThanOne = (Bool_t) Config::config().getInt(kIgnorePSGreaterThanOne);

    assert(xml->GetNodeName(mainNode) == std::string("HLT_MENU"));
    XMLNodePointer_t listNode = xml->GetChild(mainNode);
    m_menuName = xml->GetAttr(mainNode, "menu_name");
    m_prescalSetName = xml->GetAttr(mainNode, "prescale_set_name");
    Int_t chainsRead = 0;

    while (listNode != 0) { // Loop over all menu elements
      const std::string listName = xml->GetNodeName(listNode);

      if (listName != "CHAIN_LIST") {
        listNode = xml->GetNext(listNode);
        continue;
      }

      XMLNodePointer_t chainNode = xml->GetChild(listNode);
      while (chainNode != 0) {
        assert(xml->GetNodeName(chainNode) == std::string("CHAIN"));
        const std::string chainName = xml->GetAttr(chainNode, "chain_name");
        if (xml->GetAttr(chainNode,
                          "chain_counter")) m_chainCounter[chainName] =
          stringToInt(xml->GetAttr(chainNode, "chain_counter"));
        if (xml->GetAttr(chainNode, "lower_chain_name")) m_chainLowerLvl[chainName] = xml->GetAttr(chainNode,
                                                                                                       "lower_chain_name");

        if (xml->GetAttr(chainNode, "prescale")) m_chainPS[chainName] = stringToDouble(xml->GetAttr(chainNode, "prescale"));
        if (xml->GetAttr(chainNode, "pass_through")) m_chainPT[chainName] = stringToDouble(xml->GetAttr(chainNode, "pass_through"));
        if (xml->GetAttr(chainNode, "rerun_prescale")) m_chainRerunPS[chainName] = stringToDouble(xml->GetAttr(chainNode, "rerun_prescale"));
        if (xml->GetAttr(chainNode, "express_prescale")) {
          m_chainExpressPS[chainName] = stringToDouble(xml->GetAttr(chainNode, "express_prescale"));
          m_hasExpressPrescaleInfo = kTRUE;
        }
        ++chainsRead;

        if (ignorePSGreaterThanOne && m_chainPS[chainName] > 1.) m_chainPS[chainName] = -1;

        if (Config::config().debug()) {
          Info("TrigXMLService::parseMenuXML", "Parsed Chain:%s, Counter:%i, LowerChain:%s, PS:%f, PT:%f, RerunPS:%f, Express:%f",
               chainName.c_str(),
               m_chainCounter[chainName],
               m_chainLowerLvl[chainName].c_str(),
               m_chainPS[chainName],
               m_chainPT[chainName],
               m_chainRerunPS[chainName],
               m_chainExpressPS[chainName]);
        }
        chainNode = xml->GetNext(chainNode);
      }

      listNode = xml->GetNext(listNode);
    }

    Info("TrigXMLService::parseMenuXML", "Parsed prescale set %s of menu %s. Read %i chains.",
         m_prescalSetName.c_str(),
         m_menuName.c_str(),
         chainsRead);

    Config::config().set(kPrescaleSetName, m_prescalSetName, "PrescaleSetName", kUnlocked);
    Config::config().set(kMenuName, m_menuName, "MenuName", kUnlocked);

    m_serviceEnabled = kTRUE;
  }

  /**
   * Read details from a L1 menu XML file. This allows the user to supply their own prescale set.
   * We first need to read the name-CTPID mappings, then can read in the PS
   */
  void TrigXMLService::parseL1MenuXML(TXMLEngine* xml, XMLDocPointer_t xmlDoc) {
    // Get access to main node
    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);

    const Bool_t ignorePSGreaterThanOne = (Bool_t) Config::config().getInt(kIgnorePSGreaterThanOne);

    assert(xml->GetNodeName(mainNode) == std::string("LVL1Config"));
    XMLNodePointer_t listNode = xml->GetChild(mainNode);
    Int_t chainsRead = 0;

    while (listNode != 0) { // Loop over all menu elements
      const std::string listName = xml->GetNodeName(listNode);

      // TriggerMenu contains TriggerItems which hold the CTPID and names
      if (listName == "TriggerMenu") {
        XMLNodePointer_t chainNode = xml->GetChild(listNode);
        while (chainNode != 0) {
          assert(xml->GetNodeName(chainNode) == std::string("TriggerItem"));
          Int_t ctpid = stringToInt(xml->GetAttr(chainNode, "ctpid"));
          m_CTPIDToL1Name[ctpid] = xml->GetAttr(chainNode, "name");
          if (Config::config().debug()) {
            Info("TrigXMLService::parseL1MenuXML", "Step 1: CTPID %i = %s", ctpid, m_CTPIDToL1Name[ctpid].c_str());
          }
          chainNode = xml->GetNext(chainNode);
        }

        // PrescaleSet contains Prescale items which map these CTPIDs to their PS
        // The CTPID, and "n", "m" and "d" parameters are attributes, the prescale is content
      } else if (listName == "PrescaleSet") {
        XMLNodePointer_t chainNode = xml->GetChild(listNode);
        while (chainNode != 0) {
          assert(xml->GetNodeName(chainNode) == std::string("Prescale"));
          Int_t ctpid = stringToInt(xml->GetAttr(chainNode, "ctpid"));
          const std::string L1Name = m_CTPIDToL1Name[ ctpid ];
          Double_t prescale = stringToDouble(xml->GetAttr(chainNode, "value"));
          if (L1Name == Config::config().getStr(kBlankString) && prescale >= 0) {
            Error("TrigXMLService::parseL1MenuXML", "XML trying to set PS for CTPID %i, but no chain name was supplied by the file", ctpid);
            chainNode = xml->GetNext(chainNode);
            continue;
          } else if (L1Name == Config::config().getStr(kBlankString)) { // Silent continue
            chainNode = xml->GetNext(chainNode);
            continue;
          }
          m_chainCounter[L1Name] = ctpid;
          m_chainPS[L1Name] = prescale;

          if (ignorePSGreaterThanOne && m_chainPS[L1Name] > 1.) m_chainPS[L1Name] = -1;

          ++chainsRead;
          if (Config::config().debug()) {
            Info("TrigXMLService::parseL1MenuXML", "Step 2: L1 %s = PS %f", L1Name.c_str(), prescale);
          }
          chainNode = xml->GetNext(chainNode);
        }
      } else {
        Info("TrigXMLService::parseL1MenuXML", "Ignoring an unknown L1 menu XML element %s", listName.c_str());
      }

      listNode = xml->GetNext(listNode);
    }

    Info("TrigXMLService::parseL1MenuXML", "Parsed L1 prescale set. Read %i chains.", chainsRead);
    m_serviceEnabled = kTRUE;
  }

  /**
   * Alternate parser for prescale XML files, including the output of TrigCostPython
   * @param xml Pointer to XML document
   * @see TrigXMLService::parseMenuXML
   */
  void TrigXMLService::parsePrescaleXML(TXMLEngine* xml, XMLDocPointer_t xmlDoc) {
    // Get access to main node
    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);

    const Bool_t ignorePSGreaterThanOne = (Bool_t) Config::config().getInt(kIgnorePSGreaterThanOne);

    assert(xml->GetNodeName(mainNode) == std::string("trigger"));
    XMLNodePointer_t listNode = xml->GetChild(mainNode);


    while (listNode != 0) { // Loop over all menu elements
      const std::string listName = xml->GetNodeName(listNode);

      if (listName == "PredictionLumi") {
        Float_t predictionLumi = stringToFloat(xml->GetNodeContent(listNode));
        Config::config().setFloat(kPredictionLumiMenuXML, predictionLumi, "PredictionLumiMenuXML");
      }

      if (listName != "level") { // Find the "level" item
        listNode = xml->GetNext(listNode);
        continue;
      }

      XMLNodePointer_t sigNode = xml->GetChild(listNode);
      while (sigNode != 0) {
        if (xml->GetNodeName(sigNode) != std::string("signature")) { // Find the "signature" items
          sigNode = xml->GetNext(sigNode);
          continue;
        }

        XMLNodePointer_t sigDetailsNode = xml->GetChild(sigNode);

        std::string chainName;
        while (sigDetailsNode != 0) {
          if (xml->GetNodeContent(sigDetailsNode) == NULL) {
            sigDetailsNode = xml->GetNext(sigDetailsNode);
            continue;
          }

          const std::string detail = xml->GetNodeName(sigDetailsNode);
          if (detail == "sig_name") {
            chainName = xml->GetNodeContent(sigDetailsNode);
          } else if (detail == "sig_counter") {
            m_chainCounter[chainName] = stringToInt(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "prescale" || detail == "chain_prescale") { // This is an alternate name
            m_chainPS[chainName] = stringToDouble(xml->GetNodeContent(sigDetailsNode));
            //if ( isZero(m_chainPS[chainName]) ) m_chainPS[chainName] = -1.;
          } else if (detail == "lower_chain_name") {
            // Later processing here does not expect any spaces, so remove them now. Pure comma separated list
            std::string lower = xml->GetNodeContent(sigDetailsNode);
            while (lower.find(" ") != std::string::npos) {
              lower.replace(lower.find(" "), 1, Config::config().getStr(kBlankString));
            }
            m_chainLowerLvl[chainName] = lower;
          } else if (detail == "evts_passed") {
            m_chainEvtPassed[chainName] = stringToInt(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "evts_passed_weighted") {
            m_chainEvtPassedWeighted[chainName] = stringToFloat(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "rate") {
            m_chainRate[chainName] = stringToFloat(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "rate_err") {
            m_chainRateErr[chainName] = stringToFloat(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "passthrough") {
            m_chainPT[chainName] = stringToDouble(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "rerun_prescale") {
            m_chainRerunPS[chainName] = stringToDouble(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "express_prescale") {
            m_chainExpressPS[chainName] = stringToDouble(xml->GetNodeContent(sigDetailsNode));
            m_hasExpressPrescaleInfo = kTRUE;
          } else if (detail == "efficiency") {
            m_chainEff[chainName] = stringToFloat(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "efficiency_err") {
            m_chainEffErr[chainName] = stringToFloat(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "prescaled_efficiency") {
            m_chainPSEff[chainName] = stringToFloat(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "prescaled_efficiency_error") {
            m_chainPSEffErr[chainName] = stringToFloat(xml->GetNodeContent(sigDetailsNode));
          } else if (detail == "comment") {
            m_chainComment[chainName] = xml->GetNodeContent(sigDetailsNode);
            m_hasComments = kTRUE;
          }

          sigDetailsNode = xml->GetNext(sigDetailsNode);
        }

        // Sanity check here. If the user is not doing EB weighting, and is not scaling up by the L1 PS, then != 1 PS at
        // L1 make no sense.
        if (Config::config().getInt(kDoEBWeighting) == 0
            && Config::config().getInt(kRatesScaleByPS) == 0
            && chainName.substr(0, 3) == "L1_"
            && isZero(m_chainPS[chainName] - 1.) == kFALSE) {
          Error("TrigXMLService::parsePrescaleXML", "You are setting a prescale PS=%f for an L1 item %s, but you are NOT using EnhancedBias data. "
                                                    "Nor are you using the --scaleRatesByPS option to weight up chains by their L1 prescale. "
                                                    "In order to simulate changes to the L1 prescale, you should be using EB weighting. "
                                                    "Otherwise the correlations between L1 items in the CTP "
                                                    "will have been lost.", m_chainPS[chainName], chainName.c_str());
        }

        if (ignorePSGreaterThanOne && m_chainPS[chainName] > 1.) m_chainPS[chainName] = -1;

        if (Config::config().debug()) {
          Info("TrigXMLService::parsePrescaleXML", "Parsed Chain:%s, "
                                                   "Counter:%i, "
                                                   "EvtPass:%i, "
                                                   "EvtPassWeight:%.2f, "
                                                   "Rate:%.2f, "
                                                   "RateErr:%.2f, "
                                                   "PS:%f, "
                                                   "PT:%f, "
                                                   "RerunPS:%f, "
                                                   "ExpressPS:%f, "
                                                   "Lower:%s, "
                                                   "Eff:%.2f, "
                                                   "EffErr%.2f, "
                                                   "PSEff:%.2f, "
                                                   "PSEffErr:%.2f",
               chainName.c_str(),
               m_chainCounter[chainName],
               m_chainEvtPassed[chainName],
               m_chainEvtPassedWeighted[chainName],
               m_chainRate[chainName],
               m_chainRateErr[chainName],
               m_chainPS[chainName],
               m_chainPT[chainName],
               m_chainRerunPS[chainName],
               m_chainExpressPS[chainName],
               m_chainLowerLvl[chainName].c_str(),
               m_chainEff[chainName],
               m_chainEffErr[chainName],
               m_chainPSEff[chainName],
               m_chainPSEffErr[chainName]);
        }
        sigNode = xml->GetNext(sigNode);
      }

      listNode = xml->GetNext(listNode);
    }

    Info("TrigXMLService::parsePrescaleXML", "Parsed prescale set from XML. Now know PS values for %i chains.", (Int_t) m_chainPS.size());
    m_serviceEnabled = kTRUE;
  }

  /**
   * Try and parse the run XML if any - this contains EB run specifics
   * @param runNumber the run number of the XML to load
   * @param primaryRun if true, this is the primary & first input file's run. We take this run as the main set of
   *parameters
   */
  void TrigXMLService::parseRunXML(const Int_t runNumber, const Bool_t primaryRun) {
    const std::string file = std::string("enhanced_bias_run_") + intToString(runNumber) + std::string(".xml");

    std::string path;

    if (Config::config().getInt(kIsRootCore) == kTRUE) {
      path = std::string(Config::config().getStr(kDataDir) + file);
    } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
      path = PathResolverFindCalibFile("TrigCostRootAnalysis/" + file); // Get from CALIB area
      if (path == Config::config().getStr(kBlankString)) { // One more place we can look
        path = std::string(Config::config().getStr(kAFSDataDir) + "/" + file);
      }
#endif // not ROOTCORE
    }


    // 2016 high-<mu> run
    // Here we account for the three v-high-mu-bunches: ~6% of the luminosity came from the three isolated fat bunches
    Float_t mod = 1.;
    if (runNumber == 310574) {
      if (Config::config().getInt(kInvertHighMuRunVeto) == true) mod = 0.06;
      else mod = 0.94;
    }

    // If file exists
    TXMLEngine* xml = new TXMLEngine();
    XMLDocPointer_t xmlDoc = 0;

    if (path != Config::config().getStr(kBlankString) && access(path.c_str(), F_OK) != -1) {
      xmlDoc = xml->ParseFile(path.c_str());
    }

    if (xmlDoc == 0) {
      Warning("TrigXMLService::parseRunXML", "Cannot find run XML %s, hence will not do advanced partial-run rate scaling.",
              file.c_str());
      delete xml;
      return;
    }

    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
    assert(xml->GetNodeName(mainNode) == std::string("trigger"));
    XMLNodePointer_t listNode = xml->GetChild(mainNode);

    while (listNode != 0) { // Loop over all menu elements
      const std::string listName = xml->GetNodeName(listNode);

      if (listName == "lb_list") {
        XMLNodePointer_t node = xml->GetChild(listNode);
        while (node != 0) {
          assert(xml->GetNodeName(node) == std::string("lb"));
          Int_t lb = stringToInt(xml->GetAttr(node, "id"));
          UInt_t nEvents = stringToInt(xml->GetNodeContent(node));
          // Is LB to be vetoed? (like a GRL)
          std::string flag;
          if (xml->HasAttr(node, "flag")) {
            flag = xml->GetAttr(node, "flag");
          }
          if (flag == "bad") m_badLumiBlocks.insert(lb);
          // Some runs have higher granularity data
          Float_t lumi = 0.;
          if (xml->HasAttr(node, "lumi")) lumi = stringToFloat(xml->GetAttr(node, "lumi"));
          else lumi = Config::config().getFloat(kRunLumiXML);
          if (lumi < 1e20) lumi *= 1e30;
          //
          Float_t mu = 0.;
          if (xml->HasAttr(node, "mu")) mu = stringToFloat(xml->GetAttr(node, "mu"));
          else mu = Config::config().getFloat(kOnlinePeakMuAverage);
          //
          if (lb < m_minLB) m_minLB = lb;
          if (lb > m_maxLB) m_maxLB = lb;

          // We can only deal with one LB per processing, a current limitation of MultiRun
          assert(m_totalEventsPerLB.count(lb) == 0);

          m_totalEventsPerLB[lb] = nEvents;
          m_lumiPerLB[lb] = lumi;
          m_muPerLB[lb] = mu;
          m_deadtimePerLB[lb] = m_loadedDeadtime;
          m_pairedBunchesPerLB[lb] = m_loadedPairedBunches;
          node = xml->GetNext(node);
        }
      } else if (listName == "lumivalues") {
        XMLNodePointer_t node = xml->GetChild(listNode);
        while (node != 0) {
          if (xml->GetNodeName(node) == std::string("lumivalues_data")) {
            if (primaryRun) Config::config().setFloat(kRunLumiXML, stringToFloat(xml->GetNodeContent(node)) * mod, "EnhancedBiasOnlineLumi", kUnlocked);
          } else if (xml->GetNodeName(node) == std::string("lumivalues_pred")) {
            if (primaryRun) Config::config().setFloat(kPredictionLumiRunXML, stringToFloat(xml->GetNodeContent(node)), "PredictionLumiRunXML");
          } else if (xml->GetNodeName(node) == std::string("deadtime")) {
            m_loadedDeadtime = stringToFloat(xml->GetNodeContent(node));
            if (primaryRun) Config::config().setFloat(kOnlineDeadtime, m_loadedDeadtime, "OnlineDeadtime");
          } else if (xml->GetNodeName(node) == std::string("peak_mu_av")) {
            if (primaryRun) Config::config().setFloat(kOnlinePeakMuAverage, stringToFloat(xml->GetNodeContent(node)) * mod, "OnlinePeakMuAverage");
          }
          node = xml->GetNext(node);
        }
      } else if (listName == "bunchgroups") {
        XMLNodePointer_t node = xml->GetChild(listNode);
        while (node != 0) {
          assert(xml->GetNodeName(node) == std::string("bunchgroup"));
          Int_t id = stringToInt(xml->GetAttr(node, "id"));
          std::string name = xml->GetAttr(node, "name");
          Int_t bunches = stringToInt(xml->GetNodeContent(node));
          m_bunchGroupXML[ id ] = std::pair<std::string, Int_t>(name, bunches);
          node = xml->GetNext(node);
        }
        m_loadedPairedBunches = m_bunchGroupXML[1].second;
        if (runNumber == 310574) {
          if (Config::config().getInt(kInvertHighMuRunVeto) == true) m_loadedPairedBunches = 3;
          else m_loadedPairedBunches -= 3;
        }
        if (Config::config().getIsSet(kPairedBunches) == false) Config::config().set(kPairedBunches, m_loadedPairedBunches, "PairedBunches");
      }

      listNode = xml->GetNext(listNode);
    }

    const Int_t lbBegin = Config::config().getInt(kLumiStart);
    const Int_t lbEnd = Config::config().getInt(kLumiEnd);

    // May want to restrict to a greater extent than in the run XML
    if (lbBegin != INT_MIN && lbBegin > m_minLB && lbBegin <= m_maxLB) m_minLB = lbBegin;
    if (lbEnd != INT_MAX && lbEnd < m_maxLB && lbEnd >= m_minLB) m_maxLB = lbEnd;

    delete xml;
    m_parsedRunXML = kTRUE;
  }

  /**
   * @return if a given LB was flagged bad in the Run XML by rates experts. Usually because of detector issues which
   *could affect rates prediction
   */
  Bool_t TrigXMLService::getIsLBFlaggedBad(Int_t lb) {
    if (m_ignoreGRL) return kFALSE;
    return (Bool_t) m_badLumiBlocks.count(lb);
  }

  Float_t TrigXMLService::getLBMuValue(Int_t lb){
    Float_t mu=m_muPerLB[lb];
    return mu;
  }

  /**
   * @return if we managed to parse the run XML, unlocks XML bunchgroup info and nEvent/LB info
   */
  Bool_t TrigXMLService::getParsedRunXML() {return m_parsedRunXML;}

  /**
   * @return the number of bunch groups loaded in XML
   */
  Int_t TrigXMLService::getNBunchGroups() {
    return (Int_t) m_bunchGroupXML.size();
  }

  /**
   * @return If the file has at least some express stream prescales
   */
  Bool_t TrigXMLService::hasExpressPrescaleInfo() {
    return m_hasExpressPrescaleInfo;
  }

  /**
   * @return if the file has at least some comments on some chains
   */
  Bool_t TrigXMLService::hasChainComments() {
    return m_hasComments;
  }

  /**
   * @return  the number of bunch groups loaded in XML
   */
  Double_t TrigXMLService::getExpressPrescaleInfo(const std::string& name) {
    if (m_chainExpressPS.count(name) == 0) return -1.;

    return m_chainExpressPS[name];
  }

  /**
   * @return  the comment on the chain, if any
   */
  std::string TrigXMLService::getChainComment(const std::string& name) {
    if (m_chainComment.count(name) == 0) return "-";

    return m_chainComment[name];
  }

  /**
   * @return the name of a bunch group loaded from the run XML
   */
  std::string TrigXMLService::getBunchGroupName(Int_t id) {
    return m_bunchGroupXML[ id ].first;
  }

  /**
   * @return the size of a bunch group loaded from the run XML
   */
  Int_t TrigXMLService::getBunchGroupSize(Int_t id) {
    return m_bunchGroupXML[ id ].second;
  }

  /**
   * @return the total number of events to expect for a given LB, lets us know if we saw all the data
   */
  Int_t TrigXMLService::getOnlineEventsInLB(Int_t lb) {
    return m_totalEventsPerLB[ lb ];
  }

  /**
   * Load which PCs have which CPUs.
   * This used to be based on an XML but now is not, hence the naming 
   */
  void TrigXMLService::parseHLTFarmXML() {

    // Now build a map of PU hash to computer type
    const std::string hltLevel = "HLT";

    for (Int_t rack = 94; rack <= 95; ++rack) {
      for (Int_t pc = 1; pc <= 64; ++pc) {
        for (Int_t pu = 1; pu <= 12; ++pu) {
          std::stringstream ss;
          std::string s;
          ss << std::setfill('0')
              << "APP_" << hltLevel
              << ":HLTMPPU-12:HLT-12-NoTS:tpu-rack-"
              << std::setw(2) << rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << rack
              << std::setw(3) << pc
              << "-"
              << std::setw(2) << pu;
          s = ss.str();
          m_PUHashToPUType[stringToIntHash(s)] = rack;
          if (Config::config().debug()) Info("TrigXMLService::parseHLTFarmXML", "%s = %i = %i", s.c_str(), stringToIntHash(s), rack);
        }
      }
    }

    for (Int_t rack = 1; rack <= 70; ++rack) {
      if (rack > 13 && rack < 64) continue;
      for (Int_t pc = 1; pc <= 64; ++pc) {
        for (Int_t pu = 1; pu <= 12; ++pu) {
          std::stringstream ss;
          std::string s;
          ss << std::setfill('0')
              << "APP_" << hltLevel
              << ":HLTMPPU-12:HLT-12:tpu-rack-"
              << std::setw(2) << rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << rack
              << std::setw(3) << pc
              << "-"
              << std::setw(2) << pu;
          s = ss.str();
          m_PUHashToPUType[stringToIntHash(s)] = rack;
        }
      }
    }

    for (Int_t rack = 1; rack <= 70; ++rack) {
      if (rack > 13 && rack < 64) continue;
      for (Int_t pc = 1; pc <= 64; ++pc) {
        for (Int_t pu = 1; pu <= 12; ++pu) {
          std::stringstream ss;
          std::string s;
          ss << std::setfill('0')
              << "APP_" << hltLevel
              << ":HLTMPPU-12:HLT-allracks:tpu-rack-"
              << std::setw(2) << rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << rack
              << std::setw(3) << pc
              << "-"
              << std::setw(2) << pu;
          s = ss.str();
          m_PUHashToPUType[stringToIntHash(s)] = rack;
        }
      }
    }

    for (Int_t rack = 77; rack <= 77; ++rack) {
      for (Int_t pc = 1; pc <= 64; ++pc) {
        for (Int_t pu = 1; pu <= 24; ++pu) {
          std::stringstream ss;
          std::string s;
          ss << std::setfill('0')
              << "APP_" << hltLevel
              << ":HLTMPPU-24:HLT-24-Merge:tpu-rack-"
              << std::setw(2) << rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << rack
              << std::setw(3) << pc
              << "-"
              << std::setw(2) << pu;
          s = ss.str();
          m_PUHashToPUType[stringToIntHash(s)] = rack;
        }
      }
    }

    for (Int_t rack = 16; rack <= 90; ++rack) {
      if (rack > 24 && rack < 70) continue;
      for (Int_t pc = 1; pc <= 64; ++pc) {
        for (Int_t pu = 1; pu <= 24; ++pu) {
          std::stringstream ss;
          std::string s;
          ss << std::setfill('0')
              << "APP_" << hltLevel
              << ":HLTMPPU-24:HLT-24:tpu-rack-"
              << std::setw(2) << rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << rack
              << std::setw(3) << pc
              << "-"
              << std::setw(2) << pu;
          s = ss.str();
          m_PUHashToPUType[stringToIntHash(s)] = rack;
        }
      }
    }

    for (Int_t rack = 25; rack <= 25; ++rack) {
      for (Int_t pc = 1; pc <= 64; ++pc) {
        for (Int_t pu = 1; pu <= 36; ++pu) {
          std::stringstream ss;
          std::string s;
          ss << std::setfill('0')
              << "APP_" << hltLevel
              << ":HLTMPPU-36:HLT-24-HT:tpu-rack-"
              << std::setw(2) << rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << rack
              << std::setw(3) << pc
              << "-"
              << std::setw(2) << pu;
          s = ss.str();
          m_PUHashToPUType[stringToIntHash(s)] = rack;
        }
      }
    }

    for (Int_t rack = 26; rack <= 26; ++rack) {
      for (Int_t pc = 1; pc <= 64; ++pc) {
        for (Int_t pu = 1; pu <= 48; ++pu) {
          std::stringstream ss;
          std::string s;
          ss << std::setfill('0')
              << "APP_" << hltLevel
              << ":HLTMPPU-48:HLT-24-HTPlus:tpu-rack-"
              << std::setw(2) << rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << rack
              << std::setw(3) << pc
              << "-"
              << std::setw(2) << pu;
          s = ss.str();
          m_PUHashToPUType[stringToIntHash(s)] = rack;
        }
      }
    }

    for (Int_t rack = 44; rack <= 44; ++rack) {
      for (Int_t pc = 18; pc <= 20; ++pc) {
        for (Int_t pu = 1; pu <= 28; ++pu) {
          std::stringstream ss;
          std::string s;
          ss << std::setfill('0')
              << "APP_" << hltLevel
              << ":HLTMPPU-24:HLT-24-IT:tpu-rack"
              << std::setw(2) << rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << rack
              << std::setw(3) << pc
              << "-"
              << std::setw(2) << pu;
          s = ss.str();
          m_PUHashToPUType[stringToIntHash(s)] = rack;
        }
      }
    }

    return;
  }

  /**
   * @reutrn The rack number the current PU is located in. Or 0 for unknown
   */
  UInt_t TrigXMLService::getComputerType(UInt_t hash) {
    UIntUIntMapIt_t it = m_PUHashToPUType.find(hash);

    if (it == m_PUHashToPUType.end()) return m_computerUnknownID;

    return (*it).second;
  }

  /**
   * Function to load into memory the enhanced bias weighting factors for a run.
   * These are calculated in advance for each EB run and stored in XML format with one entry for each event.
   * They are loaded into memory in a map which is used to fetch the weight.
   * @param runNumber Which run's XML to load into memory
   */
  void TrigXMLService::parseEnhancedBiasXML(Int_t runNumber) {
    TXMLEngine* xml = new TXMLEngine();

    static Int_t runLoadNumber = -1;

    ++runLoadNumber;
    Info("TrigXMLService::parseEnhancedBiasXML", "Loading Enhanced Bias Weighting XML for %i. This could take a little time. (Run #%i)", runNumber, runLoadNumber);

    // Try one. Use the external (AFS) data path
    Config::config().set(kEBXMLName, "/EnhancedBiasWeights_" + intToString(runNumber) + ".xml", "EBXMLName", kLocked);
    Config::config().set(kEBXMLPath, Config::config().getStr(kAFSDataDir) + "/" + Config::config().getStr(kEBXMLName), "EBXMLPath", kUnlocked);

    XMLDocPointer_t xmlDoc = xml->ParseFile(Config::config().getStr(kEBXMLPath).c_str());
    // Could we load from AFS?

    if (xmlDoc == 0) {
      // No - then try loading locally
      if (Config::config().getInt(kIsRootCore) == kTRUE) {
        Config::config().set(kEBXMLPath, Config::config().getStr(kDataDir) + "/" + Config::config().getStr(kEBXMLName), "EBXMLPath", kUnlocked);
      } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
        std::string locAthena = PathResolverFindCalibFile("TrigCostRootAnalysis" + Config::config().getStr(kEBXMLName));
        if (locAthena == Config::config().getStr(kBlankString)) {
          Error("TrigXMLService::parseEnhancedBiasXML", "Athena cannot find Enhanced Bias weighting file %s. Critical error.", Config::config().getStr(kEBXMLName).c_str());
          abort();
        } else {
          Config::config().set(kEBXMLPath, locAthena);
          Info("TrigXMLService::parseEnhancedBiasXML", "Athena has found the file: %s", Config::config().getStr(kEBXMLPath).c_str());
        }
#endif // not ROOTCORE
      }

      // Load the file - try 2
      xmlDoc = xml->ParseFile(Config::config().getStr(kEBXMLPath).c_str());
      if (xmlDoc == 0) {
        Error("TrigXMLService::parseEnhancedBiasXML", "Unable to load EnhancedBias weighting factors from %s. Critical error.", Config::config().getStr(kEBXMLName).c_str());
        abort();
      }
    }


    // Navigate XML
    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
    assert(xml->GetNodeName(mainNode) == std::string("run"));
    XMLNodePointer_t weightsNode = xml->GetChild(mainNode);
    XMLNodePointer_t eventsNode = xml->GetNext(weightsNode);

    XMLNodePointer_t weightNode = xml->GetChild(weightsNode);
    XMLNodePointer_t eventNode = xml->GetChild(eventsNode);

    while (weightNode != 0) { // Loop over all menu elements
      assert(xml->GetNodeName(weightNode) == std::string("weight")); //Event

      UInt_t id = stringToInt(xml->GetAttr(weightNode, "id")); //Number

      // In MultiRun, we need to apply an offset here
      id += runLoadNumber * 1000;

      Float_t weight = stringToFloat(xml->GetAttr(weightNode, "value")); //Weight
      std::string bunchgroup = xml->GetAttr(weightNode, "bunchgroup"); //BG
      Bool_t unbiased = kFALSE;
      if (runNumber > 266000) unbiased = stringToInt(xml->GetAttr(weightNode, "unbiased"));

      UInt_t bunchgroupID = (UInt_t) kBG_UNSET;
      for (UInt_t i = 0; i < kEBBunchGroupType_SIZE; ++i) {
        if (bunchgroup == BunchGroupNameStr[i]) {
          bunchgroupID = i;
          break;
        }
      }

      if (bunchgroupID == (UInt_t) kBG_UNSET) {
        Error("TrigXMLService::parseEnhancedBiasXML", "I don't know about this bunchgroup: %s", bunchgroup.c_str());
      }

      m_idToWeightMap[id] = weight;
      m_idToBGMap[id] = bunchgroupID;
      m_idToUnbiased[id] = unbiased;

      if (Config::config().debug()) {
        Info("TrigXMLService::parseEnhancedBiasXML", "Enhanced bias weight %.4f has ID %i and bunchgroup %s (%i)",
             weight, id, bunchgroup.c_str(), bunchgroupID);
      }

      weightNode = xml->GetNext(weightNode);
    }

    while (eventNode != 0) { // Loop over all menu elements
      assert(xml->GetNodeName(eventNode) == std::string("e")); //Event
      UInt_t eventNumber = stringToInt(xml->GetAttr(eventNode, "n")); //Number
      UInt_t eventWeightID = stringToInt(xml->GetAttr(eventNode, "w")); //Weight ID

      // In MultiRun, we need to apply an offset here
      eventWeightID += runLoadNumber * 1000;

      assert(m_ebWeightingMap.count(eventNumber) == 0); // Event numbers are unique
      m_ebWeightingMap[eventNumber] = eventWeightID;
      eventNode = xml->GetNext(eventNode);
    }

    Info("TrigXMLService::parseEnhancedBiasXML", "Total of %i event weights loaded. Processed for run %i.", (Int_t) m_ebWeightingMap.size(), runNumber);
    m_weightsServiceEnabled = kTRUE;
    return;
  }

  /**
   * Read (if not already imported) the XML rates and bunch groups for this run.
   * Return the weight for this event and store the event's  bunch group setting using the config service.
   * @param pass Which pass through the file(s), only increment counters on first pass
   * @param bcid Current event BCID
   * @return The event weight from the EnhancedBias XML.
   */
  Float_t TrigXMLService::getEventWeight(UInt_t eventNumber, UInt_t lb, UInt_t pass) {
    static const Int_t runNum = Config::config().getInt(kRunNumber);
    static const std::string bgString = "BunchGroup";
    static const std::string ebString = "EventEBWeight";
    static const std::string rdString = "RandomOnline";
    static const Bool_t ignoreNonPhys = Config::config().getInt(kIgnoreNonPhysBunchGroups);

    // Special case for pb-p
    if (runNum == 218048) {
      Float_t eventWeight = 0;

      if (lb >= 1016) eventWeight = 7650.;
      else if (lb >= 874) eventWeight = 9560.;
      else if (lb >= 753) eventWeight = 11500.;
      else if (lb >= 620) eventWeight = 15300.;
      else if (lb >= 560) eventWeight = 19100.;
      else if (lb >= 558) eventWeight = 28700.;
      else if (lb >= 557) eventWeight = 19100.;
      else if (lb >= 529) eventWeight = 28700.;

      Config::config().set(kCurrentEventBunchGroupID, 1, bgString, kUnlocked); // only filled
      Config::config().setFloat(kCurrentEventEBWeight, eventWeight, ebString, kUnlocked);
      Config::config().set(kCurrentEventWasRandomOnline, 1, rdString, kUnlocked); // As "minbias"
      return eventWeight;
    }


    // if (Config::config().getInt(kDoEBWeighting) == kFALSE) return 1.;
    if (m_weightsServiceEnabled == kFALSE) {
      parseEnhancedBiasXML(Config::config().getInt(kRunNumber));
      std::vector<Int_t> multiRun = Config::config().getIntVec(kMultiRun);
      for (Int_t mr : multiRun) parseEnhancedBiasXML(mr);
    }

    IntIntMapIt_t ebIt = m_ebWeightingMap.find(eventNumber);
    Int_t weightID = 0;
    if (ebIt == m_ebWeightingMap.end()) {
      if (Config::config().getDisplayMsg(kMsgXMLWeight) == kTRUE) {
        Warning("TrigXMLService::getEventWeight", "Cannot find the weight for event %i in LB %i, will use weight = 0. !!", eventNumber, lb);
      }
      Config::config().set(kCurrentEventBunchGroupID, (Int_t) kBG_UNSET, bgString, kUnlocked);
      return 0.;
    } else {
      weightID = ebIt->second;
    }

    Float_t eventWeight = m_idToWeightMap[weightID];
    UInt_t eventBG = m_idToBGMap[weightID];

    // Option to disable (weight = 0) events if they're not physics collisions
    if (ignoreNonPhys == 1) {
      if (eventBG != 1) eventWeight = 0.;
    }

    // Store the event BG
    Config::config().set(kCurrentEventBunchGroupID, eventBG, bgString, kUnlocked);
    Config::config().setFloat(kCurrentEventEBWeight, eventWeight, ebString, kUnlocked);
    if (pass == 1) m_eventsPerBGCounter[eventBG] += 1;
    // Store if the event was unbiased online (triggered by random).
    // In run 2 wedo something more fancy
    Int_t isRandom = 0;
    if (runNum > 266000) {
      isRandom = m_idToUnbiased[weightID];
    } else {
      if (eventWeight > Config::config().getFloat(kUnbiasedWeightThreshold)) isRandom = 1;
    }
    if (isRandom && pass == 1) m_unbiasedPerBGCounter[eventBG] += 1;
    Config::config().set(kCurrentEventWasRandomOnline, isRandom, rdString, kUnlocked);

    return eventWeight;
  }

  /**
   * Get the map which says how many events were of each bunch group
   * @return reference of BunchGroupID -> N events map
   */
  IntIntMap_t& TrigXMLService::getBGMap() {
    return m_eventsPerBGCounter;
  }

  /**
   * Get the map which says how many unbiased (RD0 triggered online) events were of each bunch group
   * @return reference of BunchGroupID -> N unbiased events map
   */
  IntIntMap_t& TrigXMLService::getBGUnbiasedMap() {
    return m_unbiasedPerBGCounter;
  }

  /**
   * Export enhanced bias weighting values to XML for use by everyone in this application.
   * This function is called on every event when using the expert option --writeEBXML.
   * Ehanced bias weights, calculated in Athena, are read from the D3PD. Each floating point
   * weight is assigned a numeric ID and event numbers are matched to these IDs.
   * The XML should be coppied to a central accessible location, this is currently
   * /afs/cern.ch/user/a/attradm/public
   * @param eventNumber The current event number
   * @param weight The enhanced bias (un)weighting value for this event, based on the online prescale set and L1 pass
   *before prescale bits.
   */
  void TrigXMLService::exportEnhancedBiasXML(UInt_t eventNumber, Float_t weight, UInt_t bunchGroup, Int_t unbiased) {
    static std::pair<std::string, Float_t> recyclablePair;

    recyclablePair.first = BunchGroupNameStr[bunchGroup];
    recyclablePair.second = weight;

    m_ebWeightTypesCount[recyclablePair] += 1;

    Int_t location = -1;
    std::vector< std::pair<std::string, Float_t> >::iterator weightTypeIt = std::find(m_ebWeightTypes.begin(), m_ebWeightTypes.end(), recyclablePair);
    if (weightTypeIt == m_ebWeightTypes.end()) {
      //Add me
      m_ebWeightTypes.push_back(recyclablePair);
      m_ebWeightUnbiased[weight] = unbiased;
      location = m_ebWeightTypes.size() - 1;
    } else {
      location = weightTypeIt - m_ebWeightTypes.begin();
    }
    m_ebWeightingMapSave[eventNumber] = location;
  }

  /**
   * Produce the actual XML output
   * @see TrigXMLService::exportEnhancedBiasXML(UInt_t eventNumber, Float_t weight)
   */
  void TrigXMLService::saveExportedEnhancedBiasXML() {
    std::ofstream ebOut;
    const std::string outputEBWeight = "EnhancedBiasWeights_"
                                        + intToString(Config::config().getInt(kRunNumber))
                                        + ".xml";
    ebOut.open(outputEBWeight.c_str());
    XMLExport ebXML(ebOut);
    ebXML.setSpaces(0);
    ebXML.addNode(ebOut, "run");
    ebXML.addNode(ebOut, "weights");
    for (UInt_t i = 0; i < m_ebWeightTypes.size(); ++i) {
      ebXML.addNodeWProperty(ebOut, "weight",
                              "id", intToString(i),
                              "value", floatToString(m_ebWeightTypes.at(i).second, 7),
                              "bunchgroup", m_ebWeightTypes.at(i).first,
                              "unbiased", intToString(m_ebWeightUnbiased[ m_ebWeightTypes.at(i).second ]));
      Info("TrigXMLService::saveExportedEnhancedBiasXML", "Weight %f of BunchGroup %s has ID %i, Unbiased %i", m_ebWeightTypes.at(i).second, m_ebWeightTypes.at(i).first.c_str(), i, m_ebWeightUnbiased[ m_ebWeightTypes.at(i).second ]);
    }
    ebXML.endNode(ebOut);//weights
    ebXML.addNode(ebOut, "events");
    for (IntIntMapIt_t it = m_ebWeightingMapSave.begin(); it != m_ebWeightingMapSave.end(); ++it) {
      ebXML.addNodeWProperty(ebOut, "e", "n", intToString(it->first), "w", intToString(it->second));
    }
    ebXML.endNode(ebOut);//events
    ebXML.endNode(ebOut);//run
    ebOut.close();
    for (PairStringFloat_Float_MapIt_t it = m_ebWeightTypesCount.begin(); it != m_ebWeightTypesCount.end(); ++it) {
      Info("TrigXMLService::saveExportedEnhancedBiasXML", "%i events with enhanced bias weight %f and BG %s", it->second, it->first.second, it->first.first.c_str());
    }
  }

  /**
   * Write a simple dummy file Prescales.xml which contains the structure of the trigger menu, and the default prescales
   *used when
   * the trigger was run to produce the ntuple.
   * This file can then be modified by hand etc. by the end user and used to apply a custom prescale set to the trigger
   *in
   * subsiquent rate prediction campaigns.
   *
   * This is an alternative to getting the initial prescale set using the Rule Book.
   */
  void TrigXMLService::writePrescaleXML() {
    const std::string xmlName = Config::config().getStr(kOutputRootDirectory) + "/" + "Prescales.xml";

    std::ofstream fout;
    fout.open(xmlName.c_str());
    fout << std::fixed; // Use fixed width output

    Info("TrigXMLService::writePrescaleXML", "Writing a dummy file Prescales.xml with current menu.");

    XMLExport xml(fout);
    xml.setSpaces(2);
    xml.addNode(fout, "trigger");

    // Loop over all chains in metadata trigger config folder
    Int_t level = -1;
    for (UInt_t c = 0; c < TrigConfInterface::getChainN(); ++c) {
      if (TrigConfInterface::getChainLevel(c) == 1 && level == -1) {
        level = 1;
        xml.addNode(fout, "level");
        xml.addNode(fout, "lvl_name", "L1");
      } else if (TrigConfInterface::getChainLevel(c) == 2 && level == 1) {
        //switch over to HLT
        level = 2;
        xml.endNode(fout); //level
        xml.addNode(fout, "level");
        xml.addNode(fout, "lvl_name", "HLT");
      }

      xml.addNode(fout, "signature");
      std::string name = TrigConfInterface::getChainName(c);
      xml.addNode(fout, "sig_name", name);
      xml.addNode(fout, "chain_prescale", floatToString(TrigConfInterface::getPrescale(name)));
      xml.endNode(fout); //signature
    }
    xml.endNode(fout); //level
    xml.endNode(fout); //Trigger
    fout.close();

    Info("TrigXMLService::writePrescaleXML", "Exported XML with %i chains.", TrigConfInterface::getChainN());
  }

  void TrigXMLService::saveRuleBookXML(CounterCollection_t& counterCollections, std::string levelStr) {
    if (Config::config().getInt(kOutputXML) == kFALSE) return;

    // Save tables. Loop over counter collections.
    CounterCollectionIt_t colIt = counterCollections.begin();
    for (; colIt != counterCollections.end(); ++colIt) {
      const std::string counterCollectionName = colIt->first;
      const CounterMap_t* counterMap = &(colIt->second);
      // Skip if there are no counters to process
      if (counterMap->size() == 0) continue;

      const std::string outputFolder = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(kOutputXMLDirectory);
      gSystem->mkdir(outputFolder.c_str(), kTRUE);

      const std::string xmlName = outputFolder
                                   + "/TrigRate_"
                                   + Config::config().getStr(kOutputTag) + "_"
                                   + levelStr + "_"
                                   + counterCollectionName
                                   + ".xml";
      std::ofstream fout;
      fout.open(xmlName.c_str());
      fout << std::fixed; // Use fixed width output

      if (Config::config().debug()) {
        Info("TrigXMLService::saveRuleBookXML", "Doing XML output with path %s.", xmlName.c_str());
      }

      XMLExport xml(fout);
      xml.setSpaces(2);
      xml.addNode(fout, "trigger");

      // Do General Info
      xml.addNode(fout, "Xsection", intToString(0)); // TODO

      Float_t runLumi = 0.;
      if (Config::config().getIsSet(kRunLumi)) runLumi = Config::config().getFloat(kRunLumi);
      else if (Config::config().getIsSet(kRunLumiXML)) runLumi = Config::config().getFloat(kRunLumiXML);
      xml.addNode(fout, "Luminosity", floatToString(runLumi));

      xml.addNode(fout, "GenEff", intToString(0)); // TODO
      xml.addNode(fout, "n_evts", intToString(Config::config().getLong(kEventsProcessed)));

      Float_t predictionLumi = 0.;
      if (Config::config().getIsSet(kPredictionLumi)) predictionLumi = Config::config().getFloat(kPredictionLumi);
      else if (Config::config().getIsSet(kPredictionLumiMenuXML)) predictionLumi = Config::config().getFloat(kPredictionLumiMenuXML);
      else if (Config::config().getIsSet(kPredictionLumiRunXML)) predictionLumi = Config::config().getFloat(kPredictionLumiRunXML);
      xml.addNode(fout, "PredictionLumi", floatToString(predictionLumi));

      for (UInt_t f = 0; f < Config::config().getVecSize(kInputFiles); ++f) {
        xml.addNode(fout, "Dataset", Config::config().getVecEntry(kInputFiles, f));
      }
      xml.addNode(fout, "AtlasProject", TrigConfInterface::getMetaStringVal("AtlasProject"));
      xml.addNode(fout, "AtlasVersion", TrigConfInterface::getMetaStringVal("AtlasVersion"));
      xml.addNode(fout, "triggerMenuSetup", TrigConfInterface::getMetaStringVal("triggerMenuSetup"));
      xml.addNode(fout, "L1PrescaleSet", TrigConfInterface::getMetaStringVal("L1PrescaleSet"));
      xml.addNode(fout, "HLTPrescaleSet", TrigConfInterface::getMetaStringVal("HLTPrescaleSet"));
      xml.addNode(fout, "CMTPATH", TrigConfInterface::getMetaStringVal("CMTPATH"));

      // Do Bunch Group info
      // Currently we prefer XML
      if (TrigXMLService::trigXMLService().getParsedRunXML() == kTRUE) {
        xml.addNode(fout, "bunchgroup");
        for (Int_t bg = 0; bg < TrigXMLService::trigXMLService().getNBunchGroups(); ++bg) {
          xml.addNode(fout, "bunchgrouptype");
          xml.addNode(fout, "bunchgroup_keynum", intToString(bg));
          xml.addNode(fout, "bunchgroup_key", TrigXMLService::trigXMLService().getBunchGroupName(bg));
          xml.addNode(fout, "bunchgroup_size", intToString(TrigXMLService::trigXMLService().getBunchGroupSize(bg)));
          xml.endNode(fout); // bunchgrouptype
        }
        xml.endNode(fout); //bunchgroup
      } else {
        // Otherwise try from ntuple (this is broken 06/15) TODO fix in athena
        StringIntMap_t bunchGroups = TrigConfInterface::getBunchGroupSetup();
        UInt_t bgCounter = 0;
        xml.addNode(fout, "bunchgroup");
        for (StringIntMapIt_t it = bunchGroups.begin(); it != bunchGroups.end(); ++it) {
          xml.addNode(fout, "bunchgrouptype");
          xml.addNode(fout, "bunchgroup_keynum", intToString(bgCounter++));
          xml.addNode(fout, "bunchgroup_key", it->first);
          xml.addNode(fout, "bunchgroup_size", intToString(it->second));
          xml.endNode(fout); // bunchgrouptype
        }
        xml.endNode(fout); //bunchgroup
      }

      Bool_t hasL1 = kFALSE, hasL2 = kFALSE, hasHLT = kFALSE;
      for (CounterMapIt_t counterMapIt = colIt->second.begin(); counterMapIt != colIt->second.end(); ++counterMapIt) {
        CounterBaseRates* counter = static_cast<CounterBaseRates*>(counterMapIt->second);
        if (counter->getStrDecoration(kDecType) == "L1") hasL1 = kTRUE;
        else if (counter->getStrDecoration(kDecType) == "L2") hasL2 = kTRUE;
        else if (counter->getStrDecoration(kDecType) == "HLT" || counter->getStrDecoration(kDecType) == "Chain") hasHLT = kTRUE;
      }

      if (hasL1) { //Add L1 data
        xml.addNode(fout, "level");
        xml.addNode(fout, "lvl_name", "L1");
        for (CounterMapIt_t counterMapIt = colIt->second.begin(); counterMapIt != colIt->second.end(); ++counterMapIt) {
          CounterBaseRates* counter = static_cast<CounterBaseRates*>(counterMapIt->second);
          if (counter->getStrDecoration(kDecType) != "L1") continue;
          saveXMLElement(fout, xml, counter);
        }
        xml.endNode(fout); //level
      }
      if (hasL2) { //Add L2 data
        xml.addNode(fout, "level");
        xml.addNode(fout, "lvl_name", "L2");
        for (CounterMapIt_t counterMapIt = colIt->second.begin(); counterMapIt != colIt->second.end(); ++counterMapIt) {
          CounterBaseRates* counter = static_cast<CounterBaseRates*>(counterMapIt->second);
          if (counter->getStrDecoration(kDecType) != "L2") continue;
          saveXMLElement(fout, xml, counter);
        }
        xml.endNode(fout); //level
      }
      if (hasHLT) { //Add HLT data
        xml.addNode(fout, "level");
        xml.addNode(fout, "lvl_name", "HLT");
        for (CounterMapIt_t counterMapIt = colIt->second.begin(); counterMapIt != colIt->second.end(); ++counterMapIt) {
          CounterBaseRates* counter = static_cast<CounterBaseRates*>(counterMapIt->second);
          if (counter->getStrDecoration(kDecType) != "Chain") continue;
          saveXMLElement(fout, xml, counter);
        }
        xml.endNode(fout); //level
      }

      xml.endNode(fout); //trigger

      fout.close();
    }
  }

  /**
   * Save all rate data for a single chain into an output XML stream
   * @param fout Reference to output stream.
   * @param xml Reference to XML export engine.
   * @returns counter Counter to export.
   */
  void TrigXMLService::saveXMLElement(std::ofstream& fout, XMLExport& xml, CounterBaseRates* counter) {
    const UInt_t xmlPrecision = 7;

    Float_t evPassWeight = counter->getValue(kVarEventsPassed, kSavePerCall);
    Float_t evPassWeightErr = counter->getValueError(kVarEventsPassed, kSavePerCall); // Get sqrt(sumW2)

    Float_t evRun = counter->getValue(kVarEventsRun, kSavePerCall); // EB Weighted
    Float_t evPassNoPS = counter->getValue(kVarEventsPassedNoPS, kSavePerCall); // EB Weighted

    Float_t evPassRawStat = counter->getValue(kVarEventsPassRawStat, kSavePerCall); // Not EB Weighted
    //Float_t evRunRawStat  = counter->getValue(kVarEventsRunRawStat,  kSavePerCall); // Not EB Weighted

    Float_t walltime = counter->getDecoration(kDecLbLength);

    Float_t rate = evPassWeight / walltime;
    Float_t rateErr = evPassWeightErr / walltime;    // err = sqrt(events in time T)/T = sqrt(rate*T/T^2) =
                                                        // sqrt(rate/T)

    Float_t uniqueRate = counter->getDecoration(kDecUniqueRate);
    Float_t uniqueFraction = counter->getDecoration(kDecUniqueFraction); // Was in percent, change back to 0-1

    Bool_t isL1 = kFALSE;

    if (counter->getStrDecoration(kDecType) == "L1") isL1 = kTRUE;
    UNUSED(isL1);

    // TODO - THIS IS WRONG FOR WEIGHTED EVENTS
    Float_t eff = 0., effErr = 0., psEff = 0., psEffErr = 0.;
    if (evRun) {
      eff = evPassNoPS / evRun;
      effErr = (1. / evRun) * TMath::Sqrt(evPassNoPS * (1. - eff)); // Binomal
      psEff = evPassWeight / evRun;
      psEffErr = (1. / evRun) * TMath::Sqrt(evPassWeight * (1. - psEff)); // Binomal
    }

    UNUSED(effErr);
    UNUSED(psEffErr);

    xml.addNode(fout, "signature");
    xml.addNode(fout, "sig_name", counter->getName());
    xml.addNode(fout, "express_stream", intToString(0)); // TODO add me!
    xml.addNode(fout, "chain_prescale", floatToString(counter->getBasicPrescale())); // This holds the *item* PS
    xml.addNode(fout, "passthrough", "0");
    xml.addNode(fout, "lower_chain_name", "");
    xml.addNode(fout, "evts_passed", floatToString(evPassRawStat, xmlPrecision));
    xml.addNode(fout, "evts_passed_weighted", floatToString(evPassWeight, xmlPrecision));
    xml.addNode(fout, "efficiency", floatToString(eff, xmlPrecision));
    //xml.addNode(fout, "efficiency_err", floatToString( effErr, xmlPrecision)); // TODO FIX ME
    xml.addNode(fout, "efficiency_err", floatToString((Float_t) 0.));
    xml.addNode(fout, "prescaled_efficiency", floatToString(psEff, xmlPrecision));
    //xml.addNode(fout, "prescaled_efficiency_err", floatToString( psEffErr, xmlPrecision)); // TODO FIX ME
    xml.addNode(fout, "prescaled_efficiency_err", floatToString((Float_t) 0.));
    xml.addNode(fout, "rate", floatToString(rate, xmlPrecision));
    xml.addNode(fout, "rate_err", floatToString(rateErr, xmlPrecision));
    xml.addNode(fout, "unique_fraction", floatToString(uniqueFraction, xmlPrecision));
    xml.addNode(fout, "unique_rate", floatToString(uniqueRate, xmlPrecision));
    xml.endNode(fout); //signature
  }

  /**
   * We need extra information which is kept in upgrade XMLs. These share similarity with standard rate XMLs but have
   *additional fields
   * @param isoBits Pass by reference to map of isolation ID string to isolation bits map, to read from XML
   * @param upgradeChains Pass by reference to set of ChainInfo objects containing all info needed to create chain
   *counters, to read from XML
   */
  void TrigXMLService::parseUpgradeXML(StringIntMap_t& isoBits, std::multiset<ChainInfo>& upgradeChains) {
    const std::string scenario = Config::config().getStr(kUpgradeScenario);
    const std::string file = std::string("upgrade_") + scenario + std::string("_menu.xml");

    std::string path;

    if (Config::config().getInt(kIsRootCore) == kTRUE) {
      path = std::string(Config::config().getStr(kDataDir) + file);
    } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
      path = PathResolverFindDataFile(file);
#endif // not ROOTCORE
    }

    // If file exists
    TXMLEngine* xml = new TXMLEngine();
    XMLDocPointer_t xmlDoc = 0;
    std::string level = "none";

    if (path != Config::config().getStr(kBlankString) && access(path.c_str(), F_OK) != -1) {
      xmlDoc = xml->ParseFile(path.c_str());
    }

    if (xmlDoc == 0) {
      Error("MonitorRatesUpgrade::parseUpgradeXML", "Cannot find upgrade XML %s, critical error - must be supplied. Use --upgradeScenario.", file.c_str());
      delete xml;
      abort();
    }

    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
    assert(xml->GetNodeName(mainNode) == std::string("trigger"));
    XMLNodePointer_t listNode = xml->GetChild(mainNode);

    while (listNode != 0) { // Loop over all menu elements
      const std::string listName = xml->GetNodeName(listNode);
      XMLNodePointer_t node = xml->GetChild(listNode);
      if (listName == "lumivalues") {
        while (node != 0) {
          if (xml->GetNodeName(node) == std::string("target_paired_bcid")) {
            Config::config().set(kTargetPairedBunches, stringToInt(xml->GetNodeContent(node)), "TargetPairedBunches");
          } else if (xml->GetNodeName(node) == std::string("target_peak_mu_av")) {
            Config::config().setFloat(kTargetPeakMuAverage, stringToFloat(xml->GetNodeContent(node)), "TargetPeakMuAverage");
          }
          node = xml->GetNext(node);
        }
      } else if (listName == "iso_bits") {
        while (node != 0) {
          if (xml->GetNodeName(node) == std::string("iso")) {
            std::bitset<5> bits(std::string(xml->GetAttr(node, "bits")));
            isoBits[ xml->GetAttr(node, "name") ] = bits.to_ulong();
            Info("MonitorRatesUpgrade::parseUpgradeXML", "Got isobits for %s = %s => %s = %i",
                 xml->GetAttr(node, "name"), xml->GetAttr(node, "bits"), bits.to_string().c_str(), isoBits[xml->GetAttr(node, "name")]);
          }
          node = xml->GetNext(node);
        }
      } else if (listName == "level") {
        while (node != 0) {
          if (xml->GetNodeName(node) == std::string("lvl_name")) {
            level = xml->GetNodeContent(node);
            node = xml->GetNext(node);
            if (node == 0) break;
          }
          assert(xml->GetNodeName(node) == std::string("signature"));
          XMLNodePointer_t detailsNode = xml->GetChild(node);
          std::string sigName = "", group = "", comment = "", hltProxy = "", lower = "";
          Float_t l1ReductionFactor = 1., l2ReductionFactor = 1., hltReductionFactor = 1.;
          TriggerLogic triggerLogic;

          while (detailsNode != 0) {
            const std::string detail = xml->GetNodeName(detailsNode);
            if (detail != "conditions" && xml->GetNodeContent(detailsNode) == nullptr) { // Avoid NULL except with
                                                                                            // conditions
              detailsNode = xml->GetNext(detailsNode);
              continue;
            }
            if (detail == "sig_name") sigName = xml->GetNodeContent(detailsNode);
            else if (detail == "hlt_proxy") hltProxy = xml->GetNodeContent(detailsNode);
            else if (detail == "group") group = xml->GetNodeContent(detailsNode);
            else if (detail == "comment") comment = xml->GetNodeContent(detailsNode);
            else if (detail == "lower_chain_name") lower = xml->GetNodeContent(detailsNode);
            else if (detail == "l1_reduction_factor") l1ReductionFactor = stringToFloat(xml->GetNodeContent(detailsNode));
            else if (detail == "l2_reduction_factor") l2ReductionFactor = stringToFloat(xml->GetNodeContent(detailsNode));
            else if (detail == "hlt_reduction_factor") hltReductionFactor = stringToFloat(xml->GetNodeContent(detailsNode));
            else if (detail == "conditions") {
              XMLNodePointer_t requirement = xml->GetChild(detailsNode);
              while (requirement != 0) {
                std::string name = xml->GetAttr(requirement, "name");
                UInt_t multi = stringToInt(xml->GetAttr(requirement, "multi"));
                UInt_t thresh = stringToInt(xml->GetAttr(requirement, "triggerthreshold"));
                Int_t iso = isoBits[ xml->GetAttr(requirement, "iso") ];
                UInt_t min = stringToInt(xml->GetAttr(requirement, "min"));
                UInt_t max = stringToInt(xml->GetAttr(requirement, "max"));
                triggerLogic.addCondition(name, multi, thresh, iso, min, max);
                requirement = xml->GetNext(requirement);
              }
            } else if (detail == "OR") {
              Error("MonitorRatesUpgrade::parseUpgradeXML", "The OR of TOBs is currently unsupported");
            } else {
              Error("MonitorRatesUpgrade::parseUpgradeXML", "Unknown XML detail: %s", detail.c_str());
            }
            detailsNode = xml->GetNext(detailsNode);
          }
          if (level == "L1") {
            Info("MonitorRatesUpgrade::parseUpgradeXML", "Will do L1->L2 chain: %s with logic: %s", sigName.c_str(), triggerLogic.print().c_str());
            upgradeChains.insert(ChainInfo(sigName, 1, triggerLogic, group, comment, l1ReductionFactor, l2ReductionFactor));
          } else if (level == "HLT") {
            upgradeChains.insert(ChainInfo(sigName, 3, hltProxy, lower, comment, hltReductionFactor));
          } else {
            Error("MonitorRatesUpgrade::parseUpgradeXML", "Signatures from unknown trigger level: %s", level.c_str());
          }
          node = xml->GetNext(node);
        }
      }

      listNode = xml->GetNext(listNode);
    }

    delete xml;
  }
} // namespace TrigCostRootAnalysis
