/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _ExpressionEvaluation_MethodAccessor_h_
#define _ExpressionEvaluation_MethodAccessor_h_

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "ExpressionEvaluation/IProxyLoader.h"
#include "ExpressionEvaluation/IAccessor.h"

#include "TClass.h"
#include "AthContainers/normalizedTypeinfoName.h"
#include "TMethodCall.h"
#include "TVirtualCollectionProxy.h"
#include "TFunction.h"

#include <memory>
#include <map>
#include <stdexcept>
#include <sstream>
#include <cassert>

namespace ExpressionParsing {

   /** Auxiliary class to handle method calls of vector containers (AuxVectorBase)
    */
   template <class T_Cont,typename T_src>
   class CollectionMethodHelper {
   public:
      CollectionMethodHelper(TMethodCall &method_call, const TVirtualCollectionProxy &collection_proxy, const void *data, unsigned int n_elements)
         : m_methodCall( &method_call),
           m_collectionProxy( collection_proxy.Generate())
      {
         (void) n_elements;
         assert( m_methodCall->IsValid());
         m_collectionProxy->PushProxy(const_cast<void *>(data));
         assert( m_collectionProxy->Size() == n_elements);
      }

      std::size_t size(const T_Cont &cont) const {
         (void) cont;
         return m_collectionProxy->Size();
      }

      /// Get the scalar provided by the container
      T_src get(const T_Cont &cont) const {
         (void) cont;
         T_src ret;
         assert( m_collectionProxy->Size() == 1);
         // @TODO root/cling lock needed  ?
         m_methodCall->Execute((*m_collectionProxy)[0], ret); // @TODO thread safe ?
         return ret;
      }

      /// Get the specified element of the vector provided by the container
      T_src get(const T_Cont &cont, std::size_t idx) const {
         (void) cont;
         T_src ret;
         // @TODO root/cling lock needed  ?
         void *element_data=(*m_collectionProxy)[idx];
         m_methodCall->Execute(element_data, ret); // @TODO thread safe ?
         return ret;
      }

      /** Auxiliary class to create the corresponding auxiliary helper object
       */
      class Kit {
      public:
         Kit(Kit &&) = default;
         Kit(std::unique_ptr<TMethodCall> &&method_call, TVirtualCollectionProxy &collection_proxy)
            : m_methodCall(std::move(method_call)),
              m_collectionProxy(&collection_proxy)
         {}

         CollectionMethodHelper<T_Cont,T_src> create(const EventContext& ctx, SG::ReadHandle<T_Cont> &handle) const {
            (void) ctx;
            return CollectionMethodHelper<T_Cont,T_src>(*m_methodCall,*m_collectionProxy, getVectorData(*handle), getContainerSize(*handle));
         }
      private:
         std::unique_ptr<TMethodCall>  m_methodCall;
         const TVirtualCollectionProxy *m_collectionProxy;
      };

   private:
      TMethodCall                             *m_methodCall;
      std::unique_ptr<TVirtualCollectionProxy> m_collectionProxy;
   };

   /** Auxiliary class to handle method calls of "scalar" containers (AuxElement).
    */
   template <class T_Cont,typename T_src>
   class MethodHelper {
   public:
      MethodHelper(TMethodCall &method_call, const void *data)
         : m_methodCall( &method_call),
           m_data(data)
      {
         assert( m_methodCall->IsValid());
      }

      std::size_t size(const T_Cont &cont) const {
         return getContainerSize(cont); //@TODO correct ? or 1 ?
      }

      /// Get the scalar provided by the container
      T_src get(const T_Cont &cont) const {
         (void) cont;
         T_src ret;
         // @TODO root/cling lock needed  ?
         m_methodCall->Execute(const_cast<void *>(m_data), ret);
         return ret;
      }

      /// Get the specified element of the vector provided by the container
      T_src get(const T_Cont &cont, std::size_t idx) const {
         (void) cont;
         (void) idx;
         T_src ret;
         assert( idx==0);
         // @TODO root/cling lock needed  ?
         m_methodCall->Execute(const_cast<void *>(m_data), ret);
         return ret;
      }

      /** Auxiliary class to create the corresponding auxiliary helper object
       */
      class Kit {
      public:
         Kit(Kit &&) = default;
         Kit(std::unique_ptr<TMethodCall> &&method_call) : m_methodCall(std::move(method_call)) {}

         MethodHelper<T_Cont,T_src> create(const EventContext& ctx, SG::ReadHandle<T_Cont> &handle) const {
            (void) ctx;
            return MethodHelper<T_Cont,T_src>(*m_methodCall,handle.cptr());
         }
      private:
         std::unique_ptr<TMethodCall> m_methodCall;
      };
   private:
      TMethodCall *m_methodCall;
      const void  *m_data;
   };

   /** Class to create accessor which call methods of an AuxElement of an AuxVectorBase container (singleton).
    */
   class MethodAccessorFactory : public Singleton<MethodAccessorFactory> {
   private:
      class IMethodAccessorKit {
         friend class MethodAccessorFactory;
      public:
         virtual ~IMethodAccessorKit() {}
         virtual std::unique_ptr<IAccessor> create( const SG::ReadHandleKey<SG::AuxVectorBase> &key,
                                                    std::unique_ptr<TMethodCall> &&method_call,
                                                    TVirtualCollectionProxy *proxy=nullptr) const = 0;
         virtual std::unique_ptr<IAccessor> create( const SG::ReadHandleKey<SG::AuxElement> &key,
                                                    std::unique_ptr<TMethodCall> &&method_call,
                                                    TVirtualCollectionProxy *proxy=nullptr) const = 0;
      };

