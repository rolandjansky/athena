/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _ExpressionEvaluation_GenAccessor_h_
#define _ExpressionEvaluation_GenAccessor_h_

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "ExpressionEvaluation/IProxyLoader.h"

#include "BaseAccessor.h"

#include <vector>

namespace ExpressionParsing {

   /** Auxiliary class to handle scalar like containers (AuxElement).
    */
   class ScalarHelper {
   public:
      template <class T_Dest, class T_Cont, class T_Helper>
      static void fillVector(T_Helper &helper, SG::ReadHandle<T_Cont> &handle, std::vector<T_Dest> &result) {
         result.reserve(1);
         result.push_back( static_cast<T_Dest>( helper.get( *handle ) ));
      }

      template <class T_Dest, class T_Cont, class T_Helper>
      static T_Dest getScalar(T_Helper &helper, SG::ReadHandle<T_Cont> &handle,const T_Dest &dummy) {
         (void) dummy;
         return static_cast<T_Dest>( helper.get( *handle ) );
      }
   };

   /** Auxiliary class to handle vector like containers (AuxVectorBase).
    */
   class VectorHelper {
   public:
      template <class T_Dest, class T_Cont, class T_Helper>
      static void fillVector(T_Helper &helper, SG::ReadHandle<T_Cont> &handle, std::vector<T_Dest> &result) {
         std::size_t n_elements=helper.size(*handle);
         //         helper.checkSize(n_elements);
         result.reserve(n_elements);
         for(std::size_t idx =0; idx <n_elements; ++idx) {
            result.push_back( static_cast<T_Dest>( helper.get(*handle, idx)) );
         }
      }
      template <class T_Dest, class T_Cont, class T_Helper>
      static T_Dest getScalar(T_Helper &helper, SG::ReadHandle<T_Cont> &handle, const T_Dest &dummy) {
         (void) dummy;
         if (getContainerSize(*handle) != 1) {
            BaseAccessor::throwVectorContainsNotOneElement(handle.key(), getContainerSize(*handle));
         }

         return static_cast<T_Dest>( helper.get(*handle, 0));
      }
   };

   /** Generic accessor to access xAOD object content.
    * It uses several auxiliary classes to provide accessor, decoration handle, or method call based
    * access to content of AuxVectorBase or AuxElement containers.
    */
   template <class T_Cont, class T_HelperKit, class T_ScalarVectorHelper>
   class GenAccessor : public BaseAccessor {
   public:
      GenAccessor(const SG::ReadHandleKey<T_Cont> &key, T_HelperKit &&helper_kit, IProxyLoader::VariableType variable_type)
         : BaseAccessor(variable_type),
           m_key(&key),
           m_helperKit(std::move(helper_kit))
      {}

      virtual int loadInt(const EventContext& ctx) const override {
         return this->loadScalar<int>(ctx);
      }
      virtual double loadDouble(const EventContext& ctx) const override {
         return this->loadScalar<double>(ctx);
      }
      virtual std::vector<int> loadVecInt(const EventContext& ctx) const override {
         return this->loadVector<int>(ctx);
      }
      virtual std::vector<double> loadVec(const EventContext& ctx) const override {
         return this->loadVector<double>(ctx);
      }
   protected:
      template <class T_Dest>
      std::vector<T_Dest> loadVector(const EventContext& ctx) const {
         SG::ReadHandle<T_Cont> handle(*this->m_key, ctx);
         if (!handle.isValid()) {
            this->throwInvalidHandle(this->m_key->key());
         }
         std::vector<T_Dest> result;
         if (getContainerSize(*handle)>0) {
            auto helper( m_helperKit.create(ctx, handle));
            T_ScalarVectorHelper::fillVector(helper, handle,result);
         }
         return result;
      }

      template <class T_Dest>
      T_Dest loadScalar(const EventContext& ctx) const {
         SG::ReadHandle<T_Cont> handle(*this->m_key, ctx);
         if (!handle.isValid()) {
            this->throwInvalidHandle(this->m_key->key());
         }
         auto helper( m_helperKit.create(ctx, handle));
         T_Dest dummy;
         return T_ScalarVectorHelper::getScalar(helper, handle, dummy);
      }

      const SG::ReadHandleKey<T_Cont>      *m_key;  //< key to access the xAOD container
      T_HelperKit                      m_helperKit; //< auxiliary class to create an access helper
   };
}
#endif
