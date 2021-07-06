/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _ExpressionEvaluation_xAODAccessor_h_
#define _ExpressionEvaluation_xAODAccessor_h_

#include "GaudiKernel/DataHandle.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxElement.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"
#include "ExpressionEvaluation/IAccessor.h"

#include <memory>
#include <stdexcept>
#include <sstream>

#include "Utils.h"
#include "GenAccessor.h"

namespace ExpressionParsing {
   /** Auxiliary class to handle xAOD accessor based xAOD object content access.
    */
   template <class T_Cont,typename T_src>
   class AccessorHelper {
   public:
      AccessorHelper(SG::auxid_t auxid)
         : m_accessor(SG::AuxTypeRegistry::instance().getName(auxid))
      {}

      std::size_t size(const T_Cont &cont) const {
         return getContainerSize(cont);
      }

      /// Get the scalar provided by the container
      T_src get(const T_Cont &cont) const {
         return m_accessor( cont );
      }

      /// Get the specified element of the vector provided by the container
      T_src get(const T_Cont &cont, std::size_t idx) const {
         return m_accessor( cont,idx );
      }

      /** Auxiliary class to create the corresponding auxiliary helper object
       */
      class Kit {
      public:
         Kit(Kit &&) = default;
         Kit(SG::auxid_t auxid) : m_auxid(auxid) {}

         AccessorHelper<T_Cont,T_src> create(const EventContext& ctx, SG::ReadHandle<T_Cont> &handle) const {
            (void) ctx;
            (void) handle;
            return AccessorHelper<T_Cont,T_src>(m_auxid);
         }
      private:
         SG::auxid_t m_auxid;
      };

   private:
      SG::AuxElement::ConstAccessor<T_src>  m_accessor;
   };

   /** Auxiliary class to handle decorator handle based xAOD object content access.
    */
   template <class T_Cont,typename T_src>
   class DecorHelper {
   public:
      DecorHelper(const EventContext& ctx,const SG::ReadDecorHandleKey<T_Cont> &key,SG::ReadHandle<T_Cont> &handle)
         : m_decorHandle(key, ctx),
           m_data(getVectorData(*handle)->getDataArray(m_decorHandle.auxid()))
      {
         if (!m_decorHandle.isValid()) {
            BaseAccessor::throwInvalidHandle(key.key());
         }
      }

      std::size_t size(const T_Cont &cont) const {
         return getContainerSize(cont);
      }

      /// Get the scalar provided by the container
      T_src get(const T_Cont &cont) const {
         return m_decorHandle( cont );
      }

      /// Get the specified element of the vector provided by the container
      T_src get(const T_Cont &cont, std::size_t idx) const {
         (void) cont;
         return SG::AuxDataTraits<T_src>::index (m_data, idx);
      }

      /** Auxiliary class to create the corresponding auxiliary helper object
       */
      class Kit {
      public:
         Kit(Kit &&) = default;
         Kit(const SG::ReadDecorHandleKey<T_Cont> &key) : m_decorKey(&key) {}

         DecorHelper<T_Cont,T_src> create(const EventContext& ctx, SG::ReadHandle<T_Cont> &handle) const {
            (void) handle;
            try {
               return DecorHelper<T_Cont,T_src>(ctx, *m_decorKey, handle);
            }
            catch (std::exception &err) {
               dumpAux(*handle, 357);
               throw err;
            }
            return DecorHelper<T_Cont,T_src>(ctx, *m_decorKey, handle);
         }
      private:
         const SG::ReadDecorHandleKey<T_Cont> *m_decorKey;
      };

   private:
      SG::ReadDecorHandle<T_Cont,T_src> m_decorHandle;
      const void *m_data;
   };

   class AccessorFactory;

   /** Interface of a class to create an xAOD object content accessor.
    */
   class IAccessorKit {
      friend class AccessorFactory;
   public:
      virtual ~IAccessorKit() {}
      virtual std::unique_ptr<IAccessor> create( const SG::ReadHandleKey<SG::AuxVectorBase> &key,
                                                 SG::auxid_t auxid,
                                                 const SG::ReadDecorHandleKey<SG::AuxVectorBase> *decor_key=nullptr) const = 0;
      virtual std::unique_ptr<IAccessor> create( const SG::ReadHandleKey<SG::AuxElement> &key,
                                                 SG::auxid_t auxid,
                                                 const SG::ReadDecorHandleKey<SG::AuxElement> *decor_key=nullptr) const = 0;
   };

