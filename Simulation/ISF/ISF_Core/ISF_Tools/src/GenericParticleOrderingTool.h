/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericParticleOrderingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_GENERICPARTICLEORDERINGTOOL_H
#define ISF_TOOLS_GENERICPARTICLEORDERINGTOOL_H 1

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_Interfaces/IParticleOrderingTool.h"
#include "ISF_Event/SimSvcID.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

namespace ISF {

    
  /** @class GenericParticleOrderingTool

      This AtheanAlgTool orders the particles according to it's SimSvcID.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class GenericParticleOrderingTool : public AthAlgTool,
                                      public IParticleOrderingTool {

    public: 
      /** Constructor with parameters */
      GenericParticleOrderingTool( const std::string& t, const std::string& n, const IInterface* p );

      /** Virtual destructor */
      virtual ~GenericParticleOrderingTool();

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();
       
      /** Returns a pass boolean on the particle  */
      virtual ISF::ParticleOrder computeOrder(const ISFParticle& p) const;
     
    private:
      ParticleOrder                         m_geoIDorder[AtlasDetDescr::fNumAtlasRegions]; //!< particle order per sub detector
      ParticleOrder                         m_simIDorder[ISF::fMaxNumAtlasSimIDs];         //!< particle order per sim svc
  }; 
  
}

#endif //> !ISF_TOOLS_GENERICPARTICLEORDERINGTOOL_H

