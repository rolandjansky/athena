/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*- c++ -*-

// $Id: JetTrigAuxContainer_v2.h 2017-02-12 16:52 sschramm $
#ifndef XAODJET_VERSIONS_JETTRIGAUXCONTAINER_V2_H
#define XAODJET_VERSIONS_JETTRIGAUXCONTAINER_V2_H
#ifndef SIMULATIONBASE

// System include(s):
#include <vector>

// ROOT include(s):
#include <Math/Vector4D.h>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODTrigger/versions/ByteStreamAuxContainer_v1.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODJet/JetTypes.h"

namespace xAOD {

   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the jet EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 576027 $
   /// $Date: 2013-12-17 22:15:51 +0100 (Tue, 17 Dec 2013) $
   ///
   class JetTrigAuxContainer_v2 : public ByteStreamAuxContainer_v1 {

   public:
      /// Default constructor
      JetTrigAuxContainer_v2();

   private:
     /// Type used for the constituent links:
     typedef std::vector< std::vector< ElementLink< IParticleContainer > > >
     ConstituentLinks_t;
     
     std::vector<float> pt;
     std::vector<float> eta;
     std::vector<float> phi;
     std::vector<float> m;
     
     ConstituentLinks_t constituentLinks;
     ConstituentLinks_t GhostTrack;
     std::vector< std::vector< float > > constituentWeights;

   }; // class JetTrigAuxContainer_v2

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::JetTrigAuxContainer_v2, xAOD::ByteStreamAuxContainer_v1 );
#endif

#endif // XAODJET_VERSIONS_JETAUXCONTAINER_V2_H
