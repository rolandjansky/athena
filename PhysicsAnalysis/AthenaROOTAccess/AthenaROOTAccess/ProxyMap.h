// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ProxyMap.h,v 1.17 2009-05-02 05:01:28 ssnyder Exp $

/**
 * @file AthenaROOTAccess/ProxyMap.h
 * @author scott snyder
 * @date Apr 2007
 * @brief This class maintains the map of (key,clid) to data proxy
 *        instances for a transient tree.  This is used to resolve
 *        @c ElementLink references.
 *
 *        This class also holds the redirection map for merged files.
 */

#ifndef ATHENAROOTACCESS_PROXYMAP_H
#define ATHENAROOTACCESS_PROXYMAP_H


#include "AthenaROOTAccess/TTreeBranchMap.h"
#include "SGTools/StringPool.h"
#include "AthenaKernel/IProxyDict.h"
#include "GaudiKernel/StatusCode.h"
#include "Rtypes.h"
#include <unordered_map>
#include <utility>
#include <vector>


class TBranch;
class TTree;
class DataHeader_p3;
class DataHeader_p4;
class DataHeader_p5;
namespace SG {
  class DataProxy;
}


namespace AthenaROOTAccess {


/**
 * @brief This class maintains the map of (key,clid) to data proxy
 *        instances for a transient tree.  This is used to resolve
 *        @c ElementLink references.
 *
 * This is also where the proxies get built from the @c DataHeader information.
 * That's where most of the complication lies.
 */
class ProxyMap
{
public:
  /// Abbreviation for hashed key type.
  typedef SG::StringPool::sgkey_t sgkey_t;

  /**
   * @brief Constructor.
   * @param trans_tree The transient tree.
   * @param pers_tree The persistent tree.
   */
  ProxyMap (TTreeBranchMap* trans_tree, TTree* pers_tree);


  /**
   * @brief Destructor.
   */
  ~ProxyMap();


  /**
   * @brief Look up a proxy.
   * @param key The SG key.
   * @param id The class ID.
   * @param entry The entry number in the tree.
   * @return Pointer to a proxy to the object, or 0 if not found.
   *
   * If needed, this will read through the data header to create new proxies.
   */
  SG::DataProxy* getProxy (const std::string& key,
                           unsigned int id,
                           Long64_t entry);


  /**
   * @brief Look up a proxy.
   * @param sgkey The hashed key+CLID.
   * @param entry The entry number in the tree.
   * @return Pointer to a proxy to the object, or 0 if not found.
   *
   * If needed, this will read through the data header to create new proxies.
   */
  SG::DataProxy* getProxy (sgkey_t sgkey,
                           Long64_t entry);


  /**
   * @brief Return the string pool.
   */
  SG::StringPool& stringPool ();


  /**
   * @brief Clear cached data.
   *
   * Should be called when we move to a new tree in a @c TChain.
   */
  void reset();


  /**
   * @brief Dump the contents of the map to a string, for debugging.
   */
  std::string dump();


  /**
   * @brief Break up the container part of a pool token into tree and branch.
   * @param token The token to parse.
   * @param key The SG key.
   * @param[out] tree_name The name of the referenced tree.
   * @param[out] branch_name The name of the referenced branch.
   * @return True on success, false on failure.
   */
  static bool parseToken (const std::string& token,
                          const std::string& key,
                          std::string& tree_name,
                          std::string& branch_name);
    

  /**
   * @brief Called when a branch has moved to a new entry.
   *        Clears any cached data.
   * @param sgkey Hashed SG key/clid for the branch.
   */
  void resetBranch (sgkey_t sgkey);


  /**
   * @brief Read the data header and make map entries for types we find there.
   * @param entry Entry number for the data header.
   *
   * Only done if the entry number is different from the last entry
   * for which we did this.
   *
   * Returns true if the data header was scanned.
   */
  bool maybeFillProxyMap (Long64_t entry);


