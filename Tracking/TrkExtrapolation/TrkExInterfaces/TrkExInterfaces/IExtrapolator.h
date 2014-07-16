/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IExtrapolator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IEXTRAPOLATOR_H
#define TRKEXINTERFACES_IEXTRAPOLATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkExUtils/ExtrapolationCache.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParametersBase/ParametersBase.h"
// xAOD
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/NeutralParticle.h"
// STL
#include <utility>

 
namespace Trk {
  
  static const InterfaceID IID_IExtrapolator("IExtrapolator", 1, 0);
  
  /** 
   @class IExtrapolator
   Interface class for the extrapolation AlgTool, it inherits from IAlgTool
   Detailed information about private members and member functions can be found in the actual implementation class
   Extrapolator which inherits from this one.

   The ParametersBase and NeutralParameters methods only exist in configured mode,
   the strategy pattern is not really useful for straight line propagation.
       
   @author Andreas.Salzburger@cern.ch
   */
      
  class IPropagator;
  class INavigator;
  class IMaterialUpdater;
  class Track;
  class Surface;
  class TrackingVolume;
  class TrackingGeometry;
  class TransportJacobian;
  class TrackStateOnSurface;      
  class Layer;
  class Volume;

  class IExtrapolator : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IExtrapolator(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IExtrapolator; }

       /** [xAOD interface ] */
       
       /** xAOD 0) neutral xAOD particle */
       virtual const NeutralParameters* extrapolate(const xAOD::NeutralParticle& xnParticle,
                                                    const Surface& sf,
                                                    PropDirection dir=anyDirection,
                                                    BoundaryCheck bcheck = true) const = 0;



