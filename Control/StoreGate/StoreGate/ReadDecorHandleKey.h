// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file StoreGate/ReadDecorHandleKey.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        ReadDecorHandle is made.
 */


#ifndef STOREGATE_READDECORHANDLEKEY_H
#define STOREGATE_READDECORHANDLEKEY_H


#include "StoreGate/ReadHandleKey.h"


namespace SG {


/*
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        ReadDecorHandle is made.
 *
 * This class holds the key part of a ReadDecorHandle.
 *
 * Use this to read a decoration on an object such that the scheduler
 * is aware of it.  This is used just like a @c ReadHandleKey (which see)
 * except that the key string is of the form CCC.DDD, where CCC is the
 * name of the container in StoreGate and DDD is the name of the decoration.
 *
 * Example:
 *
 *@code
 *  class MyAlgorithm : public AthReentrantAlgorithm {
 *    ...
 *    SG::ReadDecorHandleKey<MyCont> m_key;
 *  };
 *  ...
 *  MyAlgorithm::MyAlgorithm (const std::string& name, ISvcLocator* svcloc)
 *    : AthReentrantAlgorithm (name, svcloc), ...
 *  {
 *    declareProperty ("Key", m_key = "container.decor");
 *    ...
 *  }
 *
 *  StatusCode MyAlgorithm::initialize()
 *  {
 *    ATH_CHECK( m_key.initialize() );
 *    ...
 *  }
 *
 *  StatusCode MyAlgorithm::execute_r (const EventContext& ctx) const
 *  {
 *    SG::ReadDecorHandle<MyCont, float> h (m_key, ctx);
 *    for (const MyObj& o : *h) {  // Access the container.
 *      doSomething (h (o));  // Access the decoration.
 *      ...
 @endcode
 */
template <class T>
class ReadDecorHandleKey
  : public ReadHandleKey<T>
{
public:
  using ReadHandleKey<T>::ReadHandleKey;
  using ReadHandleKey<T>::operator=;
};


} // namespace SG


#endif // not STOREGATE_READDECORHANDLEKEY_H
