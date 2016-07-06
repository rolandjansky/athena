// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TChainStateTracker.h 619763 2014-10-03 08:36:08Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TCHAINSTATETRACKER_H
#define XAODROOTACCESS_TOOLS_TCHAINSTATETRACKER_H

// ROOT include(s):
#include <TObject.h>

namespace xAOD {

   /// Class helping with tracking the state of TChain objects
   ///
   /// TChains are tricky. Depending on the user's usage pattern, the TChain
   /// object given to a TEvent object may change its internal state without
   /// the TEvent object knowing. In which case the internal state of TEvent
   /// becomes inconsistent.
   ///
   /// Such an object is used by TEvent to know when the TChain given to it
   /// has changed the currently opened file. So that it can re-initialise its
   /// internal state correctly when this happens.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 619763 $
   /// $Date: 2014-10-03 10:36:08 +0200 (Fri, 03 Oct 2014) $
   ///
   class TChainStateTracker : public ::TObject {

   public:
      /// Default constructor
      TChainStateTracker();

      /// Function called when the TChain opens a new input file
      virtual ::Bool_t Notify();

      /// Check whether there was an internal state change without us knowing
      ::Bool_t internalStateChanged() const;
      /// Reset the object
      void reset();

   private:
      /// Flag showing when an internal state change is detected
      ::Bool_t m_stateChanged;

   }; // class TChainStateTracker

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TCHAINSTATETRACKER_H
