/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SGxAODProxyLoader.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef SG_XAOD_PROXY_LOADER_H
#define SG_XAOD_PROXY_LOADER_H

#include "ExpressionEvaluation/IProxyLoader.h"
#include "ExpressionEvaluation/xAODVariableProxyLoaders.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/StlVectorClids.h"
#include "SGTools/BuiltinsClids.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"
#include "AthContainers/AuxVectorBase.h"

#include "CxxUtils/CachedUniquePtr.h"
#include <any>

#include "IAccessor.h"

class TMethodCall;
class TVirtualCollectionProxy;

namespace SG {
   class AuxVectorBase;
   class AuxElement;
}
#include <map>

namespace ExpressionParsing {


  /** Auxiliary class to access content of xAOD objects referred to by variables of an expressions handled by an instance of the @ref ExpressionParser.
   */
  class SGxAODProxyLoader : public IProxyLoader {

    public:
      typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
      SGxAODProxyLoader(StoreGateSvc_t &evtStore, bool verbose=false);
      virtual ~SGxAODProxyLoader();

      virtual void reset();

      virtual IProxyLoader::VariableType variableTypeFromString(const std::string &varname);

      virtual int loadIntVariableFromString(const std::string &varname);
      virtual double loadDoubleVariableFromString(const std::string &varname);
      virtual std::vector<int> loadVecIntVariableFromString(const std::string &varname);
      virtual std::vector<double> loadVecDoubleVariableFromString(const std::string &varname);

     /** Interface of an auxiliary class to pass the parent, e.g. @ref AthCommonDataStore, to this loader to declare new data handles.
      */
      class IParentHelper {
      public:
         ~IParentHelper() {}
         /** This method needs to be implement to declare the given data handle.
          */
         virtual StatusCode declare(SG::VarHandleKey &handle) const = 0;
      };

     /** Template of am auxiliary class to declare the new handles in the parent AthAlgTool, AthAlgorithm etc.
      */
      template <class T>
      class ParentHelper : public IParentHelper {
      public:
         ParentHelper(T *parent) : m_parent(parent) {}
         virtual StatusCode declare(SG::VarHandleKey &handle) const override {
            m_parent->declare(handle);
            handle.setOwner(m_parent);
            return handle.initialize();
         }
      private:
         T *m_parent;
      };

     /** auxiliary method to create the auxiliary class to declare new data handles with the calling AthAlgTool, AthAlgorithm etc.
      */
      template <class T>
      static ParentHelper<T> wrapParent(T *parent) {
         return ParentHelper<T>(parent);
      }

     /** Create extra data dependencies arising eventually from the given variables.
      * @param parent auxiliary class to declare new data handles with the calling parent
      * @param var_names the variables for which data dependencies might be created.
      * @param input_data_in read data object IDs declared by all algorithms and tools
      * @param output_data_in write data object IDs declared by all algorithms and tools
      * @param new_input_handles will be filled with read data handles newly created by this loader
      * @param new_output_handles will be filled with write data handles newly created by this loader (unused).
      * @return true if new handles have been declared.
      */
      bool updateDataDependencies(SGxAODProxyLoader::IParentHelper &parent,
                                  const std::vector<std::string> &var_names,
                                  const std::vector<const DataObjID *> &input_data_in,
                                  const std::vector<const DataObjID *> &output_data_in,
                                  std::vector<Gaudi::DataHandle *> &new_input_handles,
                                  std::vector<Gaudi::DataHandle *> &new_output_handles);

    private:
      void splitVarnameIntoContainerAndMethod(const std::string &varname, std::string &containerName, std::string &methodName) const;

     /** Auxiliary method to create an xAOD object content accessor for the given variable name.
      * @param ctx the current event context
      * @param accessor_iter location where the newly created accessor will be stored.
      * @param varname the variable name which defines the xAOD container and the content.
      */
      IAccessor &computeClassForVarname(const EventContext& ctx,
                                        std::unordered_map<std::string, CxxUtils::CachedUniquePtrT<IAccessor> >::const_iterator accessor_iter,
                                        const std::string &varname) const;

     /** Auxiliary method to create a TMethodCall and eventually also a collection proxy to call the given method of the specified container type
      * @param method_name the name of the method to be called.
      * @param info the type_info of the container whose method is to be called.
      * @return a TMethodCall object and optionally collection proxy which is owned by ROOT(?)
      */
      std::pair<std::unique_ptr<TMethodCall>, TVirtualCollectionProxy *>
      getMethodCallAccessor(const std::string &method_name, const std::type_info &info) const;

