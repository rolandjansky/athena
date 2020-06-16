/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IMETADATASVC_H
#define ATHENAKERNEL_IMETADATASVC_H

/** @file IMetaDataSvc.h
 *  @brief This file contains the class definition for the IMetaDataSvc class.
 *  @author Marcin Nowak
 **/

#include "GaudiKernel/INamedInterface.h"
#include "AthenaKernel/MetaCont.h"

#include <string>
#include <mutex>

/** @class IMetaDataSvc
 *  @brief This class provides the interface for MetaDataSvc
 **/
class IMetaDataSvc : virtual public ::INamedInterface {

public: // Non-static members

   
   /// used by AthenaPoolCnvSvc
   virtual StatusCode shmProxy(const std::string& filename) = 0;


   // =======  Methods for handling metadata objects stored in MetaContainers (EventService)
   template <typename T, class TKEY>
   T* tryRetrieve (const TKEY& key) const;

   template <typename T, class TKEY>
   const T* tryConstRetrieve(const TKEY& key) const;

   /// Record an object with a key.
   template <typename T, typename TKEY> 
   StatusCode record(T* p2BRegistered, const TKEY& key);

   /// Record an object with a key, take ownership of the unique_ptr obj
   template <typename T, typename TKEY> 
   StatusCode record(std::unique_ptr<T> pUnique, const TKEY& key);

   /// Remove object with this type+key
   template <typename T, typename TKEY> 
   StatusCode remove(const TKEY& key, bool ignoreIfAbsent=false);

   /// The output MetaData Store
   virtual StoreGateSvc* outputDataStore() const = 0;

   /// rangeID for the current EventContext - used to index MetaContainrs - 
   virtual const std::string currentRangeID() const = 0;

   /// Gaudi boilerplate
   static const InterfaceID& interfaceID();

private: // Data
   std::mutex    m_mutex;
};

 
inline const InterfaceID& IMetaDataSvc::interfaceID() {
   static const InterfaceID IID("IMetaDataSvc", 1, 0);
   return(IID);
}

/**
 * @brief Retrieve an object of type @c T from MetaDataStore 
 *        Return 0 if not found. Don't print any WARNINGs
 * @param key The key to use for the lookup.
 **/
template <typename T, class TKEY>
T* IMetaDataSvc::tryRetrieve (const TKEY& key) const
{
   const MetaCont<T>* container = outputDataStore()->tryRetrieve< MetaCont<T> >(key);
   if( container ) {
      return container->get( currentRangeID() );
   }
   return nullptr;
}

template <typename T, class TKEY>
const T* IMetaDataSvc::tryConstRetrieve (const TKEY& key) const
{
   const MetaCont<T>* container = outputDataStore()->tryRetrieve< MetaCont<T> >(key);
   if( container ) {
      return container->get( currentRangeID() );
   }
   return nullptr;
}

template <typename T, typename TKEY> 
StatusCode IMetaDataSvc::record(T* pObject, const TKEY& key)
{
   std::lock_guard lock(m_mutex);
   MetaCont<T>* container = outputDataStore()->tryRetrieve< MetaCont<T> >(key);
   if( !container ) {
      auto cont_uptr = std::make_unique< MetaCont<T> >();
      if( cont_uptr->insert( currentRangeID() , pObject) ) {
         return outputDataStore()->record( std::move(cont_uptr), key );
      }
      return StatusCode::FAILURE;
   }
   if( container->insert( currentRangeID() , pObject) )  return StatusCode::SUCCESS;
   return StatusCode::FAILURE;
}


template <typename T, typename TKEY> 
StatusCode IMetaDataSvc::record(std::unique_ptr<T> pUnique, const TKEY& key)
{
   if( this->record( pUnique.get(), key ).isSuccess() ) {
      pUnique.release();
      return StatusCode::SUCCESS;
   }
   pUnique.reset();
   return StatusCode::FAILURE;
}


template <typename T, class TKEY>
StatusCode IMetaDataSvc::remove(const TKEY& key, bool ignoreIfAbsent)
{
   std::lock_guard lock(m_mutex);
   // change erase to setting nullptr?
   MetaCont<T>* container = outputDataStore()->tryRetrieve< MetaCont<T> >(key);
   if( container and container->erase( currentRangeID() ) )  return StatusCode::SUCCESS;
   return ignoreIfAbsent? StatusCode::SUCCESS : StatusCode::FAILURE;
}

#endif
