// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHPILEUPEVENT_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPILEUPEVENT_V1_H

// Local include(s):
#include "xAODTruth/TruthEventBase.h"

namespace xAOD {

   /// Class describing a pile-up truth event in the MC record
   ///
   /// @author Andy Buckley <Andy.Buckley@cern.ch>
   /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
   ///
   /// $Revision: 622193 $
   /// $Date: 2014-10-16 18:08:34 +0200 (Thu, 16 Oct 2014) $
   ///
   class TruthPileupEvent_v1 : public TruthEventBase_v1 {

   public:
      /// Default constructor
      TruthPileupEvent_v1();

      /// @todo Some identifying info about the type of pile-up, bunch xing, etc.?

      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const;

   }; // class TruthPileupEvent_v1

} // namespace xAOD

// Declare the base class of TruthPileupEvent_v1 to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthPileupEvent_v1, xAOD::TruthEventBase_v1 );

DATAVECTOR_BASE( xAOD::TruthPileupEvent_v1, xAOD::TruthEventBase_v1 );

#endif // XAODTRUTH_VERSIONS_TRUTHPILEUPEVENT_V1_H
