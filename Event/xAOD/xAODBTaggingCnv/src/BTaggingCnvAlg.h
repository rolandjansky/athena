// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef XAODMAKER_BTAGGINGCREATOR_H
#define XAODMAKER_BTAGGINGCREATOR_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// xAOD include
#include "xAODBTagging/BTaggingContainer.h"

namespace Analysis {
  class BTagging;
}


namespace xAODMaker {

   /**
    *  @short Algorithm creating xAOD::BTagging objects from AOD collection
    *
    *         This algorithm can be used to translate the BTaggings coming
    *         from an AOD, and create xAOD::BTagging objects out of them
    *         for an output xAOD.
    *
    *
    */
   class BTaggingCnvAlg : public AthAlgorithm {

   public:

     /// Regular algorithm constructor
     BTaggingCnvAlg( const std::string& name, ISvcLocator* svcLoc );
     
      /// Function initialising the algorithm
     virtual StatusCode initialize();

     /// Function executing the algorithm
     virtual StatusCode execute();
     
   private:

     /// Fills in the SV0 information
     /*
     void setParameters(const Analysis::BTagging&, xAOD::BTagging& );
     void checkAndSetParameter(egammaParameters::ParamDef aodParameter,xAOD::EgammaParameters::ShowerShapeType xaodParameter, const Analysis::BTagging& , xAOD::BTagging& );
     */

     /// Set the element links between the various particles
     /*
       void setLinks(const Analysis::BTagging&, xAOD::BTagging& ) const;
       ElementLink<xAOD::TrackParticleContainer> getNewLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, const std::string& name) const;
     */

     /// The key of the input BTaggingContainer
     std::string m_aodContainerName;

     /// The key for the output xAOD::BTaggingContainer
     std::string m_xaodContainerName;

     //Other Keys
     /*
     std::string   m_trackMapName;           //!<Old new Track Map Name
     std::string   m_trackParticleContainerName; //!<AOD track Collection
     */

  }; // class BTaggingCnvAlg

} // namespace xAODMaker

#endif // XAODMAKER_BTAGGINGCREATOR_H
