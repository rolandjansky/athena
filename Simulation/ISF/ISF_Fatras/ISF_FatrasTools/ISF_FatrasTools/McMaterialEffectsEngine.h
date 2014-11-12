/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// McMaterialEffectsEngine.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_MCMATERIALEFFECTENIGINE_H
#define ISF_FATRASTOOLS_MCMATERIALEFFECTENIGINE_H

// GaudiKernel & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Trk
#include "TrkExInterfaces/IMaterialEffectsEngine.h"
#include "TrkExUtils/ExtrapolationCell.h"
#include "TrkExUtils/MaterialInteraction.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExUtils/MaterialUpdateMode.h"

// ISF
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticleVector.h"

// Barcode
#include "BarcodeInterfaces/PhysicsProcessCode.h"

class StoreGateSvc;

namespace Trk {
  class EnergyLoss;
  class CylinderVolumeBounds;
  class IEnergyLossUpdator;
  class IMultipleScatteringUpdator;
  class TrackingGeometry;
  class ITrackingGeometrySvc;
}

namespace ISF {
    class IParticleBroker;
    class ISFParticle;
    class ITruthSvc;
}

namespace iFatras {
  
  /** @class McMaterialEffectsEngine
  
      Updator for a track on a Trk::Layer, it extends the IMaterialEffectsEngine to 
      be used inside the Extrapolator with an update based on a Random number.
      
      The McMaterialEffectsEngine uses both an extended EnergyLossUpdator
      and the standard ATLAS MultipleScatteringUpdator configured for the Gaussian-Mixture-Model.

      @author Andreas.Salzburger@cern.ch, Noemi.Calace@cern.ch
   */
   
  class McMaterialEffectsEngine : public AthAlgTool, virtual public Trk::IMaterialEffectsEngine { 
    public:      
      /**AlgTool constructor for McMaterialEffectsEngine*/
      McMaterialEffectsEngine(const std::string&,const std::string&,const IInterface*);
      /**Destructor*/
      virtual ~McMaterialEffectsEngine();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** Updator interface (full update for a layer): 
         given track parameters are deleted internally,
         if no update has to be done, 
         the pointer is returned
        */ 
      virtual Trk::ExtrapolationCode handleMaterial(Trk::ExCellCharged& ecCharged,
						    Trk::PropDirection dir=Trk::alongMomentum,
						    Trk::MaterialUpdateStage matupstage=Trk::fullUpdate) const;

      virtual Trk::ExtrapolationCode handleMaterial(Trk::ExCellNeutral&,
						    Trk::PropDirection,
						    Trk::MaterialUpdateStage) const {return Trk::ExtrapolationCode::InProgress; }
      
   private:
      
      const Trk::TrackParameters* updateTrackParameters(const Trk::TrackParameters& parameters,
							Trk::ExCellCharged& eCell,
							Trk::PropDirection dir,
							Trk::MaterialUpdateStage matupstage) const;
      
      double simulate_theta_space(double beta, double p,double dOverX0,double Z, double scale) const;
      double * get_gaussian(double beta, double p,double dOverX0, double scale) const;
      double * get_gaussmix(double beta, double p,double dOverX0,double Z, double scale) const;
      double * get_semigauss(double beta,double p,double dOverX0,double Z, double scale) const;
      double sim_gaussmix(double * scattering_params) const;
      double sim_semigauss(double * scattering_params) const;

      void ionize(const Trk::TrackParameters& parm, AmgVector(5)& updatedPar, double dInX0,
		  Trk::PropDirection pdir, Trk::ParticleHypothesis particle ) const ;
      
      /** the private multiple scattering sigma calculation*/
      double msSigma(double dInX0, double p,Trk::ParticleHypothesis particle) const;
      
      /** the private multiple Scattering update method, thetaMs is the projected random number*/
      void multipleScatteringUpdate( const Trk::TrackParameters& parm,
				     AmgVector(5)& parameters,
				     double sigmaMSproj, double pathCorrection) const;
                 
      /** IEnergyLossUpdator */
      bool                                         m_eLoss;
      ToolHandle<Trk::IEnergyLossUpdator>          m_eLossUpdator;
      
      /** IMultipleScatteringUpdator */
      bool                                         m_ms;
      ToolHandle<Trk::IMultipleScatteringUpdator>  m_msUpdator;
      
      /** Minimum momentum cut */
      double                                       m_minimumMomentum;
      
      /** switch between MSUpdator and local parametrization */
      bool                                         m_use_msUpdator;
      
      /** Switch to use reference material */
      bool                                         m_referenceMaterial;
      
      /** Switch to use bending correction */
      bool                                         m_bendingCorrection;

      /** Random Generator service  */
      ServiceHandle<IAtRndmGenSvc>                 m_rndGenSvc;
      /** Random engine  */
      CLHEP::HepRandomEngine*                      m_randomEngine;
      std::string                                  m_randomEngineName;                   //!< Name of the random number stream
      
      mutable const Trk::TrackingGeometry*         m_trackingGeometry;                   //!< the tracking geometry owned by the navigator
      ServiceHandle<Trk::ITrackingGeometrySvc>     m_trackingGeometrySvc;                //!< ToolHandle to the TrackingGeometrySvc
      std::string                                  m_trackingGeometryName;               //!< default name of the TrackingGeometry      
      
      /** struct of Particle Masses */
      Trk::ParticleMasses                         m_particleMasses;
      
      ServiceHandle<ISF::IParticleBroker>     m_particleBroker;
      ServiceHandle<ISF::ITruthSvc>           m_truthRecordSvc;

      /** cache incoming particle */
      mutable const ISF::ISFParticle*         m_isp;

      /** cache layer properties */
      mutable const Trk::Layer*                             m_layer;
      mutable const Trk::MaterialProperties*                m_matProp;                                                                              
           
  };
  
}

#endif // ISF_FATRASTOOLS_MCMATERIALEFFECTENIGINE_H
