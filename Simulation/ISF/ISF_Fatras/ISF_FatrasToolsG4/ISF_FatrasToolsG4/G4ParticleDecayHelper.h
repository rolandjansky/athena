/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// G4ParticleDecayHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLSG4_G4PARTICLEDECAYHELPER_H
#define ISF_FATRASTOOLSG4_G4PARTICLEDECAYHELPER_H

// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
// ISF
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticleVector.h"
// iFatras
#include "ISF_FatrasInterfaces/IParticleDecayHelper.h"
// STD
#include <vector>
#include <math.h>

class StoreGateSvc;
class G4RunManager;

namespace ISF {
    class ISFParticle;
    class IParticleBroker;
    class ITruthSvc;
    class IG4RunManagerHelper;
}

namespace iFatras {

   class ITrackCreator;
   class PDGToG4Particle;
   class IPhysicsValidationTool;

 /** @class G4ParticleDecayHelper

     This Tool is a helper tool wrapping the IParticleDecayCreator,
     and organised the flight distance to 
  
      @author Andreas.Salzburger@cern.ch, Joerg.Mechnich@cern.ch
   */

   class G4ParticleDecayHelper : public AthAlgTool, 
                                 virtual public IParticleDecayHelper {
    public:
      /**AlgTool constructor for ParticleDecayHelper*/
      G4ParticleDecayHelper(const std::string&,const std::string&,const IInterface*);
      /**Destructor*/
      ~G4ParticleDecayHelper();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** free path estimator (-1 for stable particle) */
      double freePath(const ISF::ISFParticle& isp) const;

      /** decay handling secondaries */
      void decay(const ISF::ISFParticle& isp,
		 const Amg::Vector3D& vertex,
		 const Amg::Vector3D& mom,
		 double timeStamp = 0) const;

      /** decay */
      std::vector<ISF::ISFParticle*> decayParticle(const ISF::ISFParticle& parent,
                                                 const Amg::Vector3D& vertex,
                                                 const Amg::Vector3D& mom,
                                                 double timeStamp = 0) const;


   private:
      /** initialize G4RunManager on first call if not done by then */
      bool initG4RunManager() const;
                                  
      /** fill decay products: into broker svc, truth svc */
      void handleDecayParticles(const ISF::ISFParticle& isp, 
                                const ISF::ISFParticleVector& children) const;                             

      /*------------------------------------------------------------
       *  ServiceHandles
       *------------------------------------------------------------*/
      ServiceHandle<ISF::IParticleBroker> m_particleBroker;        //!< Broker Svc for ISF particles
      ServiceHandle<ISF::ITruthSvc>       m_truthRecordSvc;        //!< Truth Svc for truth tree


      ServiceHandle<IAtRndmGenSvc>        m_rndmSvc;               //!< Random Svc
      CLHEP::HepRandomEngine*             m_randomEngine;          //!< Random engine (updated to streams)
      std::string                         m_randomEngineName;      //!< Name of the random number stream
      std::string                         m_G4RandomEngineName;    //!< Name of the random number stream for G4 tools

      /*------------------------------------------------------------
       *  ToolHandles
       *------------------------------------------------------------*/
      ToolHandle<ISF::IG4RunManagerHelper> m_g4RunManagerHelper;   //!< G4RunManager needs to be initialized before G4 tables are accessed
      ToolHandle<PDGToG4Particle>         m_pdgToG4Conv;           //!< Handle for the  PDGToG4Particle converter tool

      mutable G4RunManager*                m_g4runManager;         //!< for dummy G4 initialization 

      /** Validation output with histogram service */
      bool                                                         m_validationMode; //!< turn validation mode on/off
      ToolHandle<IPhysicsValidationTool>                           m_validationTool;   //!< the ntuple
      
      /*------------------------------------------------------------
       *  Statistics counter
       *------------------------------------------------------------*/          
      mutable size_t                       m_nProcessedParticles;
      mutable size_t                       m_nStableParticles;
      mutable size_t                       m_nExitedParticles;
      mutable size_t                       m_nDecayedParticles;
      mutable size_t                       m_nChargedParticles;
      mutable size_t                       m_nNeutralParticles;
                    
      /*---------------------------------------------------------------------
       *  Private static members
       *---------------------------------------------------------------------*/
      /** the speed of light in SI units */
      static double s_speedOfLightSI;      
                    
   };               
                    
}                   

#endif
