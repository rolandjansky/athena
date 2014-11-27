// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagVertexAuxContainer_v1.h 584372 2014-02-20 10:51:12Z jerdmann $
#ifndef XAODBTAGGING_VERSIONS_BTAGVERTEXAUXCONTAINER_V1_H
#define XAODBTAGGING_VERSIONS_BTAGVERTEXAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

namespace xAOD {

   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the BTagging EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 584372 $
   /// $Date: 2014-02-20 11:51:12 +0100 (Thu, 20 Feb 2014) $
   ///
   class BTagVertexAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      BTagVertexAuxContainer_v1();

   private:
          
               
     std::vector<float> chi2;
     std::vector<float> NDF;
     std::vector<float> pv_compatibility;
     std::vector<std::vector<float> > compToOtherSV;
    // std::vector<std::vector< ElementLink<xAOD::BTagVertexContainer> > > reco_vertexLink;

     std::vector<std::vector<float> > track_chi2;
     std::vector<std::vector<float> > track_NDF;
     std::vector<std::vector<float> > track_refPx;
     std::vector<std::vector<float> > track_refPy;
     std::vector<std::vector<float> > track_refPz;
     std::vector<std::vector< ElementLink<xAOD::TrackParticleContainer > > > track_links; 

       
   }; // class BTagVertexAuxContainer_v1

} // namespace xAOD

// Set up StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::BTagVertexAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODBTAGGING_VERSIONS_BTAGGINGAUXCONTAINER_V1_H
