/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPropagator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IPROPAGATOR_H
#define TRKEXINTERFACES_IPROPAGATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"

// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkExUtils/ExtrapolationCache.h"
#include "TrkExUtils/TargetSurfaces.h"
#include "HelperStructs.h"
// STL
#include <utility>

namespace Trk {

  class Surface;
  class CylinderBounds;
  class MagneticFieldProperties;
  class IntersectionSolution;
  class TransportJacobian;     
  class TrackStateOnSurface;
  
  /** typedef for return type TrackParameters, pathlength */
  typedef std::pair< const TrackParameters*, double >  TrackParametersWithPath;

  /** typedef for input surfaces, boundary check */
  typedef std::pair< const Surface*, BoundaryCheck >  DestSurf;


  /** Interface ID for IPropagators*/  
  static const InterfaceID IID_IPropagator("IPropagator", 1, 0);
  
  /** @class IPropagator
    Interface class IPropagators
    It inherits from IAlgTool.


    */
  class IPropagator : virtual public IAlgTool {
      public:

       /**Virtual destructor*/
       virtual ~IPropagator(){}
       
       /** AlgTool and IAlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IPropagator; }

       /** [NeutralParameters] ------------------------------------------------------------- */

       /** N 0) <b>Neutral parameters method </b> 
           - returns a ParametersBase object as well, 0 if the extrapolation did not succeed 
         */

       virtual NeutralParameters* propagate(const NeutralParameters& parameters,
                                            const Surface& sf,
                                            PropDirection dir,
                                            const BoundaryCheck& bcheck,
                                            bool returnCurv = false) const = 0;

       /** N 0) <b>Neutral parameters method </b> 
           - symmetric interface for new Extrapolation engine
         */

       NeutralParameters* propagate(const NeutralParameters& parameters,
                                    const Surface& sf,
                                    PropDirection dir,
                                    const BoundaryCheck& bcheck,
                                    const MagneticFieldProperties&,
                                    ParticleHypothesis,
                                    bool returnCurv,
                                    const TrackingVolume* tVol=nullptr) const
       { //avoid warning for tVol
          return propagate(parameters,sf,dir,bcheck,returnCurv);
          if (tVol) return nullptr;
        }

       /** [TrackParameters] new interfaces WITH EventContext ------------------------------------------ */

       /** Propagation interface:
         The propagation method called by the TrkExtrapolator. The extrapolator
         is responsible for the underlying logic of which surface to go to.
         */
       virtual TrackParameters*  propagate(  const EventContext&            ctx,
                                             const TrackParameters&         parm,
                                             const Surface&                 sf,
                                             PropDirection                  dir,
                                             const BoundaryCheck&           bcheck,
                                             const MagneticFieldProperties& mprop,
                                             ParticleHypothesis             particle   = pion,
                                             bool                           returnCurv = false,
                                             const TrackingVolume*          tVol       = nullptr) const = 0;
       

       /** Propagation interface:
         
         The propagation method called by the TrkExtrapolator. The propagator
         finds the closest surface.
         */
       virtual  TrackParameters* propagate( const EventContext&            ctx,
                                            const TrackParameters&         parm,
                                            std::vector<DestSurf>&         sfs,
                                            PropDirection                  dir,
                                            const MagneticFieldProperties& mprop,
                                            ParticleHypothesis             particle,
                                            std::vector<unsigned int>&     solutions,
                                            double&                        path,
                                            bool                           usePathLim = false,
                                            bool                           returnCurv = false,
                                            const TrackingVolume*          tVol       = nullptr) const = 0;
       
