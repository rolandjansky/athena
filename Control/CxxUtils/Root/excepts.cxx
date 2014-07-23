/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/src/excepts.cxx
 * @author David Quarrie <David.Quarrie@cern.ch>
 * @date Jan, 2010
 * @brief Implementations of feenableexcept()/fedisableexcept() functions for MacOSX.
 */

#include "CxxUtils/excepts.h"

#ifdef __APPLE__
#include <fenv.h>

int
feenableexcept (unsigned int excepts)
{
   static fenv_t fenv;
   unsigned int new_excepts = excepts & FE_ALL_EXCEPT,
                old_excepts;  // previous masks

   if ( fegetenv (&fenv) ) return -1;
   old_excepts = fenv.__control & FE_ALL_EXCEPT;

   // unmask
   fenv.__control &= ~new_excepts;
   fenv.__mxcsr   &= ~(new_excepts << 7);

   return ( fesetenv (&fenv) ? -1 : old_excepts );
}

int
fedisableexcept (unsigned int excepts)
{
   static fenv_t fenv;
   unsigned int new_excepts = excepts & FE_ALL_EXCEPT,
                old_excepts;  // all previous masks

   if ( fegetenv (&fenv) ) return -1;
   old_excepts = fenv.__control & FE_ALL_EXCEPT;

   // mask
   fenv.__control |= new_excepts;
   fenv.__mxcsr   |= new_excepts << 7;

   return ( fesetenv (&fenv) ? -1 : old_excepts );
}
#endif
