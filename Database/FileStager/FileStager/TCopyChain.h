// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TCOPYCHAIN_H
#define TCOPYCHAIN_H

#include <TROOT.h>
#include "TChain.h"
#include "TString.h"

class TFile;

class TCopyChain : public TChain
{
 public:
  TCopyChain();
  TCopyChain (const char* name, const char* title = "");
  virtual ~TCopyChain();

  virtual Long64_t  GetReadEntry() const;
  virtual Long64_t  GetEntries() const;
  virtual Long64_t  GetEntries(const char* sel);
  virtual Long64_t  LoadTree(Long64_t entry);

  virtual Int_t	Add(TChain* chain) { return TChain::Add(chain); }
  virtual Int_t	Add(const char* name, Long64_t nentries = kBigNumber);
  //virtual Int_t	AddFile(const char* name, Long64_t nentries = kBigNumber, const char* tname = "");

  static  void SetOriginalTChain(bool originalTChain=true) { s_originalTChain=originalTChain; }
  virtual bool IsNewFile(){ return m_isNewFile; }
  virtual TString GetCurrentLocalFileName();

 private:
  static bool s_originalTChain;
  bool m_releasedLastFile;
  int m_bigNumber;

  TFile* m_curFile; //!
  TString m_prevFileName;
  bool m_isNewFile;

  ClassDef(TCopyChain,1)
};

//bool TCopyChain::_originalTChain=true;

#endif

