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


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#define private public
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "xAODTracking/TrackParticle.h"
#define public private
#include "TrkEventPrimitives/ParticleHypothesis.h" 
#include "xAODTracking/NeutralParticle.h"
#include "xAODTruth/TruthParticle.h"

class AtlasDetectorID;


namespace Trk {
  
  class IExtrapolator;
  class CaloExtension;

  class ParticleCaloExtensionTool : virtual public IParticleCaloExtensionTool, public AthAlgTool {
  public:
    
    ParticleCaloExtensionTool(const std::string&,const std::string&,const IInterface*);

    virtual ~ParticleCaloExtensionTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Method to dress a IParticle with the calo layers crossed by its track
        @param IParticle     reference to the particle
        @param extension     reference to a pointer to a CaloExtesion, will be updated if call is successfull
                             NEVER delete the pointer, you will cause a crash! 
        @param useCaching    configure whether the tool caches the result on the track particle
                             The default behavior is 'true' to ensure optimal performance
        @return true if the call was successful
    */
    bool caloExtension( const xAOD::IParticle& particle, const Trk::CaloExtension*& extension, bool useCaching = true  ) const final;
    Trk::CaloExtension* caloExtension( const TrackParameters& startPars, PropDirection propDir, ParticleHypothesis particleType ) const;
  private:
    
    const xAOD::TrackParticle* getTrackParticle(const xAOD::IParticle& particle ) const;

    Trk::CaloExtension* caloExtension( const xAOD::TruthParticle& particle ) const;
    Trk::CaloExtension* caloExtension( const xAOD::NeutralParticle& particle ) const;
    Trk::CaloExtension* caloExtension( const xAOD::TrackParticle& particle ) const;
    



    const AtlasDetectorID*       m_detID;
    ToolHandle< IExtrapolator >  m_extrapolator;
    ParticleHypothesis      m_particleType;
    std::string             m_particleTypeName;
    std::string             m_containerName;
    bool                    m_startFromPerigee;
  };
}

#endif
