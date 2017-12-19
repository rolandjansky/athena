/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_T2PMAP_H
#define SGTOOLS_T2PMAP_H

#include "SGTools/ptrhash.h"
#include <utility>
#include <vector>
#include <unordered_map>
#include "AthenaKernel/IPageAccessControlSvc.h"

namespace SG { 

class DataProxy;

class T2pMap {

 public:
  
  typedef std::unordered_map<const void*, DataProxy*, ptrhash> t2p;

  // constructor
  T2pMap(IPageAccessControlSvc* pac=0) : m_pac(pac) { };

  // Destructor
  ~T2pMap() { };

  // associate a void* (T*) with a proxy
  bool t2pRegister(const void* const pTrans, DataProxy* const pPers) {
    bool success(m_t2p.insert (std::make_pair (pTrans, pPers)) . second);
    if (m_pac) m_pac->controlPage(pTrans);
    return success;
  }

  // locate a proxy in t2p map
  DataProxy* locatePersistent(const void* const pTransient) const {
    t2p::const_iterator i = m_t2p.find(pTransient);

    if (i == m_t2p.end())
      return 0;
    else
      return i->second;
  }

  // clear the t2p map
  void clear() {
    m_t2p.clear();
  }

  // remove a void* from t2p
  void t2pRemove(const void* const pTrans) {
    m_t2p.erase(pTrans);
  }

  /// set IPageAccessControlSvc ptr in T2PMap
  void setPac(IPageAccessControlSvc* pac) { m_pac=pac; }
  /// request an access control report, i.e. a list of proxies that have not been accessed since under control;
  std::vector<const DataProxy*> pacReport() const;


 private:
  IPageAccessControlSvc* m_pac;
  t2p m_t2p;

};

}
#endif
