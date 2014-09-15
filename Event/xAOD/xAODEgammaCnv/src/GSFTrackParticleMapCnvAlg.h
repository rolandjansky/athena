// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef XAODMAKER_GSFTRACKPARTICLEMAPCREATOR_H
#define XAODMAKER_GSFTRACKPARTICLEMAPCREATOR_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace xAODMaker {

   /**
    *  @short Algorithm creating xAOD::Electrons from Analysis::Electrons
    *
    *         This algorithm can be used to translate the Electrons coming
    *         from an AOD, and create xAOD::Electron objects out of them
    *         for an output xAOD.
    *
    *
    */
   class GSFTrackParticleMapCnvAlg : public AthAlgorithm {

   public:
     /// Regular algorithm constructor
     GSFTrackParticleMapCnvAlg( const std::string& name, ISvcLocator* svcLoc );
     
      /// Function initialising the algorithm
     virtual StatusCode initialize();
     /// Function executing the algorithm
     virtual StatusCode execute();
     
   private:

     /// The name of the AOD GSF TrackParticle container
     std::string m_aodContainerName;    	 
     /// The name of the AOD GSF TrackParticle container
     std::string m_aodContainerNameGSF;
     /// The name of the xAOD GSF TrackParticle container
     std::string m_xaodContainerNameGSF;
     /// The name of the xAOD non-GSF TrackParticle container
     std::string m_xaodContainerName;     
     /// The name of the map from AOD GSF TrackParticles to AOD TrackParticles
     std::string   m_trackMapName;
     
  }; // class GSFTrackParticleMapCnvAlg

} // namespace xAODMaker

#endif // XAODMAKER_GSFTRACKPARTICLEMAPCREATOR_H
