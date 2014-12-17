// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigNavigationAuxInfo_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGNAVIGATIONAUXINFO_V1_H
#define XAODTRIGGER_VERSIONS_TRIGNAVIGATIONAUXINFO_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// xAOD include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

   /// Class holding the data handled by TrigNavigation_v1
   /// (bsaed on the TrigDecision_v1)
   /// This is the auxiliary class holding the trigger navigation
   /// information in xAOD files.
   ///
   /// @author Tomasz Bold
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class TrigNavigationAuxInfo_v1 : public AuxInfoBase {

   public:
      /// Default constructor
      TrigNavigationAuxInfo_v1();

   private:
      std::vector< uint32_t > serialized;

   }; // class TrigNavigationAuxInfo_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigNavigationAuxInfo_v1, xAOD::AuxInfoBase );

#endif // XAODTRIGGER_VERSIONS_TRIGNAVIGATIONAUXINFO_V1_H
