/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthPileupEvent_v1.cxx 622193 2014-10-16 16:08:34Z krasznaa $

// Local include(s):
#include "xAODTruth/versions/TruthPileupEvent_v1.h"

namespace xAOD {

   TruthPileupEvent_v1::TruthPileupEvent_v1()
      : TruthEventBase_v1() {

   }

   Type::ObjectType TruthPileupEvent_v1::type() const {

      return Type::TruthPileupEvent;
   }

} // namespace xAOD
