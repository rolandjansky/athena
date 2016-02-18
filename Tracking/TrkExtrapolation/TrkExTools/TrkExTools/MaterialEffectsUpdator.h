/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialEffectsUpdator.h, c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_MATERIALEFFECTSUPDATOR_H
#define TRKEXTOOLS_MATERIALEFFECTSUPDATOR_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExUtils/MaterialUpdateMode.h"

#define TRKEXTOOLS_MAXUPDATES 100

#ifndef COVARIANCEUPDATEWITHCHECK
#define COVARIANCEUPDATEWITHCHECK(cov, sign, value) cov += ( sign > 0 ? value : ( value > cov ? 0 : sign*value ) )
#endif

class TTree;

namespace Trk {

  class Layer;
  //class TrackParameters;
  class MaterialProperties;
  class IEnergyLossUpdator;
  class IMultipleScatteringUpdator;
  class IMaterialMapper;
  
  /** @class MaterialEffectsUpdator
    
    Point-like (also called surface-based) u
    pdate of TrackParameters and associated errors.

    It extends the TrkParametersManipulator, for applying modifications on 
    the track parameters:

        - if the update is done on a pointer to a TrackParameters object,
          the object itself is modified

        - if the update is done on a reference to a TrackParameters object,
          the object is cloned and the the modification is done on the clone


    @author Andreas.Salzburger@cern.ch
    
    */
  class MaterialEffectsUpdator : public AthAlgTool,
                                 //public TrkParametersManipulator,
                                 virtual public IMaterialEffectsUpdator {
    public:
           
      /** AlgTool like constructor */
      MaterialEffectsUpdator(const std::string&,const std::string&,const IInterface*);

      /**Virtual destructor*/
      virtual ~MaterialEffectsUpdator();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
                                          
      /** Updator interface (full update for a layer)
          ---> ALWAYS the same pointer is returned
          the pointer to the same TrackParameters object is returned,
          it is manipulated (if sf.fullUpdateMaterial())
        */ 
      const TrackParameters*  update( const TrackParameters* parm,
                                      const Layer& sf,
                                      PropDirection dir=alongMomentum,
                                      ParticleHypothesis particle=pion,
                                      MaterialUpdateMode matupmode=addNoise) const;
      
      /** Updator interface (full update for a layer) according to user
	  input through MaterialEffectsOnTrack
          ---> ALWAYS the same pointer is returned
          the pointer to the same TrackParameters object is returned,
          it is manipulated
        */ 
      const TrackParameters*  update( const TrackParameters* parm,
                                      const MaterialEffectsOnTrack& meff,
                                      Trk::ParticleHypothesis particle=pion,
                                      MaterialUpdateMode matupmode=addNoise) const;

      /** Updator interface (pre-update for a layer):
          ---> ALWAYS the same pointer is returned 
          the pointer to the same TrackParametes object is returned,
          it is manipulated (if.preUpdateMaterial())
        */ 
      const TrackParameters*   preUpdate( const TrackParameters* parm,
                                          const Layer& sf,
                                          PropDirection dir=alongMomentum,
                                          ParticleHypothesis particle=pion,
                                          MaterialUpdateMode matupmode=addNoise) const;
       
      /** Updator interface (post-update for a layer): 
          ---> ALWAYS pointer to new TrackParameters are returned
          if no postUpdate is to be done : return 0
        */ 
      const TrackParameters*   postUpdate(const TrackParameters& parm,
                                          const Layer& sf,
                                          PropDirection dir=alongMomentum,
                                          ParticleHypothesis particle=pion,
                                          MaterialUpdateMode matupmode=addNoise) const;
                                                                             

       /** Dedicated Updator interface:
           -> pass through the same track parameters
         */
       const TrackParameters*      update( const TrackParameters* parm,
                                           const MaterialProperties& mprop,
                                           double pathcorrection,
                                           PropDirection dir=alongMomentum,
                                           ParticleHypothesis particle=pion,
                                           MaterialUpdateMode matupmode=addNoise) const;

       /** Dedicated Updator interface:
           -> create new track parameters
         */
       const TrackParameters*      update( const TrackParameters& parm,
                                           const MaterialProperties& mprop,
                                           double pathcorrection,
                                           PropDirection dir=alongMomentum,
                                           ParticleHypothesis particle=pion,
                                           MaterialUpdateMode matupmode=addNoise) const;


      /** Validation Action - calls the writing and resetting of the TTree variables */
      void validationAction() const;

      /** Only has an effect if m_landauMode == true.
	  Resets mutable variables used for non-local calculation of energy loss if
	  parm == 0. Otherwise, modifies parm with the final update of the covariance matrix*/
      void modelAction(const TrackParameters* parm = 0) const;

                  
   protected:
      /** A simple check method for the 'removeNoise' update model */
      bool checkCovariance(AmgSymMatrix(5)& updated) const;
    
      const TrackParameters* finalLandauCovarianceUpdate(const TrackParameters* parm) const;

      ToolHandle< IEnergyLossUpdator >  m_eLossUpdator;     //!< AlgoTool for EnergyLoss updates
      ToolHandle< IMultipleScatteringUpdator > m_msUpdator; //!< AlgoTool for MultipleScatterin effects
      // the material mapper for the validation process
      ToolHandle< IMaterialMapper > m_materialMapper;            //!< the material mapper for recording the layer material 

      bool                         m_doCompoundLayerCheck;  //!< turn on/off the necessary checks when we may have compound layers
      bool                         m_doEloss;               //!< steer energy loss On/Off from outside
      bool                         m_doMs;                  //!< steer multiple scattering On/Off from outside
      
      double                       m_momentumCut;           //!< Minimal momentum cut for update
      double                       m_momentumMax;           //!< Maximal momentum cut for update
      bool                         m_forceMomentum;         //!< Force the momentum to be a specific value
      double                       m_forcedMomentum;        //!< Forced momentum value
      bool                         m_xKalmanStraggling;     //!< the momentum Error as calculated in xKalman
      bool                         m_useMostProbableEloss;  //!< use the most probable energy loss

      bool                         m_msgOutputValidationDirection; //!< validation direction used for screen output
      bool                         m_msgOutputCorrections;          //!< screen output of actual corrections


      // ------------ validation variables -------------------------------------------------
      bool                         m_validationMode;                     //!< Switch for validation mode
      bool                         m_validationIgnoreUnmeasured;         //!< Ignore unmeasured TrackParameters (Navigation!)

      bool                         m_landauMode;                         //!< If in Landau mode, error propagation is done as for landaus
      int                          m_validationDirection;                //!< validation direction
      mutable const Trk::Layer*    m_validationLayer;                    //!< layer in the current validation setp
      mutable bool                 m_validationReferenceSurface;         //!< validation layer has used reference surface material      
      mutable int                  m_validationSteps;                    //!< number of validation steps
      mutable double               m_validationPhi;                      //!< theta
      mutable double               m_validationEta;                      //!< eta


      mutable double               m_accumulatedElossSigma;              //!< Sigma of the eloss accumulated so far in the extrapolation. Used in Landau mode

      // static particle mass switcher
      static ParticleMasses        s_particleMasses;        //!< struct of Particle masses   
        
  };


} // end of namespace


#endif // TRKEXTOOLS_MATERIALEFFECTSUPDATOR_H

