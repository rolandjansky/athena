// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaBaseComps/HandleClassifier.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2017
 * @brief Helper to classify the type of a handle property.
 */


#ifndef ATHENABASECOMPS_HANDLECLASSIFIER_H
#define ATHENABASECOMPS_HANDLECLASSIFIER_H


#include "StoreGate/VarHandleBase.h"
#include "StoreGate/VarHandleKey.h"
#include "StoreGate/VarHandleKeyArray.h"
#include <type_traits>


namespace SG {


// Tags to describe the type of a handle property.
class VarHandleType {};
class VarHandleKeyType {};
class VarHandleKeyArrayType {};
class NotHandleType {};


/// Helper to classify the type of a handle property.
/// type is one of the following:
///   - @c VarHandleType, if @c T derives from @c VarHandleBase.
///   - @c VarHandleKeyType, if @c T derives from @c VarHandleKey.
///   - @c VarHandleKeyArrayType, if @c T derives from @c VarHandleKeyArray.
///   - @c NotHandleType otherwise.
template <class T>
struct HandleClassifier
{
  typedef typename std::conditional<std::is_base_of<SG::VarHandleBase, T>::value,
                                    VarHandleType,
                                    NotHandleType>::type type1;

  typedef typename std::conditional<std::is_base_of<SG::VarHandleKey, T>::value,
                                    VarHandleKeyType,
                                    type1>::type type2;


  typedef typename std::conditional<std::is_base_of<SG::VarHandleKeyArray, T>::value,
                                    VarHandleKeyArrayType,
                                    type2>::type type;
};


} // namespace SG


#endif // not ATHENABASECOMPS_HANDLECLASSIFIER_H