  //********************************************************************
  // Redirection map:
  //
  // When files are merged, the OID indices in tokens are not adjusted.
  // Instead, pool writes to the file metadata a set of
  // (container, offset) pairs for each container and for each
  // merged file.  This information is pulled out of the metadata
  // by the initialization code in transientTree and registered here,
  // to be then used by the code in TBranchTPConvert.


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
   * @param tokenStr Token string of the link.
   */
  void addLink(const std::string& tokenStr, Long64_t oid);


  /**
   * @brief Look up a link.
   * @param oid OID container offset.
   * @returns The container name for OID from the Links table.
   */
  std::string getLink (Long64_t oid);


  /**
   * @brief Clear all links.
   */
  void clearLinks();


  /**
   * @brief Raw addition of a proxy to the store.
   * @param id CLID of object being added.
   * @param proxy proxy to add.
   */
  StatusCode addToStore (CLID id, SG::DataProxy* proxy);

 
  /**
   * @brief Get proxy for an object at a given address.
   * @param pTransient Pointer to the object.
   * @return The proxy, or 0 on failure.
   */
  SG::DataProxy* proxy(const void* const pTransient) const;


private:
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
  SG::DataProxy* makeProxy (SG::DataProxy* proxy,
                            TBranch* br,
                            unsigned int clid,
                            const std::vector<unsigned int>& altclids,
                            const std::string& key,
                            sgkey_t sgkey);


  /**
   * @brief Find the data header for this tree.
   * @brief entry The entry in the tree to which to seek.
   *              -1 means to use the current entry.
   * @return The address of the data header, or 0 for failure.
   *
   * This should work for any data header version.
   * On success, the version found is stashed in m_dh_ver.
   */
  void* findDHCommon (Long64_t entry);


  /**
   * @brief Find a V3 data header for this tree.
   * @brief entry The entry in the tree to which to seek.
   *              -1 means to use the current entry.
   * @return The data header, or 0 on failure.
   */
  DataHeader_p3* findDH3 (Long64_t entry);


  /**
   * @brief Find a V4 data header for this tree.
   * @brief entry The entry in the tree to which to seek.
   *              -1 means to use the current entry.
   * @return The data header, or 0 on failure.
   */
  DataHeader_p4* findDH4 (Long64_t entry);


  /**
   * @brief Find a V5 data header for this tree.
   * @brief entry The entry in the tree to which to seek.
   *              -1 means to use the current entry.
   * @return The data header, or 0 on failure.
   */
  DataHeader_p5* findDH5 (Long64_t entry);


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
  Long64_t poolContainerEntry (Long64_t entry,
                               const std::string& key,
                               unsigned int pclid);


  /**
   * @brief Read the data header and make map entries for types we find there.
   * @param entry Entry number for the data header.
   */
  void fillProxyMap (Long64_t entry);


  /**
   * @brief Handle a single data header element.
   * @param pclid The primary CLID.
   * @param clids Additional CLIDs for this element.
   *              (Ok for pclid to also be in this vector.)
   * @param key The SG key.
   * @param token The pool token.
   */
  void handleDHE (unsigned int pclid,
                  const std::vector<unsigned int>& clids,
                  const std::string& key,
                  const std::string& token);


  /**
   * @brief Find the target tree/branch name from a key and a token.
   * @param key The SG key.
   * @param token The pool token.
   * @param[out] tree_out The target tree.
   * @return The target branch name.  Blank on failure.
   */
  std::string branchNameFromKeyToken (const std::string& key,
                                      const std::string& token,
                                      TTree* & tree);

  // --- The actual map is a map from hashed SG key/CLID values
  //     (which are 64-bit integers) to a payload object.
  //     Use a hash table to hold these.
  

  /**
   * @brief The hash function for the proxy map.
   *
   * This just truncates the SG key/CLID hash to the proper size.
   */
  struct keyhash
  {
    std::size_t operator() (const sgkey_t& key) const
    {
      return static_cast<std::size_t> (key);
    }
  };


