/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
ParticleCaloExtensionTool.h  -  Description
-------------------
begin   : Summer 2014
authors : Niels van Eldik (CERN PH-ATC)
***************************************************************************/
#ifndef TRKPARTICLECREATOR_PARTICLECALOEXTENSIONTOOL_H
#define TRKPARTICLECREATOR_PARTICLECALOEXTENSIONTOOL_H

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/ParticleCaloExtension.h"
#include "TrkEventPrimitives/ParticleHypothesis.h" 

class AtlasDetectorID;

namespace MagField 
{
  class IMagFieldSvc;
}


namespace Trk {
  
  class IExtrapolator;

  class ParticleCaloExtensionTool : virtual public IParticleCaloExtensionTool, public AthAlgTool {
  public:
    
    ParticleCaloExtensionTool(const std::string&,const std::string&,const IInterface*);

    virtual ~ParticleCaloExtensionTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Method to dress a TrackParticle with the calo layers crossed by its track
        @param trackParticle 
        @return ParticleCaloExtension, caller does not obtain ownership of the pointer and should not delete it
    */
    const xAOD::ParticleCaloExtension* caloExtension( xAOD::TrackParticle& trackParticle ) const;

  private:
    const AtlasDetectorID*       m_detID;
    ToolHandle< IExtrapolator >  m_extrapolator;
    ServiceHandle<MagField::IMagFieldSvc>  m_magFieldSvc;

    Trk::ParticleHypothesis      m_particleType;
    std::string                  m_particleTypeName;
    std::string                  m_containerName;
  };
}

#endif
