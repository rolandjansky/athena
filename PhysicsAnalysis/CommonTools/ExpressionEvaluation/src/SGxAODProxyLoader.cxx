/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SGxAODProxyLoader.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////
// for debugging : #define DEBUG_VARIABLE_LOADING

#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthenaKernel/BaseInfo.h"


#include "Utils.h"
#include "DebugUtils.h"
#include "BaseAccessor.h"
#include "xAODAccessor.h"
#include "MethodAccessor.h"
#include "PlainAccessor.h"

namespace ExpressionParsing {
  SGxAODProxyLoader::SGxAODProxyLoader(StoreGateSvc_t &evtStore, bool verbose) : m_evtStore(evtStore), m_emptyVectorAccessor(std::make_unique<EmptyVectorAccessor>()),m_verbose(verbose) { }

  SGxAODProxyLoader::~SGxAODProxyLoader()
  {
  }

  void  SGxAODProxyLoader::reset() {
     m_decorKeys.clear();
     m_readKeys.clear();
     m_accessor.clear();
  }

  void SGxAODProxyLoader::splitVarnameIntoContainerAndMethod(const std::string &varname, std::string &containerName, std::string &methodName) const
  {
    size_t dotPosition = varname.find('.');
    containerName = varname.substr(0, dotPosition);
    methodName = varname.substr(dotPosition + 1);
  }

   std::pair<std::unique_ptr<TMethodCall>, TVirtualCollectionProxy *>
   SGxAODProxyLoader::getMethodCallAccessor(const std::string &method_name, const std::type_info &info) const {
     if (m_verbose) {
        std::cout << "DEBUG SGxAODProxyLoader search for " << method_name << " in type " << info.name() << std::endl;
     }
     TClass *containerClass = TClass::GetClass(info);
     if (!containerClass) {
        if (m_verbose) {
           std::cout << "DEBUG SGxAODProxyLoader search for " << method_name << " in normalisze type " << SG::normalizedTypeinfoName(info) << std::endl;
        }
        containerClass = TClass::GetClass(SG::normalizedTypeinfoName(info).c_str());
        if (!containerClass) {
           std::stringstream msg;
           msg << "No dictionary for " << info.name() << std::endl;
           throw std::runtime_error(msg.str());
        }
     }
     TClass *elementClass=nullptr;
     TVirtualCollectionProxy *collection_proxy=nullptr;
     if( strcmp(containerClass->GetName(),"SG::AuxElementStandaloneData")==0 ) {
        elementClass=containerClass;
     }
     else {
        collection_proxy = containerClass->GetCollectionProxy();
        elementClass = (collection_proxy ? collection_proxy->GetValueClass()  : nullptr);
        if (!elementClass) {
           std::stringstream msg;
           msg << "No collection proxy or element class for " << info.name() << std::endl;
           throw std::runtime_error(msg.str());
        }
     }
     std::unique_ptr<TMethodCall> method_call = std::make_unique<TMethodCall>(elementClass, method_name.c_str(), "");
     if (!method_call || !method_call->IsValid()) {
        std::stringstream msg;
        msg << "No valid method " << method_name << " for " << info.name() << std::endl;
        throw std::runtime_error(msg.str());
     }
     if (m_verbose && method_call->GetMethod()) {
        // @TODO check signature "() const
        std::cout << "DEBUG SGxAODProxyLoader got method " << " . " << method_name << " : "
                  << method_call->GetMethod()->GetReturnTypeNormalizedName ()
                  << " proto=" << method_call->GetProto()
                  << " signature=" << method_call->GetMethod()->GetSignature ()
                  << " proto=" << method_call->GetMethod()->GetPrototype()
                  << std::endl;
     }
     return std::make_pair( std::move( method_call), collection_proxy);
  }


   template <class T_Aux>
   std::unique_ptr<IAccessor> SGxAODProxyLoader::createAccessor(const EventContext& ctx,
                                                                const SG::ReadHandleKey<T_Aux> &key,
                                                                const SG::ReadDecorHandleKey<T_Aux> *decor_key,
                                                                SG::auxid_t method_id,
                                                                const std::string &method_name) const
   {
      SG::ReadHandle<T_Aux> handle(key, ctx);
      if (!handle.isValid()) {
         std::stringstream msg;
         msg << "Failed to get " << key.key();
            throw std::runtime_error(msg.str());
      }
      if (getContainerSize(*handle)==0) {
         return std::unique_ptr<IAccessor>();
      }
      if (!isAvailable(*handle,method_id)) {
         std::pair<std::unique_ptr<TMethodCall>,TVirtualCollectionProxy *> method = getMethodCallAccessor(method_name, typeid(*handle));
         return MethodAccessorFactory::instance().create( key, std::move(method.first), method.second);
      }
      else {
         return ExpressionParsing::AccessorFactory::instance().create( key, method_id , decor_key );
      }
   }

