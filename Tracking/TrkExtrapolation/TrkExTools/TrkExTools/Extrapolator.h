/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Extrapolator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_EXTRAPOLATOR_H
#define TRKEXTOOLS_EXTRAPOLATOR_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Trk
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkExUtils/ExtrapolationCache.h"
// STL
#include <cstring>
#include <vector>
#include <map>
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
// xAOD
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/NeutralParticle.h"

class MsgStream;

namespace Trk {

  class Track;
  class Surface;
  class Layer;
  class Volume;                  
  class DetachedTrackingVolume;
  class TrackingGeometry;
  class TrackParticleBase;
  class IPropagator;
  class IDynamicLayerCreator;
  class INavigator;
  class IMultipleScatteringUpdator;
  class IEnergyLossUpdator;
  class IPropagator;
  class AlignableTrackingVolume;
  class ExtrapolationCache;
 
  typedef std::vector<const Trk::TrackParameters*> TrackParametersVector;

  typedef std::pair< const Surface*, BoundaryCheck > DestSurf;

  /** @struct ParametersAtBoundarySurface
      has only three member
      - BoundarySurface
      - TrackParameters
      - bool that indicated the deletion of the TrackParameters
   */
  struct ParametersNextVolume {
     //!< the members
     mutable const TrackingVolume*    nextVolume;
     mutable const TrackParameters*   nextParameters;
     mutable const TrackParameters*   navParameters;
     mutable BoundarySurfaceFace      exitFace;
     
     ParametersNextVolume(){
        nextVolume              = 0;
        nextParameters          = 0;
        navParameters           = 0;
        exitFace                = undefinedFace;
     }

     //!< update the boundaryInformation
     void boundaryInformation(const TrackingVolume* tvol,
                              const TrackParameters* nextPars,
                              const TrackParameters* navPars,
                              BoundarySurfaceFace    face=undefinedFace
                             ) const 
     {
        nextVolume       = tvol;
        nextParameters   = nextPars;
        navParameters    = navPars;
        exitFace         = face;
     }

    //!< reset the boundary information by invalidating it
    void resetBoundaryInformation(){
        nextVolume        = 0;
        nextParameters    = 0;
        navParameters     = 0;
        exitFace          = undefinedFace;
    }
     
  };
  
  /** 
    @class Extrapolator
      
    The Extrapolator can be used in different setups:
       
    - <b>Strategy-pattern</b> usage
       
    - <b>(Simple) Configured AlgTool</b> usage
       
    - <b>(Full) Configured AlgTool</b> usage

      
    This design allows external clients to choose their own propagation logic, 
    but also ensures an easy to use AlgTool for users
      
    The output level is as follows:
       INFO    : initialize / finalize information
       DEBUG   : Method call sequence
       VERBOSE : Method call sequence with values
       
    @author Andreas.Salzburger@cern.ch
   */
          
