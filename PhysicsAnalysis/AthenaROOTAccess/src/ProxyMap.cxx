/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ProxyMap.cxx,v 1.29 2009-05-02 05:01:29 ssnyder Exp $
/**
 * @file AthenaROOTAccess/src/ProxyMap.cxx
 * @author scott snyder
 * @date Apr 2007
 * @brief This class maintains the map of (key,clid) to data proxy
 *        instances for a transient tree.  This is used to resolve
 *        @c ElementLink references.
 */

#include "AthenaROOTAccess/ProxyMap.h"
#include "AthenaROOTAccess/DataBucketVoid.h"
#include "AthenaROOTAccess/ISetSGKey.h"
#include "AthenaROOTAccess/branchSeek.h"
#include "AthenaROOTAccess/TBranchAlias.h"
#include "AthenaROOTAccess/TBranchTPConvert.h"
#include "PersistentDataModelTPCnv/DataHeader_p3.h"
#include "PersistentDataModelTPCnv/DataHeader_p4.h"
#include "PersistentDataModelTPCnv/DataHeader_p5.h"
#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p5.h"
#include "SGTools/DataProxy.h"
#include "SGTools/BaseInfo.h"
#include "RootUtils/TBranchElementClang.h"
#include "TError.h"
#include "TBranch.h"
#include "TBranchObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include <string>
#include <sstream>
#include <cassert>


static const Guid s_localDb("00000000-0000-0000-0000-000000000000");