  IAccessor &SGxAODProxyLoader::computeClassForVarname(const EventContext& ctx,
                                                       std::unordered_map<std::string, CxxUtils::CachedUniquePtrT<IAccessor> >::const_iterator accessor_iter,
                                                       const std::string &varname) const
  {
    const IProxyDict *proxy = Atlas::getExtendedEventContext( ctx ).proxy();
    (void) proxy;
    std::string container_name;
    std::string method_name;
    splitVarnameIntoContainerAndMethod(varname,container_name, method_name);
    ReadHandleMap::const_iterator key_iter = m_readKeys.find(container_name);
    if (key_iter == m_readKeys.end()) {
       std::stringstream msg;
       msg << "No read handle key for " << container_name;
       throw std::runtime_error(msg.str());
    }
    SG::auxid_t method_id = SG::null_auxid;
    ReadDecorHandleMap::const_iterator decor_key_iter = m_decorKeys.end();
    if  (ReadHandleMap::isVector(key_iter->second) || ReadHandleMap::isElement(key_iter->second)) {
       method_id = SG::AuxTypeRegistry::instance().findAuxID(method_name,"");
       if (method_id != SG::null_auxid) {
          decor_key_iter=m_decorKeys.find(varname);
       }
    }
    std::unique_ptr<IAccessor> accessor;
    if (ReadHandleMap::isVector(key_iter->second)) {
       if (m_verbose) { std::cout << "DEBUG SGxAODProxyLoader::computeClassForVarname is Vector: " << varname << std::endl; }
       accessor = createAccessor(ctx,
                                 ReadHandleMap::vectorKey(key_iter->second),
                                 (decor_key_iter != m_decorKeys.end() ? &ReadDecorHandleMap::vectorKey(decor_key_iter->second) : nullptr),
                                 method_id,
                                 method_name);
    }
    else if (ReadHandleMap::isElement(key_iter->second)) {
       if (m_verbose) { std::cout << "DEBUG SGxAODProxyLoader::computeClassForVarname is element: " << varname << std::endl; }
       accessor = createAccessor(ctx,
                                 ReadHandleMap::elementKey(key_iter->second),
                                 (decor_key_iter != m_decorKeys.end() ? &ReadDecorHandleMap::elementKey(decor_key_iter->second) : nullptr),
                                 method_id,
                                 method_name);
    }
    else {
       if (m_verbose) { std::cout << "DEBUG SGxAODProxyLoader::computeClassForVarname try to create plain type accessor for " << varname << std::endl; }
       accessor = PlainAccessorFactory::instance().createAccessor(key_iter->second);
    }
    if (!accessor) {
       return *m_emptyVectorAccessor;
    }
    if (m_verbose) { std::cout << "DEBUG SGxAODProxyLoader use accessor " << typeid(*accessor).name() << " for " <<varname << std::endl; }
    accessor_iter->second.set(std::move(accessor));
    return *(accessor_iter->second);
  }

  IProxyLoader::VariableType SGxAODProxyLoader::variableTypeFromString(const std::string &varname) {
     const EventContext& ctx = Gaudi::Hive::currentContext();
     return getAccessor(ctx, varname).variableType();
  }

   namespace {
      template <typename T>
      std::ostream &operator<<(std::ostream &out, const std::vector<T> &vec) {
         for (auto elm : vec) {
            std::cout << " " << elm;
         }
         return out;
      }
      template<class T>
      T dump(const std::string &varname, T val) {
         (void) varname;
#ifdef DEBUG_VARIABLE_LOADING
         std::cout << "DEBUG SGxAODProxyLoader::dump " <<  varname << val << std::endl;
#endif
         return val;
      }
   }

