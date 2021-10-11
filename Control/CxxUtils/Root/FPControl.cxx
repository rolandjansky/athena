/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/src/FPControl.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2021
 * @brief Helper to control FP exceptions.
 */


#include "CxxUtils/FPControl.h"


namespace CxxUtils {


/**
 * @break Constructor.
 *
 * This will save the current FP environment.
 */
FPControl::FPControl()
{
  fegetenv (&m_env);
}


/**
 * @break Destructor.
 *
 * Restore the FP environment to what it was when this object was created.
 * Except for the flags telling whether exceptions occurred.
 * For exceptions which were disabled here, the corresponding
 * flags are cleared.  Other flags are left unchanged.
 */
FPControl::~FPControl()
{
  if (m_masked) {
    feclearexcept (m_masked);
  }
  feupdateenv (&m_env);
}


/**
 * @brief Disable all exceptions.
 */
void FPControl::holdExceptions()
{
  fenv_t tmp;
  feholdexcept (&tmp);
  m_masked |= FE_ALL_EXCEPT;
}


/**
 * @brief Enable a set of exceptions.
 * @param exc The set of exceptions to enable.
 */
void FPControl::enable (Exc exc)
{
  int mask = excToMask (exc);
  feenableexcept (mask);
  m_masked &= ~mask;
}


/**
 * @brief Disable a set of exceptions.
 * @param exc The set of exceptions to enable.
 */
void FPControl::disable (Exc exc)
{
  int mask = excToMask (exc);
  fedisableexcept (mask);
  m_masked |= mask;
}


/**
 * @brief Translate exceptions from Exc to a mask used by the fe* functions.
 * @param exc Mask of exceptions to translate.
 */
int FPControl::excToMask (Exc exc)
{
  // A platform isn't required to define all of these...
#ifdef FE_ALL_EXCEPT
  if (CxxUtils::test (exc, Exc::all))
    return FE_ALL_EXCEPT;
#endif
  int mask = 0;
#ifdef FE_DIVBYZERO
  if (CxxUtils::test (exc, Exc::divbyzero)) mask |= FE_DIVBYZERO;
#endif
#ifdef FE_INEXACT
  if (CxxUtils::test (exc, Exc::inexact))   mask |= FE_INEXACT;
#endif
#ifdef FE_INVALID
  if (CxxUtils::test (exc, Exc::invalid))   mask |= FE_INVALID;
#endif
#ifdef FE_OVERFLOW
  if (CxxUtils::test (exc, Exc::overflow))  mask |= FE_OVERFLOW;
#endif
#ifdef FE_UNDERFLOW
  if (CxxUtils::test (exc, Exc::underflow)) mask |= FE_UNDERFLOW;
#endif
  return mask;
}


} // namespace CxxUtils