       /** Propagation interface:
         
         The propagation method called by the TrkExtrapolator. The propagator
         finds the closest surface. Timing included.
         */
       virtual TrackParameters* propagateT( const EventContext&            ctx,
                                            const TrackParameters&         parm,
                                            std::vector<DestSurf>&         sfs,
                                            PropDirection                  dir,
                                            const MagneticFieldProperties& mprop,
                                            ParticleHypothesis             particle,
                                            std::vector<unsigned int>&     solutions,
                                            PathLimit&                     pathLim, TimeLimit& timeLim,
                                            bool                           returnCurv ,
                                            const TrackingVolume*          tVol,
                                            std::vector<Trk::HitInfo>*&    hitVector) const;
         
       /** Propagation interface:
         
         The propagation method called by the TrkExtrapolator. The propagator
         finds the closest surface. Timing included.
         */
       virtual TrackParameters* propagateT( const EventContext&            ctx,
                                            const TrackParameters&         parm,
                                            TargetSurfaces&                sfs,
                                            PropDirection                  dir,
                                            const MagneticFieldProperties& mprop,
                                            ParticleHypothesis             particle,
                                            TargetSurfaceVector&           solutions,
                                            PathLimit&                     pathLim, TimeLimit& timeLim,
                                            bool                           returnCurv ,
                                            std::vector<Trk::HitInfo>*&    hitVector) const;
         

       /** Propagation interface:
         
         The propagation method called by the TrkExEngine. All options included.
         */
      virtual Trk::ExtrapolationCode propagate( const EventContext&       ctx,
                                                Trk::ExCellCharged&       eCell,
                                                Trk::TargetSurfaces&      sfs,
                                                Trk::TargetSurfaceVector& solutions) const;         
       /** Propagation interface:
         
         The propagation method with internal material collection. The propagator
         finds the closest surface. 
         */
      virtual  TrackParameters* propagateM( const EventContext&                                        ctx,
                                            const TrackParameters&                                     parm,
                                            std::vector<DestSurf>&                                     sfs,
                                            PropDirection                                              dir,
                                            const MagneticFieldProperties&                             mprop,
                                            ParticleHypothesis                                         particle,
                                            std::vector<unsigned int>&                                 solutions,
                                            std::vector<const Trk::TrackStateOnSurface*>*&             matstates,
                                            std::vector<std::pair<const Trk::TrackParameters*,int> >*& intersections, 
                                            double&                                                    path,
                                            bool                                                       usePathLim = false,
                                            bool                                                       returnCurv = false,
                                            const TrackingVolume*                                      tVol       = nullptr,
                                            Trk::ExtrapolationCache*                                   cache      = nullptr) const;
         

       /** Propagation interface:
         
         The propagation method including the return of the TransportJacobian matrix.

         */
       virtual TrackParameters*      propagate( const EventContext&            ctx,
                                                const TrackParameters&         parm,
                                                const Surface&                 sf,
                                                PropDirection                  dir,
                                                const BoundaryCheck&           bcheck,
                                                const MagneticFieldProperties& mprop,
                                                TransportJacobian*&            jacob,
                                                double&                        pathLength,
                                                ParticleHypothesis             particle   = pion,
                                                bool                           returnCurv = false,
                                                const TrackingVolume*          tVol       = nullptr) const = 0;
       

       /** Propagation interface without Covariance matrix propagation
         the pathlength has to be returned for eventual following propagateCovariance
         */

       virtual TrackParameters*      propagateParameters( const EventContext&            ctx,
                                                          const TrackParameters&         parm,
                                                          const Surface&                 sf,
                                                          PropDirection                  dir,
                                                          const BoundaryCheck&           bcheck,
                                                          const MagneticFieldProperties& mprop,
                                                          ParticleHypothesis             particle=pion,
                                                          bool                           returnCurv = false,
                                                          const TrackingVolume*          tVol=nullptr) const = 0;
       
       virtual TrackParameters*      propagateParameters( const EventContext&            ctx,
                                                          const TrackParameters&         parm,
                                                          const Surface&                 sf,
                                                          PropDirection                  dir,
                                                          const BoundaryCheck&           bcheck,
                                                          const MagneticFieldProperties& mprop,
                                                          TransportJacobian*&            jacob,
                                                          ParticleHypothesis             particle=pion,
                                                          bool                           returnCurv = false,
                                                          const TrackingVolume*          tVol=nullptr) const = 0;
       

