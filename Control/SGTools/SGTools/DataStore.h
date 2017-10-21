/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_DATASTORE_H
#define STOREGATE_DATASTORE_H
/** The Transient Store 
 *
 * \author ATLAS Collaboration
 **/


#include "SGTools/ProxyMap.h"
#include "SGTools/T2pMap.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/DefaultKey.h"
#include "AthenaKernel/IProxyRegistry.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/StatusCode.h"
#include <boost/array.hpp>
#include <boost/type_traits/transform_traits.hpp>
#include <exception>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <typeinfo> /*typeid*/
#include <utility>  /*std::pair*/
#include <unordered_map>


class ISvcLocator;
class ISGAudSvc;
class MsgStream;
class IPageAccessControlSvc;

namespace SG {

  class DataProxy;


  /**
   * @brief Hold DataProxy instances associated with a store.
   *
   * We hold proxies in several ways, for efficiency.
   *
   * First, all proxies that we manage are entered in the m_proxies vector.
   * A given proxy appears only once in this list.
   *
   * Second, m_keyMap stores proxies indexed by the hashed CLID/key.
   * A proxy may be entered here multiple times in the case of symlinks
   * or alias; however, the entry corresponding to what proxy->sgkey()
   * returns is called the `primary' entry.  The mapped value  is actually
   * an int/DataProxy* pair.  For the primary entry, the integer is the
   * index of the proxy in m_proxies; for other entries, it is -1.
   *
   * Third, proxies are stored by CLID/key in m_storeMap.  This is a map
   * indexed by CLID; the values are maps indexed by key.  A proxy may again
   * be stored several times in this map in the case of symlinks/aliases.
   *
   * A proxy may be given to the store that has the hashed key set,
   * but not the CLID/key.  This is referred to as a `dummy' proxy.
   * This can happen when we read a link to an object that's not
   * described in the event's DataHeader.  In this case, the proxy
   * is entered in m_proxies and m_keyMap, but not in m_storeMap.
   * If we later look
   * for a proxy by CLID/key and don't find it, we also look in m_keyMap
   * to see if a dummy proxy has been entered there.  If so, that point
   * we fill in the CLID/key fields of the proxy and also enter
   * it in m_storeMap.
   */
  class DataStore : virtual public IProxyRegistry
  {

  public:

    typedef IStringPool::sgkey_t                sgkey_t;
    typedef std::map<CLID, SG::ProxyMap>        StoreMap;
    typedef StoreMap::iterator                  StoreIterator;
    typedef StoreMap::const_iterator            ConstStoreIterator;

    typedef std::pair<SG::ConstProxyIterator, SG::ConstProxyIterator> constProxyRange;

    /**
     * @brief Constructor.
     * @param pool The string pool associated with this store.
     */
    DataStore (IProxyDict& pool);
    virtual ~DataStore();

    void setStoreID(StoreID::type id) { m_storeID = id;}
    virtual StoreID::type storeID() const { return m_storeID; }
 
    // If FORCE is true, then force deleting of all proxies,
    // even if they would normally only be reset.
    /// If HARD is true, then the bound objects should also
    /// clear any data that depends on the identity
    /// of the current event store.  (See IResetable.h.)
    void clearStore(bool force, bool hard, MsgStream* pmlog);

    //////////////////////////////////////////////////////////////////
    /// \name Implementation of IProxyRegistry.
    //@{
    virtual StatusCode addToStore(const CLID& id, DataProxy* proxy);
    /// return proxy for a given type/key pair
    /// if key is empty returns the default proxy (currently last registered)
    virtual DataProxy* proxy(const TransientAddress* tAddr) const; 
    //@}
    virtual DataProxy* proxy(const CLID& id, 
			     const std::string& key=SG::DEFAULTKEY) const;

    /// get proxy with given key. Returns 0 to flag failure
    /// the key must match exactly (no wild carding for the default key)
    SG::DataProxy* proxy_exact (sgkey_t sgkey) const;

    /// get proxy with given id. Returns 0 to flag failure
    /// the key must match exactly (no wild carding for the default key)
    virtual SG::DataProxy* proxy_exact(const CLID& id,
                                       const std::string& key) const;

