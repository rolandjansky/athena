/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigOutputHandling_BareDataBucket_h
#define TrigOutputHandling_BareDataBucket_h
#include "AthenaKernel/DataBucketBase.h"
#include "DataModelRoot/RootType.h"

/**
 * @class BareDataBucket
 * @brief Allows to insert void* returned from serialisation into the store
 **/

class BareDataBucket: public DataBucketBase {
public:
  BareDataBucket() = delete;

  BareDataBucket( void * data, CLID clid, const RootType& type )
    : m_data(data), m_clid(clid), m_type( type ){}

  virtual ~BareDataBucket() {
    if ( m_data )
      m_type.Destruct( m_data );
  }

  // DataObject overrides
  virtual const CLID& clID() const override {
    return m_clid;
  }
  
  // DataBucketBase overrides

  virtual void* object() override { 
    return m_data; 
  }

  virtual const std::type_info& tinfo() const override { 
    return m_type.TypeInfo(); 
  };

  using DataBucketBase::cast;
  virtual void* cast (CLID clid,
                      SG::IRegisterTransient* ,
                      bool isConst = true) override {
    return ( m_clid == clid and isConst ) ? m_data : nullptr;
  }

  virtual void* cast (const std::type_info& tinfo,
                      SG::IRegisterTransient* ,
                      bool isConst = true) override {
    return ( tinfo == m_type.TypeInfo() and isConst ) ? m_data : nullptr;
  }
  
  virtual void relinquish() override {
    m_data = nullptr;
  };
  
  virtual void lock() override { /*not lockable I think */ };

private:
  void* m_data = 0;
  CLID m_clid  = 0;
  RootType m_type;
};

#endif