       /** Intersection interface:
         
          The intersection interface might be used by the material service as well to estimate
          the surfaces (sensitive and nonesensitive) while propagation
         */
       virtual const IntersectionSolution* intersect( const EventContext&            ctx,
                                                      const TrackParameters&         parm,
                                                      const Surface&                 sf,
                                                      const MagneticFieldProperties& mprop,
                                                      ParticleHypothesis             particle=pion,
                                                      const TrackingVolume*          tVol=nullptr) const = 0;

      /** Intersection and Intersector interface: 
        */

      virtual const TrackSurfaceIntersection* intersectSurface(const EventContext&             ctx,
                                                               const Surface&                  surface,
                                                               const TrackSurfaceIntersection* trackIntersection,
                                                               const double                    qOverP,
                                                               const MagneticFieldProperties&  mft,
                                                               ParticleHypothesis              particle) const = 0;                        
 

      /** GlobalPositions list interface:
         This is used mostly in pattern recognition in the road finder, the propagation direction is intrinsically given
         by the sign of the stepSize.
         
         To avoid memory fragmentation in multiple use of pattern recognition processes and respecting the possible iterative
         filling of the positions list, the list of GlobalPositions is given by reference through the signature and a void 
         method has been chosen.
         */
       virtual void globalPositions(const EventContext&            ctx,
                                    std::list<Amg::Vector3D>&      positionslist, 
                                    const TrackParameters&         parm,
                                    const MagneticFieldProperties& mprop,
                                    const CylinderBounds&          cylbo,
                                    double                         stepSize,
                                    ParticleHypothesis             particle=pion,
                                    const TrackingVolume*          tVol=nullptr) const = 0;

     /** a very simple propagation along a given path length */
     virtual void propagateStep(const EventContext&            ctx,
                                const Amg::Vector3D&           inputPosition, 
                                const Amg::Vector3D&           inputMomentum, 
                                double                         charge, 
                                double                         step,
                                Amg::Vector3D&                 outputPosition, 
                                Amg::Vector3D&                 outputMomentum,
                                const MagneticFieldProperties& mprop) const;


     /** Validation Action:
       Can be implemented optionally, outside access to internal validation steps */
     virtual void validationAction() const {}


       /** [TrackParameters] old interfaces WITHOUT EventContext ------------------------------------------ */

       /** Propagation interface:
         The propagation method called by the TrkExtrapolator. The extrapolator
         is responsible for the underlying logic of which surface to go to.
         */
       virtual TrackParameters*  propagate( const TrackParameters&         parm,
                                            const Surface&                 sf,
                                            PropDirection                  dir,
                                            const BoundaryCheck&           bcheck,
                                            const MagneticFieldProperties& mprop,
                                            ParticleHypothesis             particle   = pion,
                                            bool                           returnCurv = false,
                                            const TrackingVolume*          tVol       = nullptr) const;
       

       /** Propagation interface:
         
         The propagation method called by the TrkExtrapolator. The propagator
         finds the closest surface.
         */
       virtual  TrackParameters* propagate( const TrackParameters&         parm,
                                            std::vector<DestSurf>&         sfs,
                                            PropDirection                  dir,
                                            const MagneticFieldProperties& mprop,
                                            ParticleHypothesis             particle,
                                            std::vector<unsigned int>&     solutions,
                                            double&                        path,
                                            bool                           usePathLim = false,
                                            bool                           returnCurv = false,
                                            const TrackingVolume*          tVol       = nullptr) const;
       
