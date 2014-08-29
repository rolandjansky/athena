/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODTruth/versions/TruthPileupEvent_v1.h"

namespace xAOD {


  TruthPileupEvent_v1::TruthPileupEvent_v1()
    : TruthEventBase_v1() {
  }


  /////////////////////////////////////////////////////////////////////////////


  Type::ObjectType TruthPileupEvent_v1::type() const {
    return Type::TruthPileupEvent;
  }


  void TruthPileupEvent_v1::toPersistent() {
    #ifndef XAOD_STANDALONE
    TruthEventBase_v1::toPersistent();
    #endif // not XAOD_STANDALONE
  }


} // namespace xAOD
