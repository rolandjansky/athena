// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionAuxInfo_v1.h 581968 2014-02-06 16:08:52Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGDECISIONAUXINFO_V1_H
#define XAODTRIGGER_VERSIONS_TRIGDECISIONAUXINFO_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// xAOD include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

   /// Class holding the data handled by TrigDecision_v1
   ///
   /// This is the auxiliary class holding the trigger decision
   /// information in xAOD files.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 581968 $
   /// $Date: 2014-02-06 17:08:52 +0100 (Thu, 06 Feb 2014) $
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

// Set up a CLID for the type:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigDecisionAuxInfo_v1, 44251926, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::TrigDecisionAuxInfo_v1, xAOD::AuxInfoBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_TRIGDECISIONAUXINFO_V1_H