    /// remove proxy from store, unless proxy is reset only.   
    /// @param forceRemove remove the proxy no matter what
    /// If HARD is true, then the bound objects should also
    /// clear any data that depends on the identity
    /// of the current event store.  (See IResetable.h.)
    StatusCode removeProxy(DataProxy* proxy, bool forceRemove, bool hard);

    /// add symlink to store:
    StatusCode addSymLink(const CLID& linkid, DataProxy* proxy);

    /// add alias to store
    StatusCode addAlias(const std::string& aliasKey, DataProxy* proxy);

    /// Count number of object of a given type in store
    int typeCount(const CLID& id) const;

    /// Return an iterator over the StoreMap:
    StatusCode tRange(ConstStoreIterator& f, ConstStoreIterator& e) const;

    /// Return an iterator over proxy for a given CLID:
    StatusCode pRange(const CLID& id, SG::ConstProxyIterator& f,
		      SG::ConstProxyIterator& e) const;

    /// Return a list of all valid proxies in the store:
    StatusCode proxyList(std::list<DataProxy*>& pList) const;


    /// set IPageAccessControlSvc ptr in T2PMap
    void setPac(IPageAccessControlSvc* pac) { m_t2p.setPac(pac); }
    /// request an access control report, i.e. a list of proxies that have not been accessed since monitored
    std::vector<DataProxy*> pacReport() const { return m_t2p.pacReport();}

    /// methods to query the T2PMap:
    StatusCode t2pRegister(const void* const pTrans, DataProxy* const pPers);
    void t2pRemove(const void* const pTrans);

    /// locate the persistent (proxy) for a given T* (void*):
    DataProxy* locatePersistent(const void* const pTransient) const;

    // return list of keys associated with an object
    void keys(const CLID& id, std::vector<std::string>& vkeys,
	      bool includeAlias, bool onlyValid);

  private:
    /// The string pool associated with this store.
    IProxyDict& m_pool;

    /// All proxies managed by this store.  Every proxy appears exactly
    /// once in this list.
    std::vector<DataProxy*> m_proxies;

    /// Maps locating proxies by clid/key.
    StoreMap m_storeMap;

    /// Map of hashed sgkey -> DataProxy.
    /// For the primary entry, the integer is in the index of this proxy
    /// in m_proxies; otherwise, it is -1.
    typedef std::pair<int, DataProxy*> KeyPayload_t;
    typedef std::unordered_map<sgkey_t, KeyPayload_t> KeyMap_t;
    KeyMap_t m_keyMap;

    StoreID::type m_storeID;

    // Map to hold the relation between transient and persistent object:
    // we have changed to DataProxy as it is faster while recording
    // to check if it already exists
    T2pMap  m_t2p;

    void setSGAudSvc();
    ISGAudSvc * m_pSGAudSvc;
    bool m_noAudSvc;    
    inline bool doAudit() {
      if (!m_noAudSvc) setSGAudSvc();
      return (m_pSGAudSvc);
    }

    ISvcLocator* m_pSvcLoc; 
    StatusCode setSvcLoc();


    /**
     * @brief Look for (and convert) a matching dummy proxy.
     * @param id The CLID for which to search.
     * @param key The key for which to search.
     *
     * In some cases, we may enter into the store a `dummy' proxy,
     * which is identified only by the hashed CLID/key pair.
     * (This can happen when we have a link to an object that's not
     * listed in the DataHeader; in this case, we know the only hashed key
     * and not the CLID or key.)
     *
     * This function is called after we fail to find a proxy by CLID/key.
     * It additionally checks to see if there exists a dummy proxy with
     * a hash matching this CLID/key.  If so, the CLID/key are filled
     * in in the proxy, and the proxy is entered in m_storeMap.
     *
     * Returns either the matching proxy or 0.
     */
    DataProxy* findDummy (CLID id, const std::string& key);
  };
  //////////////////////////////////////////////////////////////////
  inline void
  DataStore::t2pRemove(const void* const pTrans)
  {
    m_t2p.t2pRemove(pTrans);
  }
  //////////////////////////////////////////////////////////////////
} //end namespace SG


#endif  // STOREGATE_DATASTORE

