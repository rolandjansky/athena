/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <sstream>
#include <iostream>
#include <iomanip>
#include <assert.h>

// Local include(s):
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT include(s):
#include <TLatex.h>
#include <TH1.h>
#include <TMath.h>
#include <TSystem.h>

namespace TrigCostRootAnalysis {
  /**
   * Check to see if a counter name has been specified by the user as one we're interested in.
   * Match it to the vector of chains to run over.
   * A check is made here to allow for the exlusion of rerun chains in events which failed the HLT
   * @param _counterName Const reference to counter name to test.
   * @param _invert if the results are to be inverted
   * @result If the counter is in the list of counters to process in this run.
   */
  Bool_t checkPatternNameMonitor(const std::string& _counterName, Bool_t _invert, Bool_t _isRerun) {
    Bool_t _result = checkPatternInternal(_counterName, kPatternsMonitor, _invert);
    static Bool_t _isPrediction = (Bool_t) Config::config().getInt(kIsCPUPrediction);

    if (_isPrediction == kTRUE && _result == kTRUE) {
      // Check if this chain is rerun
      Bool_t _ignoreRerun = (Bool_t) Config::config().getInt(kIgnoreRerun);
      if (_ignoreRerun == kTRUE && _isRerun == kTRUE) return kFALSE;
    }
    return _result;
  }

  /**
   * Check to see if a chain is on the list of chains to NEVER lumi weight
   * @param _counterName Const reference to counter name to test.
   * @result If the counter is on the list
   */
  Bool_t checkPatternNoLumiWeight(const std::string& _counterName) {
    return Config::config().getVecMatches(kPatternsNoLumiWeight, _counterName);
  }

  /**
   * Check to see if a chain is on the list of chains to not get the *mu* part of the lumi weight
   * Will still get the bunch part
   * @param _counterName Const reference to counter name to test.
   * @result If the counter is on the list
   */
  Bool_t checkPatternNoMuLumiWeight(const std::string& _counterName) {
    return Config::config().getVecMatches(kPatternsNoMuLumiWeight, _counterName);
  }

  /**
   * Check to see if a chain is on the list of chains to not get the *bunch* part of the lumi weight
   * Will still get the mu part
   * @param _counterName Const reference to counter name to test.
   * @result If the counter is on the list
   */
  Bool_t checkPatternNoBunchLumiWeight(const std::string& _counterName) {
    return Config::config().getVecMatches(kPatternsNoBunchLumiWeight, _counterName);
  }

  /**
   * Check to see if a chain is on the list of chains to get *exponential* scaling in <mu>
   * @param _counterName Const reference to counter name to test.
   * @result If the counter is on the list
   */
  Bool_t checkPatternExponentialWithMu(const std::string& _counterName) {
    return Config::config().getVecMatches(kPatternsExpoMuLumiWeight, _counterName);
  }

  /**
   * Check to see if a counter name has been specified by the user as one we're interested in saving.
   * Match it to the vector of chains to save.
   * @param _counterName Const reference to counter name to test.
   * @param _invert if the results are to be inverted
   * @result If the counter is in the list of counters to output from this run.
   */
  Bool_t checkPatternNameOutput(const std::string& _counterName, Bool_t _invert) {
    return checkPatternInternal(_counterName, kPatternsOutput, _invert);
  }

  /**
   * Check to see if a counter name has been specified by the user as one we're interested in
   * doing unique rates for.
   * @param _counterName Const reference to counter name to test.
   * @result If the counter is in the list of counters to output from this run.
   */
  Bool_t checkPatternUnique(const std::string& _counterName, Bool_t _invert) {
    return checkPatternInternal(_counterName, kPatternsUnique, _invert);
  }

  /**
   * Check to see if a counter name has been specified by the user as one we're interested in
   * doing overlap rates for.
   * @param _counterName Const reference to counter name to test.
   * @result If the counter is in the list of counters to output from this run.
   */
  Bool_t checkPatternOverlap(const std::string& _counterName, Bool_t _invert) {
    return checkPatternInternal(_counterName, kPatternsOverlap, _invert);
  }

