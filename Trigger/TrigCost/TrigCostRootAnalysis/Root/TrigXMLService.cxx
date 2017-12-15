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
    std::vector<Int_t> _multiRun = Config::config().getIntVec(kMultiRun);
    for (Int_t _mr : _multiRun) {
      if (_mr == Config::config().getInt(kRunNumber)) {
        Fatal("TrigXMLService::TrigXMLService",
              "If doing MultiRun, provide the 'primary' run's inputs first, followed by the additional runs whose run numbers were speificed to --multiRun");
        //Bool_t _primaryRunOpenedFirstBeforeMultiRunInputFiles = kFALSE;
        //assert(_primaryRunOpenedFirstBeforeMultiRunInputFiles);
        std::abort();
      }
      parseRunXML(_mr, kFALSE);
    }

    if (Config::config().getIsSet(kPrescaleXMLPath1) == kTRUE) parseXML(1);
    if (Config::config().getIsSet(kPrescaleXMLPath2) == kTRUE) parseXML(2);
    if (m_serviceEnabled == kFALSE) {
      Warning("TrigXMLService::TrigXMLService",
              "User did not supply any prescale XML files. L1 PS=%f and HLT PS=%f will be used.",
              Config::config().getFloat(kRateFallbackPrescaleL1), Config::config().getFloat(kRateFallbackPrescaleHLT));
    }
  }

  Double_t TrigXMLService::getPrescale(const std::string& _name) {
    if (m_serviceEnabled == kFALSE) {
      if (_name.substr(0, 3) == "L1_") {
        return (Double_t) Config::config().getFloat(kRateFallbackPrescaleL1);
      } else {
        return (Double_t) Config::config().getFloat(kRateFallbackPrescaleHLT);
      }
    }

    StringDoubleMapIt_t _it = m_chainPS.find(_name);
    if (_it == m_chainPS.end()) {
      if (_name.substr(0, 3) == "L1_") {
        if (Config::config().getDisplayMsg(kMsgXMLPrescale) == kTRUE) {
          Warning("TrigXMLService::getPrescale",
                  "Cannot find rates prescale for chain %s in the supplied XML. Will return fallback L1 PS:%f.",
                  _name.c_str(),
                  Config::config().getFloat(kRateFallbackPrescaleL1));
        }
        return (Double_t) Config::config().getFloat(kRateFallbackPrescaleL1);
      } else {
        if (Config::config().getDisplayMsg(kMsgXMLPrescale) == kTRUE) {
          Warning("TrigXMLService::getPrescale",
                  "Cannot find rates prescale for chain %s in the supplied XML. Will return fallback HLT PS:%f.",
                  _name.c_str(),
                  Config::config().getFloat(kRateFallbackPrescaleHLT));
        }
        return (Double_t) Config::config().getFloat(kRateFallbackPrescaleHLT);
      }
    }

    return _it->second;
  }

  /**
   * When running cost analysis in the HLT and looking at the effect of prescales the following should be taken into
   *account:
   * - All data should be scaled down by 1/PSL1 * 1/PSHLT
   * - If the HLT trigger or L1 trigger is disabled, the weight should be 0.
   * @param _chainName Name of the chain to get the weighting factor for. Only makes sense to this fn for HLT chains
   * @return the calculated multiplicitive weighting factor
   */
  Double_t TrigXMLService::getHLTCostWeightingFactor(const std::string& _chainName) {
    Double_t _prescaleHLT = getPrescale(_chainName);

    if (_prescaleHLT < 0.) return 0.;

    const std::string _myLowerChain = TrigConfInterface::getLowerChainName(_chainName);
    // Check we're not multi-seeded
    if (_myLowerChain.find(",") != std::string::npos) return 1. / _prescaleHLT; //What else could we do here?

    Double_t _lowerPS = getPrescale(_myLowerChain);
    if (_lowerPS < 0.) return 0.;

    return (1. / _prescaleHLT) * (1. / _lowerPS);
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
    Float_t _runLumi;
    if (Config::config().getIsSet(kRunLumi)) _runLumi = Config::config().getFloat(kRunLumi); // Prefer user supplied
    else _runLumi = Config::config().getFloat(kRunLumiXML); // Fallbac on XML

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

    Float_t _predictionLumi, _onlineDeadtime = 0.;
    std::string _predFrom;
    // Prefer the user CLI value, then the value from the prescale XML, then the default lumi point from the run XML
    if (Config::config().getIsSet(kPredictionLumi)) {
      _predFrom = "command line";
      _predictionLumi = Config::config().getFloat(kPredictionLumi);
    } else if (Config::config().getIsSet(kPredictionLumiMenuXML)) {
      _predFrom = "prescale XML";
      _predictionLumi = Config::config().getFloat(kPredictionLumiMenuXML);
    } else {
      _predFrom = "run XML (the default target L point)";
      _predictionLumi = Config::config().getFloat(kPredictionLumiRunXML);
    }

    if (Config::config().getIsSet(kOnlineDeadtime) && Config::config().getInt(kNoOnlineDeadtimeCorrection) == kFALSE) {
      _onlineDeadtime = Config::config().getFloat(kOnlineDeadtime);
    }

    // This should not be zero, and if it is set then it's set explicitly. Still, check.
    if (isZero(_predictionLumi) || isZero(_runLumi)) {
      Warning("TrigXMLService::getLumiExtrapWeight", "The prediction/run lumi of this run has been set to zero. "
                                                     "This is obviously wrong. No scaling will be perfomed.");
      return 1.;
    }

    Float_t _scalingFactor = _predictionLumi / _runLumi;
    _scalingFactor *= 1 + _onlineDeadtime;

    Double_t _averageL = 0.;
    Int_t _averageLN = 0;

    // Do we have a target mu?
    if (!isZero(Config::config().getFloat(kTargetPeakMuAverage)) && Config::config().getIsSet(kOnlinePeakMuAverage)) {
      Bool_t _doExponentialMu = Config::config().getInt(kDoExponentialMu);

      Float_t _lumiScaling = _predictionLumi / _runLumi;

      Float_t _targetMu = Config::config().getFloat(kTargetPeakMuAverage);
      Float_t _onlineMu = Config::config().getFloat(kOnlinePeakMuAverage);

      Float_t _expoRateScaleModifierL1 = Config::config().getFloat(kExpoRateScaleModifierL1);
      Float_t _expoRateScaleModifierHLT = Config::config().getFloat(kExpoRateScaleModifierHLT);

      Float_t _lumiMuScaling = _targetMu / _onlineMu;
      Float_t _lumiBunchScaling = _lumiScaling / _lumiMuScaling;

      Float_t _lumiMuScalingExpoL1 = TMath::Exp((_targetMu - _onlineMu) * _expoRateScaleModifierL1);
      Float_t _lumiMuScalingExpoHLT = TMath::Exp((_targetMu - _onlineMu) * _expoRateScaleModifierHLT);

      if (_doExponentialMu == kFALSE) {
        _lumiMuScalingExpoL1 = _lumiMuScaling;
        _lumiMuScalingExpoHLT = _lumiMuScaling;
      }

      Float_t _lumiScalingExpoL1 = _lumiBunchScaling * _lumiMuScalingExpoL1 * (1 + _onlineDeadtime);
      Float_t _lumiScalingExpoHLT = _lumiBunchScaling * _lumiMuScalingExpoHLT * (1 + _onlineDeadtime);
      _lumiBunchScaling *= (1 + _onlineDeadtime);
      _lumiMuScaling *= (1 + _onlineDeadtime);

      Int_t _maxBunches = Config::config().getInt(kMaxBunches);
      Int_t _maxBCIDs = Config::config().getInt(kMaxBCIDs);
      Int_t _targetBunches = (Int_t) std::round(m_bunchGroupXML[1].second * _lumiBunchScaling);
      Info("TrigXMLService::getLumiExtrapWeight", "PredictionLumi taken from %s.", _predFrom.c_str());
      Info("TrigXMLService::getLumiExtrapWeight",
           "Using targetMu setting %.2f. <mu> scaling factor: %.2f->%.2f = %.2f. Expo. L1 = %.2f. Expo. HLT = %.2f",
           _targetMu, _onlineMu, _targetMu, _lumiMuScaling, _lumiMuScalingExpoL1, _lumiMuScalingExpoHLT);
      Info("TrigXMLService::getLumiExtrapWeight", "Bunch scaling factor: %i->%i = %.2f",
           m_bunchGroupXML[1].second, _targetBunches, _lumiBunchScaling);
      Info("TrigXMLService::getLumiExtrapWeight",
           "Online deadtime was %.2f%%, including deadtime - the run averaged lumi scaling factors are: "
           "linear = %.2f, expo. in <mu> L1 = %.2f, expo. in <mu> HLT = %.2f, bunch only = %.2f, mu only = %.2f",
           _onlineDeadtime * 100., _scalingFactor, _lumiScalingExpoL1, _lumiScalingExpoHLT, _lumiBunchScaling,
           _lumiMuScaling);
      if (_targetBunches > _maxBunches + 15 /*allow wiggle room*/ || _targetBunches < 1) {
        Warning("TrigXMLService::getLumiExtrapWeight",
                "To get to L=%.2e with a --targetMu of %.2f requires %i bunches. A full ring is %i!",
                _predictionLumi, _targetMu, _targetBunches, _maxBunches);
      }
      // Can request exponential scaling in <mu>

      // Some extra calculations for EMPTY bunchgroup scaling
      Int_t _currentEmptyBunches = std::max(0, _maxBCIDs - m_bunchGroupXML[1].second); // Filled
      Int_t _targetEmptyBunches = std::max(0, _maxBCIDs - _targetBunches);
      Float_t _emptyExtrap = 0.;
      if (_currentEmptyBunches > 0) _emptyExtrap = _targetEmptyBunches / (Float_t) _currentEmptyBunches;

      // Write info
      Config::config().set(kDoAdvancedLumiScaling, 1, "DoAdvancedLumiScaling");
      Config::config().setFloat(kPredictionLumiFinalMuComponent, _lumiMuScaling, "PredictionLumiFinalMuComponent");
      Config::config().setFloat(kPredictionLumiFinalMuExpoL1Component, _lumiMuScalingExpoL1,
                                "PredictionLumiFinalExponentialMuL1Component");
      Config::config().setFloat(kPredictionLumiFinalMuExpoHLTComponent, _lumiMuScalingExpoHLT,
                                "PredictionLumiFinalExponentialMuHLTComponent");
      Config::config().setFloat(kPredictionLumiFinalBunchComponent, _lumiBunchScaling,
                                "PredictionLumiFinalBunchComponent");
      Config::config().setFloat(kPredictionLumiFinalExpoL1, _lumiScalingExpoL1, "PredictionLumiFinalExponentialMuL1");
      Config::config().setFloat(kPredictionLumiFinalExpoHLT, _lumiScalingExpoHLT,
                                "PredictionLumiFinalExponentialMuHLT");
      Config::config().setFloat(kEmptyBunchgroupExtrapolaion, _emptyExtrap, "EmptyBunchgroupExtrapolation");
      Config::config().set(kTargetPairedBunches, _targetBunches, "TargetPairedBunches");

      // Even more fancy is to split these details per LB
      Float_t _thisLBLumi = _runLumi;
      Float_t _thisLBMu = _onlineMu;
      Float_t _thisLBDeadtime = _onlineDeadtime;
      Int_t _thisLBEmptyBnch = _currentEmptyBunches;
      for (Int_t _lb = m_minLB; _lb <= m_maxLB; ++_lb) {
        // If we miss data - then we take the previous LB's data or the run default
        if (m_lumiPerLB.count(_lb) == 1) _thisLBLumi = m_lumiPerLB[_lb];
        if (m_muPerLB.count(_lb) == 1) _thisLBMu = m_muPerLB[_lb];
        if (m_deadtimePerLB.count(_lb) == 1) _thisLBDeadtime = m_deadtimePerLB[_lb];
        if (m_pairedBunchesPerLB.count(_lb) == 1) _thisLBEmptyBnch = std::max(0, _maxBCIDs - m_pairedBunchesPerLB[_lb]);


        Float_t _emptyExtrapPerLB = 0.;
        if (_thisLBEmptyBnch > 0) _emptyExtrapPerLB = _targetEmptyBunches / (Float_t) _thisLBEmptyBnch;

        Float_t _lumiScalingPerLB = _predictionLumi / _thisLBLumi;

        Float_t _lumiMuScalingPerLB = _targetMu / _thisLBMu;
        Float_t _lumiBunchScalingPerLB = _lumiScalingPerLB / _lumiMuScalingPerLB;

        Float_t _lumiMuScalingExpoL1PerLB = TMath::Exp((_targetMu - _thisLBMu) * _expoRateScaleModifierL1);
        Float_t _lumiMuScalingExpoHLTPerLB = TMath::Exp((_targetMu - _thisLBMu) * _expoRateScaleModifierHLT);

        if (_doExponentialMu == kFALSE) {
          _lumiMuScalingExpoL1PerLB = _lumiMuScalingPerLB;
          _lumiMuScalingExpoHLTPerLB = _lumiMuScalingPerLB;
        }

        Float_t _lumiScalingExpoL1PerLB = _lumiBunchScalingPerLB * _lumiMuScalingExpoL1PerLB;
        Float_t _lumiScalingExpoHLTPerLB = _lumiBunchScalingPerLB * _lumiMuScalingExpoHLTPerLB;

        // Protect inf
        if (isZero(_thisLBLumi)) {
          _lumiScalingPerLB = 0;
          _lumiScalingExpoL1PerLB = 0;
          _lumiScalingExpoHLTPerLB = 0;
          _lumiBunchScalingPerLB = 0;
          _lumiMuScalingPerLB = 0;
        } else {
          _averageL += _thisLBLumi;
          ++_averageLN;
        }

        m_lumiScalingFactorExpoL1[_lb] = _lumiScalingExpoL1PerLB * (1 + _thisLBDeadtime);
        m_lumiScalingFactorExpoHLT[_lb] = _lumiScalingExpoHLTPerLB * (1 + _thisLBDeadtime);
        m_lumiScalingFactorLinear[_lb] = _lumiScalingPerLB * (1 + _thisLBDeadtime);
        m_lumiScalingFactorBunchOnly[_lb] = _lumiBunchScalingPerLB * (1 + _thisLBDeadtime);
        m_lumiScalingFactorMuOnly[_lb] = _lumiMuScalingPerLB * (1 + _thisLBDeadtime);
        m_lumiScalingFactorDeadtimeOnly[_lb] = 1. * (1 + _thisLBDeadtime);
        m_lumiScalingFactorEmpty[_lb] = _emptyExtrapPerLB;
        m_lumiScalingFactorUnity[_lb] = 1.;

        Info("TrigXMLService::getLumiExtrapWeight",
             "  Per-LB scaling factors LB %i: Linear = %.2f | ExpInMuL1 = %.2f | ExpInMuHLT = %.2f | "
             "BunchOnly = %.2f | MuOnly = %.2f | DeadtimeOnly = %.2f | Empty = %.2f |",
             _lb, m_lumiScalingFactorLinear[_lb], m_lumiScalingFactorExpoL1[_lb], m_lumiScalingFactorExpoHLT[_lb],
             m_lumiScalingFactorBunchOnly[_lb], m_lumiScalingFactorMuOnly[_lb], m_lumiScalingFactorDeadtimeOnly[_lb],
             m_lumiScalingFactorEmpty[_lb]);
      }
      if (_averageLN) _averageL /= _averageLN;
    } else {
      // Build map per-LB
      Float_t _thisLBLumi = _runLumi;
      std::stringstream _perLBstr;
      _perLBstr.precision(5);

      for (Int_t _lb = m_minLB; _lb <= m_maxLB; ++_lb) {
        // If we miss data - then we take the previous LB's data or the run default
        if (m_lumiPerLB.count(_lb) == 1) _thisLBLumi = m_lumiPerLB[_lb];
        Float_t _lumiScalingPerLB = _predictionLumi / _thisLBLumi;
        if (isZero(_thisLBLumi)) {
          _lumiScalingPerLB = 0;
        } else {
          _averageL += _thisLBLumi;
          ++_averageLN;
        }
        m_lumiScalingFactorLinear[_lb] = _lumiScalingPerLB * (1 + _onlineDeadtime);
        _averageL += _thisLBLumi;
        ++_averageLN;
        _perLBstr << "LB " << _lb << "=" << m_lumiScalingFactorLinear[_lb] << ", ";
      }
      if (_averageLN) _averageL /= _averageLN;

      // This is the old-style message, only show it now if doing basic extrapolation mode
      Info("TrigXMLService::getLumiExtrapWeight", "Predictions will be scaled on average by %.4f from EB RunLumi %.2e to "
                                                  "PredictionLumi %.2e. Including a %.2f%% correction for online deadtime. PredictionLumi taken from %s.", 
                                                  _scalingFactor, _runLumi, _predictionLumi, _onlineDeadtime * 100., _predFrom.c_str());
      Info("TrigXMLService::getLumiExtrapWeight", "Per-LB, the weights are: %s", _perLBstr.str().c_str());
    }

    // Do we need to update the average lumi of this processing?
    if (!isZero(_averageL)) {
      if (fabs(1. - (_runLumi / _averageL)) > 0.05) { // If these numbers are off by more than 5% (arbitrary threshold)
        Warning("TrigXMLService::getLumiExtrapWeight",
                "Discrepancy in XML EB average run Lumi (%.4e) and calculated (%.4e). Probably due to restricted LB range.",
                _runLumi,
                _averageL);
      }
      Config::config().setFloat(kRunLumiXML, _averageL, "EnhancedBiasOnlineLumi", kLocked);
    }

    Config::config().setFloat(kLumiExtrapWeight, _scalingFactor, "FinalLumiExtrapWeight", kLocked); // Keep a note of
                                                                                                    // this factor
    Config::config().setFloat(kPredictionLumiFinal, _predictionLumi, "PredictionLumiFinal", kLocked);
    Config::config().setFloat(kDeadtimeScalingFinal, (1. + _onlineDeadtime), "DeadtimeScalingFinal", kLocked);
    return _scalingFactor;
  }

  /**
   * Read XML structure, decide on format using (usually) the head node and pass to parser function
   * @param _xmlID This program can take in two XMLs, if the L1 and HLT PS are split over them. This int says which we
   *want to process.
   */
  void TrigXMLService::parseXML(UInt_t _xmlID) {
    assert(_xmlID == 1 || _xmlID == 2);
    std::string _psFilePath;
    std::string _psFileName;
    if (_xmlID == 1) {
      _psFilePath = Config::config().getStr(kPrescaleXMLPath1);
      _psFileName = Config::config().getStr(kPrescaleXMLName1);
    } else if (_xmlID == 2) {
      _psFilePath = Config::config().getStr(kPrescaleXMLPath2);
      _psFileName = Config::config().getStr(kPrescaleXMLName2);
    }

    // Now try to parse xml file
    // Only file with restricted xml syntax are supported
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = _xml->ParseFile(_psFilePath.c_str());
    if (_xmlDoc == 0) {
      Error("TrigXMLService::parseXML",
            "Unable to parse prescale XML %s!!! Fallback PS=%f L1, PS=%fi HLT will be used for rate calculations.",
            _psFilePath.c_str(),
            Config::config().getFloat(kRateFallbackPrescaleL1),
            Config::config().getFloat(kRateFallbackPrescaleHLT));
      delete _xml;
      return;
    }

    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    if (_xml->GetNodeName(_mainNode) == std::string("trigger")) {
      parsePrescaleXML(_xml, _xmlDoc);
    } else if (_xml->GetNodeName(_mainNode) == std::string("HLT_MENU")) {
      parseMenuXML(_xml, _xmlDoc);
    } else if (_xml->GetNodeName(_mainNode) == std::string("LVL1Config")) {
      parseL1MenuXML(_xml, _xmlDoc);
    } else {
      Error("TrigXMLService::parseXML",
            "Supplied a prescale XML with root node '%s'. I do not know how to decode this!!! Fallback PS=%f L1, PS=%fi HLT will be used for rate calculations.",
            _xml->GetNodeName(_mainNode),
            Config::config().getFloat(kRateFallbackPrescaleL1),
            Config::config().getFloat(kRateFallbackPrescaleHLT));
    }

    delete _xml;
  }

  /**
   * Read details from a menu XML file. This allows the user to supply their own prescale set.
   * However this method does not supply L1 prescales.
   */
  void TrigXMLService::parseMenuXML(TXMLEngine* _xml, XMLDocPointer_t _xmlDoc) {
    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);

    const Bool_t ignorePSGreaterThanOne = (Bool_t) Config::config().getInt(kIgnorePSGreaterThanOne);

    assert(_xml->GetNodeName(_mainNode) == std::string("HLT_MENU"));
    XMLNodePointer_t _listNode = _xml->GetChild(_mainNode);
    m_menuName = _xml->GetAttr(_mainNode, "menu_name");
    m_prescalSetName = _xml->GetAttr(_mainNode, "prescale_set_name");
    Int_t _chainsRead = 0;

    while (_listNode != 0) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);

      if (_listName != "CHAIN_LIST") {
        _listNode = _xml->GetNext(_listNode);
        continue;
      }

      XMLNodePointer_t _chainNode = _xml->GetChild(_listNode);
      while (_chainNode != 0) {
        assert(_xml->GetNodeName(_chainNode) == std::string("CHAIN"));
        const std::string _chainName = _xml->GetAttr(_chainNode, "chain_name");
        if (_xml->GetAttr(_chainNode,
                          "chain_counter")) m_chainCounter[_chainName] =
          stringToInt(_xml->GetAttr(_chainNode, "chain_counter"));
        if (_xml->GetAttr(_chainNode, "lower_chain_name")) m_chainLowerLvl[_chainName] = _xml->GetAttr(_chainNode,
                                                                                                       "lower_chain_name");

        if (_xml->GetAttr(_chainNode, "prescale")) m_chainPS[_chainName] = stringToDouble(_xml->GetAttr(_chainNode, "prescale"));
        if (_xml->GetAttr(_chainNode, "pass_through")) m_chainPT[_chainName] = stringToDouble(_xml->GetAttr(_chainNode, "pass_through"));
        if (_xml->GetAttr(_chainNode, "rerun_prescale")) m_chainRerunPS[_chainName] = stringToDouble(_xml->GetAttr(_chainNode, "rerun_prescale"));
        if (_xml->GetAttr(_chainNode, "express_prescale")) {
          m_chainExpressPS[_chainName] = stringToDouble(_xml->GetAttr(_chainNode, "express_prescale"));
          m_hasExpressPrescaleInfo = kTRUE;
        }
        ++_chainsRead;

        if (ignorePSGreaterThanOne && m_chainPS[_chainName] > 1.) m_chainPS[_chainName] = -1;

        if (Config::config().debug()) {
          Info("TrigXMLService::parseMenuXML", "Parsed Chain:%s, Counter:%i, LowerChain:%s, PS:%f, PT:%f, RerunPS:%f, Express:%f",
               _chainName.c_str(),
               m_chainCounter[_chainName],
               m_chainLowerLvl[_chainName].c_str(),
               m_chainPS[_chainName],
               m_chainPT[_chainName],
               m_chainRerunPS[_chainName],
               m_chainExpressPS[_chainName]);
        }
        _chainNode = _xml->GetNext(_chainNode);
      }

      _listNode = _xml->GetNext(_listNode);
    }

    Info("TrigXMLService::parseMenuXML", "Parsed prescale set %s of menu %s. Read %i chains.",
         m_prescalSetName.c_str(),
         m_menuName.c_str(),
         _chainsRead);

    Config::config().set(kPrescaleSetName, m_prescalSetName, "PrescaleSetName", kUnlocked);
    Config::config().set(kMenuName, m_menuName, "MenuName", kUnlocked);

    m_serviceEnabled = kTRUE;
  }

  /**
   * Read details from a L1 menu XML file. This allows the user to supply their own prescale set.
   * We first need to read the name-CTPID mappings, then can read in the PS
   */
  void TrigXMLService::parseL1MenuXML(TXMLEngine* _xml, XMLDocPointer_t _xmlDoc) {
    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);

    const Bool_t ignorePSGreaterThanOne = (Bool_t) Config::config().getInt(kIgnorePSGreaterThanOne);

    assert(_xml->GetNodeName(_mainNode) == std::string("LVL1Config"));
    XMLNodePointer_t _listNode = _xml->GetChild(_mainNode);
    Int_t _chainsRead = 0;

    while (_listNode != 0) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);

      // TriggerMenu contains TriggerItems which hold the CTPID and names
      if (_listName == "TriggerMenu") {
        XMLNodePointer_t _chainNode = _xml->GetChild(_listNode);
        while (_chainNode != 0) {
          assert(_xml->GetNodeName(_chainNode) == std::string("TriggerItem"));
          Int_t _ctpid = stringToInt(_xml->GetAttr(_chainNode, "ctpid"));
          m_CTPIDToL1Name[_ctpid] = _xml->GetAttr(_chainNode, "name");
          if (Config::config().debug()) {
            Info("TrigXMLService::parseL1MenuXML", "Step 1: CTPID %i = %s", _ctpid, m_CTPIDToL1Name[_ctpid].c_str());
          }
          _chainNode = _xml->GetNext(_chainNode);
        }

        // PrescaleSet contains Prescale items which map these CTPIDs to their PS
        // The CTPID, and "n", "m" and "d" parameters are attributes, the prescale is content
      } else if (_listName == "PrescaleSet") {
        XMLNodePointer_t _chainNode = _xml->GetChild(_listNode);
        while (_chainNode != 0) {
          assert(_xml->GetNodeName(_chainNode) == std::string("Prescale"));
          Int_t _ctpid = stringToInt(_xml->GetAttr(_chainNode, "ctpid"));
          const std::string _L1Name = m_CTPIDToL1Name[ _ctpid ];
          Double_t _prescale = stringToDouble(_xml->GetAttr(_chainNode, "value"));
          if (_L1Name == Config::config().getStr(kBlankString) && _prescale >= 0) {
            Error("TrigXMLService::parseL1MenuXML", "XML trying to set PS for CTPID %i, but no chain name was supplied by the file", _ctpid);
            _chainNode = _xml->GetNext(_chainNode);
            continue;
          } else if (_L1Name == Config::config().getStr(kBlankString)) { // Silent continue
            _chainNode = _xml->GetNext(_chainNode);
            continue;
          }
          m_chainCounter[_L1Name] = _ctpid;
          m_chainPS[_L1Name] = _prescale;

          if (ignorePSGreaterThanOne && m_chainPS[_L1Name] > 1.) m_chainPS[_L1Name] = -1;

          ++_chainsRead;
          if (Config::config().debug()) {
            Info("TrigXMLService::parseL1MenuXML", "Step 2: L1 %s = PS %f", _L1Name.c_str(), _prescale);
          }
          _chainNode = _xml->GetNext(_chainNode);
        }
      } else {
        Info("TrigXMLService::parseL1MenuXML", "Ignoring an unknown L1 menu XML element %s", _listName.c_str());
      }

      _listNode = _xml->GetNext(_listNode);
    }

    Info("TrigXMLService::parseL1MenuXML", "Parsed L1 prescale set. Read %i chains.", _chainsRead);
    m_serviceEnabled = kTRUE;
  }

  /**
   * Alternate parser for prescale XML files, including the output of TrigCostPython
   * @param _xml Pointer to XML document
   * @see TrigXMLService::parseMenuXML
   */
  void TrigXMLService::parsePrescaleXML(TXMLEngine* _xml, XMLDocPointer_t _xmlDoc) {
    // Get access to main node
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);

    const Bool_t ignorePSGreaterThanOne = (Bool_t) Config::config().getInt(kIgnorePSGreaterThanOne);

    assert(_xml->GetNodeName(_mainNode) == std::string("trigger"));
    XMLNodePointer_t _listNode = _xml->GetChild(_mainNode);


    while (_listNode != 0) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);

      if (_listName == "PredictionLumi") {
        Float_t _predictionLumi = stringToFloat(_xml->GetNodeContent(_listNode));
        Config::config().setFloat(kPredictionLumiMenuXML, _predictionLumi, "PredictionLumiMenuXML");
      }

      if (_listName != "level") { // Find the "level" item
        _listNode = _xml->GetNext(_listNode);
        continue;
      }

      XMLNodePointer_t _sigNode = _xml->GetChild(_listNode);
      while (_sigNode != 0) {
        if (_xml->GetNodeName(_sigNode) != std::string("signature")) { // Find the "signature" items
          _sigNode = _xml->GetNext(_sigNode);
          continue;
        }

        XMLNodePointer_t _sigDetailsNode = _xml->GetChild(_sigNode);

        std::string _chainName;
        while (_sigDetailsNode != 0) {
          if (_xml->GetNodeContent(_sigDetailsNode) == NULL) {
            _sigDetailsNode = _xml->GetNext(_sigDetailsNode);
            continue;
          }

          const std::string _detail = _xml->GetNodeName(_sigDetailsNode);
          if (_detail == "sig_name") {
            _chainName = _xml->GetNodeContent(_sigDetailsNode);
          } else if (_detail == "sig_counter") {
            m_chainCounter[_chainName] = stringToInt(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "prescale" || _detail == "chain_prescale") { // This is an alternate name
            m_chainPS[_chainName] = stringToDouble(_xml->GetNodeContent(_sigDetailsNode));
            //if ( isZero(m_chainPS[_chainName]) ) m_chainPS[_chainName] = -1.;
          } else if (_detail == "lower_chain_name") {
            // Later processing here does not expect any spaces, so remove them now. Pure comma separated list
            std::string _lower = _xml->GetNodeContent(_sigDetailsNode);
            while (_lower.find(" ") != std::string::npos) {
              _lower.replace(_lower.find(" "), 1, Config::config().getStr(kBlankString));
            }
            m_chainLowerLvl[_chainName] = _lower;
          } else if (_detail == "evts_passed") {
            m_chainEvtPassed[_chainName] = stringToInt(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "evts_passed_weighted") {
            m_chainEvtPassedWeighted[_chainName] = stringToFloat(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "rate") {
            m_chainRate[_chainName] = stringToFloat(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "rate_err") {
            m_chainRateErr[_chainName] = stringToFloat(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "passthrough") {
            m_chainPT[_chainName] = stringToDouble(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "rerun_prescale") {
            m_chainRerunPS[_chainName] = stringToDouble(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "express_prescale") {
            m_chainExpressPS[_chainName] = stringToDouble(_xml->GetNodeContent(_sigDetailsNode));
            m_hasExpressPrescaleInfo = kTRUE;
          } else if (_detail == "efficiency") {
            m_chainEff[_chainName] = stringToFloat(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "efficiency_err") {
            m_chainEffErr[_chainName] = stringToFloat(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "prescaled_efficiency") {
            m_chainPSEff[_chainName] = stringToFloat(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "prescaled_efficiency_error") {
            m_chainPSEffErr[_chainName] = stringToFloat(_xml->GetNodeContent(_sigDetailsNode));
          } else if (_detail == "comment") {
            m_chainComment[_chainName] = _xml->GetNodeContent(_sigDetailsNode);
            m_hasComments = kTRUE;
          }

          _sigDetailsNode = _xml->GetNext(_sigDetailsNode);
        }

        // Sanity check here. If the user is not doing EB weighting, and is not scaling up by the L1 PS, then != 1 PS at
        // L1 make no sense.
        if (Config::config().getInt(kDoEBWeighting) == 0
            && Config::config().getInt(kRatesScaleByPS) == 0
            && _chainName.substr(0, 3) == "L1_"
            && isZero(m_chainPS[_chainName] - 1.) == kFALSE) {
          Error("TrigXMLService::parsePrescaleXML", "You are setting a prescale PS=%f for an L1 item %s, but you are NOT using EnhancedBias data. "
                                                    "Nor are you using the --scaleRatesByPS option to weight up chains by their L1 prescale. "
                                                    "In order to simulate changes to the L1 prescale, you should be using EB weighting. "
                                                    "Otherwise the correlations between L1 items in the CTP "
                                                    "will have been lost.", m_chainPS[_chainName], _chainName.c_str());
        }

        if (ignorePSGreaterThanOne && m_chainPS[_chainName] > 1.) m_chainPS[_chainName] = -1;

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
               _chainName.c_str(),
               m_chainCounter[_chainName],
               m_chainEvtPassed[_chainName],
               m_chainEvtPassedWeighted[_chainName],
               m_chainRate[_chainName],
               m_chainRateErr[_chainName],
               m_chainPS[_chainName],
               m_chainPT[_chainName],
               m_chainRerunPS[_chainName],
               m_chainExpressPS[_chainName],
               m_chainLowerLvl[_chainName].c_str(),
               m_chainEff[_chainName],
               m_chainEffErr[_chainName],
               m_chainPSEff[_chainName],
               m_chainPSEffErr[_chainName]);
        }
        _sigNode = _xml->GetNext(_sigNode);
      }

      _listNode = _xml->GetNext(_listNode);
    }

    Info("TrigXMLService::parsePrescaleXML", "Parsed prescale set from XML. Now know PS values for %i chains.", (Int_t) m_chainPS.size());
    m_serviceEnabled = kTRUE;
  }

  /**
   * Try and parse the run XML if any - this contains EB run specifics
   * @param _runNumber the run number of the XML to load
   * @param _primaryRun if true, this is the primary & first input file's run. We take this run as the main set of
   *parameters
   */
  void TrigXMLService::parseRunXML(const Int_t _runNumber, const Bool_t _primaryRun) {
    const std::string _file = std::string("enhanced_bias_run_") + intToString(_runNumber) + std::string(".xml");

    std::string _path;

    if (Config::config().getInt(kIsRootCore) == kTRUE) {
      _path = std::string(Config::config().getStr(kDataDir) + _file);
    } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
      _path = PathResolverFindDataFile(_file); // Get from CALIB area
      if (_path == Config::config().getStr(kBlankString)) { // One more place we can look
        _path = std::string(Config::config().getStr(kAFSDataDir) + _file);
      }
#endif // not ROOTCORE
    }


    // 2016 high-<mu> run
    // Here we account for the three v-high-mu-bunches: ~6% of the luminosity came from the three isolated fat bunches
    Float_t _mod = 1.;
    if (_runNumber == 310574) {
      if (Config::config().getInt(kInvertHighMuRunVeto) == true) _mod = 0.06;
      else _mod = 0.94;
    }

    // If file exists
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = 0;

    if (_path != Config::config().getStr(kBlankString) && access(_path.c_str(), F_OK) != -1) {
      _xmlDoc = _xml->ParseFile(_path.c_str());
    }

    if (_xmlDoc == 0) {
      Warning("TrigXMLService::parseRunXML", "Cannot find run XML %s, hence will not do advanced partial-run rate scaling.",
              _file.c_str());
      delete _xml;
      return;
    }

    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert(_xml->GetNodeName(_mainNode) == std::string("trigger"));
    XMLNodePointer_t _listNode = _xml->GetChild(_mainNode);

    while (_listNode != 0) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);

      if (_listName == "lb_list") {
        XMLNodePointer_t _node = _xml->GetChild(_listNode);
        while (_node != 0) {
          assert(_xml->GetNodeName(_node) == std::string("lb"));
          Int_t _lb = stringToInt(_xml->GetAttr(_node, "id"));
          UInt_t _nEvents = stringToInt(_xml->GetNodeContent(_node));
          // Is LB to be vetoed? (like a GRL)
          std::string _flag;
          if (_xml->HasAttr(_node, "flag")) {
            _flag = _xml->GetAttr(_node, "flag");
          }
          if (_flag == "bad") m_badLumiBlocks.insert(_lb);
          // Some runs have higher granularity data
          Float_t _lumi = 0.;
          if (_xml->HasAttr(_node, "lumi")) _lumi = stringToFloat(_xml->GetAttr(_node, "lumi"));
          else _lumi = Config::config().getFloat(kRunLumiXML);
          if (_lumi < 1e20) _lumi *= 1e30;
          //
          Float_t _mu = 0.;
          if (_xml->HasAttr(_node, "mu")) _mu = stringToFloat(_xml->GetAttr(_node, "mu"));
          else _mu = Config::config().getFloat(kOnlinePeakMuAverage);
          //
          if (_lb < m_minLB) m_minLB = _lb;
          if (_lb > m_maxLB) m_maxLB = _lb;

          // We can only deal with one LB per processing, a current limitation of MultiRun
          assert(m_totalEventsPerLB.count(_lb) == 0);

          m_totalEventsPerLB[_lb] = _nEvents;
          m_lumiPerLB[_lb] = _lumi;
          m_muPerLB[_lb] = _mu;
          m_deadtimePerLB[_lb] = m_loadedDeadtime;
          m_pairedBunchesPerLB[_lb] = m_loadedPairedBunches;
          _node = _xml->GetNext(_node);
        }
      } else if (_listName == "lumivalues") {
        XMLNodePointer_t _node = _xml->GetChild(_listNode);
        while (_node != 0) {
          if (_xml->GetNodeName(_node) == std::string("lumivalues_data")) {
            if (_primaryRun) Config::config().setFloat(kRunLumiXML, stringToFloat(_xml->GetNodeContent(_node)) * _mod, "EnhancedBiasOnlineLumi", kUnlocked);
          } else if (_xml->GetNodeName(_node) == std::string("lumivalues_pred")) {
            if (_primaryRun) Config::config().setFloat(kPredictionLumiRunXML, stringToFloat(_xml->GetNodeContent(_node)), "PredictionLumiRunXML");
          } else if (_xml->GetNodeName(_node) == std::string("deadtime")) {
            m_loadedDeadtime = stringToFloat(_xml->GetNodeContent(_node));
            if (_primaryRun) Config::config().setFloat(kOnlineDeadtime, m_loadedDeadtime, "OnlineDeadtime");
          } else if (_xml->GetNodeName(_node) == std::string("peak_mu_av")) {
            if (_primaryRun) Config::config().setFloat(kOnlinePeakMuAverage, stringToFloat(_xml->GetNodeContent(_node)) * _mod, "OnlinePeakMuAverage");
          }
          _node = _xml->GetNext(_node);
        }
      } else if (_listName == "bunchgroups") {
        XMLNodePointer_t _node = _xml->GetChild(_listNode);
        while (_node != 0) {
          assert(_xml->GetNodeName(_node) == std::string("bunchgroup"));
          Int_t _id = stringToInt(_xml->GetAttr(_node, "id"));
          std::string _name = _xml->GetAttr(_node, "name");
          Int_t _bunches = stringToInt(_xml->GetNodeContent(_node));
          m_bunchGroupXML[ _id ] = std::pair<std::string, Int_t>(_name, _bunches);
          _node = _xml->GetNext(_node);
        }
        m_loadedPairedBunches = m_bunchGroupXML[1].second;
        if (_runNumber == 310574) {
          if (Config::config().getInt(kInvertHighMuRunVeto) == true) m_loadedPairedBunches = 3;
          else m_loadedPairedBunches -= 3;
        }
        if (Config::config().getIsSet(kPairedBunches) == false) Config::config().set(kPairedBunches, m_loadedPairedBunches, "PairedBunches");
      }

      _listNode = _xml->GetNext(_listNode);
    }

    const Int_t _lbBegin = Config::config().getInt(kLumiStart);
    const Int_t _lbEnd = Config::config().getInt(kLumiEnd);

    // May want to restrict to a greater extent than in the run XML
    if (_lbBegin != INT_MIN && _lbBegin > m_minLB && _lbBegin <= m_maxLB) m_minLB = _lbBegin;
    if (_lbEnd != INT_MAX && _lbEnd < m_maxLB && _lbEnd >= m_minLB) m_maxLB = _lbEnd;

    delete _xml;
    m_parsedRunXML = kTRUE;
  }

  /**
   * @return if a given LB was flagged bad in the Run XML by rates experts. Usually because of detector issues which
   *could affect rates prediction
   */
  Bool_t TrigXMLService::getIsLBFlaggedBad(Int_t _lb) {
    if (m_ignoreGRL) return kFALSE;
    return (Bool_t) m_badLumiBlocks.count(_lb);
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
  Double_t TrigXMLService::getExpressPrescaleInfo(const std::string& _name) {
    if (m_chainExpressPS.count(_name) == 0) return -1.;

    return m_chainExpressPS[_name];
  }

  /**
   * @return  the comment on the chain, if any
   */
  std::string TrigXMLService::getChainComment(const std::string& _name) {
    if (m_chainComment.count(_name) == 0) return "-";

    return m_chainComment[_name];
  }

  /**
   * @return the name of a bunch group loaded from the run XML
   */
  std::string TrigXMLService::getBunchGroupName(Int_t _id) {
    return m_bunchGroupXML[ _id ].first;
  }

  /**
   * @return the size of a bunch group loaded from the run XML
   */
  Int_t TrigXMLService::getBunchGroupSize(Int_t _id) {
    return m_bunchGroupXML[ _id ].second;
  }

  /**
   * @return the total number of events to expect for a given LB, lets us know if we saw all the data
   */
  Int_t TrigXMLService::getOnlineEventsInLB(Int_t _lb) {
    return m_totalEventsPerLB[ _lb ];
  }

  /**
   * Load which PCs have which CPUs.
   * This used to be based on an XML but now is not, hence the naming 
   */
  void TrigXMLService::parseHLTFarmXML() {

    // Now build a map of PU hash to computer type
    const std::string _hltLevel = "HLT";

    for (Int_t _rack = 94; _rack <= 95; ++_rack) {
      for (Int_t _pc = 1; _pc <= 64; ++_pc) {
        for (Int_t _pu = 1; _pu <= 12; ++_pu) {
          std::stringstream _ss;
          std::string _s;
          _ss << std::setfill('0')
              << "APP_" << _hltLevel
              << ":HLTMPPU-12:HLT-12-NoTS:tpu-rack-"
              << std::setw(2) << _rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << _rack
              << std::setw(3) << _pc
              << "-"
              << std::setw(2) << _pu;
          _s = _ss.str();
          m_PUHashToPUType[stringToIntHash(_s)] = _rack;
          if (Config::config().debug()) Info("TrigXMLService::parseHLTFarmXML", "%s = %i = %i", _s.c_str(), stringToIntHash(_s), _rack);
        }
      }
    }

    for (Int_t _rack = 1; _rack <= 70; ++_rack) {
      if (_rack > 13 && _rack < 64) continue;
      for (Int_t _pc = 1; _pc <= 64; ++_pc) {
        for (Int_t _pu = 1; _pu <= 12; ++_pu) {
          std::stringstream _ss;
          std::string _s;
          _ss << std::setfill('0')
              << "APP_" << _hltLevel
              << ":HLTMPPU-12:HLT-12:tpu-rack-"
              << std::setw(2) << _rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << _rack
              << std::setw(3) << _pc
              << "-"
              << std::setw(2) << _pu;
          _s = _ss.str();
          m_PUHashToPUType[stringToIntHash(_s)] = _rack;
        }
      }
    }

    for (Int_t _rack = 1; _rack <= 70; ++_rack) {
      if (_rack > 13 && _rack < 64) continue;
      for (Int_t _pc = 1; _pc <= 64; ++_pc) {
        for (Int_t _pu = 1; _pu <= 12; ++_pu) {
          std::stringstream _ss;
          std::string _s;
          _ss << std::setfill('0')
              << "APP_" << _hltLevel
              << ":HLTMPPU-12:HLT-allracks:tpu-rack-"
              << std::setw(2) << _rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << _rack
              << std::setw(3) << _pc
              << "-"
              << std::setw(2) << _pu;
          _s = _ss.str();
          m_PUHashToPUType[stringToIntHash(_s)] = _rack;
        }
      }
    }

    for (Int_t _rack = 77; _rack <= 77; ++_rack) {
      for (Int_t _pc = 1; _pc <= 64; ++_pc) {
        for (Int_t _pu = 1; _pu <= 24; ++_pu) {
          std::stringstream _ss;
          std::string _s;
          _ss << std::setfill('0')
              << "APP_" << _hltLevel
              << ":HLTMPPU-24:HLT-24-Merge:tpu-rack-"
              << std::setw(2) << _rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << _rack
              << std::setw(3) << _pc
              << "-"
              << std::setw(2) << _pu;
          _s = _ss.str();
          m_PUHashToPUType[stringToIntHash(_s)] = _rack;
        }
      }
    }

    for (Int_t _rack = 16; _rack <= 90; ++_rack) {
      if (_rack > 24 && _rack < 70) continue;
      for (Int_t _pc = 1; _pc <= 64; ++_pc) {
        for (Int_t _pu = 1; _pu <= 24; ++_pu) {
          std::stringstream _ss;
          std::string _s;
          _ss << std::setfill('0')
              << "APP_" << _hltLevel
              << ":HLTMPPU-24:HLT-24:tpu-rack-"
              << std::setw(2) << _rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << _rack
              << std::setw(3) << _pc
              << "-"
              << std::setw(2) << _pu;
          _s = _ss.str();
          m_PUHashToPUType[stringToIntHash(_s)] = _rack;
        }
      }
    }

    for (Int_t _rack = 25; _rack <= 25; ++_rack) {
      for (Int_t _pc = 1; _pc <= 64; ++_pc) {
        for (Int_t _pu = 1; _pu <= 36; ++_pu) {
          std::stringstream _ss;
          std::string _s;
          _ss << std::setfill('0')
              << "APP_" << _hltLevel
              << ":HLTMPPU-36:HLT-24-HT:tpu-rack-"
              << std::setw(2) << _rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << _rack
              << std::setw(3) << _pc
              << "-"
              << std::setw(2) << _pu;
          _s = _ss.str();
          m_PUHashToPUType[stringToIntHash(_s)] = _rack;
        }
      }
    }

    for (Int_t _rack = 26; _rack <= 26; ++_rack) {
      for (Int_t _pc = 1; _pc <= 64; ++_pc) {
        for (Int_t _pu = 1; _pu <= 48; ++_pu) {
          std::stringstream _ss;
          std::string _s;
          _ss << std::setfill('0')
              << "APP_" << _hltLevel
              << ":HLTMPPU-48:HLT-24-HTPlus:tpu-rack-"
              << std::setw(2) << _rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << _rack
              << std::setw(3) << _pc
              << "-"
              << std::setw(2) << _pu;
          _s = _ss.str();
          m_PUHashToPUType[stringToIntHash(_s)] = _rack;
        }
      }
    }

    for (Int_t _rack = 44; _rack <= 44; ++_rack) {
      for (Int_t _pc = 18; _pc <= 20; ++_pc) {
        for (Int_t _pu = 1; _pu <= 28; ++_pu) {
          std::stringstream _ss;
          std::string _s;
          _ss << std::setfill('0')
              << "APP_" << _hltLevel
              << ":HLTMPPU-24:HLT-24-IT:tpu-rack"
              << std::setw(2) << _rack
              << ":pc-tdq-tpu-"
              << std::setw(2) << _rack
              << std::setw(3) << _pc
              << "-"
              << std::setw(2) << _pu;
          _s = _ss.str();
          m_PUHashToPUType[stringToIntHash(_s)] = _rack;
        }
      }
    }

    return;
  }

  /**
   * @reutrn The rack number the current PU is located in. Or 0 for unknown
   */
  UInt_t TrigXMLService::getComputerType(UInt_t _hash) {
    UIntUIntMapIt_t _it = m_PUHashToPUType.find(_hash);

    if (_it == m_PUHashToPUType.end()) return m_computerUnknownID;

    return (*_it).second;
  }

  /**
   * Function to load into memory the enhanced bias weighting factors for a run.
   * These are calculated in advance for each EB run and stored in XML format with one entry for each event.
   * They are loaded into memory in a map which is used to fetch the weight.
   * @param _runNumber Which run's XML to load into memory
   */
  void TrigXMLService::parseEnhancedBiasXML(Int_t _runNumber) {
    TXMLEngine* _xml = new TXMLEngine();

    static Int_t _runLoadNumber = -1;

    ++_runLoadNumber;
    Info("TrigXMLService::parseEnhancedBiasXML", "Loading Enhanced Bias Weighting XML for %i. This could take a little time. (Run #%i)", _runNumber, _runLoadNumber);

    // Try one. Use the external (AFS) data path
    Config::config().set(kEBXMLName, "/EnhancedBiasWeights_" + intToString(_runNumber) + ".xml", "EBXMLName", kUnlocked);
    Config::config().set(kEBXMLPath, Config::config().getStr(kAFSDataDir) + "/" + Config::config().getStr(kEBXMLName), "EBXMLPath", kUnlocked);

    XMLDocPointer_t _xmlDoc = _xml->ParseFile(Config::config().getStr(kEBXMLPath).c_str());
    // Could we load from AFS?

    if (_xmlDoc == 0) {
      // No - then try loading locally
      if (Config::config().getInt(kIsRootCore) == kTRUE) {
        Config::config().set(kEBXMLPath, Config::config().getStr(kDataDir) + "/" + Config::config().getStr(kEBXMLName), "EBXMLPath", kUnlocked);
      } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
        std::string _locAthena = PathResolverFindDataFile(Config::config().getStr(kEBXMLName));
        if (_locAthena == Config::config().getStr(kBlankString)) {
          Error("TrigXMLService::parseEnhancedBiasXML", "Athena cannot find Enhanced Bias weighting file %s. Critical error.", Config::config().getStr(kEBXMLName).c_str());
          abort();
        } else {
          Config::config().set(kEBXMLName, _locAthena);
          Info("TrigXMLService::parseEnhancedBiasXML", "Athena has found the file: %s", Config::config().getStr(kEBXMLName).c_str());
        }
#endif // not ROOTCORE
      }

      // Load the file - try 2
      _xmlDoc = _xml->ParseFile(Config::config().getStr(kEBXMLPath).c_str());
      if (_xmlDoc == 0) {
        Error("TrigXMLService::parseEnhancedBiasXML", "Unable to load EnhancedBias weighting factors from %s. Critical error.", Config::config().getStr(kEBXMLName).c_str());
        abort();
      }
    }


    // Navigate XML
    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert(_xml->GetNodeName(_mainNode) == std::string("run"));
    XMLNodePointer_t _weightsNode = _xml->GetChild(_mainNode);
    XMLNodePointer_t _eventsNode = _xml->GetNext(_weightsNode);

    XMLNodePointer_t _weightNode = _xml->GetChild(_weightsNode);
    XMLNodePointer_t _eventNode = _xml->GetChild(_eventsNode);

    while (_weightNode != 0) { // Loop over all menu elements
      assert(_xml->GetNodeName(_weightNode) == std::string("weight")); //Event

      UInt_t _id = stringToInt(_xml->GetAttr(_weightNode, "id")); //Number

      // In MultiRun, we need to apply an offset here
      _id += _runLoadNumber * 1000;

      Float_t _weight = stringToFloat(_xml->GetAttr(_weightNode, "value")); //Weight
      std::string _bunchgroup = _xml->GetAttr(_weightNode, "bunchgroup"); //BG
      Bool_t _unbiased = kFALSE;
      if (_runNumber > 266000) _unbiased = stringToInt(_xml->GetAttr(_weightNode, "unbiased"));

      UInt_t _bunchgroupID = (UInt_t) kBG_UNSET;
      for (UInt_t _i = 0; _i < kEBBunchGroupType_SIZE; ++_i) {
        if (_bunchgroup == BunchGroupNameStr[_i]) {
          _bunchgroupID = _i;
          break;
        }
      }

      if (_bunchgroupID == (UInt_t) kBG_UNSET) {
        Error("TrigXMLService::parseEnhancedBiasXML", "I don't know about this bunchgroup: %s", _bunchgroup.c_str());
      }

      m_idToWeightMap[_id] = _weight;
      m_idToBGMap[_id] = _bunchgroupID;
      m_idToUnbiased[_id] = _unbiased;

      if (Config::config().debug()) {
        Info("TrigXMLService::parseEnhancedBiasXML", "Enhanced bias weight %.4f has ID %i and bunchgroup %s (%i)",
             _weight, _id, _bunchgroup.c_str(), _bunchgroupID);
      }

      _weightNode = _xml->GetNext(_weightNode);
    }

    while (_eventNode != 0) { // Loop over all menu elements
      assert(_xml->GetNodeName(_eventNode) == std::string("e")); //Event
      UInt_t _eventNumber = stringToInt(_xml->GetAttr(_eventNode, "n")); //Number
      UInt_t _eventWeightID = stringToInt(_xml->GetAttr(_eventNode, "w")); //Weight ID

      // In MultiRun, we need to apply an offset here
      _eventWeightID += _runLoadNumber * 1000;

      assert(m_ebWeightingMap.count(_eventNumber) == 0); // Event numbers are unique
      m_ebWeightingMap[_eventNumber] = _eventWeightID;
      _eventNode = _xml->GetNext(_eventNode);
    }

    Info("TrigXMLService::parseEnhancedBiasXML", "Total of %i event weights loaded. Processed for run %i.", (Int_t) m_ebWeightingMap.size(), _runNumber);
    m_weightsServiceEnabled = kTRUE;
    return;
  }

  /**
   * Read (if not already imported) the XML rates and bunch groups for this run.
   * Return the weight for this event and store the event's  bunch group setting using the config service.
   * @param _pass Which pass through the file(s), only increment counters on first pass
   * @param _bcid Current event BCID
   * @return The event weight from the EnhancedBias XML.
   */
  Float_t TrigXMLService::getEventWeight(UInt_t _eventNumber, UInt_t _lb, UInt_t _pass) {
    static const Int_t _runNum = Config::config().getInt(kRunNumber);
    static const std::string _bgString = "BunchGroup";
    static const std::string _ebString = "EventEBWeight";
    static const std::string _rdString = "RandomOnline";
    static const Bool_t _ignoreNonPhys = Config::config().getInt(kIgnoreNonPhysBunchGroups);

    // Special case for pb-p
    if (_runNum == 218048) {
      Float_t _eventWeight = 0;

      if (_lb >= 1016) _eventWeight = 7650.;
      else if (_lb >= 874) _eventWeight = 9560.;
      else if (_lb >= 753) _eventWeight = 11500.;
      else if (_lb >= 620) _eventWeight = 15300.;
      else if (_lb >= 560) _eventWeight = 19100.;
      else if (_lb >= 558) _eventWeight = 28700.;
      else if (_lb >= 557) _eventWeight = 19100.;
      else if (_lb >= 529) _eventWeight = 28700.;

      Config::config().set(kCurrentEventBunchGroupID, 1, _bgString, kUnlocked); // only filled
      Config::config().setFloat(kCurrentEventEBWeight, _eventWeight, _ebString, kUnlocked);
      Config::config().set(kCurrentEventWasRandomOnline, 1, _rdString, kUnlocked); // As "minbias"
      return _eventWeight;
    }


    // if (Config::config().getInt(kDoEBWeighting) == kFALSE) return 1.;
    if (m_weightsServiceEnabled == kFALSE) {
      parseEnhancedBiasXML(Config::config().getInt(kRunNumber));
      std::vector<Int_t> _multiRun = Config::config().getIntVec(kMultiRun);
      for (Int_t _mr : _multiRun) parseEnhancedBiasXML(_mr);
    }

    IntIntMapIt_t _ebIt = m_ebWeightingMap.find(_eventNumber);
    Int_t _weightID = 0;
    if (_ebIt == m_ebWeightingMap.end()) {
      if (Config::config().getDisplayMsg(kMsgXMLWeight) == kTRUE) {
        Warning("TrigXMLService::getEventWeight", "Cannot find the weight for event %i in LB %i, will use weight = 0. !!", _eventNumber, _lb);
      }
      Config::config().set(kCurrentEventBunchGroupID, (Int_t) kBG_UNSET, _bgString, kUnlocked);
      return 0.;
    } else {
      _weightID = _ebIt->second;
    }

    Float_t _eventWeight = m_idToWeightMap[_weightID];
    UInt_t _eventBG = m_idToBGMap[_weightID];

    // Option to disable (weight = 0) events if they're not physics collisions
    if (_ignoreNonPhys == 1) {
      if (_eventBG != 1) _eventWeight = 0.;
    }

    // Store the event BG
    Config::config().set(kCurrentEventBunchGroupID, _eventBG, _bgString, kUnlocked);
    Config::config().setFloat(kCurrentEventEBWeight, _eventWeight, _ebString, kUnlocked);
    if (_pass == 1) m_eventsPerBGCounter[_eventBG] += 1;
    // Store if the event was unbiased online (triggered by random).
    // In run 2 wedo something more fancy
    Int_t _isRandom = 0;
    if (_runNum > 266000) {
      _isRandom = m_idToUnbiased[_weightID];
    } else {
      if (_eventWeight > Config::config().getFloat(kUnbiasedWeightThreshold)) _isRandom = 1;
    }
    if (_isRandom && _pass == 1) m_unbiasedPerBGCounter[_eventBG] += 1;
    Config::config().set(kCurrentEventWasRandomOnline, _isRandom, _rdString, kUnlocked);

    return _eventWeight;
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
   * @param _eventNumber The current event number
   * @param _weight The enhanced bias (un)weighting value for this event, based on the online prescale set and L1 pass
   *before prescale bits.
   */
  void TrigXMLService::exportEnhancedBiasXML(UInt_t _eventNumber, Float_t _weight, UInt_t _bunchGroup, Int_t _unbiased) {
    static std::pair<std::string, Float_t> _recyclablePair;

    _recyclablePair.first = BunchGroupNameStr[_bunchGroup];
    _recyclablePair.second = _weight;

    m_ebWeightTypesCount[_recyclablePair] += 1;

    Int_t _location = -1;
    std::vector< std::pair<std::string, Float_t> >::iterator _weightTypeIt = std::find(m_ebWeightTypes.begin(), m_ebWeightTypes.end(), _recyclablePair);
    if (_weightTypeIt == m_ebWeightTypes.end()) {
      //Add me
      m_ebWeightTypes.push_back(_recyclablePair);
      m_ebWeightUnbiased[_weight] = _unbiased;
      _location = m_ebWeightTypes.size() - 1;
    } else {
      _location = _weightTypeIt - m_ebWeightTypes.begin();
    }
    m_ebWeightingMapSave[_eventNumber] = _location;
  }

  /**
   * Produce the actual XML output
   * @see TrigXMLService::exportEnhancedBiasXML(UInt_t _eventNumber, Float_t _weight)
   */
  void TrigXMLService::saveExportedEnhancedBiasXML() {
    std::ofstream _ebOut;
    const std::string _outputEBWeight = "EnhancedBiasWeights_"
                                        + intToString(Config::config().getInt(kRunNumber))
                                        + ".xml";
    _ebOut.open(_outputEBWeight.c_str());
    XMLExport _ebXML(_ebOut);
    _ebXML.setSpaces(0);
    _ebXML.addNode(_ebOut, "run");
    _ebXML.addNode(_ebOut, "weights");
    for (UInt_t _i = 0; _i < m_ebWeightTypes.size(); ++_i) {
      _ebXML.addNodeWProperty(_ebOut, "weight",
                              "id", intToString(_i),
                              "value", floatToString(m_ebWeightTypes.at(_i).second, 7),
                              "bunchgroup", m_ebWeightTypes.at(_i).first,
                              "unbiased", intToString(m_ebWeightUnbiased[ m_ebWeightTypes.at(_i).second ]));
      Info("TrigXMLService::saveExportedEnhancedBiasXML", "Weight %f of BunchGroup %s has ID %i, Unbiased %i", m_ebWeightTypes.at(_i).second, m_ebWeightTypes.at(_i).first.c_str(), _i, m_ebWeightUnbiased[ m_ebWeightTypes.at(_i).second ]);
    }
    _ebXML.endNode(_ebOut);//weights
    _ebXML.addNode(_ebOut, "events");
    for (IntIntMapIt_t _it = m_ebWeightingMapSave.begin(); _it != m_ebWeightingMapSave.end(); ++_it) {
      _ebXML.addNodeWProperty(_ebOut, "e", "n", intToString(_it->first), "w", intToString(_it->second));
    }
    _ebXML.endNode(_ebOut);//events
    _ebXML.endNode(_ebOut);//run
    _ebOut.close();
    for (PairStringFloat_Float_MapIt_t _it = m_ebWeightTypesCount.begin(); _it != m_ebWeightTypesCount.end(); ++_it) {
      Info("TrigXMLService::saveExportedEnhancedBiasXML", "%i events with enhanced bias weight %f and BG %s", _it->second, _it->first.second, _it->first.first.c_str());
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
    const std::string _xmlName = Config::config().getStr(kOutputRootDirectory) + "/" + "Prescales.xml";

    std::ofstream _fout;
    _fout.open(_xmlName.c_str());
    _fout << std::fixed; // Use fixed width output

    Info("TrigXMLService::writePrescaleXML", "Writing a dummy file Prescales.xml with current menu.");

    XMLExport _xml(_fout);
    _xml.setSpaces(2);
    _xml.addNode(_fout, "trigger");

    // Loop over all chains in metadata trigger config folder
    Int_t _level = -1;
    for (UInt_t _c = 0; _c < TrigConfInterface::getChainN(); ++_c) {
      if (TrigConfInterface::getChainLevel(_c) == 1 && _level == -1) {
        _level = 1;
        _xml.addNode(_fout, "level");
        _xml.addNode(_fout, "lvl_name", "L1");
      } else if (TrigConfInterface::getChainLevel(_c) == 2 && _level == 1) {
        //switch over to HLT
        _level = 2;
        _xml.endNode(_fout); //level
        _xml.addNode(_fout, "level");
        _xml.addNode(_fout, "lvl_name", "HLT");
      }

      _xml.addNode(_fout, "signature");
      std::string _name = TrigConfInterface::getChainName(_c);
      _xml.addNode(_fout, "sig_name", _name);
      _xml.addNode(_fout, "chain_prescale", floatToString(TrigConfInterface::getPrescale(_name)));
      _xml.endNode(_fout); //signature
    }
    _xml.endNode(_fout); //level
    _xml.endNode(_fout); //Trigger
    _fout.close();

    Info("TrigXMLService::writePrescaleXML", "Exported XML with %i chains.", TrigConfInterface::getChainN());
  }

  void TrigXMLService::saveRuleBookXML(CounterCollection_t& _counterCollections, std::string _levelStr) {
    if (Config::config().getInt(kOutputXML) == kFALSE) return;

    // Save tables. Loop over counter collections.
    CounterCollectionIt_t _colIt = _counterCollections.begin();
    for (; _colIt != _counterCollections.end(); ++_colIt) {
      const std::string _counterCollectionName = _colIt->first;
      const CounterMap_t* _counterMap = &(_colIt->second);
      // Skip if there are no counters to process
      if (_counterMap->size() == 0) continue;

      const std::string _outputFolder = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(kOutputXMLDirectory);
      gSystem->mkdir(_outputFolder.c_str(), kTRUE);

      const std::string _xmlName = _outputFolder
                                   + "/TrigRate_"
                                   + Config::config().getStr(kOutputTag) + "_"
                                   + _levelStr + "_"
                                   + _counterCollectionName
                                   + ".xml";
      std::ofstream _fout;
      _fout.open(_xmlName.c_str());
      _fout << std::fixed; // Use fixed width output

      if (Config::config().debug()) {
        Info("TrigXMLService::saveRuleBookXML", "Doing XML output with path %s.", _xmlName.c_str());
      }

      XMLExport _xml(_fout);
      _xml.setSpaces(2);
      _xml.addNode(_fout, "trigger");

      // Do General Info
      _xml.addNode(_fout, "Xsection", intToString(0)); // TODO

      Float_t _runLumi = 0.;
      if (Config::config().getIsSet(kRunLumi)) _runLumi = Config::config().getFloat(kRunLumi);
      else if (Config::config().getIsSet(kRunLumiXML)) _runLumi = Config::config().getFloat(kRunLumiXML);
      _xml.addNode(_fout, "Luminosity", floatToString(_runLumi));

      _xml.addNode(_fout, "GenEff", intToString(0)); // TODO
      _xml.addNode(_fout, "n_evts", intToString(Config::config().getLong(kEventsProcessed)));

      Float_t _predictionLumi = 0.;
      if (Config::config().getIsSet(kPredictionLumi)) _predictionLumi = Config::config().getFloat(kPredictionLumi);
      else if (Config::config().getIsSet(kPredictionLumiMenuXML)) _predictionLumi = Config::config().getFloat(kPredictionLumiMenuXML);
      else if (Config::config().getIsSet(kPredictionLumiRunXML)) _predictionLumi = Config::config().getFloat(kPredictionLumiRunXML);
      _xml.addNode(_fout, "PredictionLumi", floatToString(_predictionLumi));

      for (UInt_t _f = 0; _f < Config::config().getVecSize(kInputFiles); ++_f) {
        _xml.addNode(_fout, "Dataset", Config::config().getVecEntry(kInputFiles, _f));
      }
      _xml.addNode(_fout, "AtlasProject", TrigConfInterface::getMetaStringVal("AtlasProject"));
      _xml.addNode(_fout, "AtlasVersion", TrigConfInterface::getMetaStringVal("AtlasVersion"));
      _xml.addNode(_fout, "triggerMenuSetup", TrigConfInterface::getMetaStringVal("triggerMenuSetup"));
      _xml.addNode(_fout, "L1PrescaleSet", TrigConfInterface::getMetaStringVal("L1PrescaleSet"));
      _xml.addNode(_fout, "HLTPrescaleSet", TrigConfInterface::getMetaStringVal("HLTPrescaleSet"));
      _xml.addNode(_fout, "CMTPATH", TrigConfInterface::getMetaStringVal("CMTPATH"));

      // Do Bunch Group info
      // Currently we prefer XML
      if (TrigXMLService::trigXMLService().getParsedRunXML() == kTRUE) {
        _xml.addNode(_fout, "bunchgroup");
        for (Int_t _bg = 0; _bg < TrigXMLService::trigXMLService().getNBunchGroups(); ++_bg) {
          _xml.addNode(_fout, "bunchgrouptype");
          _xml.addNode(_fout, "bunchgroup_keynum", intToString(_bg));
          _xml.addNode(_fout, "bunchgroup_key", TrigXMLService::trigXMLService().getBunchGroupName(_bg));
          _xml.addNode(_fout, "bunchgroup_size", intToString(TrigXMLService::trigXMLService().getBunchGroupSize(_bg)));
          _xml.endNode(_fout); // bunchgrouptype
        }
        _xml.endNode(_fout); //bunchgroup
      } else {
        // Otherwise try from ntuple (this is broken 06/15) TODO fix in athena
        StringIntMap_t _bunchGroups = TrigConfInterface::getBunchGroupSetup();
        UInt_t _bgCounter = 0;
        _xml.addNode(_fout, "bunchgroup");
        for (StringIntMapIt_t _it = _bunchGroups.begin(); _it != _bunchGroups.end(); ++_it) {
          _xml.addNode(_fout, "bunchgrouptype");
          _xml.addNode(_fout, "bunchgroup_keynum", intToString(_bgCounter++));
          _xml.addNode(_fout, "bunchgroup_key", _it->first);
          _xml.addNode(_fout, "bunchgroup_size", intToString(_it->second));
          _xml.endNode(_fout); // bunchgrouptype
        }
        _xml.endNode(_fout); //bunchgroup
      }

      Bool_t _hasL1 = kFALSE, _hasL2 = kFALSE, _hasHLT = kFALSE;
      for (CounterMapIt_t _counterMapIt = _colIt->second.begin(); _counterMapIt != _colIt->second.end(); ++_counterMapIt) {
        CounterBaseRates* _counter = static_cast<CounterBaseRates*>(_counterMapIt->second);
        if (_counter->getStrDecoration(kDecType) == "L1") _hasL1 = kTRUE;
        else if (_counter->getStrDecoration(kDecType) == "L2") _hasL2 = kTRUE;
        else if (_counter->getStrDecoration(kDecType) == "HLT" || _counter->getStrDecoration(kDecType) == "Chain") _hasHLT = kTRUE;
      }

      if (_hasL1) { //Add L1 data
        _xml.addNode(_fout, "level");
        _xml.addNode(_fout, "lvl_name", "L1");
        for (CounterMapIt_t _counterMapIt = _colIt->second.begin(); _counterMapIt != _colIt->second.end(); ++_counterMapIt) {
          CounterBaseRates* _counter = static_cast<CounterBaseRates*>(_counterMapIt->second);
          if (_counter->getStrDecoration(kDecType) != "L1") continue;
          saveXMLElement(_fout, _xml, _counter);
        }
        _xml.endNode(_fout); //level
      }
      if (_hasL2) { //Add L2 data
        _xml.addNode(_fout, "level");
        _xml.addNode(_fout, "lvl_name", "L2");
        for (CounterMapIt_t _counterMapIt = _colIt->second.begin(); _counterMapIt != _colIt->second.end(); ++_counterMapIt) {
          CounterBaseRates* _counter = static_cast<CounterBaseRates*>(_counterMapIt->second);
          if (_counter->getStrDecoration(kDecType) != "L2") continue;
          saveXMLElement(_fout, _xml, _counter);
        }
        _xml.endNode(_fout); //level
      }
      if (_hasHLT) { //Add HLT data
        _xml.addNode(_fout, "level");
        _xml.addNode(_fout, "lvl_name", "HLT");
        for (CounterMapIt_t _counterMapIt = _colIt->second.begin(); _counterMapIt != _colIt->second.end(); ++_counterMapIt) {
          CounterBaseRates* _counter = static_cast<CounterBaseRates*>(_counterMapIt->second);
          if (_counter->getStrDecoration(kDecType) != "Chain") continue;
          saveXMLElement(_fout, _xml, _counter);
        }
        _xml.endNode(_fout); //level
      }

      _xml.endNode(_fout); //trigger

      _fout.close();
    }
  }

  /**
   * Save all rate data for a single chain into an output XML stream
   * @param _fout Reference to output stream.
   * @param _xml Reference to XML export engine.
   * @returns _counter Counter to export.
   */
  void TrigXMLService::saveXMLElement(std::ofstream& _fout, XMLExport& _xml, CounterBaseRates* _counter) {
    const UInt_t _xmlPrecision = 7;

    Float_t _evPassWeight = _counter->getValue(kVarEventsPassed, kSavePerCall);
    Float_t _evPassWeightErr = _counter->getValueError(kVarEventsPassed, kSavePerCall); // Get sqrt(sumW2)

    Float_t _evRun = _counter->getValue(kVarEventsRun, kSavePerCall); // EB Weighted
    Float_t _evPassNoPS = _counter->getValue(kVarEventsPassedNoPS, kSavePerCall); // EB Weighted

    Float_t _evPassRawStat = _counter->getValue(kVarEventsPassRawStat, kSavePerCall); // Not EB Weighted
    //Float_t _evRunRawStat  = _counter->getValue(kVarEventsRunRawStat,  kSavePerCall); // Not EB Weighted

    Float_t _walltime = _counter->getDecoration(kDecLbLength);

    Float_t _rate = _evPassWeight / _walltime;
    Float_t _rateErr = _evPassWeightErr / _walltime;    // err = sqrt(events in time T)/T = sqrt(rate*T/T^2) =
                                                        // sqrt(rate/T)

    Float_t _uniqueRate = _counter->getDecoration(kDecUniqueRate);
    Float_t _uniqueFraction = _counter->getDecoration(kDecUniqueFraction); // Was in percent, change back to 0-1

    Bool_t _isL1 = kFALSE;

    if (_counter->getStrDecoration(kDecType) == "L1") _isL1 = kTRUE;
    UNUSED(_isL1);

    // TODO - THIS IS WRONG FOR WEIGHTED EVENTS
    Float_t _eff = 0., _effErr = 0., _psEff = 0., _psEffErr = 0.;
    if (_evRun) {
      _eff = _evPassNoPS / _evRun;
      _effErr = (1. / _evRun) * TMath::Sqrt(_evPassNoPS * (1. - _eff)); // Binomal
      _psEff = _evPassWeight / _evRun;
      _psEffErr = (1. / _evRun) * TMath::Sqrt(_evPassWeight * (1. - _psEff)); // Binomal
    }

    UNUSED(_effErr);
    UNUSED(_psEffErr);

    _xml.addNode(_fout, "signature");
    _xml.addNode(_fout, "sig_name", _counter->getName());
    _xml.addNode(_fout, "express_stream", intToString(0)); // TODO add me!
    _xml.addNode(_fout, "chain_prescale", floatToString(_counter->getBasicPrescale())); // This holds the *item* PS
    _xml.addNode(_fout, "passthrough", "0");
    _xml.addNode(_fout, "lower_chain_name", "");
    _xml.addNode(_fout, "evts_passed", floatToString(_evPassRawStat, _xmlPrecision));
    _xml.addNode(_fout, "evts_passed_weighted", floatToString(_evPassWeight, _xmlPrecision));
    _xml.addNode(_fout, "efficiency", floatToString(_eff, _xmlPrecision));
    //_xml.addNode(_fout, "efficiency_err", floatToString( _effErr, _xmlPrecision)); // TODO FIX ME
    _xml.addNode(_fout, "efficiency_err", floatToString((Float_t) 0.));
    _xml.addNode(_fout, "prescaled_efficiency", floatToString(_psEff, _xmlPrecision));
    //_xml.addNode(_fout, "prescaled_efficiency_err", floatToString( _psEffErr, _xmlPrecision)); // TODO FIX ME
    _xml.addNode(_fout, "prescaled_efficiency_err", floatToString((Float_t) 0.));
    _xml.addNode(_fout, "rate", floatToString(_rate, _xmlPrecision));
    _xml.addNode(_fout, "rate_err", floatToString(_rateErr, _xmlPrecision));
    _xml.addNode(_fout, "unique_fraction", floatToString(_uniqueFraction, _xmlPrecision));
    _xml.addNode(_fout, "unique_rate", floatToString(_uniqueRate, _xmlPrecision));
    _xml.endNode(_fout); //signature
  }

  /**
   * We need extra information which is kept in upgrade XMLs. These share similarity with standard rate XMLs but have
   *additional fields
   * @param _isoBits Pass by reference to map of isolation ID string to isolation bits map, to read from XML
   * @param _upgradeChains Pass by reference to set of ChainInfo objects containing all info needed to create chain
   *counters, to read from XML
   */
  void TrigXMLService::parseUpgradeXML(StringIntMap_t& _isoBits, std::multiset<ChainInfo>& _upgradeChains) {
    const std::string _scenario = Config::config().getStr(kUpgradeScenario);
    const std::string _file = std::string("upgrade_") + _scenario + std::string("_menu.xml");

    std::string _path;

    if (Config::config().getInt(kIsRootCore) == kTRUE) {
      _path = std::string(Config::config().getStr(kDataDir) + _file);
    } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
      _path = PathResolverFindDataFile(_file);
#endif // not ROOTCORE
    }

    // If file exists
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = 0;
    std::string _level = "none";

    if (_path != Config::config().getStr(kBlankString) && access(_path.c_str(), F_OK) != -1) {
      _xmlDoc = _xml->ParseFile(_path.c_str());
    }

    if (_xmlDoc == 0) {
      Error("MonitorRatesUpgrade::parseUpgradeXML", "Cannot find upgrade XML %s, critical error - must be supplied. Use --upgradeScenario.", _file.c_str());
      delete _xml;
      abort();
    }

    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert(_xml->GetNodeName(_mainNode) == std::string("trigger"));
    XMLNodePointer_t _listNode = _xml->GetChild(_mainNode);

    while (_listNode != 0) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);
      XMLNodePointer_t _node = _xml->GetChild(_listNode);
      if (_listName == "lumivalues") {
        while (_node != 0) {
          if (_xml->GetNodeName(_node) == std::string("target_paired_bcid")) {
            Config::config().set(kTargetPairedBunches, stringToInt(_xml->GetNodeContent(_node)), "TargetPairedBunches");
          } else if (_xml->GetNodeName(_node) == std::string("target_peak_mu_av")) {
            Config::config().setFloat(kTargetPeakMuAverage, stringToFloat(_xml->GetNodeContent(_node)), "TargetPeakMuAverage");
          }
          _node = _xml->GetNext(_node);
        }
      } else if (_listName == "iso_bits") {
        while (_node != 0) {
          if (_xml->GetNodeName(_node) == std::string("iso")) {
            std::bitset<5> _bits(std::string(_xml->GetAttr(_node, "bits")));
            _isoBits[ _xml->GetAttr(_node, "name") ] = _bits.to_ulong();
            Info("MonitorRatesUpgrade::parseUpgradeXML", "Got isobits for %s = %s => %s = %i",
                 _xml->GetAttr(_node, "name"), _xml->GetAttr(_node, "bits"), _bits.to_string().c_str(), _isoBits[_xml->GetAttr(_node, "name")]);
          }
          _node = _xml->GetNext(_node);
        }
      } else if (_listName == "level") {
        while (_node != 0) {
          if (_xml->GetNodeName(_node) == std::string("lvl_name")) {
            _level = _xml->GetNodeContent(_node);
            _node = _xml->GetNext(_node);
            if (_node == 0) break;
          }
          assert(_xml->GetNodeName(_node) == std::string("signature"));
          XMLNodePointer_t _detailsNode = _xml->GetChild(_node);
          std::string _sigName = "", _group = "", _comment = "", _hltProxy = "", _lower = "";
          Float_t _l1ReductionFactor = 1., _l2ReductionFactor = 1., _hltReductionFactor = 1.;
          TriggerLogic _triggerLogic;

          while (_detailsNode != 0) {
            const std::string _detail = _xml->GetNodeName(_detailsNode);
            if (_detail != "conditions" && _xml->GetNodeContent(_detailsNode) == nullptr) { // Avoid NULL except with
                                                                                            // conditions
              _detailsNode = _xml->GetNext(_detailsNode);
              continue;
            }
            if (_detail == "sig_name") _sigName = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "hlt_proxy") _hltProxy = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "group") _group = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "comment") _comment = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "lower_chain_name") _lower = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "l1_reduction_factor") _l1ReductionFactor = stringToFloat(_xml->GetNodeContent(_detailsNode));
            else if (_detail == "l2_reduction_factor") _l2ReductionFactor = stringToFloat(_xml->GetNodeContent(_detailsNode));
            else if (_detail == "hlt_reduction_factor") _hltReductionFactor = stringToFloat(_xml->GetNodeContent(_detailsNode));
            else if (_detail == "conditions") {
              XMLNodePointer_t _requirement = _xml->GetChild(_detailsNode);
              while (_requirement != 0) {
                std::string _name = _xml->GetAttr(_requirement, "name");
                UInt_t _multi = stringToInt(_xml->GetAttr(_requirement, "multi"));
                UInt_t _thresh = stringToInt(_xml->GetAttr(_requirement, "triggerthreshold"));
                Int_t _iso = _isoBits[ _xml->GetAttr(_requirement, "iso") ];
                UInt_t _min = stringToInt(_xml->GetAttr(_requirement, "min"));
                UInt_t _max = stringToInt(_xml->GetAttr(_requirement, "max"));
                _triggerLogic.addCondition(_name, _multi, _thresh, _iso, _min, _max);
                _requirement = _xml->GetNext(_requirement);
              }
            } else if (_detail == "OR") {
              Error("MonitorRatesUpgrade::parseUpgradeXML", "The OR of TOBs is currently unsupported");
            } else {
              Error("MonitorRatesUpgrade::parseUpgradeXML", "Unknown XML detail: %s", _detail.c_str());
            }
            _detailsNode = _xml->GetNext(_detailsNode);
          }
          if (_level == "L1") {
            Info("MonitorRatesUpgrade::parseUpgradeXML", "Will do L1->L2 chain: %s with logic: %s", _sigName.c_str(), _triggerLogic.print().c_str());
            _upgradeChains.insert(ChainInfo(_sigName, 1, _triggerLogic, _group, _comment, _l1ReductionFactor, _l2ReductionFactor));
          } else if (_level == "HLT") {
            _upgradeChains.insert(ChainInfo(_sigName, 3, _hltProxy, _lower, _comment, _hltReductionFactor));
          } else {
            Error("MonitorRatesUpgrade::parseUpgradeXML", "Signatures from unknown trigger level: %s", _level.c_str());
          }
          _node = _xml->GetNext(_node);
        }
      }

      _listNode = _xml->GetNext(_listNode);
    }

    delete _xml;
  }
} // namespace TrigCostRootAnalysis