namespace AthenaROOTAccess { 


/// An invalid payload, for default key handling.
ProxyMap::payload_t ProxyMap::s_invalid_data;


/**
 * @brief Constructor.
 * @param trans_tree The transient tree.
 * @param pers_tree The persistent tree.
 */
ProxyMap::ProxyMap (AthenaROOTAccess::TTreeBranchMap* trans_tree,
                    TTree* pers_tree)
  : m_trans_tree (trans_tree),
    m_trans_tree_dict (dynamic_cast<IProxyDict*> (trans_tree)),
    m_pers_tree (pers_tree),
    m_dh_br (0),
    m_dh_ver (0),
    m_links_map (),
    m_lastfillentry (-1)
{
  assert (m_trans_tree_dict != 0);
}


/**
 * @brief Destructor.
 */
ProxyMap::~ProxyMap()
{
  // Delete the proxies and payloads.
  for (size_t i = 0; i < m_payloads.size(); i++) {
    delete m_payloads[i]->m_proxy;
    delete m_payloads[i];
  }
}


/**
 * @brief Look up a proxy.
 * @param key The SG key.
 * @param id The class ID.
 * @param entry The entry number in the tree.
 * @return Pointer to a proxy to the object, or 0 if not found.
 *
 * If needed, this will read through the data header to create new proxies.
 */
SG::DataProxy*
ProxyMap::getProxy (const std::string& key, unsigned int id, Long64_t entry)
{
  return getProxy (m_pool.stringToKey (key, id), entry);
}


/**
 * @brief Look up a proxy.
 * @param sgkey The hashed key+CLID.
 * @param entry The entry number in the tree.
 * @return Pointer to a proxy to the object, or 0 if not found.
 *
 * If needed, this will read through the data header to create new proxies.
 */
SG::DataProxy*
ProxyMap::getProxy (sgkey_t sgkey, Long64_t entry)
{
  // Look it up in our map.
  proxymap_t::iterator i = m_proxymap.find (sgkey);

  if (i == m_proxymap.end() && maybeFillProxyMap (entry)) {
    // We didn't find it.  Try scanning the data header for new proxies.
    // This is slow, though, so only do it if the entry number has changed.
    i = m_proxymap.find (sgkey);
  }

  // Return failure if we didn't find it.
  if (i == m_proxymap.end() || i->second == &s_invalid_data)
    return 0;

  // This is a little faster than referencing the iterator each time.
  payload_t& data = *i->second;

  // Test for a dummy proxy.
  if (data.m_proxy != 0 && data.m_treename.empty())
    return data.m_proxy;

  // Now we need to fetch any cached data, and make sure we're pointing
  // at the correct element.

  // Find the tree containing the target branch.
  // If we haven't fetched it yet, we have to look it up by name
  // (in the same directory as the persistent tree).
  TTree* tree = data.m_tree;
  if (tree == 0) {
    tree = dynamic_cast<TTree*> (m_pers_tree->GetTree()->GetDirectory()->
                                 Get(data.m_treename.c_str()));
    if (!tree) {
      ::Error ("Proxymap", "Cannot find tree %s",
               data.m_treename.c_str());
      return 0;
    }
  }

  // Now find the branch.
  TBranch* br = data.m_branch;
  if (br == 0) {
    // We didn't have the branch cached --- look it up by name in the tree.
    br = tree->GetBranch (data.m_branchname.c_str());
    if (br == 0) {
      ::Error  ("ProxyMap", "Cannot find branch %s in tree %s",
                data.m_branchname.c_str(), tree->GetName());
      return 0;
    }
    data.m_branch = br;

    // Create or update the proxy itself.
    data.m_proxy = makeProxy (data.m_proxy,
                              br,
                              data.m_clid,
                              data.m_altclids,
                              data.m_key,
                              sgkey);
  }

  // ??? Should we defer the rest until the proxy is dereferenced?

  // We now need to find the proper entry in the branch.
  Long64_t local_entry;
  if (data.m_poolcontainer) {
    // In the case of an external tree, the entry won't necessarily
    // correspond to the entry in the transient tree (a given transient
    // tree entry may point to 0 or more entries in the external tree).
    // We need to look up the entry in the data header.
    // Since this is a bit slow, we cache the previous result.
    if (data.m_last_trans_entry != entry) {
      // We have to do the data header lookup.
      Long64_t pent = poolContainerEntry (entry,
                                          data.m_key, data.m_clid);
      if (pent < 0) {
        ::Error ("ProxyMap",
                 "Can't find persistent entry for %s %s %d\n",
                 tree->GetName(), data.m_key.c_str(), data.m_clid);
        return 0;
      }

      // Save in the cache.
      data.m_last_trans_entry = entry;
      data.m_last_pers_entry = pent;
    }

    // Use the cached entry.
    local_entry = data.m_last_pers_entry;
  }
  else {
    // In the case of a TChain, need to translate to the local entry index.
    local_entry = tree->LoadTree (entry);
  }

  // Seek in the branch to the proper entry.
  {
    Long64_t ent = local_entry;
    if (dynamic_cast<TBranchAlias*>(br) || dynamic_cast<TBranchTPConvert*>(br))
      ent = entry;
    branchSeek (br, ent);
  }

  // Skip out if there was a recoverable error while reading.
  ISetSGKey* set_sgkey = dynamic_cast<ISetSGKey*> (br);
  if (set_sgkey && set_sgkey->sawErr())
    return 0;

  if (data.m_poolcontainer && set_sgkey) {
    // For an external key, need to set up to get reset to happen properly.
    // See ISetSGKey.h.
    set_sgkey->setSGKey (sgkey);
  }

  // Return the proxy.
  return data.m_proxy;
}


/**
 * @brief Return the string pool.
 */
SG::StringPool& ProxyMap::stringPool ()
{
  return m_pool;
}


/**
 * @brief Clear cached data.
 *
 * Should be called when we move to a new tree in a @c TChain.
 */
void ProxyMap::reset()
{
  size_t sz = m_payloads.size();
  for (size_t i = 0; i < sz; i++) {
    payload_t& data = *m_payloads[i];
    //if (data.m_tree != m_trans_tree)
      data.m_branch = 0;
    if (data.m_tree != m_pers_tree &&
        data.m_tree != m_trans_tree &&
        !data.m_transtree)
    {
      data.m_tree = 0;
    }
  }
  m_dh_br = 0;
  m_dh_ver = 0;
  clearLinks();
}


/**
 * @brief Dump the contents of the map to a string, for debugging.
 */
std::string ProxyMap::dump()
{
  std::ostringstream os;
  for (proxymap_t::iterator i = m_proxymap.begin();
       i != m_proxymap.end();
       ++i)
  {
    unsigned int aux = 0;
    const std::string* str = m_pool.keyToString (i->first, aux);
    const char* cstr = str ? str->c_str() : "";
    os << "  " << aux << "/" << cstr << " -- "
       << i->second->m_key << " -- "
       << i->second->m_branchname
       << "\n";
  }
  return os.str();
}


/**
 * @brief Break up the container part of a pool token into tree and branch.
 * @param token The token to parse.
 * @param key The SG key.
 * @param[out] tree_name The name of the referenced tree.
 * @param[out] branch_name The name of the referenced branch.
 * @return True on success, false on failure.
 */
bool ProxyMap::parseToken (const std::string& token,
                           const std::string& key,
                           std::string& tree_name,
                           std::string& branch_name)
{
  // Recognize references to external pool containers.
  // Names are like POOLContainer_BRANCH; the name is also the tree name.
  std::string::size_type i = token.find ("POOLContainer_");
  if (i != std::string::npos) {
    std::string::size_type j = token.find (']', i);
    tree_name = token.substr (i, j);
    branch_name = token.substr (14, std::string::npos);
    return true;
  }

  // Look for a paren.
  i = token.find ('(');

  if (i == std::string::npos) {
    // If no parens, assume name is of the form CONTAINER::BRANCH.
    // The tree name is the entire name again.
    // This is used by top-level converters.
    tree_name = token;
    branch_name = token;
    i = branch_name.find ("::");
    if (i != std::string::npos)
      branch_name.erase (0, i+2);
    return true;
  }

  // Name should have the form TREE(TYPE/KEY) or TREE(TYPE/
  // or TREE(TYPE/) or TREE(TYPE).
  // Tree name is the part before the paren and may be omitted.
  tree_name = token.substr (0, i);
  // May be embedded in a full pool token, with CNT=TREE(...
  std::string::size_type j = tree_name.rfind ('=');
  if (j != std::string::npos)
    tree_name = tree_name.substr (j+1);

  j = token.find ('/', i);
  if (j == std::string::npos) {
    branch_name = token.substr (i+1);
    std::string::size_type endp = branch_name.find (')');
    branch_name.erase (endp, std::string::npos);
    return true;
  }

  // Get the type name.
  std::string type = token.substr (i+1, j-i-1);

  // Get the key.  Default to the input key if it's not there.
  std::string tokkey = key;
  std::string::size_type endp = token.find (')', j);
  if (endp == std::string::npos)
    endp = token.size();
  if (endp - j > 1)
    tokkey = token.substr (j+1, endp-j-1);

  // The branch name is of the form TYPE_KEY.
  branch_name = type + "_" + tokkey;

  return true;
}


/**
 * @brief Called when a branch has moved to a new entry.
 *        Clears any cached data.
 * @param sgkey Hashed SG key/clid for the branch.
 */
void ProxyMap::resetBranch (sgkey_t sgkey)
{
  // If we have it in our map, reset the DataBucket.
  proxymap_t::iterator i = m_proxymap.find (sgkey);
  if (i != m_proxymap.end()) {
    SG::DataProxy* proxy = i->second->m_proxy;
    if (proxy) {
      DataBucketVoid* db =
        dynamic_cast<DataBucketVoid*> (proxy->object());
      if (db)
        db->reset();
    }
  }
}


/**
 * @brief Create or update a proxy.
 * @param proxy The proxy being updated.  If 0, create a new one.
 * @param br The target branch.
 * @param clid The class ID of the object.
 * @param altclids Other class IDs symlinked to it.
 * @param key StoreGate key.
 * @param sgkey Hashed StoreGate key.
 * @return The new or updated proxy, or 0 on error.
 */
SG::DataProxy* ProxyMap::makeProxy (SG::DataProxy* proxy,
                                    TBranch* br,
                                    unsigned int clid,
                                    const std::vector<unsigned int>& altclids,
                                    const std::string& key,
                                    sgkey_t sgkey)
{
  // Get the object address from the branch.
  void** addr = reinterpret_cast<void**> (br->GetAddress());
  if (!addr || !*addr) {
    // Maybe it hasn't been set up yet.
    if (TBranchElement* be = dynamic_cast<TBranchElement*> (br)) {
      be->SetupAddresses();
      addr = reinterpret_cast<void**> (br->GetAddress());
    }
    else if (TBranchObject* be = dynamic_cast<TBranchObject*> (br)) {
      be->SetAddress(0);
      addr = reinterpret_cast<void**> (br->GetAddress());
    }
    else if (TBranchAlias* be = dynamic_cast<TBranchAlias*> (br)) {
      be->SetAddress(0);
      addr = reinterpret_cast<void**> (br->GetAddress());
    }
  }

  // Can't do anything if there's no address.
  if (!addr || !*addr) {
    delete proxy;
    return 0;
  }

  // We also need the TClass.
  TClass* cls = gROOT->GetClass (br->GetClassName());
  if (!cls) {
    delete proxy;
    return 0;
  }

  // Ok, now we can make a DataBucket for the object.
  DataObject* bucket = new DataBucketVoid (cls, *addr);

  if (proxy) {
    // Proxy already exists.
    // Just need to update the object.
    proxy->setObject (bucket);
  }
  else {
    // Create a new proxy.
    SG::TransientAddress* ta = new SG::TransientAddress (clid, key);
    ta->setSGKey (sgkey);
    for (unsigned int i = 0; i < altclids.size(); i++)
      if (altclids[i] != clid)
        ta->setTransientID (altclids[i]);
    proxy = new SG::DataProxy (bucket, ta);
    proxy->setStore (m_trans_tree_dict);
  }

  m_transientMap[addr] = proxy;

  return proxy;
}


/**
 * @brief Find the data header for this tree.
 * @brief entry The entry in the tree to which to seek.
 *              -1 means to use the current entry.
 * @return The address of the data header, or 0 for failure.
 *
 * This should work for any data header version.
 * On success, the version found is stashed in m_dh_ver.
 */
void* ProxyMap::findDHCommon (Long64_t entry)
{
  // We cache the branch for speed.
  if (!m_dh_br) {
    // We don't have the branch cached.  Need to look it up.
    // First, find the tree (in the same directory as the persistent tree).
    TTree* dh_tree =
      dynamic_cast<TTree*> (m_pers_tree->GetTree()->GetDirectory()->Get
                            ("POOLContainer_DataHeader"));
    if (!dh_tree)
      dh_tree = 
        dynamic_cast<TTree*> (m_pers_tree->GetTree()->GetDirectory()->Get
                              ("POOLContainer"));
    if (!dh_tree) {
      ::Error ("ProxyMap", "Can't find DataHeader for tree %s",
               m_pers_tree->GetName());
      return 0;
    }
    // Now get the branch in the tree.
    m_dh_br = dh_tree->GetBranch ("DataHeader_p3");
    if (m_dh_br)
      m_dh_ver = 3;
    else if ((m_dh_br = dh_tree->GetBranch ("DataHeader_p4")) != 0)
      m_dh_ver = 4;
    else if ((m_dh_br = dh_tree->GetBranch ("DataHeader_p5")) != 0)
      m_dh_ver = 5;
    else if ((m_dh_br = dh_tree->GetBranch ("DataHeader")) != 0)
      m_dh_ver = 5;
    else {
      ::Error ("ProxyMap", "Can't find DataHeader branch for tree %s",
               m_pers_tree->GetName());
      return 0;
    }
    readLinks();
  }

  if (entry >= 0) {
    // We need to seek to an entry.
    // First translate to local entry (to handle the TChain case),
    // then seek in the branch.
    Long64_t localentry = m_pers_tree->LoadTree (entry);
    branchSeek (m_dh_br, localentry);
  }

  // Now fetch the object address from the branch.
  void** addr = reinterpret_cast<void**> (m_dh_br->GetAddress());
  if (!addr) return 0;
  return *addr;
}


/**
 * @brief Find a V3 data header for this tree.
 * @brief entry The entry in the tree to which to seek.
 *              -1 means to use the current entry.
 * @return The data header, or 0 on failure.
 */
DataHeader_p3* ProxyMap::findDH3 (Long64_t entry)
{
  // Get the address.
  if (m_dh_ver != 0 && m_dh_ver != 3)
    return 0;
  void* addr = findDHCommon (entry);
  if (!addr || m_dh_ver != 3)
    return 0;

  // Convert to a data header.
  return reinterpret_cast<DataHeader_p3*>(addr);
}


/**
 * @brief Find a V4 data header for this tree.
 * @brief entry The entry in the tree to which to seek.
 *              -1 means to use the current entry.
 * @return The data header, or 0 on failure.
 */
DataHeader_p4* ProxyMap::findDH4 (Long64_t entry)
{
  // Get the address.
  if (m_dh_ver != 0 && m_dh_ver != 4)
    return 0;
  void* addr = findDHCommon (entry);
  if (!addr || m_dh_ver != 4)
    return 0;

  // Convert to a data header.
  return reinterpret_cast<DataHeader_p4*>(addr);
}


/**
 * @brief Find a V5 data header for this tree.
 * @brief entry The entry in the tree to which to seek.
 *              -1 means to use the current entry.
 * @return The data header, or 0 on failure.
 */
DataHeader_p5* ProxyMap::findDH5 (Long64_t entry)
{
  // Get the address.
  if (m_dh_ver != 0 && m_dh_ver != 5)
    return 0;
  void* addr = findDHCommon (entry);
  if (!addr || m_dh_ver != 5)
    return 0;

  // Convert to a data header.
  DataHeader_p5* dh = reinterpret_cast<DataHeader_p5*>(addr);
  Token token;
  token.fromString(dh->dhFormToken());
  if (token.contID().empty())
    token.setCont(getLink(token.oid().first));

  std::string tree_name;
  std::string branch_name;
  if (!parseToken (token.contID(),
                   "",
                   tree_name,
                   branch_name))
  {
    ::Error ("Proxymap", "Bad token format %s",
             token.toString().c_str());
    return 0;
  }

  TTree* form_tree =
    dynamic_cast<TTree*> (m_pers_tree->GetTree()->GetDirectory()->Get
                          (tree_name.c_str()));
  if (!form_tree) {
    ::Error ("Proxymap", "Cannot find DataHeaderForm tree named %s",
             tree_name.c_str());
    return 0;
  }

  TBranch* form_br = form_tree->GetBranch (branch_name.c_str());
  if (!form_br)
    form_br = form_tree->GetBranch ("DataHeaderForm_p5");
  if (!form_br) {
    ::Error ("Proxymap", "Cannot find DataHeaderForm branch %s",
             branch_name.c_str());
    return 0;
  }
  form_br->GetEntry(0);

  DataHeaderForm_p5* form =
    *reinterpret_cast<DataHeaderForm_p5**> (form_br->GetAddress());
  dh->setDhForm (form);

  return dh;
}


/**
 * @brief Return the entry for an external reference, from the data header.
 * @param entry Entry number in the data header.
 * @param key SG key of the target object.
 * @param pclid Primary class ID of the target object.
 * @return The entry number, or -1 on failure.
 *
 * In the case of an external tree, the entry number needed won't
 * necessarily correspond to the entry in the transient tree
 * (a given transient tree entry may point to 0 or more entries
 * in the external tree).  We need to find it from the data header.
 */
Long64_t ProxyMap::poolContainerEntry (Long64_t entry,
                                       const std::string& key,
                                       unsigned int pclid)
{
  // Search for the target class id/key.
  if (DataHeader_p5* dh = findDH5 (entry)) {
    DataHeader tdh;
    DataHeaderCnv_p5 cnv;
    cnv.persToTrans (dh, &tdh);
    std::vector<DataHeaderElement>::const_iterator it = tdh.begin();
    std::vector<DataHeaderElement>::const_iterator end = tdh.end();
    for (; it != end; ++it) {
      const DataHeaderElement& dhe = *it;
      if (dhe.getKey() == key && dhe.getPrimaryClassID() == pclid) {
        // Found it --- return the index from the header.
        const Token* token = dhe.getToken();
        return token != 0 ? token->oid().second : ~0x0LL;
      }
    }
  }

  else if (DataHeader_p4* dh = findDH4 (entry)) {
    const std::vector<DataHeaderElement_p4>& elts = dh->elements();
    size_t sz = elts.size();
    for (size_t i = 0; i < sz; i++) {
      const DataHeaderElement_p4& dhe = elts[i];
      if (dhe.key() == key && dhe.pClid() == pclid) {
        // Found it --- return the index from the header.
        return dhe.oid2();
      }
    }
  }

  else if (DataHeader_p3* dh = findDH3 (entry)) {
    const std::vector<DataHeaderElement_p3>& elts = dh->elements();
    size_t sz = elts.size();
    for (size_t i = 0; i < sz; i++) {
      const DataHeaderElement_p3& dhe = elts[i];
      if (dhe.key() == key && dhe.pClid() == pclid) {
        // Found it --- return the index from the header.
        return dhe.oid2();
      }
    }
  }

  // Didn't find it.
  return -1;
}


/**
 * @brief Read the data header and make map entries for types we find there.
 * @param entry Entry number for the data header.
 *
 * Only done if the entry number is different from the last entry
 * for which we did this.
 *
 * Returns true if the data header was scanned.
 */
bool ProxyMap::maybeFillProxyMap (Long64_t entry)
{
  if (entry != m_lastfillentry) {
    m_lastfillentry = entry;
    fillProxyMap (entry);
    return true;
  }
  return false;
}


/**
 * @brief Read the data header and make map entries for types we find there.
 * @param entry Entry number for the data header.
 */
void ProxyMap::fillProxyMap (Long64_t entry)
{
  // Get the data header.
  if (DataHeader_p5* dh = findDH5 (entry)) {
    DataHeader tdh;
    DataHeaderCnv_p5 cnv;
    cnv.persToTrans (dh, &tdh);
    std::vector<DataHeaderElement>::const_iterator it = tdh.begin();
    std::vector<DataHeaderElement>::const_iterator end = tdh.end();
    for (; it != end; ++it) {
      const DataHeaderElement& dhe = *it;
      const std::set<CLID>& clidset = dhe.getClassIDs();
      std::vector<unsigned int> clids (clidset.begin(), clidset.end());
      const Token* token = dhe.getToken();
      if (token && token->contID().empty())
        const_cast<Token*>(token)->setCont(getLink(token->oid().first));
      handleDHE (dhe.getPrimaryClassID(),
                 clids,
                 dhe.getKey(),
                 token != 0 ? token->toString() : "");
    }
  }

  else if (DataHeader_p4* dh = findDH4 (entry)) {
    // Loop over all elements.
    const std::vector<DataHeaderElement_p4>& elts = dh->elements();
    size_t sz = elts.size();
    for (size_t i = 0; i < sz; i++) {
      const DataHeaderElement_p4& dhe = elts[i];
      handleDHE (dhe.pClid(),
                 dhe.clids(),
                 dhe.key(),
                 dhe.token());
    }
  }

  else if (DataHeader_p3* dh = findDH3 (entry)) {
    // Loop over all elements.
    const std::vector<DataHeaderElement_p3>& elts = dh->elements();
    size_t sz = elts.size();
    for (size_t i = 0; i < sz; i++) {
      const DataHeaderElement_p3& dhe = elts[i];
      handleDHE (dhe.pClid(),
                 dhe.clids(),
                 dhe.key(),
                 dhe.token());
    }
  }

}


/**
 * @brief Add a payload to the proxy map for a SG key and applicable CLIDs.
 * @param pclid The primary CLID.
 * @param clids Additional CLIDs for this element.
 *              (Ok for pclid to also be in this vector.)
 * @param key The SG key.
 * @param data The payload to add.
 */
void ProxyMap::addPayload (unsigned int pclid,
                           const std::vector<unsigned int>& clids,
                           const std::string& key,
                           payload_t* data)
{
  // Make entry for the primary CLID.
  m_proxymap[m_pool.stringToKey (key, pclid)] = data;

  // Also make entries for any alternate CLIDs.
  for (size_t i = 0; i < clids.size(); i++)
    if (clids[i] != pclid)
      m_proxymap[m_pool.stringToKey (key, clids[i])] = data;

  // Handle auto-symlink.
  const SG::BaseInfoBase* bib = SG::BaseInfoBase::find (pclid);
  if (bib) {
    std::vector<CLID> bases = bib->get_bases();
    for (size_t i = 0; i < bases.size(); i++)
      m_proxymap[m_pool.stringToKey (key, bases[i])] = data;
  }

  // Handle CLID aliases.
  std::vector<CLID> aliases = m_trans_tree->getCLIDAliases (pclid);
  for (size_t i = 0; i < aliases.size(); i++)
    m_proxymap[m_pool.stringToKey (key, aliases[i])] = data;
}


/**
 * @brief Handle a single data header element.
 * @param pclid The primary CLID.
 * @param clids Additional CLIDs for this element.
 *              (Ok for pclid to also be in this vector.)
 * @param key The SG key.
 * @param token The pool token.
 */
void ProxyMap::handleDHE (unsigned int pclid,
                          const std::vector<unsigned int>& clids,
                          const std::string& key,
                          const std::string& token)
{
  // Hash the SG key and the primary CLID to find the primary map key.
  sgkey_t sgkey_p = m_pool.stringToKey (key, pclid);

  // Early exit if we've already seen this type.
  if (m_proxymap.find (sgkey_p) != m_proxymap.end()) return;

  // Find the target tree/branch name.
  TTree* tree;
  std::string branchname = branchNameFromKeyToken (key,
                                                   token,
                                                   tree);
  if (branchname.empty()) return;

  // Make the payload object.
  payload_t* data = makePayload (branchname,
                                 tree,
                                 pclid,
                                 clids,
                                 key);
  m_payloads.push_back (data);

  // Make entries in the map for it.
  addPayload (pclid, clids, key, data);

  // Make entries for any aliases.
  std::vector<std::string> aliases = m_trans_tree->getAliases (branchname);
  for (unsigned int i = 0; i < aliases.size(); i++)
    addPayload (pclid, clids, aliases[i], data);

  if (branchname != key &&
      strncmp (tree->GetName(), "POOLContainer_", 14) != 0)
  {
    addPayload (pclid, clids, branchname, data);
  }

  // If there's exactly one entry for a primary clid, we want
  // to be able to use a default (empty) key as well.
  // So, if there's no default entry there, make one.
  // If there is, replace it with an invalid entry.
  sgkey_t sgkey_anon = m_pool.stringToKey ("", pclid);
  if (m_proxymap.find (sgkey_anon) == m_proxymap.end())
    m_proxymap[sgkey_anon] = data;
  else {
    m_proxymap[sgkey_anon] = &s_invalid_data;
  }
}


/**
 * @brief Make a new payload object to put in the proxy map.
 * @param branchname The name of the target branch.
 * @param tree The target tree.
 * @param pclid The primary CLID.
 * @param clids Additional CLIDs for this element.
 *              (Ok for pclid to also be in this vector.)
 * @param key The SG key.
 */
ProxyMap::payload_t*
ProxyMap::makePayload (const std::string& branchname,
                       TTree* tree,
                       unsigned int pclid,
                       const std::vector<unsigned int>& clids,
                       const std::string& key)
{
  // Fill in a new payload object.
  payload_t* payload = new payload_t;

  // Basic information passed into this function.
  payload->m_branchname = branchname;
  payload->m_clid = pclid;
  payload->m_altclids = clids;
  payload->m_key = key;
  payload->m_tree = tree;
  payload->m_treename = tree->GetName();

  // Flag if the target tree is a transient tree.
  payload->m_transtree = (dynamic_cast<TTreeBranchMap*> (tree) != 0);

  // Flag if the target tree is an external pool container.
  payload->m_poolcontainer = false;
  if (strncmp (tree->GetName(), "POOLContainer_", 14) == 0)
    payload->m_poolcontainer = true;

  // Cached values, initialized to clear.
  payload->m_proxy = 0;
  payload->m_branch = 0;
  payload->m_last_trans_entry = -1;
  payload->m_last_pers_entry = -1;

  return payload;
}


/**
 * @brief Find the target tree/branch name from a key and a token.
 * @param key The SG key.
 * @param token The pool token.
 * @param[out] tree_out The target tree.
 * @return The target branch name.  Blank on failure.
 */
std::string
ProxyMap::branchNameFromKeyToken (const std::string& key,
                                  const std::string& token,
                                  TTree* & tree_out)
{
  // Assume we want the persistent tree.
  tree_out = m_pers_tree;

  // Parse the token value.
  std::string tree_name;
  std::string branch_name;
  if (!parseToken (token, key, tree_name, branch_name)) {
    // Failed.  (DataHeader is a special case --- don't complain about that.)
    if (token != "POOLContainer_DataHeader")
      ::Warning ("ProxyMap", "Unknown token format %s", token.c_str());
    return "";
  }

  // If the target tree is something other than the persistent tree,
  // look it up.
  if (!tree_name.empty() && tree_name != tree_out->GetName()) {
    tree_out = dynamic_cast<TTree*>(tree_out->GetTree()->
                                GetDirectory()->Get (tree_name.c_str()));
    if (!tree_out) return "";
  }

  // Deal with branch name mangling...
  std::string::size_type pos = 0;
  while ((pos = branch_name.find_first_of ("<>/", pos)) != std::string::npos)
    branch_name[pos] = '_';

  // Find the branch in the tree.
  std::string aux_branch_name = branch_name;
  TBranch* branch = tree_out->GetBranch (branch_name.c_str());
  if (!branch) {
    // Sometimes the namespace is dropped from the branch name in the tree.
    std::string::size_type pos = branch_name.find ("::");
    if (pos != std::string::npos) {
      aux_branch_name = branch_name.substr (pos+2, std::string::npos);
      branch = tree_out->GetBranch (aux_branch_name.c_str());
    }
  }
  if (!branch) return "";

  // Now see if there's a corresponding transient branch.
  // If so, use that instead.
  std::string tbranch_name = m_trans_tree->findBranch (branch_name);
  if (!tbranch_name.empty())
  {
    branch_name = tbranch_name;
    tree_out = m_trans_tree;
  }

  // If the tree isn't the persistent tree, it may be an external
  // tree for which there's an auxiliary transient tree.
  // Look for that.
  if (tree_name != m_pers_tree->GetName()) {
    // Switch to the auxtree directory.
    std::string aux_dir = m_trans_tree->GetName();
    aux_dir += "_auxtrees";
    TDirectory::TContext savedir (gROOT);
    if (gROOT->Cd (aux_dir.c_str())) {
      // Look for the tree.
      std::string aux_tree_name = "POOLContainer_";
      aux_tree_name += branch_name;
      aux_tree_name += "_aux";
      TTreeBranchMap* auxtree =
        dynamic_cast<TTreeBranchMap*>(gDirectory->Get (aux_tree_name.c_str()));

      if (auxtree) {
        // Found it.  Now see if there's a transient branch
        // corresponding to our target branch.
        tbranch_name = auxtree->findBranch (aux_branch_name);
        if (!tbranch_name.empty())
        {
          branch_name = tbranch_name;
          tree_out = auxtree;
        }
      }
    }
  }

  return branch_name;
}


/**
 * @brief Add a new redirection.
 * @param dbid GUID string of the database.
 * @param container Name of the container.
 * @param offset Offset of the first entry for this DB.
 */
void ProxyMap::addRedirect (const std::string& dbid,
                            const std::string& container,
                            Long64_t offset)
{
  m_redirectMap[redirect_key_t(dbid, container)] = offset;
}


/**
 * @brief Look up a redirection.
 * @param dbid GUID string of the database.
 * @param container Name of the container.
 * @returns The offset of the first entry for this DB.
   */
Long64_t ProxyMap::getRedirect (const std::string& dbid,
                                const std::string& container) const
{
  if (!m_redirectMap.empty()) {
    redirect_map_t::const_iterator it =
      m_redirectMap.find (redirect_key_t (dbid, container));
    if (it != m_redirectMap.end())
      return it->second;
  }
  return 0;
}


/**
 * @brief Clear all redirections.
 */
void ProxyMap::clearRedirect()
{
  m_redirectMap.clear();
}


/**
 * @brief Read the Links table for the current file.
 */
void ProxyMap::readLinks()
{
  clearLinks();

  TFile* file = m_pers_tree->GetCurrentFile();
  if (!file) return;

  TTree* links_tree = dynamic_cast<TTree*> (file->Get ("##Links"));
  if (!links_tree) return;

  TBranch* links_br = links_tree->GetBranch ("db_string");
  if (!links_br) return;

  char text[1024];
  links_br->SetAddress(text);
  Token token;
  for(size_t i=0, n=(size_t)(links_br->GetEntries()); i<n; ++i) {
    links_br->GetEntry(i);
    token.fromString(text);
    if (token.dbID() == s_localDb) // only 'local' links (e.g. no provenance)
      m_links_map.insert(std::pair<long long int, std::string>(token.oid().first, token.contID()));
  }
}


/**
 * @brief Add a new container string for OID from the Links table.
 * @param tokenStr Token string of the link.
 */
void ProxyMap::addLink(const std::string& tokenStr, Long64_t oid)
{
  Token token;
  token.fromString(tokenStr);
  if (token.dbID() == s_localDb) // only 'local' links (e.g. no provenance)
    m_links_map.insert(std::pair<long long int, std::string>(oid, token.contID()));
}


/**
 * @brief Look up a link.
 * @param oid OID container offset.
 * @returns The container name for OID from the Links table.
 */
std::string ProxyMap::getLink (Long64_t oid)
{
  if (m_links_map.empty()) readLinks();
  return m_links_map[oid];
}


/**
 * @brief Clear all links.
 */
void ProxyMap::clearLinks()
{
  m_links_map.clear();
}


/**
 * @brief Raw addition of a proxy to the store.
 * @param id CLID of object being added.
 * @param proxy proxy to add.
 */
StatusCode
ProxyMap::addToStore (CLID id, SG::DataProxy* proxy)
{
  if (proxy == 0)
    return StatusCode::FAILURE;

  sgkey_t sgkey = proxy->sgkey();
  if (m_proxymap.find (sgkey) != m_proxymap.end())
    return StatusCode::FAILURE;

  payload_t* payload = new payload_t;
  payload->m_proxy = proxy;
  payload->m_tree = 0;
  payload->m_branch = 0;
  payload->m_clid = id;
  payload->m_poolcontainer = false;
  payload->m_transtree = false;
  payload->m_last_trans_entry = -1;
  payload->m_last_pers_entry = -1;
  m_proxymap[sgkey] = payload;
  proxy->setStore (m_trans_tree_dict);
  return StatusCode::SUCCESS;
}


/**
 * @brief Get proxy for an object at a given address.
 * @param pTransient Pointer to the object.
 * @return The proxy, or 0 on failure.
 */
SG::DataProxy* ProxyMap::proxy(const void* const pTransient) const
{
  transient_map_t::const_iterator it = m_transientMap.find (pTransient);
  if (it != m_transientMap.end())
    return it->second;
  return 0;
}


} // namespace AthenaROOTAccess
