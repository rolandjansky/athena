// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_VERSIONS_MBTSMODULESAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_MBTSMODULESAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for xAOD::MBTSModuleContainer_v1
   ///
   /// descr
   /// @author Andrzej Zemla < azemla@cern.ch >
   ///
   class MBTSModuleAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      MBTSModuleAuxContainer_v1();

   private:
      std::vector< float > e;
      std::vector< float > eta;
      std::vector< float > phi;
      std::vector< float > time ;
      std::vector< int > mbtsQuality;
      std::vector< int > type;
      std::vector< int > mbtsModule;
      std::vector< int > channel;

   }; // class MBTSModuleAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::MBTSModuleAuxContainer_v1, xAOD::AuxContainerBase );

#endif //XAODFORWARD_VERSIONS_MBTSMODULESAUXCONTAINER_V1_H
