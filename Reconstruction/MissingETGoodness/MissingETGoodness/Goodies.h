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

    inline void setEntry(Long64_t entry)  { _entry=entry; }
    inline void setRunNumber(Int_t runnr) { _runnr=runnr; }
    inline void setValue(const TString& varKey, const double& value) { _gDouble[varKey] = value; }
    inline void setValue(const TString& varKey, const int& value) { _gInt[varKey] = value; }
    inline void setValue(const TString& varKey, const std::vector<double> & value) { _gVectorDouble[varKey]=value; }
    inline void setValue(const TString& varKey, const std::vector<int> & value) { _gVectorInt[varKey]=value; }
    inline void setVectorValue(const TString& varKey, const double& value) { _gVectorDouble[varKey].push_back(value); }
    inline void setVectorValue(const TString& varKey, const int& value) { _gVectorInt[varKey].push_back(value); }

    void print();
    void clear();
    
    Long64_t entry()  { return _entry; } 
    Int_t runNumber() { return _runnr; }
    std::map< TString, int >& gInt()       { return _gInt; }
    std::map< TString, double >& gDouble() { return _gDouble; }
    std::map< TString, std::vector<int> >& gVectorInt() { return _gVectorInt; }
    std::map< TString, std::vector<double> >& gVectorDouble() { return _gVectorDouble; }

  private:
    // default constructor 
    Goodies(Long64_t entry = -1);

    double get(const TString& varKey, const double& defaultValue) const;
    int    get(const TString& varKey, const int& defaultValue) const;
    std::vector<double> get(const TString& varKey, const std::vector<double>& defaultValue) const;
    std::vector<int> get(const TString& varKey, const std::vector<int>& defaultValue) const; 

    mutable Long64_t _entry; //!
    mutable Int_t _runnr; //!
    mutable std::map< TString, int > _gInt; //!
    mutable std::map< TString, double > _gDouble; //!
    mutable std::map< TString, std::vector<int> > _gVectorInt; //!
    mutable std::map< TString, std::vector<double> > _gVectorDouble; //!
    
    ClassDef(MET::Goodies,1)
  };
}

#endif
