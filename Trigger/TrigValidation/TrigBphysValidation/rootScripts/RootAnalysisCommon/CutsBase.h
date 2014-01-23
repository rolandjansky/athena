/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// CutsBase class
///---------------------------------------------------
/// Base class for cut algorithms
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of B2JpsiV0RootAnalysis
///===================================================

#ifndef CutsBase_H
#define CutsBase_H

#include "Log.h"

enum CutType { CUT_DOUBLE_RANGE, CUT_DOUBLE, CUT_INT, CUT_STRING };

struct CutBase {
  std::string name;         // cut variable name
  bool        enabled;      // is enabled
  CutType     type;         // cut type used for casting
};

struct DoubleRangeCut : CutBase {

  double min;               // min value
  double max;               // max value

  bool operator==(double value) {
    return value >= min && value <= max;
  }

  bool operator!=(double value) {
    return value < min || value > max;
  }

  bool operator<(double value) {
    return max < value;
  }

  bool operator>(double value) {
    return min > value;
  }

};

struct DoubleCut : CutBase {

  double val;               // cut value

  bool operator==(double value) {
    return val == value;
  }

  bool operator!=(double value) {
    return val != value;
  }

  bool operator<(double value) {
    return val < value;
  }

  bool operator>(double value) {
    return val > value;
  }

};

struct IntCut : CutBase{

  int val;                  // cut value

  bool operator==(int value) {
    return val == value;
  }

  bool operator!=(int value) {
    return val != value;
  }

  bool operator<(int value) {
    return val < value;
  }

  bool operator>(int value) {
    return val > value;
  }

};

struct StringCut : CutBase {

  std::string val;          // cut value

  bool operator==(std::string value) {
    return val == value;
  }

  bool operator!=(std::string value) {
    return val != value;
  }

};

class CutsBase {

  public:
    // costructor
    CutsBase() : m_ol(INFO), m_settings("") { };
    CutsBase(OutputLevel olIn, std::string settings) : m_ol(olIn), m_settings(settings) { };

    // read cut values from string m_settings:
    // string must be in the following format:
    //   cut_name(min,max)    for range cuts
    //   cut_name(val)        for single value cuts
    //   cut_name             use default values
    void AddCutVariable(std::string name, DoubleRangeCut& cut, double defaultMin, double defaultMax);   // double range cut
    void AddCutVariable(std::string name, DoubleCut& cut, double defaultVal);                           // single-value cut
    void AddCutVariable(std::string name, IntCut& cut, int defaultVal);                                 // int type single-value cut
    void AddCutVariable(std::string name, StringCut& cut, std::string defaultVal);                      // int type single-value cut

    // inline functions
    std::vector<CutBase*>& getCuts() { return m_cuts; }

  protected:

    OutputLevel ol() { return m_ol; }

  private:

    bool getValString(std::string name, std::string& subStr);

    OutputLevel           m_ol;                 // output level
    std::string           m_settings;           // cut settings

    std::vector<CutBase*> m_cuts;               // collection of all cut variables

};

