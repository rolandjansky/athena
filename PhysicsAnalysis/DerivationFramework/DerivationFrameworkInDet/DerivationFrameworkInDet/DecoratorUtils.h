/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _DecoratorUtils_H_
#define _DecoratorUtils_H_
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace DerivationFramework {
  // @TODO copied from  InnerDetector/InDetValidation/InDetPhysValMonitoring/src/safeDecorator.h, maybe move to a central place ?

  // convenience method to create several decorators
  template<class T_Parent, class T_Cont>
  void createDecoratorKeys(T_Parent &parent,
                        const SG::ReadHandleKey<T_Cont> &container_key,
                        const std::string &prefix,
                        const std::vector<std::string> &decor_names,
                        std::vector<SG::WriteDecorHandleKey<T_Cont> > &decor_out) {
    decor_out.clear();
    decor_out.reserve(decor_names.size());
    for (const std::string &a_decor_name : decor_names) {
      assert( !a_decor_name.empty() );
      decor_out.push_back(SG::WriteDecorHandleKey<T_Cont>(container_key.key()+"."+prefix+a_decor_name) );
    }

    for (SG::WriteDecorHandleKey<T_Cont> &a_decor_handle_key : decor_out) {
      // need to declare handles, otherwise the scheduler would not pick up the data dependencies
      // introduced by the decorations
      parent.declare(a_decor_handle_key);
      a_decor_handle_key.setOwner(&parent);
      if (a_decor_handle_key.initialize().isFailure()) {
         std::stringstream msg;
         msg << "Failed to initialize " << a_decor_handle_key.key() << " after initializing: ";
         for (SG::WriteDecorHandleKey<T_Cont> &b_decor_handle_key : decor_out) {
            if (&b_decor_handle_key == &a_decor_handle_key) break;
            msg << " " << b_decor_handle_key.key();
         }
         throw std::runtime_error(msg.str());
      }
    }
  }

  template <class T_Cont, class T>
  std::vector<SG::WriteDecorHandle<T_Cont,T> >
  createDecorators(  const std::vector<SG::WriteDecorHandleKey<T_Cont> > &keys,
                     const EventContext &ctx) {
    std::vector<SG::WriteDecorHandle<T_Cont,T> > out;
    out.reserve(keys.size());
    for( const SG::WriteDecorHandleKey<T_Cont> &a_key : keys) {
      out.push_back(SG::WriteDecorHandle<T_Cont,T>(a_key,ctx));
      if (not out.back().isValid()) {
        std::stringstream msg;
        msg << "Failed to create decorator handdle " << a_key.key();
        throw std::runtime_error( msg.str() );
      }
    }
    return out;
  }


}
#endif