      /** Auxiliary class to create a specific accessor which calls a method of an AuxElement or AuxVectorBase.
       */
      template <class T_src>
      class MethodAccessorKit : public IMethodAccessorKit {
      public:
         MethodAccessorKit(ExpressionParsing::IProxyLoader::VariableType scalar_type,
                           ExpressionParsing::IProxyLoader::VariableType vector_type)
            : m_scalarType(scalar_type),
              m_vectorType(vector_type)
         {}

         /** create an accessor which called the specified method of an AuxVectorBase.*/
         virtual std::unique_ptr<IAccessor> create( const SG::ReadHandleKey<SG::AuxVectorBase> &key,
                                                    std::unique_ptr<TMethodCall> &&method_call,
                                                    TVirtualCollectionProxy *proxy) const override {
            if (!proxy) {
               std::stringstream msg;
               msg << "Cannot use method access of types SG::AuxVectorBase without a collection proxy.";
               throw std::logic_error(msg.str());
            }
            return createAccessor<SG::AuxVectorBase, VectorHelper>(key,std::move(method_call),proxy,m_vectorType);
         }

         /** create an accessor which called the specified method of an AuxElement.*/
         virtual std::unique_ptr<IAccessor> create( const SG::ReadHandleKey<SG::AuxElement> &key,
                                                    std::unique_ptr<TMethodCall> &&method_call,
                                                    TVirtualCollectionProxy *proxy=nullptr) const override {
            if (proxy) {
               return createAccessor<SG::AuxElement,VectorHelper>(key,std::move(method_call),proxy, (!proxy ? m_scalarType : m_vectorType));
            }
            else {
               return createAccessor<SG::AuxElement,ScalarHelper>(key,std::move(method_call),proxy, (!proxy ? m_scalarType : m_vectorType));
            }
            // @TODO really vectorType and GenScalarAccessor if collection proxy is given ?
         }
      private:
         template <class T_Aux, class T_ScalarVectorHelper>
         std::unique_ptr<IAccessor> createAccessor( const SG::ReadHandleKey<T_Aux> &key,
                                                    std::unique_ptr<TMethodCall> &&method_call,
                                                    TVirtualCollectionProxy *proxy,
                                                    ExpressionParsing::IProxyLoader::VariableType variable_type) const {
            if (proxy) {
               return std::make_unique<GenAccessor<T_Aux,
                                                   typename CollectionMethodHelper<T_Aux,T_src>::Kit,
                                                   T_ScalarVectorHelper> >(key,
                                                                           typename CollectionMethodHelper<T_Aux,T_src>::Kit(std::move(method_call), *proxy),
                                                                           variable_type);
            }
            else {
               return std::make_unique<GenAccessor<T_Aux,
                                                   typename MethodHelper<T_Aux,T_src>::Kit,
                                                   T_ScalarVectorHelper> >(key,
                                                                           typename MethodHelper<T_Aux,T_src>::Kit(std::move(method_call)),
                                                                           variable_type);
            }
         }
         ExpressionParsing::IProxyLoader::VariableType m_scalarType;
         ExpressionParsing::IProxyLoader::VariableType m_vectorType;
      };

   public:
      MethodAccessorFactory() {
         m_kits.insert( std::make_pair(TInterpreter::EReturnType::kLong,std::make_unique<MethodAccessorKit<long> >(IProxyLoader::VT_INT,IProxyLoader::VT_VECINT)));
         m_kits.insert( std::make_pair(TInterpreter::EReturnType::kDouble,std::make_unique<MethodAccessorKit<double> >(IProxyLoader::VT_DOUBLE, IProxyLoader::VT_VECDOUBLE)));
      }

      /** Create an accessor which calls the specified method of an AuxElement.
       */
      std::unique_ptr<IAccessor> create(const SG::ReadHandleKey<SG::AuxElement> &key,
                                        std::unique_ptr<TMethodCall> &&method_call,
                                        TVirtualCollectionProxy *proxy=nullptr) const {
         return getKit(*method_call).create(key,std::move(method_call), proxy);
      }

      /** Create an accessor which calls the specified method of an AuxVectorBase.
       */
      std::unique_ptr<IAccessor> create(const SG::ReadHandleKey<SG::AuxVectorBase> &key,
                                        std::unique_ptr<TMethodCall> &&method_call,
                                        TVirtualCollectionProxy *proxy=nullptr) const {
         return getKit(*method_call).create(key,std::move(method_call), proxy);
      }

   private:

      /** Get an specific class which creates the accessor for the given method.
       */
      const IMethodAccessorKit &getKit(TMethodCall &method_call) const {
         std::map<TMethodCall::EReturnType, std::unique_ptr<IMethodAccessorKit> >::const_iterator
            iter = m_kits.find(method_call.ReturnType());
         if (iter == m_kits.end()) {
            std::stringstream msg;
            msg << "ExpressionParsing::MethodAccessorFactory: no kit for return type " << method_call.GetMethod()->GetReturnTypeNormalizedName ();
            throw std::runtime_error(msg.str());
         }
         return *(iter->second);
      }
      std::map<TMethodCall::EReturnType, std::unique_ptr<IMethodAccessorKit> > m_kits;
   };

}
#endif
