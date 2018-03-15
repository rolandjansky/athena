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
   * @param counterName Const reference to counter name to test.
   * @param invert if the results are to be inverted
   * @result If the counter is in the list of counters to process in this run.
   */
  Bool_t checkPatternNameMonitor(const std::string& counterName, Bool_t invert, Bool_t isRerun) {
    Bool_t result = checkPatternInternal(counterName, kPatternsMonitor, invert);
    static Bool_t isPrediction = (Bool_t) Config::config().getInt(kIsCPUPrediction);

    if (isPrediction == kTRUE && result == kTRUE) {
      // Check if this chain is rerun
      Bool_t ignoreRerun = (Bool_t) Config::config().getInt(kIgnoreRerun);
      if (ignoreRerun == kTRUE && isRerun == kTRUE) return kFALSE;
    }
    return result;
  }

  /**
   * Check to see if a chain is on the list of chains to NEVER lumi weight
   * @param counterName Const reference to counter name to test.
   * @result If the counter is on the list
   */
  Bool_t checkPatternNoLumiWeight(const std::string& counterName) {
    return Config::config().getVecMatches(kPatternsNoLumiWeight, counterName);
  }

  /**
   * Check to see if a chain is on the list of chains to not get the *mu* part of the lumi weight
   * Will still get the bunch part
   * @param counterName Const reference to counter name to test.
   * @result If the counter is on the list
   */
  Bool_t checkPatternNoMuLumiWeight(const std::string& counterName) {
    return Config::config().getVecMatches(kPatternsNoMuLumiWeight, counterName);
  }

  /**
   * Check to see if a chain is on the list of chains to not get the *bunch* part of the lumi weight
   * Will still get the mu part
   * @param counterName Const reference to counter name to test.
   * @result If the counter is on the list
   */
  Bool_t checkPatternNoBunchLumiWeight(const std::string& counterName) {
    return Config::config().getVecMatches(kPatternsNoBunchLumiWeight, counterName);
  }

  /**
   * Check to see if a chain is on the list of chains to get *exponential* scaling in \<mu\>
   * @param counterName Const reference to counter name to test.
   * @result If the counter is on the list
   */
  Bool_t checkPatternExponentialWithMu(const std::string& counterName) {
    return Config::config().getVecMatches(kPatternsExpoMuLumiWeight, counterName);
  }

  /**
   * Check to see if a counter name has been specified by the user as one we're interested in saving.
   * Match it to the vector of chains to save.
   * @param counterName Const reference to counter name to test.
   * @param invert if the results are to be inverted
   * @result If the counter is in the list of counters to output from this run.
   */
  Bool_t checkPatternNameOutput(const std::string& counterName, Bool_t invert) {
    return checkPatternInternal(counterName, kPatternsOutput, invert);
  }

  /**
   * Check to see if a counter name has been specified by the user as one we're interested in
   * doing unique rates for.
   * @param counterName Const reference to counter name to test.
   * @result If the counter is in the list of counters to output from this run.
   */
  Bool_t checkPatternUnique(const std::string& counterName, Bool_t invert) {
    return checkPatternInternal(counterName, kPatternsUnique, invert);
  }

  /**
   * Check to see if a counter name has been specified by the user as one we're interested in
   * doing overlap rates for.
   * @param counterName Const reference to counter name to test.
   * @result If the counter is in the list of counters to output from this run.
   */
  Bool_t checkPatternOverlap(const std::string& counterName, Bool_t invert) {
    return checkPatternInternal(counterName, kPatternsOverlap, invert);
  }

  /**
   * Code to actually perform the check if a counter is listed in a given vector
   * @param counterName Const reference to counter name to test.
   * @param list The key of the saved configuration vector to test in
   * @result If the counter is in the list of counters to output from this run.
   */
  Bool_t checkPatternInternal(const std::string& counterName, ConfKey_t list, Bool_t invert) {
    if (Config::config().getVecSize(list) > 0) {
      Bool_t result = Config::config().getVecMatches(list, counterName);
      if (invert == kTRUE) return !result;

      return result;
    }
    return kTRUE;
  }

  /**
   * Helper function, converts string to signed int
   * @param i string to convert.
   * @returns Integer representation of string.
   */
  Int_t stringToInt(const std::string& i) {
    Int_t result = INT_MAX;

    std::istringstream(i) >> result;
    return result;
  }

  /**
   * Helper function, converts string to float
   * @param i string to convert.
   * @returns Floating point representation of string.
   */
  Float_t stringToFloat(const std::string& i) {
    Float_t result = FLT_MAX;

    std::istringstream(i) >> result;
    return result;
  }

  /**
   * Helper function, converts string to double
   * @param i string to convert.
   * @returns Double precision point representation of string.
   */
  Double_t stringToDouble(const std::string& i) {
    Double_t result = DBL_MAX;

    std::istringstream(i) >> result;
    return result;
  }

  /**
   * Helper function, converts unsigned integer to string.
   * @param i UInt_t to convert.
   * @returns String of integer.
   */
  std::string intToString(UInt_t i, UInt_t pad) {
    return intToString((Long64_t) i, pad);
  }

  /**
   * Helper function, converts long integer to string.
   * @param i Long64_t to convert.
   * @returns String of integer.
   */
  std::string intToString(Long64_t i, UInt_t pad) {
    std::ostringstream ss;
    if (pad) ss << std::setfill('0') << std::setw(pad);
    ss << std::fixed << i;
    return ss.str();
  }

  /**
   * Helper function, converts integer to string.
   * @param i Int_t to convert.
   * @returns String of integer.
   */
  std::string intToString(Int_t i, UInt_t pad) {
    return intToString((Long64_t) i, pad);
  }

  /**
   * Helper function, converts floating point to string.
   * @param f Float_t to convert.
   * @param precision Optional number of decimal places.
   * @returns String of float.
   */
  std::string floatToString(Float_t f, Int_t precision) {
    // This is going to screen so internal precision doesn't matter
    return doubleToString((Double_t) f, precision);
  }

  /**
   * Helper function, converts double precision floating point to string.
   * @param d Float_t to convert.
   * @param precision Optional number of decimal places.
   * @returns String of double.
   */
  std::string doubleToString(Double_t d, Int_t precision) {
    std::ostringstream ss;
    ss.precision(precision);
    if (isZero(d) == kTRUE) {
      ss << "0";
    } else if (TMath::Abs(d) < 1e4) {
      ss << std::fixed << d;
    } else {
      ss << std::scientific << d;
    }
    return ss.str();
  }

  /**
   * Helper function, calculate distance in eta and phi
   */
  Float_t deltaR(Float_t phi1, Float_t phi2, Float_t eta1, Float_t eta2) {
    if (phi1 < 0) phi1 += 2 * M_PI;
    if (phi2 < 0) phi2 += 2 * M_PI;
    assert(phi1 >= 0 && phi1 <= 2 * M_PI);
    assert(phi2 >= 0 && phi2 <= 2 * M_PI);
    const Float_t phiDiff = phi2 - phi1;
    Float_t deltaPhi = phiDiff;
    if (phiDiff > M_PI) deltaPhi = -2 * M_PI + phiDiff;
    else if (phiDiff < -M_PI) deltaPhi = 2 * M_PI + phiDiff;
    Float_t dE = eta1 - eta2;
    return TMath::Sqrt(deltaPhi * deltaPhi + dE * dE);
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
   * @param h Pointer to histogram to plot.
   * @param isLogY Flag to specify if histogram is being plotted on a logarithmic canvas.
   * @param opt Drawing options to pass to the painter.
   */
  void plotHistogram(TH1F* h, Bool_t /*isLogY*/, std::string opt) {
    // Standard ROOT histogram stuff
    // if (isLogY == kTRUE) {
    //   h->GetYaxis()->SetRangeUser(.1, h->GetMaximum() * 10.);
    // } else {
    //   h->GetYaxis()->SetRangeUser(0., h->GetMaximum() * 1.1);
    // }
    h->Draw(opt.c_str());
  }

  /**
   * Helper function, check if a floating point is compatible with zero.
   * @param floatval Value to test.
   * @param precision Optional level of precision required.
   * @returns true if value is compatible with zero.
   */
  Bool_t isZero(Float_t floatval, Float_t precision) {
    if (TMath::Abs(floatval) < precision) return kTRUE;

    return kFALSE;
  }

  /**
   * Helper function, check if a floating point numbers are compatable.
   * @param float1 First value to test.
   * @param float2 Second value to test.
   * @param precision Optional level of precision required.
   * @returns true if value1 and value2 are equal to each other.
   */
  Bool_t isEqual(Float_t float1, Float_t float2, Float_t precision) {
    if (TMath::Abs(float1 - float2) < precision) return kTRUE;

    return kFALSE;
  }

  /**
   * Helper function, constructs a std::pair from a variable name and VariableOption.
   * @param name The name of the variable.
   * @param vo The desired VariableOption
   * @returns The supplied data wrapped in a pair.
   */
  ConfVariableOptionPair_t makePair(ConfKey_t name, VariableOption_t vo) {
    return std::make_pair(name, vo);
  }

  /**
   * Helper function, returns basic hash of string.
   * hash function (based on available elswhere ELF hash function)
   * @param s String to hash
   * @returns Hash value.
   */
  UInt_t stringToIntHash(std::string& s) {
    UInt_t hash;

    hash = 0xd2d84a61;
    Int_t i;

    for (i = (Int_t) s.size() - 1; i >= 0; --i) hash ^= (hash >> 5) + s[i] + (hash << 7);
    for (i = 0; i < (Int_t) s.size(); ++i) hash ^= (hash >> 5) + s[i] + (hash << 7);

    return hash;
  }

  const std::string& getLevelString(UInt_t level) {
    if (level == 1) return Config::config().getStr(kL1String);

    if (level == 2 && Config::config().getInt(kDoL2) == 1) return Config::config().getStr(kL2String);

    if (level == 2 && Config::config().getInt(kDoHLT) == 1) return Config::config().getStr(kHLTString);

    if (level == 3 && Config::config().getInt(kDoEF) == 1) return Config::config().getStr(kEFString);

    return Config::config().getStr(kUnknownString);
  }
}
