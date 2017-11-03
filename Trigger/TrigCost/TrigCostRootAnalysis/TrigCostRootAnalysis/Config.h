// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_Config_H
#define TrigCostRootAnalysis_Config_H

// STL include(s):
#include <string>
#include <vector>
#include <map>
#include <iosfwd>

// ROOT include(s):
#include <Rtypes.h>

// Local include(s):
#include "Utility.h"

/**
 * @file TrigCostRootAnalysis/Config.h
 *
 * This singleton class allows for small pieces of data, such as configuration settings, to be globally
 * distributed to the application via a singleton configuration object.
 *
 * This class also handles all command line parsing.
 *
 */

namespace TrigCostRootAnalysis {
  /**
   * @enum LockStatus
   * Specifies if a setting with a given key can be subsiquently modified or not.
   * Entries are _locked by default_
   */
  enum LockStatus_t {
    kLocked, //!< Setting is locked for the run.
    kUnlocked //!< Setting is mutable.
  };

  /**
   * @class Config
   * Singleton class
   * The config class is used to propagate settings and small data parameters around the application.
   */
  class Config {
  public:
    static Config& config(); //!< Use this method to get the singleton

    Bool_t parseCLI(int argc, char* argv[]);
    const std::string& getStr(ConfKey_t _key);
    const std::string& getName(ConfKey_t _key);
    Int_t getInt(ConfKey_t _key);
    Long64_t getLong(ConfKey_t _key);
    Float_t getFloat(ConfKey_t _key);
    const std::vector<Int_t>& getIntVec(ConfKey_t _key);
    UInt_t getVecSize(ConfKey_t _key);
    const std::string& getVecEntry(ConfKey_t _key, UInt_t);
    Bool_t getVecMatches(ConfKey_t _key, const std::string& _entry);
    Bool_t addVecEntry(ConfKey_t _key, const std::string& _toAdd);
    Bool_t removeVecEntry(ConfKey_t _key, const std::string& _toRemove);
    Bool_t clearVec(ConfKey_t _key);
    Bool_t getIntVecMatches(ConfKey_t _key, const Int_t _entry);
    Bool_t getIsSet(ConfKey_t _key);
    Bool_t getDisplayMsg(ConfKey_t _key);
    Bool_t debug();
    void messageSuppressionReport();
    void set(ConfKey_t _key, const Int_t _value, const std::string _name = "", LockStatus_t _lock = kLocked);
    void set(ConfKey_t _key, const Int_t _value, LockStatus_t _lock = kLocked);
    void setDisplayMsg(ConfKey_t _key, const Int_t _value, const std::string _name = "");
    void setFloat(ConfKey_t _key, const Float_t _value, const std::string _name = "", LockStatus_t _lock = kLocked);
    void setLong(ConfKey_t _key, const Long64_t _value, const std::string _name = "", LockStatus_t _lock = kLocked);
    void set(ConfKey_t _key, const std::vector< std::string > _value, const std::string _name = "",
             LockStatus_t _lock = kLocked);
    void set(ConfKey_t _key, const std::vector< Int_t > _value, const std::string _name = "",
             LockStatus_t _lock = kLocked);
    void set(ConfKey_t _key, const std::string _value, const std::string _name = "", LockStatus_t _lock = kLocked);
    void increment(ConfKey_t _key);
    void decrement(ConfKey_t _key);
    void dump();
    void dumpToMeta(std::ofstream& _fout, JsonExport& _json);
    Bool_t getConfKeyNameFromString(const std::string& s, ConfKey_t& result);
  private:
    Bool_t getIsLocked(ConfKey_t _key, Bool_t _printErrorMsg = kFALSE);

    Bool_t m_debug;
    std::string m_blankString; //!< Instance of blank string to return ref to on failures.
    std::map<ConfKey_t, std::string> m_settingsStr; //!< String Settings map
    std::map<ConfKey_t, std::string> m_settingsName; //!< String Name of settings map
    std::map<ConfKey_t, std::vector<std::string> > m_settingsVecStr; //!< Vector of Strings settings map
    std::map<ConfKey_t, std::vector<Int_t> > m_settingsVecInt; //!< Vector of Int settings map
    std::map<ConfKey_t, Int_t>   m_settingsInt;   //!< Int_t settings map
    std::map<ConfKey_t, Long64_t>   m_settingsLong;   //!< Long int settings map
    std::map<ConfKey_t, Float_t> m_settingsFloat; //!< Float_t settings map
    std::map<ConfKey_t, Int_t> m_settingsMaxCalls;   //!< Holds how many times an error message can been accessed, for
                                                     // supression purposes
    std::map<ConfKey_t, LockStatus_t> m_settingsLock; //!< Holds lock status for settings

    /**
     * Private constructor.
     */
    Config() : m_debug(kFALSE), m_blankString("") {
    };

    /**
     * Private destructor.
     */
    ~Config() {
    };

    /**
     * Private unimplemented copy constructor.
     */
    Config(Config const&);

    /**
     * Private unimplemented assignment constructor.
     */
    void operator = (Config const&);
  };  //class Config
}



#endif //TrigCostRootAnalysis_Config_H
