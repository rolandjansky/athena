/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __METGOODIES__
#define __METGOODIES__

#include "TObject.h"
#include "TString.h"
#include <iomanip>
#include <map>
#include <vector>

namespace MET
{
  static const int MagicNumber = -999;
  class Goodies : public TObject {
    
  public:    
    // destructor
    virtual ~Goodies( );
    // operator
    Goodies& operator=(const Goodies& other);
    
    // access to the single instance of this class (created statically on first request)
    static Goodies& instance();

    bool contains(const TString& varKey, bool suprWarn=true) const;

    inline double getDoubleValue(const TString& varKey, const double& defaultValue=MET::MagicNumber) const { return get(varKey,defaultValue); } 
    inline int getIntValue(const TString& varKey, const int& defaultValue=MET::MagicNumber) const { return get(varKey,defaultValue); }
    //inline vector<double> getVectorValue(const TString& varKey, const vector<double>& defaultValue=MET::MagicNumber) const { return get(varKey,defaultValue); }

    inline void setEntry(Long64_t entry)  { m_entry=entry; }
    inline void setRunNumber(Int_t runnr) { m_runnr=runnr; }
    inline void setValue(const TString& varKey, const double& value) { m_gDouble[varKey] = value; }
    inline void setValue(const TString& varKey, const int& value) { m_gInt[varKey] = value; }
    inline void setValue(const TString& varKey, const std::vector<double> & value) { m_gVectorDouble[varKey]=value; }
    inline void setValue(const TString& varKey, const std::vector<int> & value) { m_gVectorInt[varKey]=value; }
    inline void setVectorValue(const TString& varKey, const double& value) { m_gVectorDouble[varKey].push_back(value); }
    inline void setVectorValue(const TString& varKey, const int& value) { m_gVectorInt[varKey].push_back(value); }

    void print();
    void clear();
    
    Long64_t entry()  { return m_entry; } 
    Int_t runNumber() { return m_runnr; }
    std::map< TString, int >& gInt()       { return m_gInt; }
    std::map< TString, double >& gDouble() { return m_gDouble; }
    std::map< TString, std::vector<int> >& gVectorInt() { return m_gVectorInt; }
    std::map< TString, std::vector<double> >& gVectorDouble() { return m_gVectorDouble; }

  private:
    // default constructor 
    Goodies(Long64_t entry = -1);

    double get(const TString& varKey, const double& defaultValue) const;
    int    get(const TString& varKey, const int& defaultValue) const;
    std::vector<double> get(const TString& varKey, const std::vector<double>& defaultValue) const;
    std::vector<int> get(const TString& varKey, const std::vector<int>& defaultValue) const; 

    mutable Long64_t m_entry; //!
    mutable Int_t m_runnr; //!
    mutable std::map< TString, int > m_gInt; //!
    mutable std::map< TString, double > m_gDouble; //!
    mutable std::map< TString, std::vector<int> > m_gVectorInt; //!
    mutable std::map< TString, std::vector<double> > m_gVectorDouble; //!
    
    ClassDef(MET::Goodies,1)
  };
}

#endif
