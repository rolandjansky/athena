// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SafeDeepCopy.h 676241 2015-06-18 06:15:44Z krasznaa $
#ifndef XAODCORE_TOOLS_SAFEDEEPCOPY_H
#define XAODCORE_TOOLS_SAFEDEEPCOPY_H

// Forward declaration(s):
namespace SG {
   class AuxElement;
}

namespace xAOD {

   /// Function that can deep-copy "problematic" objects
   ///
   /// Due to a number of different issues we can produce xAOD files that have
   /// some variables only filled in certain events. When this happens, the xAOD
   /// object still reports the variable to be present in all the events, though
   /// the auxiliary store's getAuxIDs() function. Only when one asks for the
   /// auxiliary ID, does the code detect that there's no such variable in the
   /// event after all.
   ///
   /// When this happens, the assignment operator in <code>SG::AuxElement</code>
   /// throws an exception.
   ///
   /// In order to be able to make deep copies of objects even from such
   /// problematic files, this function can be used instead. This function
   /// silently ignores these sort of issues, allowing analysis code to make
   /// use of buggy files.
   ///
   /// @param orig The original object that a copy is being made of
   /// @param copy The object to fill with the original's parameters
   ///
   void safeDeepCopy( const SG::AuxElement& orig, SG::AuxElement& copy );

} // namespace xAOD

#endif // XAODCORE_TOOLS_SAFEDEEPCOPY_H
