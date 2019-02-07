/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootUtils/src/TSMethodCall.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2017
 * @brief Helper for thread-safe @c TMethodCall.  Extracted from Type.
 */


#include "RootUtils/TSMethodCall.h"
#include "TMethod.h"
#include "TError.h"


namespace RootUtils {


/**
 * @brief Default constructor.
 */
TSMethodCall::TSMethodCall()
  : m_cls(nullptr),
    m_initialized(false)
{
}


/**
 * @brief Copy constructor.
 */
TSMethodCall::TSMethodCall (const TSMethodCall& other)
  : m_cls (other.m_cls),
    m_fname (other.m_fname),
    m_args (other.m_args),
    m_meth (other.m_meth),
    m_initialized (false)
{
  // Don't copy m_tsMeth.
}


/**
 * @brief Assignment.
 */
TSMethodCall& TSMethodCall::operator= (const TSMethodCall& other)
{
  if (this != &other) {
    m_cls = other.m_cls;
    m_fname = other.m_fname;
    m_args = other.m_args;
    m_meth = other.m_meth;
    m_initialized = false;

    // Don't copy m_tsMeth.
  }
  return *this;
}


/**
 * @brief Set the function that we're to call.
 * @param cls The class of the object we're calling.
 * @param fname The name of the function.
 * @param args Its argument list.
 */
void TSMethodCall::setProto (TClass* cls,
                             const std::string& fname,
                             const std::string& args)
{
  m_cls = cls;
  m_fname = fname;
  m_args = args;
  m_initialized = false;
}


/**
 * @brief Return a pointer to the thread-specific @c TMethodCall.
 *
 * Returns nullptr if @c setProto hasn't been called or if the
 * method was not found.
 */
TMethodCall* TSMethodCall::call()
{
  // Fail if this isn't a class type.
  if (m_cls == 0) return nullptr;

  if (!m_initialized) {
    // Not initialized ... try to do so now.  First take the lock.
    std::lock_guard<std::mutex> lock (m_mutex);
    // cppcheck-suppress identicalInnerCondition; false positive
    if (!m_initialized) {
      m_meth.InitWithPrototype (m_cls, m_fname.c_str(), m_args.c_str(),
                                false, ROOT::kExactMatch);
      if (!m_meth.IsValid()) {
        ::Warning ("RootUtils::Type",
                   "Can't get method for type `%s': %s (%s).",
                   m_cls->GetName(), m_fname.c_str(), m_args.c_str());
      }
      m_initialized = true;
    }
  }

  if (!m_meth.IsValid()) return nullptr;

  if (m_tsMeth.get() == 0)
    m_tsMeth.reset (new TMethodCall (m_meth));

  return m_tsMeth.get();
}


} // namespace RootUtils