       /** Propagation interface:
         
         The propagation method called by the TrkExtrapolator. The propagator
         finds the closest surface. Timing included.
         */
       virtual TrackParameters* propagateT( const TrackParameters&         parm,
                                            std::vector<DestSurf>&         sfs,
                                            PropDirection                  dir,
                                            const MagneticFieldProperties& mprop,
                                            ParticleHypothesis             particle,
                                            std::vector<unsigned int>&     solutions,
                                            PathLimit&                     pathLim, TimeLimit& timeLim,
                                            bool                           returnCurv ,
                                            const TrackingVolume*          tVol,
                                            std::vector<Trk::HitInfo>*& hitVector) const;
         
       /** Propagation interface:
         
         The propagation method called by the TrkExtrapolator. The propagator
         finds the closest surface. Timing included.

         */
       virtual TrackParameters* propagateT( const TrackParameters&         parm,
                                            TargetSurfaces&                sfs,
                                            PropDirection                  dir,
                                            const MagneticFieldProperties& mprop,
                                            ParticleHypothesis             particle,
                                            TargetSurfaceVector&           solutions,
                                            PathLimit&                     pathLim, TimeLimit& timeLim,
                                            bool                           returnCurv ,
                                            std::vector<Trk::HitInfo>*& hitVector) const;
         

       /** Propagation interface:
         
         The propagation method called by the TrkExEngine. All options included.
         */
       virtual Trk::ExtrapolationCode propagate( Trk::ExCellCharged&  eCell,
                                                 Trk::TargetSurfaces& sfs,
                                                 Trk::TargetSurfaceVector& solutions) const;         
       /** Propagation interface:
         
         The propagation method with internal material collection. The propagator
         finds the closest surface. 
         */
       virtual  TrackParameters* propagateM( const TrackParameters&                         parm,
                                             std::vector<DestSurf>&                         sfs,
                                             PropDirection                                  dir,
                                             const MagneticFieldProperties&                 mprop,
                                             ParticleHypothesis                             particle,
                                             std::vector<unsigned int>&                     solutions,
                                             std::vector<const Trk::TrackStateOnSurface*>*& matstates,
                                             std::vector<std::pair<const Trk::TrackParameters*,int> >*& intersections, 
                                             double&                                        path,
                                             bool                                           usePathLim = false,
                                             bool                                           returnCurv = false,
                                             const TrackingVolume*                          tVol       = nullptr,
                                             Trk::ExtrapolationCache*                       cache      = nullptr) const;
         

       /** Propagation interface:
         
         The propagation method including the return of the TransportJacobian matrix.

         */
       virtual TrackParameters*      propagate( const TrackParameters&         parm,
                                                const Surface&                 sf,
                                                PropDirection                  dir,
                                                const BoundaryCheck&           bcheck,
                                                const MagneticFieldProperties& mprop,
                                                TransportJacobian*&,
                                                double&                        pathLength,
                                                ParticleHypothesis             particle   = pion,
                                                bool                           returnCurv = false,
                                                const TrackingVolume*          tVol       = nullptr) const;
       

       /** Propagation interface without Covariance matrix propagation
         the pathlength has to be returned for eventual following propagateCovariance
         */

       virtual TrackParameters*      propagateParameters( const TrackParameters&         parm,
                                                          const Surface&                 sf,
                                                          PropDirection                  dir,
                                                          const BoundaryCheck&           bcheck,
                                                          const MagneticFieldProperties& mprop,
                                                          ParticleHypothesis             particle   = pion,
                                                          bool                           returnCurv = false,
                                                          const TrackingVolume*          tVol=nullptr) const;
       
       virtual TrackParameters*      propagateParameters( const TrackParameters&         parm,
                                                          const Surface&                 sf,
                                                          PropDirection                  dir,
                                                          const BoundaryCheck&           bcheck,
                                                          const MagneticFieldProperties& mprop,
                                                          TransportJacobian*&,
                                                          ParticleHypothesis             particle   = pion,
                                                          bool                           returnCurv = false,
                                                          const TrackingVolume*          tVol       = nullptr) const;
       

