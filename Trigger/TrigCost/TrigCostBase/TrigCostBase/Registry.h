/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_REGISTRY_H
#define ANP_REGISTRY_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : Registry
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *
 * Registry class helps with configuration of job within TrigCost 
 * framework. Registry configuration is set in python script or xml file
 * and then passed to RunModule and all AlgEvent instances created by a job.
 * 
 * 
 * Class stores (key, value) pairs for following types for values:
 *  - string
 *  - double
 *  - Registry
 *
 *  - provides int, unsigned, const char * Get() functions which are 
 *    internally converted to/from the above three basic types
 *
 *  - provides access methods for reading ',' separated lists as vectors
 *
 *  Use partial template implementation of Get/Set functions.
 *
 **********************************************************************************/

// C/C++
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

// Local
#include "TrigCostBase/DataPair.h"

namespace Anp
{
  class Registry {
  public:
    
    Registry() :fUniqueKeys(true) {}
    ~Registry() {}

    // Deep copy of rhs into this Registry
    void Merge(const Registry& rhs);

    // Deep copy of rhs into this Registry: append to existing string values
    void Add(const Registry& rhs);
    
    bool KeyExists(const std::string &key) const;
    bool RemoveKey(const std::string &key);
    
    void AllowNonUniqueKeys() { fUniqueKeys = false; }

    // Full clear of this Registry 
    void Clear();
    
    // Access methods - see Registry.icc for implemented types
    template<class T> bool Get(const std::string &key, T &val) const;
    template<class T> void Set(const std::string &key, const T &val);
    template<class T> void Set(const std::string &key, const T *val);

    // Read key value as string and then convert it to vector using ',' delimeter
    bool Get(const std::string &key, std::vector<std::string> &val) const;
    bool Get(const std::string &key, std::vector<int>         &val) const;
    bool Get(const std::string &key, std::vector<double>      &val) const;

    // Try two keys: "algorithm::key" or "key"
    template<class T> bool Get(const std::string &algorithm, const std::string& key, T &val) const;   

    // Get internal storage vectors
    const std::vector<Anp::DataPair<std::string, std::string> >&   GetStr() const;
    const std::vector<Anp::DataPair<std::string, double> >&        GetDbl() const;
    const std::vector<Anp::DataPair<std::string, Anp::Registry> >& GetReg() const;

    // Print and read methods
    void Print(std::ostream &os = std::cout,
	       unsigned int margin = 0, const std::string &key = "") const;

    void Read(std::istream &is) const;
    
    void PrintConfig(std::ostream &os = std::cout, const std::string &key = "") const;
    
    void WriteXML(const std::string  &path, unsigned left_pad = 0) const;
    void WriteXML(std::ostream &outf, unsigned left_pad = 0) const;

    void ReadXML(const std::string &path);
    
  private:
    
    bool                                                    fUniqueKeys;
    std::vector<Anp::DataPair<std::string,std::string> >    fStr;
    std::vector<Anp::DataPair<std::string,double> >         fDbl;
    std::vector<Anp::DataPair<std::string,Anp::Registry> >  fReg;
  };

  inline std::ostream& operator<<(std::ostream& os, const Anp::Registry &reg) {
    reg.Print(os);
    return os;
  }
}

#include "TrigCostBase/Registry.icc"
#endif
