/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// McMaterialEffectsUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_TRACKONLAYERUPDATOR_H
#define ISF_FATRASTOOLS_TRACKONLAYERUPDATOR_H

// GaudiKernel & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Trk
#include "TrkExInterfaces/ITimedMatEffUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkDetDescrUtils/LayerIndexSampleMap.h"
#include "TrkDetDescrUtils/GeometrySignature.h" 
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"

// ISF
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticleVector.h"

// Barcode
#include "BarcodeEvent/PhysicsProcessCode.h"

#ifndef MAXHADINTCHILDREN
#define MAXHADINTCHILDREN 20
#endif

class StoreGateSvc;

class TTree;
  
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
  
  class IProcessSamplingTool;
  class IPhysicsValidationTool;
  class IHadronicInteractionProcessor;
  class IPhotonConversionTool; 
  class IParticleDecayHelper;
      
  /** @class McMaterialEffectsUpdator
  
      Updator for a track on a Trk::Layer, it extends the IMaterialEffecsUpdator to 
      be used inside the Extrapolator with an update based on a Random number.
      
      The McMaterialEffectsUpdator uses both an extended EnergyLossUpdator
      and the standard ATLAS MultipleScatteringUpdator configured for the Gaussian-Mixture-Model.

      @author Andreas.Salzburger@cern.ch, Carsten.Magass@cern.ch
   */
   
  class McMaterialEffectsUpdator : public extends<AthAlgTool, Trk::ITimedMatEffUpdator> {
    public:      
      /**AlgTool constructor for McMaterialEffectsUpdator*/
      McMaterialEffectsUpdator(const std::string&,const std::string&,const IInterface*);
      /**Destructor*/
      virtual ~McMaterialEffectsUpdator();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** Updator interface (full update for a layer): 
         given track parameters are deleted internally,
         if no update has to be done, 
         the pointer is returned
        */ 
      const Trk::TrackParameters*  update(const Trk::TrackParameters* parm,
					  const Trk::Layer& sf,
					  Trk::TimeLimit& time, Trk::PathLimit& path,
                                          Trk::GeometrySignature geoID,
					  Trk::PropDirection dir=Trk::alongMomentum,
					  Trk::ParticleHypothesis particle=Trk::pion) const;
      
      const Trk::TrackParameters*  update( double time,
					   const Trk::TrackParameters* parm,
                                      const Trk::MaterialEffectsOnTrack& meff,
                                      Trk::ParticleHypothesis particle=Trk::pion,
                                      Trk::MaterialUpdateMode matupmode=Trk::addNoise) const;
                                      
     
       /** Updator interface:
         The parmeters are given as a reference, 
         MaterialProperties based material update
         */
       const Trk::TrackParameters*   update( double time,
					   const Trk::TrackParameters& parm,
                                           const Trk::MaterialProperties& mprop,
                                           double pathcorrection,
                                           Trk::PropDirection dir=Trk::alongMomentum,
                                           Trk::ParticleHypothesis particle=Trk::pion,
                                           Trk::MaterialUpdateMode matupmode=Trk::addNoise) const;
    
       const Trk::TrackParameters*   interact(double time,
		       const Amg::Vector3D& position,
		       const Amg::Vector3D& momentum,
		       Trk::ParticleHypothesis particle,
		       int process,
		       const Trk::Material* extMatProp=0) const;


      /** the helper function for a brem photon record */
      void recordBremPhoton(double time,
	 		   double pElectron,
                           double gammaE,
                           const Amg::Vector3D& vertex,
                           Amg::Vector3D& particleDir,
                           Trk::ParticleHypothesis particle ) const;
                          
   private:
      const Trk::TrackParameters*  updateInLay(const ISF::ISFParticle* isp,
					       const Trk::TrackParameters* parm,
					       double& matFraction,
					       Trk::TimeLimit& time, Trk::PathLimit& path,
					       Trk::PropDirection dir=Trk::alongMomentum,
					       Trk::ParticleHypothesis particle=Trk::pion) const;

      ISF::ISFParticleVector  interactLay(const ISF::ISFParticle* isp,
						double time,
						const Trk::TrackParameters& parm,
						Trk::ParticleHypothesis particle,
						int process,
						const Trk::MaterialProperties* extMatProp=0) const;

      void  radiate(const ISF::ISFParticle* parent, AmgVector(5)& updatedParameters,
		    const Amg::Vector3D& pos, Amg::Vector3D& dir,
                    Trk::TimeLimit time, double dX0, double& matFraction, double matTot, 
		    Trk::PropDirection pdir,Trk::ParticleHypothesis particle) const; 

      void ionize(const Trk::TrackParameters& parm, AmgVector(5)& updatedPar,double dInX0,
		  Trk::PropDirection pdir,Trk::ParticleHypothesis particle ) const ;
   
      /** handle the Energy loss */
      //bool handleEnergyLoss(double time,
      //			  const Trk::TrackParameters& parm,
      //                      Amg::Vector3D*       updatedParameter,
      //                      Trk::EnergyLoss* sampldEnergyLoss, 
      //                      const Trk::Layer& lay,
      //                      double p, double E, double m,
      //                      Trk::ParticleHypothesis particle=Trk::pion) const;
      
     /** the private multiple scattering sigma calculation*/
     double msSigma(double dInX0,double p,Trk::ParticleHypothesis particle) const;
                                    
     /** the private multiple Scattering update method, thetaMs is the projected random number*/
     void multipleScatteringUpdate( const Trk::TrackParameters& parm,
                                   AmgVector(5)& parameters,
                                   double sigmaMSproj) const;
                                   
     /** the helper function for a brem photon record */
     void recordBremPhotonLay(const ISF::ISFParticle* parent,
			      Trk::TimeLimit time,
			      double pElectron,
			      double gammaE,
			      const Amg::Vector3D& vertex,
			      Amg::Vector3D& particleDir,
			      double matFraction,
			      Trk::PropDirection dir,
			      Trk::ParticleHypothesis particle ) const;
                          
     //!< retrieve or provide the layerIndexSampleMap */
     const Trk::LayerIndexSampleMap*  layerIndexSampleMap() const;
   
    
     //!< retrieve TrackingGeometry (almost callback ready!)
     StatusCode                                   updateTrackingGeometry() const;
     
     /** IEnergyLossUpdator */
     bool                                         m_eLoss;
     ToolHandle<Trk::IEnergyLossUpdator>          m_eLossUpdator;
     
     /** IMultipleScatteringUpdator */
     bool                                         m_ms;
     ToolHandle<Trk::IMultipleScatteringUpdator>  m_msUpdator;

      /** IPhotonConversionTool */
     ToolHandle<iFatras::IPhotonConversionTool>   m_conversionTool;

    /** MCTruth process code for TruthIncidents created by this tool */
     Barcode::PhysicsProcessCode                  m_processCode;

     /** MCTruth process sampling */
     ToolHandle<iFatras::IProcessSamplingTool>    m_samplingTool;

     /** pass geometry signature to offspring */
     mutable Trk::GeometrySignature               m_geomSignature;
      
     /** hadronic interaction setting */
     bool                                         m_hadInt;
     ToolHandle<IHadronicInteractionProcessor>    m_hadIntProcessor;
     
     /** Particle Decay */
     ToolHandle<IParticleDecayHelper>                 m_particleDecayer;
     
     /** Minimum momentum cut */
     double                                       m_minimumMomentum;

     /** Minimum momentum for brem photons */
     double                                       m_minimumBremPhotonMomentum;

     /** describe deflection parametric/do real deflection */
     bool                                         m_parametricScattering;

     /** switch between MSUpdator and local parametrization */
     bool                                         m_use_msUpdator;

     /** use the relativistic hertz dipole for brem photon radiation */
     bool                                         m_uniformHertzDipoleAngle;

     /** Switch to use reference material */
     bool                                         m_referenceMaterial;

     /** Switch to use bending correction */
     bool                                         m_bendingCorrection;

     /** Random Generator service  */
     ServiceHandle<IAtRndmGenSvc>                 m_rndGenSvc;
     /** Random engine  */
     CLHEP::HepRandomEngine*                      m_randomEngine;
     std::string                                  m_randomEngineName;                   //!< Name of the random number stream

     /** for statistics output */
     mutable int                                  m_recordedBremPhotons;

     /** for the calo energy recording */
     mutable int                                  m_currentSample;                      //!< the currentSample
     bool                                         m_recordEnergyDeposition;             //!< for deposition methods
     std::string                                  m_layerIndexCaloSampleMapName;        //!< name to record it
     mutable const Trk::LayerIndexSampleMap*      m_layerIndexCaloSampleMap;            //!< the map for the calo-layer index map
                  
     mutable const Trk::TrackingGeometry*         m_trackingGeometry;                   //!< the tracking geometry owned by the navigator
     ServiceHandle<Trk::ITrackingGeometrySvc>     m_trackingGeometrySvc;                //!< ToolHandle to the TrackingGeometrySvc
     std::string                                  m_trackingGeometryName;               //!< default name of the TrackingGeometry      
     
     /** projection factor for the non-parametric scattering */
     double                                      m_projectionFactor;

     /** struct of Particle Masses */
     Trk::ParticleMasses                         m_particleMasses;
  
     // ------------------------ Validation section ------------------------------------
     bool                          m_validationMode;
     ToolHandle<IPhysicsValidationTool>  m_validationTool;
     std::string                   m_validationTreeName;        //!< validation tree name - to be acessed by this from root
     std::string                   m_validationTreeDescription; //!< validation tree description - second argument in TTree
     std::string                   m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out

     TTree*                        m_validationTree;            //!< Root Validation Tree

     // ----------- ntuple branches
      mutable int                  m_layerIndex;                //!< ntuple variable : layer index of material effects update
      mutable float                m_tInX0;                     //!< nutple variable : t/X0
      mutable float                m_thetaMSproj;               //!< ntuple variable : projected ms 
      mutable float                m_thetaMSphi;                //!< ntuple variable : ms in phi
      mutable float                m_thetaMStheta;              //!< ntuple variable : ms in theta
      mutable float                m_deltaP;                    //!< nutple variable : energy loss
      mutable float                m_deltaPsigma;               //!< ntuple variable : stragling on energy loss


     bool                          m_bremValidation;
     std::string                   m_bremValidationTreeName;        //!< validation tree name - to be acessed by this from root
     std::string                   m_bremValidationTreeDescription; //!< validation tree description - second argument in TTree
     std::string                   m_bremValidationTreeFolder;      //!< stream/folder to for the TTree to be written out

     TTree*                        m_bremValidationTree;            //!< Root Validation Tree

     // ------ ntuple branches
     mutable float                 m_bremPointX;               //!< ntuple variable : brem point x coordinate
     mutable float                 m_bremPointY;               //!< ntuple variable : brem point y coordinate
     mutable float                 m_bremPointR;               //!< ntuple variable : brem point r distance
     mutable float                 m_bremPointZ;               //!< ntuple variable : brem point z coordinate
     mutable float                 m_bremMotherEnergy;         //!< ntuple variable : brem mother energy
     mutable float                 m_bremPhotonEnergy;         //!< ntuple variable : brem photon energy
     mutable float                 m_bremPhotonAngle;          //!< ntuple variable : brem photon angle	

     // --------------------------------------------------------------------------------

     bool                          m_edValidation;
     std::string                   m_edValidationTreeName;        //!< validation tree name - to be acessed by this from root
     std::string                   m_edValidationTreeDescription; //!< validation tree description - second argument in TTree
     std::string                   m_edValidationTreeFolder;      //!< stream/folder to for the TTree to be written out
     
     TTree*                        m_edValidationTree;           //!< Root Validation Tree

     // ------ ntuple branches
     mutable float                 m_edLayerIntersectX;          //!< ntuple variable : energy deposit x coordinate
     mutable float                 m_edLayerIntersectY;          //!< ntuple variable : energy deposit y coordinate
     mutable float                 m_edLayerIntersectZ;          //!< ntuple variable : energy deposit z coordinate
     mutable float                 m_edLayerIntersectR;          //!< ntuple variable : energy deposit r coordinate
     mutable float                 m_edLayerEnergyDeposit;       //!< ntuple variable : energy despoit - value
     mutable float                 m_edLayerSample;              //!< ntuple variable : layer sample 
                    
     /** useful for the angle calculation of the brem photon */ 
     double                        m_oneOverThree;

     ServiceHandle<ISF::IParticleBroker>     m_particleBroker;
     ServiceHandle<ISF::ITruthSvc>           m_truthRecordSvc;

     /** cache incoming particle */
     mutable const ISF::ISFParticle*         m_isp;

     /** cache layer properties */
     mutable const Trk::Layer*                             m_layer;
     mutable const Trk::MaterialProperties*                m_matProp;                                                                              
     mutable const Trk::MaterialProperties*                m_extMatProp;                                                                              

     Trk::PdgToParticleHypothesis        m_pdgToParticleHypothesis;

   };

}

#endif // ISF_FATRASTOOLS_TRACKONLAYERUPDATOR_H
