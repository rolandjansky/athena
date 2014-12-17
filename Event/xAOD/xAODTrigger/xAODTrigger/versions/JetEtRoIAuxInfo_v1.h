// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoIAuxInfo_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_JETETROIAUXINFO_V1_H
#define XAODTRIGGER_VERSIONS_JETETROIAUXINFO_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

   /// Auxiliary store for a Jet-ET RoI object
   ///
   /// This auxiliary store describes a single Jet-ET RoI object, as we
   /// only have one of those in the ATLAS events.
   ///
   /// @author Lukas Heinrich <Lukas.Heinrich@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631149 $
   /// $Date: 2014-11-26 13:26:18 +0100 (Wed, 26 Nov 2014) $
   ///
   class JetEtRoIAuxInfo_v1 : public AuxInfoBase {

   public:
      /// Default constuctor
      JetEtRoIAuxInfo_v1();

   private:
      uint32_t roiWord;
      std::vector< std::string > thrNames;

   }; // class JetEtRoIAuxInfo_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::JetEtRoIAuxInfo_v1, xAOD::AuxInfoBase );

#endif // XAODTRIGGER_VERSIONS_JETETROIAUXINFO_V1_H
