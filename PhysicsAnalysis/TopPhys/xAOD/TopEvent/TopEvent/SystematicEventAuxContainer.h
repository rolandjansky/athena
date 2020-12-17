// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ANALYSISTOP_TOPEVENT_SYSTEMATICEVENTAUXCONTAINER_H
#define ANALYSISTOP_TOPEVENT_SYSTEMATICEVENTAUXCONTAINER_H

// EDM include(s).
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/CLASS_DEF.h"

// System include(s).
#include <vector>

namespace xAOD {

   /// Auxiliary container for @c xAOD::SystematicEventContainer
   class SystematicEventAuxContainer : public AuxContainerBase {

   public:
      /// Default constructor
      SystematicEventAuxContainer();

   private:
      std::vector<std::size_t> hashValue;
      std::vector<unsigned int> ttreeIndex;
      std::vector<char> isLooseEvent;

      std::vector<std::vector<unsigned int> > goodPhotons;
      std::vector<std::vector<unsigned int> > goodElectrons;
      std::vector<std::vector<unsigned int> > goodFwdElectrons;
      std::vector<std::vector<unsigned int> > goodMuons;
      std::vector<std::vector<unsigned int> > goodSoftMuons;
      std::vector<std::vector<unsigned int> > goodTaus;
      std::vector<std::vector<unsigned int> > goodJets;
      std::vector<std::vector<unsigned int> > goodLargeRJets;
      std::vector<std::vector<unsigned int> > goodTrackJets;
      std::vector<std::vector<unsigned int> > goodTracks;

   }; // class SystematicEventAuxContainer

} // namespace xAOD

// Define a ClassID for the type.
CLASS_DEF( xAOD::SystematicEventAuxContainer, 1283481454, 1 )

#endif // not ANALYSISTOP_TOPEVENT_SYSTEMATICEVENTAUXCONTAINER_H
