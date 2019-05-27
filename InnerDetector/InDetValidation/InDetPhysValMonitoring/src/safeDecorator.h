/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  // create a pair composed of a WriteDecorHandleKey to create a decorator handle
  // and an accessor to check the availablilty of a decoration
  template <class T_Parent, class T_Cont, class T>
  void createDecoratorKeysAndAccessor(T_Parent &parent,
                                      const SG::ReadHandleKey<T_Cont> &container_key,
                                      const std::string &prefix,
                                      const std::vector<std::string> &decor_names,
                                      std::vector<std::pair<SG::WriteDecorHandleKey<T_Cont>, SG::AuxElement::ConstAccessor<T> > > &decor_out) {
    decor_out.clear();
    decor_out.reserve(decor_names.size());
    for (const std::string &a_decor_name: decor_names) {
      decor_out.push_back(std::make_pair(SG::WriteDecorHandleKey<T_Cont>(container_key.key()+"."+prefix+a_decor_name),
                                         SG::AuxElement::ConstAccessor<T>(a_decor_name)));
      parent.declare(decor_out.back().first);
      decor_out.back().first.setOwner(&parent);
      decor_out.back().first.initialize().ignore();
    }
  }

  template <class T_Cont, class T>
  std::vector<std::pair<SG::WriteDecorHandle<T_Cont,T>, const SG::AuxElement::ConstAccessor<T> &> >
  createDecoratorsWithAccessor(const std::vector<std::pair<SG::WriteDecorHandleKey<T_Cont>,
                                                           SG::AuxElement::ConstAccessor<T> > > &keys,
                               const EventContext &ctx) {
    std::vector<std::pair<SG::WriteDecorHandle<T_Cont,T>, const SG::AuxElement::ConstAccessor<T> &> > out;
    out.reserve(keys.size());
    for( const std::pair<SG::WriteDecorHandleKey<T_Cont>, SG::AuxElement::ConstAccessor<T> > &a_key : keys) {
      out.push_back(std::make_pair(SG::WriteDecorHandle<T_Cont,T>(a_key.first,ctx), a_key.second));
      if (not out.back().first.isValid()) {
        std::stringstream msg;
        msg << "Failed to create decorator handdle " << a_key.first.key();
        throw std::runtime_error( msg.str() );
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
      out.push_back(SG::WriteDecorHandle<T_Cont,T>(a_key,ctx));
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
      decor_out.push_back(SG::WriteDecorHandleKey<T_Cont>(container_key.key()+"."+prefix+a_decor_name) );
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
      decor_out.push_back( SG::ReadDecorHandleKey<T_Cont>(container_key.key()+"."+prefix+a_decor_name) );
      parent.declare(decor_out.back());
      decor_out.back().setOwner(&parent);
      decor_out.back().initialize().ignore();
    }
  }

  /// Safe decorators
  template <class T_Cont, class T_Cont_Elm, class T>
  void decorateOrRejectWithWarning(const T_Cont_Elm& particle,
                                   std::pair<SG::WriteDecorHandle<T_Cont,T>, const SG::AuxElement::ConstAccessor<T> &> &decorator,
                                   const T& value) {
    if (decorator.second.isAvailable(particle)) {
      std::cout << "WARNING IDPVM::safeDecorator: " << decorator.first.key() <<
        " Already exists on this object; update rejected.\n";
    } else {
      decorator.first(particle) = value;
    }
  }

  template <class T_Cont, class T_Cont_Elm, class T>
  void decorateOrWarnIfUnequal(const T_Cont_Elm& particle,
                                   std::pair<SG::WriteDecorHandle<T_Cont,T>, const SG::AuxElement::ConstAccessor<T> &> &decorator,
                                   const T& value) {
    if (decorator.second.isAvailable(particle)) {
      const T existing = decorator.second(particle);
      if (not IDPVM::roughlyEqual(existing, value)) {
        std::cout << "WARNING IDPVM::safeDecorator: " << decorator.first.key() <<
          " Already exists on this object with a different value.\n";
      }
    } else {
      decorator.first(particle) = value;
    }
  }

  template <class T_Cont, class T_Cont_Elm, class T>
  void decorateOrRejectQuietly(const T_Cont_Elm& particle,
                                   std::pair<SG::WriteDecorHandle<T_Cont,T>, const SG::AuxElement::ConstAccessor<T> &> &decorator,
                                   const T& value) {
      if (not decorator.second.isAvailable(particle)) {
        decorator.first(particle) = value;
      }
  }

  // unsafe method for convenience
  template <class T_Cont, class T_Cont_Elm, class T>
  void decorate(const T_Cont_Elm& particle,
                std::pair<SG::WriteDecorHandle<T_Cont,T>, const SG::AuxElement::ConstAccessor<T> &> &decorator,
                const T& value) {
    decorator.first(particle) = value;
  }


}
#endif
