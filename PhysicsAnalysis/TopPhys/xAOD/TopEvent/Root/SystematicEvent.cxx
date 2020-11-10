/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// Local include(s).
#include "TopEvent/SystematicEvent.h"

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

namespace xAOD {

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(SystematicEvent, std::size_t, hashValue, setHashValue)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(SystematicEvent, unsigned int, ttreeIndex, setTtreeIndex)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(SystematicEvent, char, isLooseEvent, setIsLooseEvent)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodPhotons, setGoodPhotons)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodElectrons, setGoodElectrons)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodFwdElectrons, setGoodFwdElectrons)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodMuons, setGoodMuons)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodSoftMuons, setGoodSoftMuons)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodTaus, setGoodTaus)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodJets, setGoodJets)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodLargeRJets, setGoodLargeRJets)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodTrackJets, setGoodTrackJets)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(SystematicEvent, std::vector<unsigned int>, goodTracks, setGoodTracks)

} // namespace xAOD