  /**
   * Code to actually perform the check if a counter is listed in a given vector
   * @param _counterName Const reference to counter name to test.
   * @param _list The key of the saved configuration vector to test in
   * @result If the counter is in the list of counters to output from this run.
   */
  Bool_t checkPatternInternal(const std::string& _counterName, ConfKey_t _list, Bool_t _invert) {
    if (Config::config().getVecSize(_list) > 0) {
      Bool_t _result = Config::config().getVecMatches(_list, _counterName);
      if (_invert == kTRUE) return !_result;

      return _result;
    }
    return kTRUE;
  }

  /**
   * Helper function, converts string to signed int
   * @param _i string to convert.
   * @returns Integer representation of string.
   */
  Int_t stringToInt(const std::string& _i) {
    Int_t _result = INT_MAX;

    std::istringstream(_i) >> _result;
    return _result;
  }

  /**
   * Helper function, converts string to float
   * @param _i string to convert.
   * @returns Floating point representation of string.
   */
  Float_t stringToFloat(const std::string& _i) {
    Float_t _result = FLT_MAX;

    std::istringstream(_i) >> _result;
    return _result;
  }

  /**
   * Helper function, converts string to double
   * @param _i string to convert.
   * @returns Double precision point representation of string.
   */
  Double_t stringToDouble(const std::string& _i) {
    Double_t _result = DBL_MAX;

    std::istringstream(_i) >> _result;
    return _result;
  }

  /**
   * Helper function, converts unsigned integer to string.
   * @param _i UInt_t to convert.
   * @returns String of integer.
   */
  std::string intToString(UInt_t _i, UInt_t _pad) {
    return intToString((Long64_t) _i, _pad);
  }

  /**
   * Helper function, converts long integer to string.
   * @param _i Long64_t to convert.
   * @returns String of integer.
   */
  std::string intToString(Long64_t _i, UInt_t _pad) {
    std::ostringstream _ss;
    if (_pad) _ss << std::setfill('0') << std::setw(_pad);
    _ss << std::fixed << _i;
    return _ss.str();
  }

  /**
   * Helper function, converts integer to string.
   * @param _i Int_t to convert.
   * @returns String of integer.
   */
  std::string intToString(Int_t _i, UInt_t _pad) {
    return intToString((Long64_t) _i, _pad);
  }

  /**
   * Helper function, converts floating point to string.
   * @param _f Float_t to convert.
   * @param _precision Optional number of decimal places.
   * @returns String of float.
   */
  std::string floatToString(Float_t _f, Int_t _precision) {
    // This is going to screen so internal precision doesn't matter
    return doubleToString((Double_t) _f, _precision);
  }

  /**
   * Helper function, converts double precision floating point to string.
   * @param _d Float_t to convert.
   * @param _precision Optional number of decimal places.
   * @returns String of double.
   */
  std::string doubleToString(Double_t _d, Int_t _precision) {
    std::ostringstream ss;
    ss.precision(_precision);
    if (isZero(_d) == kTRUE) {
      ss << "0";
    } else if (TMath::Abs(_d) < 1e4) {
      ss << std::fixed << _d;
    } else {
      ss << std::scientific << _d;
    }
    return ss.str();
  }

  /**
   * Helper function, calculate distance in eta and phi
   */
  Float_t deltaR(Float_t _phi1, Float_t _phi2, Float_t _eta1, Float_t _eta2) {
    if (_phi1 < 0) _phi1 += 2 * M_PI;
    if (_phi2 < 0) _phi2 += 2 * M_PI;
    assert(_phi1 >= 0 && _phi1 <= 2 * M_PI);
    assert(_phi2 >= 0 && _phi2 <= 2 * M_PI);
    const Float_t _phiDiff = _phi2 - _phi1;
    Float_t _deltaPhi = _phiDiff;
    if (_phiDiff > M_PI) _deltaPhi = -2 * M_PI + _phiDiff;
    else if (_phiDiff < -M_PI) _deltaPhi = 2 * M_PI + _phiDiff;
    Float_t _dE = _eta1 - _eta2;
    return TMath::Sqrt(_deltaPhi * _deltaPhi + _dE * _dE);
  }

