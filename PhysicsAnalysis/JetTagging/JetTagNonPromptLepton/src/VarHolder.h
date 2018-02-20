// This is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROMPT_VARHOLDER_H
#define PROMPT_VARHOLDER_H

/**********************************************************************************
 * @Package: JetTagNonPromptLepton
 * @Class  : VarHolder
 * @Author : Rustem Ospanov
 * @Author : Rhys Roberts
 *
 * @Brief  :
 * 
 *  VarHolder is a generic analysis object that holds variables as (int, double)
 *  
 **********************************************************************************/

// C/C++
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace Prompt
{
  //======================================================================================================
  namespace Def
  {
    enum Var 
    {
      NONE = 0, 
      TrackJetNTrack,
      sv1_ntkv,
      jf_ntrkv,
      sv1_jf_ntrkv,
      ip2,
      ip2_cu,
      ip3,
      ip3_cu,
      EtTopoCone20Rel,
      EtTopoCone30Rel,
      TopoEtCone30Rel,
      PtVarCone30Rel,
      DRlj,
      LepJetPtFrac,
      PtFrac,
      PtRel,
      DL1mu,
      rnnip,
      MV2c10rnn,
      SV1,
      JetF
    };

    int RegisterAllVars();
    
    bool RegisterVar(Var var, const std::string &name);

    const std::vector<std::string>&      GetAllVarNames();
    const std::vector<Prompt::Def::Var>& GetAllVarEnums();

    std::string       AsStr(uint32_t key, double val);
    std::string       AsStr(Var var);
    std::string Convert2Str(Var var);
    Var         Convert2Var(const std::string &var);   
    Var         Convert2Var(uint32_t           key);   

    std::vector<Var> ReadVars(const std::string &config);
    std::vector<Var> ReadVectorVars(const std::vector<std::string> &keys);

    void PrintVarNames();
    
    void StringTok(std::vector<std::string>& ls,
		   const std::string& str,
		   const std::string& tok);  
  }

  //======================================================================================================
  class VarEntry
  {
  public:
    
    VarEntry();
    VarEntry(unsigned key, double value);
    ~VarEntry() {}
    
    unsigned GetKey ()  const { return fKey;  }
    double   GetVar ()  const { return fData; }
    double   GetData()  const { return fData; }
  
  private:
    
    uint32_t fKey;  // variable key
    double   fData; // variable value
  };

  typedef std::vector<Prompt::VarEntry> VarEntryVec;

  //======================================================================================================
  class VarHolder 
  {
  public:

    VarHolder() {}
    virtual ~VarHolder() {}
    
    static std::string GetObjectType() { return "VarHolder"; }
    
    bool  ReplaceVar(unsigned key, double value);
    bool  AddVar    (unsigned key, double value);
    bool  DelVar    (unsigned key);

    double GetVar   (unsigned key) const;
    bool   GetVar   (unsigned key, double &value) const;
    bool   GetVar   (unsigned key, float  &value) const;

    bool  HasKey(unsigned key) const;
    bool  HasVar(unsigned key) const;

          VarEntryVec& GetVars    ()       { return fVars; }
    const VarEntryVec& GetVars    () const { return fVars; }
    const VarEntryVec& GetVarEntry() const { return fVars; }

    virtual void ClearVars();

  private:

    VarEntryVec fVars;
  }; 

  //======================================================================================================
  // VarEntry inline functions and comparison operators
  //
  inline VarEntry::VarEntry() :fKey(0), fData(0.)
  {
  }

  inline VarEntry::VarEntry(unsigned int key, double data)
    :fKey(key), fData(data)
  {
  }

  inline bool operator==(const VarEntry &lhs, const VarEntry &rhs) 
  {
    return lhs.GetKey() == rhs.GetKey();
  }
  inline bool operator<(const VarEntry &lhs, const VarEntry &rhs)
  {
    return lhs.GetKey() < rhs.GetKey();
  }

  inline bool operator==(const VarEntry &var, unsigned key) { return var.GetKey() == key; }
  inline bool operator==(unsigned key, const VarEntry &var) { return var.GetKey() == key; }

  inline bool operator<(const VarEntry &var, unsigned key) { return var.GetKey() < key; }
  inline bool operator<(unsigned key, const VarEntry &var) { return key < var.GetKey(); }

  //======================================================================================================
  // VarHolder inline functions
  //
  inline bool VarHolder::ReplaceVar(const unsigned key, const double value)
  {
    if(!HasKey(key)) {
      fVars.push_back(VarEntry(key, value));
      return true;
    }
    else{
      DelVar(key);
      AddVar(key, value);
    }
    return false;
  }

  inline bool VarHolder::AddVar(const unsigned key, const double value)
  {
    if(!HasKey(key)) {
      fVars.push_back(VarEntry(key, value));
      return true;
    }
    
    std::cout << GetObjectType() << "::AddVar(" << key << ", " << value << ") - key already exists" << std::endl;
    return false;
  }
  
  inline bool VarHolder::DelVar(const unsigned key)
  {
    VarEntryVec::iterator vit = fVars.begin();
    while(vit != fVars.end()) {
      if(vit->GetKey() == key) {
	vit = fVars.erase(vit);
      }
      else {
	vit++;
      }
    }
    
    return false;
  }

  inline bool VarHolder::HasKey(unsigned key) const
  {
    return std::find(fVars.begin(), fVars.end(), key) != fVars.end();
  }
  inline bool VarHolder::HasVar(unsigned key) const
  {
    return std::find(fVars.begin(), fVars.end(), key) != fVars.end();
  }
  
  inline bool VarHolder::GetVar(unsigned key, float &value) const
  {
    //
    // Read variable
    //
    const VarEntryVec::const_iterator ivar = std::find(fVars.begin(), fVars.end(), key);
    if(ivar != fVars.end()) {
      value = ivar->GetData();
      return true;
    }
    
    return false;
  }
    
  inline bool VarHolder::GetVar(unsigned key, double &value) const
  {
    //
    // Read variable
    //
    const VarEntryVec::const_iterator ivar = std::find(fVars.begin(), fVars.end(), key);
    if(ivar != fVars.end()) {
      value = ivar->GetData();
      return true;
    }
      
    return false;
  }

  inline double VarHolder::GetVar(const unsigned key) const
  {
    //
    // Find and return, if exists, value stored at key
    //
    double val = -1.0e9;
    GetVar(key, val);    
    return val;
  }

  inline void VarHolder::ClearVars()
  {
    fVars.clear();
  }
}

#endif // PROMPT_VARHOLDER_H
