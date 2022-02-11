// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/FPControl.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2021
 * @brief Helper to control FP exceptions.
 */


#ifndef CXXUTILS_FPCONTROL_H
#define CXXUTILS_FPCONTROL_H


#include "CxxUtils/bitmask.h"
#include <fenv.h>


namespace CxxUtils {


/**
 * @brief Helper to control FP exceptions.
 *
 * This helper can be used where wants to temporarily disable the
 * detection of FP exceptions.  For example:
 *
 @code
 *  {
 *    CxxUtils::FPControl ctl;
 *    ctl.disable (CxxUtils::FPControl::Exc::divbyzero);
 *      ... division by zero exception is now disabled.
 *  }
 *  // Previous exception setting is restored.
 *  // The flag saying whether a div-by-zero exception occurred
 *  // will be left cleared.
 */
class FPControl
{
public:
  // Known exception types.
  enum class Exc
  {
    divbyzero = 1,
    inexact = 2,
    invalid = 4,
    overflow = 8,
    underflow = 16,
    // Special value meaning all known exceptions.
    // Not necessarily the same as the OR of all the above
    // if the platform defines additional exception types beyond these.
    all = 32,
    ATH_BITMASK
  };


  /**
   * @break Constructor.
   *
   * This will save the current FP environment.
   */
  FPControl();


  /**
   * @break Destructor.
   *
   * Restore the FP environment to what it was when this object was created.
   * Except for the flags telling whether exceptions occurred.
   * For exceptions which were disabled here, the corresponding
   * flags are cleared.  Other flags are left unchanged.
   */
  ~FPControl();


  /**
   * @brief Disable all exceptions.
   */
  void holdExceptions();


  /**
   * @brief Enable a set of exceptions.
   * @param exc The set of exceptions to enable.
   */
  void enable (Exc exc);


  /**
   * @brief Disable a set of exceptions.
   * @param exc The set of exceptions to enable.
   */
  void disable (Exc exc);


private:
  /**
   * @brief Translate exceptions from Exc to a mask used by the fe* functions.
   * @param exc Mask of exceptions to translate.
   */
  int excToMask (Exc exc);

  /// FP environment saved from the constructor.
  fenv_t m_env;

  // Mask of exceptions that have been disabled.
  int m_masked = 0;
};



} // namespace CxxUtils


#endif // not CXXUTILS_FPCONTROL_H
