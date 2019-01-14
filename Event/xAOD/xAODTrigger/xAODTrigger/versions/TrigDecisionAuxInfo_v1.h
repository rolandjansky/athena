// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionAuxInfo_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGDECISIONAUXINFO_V1_H
#define XAODTRIGGER_VERSIONS_TRIGDECISIONAUXINFO_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// xAOD include(s):
#include "xAODCore/AuxInfoBase.h"

// TrigCompositeUtils::DecisionID
#include "xAODTrigger/TrigComposite.h"

namespace xAOD {

   /// Class holding the data handled by TrigDecision_v1
   ///
   /// This is the auxiliary class holding the trigger decision
   /// information in xAOD files.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class TrigDecisionAuxInfo_v1 : public AuxInfoBase {

   public:
      /// Default constructor
      TrigDecisionAuxInfo_v1();

   private:
      /// @name Trigger configuration key(s)
      /// @{
      uint32_t smk;
      /// @}

      /// @name Level-1 result variables
      /// @{
      char bgCode;
      std::vector< uint32_t > tav;
      std::vector< uint32_t > tap;
      std::vector< uint32_t > tbp;
      /// @}

      /// @name HLT result variables
      /// @{
      uint32_t lvl2ErrorBits;
      uint32_t efErrorBits;
      bool lvl2Truncated;
      bool efTruncated;
      std::vector< uint32_t > lvl2PassedPhysics;
      std::vector< uint32_t > efPassedPhysics;
      std::vector< uint32_t > lvl2PassedRaw;
      std::vector< uint32_t > efPassedRaw;
      std::vector< uint32_t > lvl2PassedThrough;
      std::vector< uint32_t > efPassedThrough;
      std::vector< uint32_t > lvl2Prescaled;
      std::vector< uint32_t > efPrescaled;
      std::vector< uint32_t > lvl2Resurrected;
      std::vector< uint32_t > efResurrected;
      /// @}

   }; // class TrigDecisionAuxInfo_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigDecisionAuxInfo_v1, xAOD::AuxInfoBase );

#endif // XAODTRIGGER_VERSIONS_TRIGDECISIONAUXINFO_V1_H
