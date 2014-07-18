// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Conditions.h 317606 2010-08-25 11:32:46Z krasznaa $
#ifndef TRIGGERD3PDMAKER_CONDITIONS_H
#define TRIGGERD3PDMAKER_CONDITIONS_H

namespace D3PD {

   /**
    *  @short Namespace for the trigger decision types
    *
    *         To have a similar formalism as the offline TrigDecisionTool,
    *         the DecisionTypes enumeration is put into its own namespace.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 337331 $
    * $Date: 2010-08-25 13:32:46 +0200 (Wed, 25 Aug 2010) $
    */
   namespace TrigDefs {

      /**
       *  @short Trigger decision types available from the D3PDs
       *
       *         This is the list of trigger decision types that can potentially
       *         be retrieved from a D3PD. The naming tries to follow the one used
       *         in the offline TrigDecisionTool code, but can't follow it 1-1 for
       *         obvious reasons.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 337331 $
       * $Date: 2010-08-25 13:32:46 +0200 (Wed, 25 Aug 2010) $
       */
      enum DecisionTypes {

         /// "The" physics decision
         /**
          * This type should be used when you're interested in the trigger decision
          * that should be used for physics. If the decision is true with this requirement,
          * it means that the item/chain was not prescaled, was not resurrected, and it
          * didn't just pass because of passthrough.
          */
         Physics = 0,
         /// The "raw" decision
         /**
          * This type should be used when you're only interested in whether the particular
          * HLT chain fired or not. The chain could've fired for many reasons that would
          * exclude it from returning <code>kTRUE</code> with the Physics requirement.
          *
          * Note: Only meaningful for HLT chains
          */
         requireDecision = 1,
         /// The decision while allowing the chain to be resurrected
         /**
          * If a chain was prescaled away, but since the event was accepted using another
          * chain this chain was resurrected, this condition can be used to check whether
          * this resurrected chain passed the trigger conditions. This is mostly useful
          * for trigger efficiency studies, should not be used in physics analyses!
          *
          * Note: Only meaningful for HLT chains
          */
         allowResurrected = 2,
         /// The passthrough decision for the chain
         /**
          * A chain can fire because of passthrough. Using this condition one can figure out
          * if a chain was set to passthrough in the current event or not. This is designed
          * for expert-level trigger analyses.
          *
          * Note: Only meaningful for HLT chains
          */
         passedThrough = 3,
         /// LVL1 Trigger Before Prescale decision
         /**
          * This is the first "level" of the LVL1 decision. It shows the LVL1 result before
          * prescale and applying the vetoes. Only intended for specific trigger analyses.
          *
          * Note: Only meaningful for LVL1 items
          */
         L1_TBP = 4,
         /// LVL1 Trigger After Prescale decision
         /**
          * This is the second "level" of the LVL1 decision. It shows the LVL1 result after
          * prescale, but before applying the vetoes. Only inteded for specific trigger
          * analyses.
          *
          * Note: Only meaningful for LVL1 items
          */
         L1_TAP = 5

      }; // enum DecisionTypes

   } // namespace TrigDefs

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_CONDITIONS_H
