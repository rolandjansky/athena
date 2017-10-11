/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
ParticleToCaloExtrapolationTool.h  -  Description
-------------------
begin   : February 2017
authors : Noemi Calace
***************************************************************************/
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_PARTICLETOCALOEXTRAPOLATIONTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_PARTICLETOCALOEXTRAPOLATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#define private public
#include "xAODTracking/TrackParticle.h"
#define public private
#include "TrackCaloClusterRecInterfaces/IParticleExtrapolationTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h" 
#include "xAODTracking/NeutralParticle.h"
#include "xAODTruth/TruthParticle.h"

class AtlasDetectorID;

namespace Trk {
  class IExtrapolator;
  class CaloExtension;
}

class ParticleToCaloExtrapolationTool : virtual public IParticleExtrapolationTool, public AthAlgTool {
  public:
    ParticleToCaloExtrapolationTool(const std::string&,const std::string&,const IInterface*);
    virtual ~ParticleToCaloExtrapolationTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Method to dress a IParticle with the calo layers crossed by its track
     *  Tracks are decorated using the calo layer index and the intersection
     *  @param IParticle     reference to the particle
     *  @param extension     reference to a pointer to a CaloExtesion, will be updated if call is successfull
     *                       NEVER delete the pointer, you will cause a crash! 
     *  @param useCaching    configure whether the tool caches the result on the track particle
     *                       The default behavior is 'true' to ensure optimal performance
     *  @return true if the call was successful
     * 
     */
    bool particleToCaloExtrapolate( const xAOD::IParticle& particle, const Trk::CaloExtension*& extension) const final;
    Trk::CaloExtension* caloExtension( const Trk::TrackParameters& startPars, Trk::PropDirection propDir, Trk::ParticleHypothesis particleType ) const;
    
  private:    
    const xAOD::TrackParticle* getTrackParticle(const xAOD::IParticle& particle ) const;
    Trk::CaloExtension* particleToCaloExtrapolate( const xAOD::TruthParticle& particle ) const;
    Trk::CaloExtension* particleToCaloExtrapolate( const xAOD::NeutralParticle& particle ) const;
    Trk::CaloExtension* particleToCaloExtrapolate( const xAOD::TrackParticle& particle ) const;

    const AtlasDetectorID*            m_detID;
    ToolHandle< Trk::IExtrapolator >  m_extrapolator;
    Trk::ParticleHypothesis           m_particleType;
    std::string                       m_particleTypeName;
    std::string                       m_containerName;
    
};

#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_PARTICLETOCALOEXTRAPOLATIONTOOL_H
