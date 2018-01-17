/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_CONDHANDLEKEY_H
#define STOREGATE_CONDHANDLEKEY_H 1

#include "AthenaKernel/CondCont.h"
#include "StoreGate/VarHandleKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IClassIDSvc.h"

namespace SG {

  template <class T>
  class CondHandleKey : public VarHandleKey {

  public:
    CondHandleKey( const std::string& key,
                   const std::string& dbKey,
                   Gaudi::DataHandle::Mode a );

//    CondHandleKey& operator= (const std::string& sgkey);

    StatusCode initialize();

    const std::string& dbKey() const { return m_dbKey; }
    void setDbKey(const std::string& dbKey) { m_dbKey = dbKey; }


    /**
     * @brief Called by the owning algorithm during the START transition.
     *
     * AthAlgorithm and friends will call this during START.  This allows
     * for extra initialization that we can't do during initialize(), such
     * as retrieving a conditions container from the store.
     */
    virtual StatusCode start() override;


  protected:
    bool isInit() const { return m_isInit; }

    CondCont<T>* getCC() const { return m_cc; }

    StoreGateSvc* getCS() const;

  private:

    ServiceHandle<StoreGateSvc> m_cs;
    CondCont<T>* m_cc{0};

    std::string m_dbKey{""};

    bool m_isInit {false};

  };


}

#include "StoreGate/CondHandleKey.icc"

#endif
