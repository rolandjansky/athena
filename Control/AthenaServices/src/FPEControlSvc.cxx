/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FPEControlSvc.cxx,v 1.5 2008-09-23 22:00:47 binet Exp $
/**
 * @file  AthenaServices/src/FPEControlSvc.cxx
 * @author scott snyder
 * @date Nov 2007
 * @brief Service to enable or disable floating-point exceptions.
 */

#include "FPEControlSvc.h"
#include "AthenaKernel/errorcheck.h"
#include <fenv.h>
#include <cstring>


/**
 * @brief Constructor.
 * @param name The service name.
 * @param svcloc The service locator.
 */
FPEControlSvc::FPEControlSvc( const std::string& name, ISvcLocator* svc )
  : AthService( name, svc ),
    m_toolSvc ("ToolSvc", name),
    m_env(),
    m_haveEnv(false),
    m_enabled (0),
    m_disabled (0),
    m_removeInFinalize(false),
    m_feSetRounding("")
{
  declareProperty("Exceptions", m_exceptions);
  declareProperty("ToolSvc",    m_toolSvc);
  declareProperty("FERoundingMode", m_feSetRounding);
  m_exceptions.declareUpdateHandler (&FPEControlSvc::prophand, this);

  // Set up the default exceptions.
  std::vector<std::string> defexc;
  defexc.push_back ("invalid");
  defexc.push_back ("divbyzero");
  defexc.push_back ("overflow");
  m_exceptions = defexc;
}


/**
 * @brief Standard initialize method.
 */
StatusCode FPEControlSvc::initialize()
{
  // And change the exception mask.
  prophand (m_exceptions);

  // Add ourself as an observer.
  CHECK( m_toolSvc.retrieve() );
  m_toolSvc->registerObserver (this);
  
  if(m_feSetRounding!="")
    {
      REPORT_MESSAGE (MSG::INFO) << "FE Rounding mode " << m_feSetRounding << " requested";
      if(m_feSetRounding=="FE_TONEAREST")
	{
	  if ( fesetround(FE_TONEAREST) )
	      REPORT_MESSAGE (MSG::WARNING) << "Couldn't change FE Rounding to mode FE_TONEAREST !";
	}
      else if(m_feSetRounding=="FE_UPWARD")
	{
	  if ( fesetround(FE_UPWARD) )
	    REPORT_MESSAGE (MSG::WARNING) << "Couldn't change FE Rounding to mode FE_UPWARD !";
	}
      else if(m_feSetRounding=="FE_DOWNWARD")
	{
	  if ( fesetround(FE_DOWNWARD) )
	    REPORT_MESSAGE (MSG::WARNING) << "Couldn't change FE Rounding to mode FE_DOWNWARD !";
	}
      else if(m_feSetRounding=="FE_TOWARDZERO")
	{
	  if ( fesetround(FE_TOWARDZERO) )
	    REPORT_MESSAGE (MSG::WARNING) << "Couldn't change FE Rounding to mode FE_TOWARDZERO !";
	}
      else
	REPORT_MESSAGE (MSG::WARNING) << "Don't know FE Rounding to mode " << m_feSetRounding;
    }
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard finalize method.
 */
StatusCode FPEControlSvc::finalize()
{
  // remove only if requested
  if (m_removeInFinalize)
    {
      // Restore the FP environment to what is was before we ran.
      fesetenv (&m_env);
    }
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Observer callback.
 *
 * This is called after each tool is created.
 * We make sure that the FPE mask is still set correctly.
 * We do this because some tools have been observed to mask off exceptions.
 * In particular, libgfortran will mask exceptions off when it's first
 * loaded.  So, if libgfortran hasn't yet been loaded when we initialize,
 * but then gets loaded later, then exceptions will remain masked off.
 */
void FPEControlSvc::onCreate(const IAlgTool*)
{
  setFPU();
}


namespace {


/**
 * @brief Convert an exception mask into a string.
 * @param mask The exception mask.
 */
std::string mask_to_string (int mask)
{
  std::string out;
  if (mask & FE_INEXACT)
    out += "inexact ";
  if (mask & FE_DIVBYZERO)
    out += "divbyzero ";
  if (mask & FE_UNDERFLOW)
    out += "underflow ";
  if (mask & FE_OVERFLOW)
    out += "overflow ";
  if (mask & FE_INVALID)
    out += "invalid ";
  if (out.size() == 0)
    out = "(none) ";
  return out;
}


} // anonymous namespace


/**
 * @brief Property change handler.
 */
void FPEControlSvc::prophand (Gaudi::Details::PropertyBase& /*prop*/)
{
  if (!m_haveEnv) {
    // Save the current FP environment.
    fegetenv (&m_env);
    m_haveEnv = true;
  }
  else {
    // Reset to the FP environment before we started.
    fesetenv (&m_env);
  }

  // Figure out which exceptions to enable/disable.
  m_enabled = 0;
  m_disabled = 0;
  const std::vector<std::string>& v = m_exceptions.value();
  for (size_t i = 0; i < v.size(); i++) {
    bool onoff = true;
    const char* s = v[i].c_str();
    if (!s) continue;
    if (s[0] == '!') {
      onoff = false;
      ++s;
    }
    int thisexc = 0;
    if (strcasecmp (s, "inexact") == 0)
      thisexc = FE_INEXACT;
    else if (strcasecmp (s, "divbyzero") == 0)
      thisexc = FE_DIVBYZERO;
    else if (strcasecmp (s, "underflow") == 0)
      thisexc = FE_UNDERFLOW;
    else if (strcasecmp (s, "overflow") == 0)
      thisexc = FE_OVERFLOW;
    else if (strcasecmp (s, "invalid") == 0)
      thisexc = FE_INVALID;
    else {
      REPORT_MESSAGE (MSG::INFO)
        << "Unknown exception name: " << v[i];
      continue;
    }

    if (onoff) {
      m_enabled |= thisexc;
      m_disabled &= ~thisexc;
    }
    else {
      m_disabled |= thisexc;
      m_enabled &= ~thisexc;
    }
  }

  // Say what we're doing, and change the masks.
  REPORT_MESSAGE (MSG::INFO)
    << "Enable: " << mask_to_string (m_enabled)
    << "Disable: " << mask_to_string (m_disabled);

  setFPU();
}


/**
 * @brief Set the FPU exception masks from m_enabled and m_disabled.
 */
void FPEControlSvc::setFPU()
{
#ifdef __GLIBC__
  feenableexcept (m_enabled);
  fedisableexcept (m_disabled);
#elif defined __APPLE__
#include "CxxUtils/excepts.h"
  feenableexcept (m_enabled);
  fedisableexcept (m_disabled);
#else
  // The functions above are gnu-specific.
  // Without GNU, we do it the harder way.
  fenv_t newval;
  fegetenv(&newval);

  // CSR bit on means that the exception is masked.

  newval.__control_word &= ~m_enabled;
  newval.__control_word |=  m_disabled;
# ifdef __x86_64
  // SSE floating point uses separate exception masks.
  newval.__mxcsr &= (~m_enabled) << 7;
  newval.__mxcsr |= ( m_disabled) << 7;
# endif
  fesetenv(&newval);
#endif
}
