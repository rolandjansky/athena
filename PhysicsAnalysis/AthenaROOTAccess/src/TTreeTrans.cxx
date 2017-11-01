/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/TTreeTrans.cxx
 * @author scott snyder
 * @date Oct 2007
 * @brief Tree holding transient branches.
 */

#include "AthenaROOTAccess/TTreeTrans.h"
#include "AthenaROOTAccess/ProxyMap.h"
#include "SGTools/CurrentEventStore.h"
#include "AthLinks/tools/DataProxyHolder.h"
#include "PersistentDataModel/Token.h"
#include "RootUtils/TBranchElementClang.h"
#include "TError.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TPython.h"
#include "TFile.h"
#include "TVirtualCollectionProxy.h"


namespace AthenaROOTAccess {


//**************************************************************************
// TBranchElement leaks collection proxies.
// Try to fix that here.
//

class CleanProxies
  : public TBranchElement
{
public:
  void doclean (TVirtualCollectionProxy* topproxy);
  static void cleaner (TTree* t);
};


void CleanProxies::doclean (TVirtualCollectionProxy* topproxy)
{
  TVirtualCollectionProxy* todel = 0;
  if (fCollProxy != topproxy) {
    topproxy = fCollProxy;
    todel = fCollProxy;
  }
  fCollProxy = 0;

  Int_t nbranches = fBranches.GetEntries();
  for (Int_t i = 0; i < nbranches; ++i) {
    TBranchElement* bre = dynamic_cast<TBranchElement*>(fBranches.At(i));
    if (bre)
      reinterpret_cast<CleanProxies*>(bre)->doclean (topproxy);
  }
  if (todel)
    delete todel;
}


class TreeCleaner
  : public TObject
{
public:
  /// Constructor.
  TreeCleaner (TTree* tree);

  /// Destructor.
  ~TreeCleaner ();

  TTree* m_tree;
};


/// Constructor.
TreeCleaner::TreeCleaner (TTree* tree)
  : m_tree (tree)
{
}


TreeCleaner::~TreeCleaner ()
{
  TIter bit (m_tree->GetListOfBranches());
  while (TObject* br = bit()) {
    if (TBranchElement* be = dynamic_cast<TBranchElement*> (br)) {
      reinterpret_cast<CleanProxies*>(be)->doclean (0);
    }
  }
}


//*************************************************************************


/**
 * @brief Constructor.
 * @param pers_tree The tree containing the persistent data.
 * @param primary The primary tree, or null of this tree is primary.
 * @param pers_primary The primary persistent tree.
 * @param name The tree name.
 * @param title The tree title.
 */
TTreeTrans::TTreeTrans (TTree* pers_tree,
                        TTreeTrans* primary,
                        TTree* /*pers_primary*/,
                        const char* name,
                        const char* title /*= ""*/)
  : TTreeBranchMap (name, title),
    m_entry (-1),
    m_proxyMap (primary? primary->m_proxyMap : new ProxyMap (this, pers_tree)),
    m_pers_tree (pers_tree),
    m_notify_chain (pers_tree->GetNotify()),
    m_primary_tree (primary)
{
  // Receive notifications from the persistent tree.
  pers_tree->SetNotify (this);

  // Set up to clean up proxies on the persistent tree when it gets deleted.
  m_pers_tree->GetUserInfo()->Add (new TreeCleaner (m_pers_tree) );
}


/**
 * @brief Destructor.
 */
TTreeTrans::~TTreeTrans()
{
  if (!m_primary_tree)
    delete m_proxyMap;

  if (SG::CurrentEventStore::store() == this)
    SG::CurrentEventStore::setStore(0);
}


/**
 * @brief Note the current entry number.
 * @param entry The entry number.
 *
 * This is needed when we resolve @c ElementLink's --- this sets the
 * entry number we'll use in the referenced branch.
 */
void TTreeTrans::setEntry (Long64_t entry)
{
  m_entry = entry;
}


/**
 * @brief Format the contents of the contained proxy map as a string.
 * (For testing purposes.)
 */
std::string TTreeTrans::dumpProxyMap()
{
  return m_proxyMap->dump();
}


/**
 * @brief Called when a branch has moved to a new entry.
 *        Clears any cached data.
 * @param sgkey Hashed SG key/clid for the branch.
 */
void TTreeTrans::resetBranch (sgkey_t sgkey)
{
  return m_proxyMap->resetBranch (sgkey);
}


/**
 * @brief Set the current tree.
 * @param tree The new current tree, or 0 for none.
 * @return The previous current tree, or 0 for none.
 *
 * The current tree is the one which gets installed as the source
 * when @c ElementLink's are constructed.
 */
IProxyDict* TTreeTrans::setCurTree (IProxyDict* tree)
{
  return SG::CurrentEventStore::setStore (tree);
}


/**
 * @brief Change the current tree to @a t and save the previous current tree.
 * @param t The new current tree.
 */
TTreeTrans::Push::Push (TTreeTrans* t)
  : m_saved (TTreeTrans::setCurTree (t->m_primary_tree? t->m_primary_tree : t))
{
}


/**
 * @brief Destructor.
 */
TTreeTrans::Push::~Push()
{
  TTreeTrans::setCurTree (m_saved);
}


/**
 * @brief Test to see if we've already seen @a file.
 * @param file The file to test.
 *
 * The first time we're called for a given file name, return false.
 * On subsequent calls with that name, return true.
 */
bool TTreeTrans::sawFile (TFile* file)
{
  if (!file)
    return true;
  const char* name = file->GetName();
  std::set<std::string>::iterator i = m_filesSeen.find (name);
  if (i != m_filesSeen.end())
    return true;
  m_filesSeen.insert (name);
  return false;
}


/**
 * @brief Return the associated persistent tree.
 */
TTree* TTreeTrans::getPersTree() const
{
  return m_pers_tree;
}



/**
 * @brief Standard Root method to read a new entry.
 * @param entry The entry number to be read.
 * @param getall If true, ignore branch disable flags.
 * @return The number of bytes read.
 */
Int_t TTreeTrans::GetEntry (Long64_t entry, Int_t getall /*= 0*/)
{
  // We need to make sure that this instance is set as the current tree.
  Push save_tree (this);

  setEntry (entry);

  // Read pers tree first.
  Int_t nbytes = m_pers_tree->GetEntry (entry);
  nbytes += TTree::GetEntry (entry, getall);
  return nbytes;
}


/**
 * @brief Standard Root notify hook.
 *
 * This will be called when the persistent tree changes files.
 */
Bool_t TTreeTrans::Notify()
{
  resetBranches();

  // The first time we see a file, we need to look for any new
  // external trees.  And we need to rebuild the redirect table every time.
  if (!m_primary_tree) {
    TFile* file = m_pers_tree->GetCurrentFile();
    TPython::Exec ("from AthenaROOTAccess.transientTree import"
                   " _collect_redirections as "
                   "_ARAINTERNAL_collect_redirections");
    TPython::Bind (this, "_ARAINTERNAL_trans_tree");
    TPython::Bind (m_pers_tree, "_ARAINTERNAL_pers_tree");
    TPython::Exec ("_ARAINTERNAL_collect_redirections ("
                   "_ARAINTERNAL_trans_tree, _ARAINTERNAL_pers_tree)");

    if (!sawFile (file)) {
      TPython::Exec ("from AthenaROOTAccess.transientTree import"
                     " _handle_other_aux_trees as "
                     "_ARAINTERNAL_handle_other_aux_trees");
      TPython::Exec ("_ARAINTERNAL_handle_other_aux_trees ("
                     "_ARAINTERNAL_trans_tree, _ARAINTERNAL_pers_tree)");
      TPython::Exec ("del _ARAINTERNAL_handle_other_aux_trees");
    }

    TPython::Exec ("del _ARAINTERNAL_collect_redirections, "
                   "_ARAINTERNAL_trans_tree, _ARAINTERNAL_pers_tree");
  }

  // Also need to rebuild the redirect table...
  {
  }

  // Forward the notification, if needed.
  TObject* tnotify = this->GetNotify();
  if (tnotify) tnotify->Notify();
  if (m_notify_chain) m_notify_chain->Notify();
  return true;
}


/**
 * @brief Clear references to all persistent branches.
 */
void TTreeTrans::resetBranches()
{
  // Reset the addresses.
  if (!m_primary_tree)
    m_proxyMap->reset();

  TIter bit (this->GetListOfBranches());
  while (TObject* br = bit()) {
    if (TBranch* tpbr = dynamic_cast<TBranch*> (br)) {
      tpbr->Reset();
    }
  }
}


/**
 * @brief Set current entry; return local entry.
 */
Long64_t TTreeTrans::LoadTree(Long64_t entry)
{
  TTree::LoadTree(entry);
  return m_pers_tree->LoadTree(entry);
}


/**
 * @brief Get proxy with given id and key. Returns 0 to flag failure.
 * @param id The class ID of the object to find.
 * @param key The StoreGate key of the object to find.
 * @return The proxy, or 0 on failure.
 */
SG::DataProxy*
TTreeTrans::proxy(const CLID& id, const std::string& key) const
{
  // Look it up in the proxy map.
  return m_proxyMap->getProxy (key, id, m_entry);
}


/**
 * @brief Get proxy given a hashed key+clid.
 * @param sgkey Hashed key to look up.
 * @return The proxy, or 0 on failure.
 */
SG::DataProxy*
TTreeTrans::proxy_exact (SG::sgkey_t sgkey) const
{
  // Look it up in the proxy map.
  return m_proxyMap->getProxy (sgkey, m_entry);
}


/**
 * @brief Get proxy for an object at a given address.
 * @param pTransient Pointer to the object.
 * @return The proxy, or 0 on failure.
 *
 * This shouldn't be used from AthenaROOTAccess, and is unimplemented.
 */
SG::DataProxy*
TTreeTrans::proxy(const void* const pTransient) const
{
  return m_proxyMap->proxy (pTransient);
}


/**
 * @brief Return the list of all proxies
 * @return The proxies
 *
 * This shouldn't be used from AthenaROOTAccess, and is unimplemented.
 */
std::vector<const SG::DataProxy*> 
TTreeTrans::proxies() const
{
  std::abort();
}


/**
 * @brief Raw addition of a proxy to the store.
 * @param id CLID of object being added.
 * @param proxy proxy to add.
 */
StatusCode
TTreeTrans::addToStore (CLID id, SG::DataProxy* proxy)
{
  return m_proxyMap->addToStore (id, proxy);
}


/**
 * @brief Record an object in the store.
 * @param obj The data object to store.
 * @param key The key as which it should be stored.
 * @param allowMods If false, the object will be recorded as const.
 * @param returnExisting If true, return proxy if this key already exists.
 *
 * This shouldn't be used from AthenaROOTAccess, and is unimplemented.
 */
SG::DataProxy*
TTreeTrans::recordObject (SG::DataObjectSharedPtr<DataObject> /*obj*/,
                          const std::string& /*key*/,
                          bool /*allowMods*/,
                          bool /*returnExisting*/)
{
  std::abort();
}


/**
 * @brief Inform HIVE that an object has been updated.
 * @param id The CLID of the object.
 * @param key The key of the object.
 *
 * This shouldn't be used from AthenaROOTAccess, and is unimplemented.
 */
StatusCode TTreeTrans::updatedObject (CLID /*id*/, const std::string& /*key*/)
{
  std::abort();
}


/**
 * @brief Find the key for a string/CLID pair.
 * @param str The string to look up.
 * @param clid The CLID associated with the string.
 * @return A key identifying the string.
 *         A given string will always return the same key.
 *         Will abort in case of a hash collision!
 */
TTreeTrans::sgkey_t TTreeTrans::stringToKey (const std::string& str, CLID clid)
{
  return m_proxyMap->stringPool().stringToKey (str, clid);
}


/**
 * @brief Find the string corresponding to a given key.
 * @param key The key to look up.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* TTreeTrans::keyToString (sgkey_t key) const
{
  const std::string* ret =  m_proxyMap->stringPool().keyToString (key);
  if (!ret && m_proxyMap->maybeFillProxyMap (m_entry))
    ret =  m_proxyMap->stringPool().keyToString (key);
  return ret;
}


/**
 * @brief Find the string and CLID corresponding to a given key.
 * @param key The key to look up.
 * @param clid[out] The found CLID.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* TTreeTrans::keyToString (sgkey_t key, CLID& clid) const
{
  const std::string* ret =  m_proxyMap->stringPool().keyToString (key, clid);
  if (!ret && m_proxyMap->maybeFillProxyMap (m_entry))
    ret =  m_proxyMap->stringPool().keyToString (key, clid);
  return ret;
}


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
void TTreeTrans::registerKey (sgkey_t key,
                              const std::string& str,
                              CLID clid)
{
  m_proxyMap->stringPool().registerKey (key, str, clid);
}


/**
 * @brief Add a new redirection.
 * @param dbid GUID string of the database.
 * @param container Name of the container.
 * @param offset Offset of the first entry for this DB.
 */
void TTreeTrans::addRedirect (const std::string& dbid,
                              const std::string& container,
                              Long64_t offset)
{
  m_proxyMap->addRedirect (dbid, container, offset);
}


/**
 * @brief Look up a redirection.
 * @param dbid GUID string of the database.
 * @param container Name of the container.
 * @returns The offset of the first entry for this DB.
 */
Long64_t TTreeTrans::getRedirect (const std::string& dbid,
                                  const std::string& container) const
{
  return m_proxyMap->getRedirect (dbid, container);
}


/**
 * @brief Clear all redirections.
 */
void TTreeTrans::clearRedirect()
{
  m_proxyMap->clearRedirect();
}

void TTreeTrans::readLinks()
{
  m_proxyMap->readLinks();
}

void TTreeTrans::addLink(const std::string& token, Long64_t oid)
{
  return m_proxyMap->addLink (token, oid);
}

std::string TTreeTrans::getLink (Long64_t oid)
{
  return m_proxyMap->getLink (oid);
}

std::string TTreeTrans::getLink (const std::string& token)
{
  Token* tokPtr = new Token();
  tokPtr->fromString(token);
  std::string cont = getLink (tokPtr->oid().first);
  delete tokPtr; tokPtr = 0;
  return cont;
}

void TTreeTrans::clearLinks()
{
  m_proxyMap->clearLinks();
}



/// Boilerplate to fill out @c INamedInterface interface.
const std::string& TTreeTrans::name() const { std::abort(); }
StatusCode TTreeTrans::queryInterface(const InterfaceID& /*riid*/,
                               void** /*ppvInterface*/) { std::abort(); }
unsigned long TTreeTrans::addRef() { std::abort(); }
unsigned long TTreeTrans::release() { std::abort(); }


} // namespace AthenaROOTAccess
