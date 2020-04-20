/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_METAHANDLEKEY_H
#define STOREGATE_METAHANDLEKEY_H

#include "AthenaKernel/MetaCont.h"
#include "StoreGate/VarHandleKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IClassIDSvc.h"

namespace SG {

  template <class T>
  class MetaHandleKey : public VarHandleKey {

  public:
    MetaHandleKey( const std::string& key,
                   const std::string& dbKey,
                   Gaudi::DataHandle::Mode a );

    StatusCode initialize();

    const MetaContBase::SourceID& dbKey() const { return m_dbKey; }
    void setDbKey(const MetaContBase::SourceID& dbKey) { m_dbKey = dbKey; }

  protected:
    bool isInit() const { return m_isInit; }

    MetaCont<T>* getContainer ATLAS_NOT_CONST_THREAD_SAFE () const { return m_cont; }

    StoreGateSvc* getStore() const;

  private:

    ServiceHandle<StoreGateSvc> m_store;
    MetaCont<T>* m_cont{nullptr};

    MetaContBase::SourceID m_dbKey{""};

    bool m_isInit {false};

  };


}

#include "StoreGate/MetaHandleKey.icc"

#endif