       /** xAOD 0) neutral xAOD particle */
        virtual const TrackParameters* extrapolate(const xAOD::TrackParticle& particleBase,
                                                   const Surface& sf,
                                                   PropDirection dir=anyDirection,
                                                   BoundaryCheck bcheck = true,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const = 0;


       /** [NeutralParameters] ------------------------------------------------------------- */

       /** N 0) <b>Neutral parameters method </b> 
           - returns a ParametersBase object as well, 0 if the extrapolation did not succeed 
         */

       virtual const NeutralParameters* extrapolate(const NeutralParameters& parameters,
                                                    const Surface& sf,
                                                    PropDirection dir=anyDirection,
                                                    BoundaryCheck bcheck = true) const = 0;

       /** [TrackParameters] --------------------------------------------------------------- */

              
       /** S 1) <b>Strategy Pattern extrapolation method</b>
        - returns the TrackParameters at the Destination Surface (if extrapolation succeeds),
          0 if extrapolation to destination surface does not suceed */
       virtual const TrackParameters*  extrapolate(const IPropagator& prop,
                                                   const TrackParameters& parm,
                                                   const Surface& sf,
                                                   PropDirection dir=anyDirection,
                                                   BoundaryCheck bcheck = true,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const = 0;

       /** S 2) <b>Strategy Pattern extrapolation method</b>
        - returns a vector of TrackParameters representing the tracking detector elements
          hit in between and the TrackParameters at the destination Surface (if final extrapolation suceeds),
          0 if the extrapolation to the destination surface does not suceed*/
       virtual const std::vector<const TrackParameters*>*  extrapolateStepwise(
                                                             const IPropagator& prop,
                                                             const TrackParameters& parm,
                                                             const Surface& sf,
                                                             PropDirection dir=anyDirection,
                                                             BoundaryCheck bcheck = true,
                                                             ParticleHypothesis particle=pion) const = 0;
                                                                                                                                         
       /** S 3) <b>Strategy Pattern extrapolation method</b>: 
        - searches the closest TrackParameters of the Track to the destination Surface
        - returns the TrackParameters at the Destination Surface (if extrapolation succeeds),
          0 if extrapolation to destination surface does not suceed */
       virtual const TrackParameters* extrapolate(const IPropagator& prop,
                                                  const Track& trk,
                                                  const Surface& sf,
                                                  PropDirection dir=anyDirection,
                                                  BoundaryCheck bcheck = true,
                                                  ParticleHypothesis particle=pion,
                                                  MaterialUpdateMode matupmode=addNoise) const = 0;
       
                                                                                                        
          /** S 4) <b>Strategy Pattern extrapolation method</b>:
         - direct extrapolation to the destination surface, no material effects 
           or intermediate steps are taken into account
        */                   
          virtual const TrackParameters* extrapolateDirectly(const IPropagator& prop,
                                                             const TrackParameters& parm,
                                                             const Surface& sf,
                                                             PropDirection dir=anyDirection,
                                                             BoundaryCheck bcheck = true,
                                                             ParticleHypothesis particle=pion) const = 0;
                                                   
        /** S 5) <b>Strategy Pattern extrapolation method</b>:
         - blind inside the given tracking Volume (boundaryVol), 
           if non is given the reference surface for destination is used
        */          
        virtual const std::vector<const TrackParameters*>* extrapolateBlindly(const IPropagator& prop,
                                                                              const TrackParameters& parm,
                                                                              PropDirection dir=anyDirection,
                                                                              BoundaryCheck bcheck = true,
                                                                              ParticleHypothesis particle=pion,
                                                                              const Volume* boundaryVol=0) const = 0;

        /** S 6) <b>Strategy Pattern extrapolation method</b>:
         - extrapolation to the next active layer, based on the extrapolation to the next layer
           and layer identification
        */          
        virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayer(
                                                                                       const IPropagator& prop,
											                                           const TrackParameters& parm,
											                                           PropDirection dir,
											                                           BoundaryCheck bcheck,
											                                           ParticleHypothesis particle=pion,
											                                           MaterialUpdateMode matupmode=addNoise) const=0;

        virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayerM(
                                                                                       const IPropagator& prop,
											                                           const TrackParameters& parm,
											                                           PropDirection dir,
											                                           BoundaryCheck bcheck,
											                                           std::vector<const Trk::TrackStateOnSurface*>& material,
											                                           ParticleHypothesis particle=pion,
											                                           MaterialUpdateMode matupmode=addNoise) const=0;
              
        /** S 8) <b>Strategy Pattern extrapolation method</b>:
         - Extrapolation using specific intermediate surfaces and energy loss effects to be accounted for at
         each surface as specified by the corresponding MaterialEffectsOnTrack. The last propagation ends
         at the last surface given, applying the corresponding MaterialEffectsOnTrack to the track parameters
         before returning.
        */
        virtual const TrackParameters* extrapolate(const IPropagator& prop,
                                                   const TrackParameters& parm,
                                                   const std::vector< MaterialEffectsOnTrack >& sfMeff,
                                                   const Trk::TrackingVolume& tvol,
                                                   PropDirection dir,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const = 0;

        /** S 9) <b>Strategy Pattern extrapolation method</b>:
         - extrapolation to the next active layer, based on the extrapolation to the next layer
           and layer identification
        */          
        virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextStation(const IPropagator& prop,
											                                            const TrackParameters& parm,
											                                            PropDirection dir,
											                                            BoundaryCheck bcheck,
											                                            ParticleHypothesis particle=pion,
											                                            MaterialUpdateMode matupmode=addNoise) const=0;

        /** S 10) <b>Strategy Pattern extrapolation method</b>:
         - extrapolation to a volume boundary of an arbitrary tracking volume ( not necessarily part of a tracking geometry ) 
        */          
        virtual const TrackParameters* extrapolateToVolume(const IPropagator& prop,
                                                           const TrackParameters& parm,
                                                           const Trk::TrackingVolume& vol,
                                                           PropDirection dir=anyDirection,
                                                           ParticleHypothesis particle=pion) const=0;
                            
        /** C 1) <b>Configured AlgTool extrapolation method</b> of S 1):*/
        virtual const TrackParameters* extrapolate(const TrackParameters& parm,
                                                   const Surface& sf,
                                                   PropDirection dir=anyDirection,
                                                   BoundaryCheck bcheck = true,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise,
                                                   Trk::ExtrapolationCache* cache = 0 ) const = 0;
        
        /** C 2) <b>Configured AlgTool extrapolation method</b> of S 2):*/
        virtual const std::vector<const TrackParameters*>*  extrapolateStepwise(const TrackParameters& parm,
                                                                                const Surface& sf,
                                                                                PropDirection dir=anyDirection,
                                                                                BoundaryCheck bcheck = true,
                                                                                ParticleHypothesis particle=pion ) const = 0;
                                                   
        /** C 3) <b>Configured AlgTool extrapolation method</b> of S 3):*/
        virtual const TrackParameters* extrapolate(const Track& trk,
                                                   const Surface& sf,
                                                   PropDirection dir=anyDirection,
                                                   BoundaryCheck bcheck = true,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise,
                                                   Trk::ExtrapolationCache* cache = 0 ) const = 0;
                                                   
        /** C 4) <b>Configured AlgTool extrapolation method</b> of S 4):*/
        virtual const TrackParameters* extrapolateDirectly(const TrackParameters& parm,
                                                           const Surface& sf,
                                                           PropDirection dir=anyDirection,
                                                           BoundaryCheck bcheck = true,
                                                           ParticleHypothesis particle=pion) const = 0;
                                                           
        /** C 5) <b>Configured AlgTool extrapolation method</b> of S 5):*/                        
        virtual const std::vector<const TrackParameters*>*  extrapolateBlindly(const TrackParameters& parm,
                                                                               PropDirection dir=anyDirection,
                                                                               BoundaryCheck bcheck = true,
                                                                               ParticleHypothesis particle=pion,
                                                                               const Volume* boundaryVol=0) const = 0;

        /** C 6) <b>Configured AlgTool extrapolation method</b> of S 6):*/                        
        virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayer(
                                                                 const TrackParameters& parm,
				                                                      	 PropDirection dir=anyDirection,
											                                           BoundaryCheck bcheck = true,
											                                           ParticleHypothesis particle=pion,
											                                           MaterialUpdateMode matupmode=addNoise) const=0;
        
        /** C 7) <b>Configured AlgTool extrapolation method</b> of S 7):*/                        	
        virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayerM(
                                                                 const TrackParameters& parm,
											                                           PropDirection dir,
											                                           BoundaryCheck bcheck,
											                                           std::vector<const Trk::TrackStateOnSurface*>& material,
											                                           ParticleHypothesis particle=pion,
											                                           MaterialUpdateMode matupmode=addNoise) const=0;

        /** C 8) <b>Configured AlgTool extrapolation method</b> of S 8):*/                        
        virtual std::pair<const TrackParameters*,const Layer*> extrapolateToNextStation(
                                                                 const TrackParameters& parm,
											                     PropDirection dir=anyDirection,
											                     BoundaryCheck bcheck = true,
											                     ParticleHypothesis particle=pion,
											                     MaterialUpdateMode matupmode=addNoise) const=0;

        /** C 9) <b>Configured AlgTool extrapolation method</b> of S 9):*/
        virtual const TrackParameters* extrapolateToVolume(const TrackParameters& parm,
                                                           const Trk::TrackingVolume& vol,
                                                           PropDirection dir=anyDirection,
                                                           ParticleHypothesis particle=pion) const=0;
          
        /** C 10) <b>Configured AlgTool extrapolation method</b>:
         - Extrapolate to a destination surface, while collecting all the material layers in between. 
        */                                                                                           
        virtual const std::vector<const TrackStateOnSurface*>* extrapolateM(const TrackParameters& parameters,
                                                                            const Surface& sf,
                                                                            PropDirection dir,
                                                                            BoundaryCheck bcheck,
                                                                            ParticleHypothesis particle=pion, 
                                                                            Trk::ExtrapolationCache* cache = 0) const = 0;

        /** C 11) <b>Configured AlgTool extrapolation method</b>:
         - Extrapolate to a destination surface, while collecting all the material layers and transport jacobians in between.
        */
        virtual const std::vector<const TrackParameters*>* extrapolateM(const TrackParameters& parameters,
                                                                        const Surface& sf,
                                                                        PropDirection dir,
                                                                        BoundaryCheck bcheck,
                                                                        std::vector<MaterialEffectsOnTrack> &material,
                                                                        std::vector<Trk::TransportJacobian*> &jacs,
                                                                        ParticleHypothesis particle=pion,
                                                                        Trk::ExtrapolationCache* cache = 0) const = 0;


	virtual const Trk::TrackParameters*  extrapolateWithPathLimit(
							      const Trk::TrackParameters& parm,
							      double& pathLim,
							      Trk::PropDirection dir,
							      Trk::ParticleHypothesis particle,
							      std::vector<const Trk::TrackParameters*>*&  parmOnSf,
							      std::vector<const Trk::TrackStateOnSurface*>*&  material,
							      const Trk::TrackingVolume* boundaryVol=0,
							      MaterialUpdateMode matupmod = Trk::addNoise) const = 0;

        /** extrapolation method collecting intersections with subdetector boundaries and active volumes/layers.
            A primitive identification is provided - to be replaced with appropriate identifier, and possibly merged
            with TrackParameters. Material collection in option. Destination (subdetector boundary) : geoID (exit)
        */
	virtual const std::vector< std::pair< const Trk::TrackParameters*, int > >*  extrapolate(
							      const Trk::TrackParameters& parm,
							      Trk::PropDirection dir,
							      Trk::ParticleHypothesis particle,
							      std::vector<const Trk::TrackStateOnSurface*>*&  material,
							      int destination = 3) const = 0;
 
        /** Return the TrackingGeometry used by the Extrapolator (forwards information from Navigator) */
        virtual const TrackingGeometry* trackingGeometry() const = 0;

        /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps
            */
        virtual void validationAction() const = 0;

        /** Access the subPropagator to the given volume*/
        virtual const IPropagator* subPropagator(const TrackingVolume& tvol) const = 0;
 
  };

  inline const Trk::TrackParameters*  Trk::IExtrapolator::extrapolateWithPathLimit(
										   const Trk::TrackParameters& /*parm*/,
										   double& /*pathLim*/,
										   Trk::PropDirection /*dir*/,
										   Trk::ParticleHypothesis /*particle*/,
										   std::vector<const Trk::TrackParameters*>*&  /*parmOnSf*/,
										   std::vector<const Trk::TrackStateOnSurface*>*&  /*material*/,
										   const Trk::TrackingVolume* /*boundaryVol*/,
                                                                                   MaterialUpdateMode /*matupmod*/) const 
    {
      std::cout << "CONFIGURATION ERROR: dummy version of extrapolation-with-path-limit method called " << std::endl;
      return 0;
    }

  inline const std::vector< std::pair< const Trk::TrackParameters*, int > >*  
    Trk::IExtrapolator::extrapolate(  const Trk::TrackParameters& /*parm*/,
				      Trk::PropDirection /*dir*/,
				      Trk::ParticleHypothesis /*particle*/,
				      std::vector<const Trk::TrackStateOnSurface*>*&  /*material*/,
				      int /*destination*/) const 
    {
      std::cout << "CONFIGURATION ERROR: dummy version of extrapolation through active regions called " << std::endl;
      return 0;
    }


} // end of namespace

#endif // TRKEXINTERFACES_ITRKEXTRAPOLATOR_H

