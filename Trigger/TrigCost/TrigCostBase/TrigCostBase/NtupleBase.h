/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_NTUPLEBASE_H
#define ANP_NTUPLEBASE_H

//
// Helper classes to read/write ntuple data
//

// ROOT
#include "TTree.h"

// C/C++
#include <iostream>
#include <string>

namespace Anp
{
  //
  // State/type of ntuple branch
  //
  namespace Ntuple {
    enum Type { kNONE, kREAD, kFILL, kPASS, kFAIL };
  }

  //
  // Base class for ntuple branch manipulations
  //
  class NtupleBase {
  public:

    virtual ~NtupleBase() {}

    virtual TBranch* Branch(TTree *tree, const std::string &branch, bool fill_mode) = 0;
    
    virtual NtupleBase* Clone() = 0;

    virtual void Copy(NtupleBase *rhs) = 0;
    
    void SetEvent(Ntuple::Type t) { fEvent = t; }
    void SetState(Ntuple::Type t) { fState = t; }

    Ntuple::Type GetEvent() const { return fEvent; }
    Ntuple::Type GetState() const { return fState; }

    const std::string& GetType() const { return fType; }
    const std::string& GetName() const { return fName; }

  private:

    // These functions not implemented by design
    NtupleBase(NtupleBase &);
    NtupleBase &operator=(NtupleBase &);

  protected:

    NtupleBase() : fEvent(Ntuple::kNONE), fState(Ntuple::kNONE) {}  
    
    Ntuple::Type fEvent;       // Event type
    Ntuple::Type fState;       // State type 

    std::string  fType;        // Type T for NtupleData<T>
    std::string  fName;        // Branch name
  };

  //
  // Derived template class for ntuple branch manipulations
  //
  template <class T> class NtupleData : public NtupleBase {
    public:

    NtupleData(T *data, bool own_ptr);
    NtupleData(T *data, const std::string &dtype, bool own_ptr);
    virtual ~NtupleData();
    
    NtupleBase* Clone();

    void Copy(NtupleBase *rhs);

    T* GetData() const { return  fData; }
    T* GetPtr()  const { return  fData; }
    T& GetRef()  const { return *fData; }
    
    TBranch* Branch(TTree *tree, const std::string &branch, bool fill_mode);
    
    void SetType(const std::string &type) { fType = type; }

  private:

    TBranch* ActivateBranch(TTree *tree, const std::string &branch, bool fill_mode);
    
  private:

    T              *fData;       //! Smart pointer to owned data
    bool            fOwnPtr;     //! Do I own data pointer?
  };
}

//
// Inline and templated functions
//
//--------------------------------------------------------------------------------------
template<class T>
inline Anp::NtupleData<T>::NtupleData(T *data, bool own_ptr)
  : NtupleBase(), fData(data) , fOwnPtr(own_ptr)
{
}

//--------------------------------------------------------------------------------------
template<class T>
inline Anp::NtupleData<T>::NtupleData(T *data, const std::string &dtype, bool own_ptr)
  : NtupleBase(), fData(data) , fOwnPtr(own_ptr)
{
  fType = dtype;
}

//--------------------------------------------------------------------------------------
template<class T>
inline Anp::NtupleData<T>::~NtupleData()
{
  if(fData && fOwnPtr) { delete fData; fData = 0; fOwnPtr = false; }
}

//--------------------------------------------------------------------------------------
template<class T>
inline Anp::NtupleBase* Anp::NtupleData<T>::Clone()
{
  NtupleData<T> *data = new NtupleData<T>(new T, true);
  data -> fName = fName;
  data -> fType = fType;

  return data;
}

//--------------------------------------------------------------------------------------
template<class T>
inline void Anp::NtupleData<T>::Copy(NtupleBase *rhs)
{
  if(!rhs || !fData) return;

  NtupleData<T> *data = dynamic_cast<NtupleData<T> *>(rhs);
  if(!data) {
    std::cerr << "NtupleData<T>::Copy - failed to cast" << std::endl;
    return;
  }
  if(!(data->fData)) {
    std::cerr << "NtupleData<T>::Copy - null data pointer" << std::endl;
    return;
  }

  (*fData) = *(data->fData);
  fState = data->fState;
  fEvent = data->fEvent;
}

//--------------------------------------------------------------------------------------
template<class T>
inline TBranch* Anp::NtupleData<T>::Branch(TTree *tree,
					   const std::string &branch,
					   bool fill_mode)
{
  //
  // Make new branch or reset old branch
  //
  if(branch.empty()) {
    std::cerr << "NtupleData<T>::Branch - branch name is empty" << std::endl;
    fState = Ntuple::kNONE; return 0;
  }

  // Check/save branch name
  if(fName.empty()) {
    fName = branch;
  }
  else if(fName != branch) {
    std::cerr << "NtupleData<T>::Branch - branch mismatch: " 
	      << fName  << "!=" << branch << std::endl;
    fState = Ntuple::kNONE; return 0;
  }

  // Check data
  if(!fData) {
    std::cerr << "NtupleData<T>::Branch - null fData pointer" << std::endl;
    fState = Ntuple::kNONE; return 0;
  }

  // Current TTree is finished
  if(!tree) { 
    fState = Ntuple::kNONE; return 0;
  }
  
  TBranch *branch_ptr = ActivateBranch(tree, branch, fill_mode);
  if(!branch_ptr) { 
    std::cerr << "NtupleData<T>::Branch - null TBranch pointer" 
	      << " with tree \"" << tree -> GetName() << "\"" << std::endl;
    fState = Ntuple::kNONE;
  }
    
  return branch_ptr;
}

