// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/ShallowCopyDecorDeps.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2020
 * @brief Helper to propagate decoration dependencies to a shallow copy.
 */


#ifndef STOREGATE_SHALLOWCOPYDECORDEPS_H
#define STOREGATE_SHALLOWCOPYDECORDEPS_H


#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKeyArray.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ThreadLocalContext.h"


namespace SG {


/**
 * @brief Helper to propagate decoration dependencies to a shallow copy.
 *
 * When we do a shallow copy of ORIG to COPY, any decorations of ORIG
 * are readable via COPY.  However, the scheduler doesn't know about this.
 * The scheduler keeps track of decoration dependencies via aliases
 * of the form ORIG.DECOR.  If one tries to use a ReadDecorHandle
 * for COPY.DECOR, the scheduler won't be able to resolve that dependency
 * because that alias doesn't exist --- only ORIG.DECOR.
 *
 * This class will help in propagating the decoration aliases
 * from the original object to the shallow copy.  The set of decorations
 * does need to be specified in advance.
 *
 * It is assumed that you have a Gaudi component which does the shallow copy,
 * and that the original and copy are specified by a ReadHandleKey
 * and a WriteHandleKey, respectively.
 *
 * In the definition of the component class, add a ShallowCopyDecorDeps
 * member as a property.  The value of the property should be the list
 * of decorations to be copied:
 *
 *@code
 *  SG::ShallowCopyDecorDeps<MyObj> m_decorDeps
 *  { this, "DecorDeps", {"decor1", "decor2"},
 *    "List of decorations to propagate through the shallow copy." };
 @endcode
 * 
 * In the @c initialize() method, call @c initialize on the
 * ShallowCopyDecorDeps object, passing in the read and write
 * handle keys for the original object and the shallow copy:
 *
 *@code
 *   ATH_CHECK( m_decorDeps.initialize (m_origKey, m_copyKey) );
 @endcode
 *
 * Finally, after the copy has been recorded, call @c linkDecors,
 * passing the read handle key for the original object:
 *
 *@code
 *  ATH_CHECK( m_decorDeps.linkDecors (m_origKey, ctx) );
 @endcode
 */
template <class T>
class ShallowCopyDecorDeps
{
public:
  /** 
   * @brief Auto-declaring Property constructor.
   * @param owner Owning component.
   * @param name Name of the Property.
   * @param l Default list of decorations to propagate.
   * @param doc Documentation string.
   */
  template <class OWNER>
  ShallowCopyDecorDeps (OWNER* owner,
                        const std::string& name,
                        std::initializer_list<std::string> l,
                        const std::string& doc = "");


  /**
   * @brief Initialize this property.  Call this from initialize().
   * @param origKey Key for the source of the shallow copy.
   * @param copyKey Key for the result of the shallow copy.
   * @param used If false, then this handle is not to be used.
   *             Instead of normal initialization, the keys will be cleared.
   */
  StatusCode initialize (const SG::ReadHandleKey<T>& origKey,
                         const SG::WriteHandleKey<T>& copyKey,
                         bool used = true);


  /**
   * @brief Create alias for the decorations, linked to the shallow copy.
   * @param origKey Key for the source of the shallow copy.
   * @param ctx The current EventContext.
   *
   * Call this after the shallow copy has been recorded in SG.
   */
  StatusCode linkDecors (const SG::ReadHandleKey<T>& origKey,
                         const EventContext& ctx = Gaudi::Hive::currentContext()) const;


private:
  /// Keys for the decorations on the original object.
  SG::ReadHandleKeyArray<T> m_readKeys;

  /// Keys for decorations to alias for the shallow copy.
  SG::WriteHandleKeyArray<T> m_writeKeys;
};


} // namespace SG


#include "StoreGate/ShallowCopyDecorDeps.icc"


#endif // not STOREGATE_SHALLOWCOPYDECORDEPS_H
