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

bool TCopyChain::s_originalTChain=true;

TCopyChain::TCopyChain()
  : TChain ()
  , m_releasedLastFile(false)
  , m_bigNumber(1234567890) // big number like GetEntriesFast()
  , m_curFile(0)
  , m_prevFileName("")
  , m_isNewFile(true)
{
}


TCopyChain::TCopyChain (const char* name,
                                    const char* title /*= ""*/)
  : TChain (name, title)
  , m_releasedLastFile(false)
  , m_bigNumber(1234567890) // big number like GetEntriesFast()
  , m_curFile(0)
  , m_prevFileName("")
  , m_isNewFile(true)
{
}


TCopyChain::~TCopyChain()
{
  //this->TChain::~TChain();  
}


Long64_t TCopyChain::GetReadEntry() const
{
  Long64_t entry = TChain::GetReadEntry();
  if (s_originalTChain) { return entry; }

  // FIXME: Check --- i think i saw that the behavior of
  // TChain::GetReadEntry changed in 5.16.
  if (fTreeNumber != -1) entry += fTreeOffset[fTreeNumber];
  return entry;
}


Long64_t TCopyChain::GetEntries() const
{
   if (s_originalTChain) return TChain::GetEntries();
   else return m_bigNumber; 
}


Long64_t TCopyChain::GetEntries(const char* sel)
{
   if (s_originalTChain) return TChain::GetEntries(sel);
   else return m_bigNumber; 
}


Long64_t TCopyChain::LoadTree(Long64_t entry)
{
  if (!s_originalTChain && m_curFile!=0) { 
    m_prevFileName = m_curFile->GetName();
  }

  Long64_t readEntry = this->TChain::LoadTree(entry);
  if (s_originalTChain) return readEntry;

  // check if file has changed
  m_curFile=TChain::GetCurrentFile(); 

  if (m_curFile!=0) {
    if (m_prevFileName!=m_curFile->GetName()) {
      m_isNewFile=true;
      std::cout << "TCopyChain::LoadTree() : Opened new file <" 
  	        << m_curFile->GetName() 
	        << ">" 
	        << std::endl;
    } 
    else{ m_isNewFile=false; }
  }
  else{ m_isNewFile=true; }

  return readEntry;
}


TString TCopyChain::GetCurrentLocalFileName()
{
  if(m_curFile!=0){
    TStageManager& manager(TStageManager::instance());
    return TString(manager.getTmpFilename(m_curFile->GetName()));
  }
  return TString(0);
}

Int_t TCopyChain::Add(const char* name, Long64_t nentries) 
{
  if (s_originalTChain) return TChain::Add(name, nentries); 

  TStageManager& manager(TStageManager::instance());
  manager.addToList(name);
  return this->TChain::Add(name, nentries); 
}

