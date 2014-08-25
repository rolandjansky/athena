/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
ParticleCaloCellAssociationTool.h  -  Description
-------------------
begin   : Summer 2014
authors : Niels van Eldik (CERN PH-ATC)
***************************************************************************/
#ifndef TRKPARTICLECREATOR_PARTICLECALOCELLASSOCIATION_H
#define TRKPARTICLECREATOR_PARTICLECALOCELLASSOCIATION_H

#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODTracking/TrackParticle.h"


namespace Trk {
  
  class IParticleCaloExtensionTool;

  class ParticleCaloCellAssociationTool : virtual public IParticleCaloCellAssociationTool, public AthAlgTool {
  public:
    
    ParticleCaloCellAssociationTool(const std::string&,const std::string&,const IInterface*);

    virtual ~ParticleCaloCellAssociationTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Method to dress a TrackParticle with the calo cells crossed by its track
        @param trackParticle 
        @return ParticleCaloExtension, caller does not obtain ownership of the pointer and should not delete it
    */
    const xAOD::ParticleCaloExtension* caloAssociation( xAOD::TrackParticle& trackParticle ) const;

  private:
    ToolHandle< IParticleCaloExtensionTool >  m_caloExtensionTool;
  };
}

#endif