       /** Intersection interface:
         
          The intersection interface might be used by the material service as well to estimate
          the surfaces (sensitive and nonesensitive) while propagation
         */
       virtual const IntersectionSolution* intersect( const TrackParameters&         parm,
                                                      const Surface&                 sf,
                                                      const MagneticFieldProperties& mprop,
                                                      ParticleHypothesis             particle = pion,
                                                      const TrackingVolume*          tVol     = nullptr) const;

      /** Intersection and Intersector interface: 
        */

       virtual const TrackSurfaceIntersection* intersectSurface(const Surface&                  surface,
                                                                const TrackSurfaceIntersection* trackIntersection,
                                                                const double                    qOverP,
                                                                const MagneticFieldProperties&  mft,
                                                                ParticleHypothesis              particle) const;                        
 

      /** GlobalPositions list interface:
         This is used mostly in pattern recognition in the road finder, the propagation direction is intrinsically given
         by the sign of the stepSize.
         
         To avoid memory fragmentation in multiple use of pattern recognition processes and respecting the possible iterative
         filling of the positions list, the list of GlobalPositions is given by reference through the signature and a void 
         method has been chosen.
         */
       virtual void globalPositions(std::list<Amg::Vector3D>&      positionslist, 
                                    const TrackParameters&         parm,
                                    const MagneticFieldProperties& mprop,
                                    const CylinderBounds&          cylbo,
                                    double                         stepSize,
                                    ParticleHypothesis             particle = pion,
                                    const TrackingVolume*          tVol     = nullptr) const;

     /** a very simple propagation along a given path length */
     virtual void propagateStep(const Amg::Vector3D& inputPosition, 
                                const Amg::Vector3D& inputMomentum, 
                                double               charge, 
                                double               step,
                                Amg::Vector3D&       outputPosition, 
                                Amg::Vector3D&       outputMomentum,
                                const MagneticFieldProperties& mprop) const;


  };

} // end of namespace

// default methods for some methods which will not always be implemented

inline  Trk::TrackParameters* Trk::IPropagator::propagate( const EventContext&,
                                                           const TrackParameters&,
                                                           std::vector<DestSurf>&,
                                                           PropDirection ,
                                                           const MagneticFieldProperties&,
                                                           ParticleHypothesis,
                                                           std::vector<unsigned int>&,
                                                           double&,
                                                           bool,
                                                           bool,
                                                           const Trk::TrackingVolume*) const
{                  
    return nullptr;
}

inline Trk::TrackParameters* Trk::IPropagator::propagateT( const EventContext&,
                                                           const TrackParameters&,
                                                           std::vector<DestSurf>&,
                                                           PropDirection ,
                                                           const MagneticFieldProperties&,
                                                           ParticleHypothesis,
                                                           std::vector<unsigned int>&,
                                                           PathLimit&, TimeLimit&,
                                                           bool,const Trk::TrackingVolume*,
                                                           std::vector<Trk::HitInfo>*&) const
{                  
  return nullptr;
}

inline  Trk::TrackParameters* Trk::IPropagator::propagateT( const EventContext&,
                                                            const TrackParameters& ,
                                                            Trk::TargetSurfaces& ,
                                                            PropDirection ,
                                                            const MagneticFieldProperties& ,
                                                            ParticleHypothesis ,
                                                            Trk::TargetSurfaceVector& ,
                                                            PathLimit& , TimeLimit& ,
                                                            bool,std::vector<Trk::HitInfo>*& ) const
{
    return nullptr;
}

inline Trk::ExtrapolationCode Trk::IPropagator::propagate( const EventContext&,
                                                           Trk::ExCellCharged& ,
                                                           Trk::TargetSurfaces&,
                                                           Trk::TargetSurfaceVector& ) const
{
    return Trk::ExtrapolationCode::FailureConfiguration;
}         

