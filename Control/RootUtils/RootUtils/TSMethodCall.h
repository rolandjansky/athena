// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RootUtils/TSMethodCall.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2017
 * @brief Helper for thread-safe @c TMethodCall.  Extracted from Type.
 */


#ifndef ROOTUTILS_TSMETHODCALL_H
#define ROOTUTILS_TSMETHODCALL_H


#include "TClass.h"
#include "TMethodCall.h"
#include "boost/thread/tss.hpp"
#include <atomic>
#include <mutex>


namespace RootUtils {


/// Helper for making a thread-safe function call.
/// We can't use TMethodCall directly because we have to change
/// the object state to make a call.
class TSMethodCall
{
public:
  /// Constructor.
  TSMethodCall();

  // Copy constructor.
  TSMethodCall (const TSMethodCall& other);

  // Assignment.
  TSMethodCall& operator= (const TSMethodCall& other);

  // Destructor.
  ~TSMethodCall();

    
  /**
   * @brief Set the function that we're to call.
   * @param cls The class of the object we're calling.
   * @param fname The name of the function.
   * @param args Its argument list.
   */
  void setProto (TClass* cls,
                 const std::string& fname,
                 const std::string& args);


  /**
   * @brief Return a pointer to the thread-specific @c TMethodCall.
   *
   * Returns nullptr if @c setProto hasn't been called or if the
   * method was not found.
   */
  TMethodCall* call();


private:
  /// Class that we're calling.
  TClass* m_cls;
  
  /// Name of the function we're calling.
  std::string m_fname;

  /// Argument list.
  std::string m_args;

  /// Object to call the method on the payload type.
  /// This will be left invalid if the payload is not a class type.
  /// This instance is not actually used for calls --- since we have
  /// to change the object state to make a call (registering the
  /// arguments), this is not thread-safe.  Instead, we use the
  /// thread-specific instances accessed through @c m_tsMeth.
  ///
  /// Held by pointer rather than value to work around a ROOT problem
  /// where the TMethodCall destructor will crash if gCling has already
  /// been destroyed.
  std::unique_ptr<TMethodCall> m_meth;

  /// Flag whether or not m_meth has been initialized.
  /// We don't want to do that before we actually need it,
  /// to prevent needless parses with root6.
  std::atomic<bool> m_initialized;

  /// Control access to m_assign for initialization.
  std::mutex m_mutex;

  /// Objects used to call the method on the payload object.
  /// Left invalid if the payload does not have class type.
  /// Copied from @c m_meth the first time it's used in
  /// a particular thread.
  boost::thread_specific_ptr<TMethodCall> m_tsMeth;
};


} // namespace RootUtils


#endif // not ROOTUTILS_TSMETHODCALL_H
