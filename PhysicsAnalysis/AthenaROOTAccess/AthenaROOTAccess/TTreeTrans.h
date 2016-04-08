// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

/**
 * @file AthenaROOTAccess/TTreeTrans.h
 * @author scott snyder
 * @date Oct 2007
 * @brief Tree holding transient branches.
 */

#ifndef ATHENAROOTACCESS_TTREETRANS_H
#define ATHENAROOTACCESS_TTREETRANS_H

#include "AthenaROOTAccess/TTreeBranchMap.h"
#include "SGTools/IProxyDictWithPool.h"
#include "Rtypes.h"
#include <set>
#include <string>


class TTree;
class TBranch;
class TObject;
namespace SG {
class DataProxy;
}


namespace AthenaROOTAccess {


class ProxyMap;
class TTreeTransDeleter;


/**
 * @brief Tree holding transient branches.
 *
 * This is a @c TTree that holds the transient TBranchTPConvert
 * (and TBranchAlias) objects.  It derives from TTreeBranchMap,
 * which in turn derives from @c TTree.  TTreeBranchMap is responsible
 * for allowing one to find the appropriate transient branch
 * given a persistent branch name.  (This functionality is broken
 * out to avoid circular dependencies.)
 *
 * TTreeTrans also implements the @c IProxyDict interface,
 * which allows finding a @c DataProxy given a SG key and class ID.
 * This is used for @c ElementLink resolution.
 *
 * In order for ElementLink resolution to work, though, the TTreeTrans
 * object must be installed as the data source for the @c ElementLink.
 * One TTreeTrans is designated as the `current' one; for the ElementLink
 * to pick up the correct source, the TTreeTrans containing it should
 * be current at the time the @c ElementLink is read.  
 *
 * The current TTreeTrans is set by calling @c setCurTree().  The nested class
 * Push may be used to change the current tree in a stack-like manner.
 *
 * Some objects in the pool file live outside the primary CollectionTree,
 * in separate tree objects.  These objects may also need to have
 * P->T conversions run.  In such a case, we build a new TTreeTrans
 * object for each of these external persistent trees; these are
 * called `aux' trees.  They are collected in a special directory,
 * so that they can be found.  These `aux' trees
 * have pointer to the `primary' persistent tree; the presence of this
 * pointer is what identifies such a tree.
 */
class TTreeTrans
  : public TTreeBranchMap,
    public IProxyDictWithPool
{
public:
  //========================================================================
  /** @name Constructors, destructors, and TTreeTrans unique interface. */
  //@{


  /**
   * @brief Constructor.
   * @param pers_tree The tree containing the persistent data.
   * @param primary The primary tree, or null of this tree is primary.
   * @param pers_primary The primary persistent tree.
   * @param name The tree name.
   * @param title The tree  title.
   */
  TTreeTrans (TTree* pers_tree,
              TTreeTrans* primary,
              TTree* pers_primary,
              const char* name,
              const char* title = "");


  /**
   * @brief Destructor.
   */
  virtual ~TTreeTrans() override;


  /**
   * @brief Note the current entry number.
   * @param entry The entry number.
   *
   * This is needed when we resolve @c ElementLink's --- this sets the
   * entry number we'll use in the referenced branch.
   */
  void setEntry (Long64_t entry);


  /**
   * @brief Format the contents of the contained proxy map as a string.
   * (For testing purposes.)
   */
  std::string dumpProxyMap();


  /**
   * @brief Called when a branch has moved to a new entry.
   *        Clears any cached data.
   * @param sgkey Hashed SG key/clid for the branch.
   */
  void resetBranch (sgkey_t sgkey);


  /**
   * @brief Set the current tree.
   * @param tree The new current tree, or 0 for none.
   * @return The previous current tree, or 0 for none.
   *
   * The current tree is the one which gets installed as the source
   * when @c ElementLink's are constructed.
   */
  static IProxyDictWithPool* setCurTree (IProxyDictWithPool* tree);


  /**
   * @brief Helper to manage changes in the current tree.
   */
  class Push
  {
  public:
    /// Change the current tree to @a t and save the previous current tree.
    Push (TTreeTrans* t);

    /// Restore the previous current tree.
    ~Push();

  private:
    /// The previous current tree.
    IProxyDictWithPool* m_saved;
  };


  /**
   * @brief Test to see if we've already seen @a file.
   * @param file The file to test.
   *
   * The first time we're called for a given file name, return false.
   * On subsequent calls with that name, return true.
   */
  bool sawFile (TFile* file);


  /**
   * @brief Return the associated persistent tree.
   */
  TTree* getPersTree() const;


  //@}
  //========================================================================
  /** @name TTree interface. */
  //@{


  /**
   * @brief Standard Root method to read a new entry.
   * @param entry The entry number to be read.
   * @param getall If true, ignore branch disable flags.
   * @return The number of bytes read.
   */
  virtual Int_t  GetEntry(Long64_t entry, Int_t getall = 0) override;


  /**
   * @brief Standard Root notify hook.
   *
   * This will be called when the persistent tree changes files.
   */
  virtual Bool_t Notify() override;


  /**
   * @brief Clear references to all persistent branches.
   */
  void resetBranches();


  /**
   * @brief Set current entry; return local entry.
   */
  virtual Long64_t LoadTree(Long64_t entry) override;


  //@}
  //========================================================================
  /** @name IProxyDictWithPool interface. */
  //@{


  /**
   * @brief Get proxy with given id and key. Returns 0 to flag failure.
   * @param id The class ID of the object to find.
   * @param key The StoreGate key of the object to find.
   * @return The proxy, or 0 on failure.
   */
  virtual
  SG::DataProxy* proxy(const CLID& id, const std::string& key) const override;


  /**
   * @brief Get proxy for an object at a given address.
   * @param pTransient Pointer to the object.
   * @return The proxy, or 0 on failure.
   */
  virtual SG::DataProxy* proxy(const void* const pTransient) const override;


  /**
   * @brief Get proxy given a hashed key+clid.
   * @param sgkey Hashed key to look up.
   * @return The proxy, or 0 on failure.
   */
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t sgkey) const override;