  class Extrapolator : public AthAlgTool,
                       virtual public IExtrapolator {
     public:
       /**Constructor */
       Extrapolator(const std::string&,const std::string&,const IInterface*);
       /**Destructor*/
       ~Extrapolator();
       
       /** AlgTool initailize method.
       In this method the extrapolator should retrieve the Propagator of highest order which is then passed through
       the extrapolate method. The Propagator itself should be specified whether to use propagators of a lower hirarchy
       level or not.
        */
       StatusCode initialize();
       /** AlgTool finalize method */
       StatusCode finalize();


       /** [xAOD] interface ------------------------------------------------------------------ */

       /** xAOD 0) neutral xAOD particle */
       const NeutralParameters* extrapolate(const xAOD::NeutralParticle& xnParticle,
                                            const Surface& sf,
                                            PropDirection dir=anyDirection,
                                            BoundaryCheck bcheck = true) const;



       /** xAOD 0) neutral xAOD particle */
       const TrackParameters* extrapolate(const xAOD::TrackParticle& particleBase,
                                          const Surface& sf,
                                          PropDirection dir=anyDirection,
                                          BoundaryCheck bcheck = true,
                                          ParticleHypothesis particle=pion,
                                          MaterialUpdateMode matupmode=addNoise) const;


       /** [TrackParticleBase] ---------------------------------------------------------------- */

       /** P 0) <b>Base class extrapolation method</b> 
           - returns a ParametersBase object as well, 0 if the extrapolation did not succeed 
         */
       const TrackParameters* extrapolate(const TrackParticleBase& particleBase,
                                          const Surface& sf,
                                          PropDirection dir=anyDirection,
                                          BoundaryCheck bcheck = true,
                                          ParticleHypothesis particle=pion,
                                          MaterialUpdateMode matupmode=addNoise) const;


      /** [NeutralParameters] ------------------------------------------------------------- */
	
      /** N 0) <b>Neutral parameters method </b>
          - returns a ParametersBase object as well, 0 if the extrapolation did not succeed
         */

       const NeutralParameters* extrapolate(const NeutralParameters& parameters,
                                            const Surface& sf,
                                            PropDirection dir=anyDirection,
                                            BoundaryCheck bcheck=true) const;
	
       /** [TrackParameters] --------------------------------------------------------------- */
         
       /** S 1) <b>Strategy Pattern extrapolation method</b>
        - returns the TrackParameters at the Destination Surface (if extrapolation succeeds),
          0 if extrapolation to destination surface does not suceed */
       const TrackParameters*  extrapolate(const IPropagator& prop,
                                           const TrackParameters& parm,
                                           const Surface& sf,
                                           PropDirection dir=anyDirection,
                                           BoundaryCheck bcheck = true,
                                           ParticleHypothesis particle=pion,
                                           MaterialUpdateMode matupmode=addNoise) const;

       /** S 2) <b>Strategy Pattern extrapolation method</b>
        - returns a vector of TrackParameters representing the tracking detector elements
          hit in between and the TrackParameters at the destination Surface (if final extrapolation suceeds),
          0 if the extrapolation to the destination surface does not suceed*/
       const TrackParametersVector*  extrapolateStepwise(const IPropagator& prop,
                                                         const TrackParameters& parm,
                                                         const Surface& sf,
                                                         PropDirection dir=anyDirection,
                                                         BoundaryCheck bcheck = true,
                                                         ParticleHypothesis particle=pion ) const;
                                                                                                                                         
       /** S 3) <b>Strategy Pattern extrapolation method</b>: 
        - searches the closest TrackParameters of the Track to the destination Surface
        - returns the TrackParameters at the Destination Surface (if extrapolation succeeds),
          0 if extrapolation to destination surface does not suceed */
       const TrackParameters* extrapolate(const IPropagator& prop,
                                          const Track& trk,
                                          const Surface& sf,
                                          PropDirection dir=anyDirection,
                                          BoundaryCheck bcheck = true,
                                          ParticleHypothesis particle=pion,
                                          MaterialUpdateMode matupmode=addNoise) const;
                                                                                                           
        /** S 4) <b>Strategy Pattern extrapolation method</b>:
         - direct extrapolation to the destination surface, no material effects 
           or intermediate steps are taken into account
        */           
        const TrackParameters* extrapolateDirectly(const IPropagator& prop,
                                                   const TrackParameters& parm,
                                                   const Surface& sf,
                                                   PropDirection dir=anyDirection,
                                                   BoundaryCheck bcheck = true,
                                                   ParticleHypothesis particle=pion) const;
                                                   
        /** S 5) <b>Strategy Pattern extrapolation method</b>:
         - blind extrapolation inside the given TrackingVolume
        */          
        const TrackParametersVector* extrapolateBlindly(const IPropagator& prop,
                                                        const TrackParameters& parm,
                                                        PropDirection dir,
                                                        BoundaryCheck bcheck,
                                                        ParticleHypothesis particle=pion,
                                                        const Volume* boundaryVol=0) const;


        /** S 6) <b>Strategy Pattern extrapolation method</b>:
         - extrapolation to the next active layer, based on the extrapolation to the next (material) layer
           and layer identification
        */          
        std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayer(
                                                                             const IPropagator& prop,
										                                     const TrackParameters& parm,
										                                     PropDirection dir,
										                                     BoundaryCheck bcheck,
										                                     ParticleHypothesis particle=pion,
										                                     MaterialUpdateMode matupmode=addNoise) const;

        /** S 7) <b>Strategy Pattern extrapolation method</b>:
         - extrapolation to the next active layer, based on the extrapolation to the next (material) layer
           and layer identification
        */          
                                                         
        std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayerM(
                                                                             const IPropagator& prop,
										                                     const TrackParameters& parm,
										                                     PropDirection dir,
										                                     BoundaryCheck bcheck,
										                                     std::vector<const Trk::TrackStateOnSurface*>& material,
										                                     ParticleHypothesis particle=pion,
										                                     MaterialUpdateMode matupmode=addNoise) const;
       
        /** S 8) <b>Strategy Pattern extrapolation method</b>:
         - Extrapolation using specific intermediate surfaces and energy loss effects to be accounted for at
         each surface as specified by the corresponding MaterialEffectsOnTrack
         -Final boolean only relevant if LandauMode = true for the configured MaterialEffectsUpdator
        */                                                                           
        const TrackParameters* extrapolate(const IPropagator& prop,
                                           const TrackParameters& parm,
                                           const std::vector< MaterialEffectsOnTrack >& sfMeff,
                                           const TrackingVolume& tvol,
                                           PropDirection dir,
                                           ParticleHypothesis particle=pion,
                                           MaterialUpdateMode matupmode=addNoise) const;

        /** S 9) <b>Strategy Pattern extrapolation method</b>:
         - extrapolation to the next active layer, based on the extrapolation to the next (material) layer
           and layer identification
        */          
        std::pair<const TrackParameters*,const Layer*> extrapolateToNextStation(
                                                                             const IPropagator& prop,
				                              						         const TrackParameters& parm,
										                                     PropDirection dir,
										                                     BoundaryCheck bcheck,
										                                     ParticleHypothesis particle=pion,
										                                     MaterialUpdateMode matupmode=addNoise) const; 
                                                                                                                     
        /** S 10) <b>Strategy Pattern extrapolation method</b>:
         - extrapolation to a volume boundary of an arbitrary tracking volume ( not necessarily part of a tracking geometry ) 
        */          
        const TrackParameters* extrapolateToVolume(const IPropagator& prop,
                                              const TrackParameters& parm,
                                              const Trk::TrackingVolume& vol,
                                              PropDirection dir=anyDirection,
                                              ParticleHypothesis particle=pion) const;

        /** C 1) <b>Configured AlgTool extrapolation method</b> of S 1):*/
        const TrackParameters* extrapolate(const TrackParameters& parm,
                                           const Surface& sf,
                                           PropDirection dir=anyDirection,
                                           BoundaryCheck bcheck = true,
                                           ParticleHypothesis particle=pion,
                                           MaterialUpdateMode matupmode=addNoise,
                                           Trk::ExtrapolationCache* cache = 0) const;

//        const TrackParameters* extrapolate(const TrackParameters& parm,
//                                           const Surface& sf,
//                                           PropDirection dir=anyDirection,
//                                           BoundaryCheck bcheck = true,
//                                           ParticleHypothesis particle=pion,
//                                           MaterialUpdateMode matupmode=addNoise) const;
        
        /** C 2) <b>Configured AlgTool extrapolation method</b> of S 2):*/
        const TrackParametersVector*  extrapolateStepwise(const TrackParameters& parm,
                                                          const Surface& sf,
                                                          PropDirection dir=anyDirection,
                                                          BoundaryCheck bcheck = true,
                                                          ParticleHypothesis particle=pion) const;
                                                   
        /** C 3) <b>Configured AlgTool extrapolation method</b> of S 3):*/
        const TrackParameters* extrapolate(const Track& trk,
                                           const Surface& sf,
                                           PropDirection dir=anyDirection,
                                           BoundaryCheck bcheck = true,
                                           ParticleHypothesis particle=pion,
                                           MaterialUpdateMode matupmode=addNoise,
					   Trk::ExtrapolationCache* cache = 0) const;
                                                                                    
        /** C 4) <b>Configured AlgTool extrapolation method</b> of S 4):*/
        const TrackParameters* extrapolateDirectly(const TrackParameters& parm,
                                                   const Surface& sf,
                                                   PropDirection dir=anyDirection,
                                                   BoundaryCheck bcheck = true,
                                                   ParticleHypothesis particle=pion) const;
                                                           
        /** C 5) <b>Configured AlgTool extrapolation method</b> of S 5):*/                        
        const TrackParametersVector* extrapolateBlindly(const TrackParameters& parm,
                                                        PropDirection dir=anyDirection,
                                                        BoundaryCheck bcheck = true,
                                                        ParticleHypothesis particle=pion,
                                                        const Volume* boundaryVol=0) const;


        /** C 6) <b>Configured AlgTool extrapolation method</b> of S 6):*/                        
        std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayer(
                                                         const TrackParameters& parm,
										                                     PropDirection dir=anyDirection,
										                                     BoundaryCheck bcheck = true,
										                                     ParticleHypothesis particle=pion,
										                                     MaterialUpdateMode matupmode=addNoise) const;
        
        /** C 7) <b>Configured AlgTool extrapolation method</b> of S 7):*/                        
        std::pair<const TrackParameters*,const Layer*> extrapolateToNextActiveLayerM(
                                                         const TrackParameters& parm,
										                                     PropDirection dir,
										                                     BoundaryCheck bcheck,
										                                     std::vector<const Trk::TrackStateOnSurface*>& material,
										                                     ParticleHypothesis particle=pion,
										                                     MaterialUpdateMode matupmode=addNoise) const;

        /** C 8) <b>Configured AlgTool extrapolation method</b> of S 8):*/                        
        std::pair<const TrackParameters*,const Layer*> extrapolateToNextStation(
                                                         const TrackParameters& parm,
										                                     PropDirection dir=anyDirection,
										                                     BoundaryCheck bcheck = true,
										                                     ParticleHypothesis particle=pion,
										                                     MaterialUpdateMode matupmode=addNoise) const;
	
        /** C 9) <b>Configured AlgTool extrapolation method</b> of S 9):*/
        const TrackParameters* extrapolateToVolume(const TrackParameters& parm,
                                                   const Trk::TrackingVolume& vol,
                                                   PropDirection dir=anyDirection,
                                                   ParticleHypothesis particle=pion) const;

        /** C 10) <b>Configured AlgTool extrapolation method</b>:
         - Extrapolate to a destination surface, while collecting all the material layers in between.
        */
        const std::vector<const TrackStateOnSurface*>* extrapolateM(const TrackParameters& parameters,
                                                                    const Surface& sf,
                                                                    PropDirection dir,
                                                                    BoundaryCheck bcheck,
                                                                    ParticleHypothesis particle=pion,
                                                                    Trk::ExtrapolationCache* cache = 0) const;

        /** C 11) <b>Configured AlgTool extrapolation method</b>:
         - Extrapolate to a destination surface, while collecting all the material layers and transport jacobians in between.
        */
        const std::vector<const TrackParameters*>* extrapolateM(const TrackParameters& parameters,
                                                                const Surface& sf,
                                                                PropDirection dir,
                                                                BoundaryCheck bcheck,
                                                                std::vector<MaterialEffectsOnTrack>& material,
                                                                std::vector<Trk::TransportJacobian *>& jacs,
                                                                ParticleHypothesis particle=pion,
                                                                Trk::ExtrapolationCache* cache = 0) const;

	const Trk::TrackParameters*  extrapolateWithPathLimit(
							      const Trk::TrackParameters& parm,
							      double& pathLim,
							      Trk::PropDirection dir,
							      Trk::ParticleHypothesis particle,
							      std::vector<const Trk::TrackParameters*>*& parmOnSf,
							      std::vector<const Trk::TrackStateOnSurface*>*& material,
							      const Trk::TrackingVolume* boundaryVol=0,
							      MaterialUpdateMode matupmod=Trk::addNoise) const;


        /** extrapolation method collecting intersections with subdetector boundaries and active volumes/layers.
            A primitive identification is provided - to be replaced with appropriate identifier, and possibly merged
            with TrackParameters. Material collection in option. Destination (subdetector boundary) : geoID (+ entry, -exit)
            ( default MS exit )
        */
	const std::vector< std::pair< const Trk::TrackParameters*, int > >*  extrapolate(
							      const Trk::TrackParameters& parm,
							      Trk::PropDirection dir,
							      Trk::ParticleHypothesis particle,
							      std::vector<const Trk::TrackStateOnSurface*>*&  material,
							      int destination = 3 ) const;



       /** Return the TrackingGeometry used by the Extrapolator (forward information from Navigator)*/
       const TrackingGeometry* trackingGeometry() const; 

      /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps */
      virtual void validationAction() const;
                                           
  private:

    /** Private method for extrapolation in final volume to destination surface
         - Parameters are: IPropagator& prop            ... propagator to be used
                           TrackParameters& parm        ... starting parameters
                           Surface& sf                  ... destination surface
                           TrackingVolume&              ... the initial volume
                           Layer* associatedLayer       ... layer associatiated with starting parameters (steers postupdate)
                           PropDirection dir            ... propagation direction
                           BoundaryCheck bcheck         ... boolean for bounday check
                           ParticleHypothesis  particle ... the particle hypothesis
                           std::vector<const TrackParameters*>* dethits ... for blind extrapolation     

           it will call:
                - A) insideVolumeStaticLayers() for a TrackingVolume with static layers
                - C) insideVolumeDetachedVolumes() for a TrackingVolume with detached inner Volumes
    */
    const TrackParameters* extrapolateInsideVolume(const IPropagator& prop,
                                                   const TrackParameters& parm,
                                                   const Surface& sf,
                                                   const Layer* associatedLayer,
                                                   const TrackingVolume& tvol,
                                                   PropDirection dir = anyDirection,
                                                   BoundaryCheck bcheck = true,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const;



    /** A) call from extrapolateInsideVolume or toBoundary,
       if it is to boundary, the return parameters are the parameters at the boundary
     */
   const TrackParameters* insideVolumeStaticLayers(bool toBoundary,
                                                   const IPropagator& prop,
                                                   const TrackParameters& parm,
                                                   const Layer* associatedLayer,
                                                   const TrackingVolume& tvol,
                                                   PropDirection dir = anyDirection,
                                                   BoundaryCheck bcheck = true,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const;

    /** C) call from extrapolateInsideVolume */
     const TrackParameters* extrapolateWithinDetachedVolumes(
                                                    const IPropagator& prop,
                                                    const TrackParameters& parm,
                                                    const Surface& sf,
                                                    const TrackingVolume& tvol,
                                                    PropDirection dir = anyDirection,
                                                    BoundaryCheck bcheck = true,
                                                    ParticleHypothesis particle=pion,
                                                    MaterialUpdateMode matupmode=addNoise) const;

     const TrackParameters* extrapolateToNextMaterialLayer(const IPropagator& prop,
                 const TrackParameters& parm,
                 const Trk::Surface* destSurf, 
                 const Trk::TrackingVolume* vol,
                 PropDirection dir,
                 BoundaryCheck bcheck,
                 ParticleHypothesis particle=pion,
                  MaterialUpdateMode matupmode=addNoise) const;

     const TrackParameters* extrapolateInAlignableTV(const IPropagator& prop,
                 const TrackParameters& parm,
                 const Trk::Surface* destSurf, 
                 const Trk::AlignableTrackingVolume* vol,
                 PropDirection dir,
                 ParticleHypothesis particle=pion) const;

     const Trk::TrackParameters*  extrapolateToVolumeWithPathLimit(
								   const Trk::TrackParameters& parm,
								   double pathLim,
								   Trk::PropDirection dir,
								   Trk::ParticleHypothesis particle,
								   const Trk::TrackingVolume* destVol,
								   MaterialUpdateMode matupmod=addNoise) const;
       

      /** Private method for extrapolation in intermediate volume to boundary surface
         - Parameters are: IPropagator& prop            ... propagator to be used
                           TrackParameters& parm        ... starting parameters
                           Surface& sf                  ... destination surface
                           TrackingVolume&              ... the initial volume
                           Layer* associatedLayer       ... layer associatiated with starting parameters (steers postupdate)
                           PropDirection dir            ... propagation direction
                           BoundaryCheck bcheck         ... boolean for bounday check
                           ParticleHypothesis  particle ... the particle hypothesis
                           std::vector<const TrackParameters*>* dethits ... for blind extrapolation

           it will call:
                - A) toVolumeBoundaryStaticLayers() for a TrackingVolume with static layers
                - C) toVolumeBoundaryDetachedVolumes() for a TrackingVolume with detached inner Volumes
    */
    void extrapolateToVolumeBoundary(const IPropagator& prop,
                                     const TrackParameters& parm,
                                     const Layer* associatedLayer,
                                     const TrackingVolume& tvol,
                                     PropDirection dir = anyDirection,
                                     BoundaryCheck bcheck = true,
                                     ParticleHypothesis particle=pion,
                                     MaterialUpdateMode matupmode=addNoise
                                     ) const;
 
    /** Private method to step from one to the last 
        layer and stop at last layer (before 0) or before destination layer */
    const TrackParameters* extrapolateFromLayerToLayer(const IPropagator& prop,
                                                       const TrackParameters& parm,
                                                       const TrackingVolume& tvol,
                                                       const Layer* nextLayer,                                                       
                                                       const Layer* destinationLayer = 0,
                                                       const TrackParameters* navParameters = 0,
                                                       PropDirection dir = anyDirection,
                                                       BoundaryCheck bcheck = true,
                                                       ParticleHypothesis particle=pion,
                                                       MaterialUpdateMode matupmode=addNoise
                                                       ) const;

    /** Private to extrapolate to the destination layer + surface
      */
    const TrackParameters* extrapolateToDestinationLayer(const IPropagator& prop,
                                                         const TrackParameters& parm,
                                                         const Surface& sf,
                                                         const Layer& lay,
                                                         const TrackingVolume& tvol,
                                                         const Layer* startLayer,
                                                         PropDirection dir = anyDirection,
                                                         BoundaryCheck bcheck = true,
                                                         ParticleHypothesis particle=pion,
                                                         MaterialUpdateMode matupmode=addNoise
                                                         ) const;
                                                                         
    /** Private to extrapolate to the destination layer + surface, special treatment for exit layer
      */
    const TrackParameters* extrapolateToIntermediateLayer(const IPropagator& prop,
                                                          const TrackParameters& parm,
                                                          const Layer& lay,
                                                          const TrackingVolume& tvol,
                                                          PropDirection dir = anyDirection,
                                                          BoundaryCheck bcheck = true,
                                                          ParticleHypothesis particle=pion,
                                                          MaterialUpdateMode matupmode=addNoise,
                                                          bool perpendicularCheck = true) const;

    /** Private to search for overlap surfaces */
    void overlapSearch(const IPropagator& prop,
                       const TrackParameters& parm,
                       const TrackParameters& parsOnLayer,
                       const Layer& lay,
                       const TrackingVolume& tvol,
                       PropDirection dir = anyDirection,
                       BoundaryCheck bcheck = true,
                       ParticleHypothesis particle=pion,
                       bool startingLayer = false) const;


    /** Private method for chosing the propagator in the simple/full configured mode */
    unsigned int propagatorType(const TrackingVolume& tvol) const;
    
    /** Private method for Initial Extrapolation setup 
       -> overwrites the given pointers for the start and destination parameters
       -> returns a direction for the Navigation :
             - usually dir if dir @f$ \in  @f$ [ Trk::alongMomentum, Trk::oppositeMomentum ] 
             - a chosen one if dir == Trk::anyDirection
     */
    PropDirection initializeNavigation(const Trk::IPropagator&             prop,
                                       const Trk::TrackParameters&         startPars,
                                       const Trk::Surface&                 destSurface,
                                       Trk::PropDirection                  dir,
                                       ParticleHypothesis                  particle, 
                                       const Trk::TrackParameters*&        referenceParameters,
                                       const Trk::Layer*&                  associatedLayer,
                                       const Trk::TrackingVolume*&         associatedVolume,
                                       const Trk::TrackingVolume*&         destinationVolume) const;
    
    /** RadialDirection helper method 
        - inbound : -1
        - outbound:  1  */
    int radialDirection(const Trk::TrackParameters& pars, PropDirection dir) const;                         

    /** Check for punchThrough in case of radial (perpendicular) direction change,
        returns true if the radial direction change is actually ok (i.e. punch-through allowed)
     */
    bool radialDirectionCheck(const IPropagator& prop,
                              const TrackParameters& startParm,
                              const TrackParameters& parsOnLayer,
                              const TrackingVolume& tvol,
                              PropDirection dir = anyDirection,
                              ParticleHypothesis particle=pion) const;
                                       
    /** Access the subPropagator to the given volume*/
    const IPropagator* subPropagator(const TrackingVolume& tvol) const;
                                       
    /** Access the subPropagator to the given volume*/
    const IMaterialEffectsUpdator* subMaterialEffectsUpdator(const TrackingVolume& tvol) const;
        
    /** Private method for setting recall Information */
    void setRecallInformation(const Surface&, const Layer&, const TrackingVolume&) const;    

    /** Private method for resetting the recallInformation */
    void resetRecallInformation() const;

    /** Private method for throwing into the GarbageBin */
    void throwIntoGarbageBin(const Trk::TrackParameters* garbage) const;

    /** Private method for emptying the GarbageBin */
    void emptyGarbageBin() const;
    void emptyGarbageBin(const Trk::TrackParameters*) const;

    /** Private method to return from extrapolate() main method,
        cleans up, calls model action or validation action, empties garbage bin and leaves */
    const Trk::TrackParameters* returnResult(const Trk::TrackParameters* result, const Trk::TrackParameters* refParameters) const;

    /** For the output - layer */
    std::string layerRZoutput(const Trk::Layer& lay) const;
    
    /** For the output - global position */
    std::string positionOutput(const Amg::Vector3D& pos) const;
    /** For the output - global momentum */
    std::string momentumOutput(const Amg::Vector3D& mom) const;
    
    /** helper method for MaterialEffectsOnTrack to be added */
    void addMaterialEffectsOnTrack(const Trk::IPropagator& prop,
                                   const Trk::TrackParameters& parm,
                                   const Trk::Layer& lay,
                                   const Trk::TrackingVolume& vol,
                                   Trk::PropDirection propDir,
                                   Trk::ParticleHypothesis) const;

    void dumpCache(std::string txt) const; 
    bool checkCache(std::string txt) const;

    /** Private method for conversion of the synchronized geometry signature to the natural subdetector ordering */
    //unsigned int geoIDToDetOrder(Trk::GeometrySignature geoid) const;


   // --------------- Used Tools ----------------------------- //
 
    ToolHandleArray< IPropagator >              m_propagators;                   //!<  Array of Propagators
    ToolHandle< IPropagator >                   m_stepPropagator;                //!<  Array of Propagators
    ToolHandle<  INavigator >                   m_navigator;                     //!<  Navigator for TrackingGeometry and magnetic fiels acces
    ToolHandleArray< IMaterialEffectsUpdator >  m_updators;                      //!<  Array of Material Updators
    ToolHandleArray< IMultipleScatteringUpdator >  m_msupdators;                 //!<  Array of MultipleScattering Updators
    ToolHandleArray< IEnergyLossUpdator >       m_elossupdators;                      //!<  Array of EnergyLoss Updators

    // ---------------- For Extrapolation handling ------------ //
     
    std::vector<const IPropagator*>             m_subPropagators;                //!< Propagators to chose from (steered by signature)
    std::vector<const IMaterialEffectsUpdator*> m_subUpdators;                   //!< Updators to chose from (steered by signature)

    // ---------------- For Extrapolator configuration ------------ //

    std::vector<std::string>                    m_propNames;                    //!<  configuration of subPropagators
    std::vector<std::string>                    m_updatNames;                   //!<  configuration of subUpdators

    // --------------- General steering & Navigation -------------- //

    unsigned int                    m_meotpIndex;                    //!< if several meotps are available in a volume steer which one to use
    unsigned int                    m_configurationLevel;            //!< see the supported levels of configuration above
    unsigned int                    m_searchLevel;                   //!< see the supported search levels above
    bool                            m_includeMaterialEffects;        //!< boolean to switch on/off material effects
    bool                            m_requireMaterialDestinationHit; //!< require the destination surface hit for material collection
    bool                            m_stopWithNavigationBreak;       //!< return 0 if navigation breaks - for validation reasons
    bool                            m_stopWithUpdateZero;            //!< return 0 if update kills the trajectory 
    bool                            m_subSurfaceLevel;               //!< tep down to sub-surface level
    bool                            m_skipInitialLayerUpdate;        //!< skip the initial post-Update at the layer [Fatras conversion mode]
    bool                            m_referenceMaterial;             //!< use the reference material for the update
    bool                            m_extendedLayerSearch;           //!< extended layer search
    unsigned int                    m_initialLayerAttempts;          //!< allowed layer intersection attempts at the start of a volume
    unsigned int                    m_successiveLayerAttempts;       //!< layer intersection attemps after one layer has been hit sucessfully

    double                          m_tolerance;                    //!< surfacen & volume tolerance
    
    // ------------------------------------------------------- //      
   
    bool                            m_activeOverlap;                 //!<  consider overlaps between active muon volumes  
    bool                            m_useMuonMatApprox;              //!<  use approximative MS inert material
    bool                            m_useDenseVolumeDescription;     //!<  use dense volume description when available in ID/Calo
    bool                            m_checkForCompundLayers;         //!<  use the multi-layer tests for compound layers
    mutable bool                    m_dense;                         //!<  internal switch for resolved configuration 
    
    // ------------ Re-initialize (depending on volumes)  ----------------------- //    
    mutable const Surface*          m_destinationSurface;                    
    mutable const Surface*          s_referenceSurface;

    // ------------ Recall / Boundary Information ----------------------- //    
    mutable const Volume*           m_boundaryVolume;                 //!< the boundary volume check

    mutable bool                    m_recall;                         //!< Flag the recall solution
    mutable const Surface*          m_recallSurface;                  //!< Destination Surface for recall
    mutable const Layer*            m_recallLayer;                    //!< Destination Layer for recall
    mutable const TrackingVolume*   m_recallTrackingVolume;           //!< Destination TrackingVolume for recall
    
    mutable ParametersNextVolume    m_parametersAtBoundary;           //!< return helper for parameters and boundary  
    mutable ParametersNextVolume    m_parametersAtDetachedBoundary;   //!< return helper for parameters and boundary 
    mutable TrackParametersVector*  m_parametersOnDetElements;        //!< return helper for parameters on detector elements
    typedef std::vector< std::pair< const Trk::TrackParameters*, int > > identifiedParameters_t;
    mutable std::unique_ptr<identifiedParameters_t>  m_identifiedParameters;  // for active volumes 

    mutable const Trk::TrackParameters* m_lastValidParameters;            //!< parameters to be used for final propagation in case of fallback

    mutable std::map<const Trk::TrackParameters*, bool> m_garbageBin; //!< garbage collection during extrapolation

    mutable const Trk::TrackingVolume*                                m_currentStatic;
    mutable const Trk::TrackingVolume*                                m_currentDense;
    mutable const Trk::TrackingVolume*                                m_highestVolume;
    mutable bool                                                      m_resolveActive;
    mutable bool                                                      m_resolveMultilayers;
    mutable std::pair<unsigned int, unsigned int>                     m_denseResolved;
    mutable unsigned int                                              m_layerResolved;  
    mutable std::vector<std::pair<const Trk::DetachedTrackingVolume*,unsigned int> >    m_detachedVols;
    mutable std::vector<std::pair<const Trk::TrackingVolume*,unsigned int> >            m_denseVols;
    mutable std::vector<std::pair<const Trk::TrackingVolume*,const Trk::Layer*> >       m_navigLays;
    mutable std::vector<DestSurf>                                     m_staticBoundaries;
    mutable std::vector<DestSurf>                                     m_detachedBoundaries;
    mutable std::vector<DestSurf>                                     m_denseBoundaries;
    mutable std::vector<DestSurf>                                     m_navigBoundaries;
    mutable std::vector<DestSurf>                                     m_layers;
    bool                                                              m_returnPassiveLayers;
    mutable bool                                                      m_robustSampling;
    mutable double                                                    m_path;    

    //------------------------- NAVIGATION -------- ----------------------------------------------//
    mutable int                     m_methodSequence;
    mutable int                     m_maxMethodSequence;

    //-------------------------- SCREEN output steering -------------------------------------------//
    bool                            m_printHelpOutputAtInitialize;
    bool                            m_printRzOutput;

    //------------------------- VALIDATION MODE SECTION ------------------------------------------//

    mutable int                     m_extrapolateCalls;              //!< number of calls: extrapolate() method
    mutable int                     m_extrapolateBlindlyCalls;       //!< number of calls: extrapolateBlindly() method 
    mutable int                     m_extrapolateDirectlyCalls;      //!< number of calls: extrapolateDirectly() method
    mutable int                     m_extrapolateStepwiseCalls;      //!< number of calls: extrapolateStepwise() method

    mutable int                     m_startThroughAssociation;        //!< navigation intialization
    mutable int                     m_startThroughRecall;             //!< navigation intialization
    mutable int                     m_startThroughGlobalSearch;       //!< navigation intialization
    mutable int                     m_destinationThroughAssociation;  //!< navigation intialization
    mutable int                     m_destinationThroughRecall;       //!< navigation intialization
    mutable int                     m_destinationThroughGlobalSearch; //!< navigation intialization
    
    mutable int                     m_layerSwitched;                 //!< number of layers that have been switched 

    // ----------------------------- navigation validation section -----------------------------------------------------------

    bool                            m_navigationStatistics;           //!< steer the output for the navigaiton statistics
    bool                            m_navigationBreakDetails;         //!< steer the output for the navigation break details
    mutable int                     m_navigationBreakLoop;            //!< number of navigation breaks due to loop
    mutable std::map<const Trk::TrackingVolume*,int> m_loopVolumes;   //!< record name of the volumes where oscillation happened
    mutable int                     m_navigationBreakOscillation;     //!< number of navigation breaks due to oscillation
    mutable std::map<const Trk::TrackingVolume*,int> m_oscillationVolumes;    //!< record name of the volumes where oscillation happened
    mutable int                     m_navigationBreakNoVolume;       //!< number of navigation breaks due no Volume found
    mutable std::map<const Trk::TrackingVolume*,int> m_noNextVolumes;         //!< record names of the volumes where no next one is found
    mutable int                     m_navigationBreakDistIncrease;   //!< number of navigation breaks due to distance increase
    mutable std::map<const Trk::TrackingVolume*,int> m_distIncreaseVolumes;   //!< record name of  the voluems where the distance increases
    mutable int                     m_navigationBreakVolumeSignature;   //!< number of navigation breaks due to distance increase
    mutable std::map<const Trk::TrackingVolume*,int> m_volSignatureVolumes;   //!< record name of  the voluems where the distance increases
    mutable int                     m_overlapSurfaceHit;             //!< number of OverlapSurfaces found

    bool                            m_materialEffectsOnTrackValidation; //!< mat effects on track validation
    mutable int                     m_meotSearchCallsFw;                //!< how often the meot search is called: forward
    mutable int                     m_meotSearchCallsBw;                //!< how often the meot search is called: backward
    mutable int                     m_meotSearchSuccessfulFw;           //!< how often the meot search was successful: forward
    mutable int                     m_meotSearchSuccessfulBw;           //!< how often the meot search was successful: backward

    // ------------------------------- cache --------------------------------------------------------------------

    mutable const Layer*                                       m_lastMaterialLayer; //!< cache layer with last material update
    bool                                                       m_cacheLastMatLayer;   // steering of the material layer cache 
    mutable std::vector<const Trk::TrackStateOnSurface*>       *m_matstates; //!< cache of TrackStateOnSurfaces
    mutable std::vector<Trk::TransportJacobian*>               *m_jacs;      //!< cache of Transport Jacobians
    mutable Trk::ExtrapolationCache*                           m_extrapolationCache;      //!< cache for collecting the total X0 ans Eloss
    mutable const Trk::EnergyLoss*                                           m_cacheEloss;      //!< cache pointer for Eloss


    // ------------------------------- static members --------------------------------------------------------------------
    static double                   s_distIncreaseTolerance;         //!< distance increatse tolerance to account for straight line approx.

    unsigned int m_maxNavigSurf;
    unsigned int m_maxNavigVol;
    mutable std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >  m_navigSurfs;
    mutable std::vector<const Trk::DetachedTrackingVolume*>                  m_navigVols;
    mutable std::vector<std::pair<const Trk::TrackingVolume*,unsigned int> > m_navigVolsInt;

   bool m_dumpCache;
   //------------ Magnetic field properties
   bool m_fastField;
   Trk::MagneticFieldProperties m_fieldProperties;

  };

inline const TrackingGeometry* Extrapolator::trackingGeometry() const 
 { 
   if (m_navigator) return m_navigator->trackingGeometry();
   return 0;
 }

 
inline const IPropagator* Extrapolator::subPropagator(const Trk::TrackingVolume& tvol) const
{
  const IPropagator* currentPropagator = (tvol.geometrySignature() < m_subPropagators.size()) ?
    m_subPropagators[tvol.geometrySignature()] : 0;

  if (tvol.geometrySignature()==Trk::Calo && m_useDenseVolumeDescription) currentPropagator= (Trk::MS < m_subPropagators.size()) ?
    m_subPropagators[Trk::MS] : 0; 

  if (!currentPropagator)
      msg(MSG::ERROR) << "[!] Configuration problem: no Propagator found for volumeSignature: " << tvol.geometrySignature() << endreq;
  return currentPropagator;         
}
                                       
inline const IMaterialEffectsUpdator* Extrapolator::subMaterialEffectsUpdator(const Trk::TrackingVolume& tvol) const
{
  return (tvol.geometrySignature() < m_subUpdators.size()) ?
    m_subUpdators[tvol.geometrySignature()] : 0;
}

     
inline void Extrapolator::setRecallInformation(const Surface& rsf, const Layer& rlay, const TrackingVolume& rvol) const
 {
   m_recall               = true;
   m_recallSurface        = &rsf;
   m_recallLayer          = &rlay;
   m_recallTrackingVolume = &rvol;
 }

inline void Extrapolator::resetRecallInformation() const
 {
   m_recall               = false;
   m_recallSurface        = 0;
   m_recallLayer          = 0;
   m_recallTrackingVolume = 0;
 }


inline void Extrapolator::throwIntoGarbageBin(const Trk::TrackParameters* pars) const
{ if (pars) m_garbageBin[pars] = true; }

inline const Trk::TrackParameters* Extrapolator::returnResult(const Trk::TrackParameters* result,
                                                              const Trk::TrackParameters* refParameters) const
{
    delete refParameters;
    // memory cleanup
    emptyGarbageBin();
    // call the model action on the material effect updators
    for (unsigned int imueot = 0; imueot < m_subUpdators.size(); ++imueot) 
         m_subUpdators[imueot]->modelAction();            
    // return the result
    return result;
}

/*
inline unsigned int Extrapolator::geoIDToDetOrder(Trk::GeometrySignature geoid) const
{
  if ( geoid == Trk::ID ) return 0;
  else if ( geoid == Trk::Calo ) return 1;
  else if ( geoid == Trk::MS ) return 2;
  
  return 0; 
}
*/

} // end of namespace


#endif // TRKEXTOOLS_TRKEXTRAPOLATOR_H

