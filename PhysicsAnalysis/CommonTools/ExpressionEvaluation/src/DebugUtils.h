/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _ExpressionEvaluation_DebugUtils_h_
#define _ExpressionEvaluation_DebugUtils_h_
#include <string>
#include <iostream>
#include "Utils.h"

namespace ExpressionParsing {

   /** Method for debugging to dump information for the given aux member.*/
   template <class T>
   void dumpAuxId(const T &cont, SG::auxid_t method_id ) {
         std::string name;
         std::string class_name;
         std::string type_name;
         std::string vec_name;
         bool is_avaail=false;
         try {
            is_avaail=isAvailable (cont, method_id);
         }
         catch (...) {
         }
         try {
            name = SG::AuxTypeRegistry::instance().getName(method_id);
         }
         catch (...) {
         }
         try {
            class_name = SG::AuxTypeRegistry::instance().getClassName(method_id);
         }
         catch (...) {
         }
         try {
            type_name  = SG::AuxTypeRegistry::instance().getTypeName(method_id);
         }
         catch (...) {
         }
         try {
            vec_name   = SG::AuxTypeRegistry::instance().getVecTypeName(method_id);
         }
         catch (...) {
         }
         std::cout << "DEBUG SGxAODProxyLoader aux " << method_id << " : " << name << " -> "
                   << " class=" << class_name
                   << " type="  << type_name
                   << " type="  << vec_name
                   << " avail=" << is_avaail
                   << std::endl;
   }

   /** Method for debugging to dump information of all aux member of the given container and the specific aux "member".*/
   template <class T>
   void dumpAux(const T &cont, SG::auxid_t method_id) {
      dumpAuxId(cont, method_id);
      for ( auto id : cont.getAuxIDs()) {
         dumpAuxId(cont,id);
      }
   }

     /** Function for debugging which dumps information about newly declared data handles.
    */
   template <class T>
   void dumpDeclare(const T *key) {
      if (key) {
         std::string type_name;
         const SG::BaseInfoBase* base_info = SG::BaseInfoBase::find (key->clid());
         if (base_info) {
            type_name =base_info->typeinfo().name();
         }
         std::cout << "DEBUG SGxAODProxyLoader declare " << key->key() << " -> " << type_name << std::endl;
      }
   }
}
#endif
