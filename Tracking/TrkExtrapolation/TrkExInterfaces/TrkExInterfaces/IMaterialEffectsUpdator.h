/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMaterialEffectsUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IMATERIALEFFECTSUPDATOR_H
#define TRKEXINTERFACES_IMATERIALEFFECTSUPDATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

  class Layer;
  class MaterialProperties;
  class MaterialEffectsOnTrack;  

  /** Interface ID for IMaterialEffectsUpdator*/  
  static const InterfaceID IID_IMaterialEffectsUpdator("IMaterialEffectsUpdator", 1, 0);
  
  /** @class IMaterialEffectsUpdator
     Interface class for the updater AlgTool, it inherits from IAlgTool
     Detailed information about private members and member functions can be found in the actual implementation class
     MaterialUpdator which inherits from this one.
    
     @author Andreas.Salzburger@cern.ch
    */
  class IMaterialEffectsUpdator : virtual public IAlgTool {
      public:

       /**Virtual destructor*/
       virtual ~IMaterialEffectsUpdator(){}
       
       /** AlgTool and IAlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IMaterialEffectsUpdator; }

       /** Updator interface (full update for a layer):
         The parmeters are given as a pointer, they are delete inside the update method.
         Layer-based material update
         */
       virtual const TrackParameters*      update( const TrackParameters* parm,
                                                   const Layer& sf,
                                                   PropDirection dir=alongMomentum,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const = 0;
      
       /** User updator interface (full update for a layer):
         The parmeters are given as a pointer, they are deleted inside the update method.
         Update occurs on the place where the parameters parm are according to the specified MaterialEffectsOnTrack
         */
       virtual const TrackParameters*      update( const TrackParameters* parm,
                                                   const MaterialEffectsOnTrack&,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const { (void)particle; 
                                                                                                  (void)matupmode;
                                                                                                   return parm; }

       /** Updator interface (pre-update for a layer):
         The parmeters are given as a pointer, they are delete inside the update method.
         Layer-based material update
         */
       virtual const TrackParameters*      preUpdate( const TrackParameters* parm,
                                                      const Layer& sf,
                                                      PropDirection dir=alongMomentum,
                                                      ParticleHypothesis particle=pion,
                                                      MaterialUpdateMode matupmode=addNoise) const = 0;
      
       /** Updator interface (pre-update for a layer):
         The parmeters are given as a pointer, they are delete inside the update method.
         Layer-based material update
         
         if the postUpdate fails, it returns 0
         */
       virtual const TrackParameters*      postUpdate(const TrackParameters& parm,
                                                      const Layer& sf,
                                                      PropDirection dir=alongMomentum,
                                                      ParticleHypothesis particle=pion,
                                                      MaterialUpdateMode matupmode=addNoise) const = 0;
                                                      
       /** Updator interface:
         The parmeters are given as a pointer, they are delete inside the update method.
         MaterialProperties based material update
         - used by all Layer-based methods
         */
       virtual const TrackParameters*      update( const TrackParameters& parm,
                                                   const MaterialProperties& mprop,
                                                   double pathcorrection,
                                                   PropDirection dir=alongMomentum,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const = 0;
      /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps */
       virtual void validationAction() const {}

      /** Model Action:
          Can be implemented optionally. Provides the possibility of
	  doing non-local MaterialEffectsUpdates for different models
      */
       virtual void modelAction(const TrackParameters* parm=0) const { if(parm) return; }
                                                                
  };


} // end of namespace


#endif // TRKEXINTERFACES_MATERIALEFFECTSUPDATOR_H


