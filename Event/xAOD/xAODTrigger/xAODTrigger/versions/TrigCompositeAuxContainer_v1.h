// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCompositeAuxContainer_v1.h 631318 2014-11-26 18:32:04Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGCOMPOSITEAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_TRIGCOMPOSITEAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// Local include(s):
#include "xAODTrigger/versions/ByteStreamAuxContainer_v1.h"

namespace xAOD {

   /// Auxiliary store for TrigComposite_v1 containers
   ///
   /// This is an auxiliary container that can be streamed with ROOT without
   /// any special code necessary, handling all the variable types that we
   /// want to provide for the HLT algorithms.
   ///
   /// @author Tomasz Bold <Tomasz.Bold@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 631318 $
   /// $Date: 2014-11-26 19:32:04 +0100 (Wed, 26 Nov 2014) $
   ///
   class TrigCompositeAuxContainer_v1 : public ByteStreamAuxContainer_v1 {

   public:
      /// Default constuctor
      TrigCompositeAuxContainer_v1();

   private:
      std::vector< std::string > name;

      std::vector< std::vector< std::string > > linkColNames;
      std::vector< std::vector< uint32_t > >    linkColKeys;
      std::vector< std::vector< uint16_t > >    linkColIndices;
      std::vector< std::vector< uint32_t > >    linkColClids;

   }; // class TrigCompositeAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigCompositeAuxContainer_v1, xAOD::ByteStreamAuxContainer_v1 );

#endif // XAODTRIGGER_VERSIONS_TRIGCOMPOSITEAUXCONTAINER_V1_H
