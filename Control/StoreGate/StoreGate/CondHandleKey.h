/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_CONDHANDLEKEY_H
#define STOREGATE_CONDHANDLEKEY_H 1

#include "AthenaKernel/CondCont.h"
#include "StoreGate/VarHandleKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "CxxUtils/checker_macros.h"
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

    StatusCode initialize(bool used = true);
    StatusCode initialize (AllowEmptyEnum);

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

    // Deliberately returning a non-const pointer here from a const
    // member function.  We don't own the CondCont, we just reference it.
    // The Handle<> classes need to get a non-const CondCont from a
    // const HandleKey<>.
    CondCont<T>* getCC ATLAS_NOT_CONST_THREAD_SAFE () const { return m_cc; }

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