inline  Trk::TrackParameters* Trk::IPropagator::propagateM( const EventContext&,
                                                            const TrackParameters&,
                                                            std::vector<DestSurf>&,
                                                            PropDirection ,
                                                            const MagneticFieldProperties&,
                                                            ParticleHypothesis,
                                                            std::vector<unsigned int>&,
                                                            std::vector<const Trk::TrackStateOnSurface*>*&,
                                                            std::vector<std::pair<const Trk::TrackParameters*,int> >*& , 
                                                            double&,
                                                            bool,
                                                            bool,
                                                            const Trk::TrackingVolume*,                                                          
                                                            Trk::ExtrapolationCache*) const
{                  
    return nullptr;
}


/** a very simple propagation along a given path length */
inline void Trk::IPropagator::propagateStep(const EventContext&,
                                            const Amg::Vector3D&, 
                                            const Amg::Vector3D&, 
                                            double, 
                                            double, 
                                            Amg::Vector3D&, 
                                            Amg::Vector3D&, 
                                            const MagneticFieldProperties&) const {}



/** [TrackParameters] new interfaces WITH EventContext ------------------------------------------ */
/** temporarily default to old interfaces, taking the EventContext from GaudiHive */

//  Propagation interface:
//    The propagation method called by the TrkExtrapolator. The extrapolator
//    is responsible for the underlying logic of which surface to go to.

inline Trk::TrackParameters*
Trk::IPropagator::propagate(  const Trk::TrackParameters&         parm,
                              const Trk::Surface&                 sf,
                              Trk::PropDirection                  dir,
                              const BoundaryCheck&                bcheck,
                              const Trk::MagneticFieldProperties& mprop,
                              Trk::ParticleHypothesis             particle,
                              bool                                returnCurv,
                              const Trk::TrackingVolume*          tVol) const
{
    return propagate( Gaudi::Hive::currentContext(),
                      parm,
                      sf,
                      dir,
                      bcheck,
                      mprop,
                      particle,
                      returnCurv,
                      tVol);
}

        

/** Propagation interface:
         
    The propagation method called by the TrkExtrapolator. The propagator
    finds the closest surface.
*/
inline  Trk::TrackParameters*
Trk::IPropagator::propagate( const Trk::TrackParameters&         parm,
                             std::vector<Trk::DestSurf>&         sfs,
                             Trk::PropDirection                  dir,
                             const Trk::MagneticFieldProperties& mprop,
                             Trk::ParticleHypothesis             particle,
                             std::vector<unsigned int>&          solutions,
                             double&                             path,
                             bool                                usePathLim,
                             bool                                returnCurv,
                             const Trk::TrackingVolume*          tVol) const
{
    
    return propagate( Gaudi::Hive::currentContext(),
                      parm,
                      sfs,
                      dir,
                      mprop,
                      particle,
                      solutions,
                      path,
                      usePathLim,
                      returnCurv,
                      tVol);
}


/** Propagation interface:
         
    The propagation method called by the TrkExtrapolator. The propagator
    finds the closest surface. Timing included.
*/
inline Trk::TrackParameters*
Trk::IPropagator::propagateT( const Trk::TrackParameters&         parm,
                              std::vector<Trk::DestSurf>&         sfs,
                              Trk::PropDirection                  dir,
                              const Trk::MagneticFieldProperties& mprop,
                              Trk::ParticleHypothesis             particle,
                              std::vector<unsigned int>&          solutions,
                              PathLimit&                          pathLim, TimeLimit& timeLim,
                              bool                                returnCurv ,
                              const Trk::TrackingVolume*          tVol,
                              std::vector<Trk::HitInfo>*&         hitVector) const
{
    
    return  propagateT( Gaudi::Hive::currentContext(),
                        parm,
                        sfs,
                        dir,
                        mprop,
                        particle,
                        solutions,
                        pathLim, timeLim,
                        returnCurv ,
                        tVol,
                        hitVector);
}


