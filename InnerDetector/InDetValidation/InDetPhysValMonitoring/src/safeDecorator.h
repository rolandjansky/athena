/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDPVM_safeDecorator_h
#define IDPVM_safeDecorator_h
/**
 * @file safeDecorator.h
 * header file for function of same name
 * @author shaun roe
 * @date 20 July 2016
 */



#include "AthContainers/AuxElement.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"
#include "GaudiKernel/EventContext.h"

#include <iostream>
#include <utility>
#include <vector>

#include <cstdlib>

namespace IDPVM {
  // @name roughlyEqual overloaded functions
  // @{
  bool roughlyEqual(const int i, const int j);
  bool roughlyEqual(const unsigned int i, const unsigned int j);
  bool roughlyEqual(const float i, const float j);
  // @}
  // @enum DuplicateBehaviour
  ///Behaviour in case of trying to add a decoration which previously exists
  enum DuplicateBehaviour {
    DO_NOTHING, REJECT_QUIETLY, REJECT_WITH_WARNING, REJECT_WARN_IF_UNEQUAL
  };
  template<class ContainerType, class VariableType>
  using WriteKeyAccessorPair = std::pair<SG::WriteDecorHandleKey<ContainerType>, SG::AuxElement::ConstAccessor<VariableType> >;
  //
  template<class ContainerType, class VariableType>
  using WriteAccessorRefPair = std::pair<SG::WriteDecorHandle<ContainerType, VariableType>, SG::AuxElement::ConstAccessor<VariableType>& >;

  template<class ContainerType, class VariableType>
  using OptionalDecoration = std::pair<SG::WriteDecorHandle<ContainerType, VariableType>, bool >;
  // create a pair composed of a WriteDecorHandleKey to create a decorator handle
  // and an accessor to check the availablilty of a decoration
  template <class T_Parent, class T_Cont, class T>
  void createDecoratorKeysAndAccessor(T_Parent &parent,
                                      const SG::ReadHandleKey<T_Cont> &container_key,
                                      const std::string &prefix,
                                      const std::vector<std::string> &decor_names,
                                      std::vector<WriteKeyAccessorPair<T_Cont, T > > &decor_out) {
    decor_out.clear();
    decor_out.reserve(decor_names.size());
    for (const std::string &a_decor_name: decor_names) {
      decor_out.emplace_back(SG::WriteDecorHandleKey<T_Cont>(container_key.key()+"."+prefix+a_decor_name),
                             SG::AuxElement::ConstAccessor<T>(prefix+a_decor_name));
      parent.declare(decor_out.back().first);
      decor_out.back().first.setOwner(&parent);
      decor_out.back().first.initialize().ignore();
    }
  }

  template <class T_Cont, class T>
  std::vector<OptionalDecoration<T_Cont,T> >
  createDecoratorsIfNeeded( const T_Cont &container,
                            const std::vector<WriteKeyAccessorPair<T_Cont, T > > &keys,
                            const EventContext &ctx,
                            bool verbose=false) {
    std::vector<OptionalDecoration<T_Cont, T> > out;
    bool all_available=true;
    if (!container.empty()) {
       std::vector<bool> decorate;
       decorate.reserve(keys.size());
       for( const WriteKeyAccessorPair<T_Cont, T> &a_key : keys) {
          decorate.push_back(!a_key.second.isAvailable(*container[0])  );
          all_available &= !decorate.back();
          if (verbose && !decorate.back()) {
             std::cout << "WARNING IDPVM::createDecoratorsIfNeeded: Decoration " << a_key.first.key() <<
                " already exists; reject update.\n";
          }
       }
       if (!all_available) {
          std::size_t idx=0;
          out.reserve(keys.size());
          for( const WriteKeyAccessorPair<T_Cont, T> &a_key : keys) {
             assert( idx < decorate.size());
             out.emplace_back(SG::WriteDecorHandle<T_Cont,T>(a_key.first,ctx), decorate[idx++]);
             if (not out.back().first.isPresent()) {
                std::stringstream msg;
                msg << "Container " << a_key.first.key() << " to be decorated does not exist.";
                throw std::runtime_error( msg.str() );
             }
          }
       }
    }
    return out;
  }


  template <class T_Cont, class T>
  std::vector<SG::WriteDecorHandle<T_Cont,T> >
  createDecorators(  const std::vector<SG::WriteDecorHandleKey<T_Cont> > &keys,
                     const EventContext &ctx) {
    std::vector<SG::WriteDecorHandle<T_Cont,T> > out;
    out.reserve(keys.size());
    for( const SG::WriteDecorHandleKey<T_Cont> &a_key : keys) {
      out.emplace_back(a_key,ctx);
      if (not out.back().isValid()) {
        std::stringstream msg;
        msg << "Failed to create decorator handdle " << a_key.key();
        throw std::runtime_error( msg.str() );
      }
    }
    return out;
  }


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
      decor_out.emplace_back(container_key.key()+"."+prefix+a_decor_name);
      // need to declare handles, otherwise the scheduler would not pick up the data dependencies
      // introduced by the decorations
      parent.declare(decor_out.back());
      decor_out.back().setOwner(&parent);
      decor_out.back().initialize().ignore();
    }
  }


  template<class T_Parent, class T_Cont>
  void addReadDecoratorHandleKeys(T_Parent &parent,
                                  const SG::ReadHandleKey<T_Cont> &container_key,
                                  const std::string &prefix,
                                  const std::vector<std::string> &decor_names,
                                  std::vector<SG::ReadDecorHandleKey<T_Cont> > &decor_out) {
    decor_out.reserve(decor_out.size() + decor_names.size());
    for (const std::string &a_decor_name : decor_names) {
      decor_out.emplace_back( container_key.key()+"."+prefix+a_decor_name);
      parent.declare(decor_out.back());
      decor_out.back().setOwner(&parent);
      decor_out.back().initialize().ignore();
    }
  }


  template <class T_Cont, class T_Cont_Elm, class T>
  void decorateOrWarnIfUnequal(const T_Cont_Elm& particle,WriteAccessorRefPair<T_Cont, T> &decorator,
                                   const T& value) {
     if (!decorator.second.isAvailable(particle)) {
      const T existing = decorator.second(particle);
      if (not IDPVM::roughlyEqual(existing, value)) {
        std::cout << "WARNING IDPVM::safeDecorator: " << decorator.first.decorKey() <<
          " Already exists on this object with a different value.\n";
      }
    } else {
      decorator.first(particle) = value;
    }
  }

  template <class T_Cont, class T_Cont_Elm, class T>
  void decorateOrRejectQuietly(const T_Cont_Elm& particle,OptionalDecoration<T_Cont,T> &decorator,
                                   const T& value) {
      if (decorator.second) {
        decorator.first(particle) = value;
      }
  }

  // unsafe method for convenience
  template <class T_Cont, class T_Cont_Elm, class T>
  void decorate(const T_Cont_Elm& particle,OptionalDecoration<T_Cont,T> &decorator,
                const T& value) {
    decorator.first(particle) = value;
  }


}
#endif
