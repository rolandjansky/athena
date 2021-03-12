/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _ExpressionEvaluation_Utils_h_
#define _ExpressionEvaluation_Utils_h_

#include <stdexcept>
#include "GaudiKernel/DataHandle.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxElement.h"

namespace ExpressionParsing {
/** Auxiliary function to initialize newly create data handles.
    */
   template <class T_Key>
   void initializeHandle(T_Key *key, std::vector<Gaudi::DataHandle *> &new_input_handles) {
      if (key) {
         if (key->initialize().isFailure()) {
            std::stringstream msg;
            msg << "Failed to initialize read handle " << key->key();
            throw std::runtime_error( msg.str());
         }
         key->init();
         new_input_handles.push_back( key);
      }
   }

 
   // helper functions to provide a uniform interface for AuxElement and AuxVectorBase
   template <class T_Aux>
   bool isAvailable(const T_Aux &cont, SG::auxid_t auxid);

   template <>
   bool isAvailable<SG::AuxVectorBase>(const SG::AuxVectorBase &cont, SG::auxid_t auxid) {
      return cont.isAvailable(auxid);
   }

   template <>
   bool isAvailable<SG::AuxElement>(const SG::AuxElement &cont, SG::auxid_t auxid) {
      return cont.container() && cont.container()->isAvailable(auxid);
   }

      // helper functions to provide a uniform interface for AuxElement and AuxVectorBase
   template <class T>
   class SizeHelper {
   public:
      static std::size_t getSize(const T &cont) {
         (void) cont;
         return 1;
      }
   };

   template <typename T>
   class SizeHelper<std::vector<T> > {
   public:
      static std::size_t getSize(const std::vector<T> &cont) { return cont.size(); }
   };

   template <>
   class SizeHelper<SG::AuxVectorBase > {
   public:
      static std::size_t getSize(const SG::AuxVectorBase &cont) { return cont.size_v(); }
   };

   template <>
   class SizeHelper<SG::AuxElement > {
   public:
      static std::size_t getSize(const SG::AuxElement &cont) {       return cont.container() ? cont.container()->size_v() : 0; }
   };

   template <class T>
   std::size_t getContainerSize(const T &cont) {
      return SizeHelper<T>::getSize(cont);
   }

   template <class T>
   const SG::AuxVectorData *getVectorData(const T &cont) {
      return &cont;
   }

   template <>
   const SG::AuxVectorData *getVectorData<SG::AuxElement>(const SG::AuxElement &cont) {
      return cont.container();
   }

   template <class T_Derived>
   class Singleton {
   public:
      static T_Derived &instance() {
         if (!s_instance) {
            s_instance.set(std::make_unique<T_Derived>());
         }
         return *s_instance;
      }
   private:
      static CxxUtils::CachedUniquePtrT<T_Derived> s_instance;
   };
   template <class T_Derived>
   CxxUtils::CachedUniquePtrT<T_Derived> Singleton<T_Derived>::s_instance;

}
#endif
