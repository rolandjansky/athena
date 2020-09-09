/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RootConversions/src/TConvertingStreamerInfo.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2008
 * @brief Variant of @c TStreamerInfo to handle conversions.
 */


#include "RootConversions/TConvertingStreamerInfo.h"
#include "RootConversions/TConverterRegistry.h"
#include "CxxUtils/checker_macros.h"
#include "TStreamerElement.h"
#include "TMemberStreamer.h"
#include "TROOT.h"
#include "TClass.h"
#include "TError.h"
#include <string>
#include <vector>
#include <cassert>
#include <memory>
#include <mutex>


/// Hook to get this object back from the static @c errhand.
thread_local TConvertingStreamerInfo* TConvertingStreamerInfo::s_self;


/**
 * @brief Constructor.
 */
TConvertingStreamerInfo::TConvertingStreamerInfo()
  : m_oldhand (0)
{
}


/**
 * @brief Set up conversions.
 */
void TConvertingStreamerInfo::Initialize()
{
  static bool initialized ATLAS_THREAD_SAFE = false;
  static std::mutex initmutex;
  std::lock_guard<std::mutex> lock (initmutex);

  if (initialized)
    return;
  initialized = true;

  TClass* cl = gROOT->GetClass ("TStreamerInfo", true);

  // Make sure the I/O plugin has been loaded.
  // This will change the new hook for the TStreamerInfo class,
  // so we need to make sure that this is done before we change
  // it ourselves.
  TVirtualStreamerInfo::Factory ();
  if (!cl) {
    ::Error ("TConvertingStreamerInfo",
             "Can't find TClass for TStreamerInfo");
    return;
  }

  // Change the @c New() method for @c TStreamerInfo to make
  // an instance of this class instead.
  cl->SetNew (TConvertingStreamerInfo::new_TConvertingStreamerInfo);
}


/**
 * @brief new() method for this object.
 * @param p Address for placement new, or 0.
 * @return Address of the new object.
 *
 * This is installed as the @c New method in @c TStreamerInfo's @c TClass.
 * Thus, when we read from a file a @c TStreamerInfo, we actually
 * make an instance of this class.
 */
void* TConvertingStreamerInfo::new_TConvertingStreamerInfo (void *p)
{
  if (p)
    return new (p) TConvertingStreamerInfo;
  return new TConvertingStreamerInfo;
}


/**
 * @brief Compare with class definition to handle schema evolution.
 *
 * This is a standard @c TStreamerInfo method.
 * We override this to intercept warning messages that it produces.
 * If we see a warning related to one of the conversions that we
 * handle, we patch up the @c TStreamerElement and swallow the warning.
 */
void TConvertingStreamerInfo::BuildOld()
{
  // Call the base class BuildOld.
  // But first, we need to interpose our error handler,
  // and also set @c s_self so we can get back to this object
  // from the static error handler function.

  // Be careful to handle the cases of recursion and exiting via an exception.

  TConvertingStreamerInfo* oldself = s_self;
  s_self = this;

  ErrorHandlerFunc_t oldhand = ::SetErrorHandler (errhand);
  if (oldhand != errhand)
    m_oldhand = oldhand;
  else
    m_oldhand = oldself->m_oldhand;

  try {
    TStreamerInfo::BuildOld();
  }
  catch (...) {
    // Restore things.
    ::SetErrorHandler (oldhand);
    s_self = oldself;
    throw;
  }

  ::SetErrorHandler (oldhand);
  s_self = oldself;
}


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
bool TConvertingStreamerInfo::patch (const std::string& field,
                                     const std::string& from,
                                     const std::string& to)
{
  if (TMemberStreamer* conv =
      TConverterRegistry::Instance()->GetStreamerConverter (from, to))
  {
    Int_t offset;
    TStreamerElement* elem = GetStreamerElement (field.c_str(), offset);
    TClass* cl = gROOT->GetClass (to.c_str());
    if (elem && cl) {
      elem->SetTypeName (to.c_str());
      elem->Update (elem->GetClass(), cl);
      elem->SetNewType (elem->GetType());
      elem->SetStreamer (conv);
      return true;
    }
  }

  return false;
}


/**
 * @brief Error message parser.
 * @param msg The error message.
 * @returns True if the message was handled; false otherwise.
 *
 * If this is an error message corresponding to a conversion
 * that we handle, we fix up the @c TStreamerElement and return true.
 * Otherwise, return false.
 */
bool TConvertingStreamerInfo::parse (const std::string& smsg)
{
  if (smsg.substr (0, 15) != "Cannot convert ") return false;

  std::string::size_type ifrom = smsg.find (" from type:");
  if (ifrom == std::string::npos) return false;
  
  std::string::size_type ifield = smsg.rfind ("::", ifrom);
  if (ifield == std::string::npos) return false;

  std::string field (smsg, ifield+2, ifrom - (ifield+2));

  std::string::size_type ito = smsg.find (" to type:");
  if (ito == std::string::npos) return false;

  ifrom += 11;
  while (ifrom < ito && smsg[ifrom] == ' ')
    ++ifrom;
  std::string from (smsg, ifrom, ito - ifrom);

  std::string::size_type iskip = smsg.find (", skip element");
  if (iskip == std::string::npos) return false;

  ito += 9;
  while (ito < iskip && smsg[ito] == ' ')
    ++ito;
  std::string to (smsg, ito, iskip - ito);

  return s_self->patch (field, from, to);
}


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
void TConvertingStreamerInfo::errhand (int level,
                                       Bool_t abort,
                                       const char* location,
                                       const char* msg)
{
  if (!s_self->parse (msg))
    s_self->m_oldhand (level, abort, location, msg);
}


