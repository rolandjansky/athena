/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef APRTEST_AUXSTORE
#define APRTEST_AUXSTORE

#include "AthContainers/AuxStoreInternal.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"


namespace APRTest
{

   class AuxStore : public SG::AuxStoreInternal, SG::IAuxStoreHolder
   {
     IAuxStore* m_extra_store = 0;
  public:
     int        m_data  = 0;

     virtual  ~AuxStore() {}

     
     virtual void setStore( IAuxStore* store ) override {
        m_extra_store = store;
     }

     using  SG::AuxStoreInternal::getData;
        
     virtual void* getData(SG::auxid_t auxid, size_t size, size_t capacity ) override; 
     virtual const void* getData(SG::auxid_t auxid ) const override; 
     
     virtual IAuxStore* getStore() override { return (IAuxStore*)this; }
     virtual const IAuxStore* getStore() const override { return (IAuxStore*)this; }

     virtual AuxStoreType getStoreType() const override  { return AST_ContainerStore; }

     virtual size_t size() const override
     { return m_extra_store ? m_extra_store->size() : SG::AuxStoreInternal::size(); }

     
   };
   
   inline
   void* AuxStore::getData(SG::auxid_t auxid, size_t size, size_t capacity )
   {
      return m_extra_store?   m_extra_store->getData( auxid, size, capacity)
         : SG::AuxStoreInternal::getData( auxid, size, capacity);
   }

   inline
   const void* AuxStore::getData(SG::auxid_t auxid) const
   {
      return m_extra_store?   m_extra_store->getData( auxid)
         : SG::AuxStoreInternal::getData( auxid);
   }

 
}


#endif