///===================================================
/// Begin of definitions
///===================================================
void CutsBase::AddCutVariable(std::string name, DoubleRangeCut& cut, double defaultMin, double defaultMax) {
  Log log("CutsBase::AddCutVariable", ol());

  cut.name = name;
  cut.enabled = false;
  cut.min = defaultMin;
  cut.max = defaultMax;

  // register cut variable
  cut.type = CUT_DOUBLE_RANGE;
  m_cuts.push_back(&cut);

  // is variable defined?
  std::string::size_type pos = m_settings.find(name);
  if(pos==std::string::npos) {
    log.info("** " + name + ": disabled (default = " + fToStr(defaultMin) + ", " + fToStr(defaultMax) + ")");
    return;
  }

  // variable is defined
  cut.enabled = true;

  // read values
  std::string valStr;
  if(getValString(name, valStr)) {
    // read minimum
    stringstream ss1(valStr);
    ss1 >> cut.min;

    //read maximum, if it exists
    std::string::size_type pos2 = valStr.find(",");
    if(pos2!=std::string::npos) {
      stringstream ss2(valStr.substr(pos2+1));
      ss2 >> cut.max;
    }
  }

  log.info("** " + name + ": " + fToStr(cut.min) + ", " + fToStr(cut.max));

}
///===================================================
void CutsBase::AddCutVariable(std::string name, DoubleCut& cut, double defaultVal) {
  Log log("CutsBase::AddCutVariable", ol());

  cut.name = name;
  cut.enabled = false;
  cut.val = defaultVal;

  // register cut variable
  cut.type = CUT_DOUBLE;
  m_cuts.push_back(&cut);

  // is variable defined?
  std::string::size_type pos = m_settings.find(name);
  if(pos==std::string::npos) {
    log.info("** " + name + ": disabled (default = " + fToStr(defaultVal) + ")");
    return;
  }

  // variable is defined
  cut.enabled = true;

  // read values
  std::string valStr;
  if(getValString(name, valStr)) {
    // read value
    stringstream ss1(valStr);
    ss1 >> cut.val;
  }

  log.info("** " + name + ": " + fToStr(cut.val));

}
///===================================================
void CutsBase::AddCutVariable(std::string name, IntCut& cut, int defaultVal) {
  Log log("CutsBase::AddCutVariable", ol());

  cut.name = name;
  cut.enabled = false;
  cut.val = defaultVal;

  // register cut variable
  cut.type = CUT_INT;
  m_cuts.push_back(&cut);

  // is variable defined?
  std::string::size_type pos = m_settings.find(name);
  if(pos==std::string::npos) {
    log.info("** " + name + ": disabled (default = " + iToStr(defaultVal) + ")");
    return;
  }

  // variable is defined
  cut.enabled = true;

  // read values
  std::string valStr;
  if(getValString(name, valStr)) {
    // read value
    stringstream ss1(valStr);
    ss1 >> cut.val;
  }

  log.info("** " + name + ": " + iToStr(cut.val));


}
///===================================================
void CutsBase::AddCutVariable(std::string name, StringCut& cut, std::string defaultVal) {
  Log log("CutsBase::AddCutVariable", ol());

  cut.name = name;
  cut.enabled = false;
  cut.val = defaultVal;

  // register cut variable
  cut.type = CUT_STRING;
  m_cuts.push_back(&cut);

  // is variable defined?
  std::string::size_type pos = m_settings.find(name);
  if(pos==std::string::npos) {
    log.info("** " + name + ": disabled (default = " + defaultVal + ")");
    return;
  }

  // variable is defined
  cut.enabled = true;

  // read values
  std::string valStr;
  if(getValString(name, valStr)) {
    // read value
    cut.val = valStr;
  }

  log.info("** " + name + ": " + cut.val);

}
///===================================================
bool CutsBase::getValString(std::string name, std::string& subStr) {
  Log log("CutsBase::getValString", ol() );

  std::string::size_type pos = m_settings.find(name+"(");
  if(pos==std::string::npos) return false;

  // find closing bracket
  std::string::size_type posEnd = m_settings.substr( pos + name.length() + 1 ).find( ")" );
  if(posEnd==std::string::npos) {
    log.error("Closing bracket for variable "+name+" wasn't found. Default values will be used");
    return false;
  }

  //copy the string between the brackets
  std::string tmpStr = m_settings.substr(pos+name.length() + 1, posEnd);

  log.debug("tmpStr = " + tmpStr);

  //remove spaces
  subStr = "";
  for(uint i=0; i<tmpStr.size(); ++i) {
    if(tmpStr.at(i) != ' ') subStr += tmpStr.at(i);
  }

  return true;
}

#endif

///===================================================
///      . .
///====o==V==o========================================
