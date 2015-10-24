/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleSmearer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMTOOLS_TRACKPARTICLESMEARER_H
#define ISF_PARSIMTOOLS_TRACKPARTICLESMEARER_H


// Fatras
#include "ISF_Interfaces/IParticleProcessor.h"
#include "ISF_ParSimInterfaces/IISPtoPerigeeTool.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
// xAOD
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "ISF_ParSimInterfaces/IChargedSmearer.h"
// CLHEP
//#include "CLHEP/Random/RandomEngine.h"
namespace ISF {
    class ISFParticle;
}

namespace Trk {
    class IExtrapolationEngine;
}

namespace iParSim {
      
  /** 
   @class TrackParticleSmearer

   Create xAOD::TrackParticles from ISFParticles,
   the IIncidentListener handles the TrackParticleCollection

   @author Andreas.Salzburger -at- cern.ch 
   */
      
  class TrackParticleSmearer : public AthAlgTool, 
        virtual public ISF::IParticleProcessor, virtual public IIncidentListener
  {
    public:

      /**Constructor */
      TrackParticleSmearer(const std::string&,const std::string&,const IInterface*);
      
      /**Destructor*/
      ~TrackParticleSmearer();
      
      /** AlgTool initailize method.*/
      StatusCode initialize();
      
      /** AlgTool finalize method */
      StatusCode finalize();
    
      /** handle for incident service */
      void handle(const Incident& inc); 
      
      /** Creates a new ISFParticle from a given ParticleState, universal transport tool */
      virtual ISF::ISFParticle* process(const ISF::ISFParticle& isp); 

    protected:
      
      /* Incident Service */  
      ServiceHandle<IIncidentSvc>                   m_incidentSvc; 
      
      std::string                                   m_tpContainerName;            //!< output collection name
      xAOD::TrackParticleContainer*                 m_trackParticleContainer;     //!< this is the output container      
      xAOD::TrackParticleAuxContainer*              m_trackParticleAuxContainer;  //!< this is the output container      

      ToolHandleArray<IChargedSmearer>              m_chargedSmearers;            //!< the Charged smearers to be retrieved
      std::map<unsigned int, const IChargedSmearer*>      m_chargedSmearerMap;    //!< reordered in a map for look-up
            
      ToolHandle<Trk::IExtrapolationEngine>         m_extrapolationEngine;        //!< the extrapolation engine to create the parameters 

      ToolHandle<IISPtoPerigeeTool>                 m_ISPtoPerigeeTool; 
     
      
  };

} // end of namespace

#endif 

