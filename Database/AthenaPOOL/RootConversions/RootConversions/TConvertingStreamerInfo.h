// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RootConversions/TConvertingStreamerInfo.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2008
 * @brief Variant of @c TStreamerInfo to handle conversions.
 *
 * The @c Initialize method will change the @c New() method of the
 * @c TStreamerInfo class to point at our new method.  Thus,
 * when we read a @c TStreamerInfo from a file, we'll actually
 * make an instance of this class instead.  (@c TStreamerInfo
 * instances that are created explicitly, for writing, are not
 * affected by this.)
 *
 * This can handle some conversions in which the type name
 * changes between the transient and persistent versions.
 */


#ifndef ROOTCONVERSIONS_TCONVERTINGSTREAMERINFO_H
#define ROOTCONVERSIONS_TCONVERTINGSTREAMERINFO_H

#include "TStreamerInfo.h"
#include "TError.h"


/**
 * @brief Variant of @c TStreamerInfo to handle conversions.
 */
class TConvertingStreamerInfo
#ifndef __REFLEX__
#ifndef __CLING__
  : public TStreamerInfo
  // Need to hide the base class from reflex and cling so that it doesn't
  // insist on generating the ClassDef functions.
#endif
#endif
{
public:
  /// Constructor.
  TConvertingStreamerInfo();


  /**
   * @brief Set up conversions.
   */
  static void Initialize();


  /**
   * @brief Compare with class definition to handle schema evolution.
   *
   * This is a standard @c TStreamerInfo method.
   * We override this to intercept warning messages that it produces.
   * If we see a warning related to one of the conversions that we
   * handle, we patch up the @c TStreamerElement and swallow the warning.
   */
  virtual void BuildOld();


private:
  /**
   * @brief new() method for this object.
   * @param p Address for placement new, or 0.
   * @return Address of the new object.
   *
   * This is installed as the @c New method in @c TStreamerInfo's @c TClass.
   * Thus, when we read from a file a @c TBranchElement, we actually
   * make an instance of this class.
   */
  static void* new_TConvertingStreamerInfo (void* p);


  /**
   * @brief Error message interceptor.
   * @param level Error message level.
   * @param abort Abort flag.
   * @param location Function that raised the error.
   * @param msg Error message.
   *
   * We interpose this in front of the default error handler while
   * @c BuildOld is running.  If we see a message corresponding
   * to a conversion that we handle, we fix up the @c TStreamerElement
   * and swallow the message.
   */
  static void errhand (int level,
                       Bool_t abort,
                       const char* location,
                       const char* msg);


  /**
   * @brief Error message parser.
   * @param msg The error message.
   * @returns True if the message was handled; false otherwise.
   *
   * If this is an error message corresponding to a conversion
   * that we handle, we fix up the @c TStreamerElement and return true.
   * Otherwise, return false.
   */
  bool parse (const std::string& msg);


  /**
   * @param Streamer patcher.
   * @param field Name of the field to patch.
   * @param from Original (persistent) type.
   * @param to Target (transient) type.
   * @returns True if the conversion was handled; false otherwise.
   *
   * This is called after an error message for a type mismatch
   * has been successfully parsed.  Fix up the @c TStreamerElement
   * and return true if this is a conversion we handle; return false
   * otherwise.
   */
  bool patch (const std::string& field,
              const std::string& from,
              const std::string& to);


private:
  /// Previous error handler.
  ErrorHandlerFunc_t m_oldhand;

  /// Hook to get this object back from the static @c errhand.
  static thread_local TConvertingStreamerInfo* s_self;


  TConvertingStreamerInfo& operator= (const TConvertingStreamerInfo&);
  TConvertingStreamerInfo (const TConvertingStreamerInfo&);
};


#endif // not ROOTCONVERSIONS_TCONVERTINGSTREAMERINFO_H