  int SGxAODProxyLoader::loadIntVariableFromString(const std::string &varname)
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    return dump(varname,getAccessor(ctx, varname).loadInt(ctx));
  }

  double SGxAODProxyLoader::loadDoubleVariableFromString(const std::string &varname)
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    return dump(varname,getAccessor(ctx, varname).loadDouble(ctx));
  }

  std::vector<int> SGxAODProxyLoader::loadVecIntVariableFromString(const std::string &varname)
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    return dump(varname,getAccessor(ctx, varname).loadVecInt(ctx));
  }

  std::vector<double> SGxAODProxyLoader::loadVecDoubleVariableFromString(const std::string &varname)
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    return dump(varname,getAccessor(ctx, varname).loadVec(ctx));
  }

  bool SGxAODProxyLoader::updateDataDependencies(SGxAODProxyLoader::IParentHelper &parent,
                                                 const std::vector<std::string> &var_names,
                                                 const std::vector<const DataObjID *> &input_data_in,
                                                 const std::vector<const DataObjID *> &output_data_in,
                                                 std::vector<Gaudi::DataHandle *> &new_input_handles,
                                                 std::vector<Gaudi::DataHandle *> &new_output_handles)
  {
     (void) new_output_handles;
     std::size_t initial_size=m_decorKeys.size()+m_readKeys.size();
     for (const std::string &var_name : var_names) {
        if (   m_decorKeys.find(var_name)!= m_decorKeys.end()) continue;
        std::string container_name;
        std::string method_name;
        splitVarnameIntoContainerAndMethod(var_name,container_name, method_name);
        if (m_verbose) {
           std::cout << "DEBUG SGxAODProxyLoader::updateDataDependencies var " << var_name << " -> "  << container_name << " . " << method_name
                     << " i:" << input_data_in.size() << " o:" << output_data_in.size()
                     << std::endl;
        }
        const DataObjID *container_data_id=nullptr;
        const DataObjID *method_data_id=nullptr;
        bool verbose = m_verbose;

        // Decide whether a variable just needs a container read handle key or a read decor handle key.
        // Search in data handles for data handles and decor handles:
        //   decor handle : "container_name.decoration_name"
        //   data handle  : "container_name"
        // if a decor handle is found assume that it is a decoration, otherwise assume that it is either a default aux item
        // or refers to  a method call
        auto set_ids = [&method_data_id,&container_data_id,&var_name,&container_name,verbose](const DataObjID *obj_data_id) -> bool {
                              std::string_view handle_key(obj_data_id->key());
                              std::string_view handle_full_key(handle_key);
                              std::string::size_type pos=obj_data_id->key().find("+");
                              pos =  (pos==std::string::npos  ?  0 : pos+1);
                              handle_key=handle_key.substr(pos,handle_key.size()-pos);
                              if (verbose && (handle_key == container_name || handle_key == var_name)) {
                                 const SG::BaseInfoBase* base_info = SG::BaseInfoBase::find (obj_data_id->clid());
                                 std::cout << "DEBUG SGxAODProxyLoader::updateDataDependencies " << handle_key << " matches " << var_name <<  " ? "
                                           << (handle_key == var_name ? " method " : "")
                                           << (handle_key == container_name ? " container " : "")
                                           << "-> " << (base_info ? base_info->typeinfo().name() : "")
                                           << std::endl;
                              }
                              if (handle_key == var_name) {
                                 method_data_id = obj_data_id;
                                 if (container_data_id) return true;
                              }
                              else if (handle_key == container_name ) {
                                 container_data_id = obj_data_id;
                                 if (method_data_id) return true;
                              }
                              return false;
                           };
        // start with output handles which are expected to contain the decorator handles
        for( std::vector<const DataObjID *>::const_iterator elm_iter = output_data_in.begin(); elm_iter != output_data_in.end() && !set_ids(*elm_iter); ++elm_iter) {}
        if (!method_data_id) {
           for( std::vector<const DataObjID *>::const_iterator elm_iter = input_data_in.begin(); elm_iter != input_data_in.end() && !set_ids(*elm_iter); ++elm_iter) {}
        }
        if (method_data_id) {
           if (!container_data_id) {
              container_data_id=method_data_id;
           }
        }
        // ---DEBUG
        if (m_verbose && method_data_id && method_data_id != container_data_id) {
           std::cout << "DEBUG SGxAODProxyLoader method_clid "    << method_data_id->clid() << std::endl;
        }
        if (m_verbose && container_data_id) {
           std::cout << "DEBUG SGxAODProxyLoader container_clid " << container_data_id->clid() << std::endl;
        }
        // ___DEBUG

        if (container_data_id) {
           m_accessor.insert( std::make_pair(var_name, CxxUtils::CachedUniquePtrT<IAccessor>()));
           bool have_container_read_key = m_readKeys.find(container_name) != m_readKeys.end();
           if (have_container_read_key && !method_data_id) continue;

           auto clid = container_data_id->clid();
           (void) clid;
           const SG::BaseInfoBase* base_info = (container_data_id ? SG::BaseInfoBase::find (container_data_id->clid()) : nullptr);
           if (!base_info) {
              std::stringstream msg;
              msg << "Missing type information about container  " << container_name << ".";
              throw std::runtime_error(msg.str());
           }
           // ---DEBUG
           if (m_verbose && base_info) {
              std::cout << "DEBUG SGxAODProxyLoader " << container_name << " -> " << base_info->typeinfo().name() << std::endl;
              for ( CLID clid :  base_info->get_bases ()) {
                 const SG::BaseInfoBase* a_base_info = SG::BaseInfoBase::find (clid);
                 std::cout << "DEBUG SGxAODProxyLoader " << container_name << " base: " << a_base_info->typeinfo().name() << std::endl;
              }
           }
           // ___DEBUG


           std::any read_key;
           std::any decor_key;
           if (base_info->is_base(ClassID_traits<SG::AuxVectorBase>::ID())) {
              if (!have_container_read_key) {
                 read_key = std::make_any<SG::ReadHandleKey<SG::AuxVectorBase> >(container_name);
              }
              if (method_data_id) {
                 decor_key = std::make_any<SG::ReadDecorHandleKey<SG::AuxVectorBase> >(var_name);
              }
           }
           else if (base_info->is_base(ClassID_traits<SG::AuxElement>::ID())) {
              if (!have_container_read_key) {
                 read_key = std::make_any<SG::ReadHandleKey<SG::AuxElement> >(container_name);
              }
              if (method_data_id) {
                 decor_key = std::make_any<SG::ReadDecorHandleKey<SG::AuxElement> >(var_name);
              }
           }
           else {
              if (method_data_id && method_data_id != container_data_id && method_data_id->clid() != container_data_id->clid()) {
                 std::stringstream msg;
                 msg << "Did not expect a \"method\" handle for the plain type " << var_name;
                 throw std::logic_error(msg.str());
              }
              bool registered_key =  PlainAccessorFactory::instance().registerReadKey(m_readKeys, base_info->typeinfo(), parent, var_name, new_input_handles, m_verbose);
              if (registered_key) {
                 continue;
              }
           }

           if (!have_container_read_key) {
              if (!read_key.has_value()) {
                 std::stringstream msg;
                 msg << "Container  " << container_name << " is of unsupported type " << base_info->typeinfo().name()
                     << " expected a type derived from SG::AuxVectorBase or SG::AuxElement";
                 throw std::runtime_error(msg.str());
              }
              std::pair<ReadHandleMap::iterator, bool> ret = m_readKeys.insert(std::make_pair(container_name, std::move(read_key)));
              if (ret.second) {
                 if (   (ReadHandleMap::isVector(ret.first->second)  && parent.declare(ReadHandleMap::vectorKey(ret.first->second)).isFailure())
                     || (ReadHandleMap::isElement(ret.first->second) && parent.declare(ReadHandleMap::elementKey(ret.first->second)).isFailure())) {
                    std::stringstream msg;
                    msg << "Failed to add read handle key for " << container_name;
                    throw std::runtime_error(msg.str());
                 }
                 if (m_verbose) {
                    if (ReadHandleMap::isVector(ret.first->second)) dumpDeclare(&ReadHandleMap::vectorKey(ret.first->second) );
                    else                                            dumpDeclare(&ReadHandleMap::elementKey(ret.first->second));
                 }
                 if (ReadHandleMap::isVector(ret.first->second)) initializeHandle(&ReadHandleMap::vectorKey(ret.first->second), new_input_handles);
                 else                                            initializeHandle(&ReadHandleMap::elementKey(ret.first->second),new_input_handles);

              }
           }
           if (method_data_id) {
              if (!decor_key.has_value()) {
                 std::stringstream msg;
                 msg << "Container  " << container_name << " is of unsupported type " << base_info->typeinfo().name()
                     << " expected a type derived from SG::AuxVectorBase or SG::AuxElement";
                 throw std::runtime_error(msg.str());
              }
              std::pair<ReadDecorHandleMap::iterator, bool> ret = m_decorKeys.insert(std::make_pair(var_name,std::move(decor_key)));
              if (ret.second) {
                 if (   (ReadDecorHandleMap::isVector(ret.first->second)  && parent.declare(ReadDecorHandleMap::vectorKey(ret.first->second)).isFailure())
                     || (!ReadDecorHandleMap::isVector(ret.first->second) && parent.declare(ReadDecorHandleMap::elementKey(ret.first->second)).isFailure())) {
                    std::stringstream msg;
                    msg << "Failed to add read decor handle key for " << var_name;
                    throw std::runtime_error(msg.str());
                 }
                 if (m_verbose) {
                    if (ReadDecorHandleMap::isVector(ret.first->second)) dumpDeclare(&ReadDecorHandleMap::vectorKey(ret.first->second) );
                    else                                                 dumpDeclare(&ReadDecorHandleMap::elementKey(ret.first->second));
                 }
                 if (ReadDecorHandleMap::isVector(ret.first->second)) initializeHandle(&ReadDecorHandleMap::vectorKey(ret.first->second),new_input_handles );
                 else                                                 initializeHandle(&ReadDecorHandleMap::elementKey(ret.first->second),new_input_handles);
              }
           }
        }
     }
     return initial_size != m_decorKeys.size()+m_readKeys.size();
  }
}
