/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileStager/TCopyChain.h"
#include "FileStager/TStageManager.h"
#include "TROOT.h"
#include "TError.h"
#include "TFile.h"
#include <iostream>

ClassImp(TCopyChain)

bool TCopyChain::_originalTChain=true;

TCopyChain::TCopyChain()
  : TChain ()
  , _releasedLastFile(false)
  , _bigNumber(1234567890) // big number like GetEntriesFast()
  , _curFile(0)
  , _prevFileName("")
  , _isNewFile(true)
{
}


TCopyChain::TCopyChain (const char* name,
                                    const char* title /*= ""*/)
  : TChain (name, title)
  , _releasedLastFile(false)
  , _bigNumber(1234567890) // big number like GetEntriesFast()
  , _curFile(0)
  , _prevFileName("")
  , _isNewFile(true)
{
}


TCopyChain::~TCopyChain()
{
  //this->TChain::~TChain();  
}


Long64_t TCopyChain::GetReadEntry() const
{
  Long64_t entry = TChain::GetReadEntry();
  if (_originalTChain) { return entry; }

  // FIXME: Check --- i think i saw that the behavior of
  // TChain::GetReadEntry changed in 5.16.
  if (fTreeNumber != -1) entry += fTreeOffset[fTreeNumber];
  return entry;
}


Long64_t TCopyChain::GetEntries() const
{
   if (_originalTChain) return TChain::GetEntries();
   else return _bigNumber; 
}


Long64_t TCopyChain::GetEntries(const char* sel)
{
   if (_originalTChain) return TChain::GetEntries(sel);
   else return _bigNumber; 
}


Long64_t TCopyChain::LoadTree(Long64_t entry)
{
  if (!_originalTChain && _curFile!=0) { 
    _prevFileName = _curFile->GetName();
  }

  Long64_t readEntry = this->TChain::LoadTree(entry);
  if (_originalTChain) return readEntry;

  // check if file has changed
  _curFile=TChain::GetCurrentFile(); 

  if (_curFile!=0) {
    if (_prevFileName!=_curFile->GetName()) {
      _isNewFile=true;
      std::cout << "TCopyChain::LoadTree() : Opened new file <" 
  	        << _curFile->GetName() 
	        << ">" 
	        << std::endl;
    } 
    else{ _isNewFile=false; }
  }
  else{ _isNewFile=true; }

  return readEntry;
}


TString TCopyChain::GetCurrentLocalFileName()
{
  if(_curFile!=0){
    TStageManager& manager(TStageManager::instance());
    return TString(manager.getTmpFilename(_curFile->GetName()));
  }
  return TString(0);
}

Int_t TCopyChain::Add(const char* name, Long64_t nentries) 
{
  if (_originalTChain) return TChain::Add(name, nentries); 

  TStageManager& manager(TStageManager::instance());
  manager.addToList(name);
  return this->TChain::Add(name, nentries); 
}

