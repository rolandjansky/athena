// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_VERSIONS_FORWARDEVENTINFOSAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_FORWARDEVENTINFOSAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for xAOD::ForwardEventInfoContainer_v1
   ///
   /// descr
   /// @author Andrzej Zemla < azemla@cern.ch >
   ///
   class ForwardEventInfoAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      ForwardEventInfoAuxContainer_v1();

   private:

      std::vector<float> timeDiff;
      std::vector<float> timeA;
      std::vector<float> timeC;
      std::vector<unsigned short> countA;
      std::vector<unsigned short> countC;

   }; // class ForwardEventInfoAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ForwardEventInfoAuxContainer_v1, xAOD::AuxContainerBase );

#endif //XAODFORWARD_VERSIONS_FORWARDEVENTINFOSAUXCONTAINER_V1_H
