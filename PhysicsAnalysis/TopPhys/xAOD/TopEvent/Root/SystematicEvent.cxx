/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: SystematicEvent.cxx 692539 2015-09-02 00:20:52Z morrisj $
#include "TopEvent/SystematicEvent.h"

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODCore/AddDVProxy.h"

namespace xAOD {
  // Aux Container
  SystematicEventAuxContainer::SystematicEventAuxContainer() :
    AuxContainerBase() {
    AUX_VARIABLE(hashValue);
    AUX_VARIABLE(ttreeIndex);
    AUX_VARIABLE(isLooseEvent);

    AUX_VARIABLE(goodPhotons);
    AUX_VARIABLE(goodElectrons);
    AUX_VARIABLE(goodFwdElectrons);
    AUX_VARIABLE(goodMuons);
    AUX_VARIABLE(goodSoftMuons);
    AUX_VARIABLE(goodTaus);
    AUX_VARIABLE(goodJets);
    AUX_VARIABLE(goodLargeRJets);
    AUX_VARIABLE(goodTrackJets);
    AUX_VARIABLE(goodTracks);
  }

  /// Interface class
  SystematicEvent::SystematicEvent() :
    SG::AuxElement() {
  }

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
}
ADD_NS_DV_PROXY(xAOD, SystematicEventContainer);
