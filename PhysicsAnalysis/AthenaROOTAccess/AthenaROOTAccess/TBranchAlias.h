// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TBranchAlias.h,v 1.8 2008-12-23 05:01:34 ssnyder Exp $

/**
 * @file AthenaROOTAccess/TBranchAlias.h
 * @author scott snyder
 * @date Aug 2007
 * @brief Redirect queries to another branch.
 */

#ifndef ATHENAROOTACCESS_TBRANCHALIAS_H
#define ATHENAROOTACCESS_TBRANCHALIAS_H


#include "AthenaROOTAccess/IAuxBranches.h"
#include "GaudiKernel/ClassID.h"
#include "AthenaKernel/IStringPool.h"
#include "TBranch.h"
#include "TMethodCall.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <memory>
#include <unordered_map>
class TTree;
class TBranch;
class TClass;
class ITPCnvBase;


namespace SG {
class AuxStoreInternal;
}


namespace AthenaROOTAccess {


class TTreeTrans;
class AuxStoreARA;
class IARAFixup;


/**
 * @brief Redirect queries to another branch.
 *
 * This branch can be used when you want to refer to a branch by
 * an alternate name.  It redirects reads to the contents
 * of another branch (specified by tree and branch name).
 *
 * Note: writing to the branch is unimplemented and may behave strangely!
 */
class TBranchAlias
  : public TBranch, public IAuxBranches
{
public:
  /// Destructor.
  virtual ~TBranchAlias();


  /**
   * @brief Create a new TBranchAlias object and add it to a tree.
   * @param tree The tree to hold the new branch.
   * @param name The name of the new branch.
   * @param key The SG key for the object we're managing.
   * @param clid The primary SG clid.
   * @param targ_tree The tree containing the branch to be aliased.
   * @param targ_branchname The name of the branch being allocated.
   * @param aux_branchname Branch name of associated aux store, or
   *                       a null string if none.
   * @return The new branch.
   */
  static TBranchAlias* addToTree (TTreeTrans* tree,
                                  const char* name,
                                  const char* key,
                                  CLID clid,
                                  TTree* targ_tree,
                                  const char* targ_branchname,
                                  const char* aux_branchname = "");

  /**
   * @brief Standard Root method to read a new entry.
   * @param entry The entry number to be read.
   * @param getall If true, ignore branch disable flags.
   * @return The number of bytes read.
   */
  virtual Int_t GetEntry(Long64_t entry, Int_t getall = 0) override;


  /**
   * @brief Change the buffer address for this branch.
   * @param addr The new buffer address.  (Pointer-to-pointer to buffer.)
   *
   * This will be forwarded to the aliased branch.
   */
  virtual void SetAddress (void* addr) override;


  /**
   * @brief Root notification callback.
   *
   * We'll arrange for this to be called whenever we move to a new
   * file for the target tree.  (Only applicable if it's a chain.)
   */
  virtual Bool_t      Notify() override;

  virtual const char* GetClassName() const override;


  virtual std::vector<IAuxBranches::auxpair_t> auxBranches() const override;
  virtual TBranch* findAuxBranch (SG::auxid_t auxid) const override;
  virtual StatusCode readAuxBranch (TBranch& br, void* p, long long entry) override;


  /**
   * @brief Set a fixup to be run on this branch after GetEntry.
   * @param fixup The fixup to run.  (Takes ownership.)
   */
  void setFixup (IARAFixup* fixup);


  ClassDefOverride (AthenaROOTAccess::TBranchAlias, 1);


private:
  /**
   * @brief Fetch our cached branch address, if needed.
   * @return True on success; false if the branch couldn't be found.
   */
  bool checkBranch();


  /**
   * @brief Set up for reading aux data, if this type has any.
   */
  void setupAux();


  /**
   * @brief Look for an aux data branch.
   */
  void findAux();


  /**
   * @brief Set/clear the aux store for our object.
   * @param clear If true, clear the store; otherwise set it if possible.
   * @param entry Entry number being read.
   */
  void setStore (bool clear, long long entry);


  /**
   * @brief Constructor.
   * @param tree The transient tree.
   * @param name The name of the new branch.
   * @param key The SG key for the object we're managing.
   * @param clid The primary SG clid.
   * @param targ_tree The tree containing the branch to be aliased.
   * @param targ_branchname The name of the branch being allocated.
   * @param aux_branchname Branch name of associated aux store, or
   *                       a null string if none.
   */
  TBranchAlias (TTreeTrans* tree,
                const char* name,
                const char* key,
                CLID clid,
                TTree* targ_tree,
                const char* targ_branchname,
                const char* aux_branchname);

  /// The transient tree of which we're a part.
  TTreeTrans* m_trans_tree;

  /// The tree containing the aliased branch.
  TTree* m_targ_tree;

  /// The name of the aliased branch.
  std::string m_targ_branchname;

  /// The aliased branch.
  /// This is just a cache; we need to clear it
  /// if the target tree moves to a new file.
  TBranch* m_targ_branch;

  /// Address pointer.
  void* m_addrtmp;

  /// To hold any previously-registered notify routine.
  TObject* m_notify_chain;

  /// Hashed SG key/clid for this branch.
  IStringPool::sgkey_t m_sgkey;

  /// The name of the auxiliary store branch, or empty if none.
  std::string m_aux_branchname;

  /// Prefix for dynamic branches.
  std::string m_dyn_branch_prefix;

  /// The aux branch.
  TBranch* m_aux_branch;

  /// Dynamic aux branch manager.
  AuxStoreARA* m_dyn_store;

  /// The offset of the SG::IConstAuxStore in the aux object.
  int m_aux_offset;

  /// The offset of the SG::IAuxStoreHolder in the aux object.
  int m_holder_offset;

  /// To dynamically call @c setStore on the contained object.
  TMethodCall m_setStore_method;

  /// To dynamically call @c clearCache on the contained object.
  TMethodCall m_clearCache_method;

  /// Dynamic branches indexed by auxid.
  mutable std::unordered_map<SG::auxid_t, TBranch*> m_dyn_branch_map;

  /// Original branch being read.  May be different from m_aux_branch
  /// in the case of a conversion.
  TBranch* m_aux_branch_orig;
  int m_holder_offset_orig;

  /// Fixup to run after reading the branch, if any.
  std::unique_ptr<IARAFixup> m_fixup;

  std::string m_className;

  // Disallow copying
  TBranchAlias& operator= (const TBranchAlias&);
  TBranchAlias (const TBranchAlias&);
};


} // namespace AthenaROOTAccess


#endif // not TBRANCHALIAS_H

