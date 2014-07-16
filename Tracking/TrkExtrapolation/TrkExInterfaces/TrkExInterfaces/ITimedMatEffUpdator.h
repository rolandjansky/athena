/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMaterialEffectsUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_ITIMEDMATEFFUPDATOR_H
#define TRKEXINTERFACES_ITIMEDMATEFFUPDATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "ITimedExtrapolator.h"

namespace Trk {

  class Layer;
  class Material;
  class MaterialProperties;
  class MaterialEffectsOnTrack;  

  /** Interface ID for ITimedMatEffUpdator*/  
  static const InterfaceID IID_ITimedMatEffUpdator("ITimedMatEffUpdator", 1, 0);
  
  /** @class ITimedMatEffUpdator
     Interface class for the updater AlgTool, it inherits from IAlgTool
     Detailed information about private members and member functions can be found in the actual implementation class
     MaterialUpdator which inherits from this one.
    
     @author Andreas.Salzburger@cern.ch, sarka.todorova@cern.ch
    */
  class ITimedMatEffUpdator : virtual public IAlgTool {
      public:

       /**Virtual destructor*/
       virtual ~ITimedMatEffUpdator(){}
       
       /** AlgTool and IAlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_ITimedMatEffUpdator; }

       /** Updator interface (full update for a layer):
         The parmeters are given as a pointer, they are delete inside the update method.
         Layer-based material update
         */
       virtual const TrackParameters*      update( const TrackParameters* parm,
                                                   const Layer& sf,
                                                   TimeLimit& time, PathLimit& path,
                                                   Trk::GeometrySignature geoID,
                                                   PropDirection dir=alongMomentum,
                                                   ParticleHypothesis particle=pion) const = 0;
      
       /** User updator interface (full update for a layer):
         The parmeters are given as a pointer, they are deleted inside the update method.
         Update occurs on the place where the parameters parm are according to the specified MaterialEffectsOnTrack
         */
       virtual const TrackParameters*      update( double /* time*/,
						                           const TrackParameters* parm,
                                                   const MaterialEffectsOnTrack&,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const { (void)particle; 
                                                                                                  (void)matupmode;
                                                                                                   return parm; }
                                                      
       /** Updator interface:
         The parmeters are given as a pointer, they are delete inside the update method.
         MaterialProperties based material update
         - used by all Layer-based methods
         */
       virtual const TrackParameters*      update( double time,
                                                   const TrackParameters& parm,
                                                   const MaterialProperties& mprop,
                                                   double pathcorrection,
                                                   PropDirection dir=alongMomentum,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const = 0;

       /** Updator interface:
         Perform pre-selected interaction.
         */
       virtual const TrackParameters*      interact(double time,
                                                    const  Amg::Vector3D& position,
                                                    const  Amg::Vector3D& momentum,
                                                    ParticleHypothesis particle,
                                                    int process,
                                                    const Trk::Material* extMatProp=0) const;

       /** the helper function for a brem photon record */
       virtual void recordBremPhoton(double ,
				     double ,      // emitting electron momentum
				     double ,         // brem photon momentum
				     const Amg::Vector3D& ,
				     Amg::Vector3D& ,
				     Trk::ParticleHypothesis ) const {}
                          

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

inline const Trk::TrackParameters*  Trk::ITimedMatEffUpdator::interact(double ,
                                                                       const Amg::Vector3D& ,
                                                                       const Amg::Vector3D& ,
                                                                       ParticleHypothesis ,
                                                                       int, 
                                                                       const Trk::Material*  ) const 
{
  return 0;
}


#endif // TRKEXINTERFACES_TIMEDMATEFFUPDATOR_H


