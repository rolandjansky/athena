// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPassBitsAuxContainer_v1.h 703368 2015-10-27 08:45:13Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGPASSBITSAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_TRIGPASSBITSAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// xAOD include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary container for TrigPassBitsContainer_v1
   ///
   /// This auxiliary container holds all the variables normally saved on
   /// <code>xAOD::TrigPassBits_v1</code> objects.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 703368 $
   /// $Date: 2015-10-27 09:45:13 +0100 (Tue, 27 Oct 2015) $
   ///
   class TrigPassBitsAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      TrigPassBitsAuxContainer_v1();

   private:
      std::vector< uint32_t > size;
      std::vector< std::vector< uint32_t > > passBits;
      std::vector< uint32_t > containerKey;
      std::vector< uint32_t > containerClid;

   }; // class TrigPassBitsAuxContainer_v1

} // namespace xAOD

// Declare the inheritqance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrigPassBitsAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGGER_VERSIONS_TRIGPASSBITSAUXCONTAINER_V1_H