/** Propagation interface:
         
    The propagation method called by the TrkExtrapolator. The propagator
    finds the closest surface. Timing included.
*/
inline Trk::TrackParameters*
Trk::IPropagator::propagateT( const Trk::TrackParameters&         parm,
                              Trk::TargetSurfaces&                sfs,
                              Trk::PropDirection                  dir,
                              const Trk::MagneticFieldProperties& mprop,
                              Trk::ParticleHypothesis             particle,
                              TargetSurfaceVector&                solutions,
                              PathLimit&                          pathLim, TimeLimit& timeLim,
                              bool                                returnCurv ,
                              std::vector<Trk::HitInfo>*&         hitVector) const
{
    return propagateT( Gaudi::Hive::currentContext(),
                       parm,
                       sfs,
                       dir,
                       mprop,
                       particle,
                       solutions,
                       pathLim, timeLim,
                       returnCurv ,
                       hitVector);
}

/** Propagation interface:
         
    The propagation method called by the TrkExEngine. All options included.
*/
inline Trk::ExtrapolationCode
Trk::IPropagator::propagate( Trk::ExCellCharged&  eCell,
                             Trk::TargetSurfaces& sfs,
                             Trk::TargetSurfaceVector& solutions) const
{
    return  propagate( Gaudi::Hive::currentContext(),
                       eCell,
                       sfs,
                       solutions);         
}

/** Propagation interface:
         
    The propagation method with internal material collection. The propagator
    finds the closest surface. 
*/
inline  Trk::TrackParameters*
Trk::IPropagator::propagateM( const Trk::TrackParameters&         parm,
                              std::vector<Trk::DestSurf>&         sfs,
                              Trk::PropDirection                  dir,
                              const Trk::MagneticFieldProperties& mprop,
                              Trk::ParticleHypothesis             particle,
                              std::vector<unsigned int>&          solutions,
                              std::vector<const Trk::TrackStateOnSurface*>*& matstates,
                              std::vector<std::pair<const Trk::TrackParameters*,int> >*& intersections, 
                              double&                             path,
                              bool                                usePathLim,
                              bool                                returnCurv,
                              const Trk::TrackingVolume*          tVol,
                              Trk::ExtrapolationCache*            cache) const
{
    return propagateM( Gaudi::Hive::currentContext(),
                       parm,
                       sfs,
                       dir,
                       mprop,
                       particle,
                       solutions,
                       matstates,
                       intersections, 
                       path,
                       usePathLim,
                       returnCurv,
                       tVol,
                       cache);
}

         

/** Propagation interface:
         
    The propagation method including the return of the TransportJacobian matrix.

*/
inline Trk::TrackParameters*
Trk::IPropagator::propagate( const Trk::TrackParameters&         parm,
                             const Trk::Surface&                 sf,
                             Trk::PropDirection                  dir,
                             const BoundaryCheck&                bcheck,
                             const Trk::MagneticFieldProperties& mprop,
                             TransportJacobian*&                 jacob,
                             double&                             pathLength,
                             Trk::ParticleHypothesis             particle,
                             bool                                returnCurv,
                             const Trk::TrackingVolume*          tVol) const
{
    return propagate( Gaudi::Hive::currentContext(),
                      parm,
                      sf,
                      dir,
                      bcheck,
                      mprop,
                      jacob,
                      pathLength,
                      particle,
                      returnCurv,
                      tVol);
}

       

/** Propagation interface without Covariance matrix propagation
    the pathlength has to be returned for eventual following propagateCovariance
*/

