// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHPILEUPEVENT_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPILEUPEVENT_V1_H

#include "xAODTruth/TruthEventBase.h"

namespace xAOD {


  /// Class describing a pile-up truth event in the MC record
  ///
  /// @author Andy Buckley <Andy.Buckley@cern.ch>
  /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
  ///
  class TruthPileupEvent_v1 : public TruthEventBase_v1 {
  public:

    /// Default constructor
    TruthPileupEvent_v1();

    /// @todo Some identifying info about the type of pile-up, bunch xing, etc.?

    /// The type of the object as a simple enumeration
    Type::ObjectType type() const;

    /// Function making sure that the object is ready for persistification
    void toPersistent();

  };


}

#include "xAODCore/BaseInfo.h"
SG_BASE (xAOD::TruthPileupEvent_v1, xAOD::TruthEventBase_v1);

#endif
