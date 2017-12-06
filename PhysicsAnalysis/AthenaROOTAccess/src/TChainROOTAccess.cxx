/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TChainROOTAccess.cxx,v 1.4 2009-01-11 20:53:50 ssnyder Exp $
/**
 * @file AthenaROOTAccess/src/TChainROOTAccess.cxx
 * @author scott snyder
 * @date Jul 2007
 * @brief Work around Root @c TChain::GetReadEntry problem.
 */


#include "AthenaROOTAccess/TChainROOTAccess.h"
#include "TROOT.h"
#include "TError.h"
#include "TTreeCache.h"
#include "TFile.h"
#include <set>


namespace AthenaROOTAccess {


// Keep track of extant chains.
std::set<TChainROOTAccess*> s_allchains;


/**
 * @brief Default constructor.
 */
TChainROOTAccess::TChainROOTAccess()
{
  s_allchains.insert (this);
}


/**
 * @brief Destructor.
 */
TChainROOTAccess::~TChainROOTAccess()
{
  s_allchains.erase (this);
}


/**
 * @brief Constructor.
 * @param name The chain name.
 * @param title The chain title.
 */
TChainROOTAccess::TChainROOTAccess (const char* name,
                                    const char* title /*= ""*/)
  : TChain (name, title)
{
  s_allchains.insert (this);
}


/**
 * @brief Return the current entry number of the chain.
 */
Long64_t TChainROOTAccess::GetReadEntry() const
{
  Long64_t entry = TChain::GetReadEntry();

  // Work around the root TChain::GetReadEntry bug.
  // Note that this is fixed in later root versions, so check
  // that we actually need to fix it.
  if (fTreeNumber != -1 && entry >= 0 && entry < fTreeOffset[fTreeNumber])
    entry += fTreeOffset[fTreeNumber];

  return entry;
}


/**
 * Call Reset() on all TChainROOTAccess instances.
 */
void TChainROOTAccess::ResetAll()
{
  for (std::set<TChainROOTAccess*>::iterator i = s_allchains.begin();
       i != s_allchains.end();
       ++i)
    (*i)->Reset();
}


/**
 * @brief Work around caching crash.
 */
Long64_t  TChainROOTAccess::LoadTree(Long64_t entry)
{
  if (fFile && fTree) {
    TTreeCache* c = dynamic_cast<TTreeCache*>(fFile->GetCacheRead (fTree));
    if (c) {
      c->ResetCache();
      fFile->SetCacheRead (nullptr, fTree);
    }
  }
  return TChain::LoadTree (entry);
}

} // namespace AthenaROOTAccess


/**
 * @brief Constructor.
 * @param name The chain name.
 * @param title The chain title.
 */
TChainROOTAccess::TChainROOTAccess (const char* name,
                                    const char* title /*= ""*/)
  : AthenaROOTAccess::TChainROOTAccess (name, title)
{
  ::Warning ("TChainROOTAccess",
             "::TChainROOTAccess is deprecated; "
             "use AthenaROOTAccess::TChainROOTAccess instead.");
}
