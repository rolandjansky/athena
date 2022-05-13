/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_INDETCANDIDATECOLLECTION_H
#define MUONCOMBINEDEVENT_INDETCANDIDATECOLLECTION_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "MuonCombinedEvent/InDetCandidate.h"

/**
This typedef represents a collection of InDetCandidate objects.
*/
typedef DataVector<MuonCombined::InDetCandidate> InDetCandidateCollection;
CLASS_DEF(InDetCandidateCollection, 1204676707, 1)

#endif