  /**
   * @brief Return the list of all proxies
   * @return The proxies
   *
   * This shouldn't be used from AthenaROOTAccess, and is unimplemented.
   */
  std::vector<const SG::DataProxy*> proxies() const override;


  /**
   * @brief Raw addition of a proxy to the store.
   * @param id CLID of object being added.
   * @param proxy proxy to add.
   */
  virtual StatusCode addToStore (CLID id, SG::DataProxy* proxy) override;


  /**
   * @brief Record an object in the store.
   * @param obj The data object to store.
   * @param key The key as which it should be stored.
   * @param allowMods If false, the object will be recorded as const.
   * @param returnExisting If true, return proxy if this key already exists.
   *
   * This shouldn't be used from AthenaROOTAccess, and is unimplemented.
   */
  virtual SG::DataProxy* recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                                       const std::string& key,
                                       bool allowMods,
                                       bool returnExisting) override;


  /**
   * @brief Inform HIVE that an object has been updated.
   * @param id The CLID of the object.
   * @param key The key of the object.
   *
   * This shouldn't be used from AthenaROOTAccess, and is unimplemented.
   */
  virtual
  StatusCode updatedObject (CLID id, const std::string& key) override;


  /**
   * @brief Find the key for a string/CLID pair.
   * @param str The string to look up.
   * @param clid The CLID associated with the string.
   * @return A key identifying the string.
   *         A given string will always return the same key.
   *         Will abort in case of a hash collision!
   */
  virtual
  sgkey_t stringToKey (const std::string& str, CLID clid) override;


  /**
   * @brief Find the string corresponding to a given key.
   * @param key The key to look up.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key) const override;


  /**
   * @brief Find the string and CLID corresponding to a given key.
   * @param key The key to look up.
   * @param clid[out] The found CLID.
   * @return Pointer to the string found, or null.
   *         We can find keys as long as the corresponding string
   *         was given to either @c stringToKey() or @c registerKey().
   */
  virtual
  const std::string* keyToString (sgkey_t key,
                                  CLID& clid) const override;


  /**
   * @brief Remember an additional mapping from key to string/CLID.
   * @param key The key to enter.
   * @param str The string to enter.
   * @param clid The CLID associated with the string.
   * @return True if successful; false if the @c key already
   *         corresponds to a different string.
   *
   * This registers an additional mapping from a key to a string;
   * it can be found later through @c lookup() on the string.
   * Logs an error if @c key already corresponds to a different string.
   */
  virtual
  void registerKey (sgkey_t key,
                    const std::string& str,
                    CLID clid) override;


  //@}
  //========================================================================
  /** @name Forward redirection interface from @c ProxyMap. */
  //@{

  
  /**
   * @brief Add a new redirection.
   * @param dbid GUID string of the database.
   * @param container Name of the container.
   * @param offset Offset of the first entry for this DB.
   */
  void addRedirect (const std::string& dbid,
                    const std::string& container,
                    Long64_t offset);


  /**
   * @brief Look up a redirection.
   * @param dbid GUID string of the database.
   * @param container Name of the container.
   * @returns The offset of the first entry for this DB.
   */
  Long64_t getRedirect (const std::string& dbid,
                        const std::string& container) const;


  /**
   * @brief Clear all redirections.
   */
  void clearRedirect();


  /**
   * @brief Read the Links table for the current file.
   */
  void readLinks();


  /**
   * @brief Add a new container string for OID from the Links table.
   * @param token Token string of the link.
   */
  void addLink(const std::string& token, Long64_t oid);


  /**
   * @brief Return the container string for OID from the Links table.
   */
  std::string getLink (Long64_t oid);


  /**
   * @brief Return the container string for OID from TOKEN
   *        from the Links table.
   */
  std::string getLink (const std::string& token);


  /**
   * @brief Clear all links.
   */
  void clearLinks();


  //@}


private:
  /// Boilerplate to fill out @c INamedInterface interface.
  virtual const std::string& name() const override;
  /// Boilerplate to fill out @c INamedInterface interface.
  virtual StatusCode queryInterface(const InterfaceID& riid,
                                    void** ppvInterface) override;
  /// Boilerplate to fill out @c INamedInterface interface.
  virtual unsigned long addRef() override;
  /// Boilerplate to fill out @c INamedInterface interface.
  virtual unsigned long release() override;
  

  ClassDefOverride (AthenaROOTAccess::TTreeTrans, 1);


private:
  friend class TTreeTransDeleter;

  /// The current entry number;
  Long64_t m_entry;

  /// The proxy map object.  If not primary, this points at the primary's.
  ProxyMap* m_proxyMap;

  /// The persistent tree.
  TTree* m_pers_tree;

  /// For forwarding notifications.
  TObject* m_notify_chain;

  /// Pointer to our primary tree, or null if this tree is the primary.
  TTreeTrans* m_primary_tree;

  /// Set of files given to @c sawFile.
  std::set<std::string> m_filesSeen;

  // Don't allow copies.
  TTreeTrans (const TTreeTrans&);
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_TTREETRANS_H