  /**
   * @brief The payload object for the proxy map.
   */
  struct payload_t
  {
    /// The proxy object.
    SG::DataProxy* m_proxy;

    /// The target tree.  This may be cleared on a reset.
    TTree* m_tree;

    /// The target tree name.
    std::string m_treename;

    /// The target branch.  This may be cleared on a reset.
    TBranch* m_branch;

    /// The target branch name.
    std::string m_branchname;

    /// Primary class id of the target object.
    unsigned int m_clid;

    /// Alternate class ids for the target object.
    /// (May contain m_clid.)
    std::vector<unsigned int> m_altclids;

    /// SG key for the target object.
    std::string m_key;

    /// If true, the target is in an external pool container.
    bool m_poolcontainer;

    /// If true, the target branch is in a transient tree.
    bool m_transtree;

    /// For an external pool container: the last translated
    /// transient entry number
    Long64_t m_last_trans_entry;

    /// For an external pool container: the last translated
    /// persistent entry  number
    Long64_t m_last_pers_entry;
  };


  /**
   * @brief Make a new payload object to put in the proxy map.
   * @param branchname The name of the target branch.
   * @param tree The target tree.
   * @param pclid The primary CLID.
   * @param clids Additional CLIDs for this element.
   *              (Ok for pclid to also be in this vector.)
   * @param key The SG key.
   */
  payload_t* makePayload (const std::string& branchname,
                          TTree* tree,
                          unsigned int pclid,
                          const std::vector<unsigned int>& clids,
                          const std::string& key);


  /**
   * @brief Add a payload to the proxy map for a SG key and applicable CLIDs.
   * @param pclid The primary CLID.
   * @param clids Additional CLIDs for this element.
   *              (Ok for pclid to also be in this vector.)
   * @param key The SG key.
   * @param data The payload to add.
   */
  void addPayload (unsigned int pclid,
                   const std::vector<unsigned int>& clids,
                   const std::string& key,
                   payload_t* data);


  /// Type for the proxy map.
  /// We hold payload_t via pointers because we want entries
  /// corresponding to symlinks to point at the same entry.
  typedef std::unordered_map<sgkey_t, payload_t*, keyhash> proxymap_t;

  /// The actual proxy map.
  proxymap_t m_proxymap;

  /// Container of payload objects.
  std::vector<payload_t*> m_payloads;

  /// String pool for SG key/CLID hashing.
  SG::StringPool m_pool;

  /// Transient tree.
  TTreeBranchMap* m_trans_tree;

  /// Transient tree, cast to IProxyDict.
  IProxyDict* m_trans_tree_dict;

  /// Persistent tree.
  TTree* m_pers_tree;

  /// Cached data header branch.
  TBranch* m_dh_br;

  /// Version of the cached data header branch (0 for unknown).
  int m_dh_ver;

  /// Cached POOL/APR links branch.
  std::map<long long int, std::string> m_links_map;

  /// The entry number where we last scanned the data header.
  Long64_t m_lastfillentry;

  /// An invalid payload, for default key handling.
  static payload_t s_invalid_data;


  struct redirect_key_t
  {
    redirect_key_t (const std::string& dbid, const std::string& container)
      : m_dbid (dbid), m_container (container) {}
    bool operator== (const redirect_key_t& other) const
    {
      return m_dbid == other.m_dbid && m_container == other.m_container;
    }

    std::string m_dbid;
    std::string m_container;
  };


  struct redirect_key_hash
  {
    std::size_t operator() (const redirect_key_t& key) const
    {
      std::hash<std::string> h;
      return h (key.m_dbid) + h (key.m_container);
    }
  };

  typedef std::unordered_map<redirect_key_t, size_t, redirect_key_hash>
    redirect_map_t;

  /// The redirection map.
  redirect_map_t m_redirectMap;

  /// Map from transient objects to pointers.
  typedef std::unordered_map<const void*, SG::DataProxy*> transient_map_t;
  transient_map_t m_transientMap;
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_PROXYMAP_H
