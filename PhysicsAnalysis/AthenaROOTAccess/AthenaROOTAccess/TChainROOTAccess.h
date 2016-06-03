// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TChainROOTAccess.h,v 1.5 2009-01-11 20:53:49 ssnyder Exp $

/**
 * @file AthenaROOTAccess/TChainROOTAccess.h
 * @author scott snyder
 * @date Jul 2007
 * @brief Work around Root @c TChain::GetReadEntry problem.
 */


#ifndef ATHENAROOTACCESS_TCHAINROOTACCESS_H
#define ATHENAROOTACCESS_TCHAINROOTACCESS_H

#include "TChain.h"


namespace AthenaROOTAccess {


/**
 * @brief Work around Root @c TChain::GetReadEntry problem.
 *
 * @c TChain::GetReadEntry is supposed to return the current entry
 * in the chain.  Instead, though, for Root versions < 5.16, it returns
 * the entry in the current tree.  This causes friendship to malfunction,
 * when a persistent @c TChain is made a friend of the transient tree.
 * Using this class instead works around this problem.
 *
 * We can probably get rid of this once we've moved to 5.17.
 */
class TChainROOTAccess
  : public TChain
{
public:
  /**
   * @brief Default constructor.
   */
  TChainROOTAccess();


  // Destructor.
  virtual ~TChainROOTAccess();


  /**
   * @brief Constructor.
   * @param name The chain name.
   * @param title The chain title.
   */
  TChainROOTAccess (const char* name, const char* title = "");


  /**
   * @brief Return the current entry number of the chain.
   */
  virtual Long64_t  GetReadEntry() const override;


  /**
   * @brief Work around caching crash.
   */
  virtual Long64_t  LoadTree(Long64_t entry) override;


  /**
   * Call Reset() on all TChainROOTAccess instances.
   */
  static void ResetAll();


  ClassDefOverride (AthenaROOTAccess::TChainROOTAccess, 1);
};


} // namespace AthenaROOTAccess


/**
 * @brief Work around Root @c TChain::GetReadEntry problem.
 *
 * The preferred version of the class to use is the one in the namespace.
 * However, we advertised @c ::TChainROOTAccess earlier, so keep that working
 * for a while.
 */
class TChainROOTAccess
  : public AthenaROOTAccess::TChainROOTAccess
{
public:
  /**
   * @brief Constructor.
   * @param name The chain name.
   * @param title The chain title.
   */
  TChainROOTAccess (const char* name, const char* title = "");


  ClassDef (TChainROOTAccess, 1);

private:
  // Disallow copying.
  TChainROOTAccess& operator= (const TChainROOTAccess&);
  TChainROOTAccess (const TChainROOTAccess&);
};


#endif // ATHENAROOTACCESS_TCHAINROOTACCESS_H

