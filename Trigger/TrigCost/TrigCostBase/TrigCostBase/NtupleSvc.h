/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_NTUPLESVC_H
#define ANP_NTUPLESVC_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : NtupleSvc
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  NtuplSvc is singleton class that controls read/write access to TTree branches.
 * 
 *  This method allows READ/WRITE access to branches:
 *    - Branch<T>(const std::string &, const std::string &, T *)
 * 
 *  This method allows READ only access to single branch matching type T:
 *    - ReadBranch<T>(const std::string &branch);
 * 
 **********************************************************************************/

// C/C++
#include <cassert>
#include <vector>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/NtupleBase.h"

namespace Anp
{
  class Registry;

  template <class T> class Branch {
  public:   

    Branch() :fData(0) {}
    explicit Branch(NtupleData<T> *data) : fData(data) { assert(fData && "null branch pointer"); }
    ~Branch() {}

    bool IsValid() { return fData; }
    bool   valid() { return fData; }

    void release() { fData = 0; }

    T* GetData() { return fData->GetData(); }
    T* GetPtr()  { return fData->GetPtr(); }
    T& GetRef()  { return fData->GetRef(); }

    T* get()  { return fData->GetPtr(); }
    T& ref()  { return fData->GetRef(); }

    T* operator->() { return fData->GetPtr(); }
    T& operator *() { return fData->GetRef(); }

    void SetEvent(Ntuple::Type t) { fData->SetEvent(t); }
    void SetState(Ntuple::Type t) { fData->SetState(t); }

    Ntuple::Type GetEvent() { return fData->GetEvent(); }
    Ntuple::Type GetState() { return fData->GetState(); }
    
    void SetType(const std::string &type) { fData->SetType(type); }

  private:

    NtupleData<T> *fData;
  };

  class NtupleSvc {
  public:

    enum Mode { NONE, READ, FILL, NEW_ENTRY };

    static NtupleSvc& Instance(const std::string &tree_name);
    
    bool Set(TTree *tree, Mode mode);

    bool Set(Mode mode);

    bool Reset();

    void CopyTo(NtupleSvc &rhs);

    void Config(const Registry &reg);

    void Print() const;

    template<class T> Anp::Branch<T> Branch(const std::string &branch);

  private:

    // These functions are private by design
    NtupleSvc();
    ~NtupleSvc() {}    
    
    NtupleSvc(const NtupleSvc &);
    NtupleSvc& operator=(const NtupleSvc &);

  private:

    typedef std::map<std::string, Anp::NtupleBase *> BranchMap;
    
  private:

    TTree            *fTree;
    Mode              fMode;
    BranchMap         fBranchMap;
    
    bool              fBranchStatusOff;  // disable all branches by default
    bool              fDebug;            // print debug messages
  };
}

//
// Implementation details
//
#include "NtupleSvc.icc"
#endif