     /** Get an existing or create a new accessor for the given variable name.
      * @param ctx the current event context
      * @param varname the name of the variable for which an accessor will be returned.
      * The event context is needed if a new accessor has to be created to gather information
      * about the type which the variable refers to.
      */
      IAccessor &getAccessor(const EventContext& ctx, const std::string &varname) const {
         std::unordered_map<std::string, CxxUtils::CachedUniquePtrT<IAccessor> >::const_iterator
            accessor_iter = m_accessor.find(varname);
         if (accessor_iter == m_accessor.end()) {
            std::stringstream msg;
            msg << "No accessor stub was created for  " << varname << " i.e. variable is unknown.";
            throw std::runtime_error(msg.str());
         }
         if (accessor_iter->second) return *(accessor_iter->second);
         return computeClassForVarname(ctx,accessor_iter, varname);
      }

     /** Auxiliary method to create an accessor for the given method_name.
      * @param ctx the current event context.
      * @param key a valid read handle key to retrieve the corresponding container.
      * @param decor_key a valid read handle key or nullptr to create a decoration handle for the given method
      * @param method_id the xAOD ID of the given method
      * @param the name of the method.
      * @return returns an accessor for the xAOD content.
      */
      template <class T_Aux>
      std::unique_ptr<IAccessor> createAccessor(const EventContext& ctx,
                                                const SG::ReadHandleKey<T_Aux> &key,
                                                const SG::ReadDecorHandleKey<T_Aux> *decor_key,
                                                SG::auxid_t method_id,
                                                const std::string &method_name) const;

      StoreGateSvc_t m_evtStore;

      /** Particular map which is used to associate names to read handles of AuxElements or AuxVectorBase.
       */
      template <class T_Element_Key, class T_Vector_Key>
      class ReadHandleMapTmpl : public std::unordered_map<std::string, std::any > {
      public:
         /** Return a reference for the given pointer.
          * Will throw an exception if the pointer is nullptr.
          */
         template <class T>
         static T &checkedRef(T *a) {
            if (!a) {
               std::stringstream msg; msg << "Conversion to " << typeid(T).name() << " failed.";
               throw std::runtime_error(msg.str());
            }
            return *a;
         }

         /** Check whether the given any is the "vector" object.*/
         static bool isVector(const std::any &anything)                    { return anything.type() == typeid(T_Vector_Key); }
         /** Check whether the given any is the "element" object.*/
         static bool isElement(const std::any &anything)                   { return anything.type() == typeid(T_Element_Key); }
         /** Get a reference to the element object referred to by any or throw an exception.*/
         static T_Element_Key &elementKey(std::any &anything)              { return checkedRef(std::any_cast<T_Element_Key>(&anything)); }
         /** Get a reference to the vector object referred to by any or throw an exception.*/
         static T_Vector_Key &vectorKey(std::any &anything)                { return checkedRef(std::any_cast<T_Vector_Key>(&anything)); }
         /** Get a reference to the element object referred to by any or throw an exception (read only).*/
         static const T_Element_Key &elementKey(const std::any &anything)  { return checkedRef(std::any_cast<T_Element_Key>(&anything)); }
         /** Get a reference to the vector object referred to by any or throw an exception (read only).*/
         static const T_Vector_Key &vectorKey(const std::any &anything)    { return checkedRef(std::any_cast<T_Vector_Key>(&anything)); }
      };
      using ReadHandleMap = ReadHandleMapTmpl<SG::ReadHandleKey<SG::AuxElement>,
                                              SG::ReadHandleKey<SG::AuxVectorBase> >;
      using ReadDecorHandleMap = ReadHandleMapTmpl<SG::ReadDecorHandleKey<SG::AuxElement>,
                                                   SG::ReadDecorHandleKey<SG::AuxVectorBase> >;
      ReadHandleMap      m_readKeys;  //< Association of variable names and read handle keys to AuxElement or AuxVectorBase.
      ReadDecorHandleMap m_decorKeys; //< Association of variable names and read decoration handle keys for  AuxElement or AuxVectorBase.

     /** Association of variable names to accessors for corresponding xAOD object content.
      * The map structure is constructed before the event loop. The content of its leaves is completed
      * during the event loop.
      */
      mutable std::unordered_map<std::string, CxxUtils::CachedUniquePtrT<IAccessor> > m_accessor;

      /** Special accessor to handle empty vectors
       * Accessor are constructed at time of first evaluation. This fails for empty containers because
       * empty container do not necessarily provide enough type information to create the correct
       * accessor. This is mitigated by this special accessor which is used until the final
       * accessor can be created.
       */
      std::unique_ptr<IAccessor>    m_emptyVectorAccessor;
      bool m_verbose = false;
  };

}

#endif // SG_XAOD_PROXY_LOADER_H