inline Trk::TrackParameters*
Trk::IPropagator::propagateParameters( const Trk::TrackParameters&         parm,
                                       const Trk::Surface&                 sf,
                                       Trk::PropDirection                  dir,
                                       const BoundaryCheck&                bcheck,
                                       const Trk::MagneticFieldProperties& mprop,
                                       Trk::ParticleHypothesis             particle,
                                       bool                                returnCurv,
                                       const Trk::TrackingVolume*          tVol) const
{
    return propagateParameters( Gaudi::Hive::currentContext(),
                                parm,
                                sf,
                                dir,
                                bcheck,
                                mprop,
                                particle=pion,
                                returnCurv,
                                tVol);
}

       
inline Trk::TrackParameters*
Trk::IPropagator::propagateParameters( const Trk::TrackParameters&         parm,
                                       const Trk::Surface&                 sf,
                                       Trk::PropDirection                  dir,
                                       const BoundaryCheck&                bcheck,
                                       const Trk::MagneticFieldProperties& mprop,
                                       TransportJacobian*&                 jacob,
                                       Trk::ParticleHypothesis             particle,
                                       bool                                returnCurv,
                                       const Trk::TrackingVolume*          tVol) const
{
    return propagateParameters( Gaudi::Hive::currentContext(),
                                parm,
                                sf,
                                dir,
                                bcheck,
                                mprop,
                                jacob,
                                particle=pion,
                                returnCurv,
                                tVol);
}

       

/** Intersection interface:
         
    The intersection interface might be used by the material service as well to estimate
    the surfaces (sensitive and nonesensitive) while propagation
*/
inline const Trk::IntersectionSolution*
Trk::IPropagator::intersect( const Trk::TrackParameters&         parm,
                             const Trk::Surface&                 sf,
                             const Trk::MagneticFieldProperties& mprop,
                             Trk::ParticleHypothesis             particle,
                             const Trk::TrackingVolume*          tVol) const
{
    return intersect( Gaudi::Hive::currentContext(),
                      parm,
                      sf,
                      mprop,
                      particle=pion,
                      tVol);
}


/** Intersection and Intersector interface: 
 */

inline const Trk::TrackSurfaceIntersection*
Trk::IPropagator::intersectSurface(const Trk::Surface&                 surface,
                                   const TrackSurfaceIntersection*     trackIntersection,
                                   const double                        qOverP,
                                   const Trk::MagneticFieldProperties& mft,
                                   Trk::ParticleHypothesis             particle) const
{
    return intersectSurface(Gaudi::Hive::currentContext(),
                            surface,
                            trackIntersection,
                            qOverP,
                            mft,
                            particle);
}

 

/** GlobalPositions list interface:
    This is used mostly in pattern recognition in the road finder, the propagation direction is intrinsically given
    by the sign of the stepSize.
         
    To avoid memory fragmentation in multiple use of pattern recognition processes and respecting the possible iterative
    filling of the positions list, the list of GlobalPositions is given by reference through the signature and a void 
    method has been chosen.
*/
inline void
Trk::IPropagator::globalPositions(std::list<Amg::Vector3D>&           positionslist, 
                                  const Trk::TrackParameters&         parm,
                                  const Trk::MagneticFieldProperties& mprop,
                                  const CylinderBounds&               cylbo,
                                  double                              stepSize,
                                  Trk::ParticleHypothesis             particle,
                                  const Trk::TrackingVolume*          tVol) const
{
    globalPositions(Gaudi::Hive::currentContext(),
                    positionslist, 
                    parm,
                    mprop,
                    cylbo,
                    stepSize,
                    particle=pion,
                    tVol);
}


/** a very simple propagation along a given path length */
inline void
Trk::IPropagator::propagateStep(const Amg::Vector3D& inputPosition, 
                                const Amg::Vector3D& inputMomentum, 
                                double               charge, 
                                double               step,
                                Amg::Vector3D&       outputPosition, 
                                Amg::Vector3D&       outputMomentum,
                                const Trk::MagneticFieldProperties& mprop) const
{
    propagateStep(Gaudi::Hive::currentContext(),
                  inputPosition, 
                  inputMomentum, 
                  charge, 
                  step,
                  outputPosition, 
                  outputMomentum,
                  mprop);
}



#endif // TRKEXINTERFACES_PROPAGATOR_H


