/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCore/AuxStoreAccessorMacros.h"

#include "xAODTrigMinBias/versions/TrigTrackCounts_v1.h"

namespace xAOD {

  // Default ctor
  TrigTrackCounts_v1::TrigTrackCounts_v1()
    : SG::AuxElement() {
  }

  TrigTrackCounts_v1::~TrigTrackCounts_v1(){}
  // Simple accessor functions for the "default variable" access pattern

  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigTrackCounts_v1, std::vector<float>, z0_pt, setZ0_pt)
  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigTrackCounts_v1, std::vector<float>, eta_phi, setEta_phi)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, unsigned int, z0Bins, setZ0Bins)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, float, z0Min, setZ0Min)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, float, z0Max, setZ0Max)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, unsigned int, ptBins, setPtBins)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, float, ptMin, setPtMin)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, float, ptMax, setPtMax)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, unsigned int, etaBins, setEtaBins)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, float, etaMin, setEtaMin)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, float, etaMax, setEtaMax)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, unsigned int, phiBins, setPhiBins)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, float, phiMin, setPhiMin)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigTrackCounts_v1, float, phiMax, setPhiMax)
}
