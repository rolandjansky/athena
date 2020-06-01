/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IExtrapolator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IEXTRAPOLATOR_H
#define TRKEXINTERFACES_IEXTRAPOLATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"

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
                                                    const BoundaryCheck& bcheck = true) const = 0;



       /** [NeutralParameters] ------------------------------------------------------------- */

       /** N 0) <b>Neutral parameters method </b> 
           - returns a ParametersBase object as well, 0 if the extrapolation did not succeed 
         */

       virtual const NeutralParameters* extrapolate(const NeutralParameters& parameters,
                                                    const Surface& sf,
                                                    PropDirection dir=anyDirection,
                                                    const BoundaryCheck& bcheck = true) const = 0;


       /** [TrackParameters] interfaces WITH EventContext ------------------------------------------ */

      
        /** xAOD 0) xAOD track particle */
        virtual const TrackParameters* extrapolate(const EventContext& ctx,
                                                   const xAOD::TrackParticle& particleBase,
                                                   const Surface& sf,
                                                   PropDirection dir=anyDirection,
                                                   const BoundaryCheck& bcheck = true,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const = 0;


      
       /** 1) <b>Configured AlgTool extrapolation method</b>):
        -  returns the TrackParameters at the Destination Surface (if extrapolation succeeds),
        nullptr if extrapolation to destination surface does not succeed */
       virtual const TrackParameters* extrapolate(const EventContext& ctx,
                                                  const TrackParameters& parm,
                                                  const Surface& sf,
                                                  PropDirection dir = anyDirection,
                                                  const BoundaryCheck& bcheck = true,
                                                  ParticleHypothesis particle = pion,
                                                  MaterialUpdateMode matupmode = addNoise,
                                                  Trk::ExtrapolationCache* cache = nullptr) const = 0;

       /** 2) <b>Configured AlgTool extrapolation method</b>):
          - returns a vector of TrackParameters representing the tracking detector elements
          hit in between and the TrackParameters at the destination Surface (if final extrapolation suceeds),
          empty if the extrapolation to the destination surface does not suceed*/
       virtual std::vector<std::unique_ptr<const TrackParameters>> extrapolateStepwise(
         const EventContext& ctx,
         const TrackParameters& parm,
         const Surface& sf,
         PropDirection dir = anyDirection,
         const BoundaryCheck& bcheck = true,
         ParticleHypothesis particle = pion) const = 0;

       /** 3) <b>Configured AlgTool extrapolation method</b>):
        - searches the closest TrackParameters of the Track to the destination Surface
       - returns the TrackParameters at the Destination Surface (if extrapolation succeeds), 
       0 if extrapolation to destination surface does not suceed */
       virtual const TrackParameters* extrapolate(const EventContext& ctx,
                                                  const Track& trk,
                                                  const Surface& sf,
                                                  PropDirection dir = anyDirection,
                                                  const BoundaryCheck& bcheck = true,
                                                  ParticleHypothesis particle = pion,
                                                  MaterialUpdateMode matupmode = addNoise,
                                                  Trk::ExtrapolationCache* cache = nullptr) const = 0;

       /** 4) <b>Configured AlgTool extrapolation method</b>):
        - direct extrapolation to the destination surface, no material effects 
        or intermediate steps are taken into account
       */
       virtual TrackParameters* extrapolateDirectly(const EventContext& ctx,
                                                    const TrackParameters& parm,
                                                    const Surface& sf,
                                                    PropDirection dir = anyDirection,
                                                    const BoundaryCheck& bcheck = true,
                                                    ParticleHypothesis particle = pion) const = 0;

       /** 4.1) <b>Configured AlgTool extrapolation method</b>):
        Same as 4 but with propagator arguement (needed by a single client TRT_TrackExtensionToolCosmics
       */
       virtual TrackParameters* extrapolateDirectly(const EventContext& ctx,
                                                    const IPropagator& prop,
                                                    const TrackParameters& parm,
                                                    const Surface& sf,
                                                    PropDirection dir = anyDirection,
                                                    const BoundaryCheck& bcheck = true,
                                                    ParticleHypothesis particle = pion) const = 0;

       /** 5) <b>Configured AlgTool extrapolation method</b>):
         - blind inside the given tracking Volume (boundaryVol), 
         if none is given the reference surface for destination is used
        */
       virtual std::vector<std::unique_ptr<const TrackParameters>> extrapolateBlindly(
         const EventContext& ctx,
         const TrackParameters& parm,
         PropDirection dir = anyDirection,
         const BoundaryCheck& bcheck = true,
         ParticleHypothesis particle = pion,
         const Volume* boundaryVol = nullptr) const = 0;

       /** 6) <b>Configured AlgTool extrapolation method</b>):
        *- extrapolation to the next active layer, based on the extrapolation to the next layer
        and layer identification*/
       virtual std::pair<const TrackParameters*, const Trk::Layer*> extrapolateToNextActiveLayer(
         const EventContext& ctx,
         const TrackParameters& parm,
         PropDirection dir = anyDirection,
         const BoundaryCheck& bcheck = true,
         ParticleHypothesis particle = pion,
         MaterialUpdateMode matupmode = addNoise) const = 0;

       /** 7) <b>Configured AlgTool extrapolation method</b>):*/
       virtual std::pair<const TrackParameters*, const Trk::Layer*> extrapolateToNextActiveLayerM(
         const EventContext& ctx,
         const TrackParameters& parm,
         PropDirection dir,
         const BoundaryCheck& bcheck,
         std::vector<const Trk::TrackStateOnSurface*>& material,
         ParticleHypothesis particle = pion,
         MaterialUpdateMode matupmode = addNoise) const = 0;

      /** 8) <b>Configured AlgTool extrapolation method</b> ):
        - extrapolation to the next active layer, based on the extrapolation to the next layer
        and layer identification
        * */
       virtual const TrackParameters* extrapolateToVolume(const EventContext& ctx,
                                                          const TrackParameters& parm,
                                                          const Trk::TrackingVolume& vol,
                                                          PropDirection dir = anyDirection,
                                                          ParticleHypothesis particle = pion) const = 0;

       /** 9) <b>Configured AlgTool extrapolation method</b>:
        - Extrapolate to a destination surface, while collecting all the material layers in between.
       */
       virtual std::vector<const TrackStateOnSurface*>* extrapolateM(const EventContext& ctx,
                                                                     const TrackParameters& parameters,
                                                                     const Surface& sf,
                                                                     PropDirection dir,
                                                                     const BoundaryCheck& bcheck,
                                                                     ParticleHypothesis particle = pion,
                                                                     Trk::ExtrapolationCache* cache = nullptr) const = 0;

       /** 10) <b>Configured AlgTool extrapolation method</b>:
        - Extrapolate to a destination surface, while collecting all the material layers and transport jacobians in
        between.
       */
       virtual std::vector<const TrackParameters*>* extrapolateM(const EventContext& ctx,
                                                                 const TrackParameters& parameters,
                                                                 const Surface& sf,
                                                                 PropDirection dir,
                                                                 const BoundaryCheck& bcheck,
                                                                 std::vector<MaterialEffectsOnTrack>& material,
                                                                 std::vector<Trk::TransportJacobian*>& jacs,
                                                                 ParticleHypothesis particle = pion,
                                                                 Trk::ExtrapolationCache* cache = nullptr) const = 0;

       virtual const Trk::TrackParameters* extrapolateWithPathLimit(
         const EventContext& ctx,
         const Trk::TrackParameters& parm,
         double& pathLim,
         Trk::PropDirection dir,
         Trk::ParticleHypothesis particle,
         std::vector<const Trk::TrackParameters*>*& parmOnSf,
         std::vector<const Trk::TrackStateOnSurface*>*& material,
         const Trk::TrackingVolume* boundaryVol = nullptr,
         MaterialUpdateMode matupmod = Trk::addNoise) const = 0;

       /** extrapolation method collecting intersections with subdetector boundaries and active volumes/layers.
           A primitive identification is provided - to be replaced with appropriate identifier, and possibly merged
           with TrackParameters. Material collection in option. Destination (subdetector boundary) : geoID (exit)
       */
       virtual const std::vector<std::pair<const Trk::TrackParameters*, int>>* extrapolate(
         const EventContext& ctx,
         const Trk::TrackParameters& parm,
         Trk::PropDirection dir,
         Trk::ParticleHypothesis particle,
         std::vector<const Trk::TrackStateOnSurface*>*& material,
         int destination = 3) const = 0;

       /** Return the TrackingGeometry used by the Extrapolator (forwards information from Navigator) */
       virtual const TrackingGeometry* trackingGeometry() const = 0;

       /** Validation Action:
         Can be implemented optionally, outside access to internal validation steps
           */
       virtual void validationAction() const = 0;

       /** Access the subPropagator to the given volume*/
       virtual const IPropagator* subPropagator(const TrackingVolume& tvol) const = 0;


       /** [TrackParameters] old interfaces WITHOUT EventContext ------------------------------------------ */

      
        /** xAOD 0) xAOD track particle */
        virtual const TrackParameters* extrapolate(const xAOD::TrackParticle& particleBase,
                                                   const Surface& sf,
                                                   PropDirection dir=anyDirection,
                                                   const BoundaryCheck& bcheck = true,
                                                   ParticleHypothesis particle=pion,
                                                   MaterialUpdateMode matupmode=addNoise) const;


      
       /** 1) <b>Configured AlgTool extrapolation method</b>):
        -  returns the TrackParameters at the Destination Surface (if extrapolation succeeds),
        nullptr if extrapolation to destination surface does not succeed */
       virtual const TrackParameters* extrapolate(const TrackParameters& parm,
                                                  const Surface& sf,
                                                  PropDirection dir = anyDirection,
                                                  const BoundaryCheck& bcheck = true,
                                                  ParticleHypothesis particle = pion,
                                                  MaterialUpdateMode matupmode = addNoise,
                                                  Trk::ExtrapolationCache* cache = nullptr) const;

       /** 2) <b>Configured AlgTool extrapolation method</b>):
          - returns a vector of TrackParameters representing the tracking detector elements
          hit in between and the TrackParameters at the destination Surface (if final extrapolation suceeds),
          empty if the extrapolation to the destination surface does not suceed*/
       virtual std::vector<std::unique_ptr<const TrackParameters>> extrapolateStepwise(
         const TrackParameters& parm,
         const Surface& sf,
         PropDirection dir = anyDirection,
         const BoundaryCheck& bcheck = true,
         ParticleHypothesis particle = pion) const;

       /** 3) <b>Configured AlgTool extrapolation method</b>):
        - searches the closest TrackParameters of the Track to the destination Surface
       - returns the TrackParameters at the Destination Surface (if extrapolation succeeds), 
       0 if extrapolation to destination surface does not suceed */
       virtual const TrackParameters* extrapolate(const Track& trk,
                                                  const Surface& sf,
                                                  PropDirection dir = anyDirection,
                                                  const BoundaryCheck& bcheck = true,
                                                  ParticleHypothesis particle = pion,
                                                  MaterialUpdateMode matupmode = addNoise,
                                                  Trk::ExtrapolationCache* cache = nullptr) const;

       /** 4) <b>Configured AlgTool extrapolation method</b>):
        - direct extrapolation to the destination surface, no material effects 
        or intermediate steps are taken into account
       */
       virtual TrackParameters* extrapolateDirectly(const TrackParameters& parm,
                                                    const Surface& sf,
                                                    PropDirection dir = anyDirection,
                                                    const BoundaryCheck& bcheck = true,
                                                    ParticleHypothesis particle = pion) const;

       /** 4.1) <b>Configured AlgTool extrapolation method</b>):
        Same as 4 but with propagator arguement (needed by a single client TRT_TrackExtensionToolCosmics
       */
       virtual TrackParameters* extrapolateDirectly(const IPropagator& prop,
                                                    const TrackParameters& parm,
                                                    const Surface& sf,
                                                    PropDirection dir = anyDirection,
                                                    const BoundaryCheck& bcheck = true,
                                                    ParticleHypothesis particle = pion) const;

       /** 5) <b>Configured AlgTool extrapolation method</b>):
         - blind inside the given tracking Volume (boundaryVol), 
         if none is given the reference surface for destination is used
        */
       virtual std::vector<std::unique_ptr<const TrackParameters>> extrapolateBlindly(
         const TrackParameters& parm,
         PropDirection dir = anyDirection,
         const BoundaryCheck& bcheck = true,
         ParticleHypothesis particle = pion,
         const Volume* boundaryVol = 0) const;

       /** 6) <b>Configured AlgTool extrapolation method</b>):
        *- extrapolation to the next active layer, based on the extrapolation to the next layer
        and layer identification*/
       virtual std::pair<const TrackParameters*, const Trk::Layer*> extrapolateToNextActiveLayer(
         const TrackParameters& parm,
         PropDirection dir = anyDirection,
         const BoundaryCheck& bcheck = true,
         ParticleHypothesis particle = pion,
         MaterialUpdateMode matupmode = addNoise) const;

       /** 7) <b>Configured AlgTool extrapolation method</b>):*/
       virtual std::pair<const TrackParameters*, const Trk::Layer*> extrapolateToNextActiveLayerM(
         const TrackParameters& parm,
         PropDirection dir,
         const BoundaryCheck& bcheck,
         std::vector<const Trk::TrackStateOnSurface*>& material,
         ParticleHypothesis particle = pion,
         MaterialUpdateMode matupmode = addNoise) const;

      /** 8) <b>Configured AlgTool extrapolation method</b> ):
        - extrapolation to the next active layer, based on the extrapolation to the next layer
        and layer identification
        * */
       virtual const TrackParameters* extrapolateToVolume(const TrackParameters& parm,
                                                          const Trk::TrackingVolume& vol,
                                                          PropDirection dir = anyDirection,
                                                          ParticleHypothesis particle = pion) const;

       /** 9) <b>Configured AlgTool extrapolation method</b>:
        - Extrapolate to a destination surface, while collecting all the material layers in between.
       */
       virtual std::vector<const TrackStateOnSurface*>* extrapolateM(const TrackParameters& parameters,
                                                                     const Surface& sf,
                                                                     PropDirection dir,
                                                                     const BoundaryCheck& bcheck,
                                                                     ParticleHypothesis particle = pion,
                                                                     Trk::ExtrapolationCache* cache = nullptr) const;

       /** 10) <b>Configured AlgTool extrapolation method</b>:
        - Extrapolate to a destination surface, while collecting all the material layers and transport jacobians in
        between.
       */
       virtual std::vector<const TrackParameters*>* extrapolateM(const TrackParameters& parameters,
                                                                 const Surface& sf,
                                                                 PropDirection dir,
                                                                 const BoundaryCheck& bcheck,
                                                                 std::vector<MaterialEffectsOnTrack>& material,
                                                                 std::vector<Trk::TransportJacobian*>& jacs,
                                                                 ParticleHypothesis particle = pion,
                                                                 Trk::ExtrapolationCache* cache = nullptr) const;

       virtual const Trk::TrackParameters* extrapolateWithPathLimit(
         const Trk::TrackParameters& parm,
         double& pathLim,
         Trk::PropDirection dir,
         Trk::ParticleHypothesis particle,
         std::vector<const Trk::TrackParameters*>*& parmOnSf,
         std::vector<const Trk::TrackStateOnSurface*>*& material,
         const Trk::TrackingVolume* boundaryVol = 0,
         MaterialUpdateMode matupmod = Trk::addNoise) const;

       /** extrapolation method collecting intersections with subdetector boundaries and active volumes/layers.
           A primitive identification is provided - to be replaced with appropriate identifier, and possibly merged
           with TrackParameters. Material collection in option. Destination (subdetector boundary) : geoID (exit)
       */
       virtual const std::vector<std::pair<const Trk::TrackParameters*, int>>* extrapolate(
         const Trk::TrackParameters& parm,
         Trk::PropDirection dir,
         Trk::ParticleHypothesis particle,
         std::vector<const Trk::TrackStateOnSurface*>*& material,
         int destination = 3) const;

      
  };


} // end of namespace


#include "TrkExInterfaces/IExtrapolator.icc"


#endif // TRKEXINTERFACES_ITRKEXTRAPOLATOR_H