   /** Implementation of an accessor creator which provides accessors for xAOD accessor or decor handle access to content of AuxElement or AuxVectorBase
    */
   template <class T, IProxyLoader::VariableType T_variable_type>
   class AccessorKit : public IAccessorKit {
      virtual std::unique_ptr<IAccessor> create( const SG::ReadHandleKey<SG::AuxVectorBase> &key,
                                                 SG::auxid_t auxid,
                                                 const SG::ReadDecorHandleKey<SG::AuxVectorBase> *decor_key=nullptr) const override {
         return createAccessor<SG::AuxVectorBase,VectorHelper>(key,auxid, decor_key);
      }
      virtual std::unique_ptr<IAccessor> create( const SG::ReadHandleKey<SG::AuxElement> &key,
                                                 SG::auxid_t auxid,
                                                 const SG::ReadDecorHandleKey<SG::AuxElement> *decor_key=nullptr) const override {
         return createAccessor<SG::AuxElement,ScalarHelper>(key,auxid, decor_key);
      }
   private:
      template <class T_Cont, class T_ScalarVectorHelper>
      std::unique_ptr<IAccessor> createAccessor( const SG::ReadHandleKey<T_Cont> &key,
                                                 SG::auxid_t auxid,
                                                 const SG::ReadDecorHandleKey<T_Cont> *decor_key=nullptr) const {
         if (decor_key) {
            return std::make_unique<GenAccessor<T_Cont,
                                                typename DecorHelper<T_Cont, T>::Kit,
                                                T_ScalarVectorHelper> >(key,
                                                                      typename DecorHelper<T_Cont, T>::Kit(*decor_key),
                                                                      T_variable_type);
         }
         else {
            return std::make_unique<GenAccessor<T_Cont,
                                                typename AccessorHelper<T_Cont,T>::Kit,
                                                T_ScalarVectorHelper> >(key,
                                                                        typename AccessorHelper<T_Cont,T>::Kit(auxid),
                                                                        T_variable_type);
         }
      }
   };



   /** Class which creates accessors for the specified xAOD object content (singleton).
    * The accessors will either use xAOD accessor access or read decoration handles.
    */
   //   class AccessorFactory;
   class AccessorFactory : public Singleton<AccessorFactory> {
   public:
      AccessorFactory() {
         m_kits.insert( std::make_pair(typeid(float).hash_code(),          std::make_unique<AccessorKit<float,IProxyLoader::VT_VECDOUBLE> >()));
         m_kits.insert( std::make_pair(typeid(double).hash_code(),         std::make_unique<AccessorKit<double,IProxyLoader::VT_VECDOUBLE> >()));
         m_kits.insert( std::make_pair(typeid(bool).hash_code(),           std::make_unique<AccessorKit<bool,IProxyLoader::VT_VECINT> >()));
         m_kits.insert( std::make_pair(typeid(unsigned char).hash_code(),  std::make_unique<AccessorKit<unsigned char,IProxyLoader::VT_VECINT> >()));
         m_kits.insert( std::make_pair(typeid(unsigned short).hash_code(), std::make_unique<AccessorKit<unsigned short,IProxyLoader::VT_VECINT> >()));
         m_kits.insert( std::make_pair(typeid(unsigned int).hash_code(),   std::make_unique<AccessorKit<unsigned int,IProxyLoader::VT_VECINT> >()));
         m_kits.insert( std::make_pair(typeid(unsigned long).hash_code(),  std::make_unique<AccessorKit<unsigned long,IProxyLoader::VT_VECINT> >())); // @TODO might get cropped if int interface is used
         m_kits.insert( std::make_pair(typeid(char).hash_code(),           std::make_unique<AccessorKit<char,IProxyLoader::VT_VECINT> >()));
         m_kits.insert( std::make_pair(typeid(short).hash_code(),          std::make_unique<AccessorKit<short,IProxyLoader::VT_VECINT> >()));
         m_kits.insert( std::make_pair(typeid(int).hash_code(),            std::make_unique<AccessorKit<int,IProxyLoader::VT_VECINT> >()));
         m_kits.insert( std::make_pair(typeid(long).hash_code(),           std::make_unique<AccessorKit<long,IProxyLoader::VT_VECINT> >()));          // @TODO might get cropped if int interface is used
      }

      /** create an accessor for the specified content of an AuxElement.*/
      std::unique_ptr<IAccessor> create(const SG::ReadHandleKey<SG::AuxElement> &key,
                                        SG::auxid_t auxid,
                                        const SG::ReadDecorHandleKey<SG::AuxElement> *decor_key=nullptr) const {
         return getKit(auxid).create(key,auxid, decor_key);
      }
      /** create an accessor for the specified content of an AuxVectorBase.*/
      std::unique_ptr<IAccessor> create(const SG::ReadHandleKey<SG::AuxVectorBase> &key,
                                        SG::auxid_t auxid,
                                        const SG::ReadDecorHandleKey<SG::AuxVectorBase> *decor_key=nullptr) const {
         return getKit(auxid).create(key,auxid, decor_key);
      }

   private:

      /** Get an auxiliary class which creates an accessor for the specified content.
       */
      const IAccessorKit &getKit(SG::auxid_t auxid) const {
         const std::type_info* the_type_info = SG::AuxTypeRegistry::instance().getType (auxid);
         if (!the_type_info) {
            std::stringstream msg;
            msg << "ExpressionParsing::AccessorFactory: no type information about " << auxid
                << " (" <<  SG::AuxTypeRegistry::instance().getName(auxid) << ")";
            throw std::runtime_error(msg.str());
         }
         std::map<std::size_t, std::unique_ptr<IAccessorKit> >::const_iterator
            iter = m_kits.find(the_type_info->hash_code());
         if (iter == m_kits.end()) {
            std::stringstream msg;
            msg << "ExpressionParsing::AccessorFactory: no type information about " << auxid
                << " (" <<  SG::AuxTypeRegistry::instance().getName(auxid) << ")";
            throw std::runtime_error(msg.str());
         }
         return *(iter->second);
      }
      std::map<std::size_t, std::unique_ptr<IAccessorKit> > m_kits;
   };

}
#endif
