/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUONCANDIDATECOLLECTION_H
#define MUONCOMBINEDEVENT_MUONCANDIDATECOLLECTION_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "MuonCombinedEvent/MuonCandidate.h"

/**
This typedef represents a collection of MuonCandidate objects.
*/
typedef DataVector<MuonCombined::MuonCandidate> MuonCandidateCollection;
CLASS_DEF(MuonCandidateCollection, 1179531267, 1)

#endif