//-------------------------------------------------------------------------------------
template<class T>
inline TBranch* Anp::NtupleData<T>::ActivateBranch(TTree *tree, const std::string &branch, bool fill_mode)
{
  if(!tree) {
    std::cout << "NtupleData<T>::ActivateBranch - Invalid TTree pointer" << std::endl;
    return 0;
  
  }

  TBranch *branch_ptr = 0;

  if(fill_mode) {
    if(fType.empty()) {
      std::cerr << "NtupleData<T>::ActivateBranch - empty branch name" << std::endl;
      fState = Ntuple::kNONE; return 0;
    }
    else {
      branch_ptr = tree->Branch(branch.c_str(), fType.c_str(), &fData);
      fState = Ntuple::kFILL;
    }
  } else {
    tree->SetBranchStatus((branch+"*").c_str(), 1);
    tree->SetBranchAddress(branch.c_str(), &fData, &branch_ptr);
    fState = Ntuple::kREAD;
  }

  return branch_ptr;
}

//------------------------------------------------------------------------------------
// Template specializations for ActivateBranch
//
namespace Anp 
{
  template<>
    inline TBranch* NtupleData<int>::ActivateBranch(TTree *tree, const std::string &branch, bool fill_mode)
    {
      if(!tree) {
	std::cout << "NtupleData<int>::ActivateBranch - Invalid TTree pointer" << std::endl;
	return 0;
      }
      
      TBranch *branch_ptr = 0;
      
      if(fill_mode) {
	branch_ptr = tree->Branch(branch.c_str(), fType.c_str(), &(*fData));
	fState = Ntuple::kFILL;
      } else {
	tree->SetBranchStatus((branch+"*").c_str(), 1);
	tree->SetBranchAddress(branch.c_str(), &(*fData), &branch_ptr);
	fState = Ntuple::kREAD;
      }
      
      return branch_ptr;
    }


  template<>
    inline TBranch* NtupleData<unsigned int>::ActivateBranch(TTree *tree, const std::string &branch, bool fill_mode)
    {
      if(!tree) {
	std::cout << "NtupleData<int>::ActivateBranch - Invalid TTree pointer" << std::endl;
	return 0;
      }
      
      TBranch *branch_ptr = 0;
      
      if(fill_mode) {
	branch_ptr = tree->Branch(branch.c_str(), fType.c_str(), &(*fData));
	fState = Ntuple::kFILL;
      } else {
	tree->SetBranchStatus((branch+"*").c_str(), 1);
	tree->SetBranchAddress(branch.c_str(), &(*fData), &branch_ptr);
	fState = Ntuple::kREAD;
      }
      
      return branch_ptr;
    }

  template<>
    inline TBranch* NtupleData<float>::ActivateBranch(TTree *tree, const std::string &branch, bool fill_mode) 
    {
      if(!tree) {
	std::cout << "NtupleData<float>::ActivateBranch - Invalid TTree pointer" << std::endl;
	return 0;
      }
      
      TBranch *branch_ptr = 0;
      
      if(fill_mode) {
	branch_ptr = tree->Branch(branch.c_str(), fType.c_str(), &(*fData));
	fState = Ntuple::kFILL;
      } else {
	tree->SetBranchStatus((branch+"*").c_str(), 1);
	tree->SetBranchAddress(branch.c_str(), &(*fData), &branch_ptr);
	fState = Ntuple::kREAD;
      }
      
      return branch_ptr;
    }

  template<>
    inline TBranch* NtupleData<double>::ActivateBranch(TTree *tree, const std::string &branch, bool fill_mode)
    {
      if(!tree) {
	std::cout << "NtupleData<double>::ActivateBranch - Invalid TTree pointer" << std::endl;
	return 0;
      }
      
      TBranch *branch_ptr = 0;
      
      if(fill_mode) {
	branch_ptr = tree->Branch(branch.c_str(), fType.c_str(), &(*fData));
	fState = Ntuple::kFILL;
      } else {
	tree->SetBranchStatus((branch+"*").c_str(), 1);
	tree->SetBranchAddress(branch.c_str(), &(*fData), &branch_ptr);
	fState = Ntuple::kREAD;
      }
      
      return branch_ptr;
    }

  template<>
    inline TBranch* NtupleData<bool>::ActivateBranch(TTree *tree, const std::string &branch, bool fill_mode)
    {
      if(!tree) {
	std::cout << "NtupleData<double>::ActivateBranch - Invalid TTree pointer" << std::endl;
	return 0;
      }
      
      TBranch *branch_ptr = 0;
      
      if(fill_mode) {
	branch_ptr = tree->Branch(branch.c_str(), fType.c_str(), &(*fData));
	fState = Ntuple::kFILL;
      } else {
	tree->SetBranchStatus((branch+"*").c_str(), 1);
	tree->SetBranchAddress(branch.c_str(), &(*fData), &branch_ptr);
	fState = Ntuple::kREAD;
      }
      
      return branch_ptr;
    }
}

#endif
