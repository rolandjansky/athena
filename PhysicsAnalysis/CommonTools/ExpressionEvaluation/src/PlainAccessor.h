/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ExpressionParsing_PlainAccessor_h_
#define ExpressionParsing_PlainAccessor_h_

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/EventContext.h"
#include "ExpressionEvaluation/IAccessor.h"

#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <stdexcept>

#include "GenAccessor.h"
#include "Utils.h"

namespace ExpressionParsing {
   /** Auxiliary class to handle method calls of "scalar" containers (AuxElement).
    */
   template <typename T_src>
   class StdVectorHelper {
   public:
      std::size_t size(const std::vector<T_src> &cont) const {
         return cont.size(); //@TODO correct ? or 1 ?
      }

      /// Get the scalar provided by the container
      T_src get(const std::vector<T_src> &cont) const {
         (void) cont;
         return cont[0];
      }

      /// Get the specified element of the vector provided by the container
      T_src get(const std::vector<T_src> &cont, std::size_t idx) const {
         (void) cont;
         (void) idx;
         return cont[idx];
      }

      /** Auxiliary class to create the corresponding auxiliary helper object
       */
      class Kit {
      public:
         StdVectorHelper<T_src> create(const EventContext& ctx, SG::ReadHandle<std::vector<T_src> > &handle) const {
            (void) ctx;
            (void) handle;
            return StdVectorHelper<T_src>();
         }
      };
   };

   template <typename T_src>
   class PlainValueHelper {
   public:
      std::size_t size(const T_src &cont) const {
         (void) cont;
         return 1; //@TODO correct ? or 1 ?
      }

      /// Get the scalar provided by the container
      T_src get(const T_src &cont) const {
         (void) cont;
         return cont;
      }

      /// Get the specified element of the vector provided by the container
      T_src get(const T_src &cont, std::size_t idx) const {
         (void) cont;
         (void) idx;
         assert( idx == 0);
         return cont;
      }

      /** Auxiliary class to create the corresponding auxiliary helper object
       */
      class Kit {
      public:
         PlainValueHelper<T_src> create(const EventContext& ctx, SG::ReadHandle<T_src > &handle) const {
            (void) ctx;
            (void) handle;
            return PlainValueHelper<T_src>();
         }
      };
   };

   class PlainAccessorFactory : public Singleton<PlainAccessorFactory> {
   public:
      class IKit {
      public:
         virtual ~IKit() {}
         virtual std::unique_ptr<IAccessor> createAccessor( const std::any &handle_key) const = 0;
         virtual bool registerReadKey(std::unordered_map<std::string, std::any > &read_keys,
                                      SGxAODProxyLoader::IParentHelper &parent,
                                      const std::string &var_name,
                                      std::vector<Gaudi::DataHandle *> &new_input_handles,
                                      bool verbose) const = 0;
      };


      static void throwFailedToAddHandle(const std::string &var_name) {
         std::stringstream msg;
         msg << "Failed to add read handle key for " << var_name;
         throw std::runtime_error(msg.str());
      }

      template <class T_Cont>
      static bool registerReadKey(std::unordered_map<std::string, std::any > &read_keys,
                                  SGxAODProxyLoader::IParentHelper &parent,
                                  const std::string &var_name,
                                  std::vector<Gaudi::DataHandle *> &new_input_handles,
                                  bool verbose) {
         SG::ReadHandleKey< T_Cont > key(var_name);
         std::pair<std::unordered_map<std::string, std::any >::iterator, bool> ret = read_keys.insert(std::make_pair(var_name, std::move(key)));
         if (!ret.second) {
            if (ret.first != read_keys.end()
                && ret.first->first == var_name
                && ret.first->second.type().hash_code() == typeid(key).hash_code()) {
               return true;
            }
            PlainAccessorFactory::throwFailedToAddHandle(var_name);
         }
         SG::ReadHandleKey< T_Cont > *key_final( std::any_cast<SG::ReadHandleKey< T_Cont > >( &ret.first->second));
         if (!key_final) {
            PlainAccessorFactory::throwFailedToAddHandle(var_name);
         }
         if (parent.declare(*key_final).isFailure()) {
            PlainAccessorFactory::throwFailedToAddHandle(var_name);
         }
         if (verbose) dumpDeclare(key_final);
         initializeHandle(key_final, new_input_handles);
         if (verbose) {
            std::cout << "DEBUG PlainAccessorFactory::registerReadKey register read handle key for " << var_name << " [" << typeid(T_Cont).name() << "]."<< std::endl;
         }
         return true;
      }