  /**
   * Helper function, draws a TLatex on the current active TCanvas.
   * @param x,y Coordinates of text in NDC (Range: 0 to 1).
   * @param text Const character array of text to print.
   */
  void plotText(Double_t x, Double_t y, const char* text) {
    TLatex l; //l.SetTextAlign(12);

    l.SetTextSize(0.05);
    l.SetNDC();
    l.DrawLatex(x, y, text);
  }

  /**
   * Helper function, plots histogram to current canvas.
   * @param _h Pointer to histogram to plot.
   * @param _isLogY Flag to specify if histogram is being plotted on a logarithmic canvas.
   * @param _opt Drawing options to pass to the painter.
   */
  void plotHistogram(TH1F* _h, Bool_t _isLogY, std::string _opt) {
    UNUSED(_isLogY); //TODO remove me?
    // Standard ROOT histogram stuff
    // if (_isLogY == kTRUE) {
    //   _h->GetYaxis()->SetRangeUser(.1, _h->GetMaximum() * 10.);
    // } else {
    //   _h->GetYaxis()->SetRangeUser(0., _h->GetMaximum() * 1.1);
    // }
    _h->Draw(_opt.c_str());
  }

  /**
   * Helper function, check if a floating point is compatible with zero.
   * @param _float Value to test.
   * @param _precision Optional level of precision required.
   * @returns true if value is compatible with zero.
   */
  Bool_t isZero(Float_t _float, Float_t _precision) {
    if (TMath::Abs(_float) < _precision) return kTRUE;

    return kFALSE;
  }

  /**
   * Helper function, check if a floating point numbers are compatable.
   * @param _float1 First value to test.
   * @param _float2 Second value to test.
   * @param _precision Optional level of precision required.
   * @returns true if value1 and value2 are equal to each other.
   */
  Bool_t isEqual(Float_t _float1, Float_t _float2, Float_t _precision) {
    if (TMath::Abs(_float1 - _float2) < _precision) return kTRUE;

    return kFALSE;
  }

  /**
   * Helper function, constructs a std::pair from a variable name and VariableOption.
   * @param _name The name of the variable.
   * @param _vo The desired VariableOption
   * @returns The supplied data wrapped in a pair.
   */
  ConfVariableOptionPair_t makePair(ConfKey_t _name, VariableOption_t _vo) {
    return std::make_pair(_name, _vo);
  }

  /**
   * Helper function, returns basic hash of string.
   * hash function (based on available elswhere ELF hash function)
   * @param _s String to hash
   * @returns Hash value.
   */
  UInt_t stringToIntHash(const std::string& _s) {
    UInt_t _hash;

    _hash = 0xd2d84a61;
    Int_t i;

    for (i = (Int_t) _s.size() - 1; i >= 0; --i) _hash ^= (_hash >> 5) + _s[i] + (_hash << 7);
    for (i = 0; i < (Int_t) _s.size(); ++i) _hash ^= (_hash >> 5) + _s[i] + (_hash << 7);

    return _hash;
  }

  const std::string& getLevelString(UInt_t _level) {
    if (_level == 1) return Config::config().getStr(kL1String);

    if (_level == 2 && Config::config().getInt(kDoL2) == 1) return Config::config().getStr(kL2String);

    if (_level == 2 && Config::config().getInt(kDoHLT) == 1) return Config::config().getStr(kHLTString);

    if (_level == 3 && Config::config().getInt(kDoEF) == 1) return Config::config().getStr(kEFString);

    return Config::config().getStr(kUnknownString);
  }
}
