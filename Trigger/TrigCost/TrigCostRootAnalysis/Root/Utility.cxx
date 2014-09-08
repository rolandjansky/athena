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
   * Helper function, converts string to signed int
   * @param _i string to convert.
   * @returns Integer representation of string.
   */
  Int_t stringToInt(const std::string& _i) {
    Int_t _result = INT_MAX;
    std::istringstream( _i ) >> _result;
    return _result;
  }

  /**
   * Helper function, converts string to float
   * @param _i string to convert.
   * @returns Floating point representation of string.
   */
  Float_t stringToFloat(const std::string& _i) {
    Float_t _result = FLT_MAX;
    std::istringstream( _i ) >> _result;
    return _result;
  }

  /**
   * Helper function, converts unsigned integer to string.
   * @param _i UInt_t to convert.
   * @returns String of integer.
   */
  std::string intToString(UInt_t _i, UInt_t _pad) {
    return intToString((Int_t) _i, _pad);
  }
  
  /**
   * Helper function, converts integer to string.
   * @param _i Int_t to convert.
   * @returns String of integer.
   */
  std::string intToString(Int_t _i, UInt_t _pad) {
    std::ostringstream _ss;
    if (_pad) _ss << std::setfill('0') << std::setw(_pad); 
    _ss << std::fixed << _i;
    return _ss.str();
  }
  
  /**
   * Helper function, converts floating point to string.
   * @param _f Float_t to convert.
   * @param _precision Optional number of decimal places.
   * @returns String of float.
   */
  std::string floatToString(Float_t _f, Int_t _precision) {
    // This is going to screen so internal precision doesn't matter
    return doubleToString( (Double_t) _f, _precision );
  }
  
  /**
   * Helper function, converts double precision floating point to string.
   * @param _d Float_t to convert.
   * @param _precision Optional number of decimal places.
   * @returns String of double.
   */
  std::string doubleToString(Double_t _d, Int_t _precision) {
    std::ostringstream ss;
    ss.precision( _precision );
    if (TMath::Abs(_d) < 1e4) {
      ss << std::fixed << _d;
    } else {
      ss << std::scientific << _d;
    }
    return ss.str();
  }
  
  /**
   * Helper function, draws a TLatex on the current active TCanvas.
   * @param x,y Coordinates of text in NDC (Range: 0 to 1).
   * @param text Const character array of text to print.
   */
  void plotText(Double_t x, Double_t y, const char *text) {
    TLatex l; //l.SetTextAlign(12);
    l.SetTextSize(0.05 );
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
   * Helper function, returns basic hash of two integer values.
   * @param _a,_b Values to hash
   * @returns Hash value.
   */
  Int_t intIntToIntHash( Int_t _a, Int_t _b ) {
    return (_a << 16) | (_b && 0xFFFF);
  }
  
  /**
   * Helper function, returns basic hash of string.
   * Based on http://stackoverflow.com/questions/2535284/
   * @param _s String to hash
   * @returns Hash value.
   */
  Int_t stringToIntHash( std::string _s ) {
    Int_t _hash = 0;
    Int_t _offset = 'a' - 1;
    for(std::string::const_iterator it = _s.begin(); it != _s.end(); ++it) {
      _hash = _hash << 1 | (*it - _offset);
    }
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