      template <typename T, IProxyLoader::VariableType T_variable_type>
      class StdVectorKit : public IKit {
      public:
         virtual std::unique_ptr<IAccessor> createAccessor( const std::any &handle_key) const override {
            const SG::ReadHandleKey<std::vector<T> > *key = std::any_cast<SG::ReadHandleKey<std::vector<T> > >(&handle_key);
            assert( key );
            return std::make_unique<GenAccessor<std::vector<T>,
                                                typename StdVectorHelper<T>::Kit,
                                                VectorHelper> >(*key,
                                                                typename StdVectorHelper<T>::Kit(),
                                                                T_variable_type);
         }
         virtual bool registerReadKey(std::unordered_map<std::string, std::any > &read_keys,
                                      SGxAODProxyLoader::IParentHelper &parent,
                                      const std::string &var_name,
                                      std::vector<Gaudi::DataHandle *> &new_input_handles,
                                      bool verbose) const override {
            return PlainAccessorFactory::registerReadKey<std::vector<T> >(read_keys, parent, var_name, new_input_handles, verbose);
         }
      };
      template <typename T, IProxyLoader::VariableType T_variable_type>
      class PlainValueKit : public IKit {
      public:
         virtual std::unique_ptr<IAccessor> createAccessor( const std::any &handle_key) const override {
            const SG::ReadHandleKey<T > *key = std::any_cast<SG::ReadHandleKey<T > >(&handle_key);
            assert( key );
            return std::make_unique<GenAccessor<T,
                                                typename PlainValueHelper<T>::Kit,
                                                ScalarHelper> >(*key,
                                                                typename PlainValueHelper<T>::Kit(),
                                                                T_variable_type);
         }
         virtual bool registerReadKey(std::unordered_map<std::string, std::any > &read_keys,
                                      SGxAODProxyLoader::IParentHelper &parent,
                                      const std::string &var_name,
                                      std::vector<Gaudi::DataHandle *> &new_input_handles,
                                      bool verbose) const override {
            return PlainAccessorFactory::registerReadKey<T >(read_keys, parent, var_name, new_input_handles, verbose);
         }
      };

      template <typename T,IProxyLoader::VariableType T_variable_type>
      void registerStdVectorKit() {
         m_kits.insert( std::make_pair(typeid(std::vector<T>).hash_code(), std::make_unique< StdVectorKit<T,T_variable_type> >()));
         m_kits.insert( std::make_pair(typeid(SG::ReadHandleKey<std::vector<T> >).hash_code(), std::make_unique< StdVectorKit<T,T_variable_type> >()));
      }
      template <typename T,IProxyLoader::VariableType T_variable_type>
      void registerPlainValueKit() {
         m_kits.insert( std::make_pair(typeid(SG::ReadHandleKey<T>).hash_code(), std::make_unique<PlainValueKit<T,T_variable_type> >()));
         m_kits.insert( std::make_pair(typeid(T).hash_code(), std::make_unique<PlainValueKit<T,T_variable_type> >()));
      }

      PlainAccessorFactory() {
         //         registerPlainValueKit<bool,IProxyLoader::VT_INT>();
         registerPlainValueKit<bool,IProxyLoader::VT_INT>();
         registerPlainValueKit<int,IProxyLoader::VT_INT>();
         registerPlainValueKit<unsigned int,IProxyLoader::VT_INT>();
         registerPlainValueKit<float,IProxyLoader::VT_DOUBLE>();
         registerPlainValueKit<double,IProxyLoader::VT_DOUBLE>();

         registerStdVectorKit<bool,IProxyLoader::VT_VECINT>();
         registerStdVectorKit<int,IProxyLoader::VT_VECINT>();
         registerStdVectorKit<unsigned int,IProxyLoader::VT_VECINT>();
         registerStdVectorKit<float,IProxyLoader::VT_VECDOUBLE>();
         registerStdVectorKit<double,IProxyLoader::VT_VECDOUBLE>();
      }
      std::unique_ptr<IAccessor> createAccessor( const std::any &handle_key) const {
         std::unordered_map<std::size_t,std::unique_ptr<IKit> >::const_iterator
            kit_iter=m_kits.find(handle_key.type().hash_code());
         if (kit_iter == m_kits.end()) {
            std::stringstream msg;
            msg << "ExpressionParsing::PlainAccessorFactory: no kit for type " << handle_key.type().name();
            throw std::runtime_error(msg.str());
         }
         return kit_iter->second->createAccessor(handle_key);
      }
      bool registerReadKey(std::unordered_map<std::string, std::any > &read_keys,
                           const std::type_info &the_type,
                           SGxAODProxyLoader::IParentHelper &parent,
                           const std::string &var_name,
                           std::vector<Gaudi::DataHandle *> &new_input_handles,
                           bool verbose=false) {
         std::unordered_map<std::size_t,std::unique_ptr<IKit> >::const_iterator
            kit_iter=m_kits.find(the_type.hash_code());
         if (kit_iter == m_kits.end()) {
            if (verbose) {
               std::cout << "DEBUG PlainAccessorFactory::createReadKey no matching kit for "  << the_type.name() << " " << the_type.hash_code() << std::endl;
               for (const std::pair<const std::size_t,std::unique_ptr<IKit> > &elm : m_kits) {
                  std::cout << "DEBUG PlainAccessorFactory::createReadKey have "  << elm.first << std::endl;
               }
            }
            return false;
         }
         return kit_iter->second->registerReadKey(read_keys, parent, var_name, new_input_handles, verbose);
      }
   private:
      std::unordered_map<std::size_t,std::unique_ptr<IKit> > m_kits;
   };

}
#endif
