/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_TRANSPORTENGINE_H
#define ISF_FATRASTOOLS_TRANSPORTENGINE_H

// Athena Base
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "TrkExInterfaces/IExtrapolationEngine.h"
#include "TrkExUtils/ExtrapolationCell.h"
// ISF
#include "ISF_Interfaces/IParticleProcessor.h"

// Tracking
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkDetDescrUtils/GeometrySignature.h"

class IIncidentSvc;

namespace Trk {
  class HitInfo;
}

namespace ISF {
  class ISFParticle;
  class IParticleFilter;
}

namespace iFatras 
{
  class ISimHitCreator;    
  class IParticleDecayHelper;
  class IProcessSamplingTool;
  class IPhysicsValidationTool;
   
  /** @class TransportEngine 
      
      NEW Fatras AlgTool to create a ISFParticle at a volume entry/exit
      - Transport tool for new Extrapolation Engine
      
      @author Noemi Calace -at- cern.ch, Andreas Salzburger -at cern.ch 
  */  
  
  class TransportEngine : virtual public ISF::IParticleProcessor,
    public AthAlgTool
    {
    public:
      /** Constructor */
      TransportEngine(const std::string&,const std::string&,const IInterface*);
      
      /** Destructor */
      virtual ~TransportEngine ();
      
      /** AlgTool initialize method */
      virtual StatusCode initialize();
      
      /** AlgTool finalize method */
      virtual StatusCode finalize();
      
      /** Creates a new ParticleState from a given ParticleState, universal transport tool */
      ISF::ISFParticle* process(const ISF::ISFParticle& isp );

    private:
      
      /** handle the return of the extrapolation */
      ISF::ISFParticle* handleExtrapolationResult(const ISF::ISFParticle& isp,
                                                  Trk::ExtrapolationCode eCode,
                                                  const Amg::Vector3D& position,
                                                  const Amg::Vector3D& momentum,
						  double stime,
						  Trk::GeometrySignature geoID);
      
      /** templated Tool retrieval - gives unique handling & look and feel */
      template <class T> StatusCode retrieveTool(ToolHandle<T>& thandle)
      {
        if (!thandle.empty() && thandle.retrieve().isFailure()){
            ATH_MSG_FATAL( "[ fatras setup ] Cannot retrieve " << thandle << ". Abort.");
            return StatusCode::FAILURE;
        } else
            ATH_MSG_DEBUG("[ fatras setup ] Successfully retrieved " << thandle);
        return StatusCode::SUCCESS;
      }
      
      
      /** prepare the ExtrapolationCell */ 
      template <class CELL> void configureExtrapolationCell(CELL& cell,
                                                            Trk::ParticleHypothesis pHypo,
                                                            double pLimit,
                                                            double x0Limit,
                                                            double l0Limit);
      
      /*---------------------------------------------------------------------
       *  Private members
       *---------------------------------------------------------------------*/
      
      /** Random Generator service  */
      ServiceHandle<IAtRndmGenSvc>                 m_rndGenSvc;
      CLHEP::HepRandomEngine*                      m_randomEngine;
      std::string                                  m_randomEngineName;         //!< Name of the random number stream

      /** Particle Decay */
      ToolHandle<IParticleDecayHelper>             m_particleDecayHelper; 
      
      /** The SimHit Creation */
      ToolHandle<ISimHitCreator>                   m_simHitCreatorID;
      
      /** The Extrapolator setup */
      ToolHandle<Trk::IExtrapolationEngine>        m_extrapolationEngine;     
      
      /** Filtering setup & other ISF stuff*/
      ToolHandle<ISF::IParticleFilter>             m_trackFilter;
      ToolHandle<ISF::IParticleFilter>             m_neutralHadronFilter;
      ToolHandle<ISF::IParticleFilter>             m_photonFilter;
      ToolHandle<IProcessSamplingTool>             m_samplingTool;
                                                
      Trk::PdgToParticleHypothesis                 m_pdgToParticleHypothesis;
      Trk::ParticleMasses                          m_particleMasses;      //!< Struct of Particle masses

      bool                                         m_validationMode;
      ToolHandle<IPhysicsValidationTool>           m_validationTool;
                                                                                                
    }; 
    
    
  template <class CELL> void TransportEngine::configureExtrapolationCell(CELL& ecc, 
                                                                         Trk::ParticleHypothesis pHypo,
                                                                         double pLimit,
                                                                         double x0Limit,
                                                                         double l0Limit)
    {
        // if it reaches the boundary - it has to stop there
        ecc.addConfigurationMode(Trk::ExtrapolationMode::StopAtBoundary);
        // set the particle hypothesis
        ecc.setParticleHypothesis(pHypo);
        // set the path limit - if needed
        if (pLimit > 0){
            // for decaying particles, sets the pathLimit how long they can freely travel before decay
            ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithPathLimit);
            ecc.pathLimit = pLimit;
        }
        // set the X0 material limit - if needed
        if (x0Limit > 0){
            // for neutrals this only applies to photons, set's the material limit until a conversion happens
            ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitL0);
            ecc.materialLimitX0 = x0Limit;
        }
        // set the L0 material limit - if needed
        if (l0Limit > 0){
            // for hadrons, sets the material limit until a hadronic interaction happens
            ecc.addConfigurationMode(Trk::ExtrapolationMode::StopWithMaterialLimitL0);
            ecc.materialLimitL0 = l0Limit;
        }
    }
    
}

#endif // ISF_FATRASTOOLS_TRANSPORTENGINE_H
