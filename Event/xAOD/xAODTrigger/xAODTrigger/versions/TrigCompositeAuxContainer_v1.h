// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
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
   /// $Revision$
   /// $Date$
   ///
   class TrigCompositeAuxContainer_v1 : public ByteStreamAuxContainer_v1 {

   public:
      /// Default constuctor
      TrigCompositeAuxContainer_v1();

   private:
      std::vector< std::string > name;

      std::vector< std::vector< std::string > > linkColName;
      std::vector< std::vector< int > >         linkColIndex;

   }; // class TrigCompositeAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigCompositeAuxContainer_v1, 1175586382, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::TrigCompositeAuxContainer_v1, xAOD::ByteStreamAuxContainer_v1 );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_TRIGCOMPOSITEAUXCONTAINER_V1_H
