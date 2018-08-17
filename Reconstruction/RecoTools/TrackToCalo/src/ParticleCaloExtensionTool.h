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

  class ParticleCaloExtensionTool : virtual public IParticleCaloExtensionTool, public AthAlgTool {
  public:
    
    ParticleCaloExtensionTool(const std::string&,const std::string&,const IInterface*);

    virtual ~ParticleCaloExtensionTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    /** Method returning the calo layers crossed 
   * by an IParticle track or itself if it is a TruthParticle
   * @param IParticle     reference to the Particle
   * @param extension     unique_ptr ref to a CaloExtension, will be updated if call is successfull 
   * @return true if the call was successful
   */
  virtual bool caloExtension( const xAOD::IParticle& particle, std::unique_ptr<Trk::CaloExtension>& extension ) const override final;
  
  /** Method returning the calo layers crossed 
   * by an IParticle track or itself if it is a TruthParticle
   * @param IParticle     reference to the Particle
   * @param extension     ptr to a CaloExtesion, will be updated if call is successfull 
   * @param cache         An alg using the same IParticle multiple times can pass a cache of
   * the form std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>> , where the key is the  
   * value of IParticle::index()
   * In this case it is the cache that has ownership of the pointers
   * @return true if the call was successful
   */  
 
  virtual bool caloExtension( const xAOD::IParticle& particle, Trk::CaloExtension* extension, 
                              std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>& cache ) const override final;
 
  /**
   * Method returning a unique_ptr to the caloExtension
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension( const TrackParameters& startPars, 
                                                             PropDirection propDir, ParticleHypothesis particleType ) const  override final;


  private:
    
    const xAOD::TrackParticle* getTrackParticle(const xAOD::IParticle& particle ) const;

    std::unique_ptr<Trk::CaloExtension>  caloExtension( const xAOD::TruthParticle& particle ) const;
    std::unique_ptr<Trk::CaloExtension> caloExtension( const xAOD::NeutralParticle& particle ) const;
    std::unique_ptr<Trk::CaloExtension>  caloExtension( const xAOD::TrackParticle& particle ) const;
    
    const AtlasDetectorID*       m_detID;
    ToolHandle< IExtrapolator >  m_extrapolator;
    ParticleHypothesis      m_particleType;
    std::string             m_particleTypeName;
    bool                    m_startFromPerigee;
  };
}

#endif
