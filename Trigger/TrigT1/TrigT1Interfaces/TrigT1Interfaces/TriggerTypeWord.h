// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_TRIGGERTYPEWORD_H
#define TRIGT1INTERFACES_TRIGGERTYPEWORD_H

namespace LVL1 {

   /**
    *  @short Namespace for the trigger type word constants
    *
    *         This is a fairly old, and as far as I know, absolutely unused
    *         namespace. I guess it could've been used in the earliest
    *         streaming tests, but nowadays the simulation shouldn't depend
    *         on it. (These things come from the trigger configuration if
    *         I'm not mistaken...)
    *
    * @author Tadashi Maeno <Tadashi.Maeno@cern.ch>
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   namespace TriggerTypeWord {
 
      enum {
         Physics       = 0x80,
         PriorityLevel = 0x40,
         Egamma        = 0x20,
         TauHdr        = 0x10,
         Jet           = 0x08,
         EnergySum     = 0x04,
         HighPtMuon    = 0x02,
         LowPtMuon     = 0x01
      };

   } // namespace TriggerTypeWord

} // namespace LVL1

#endif
