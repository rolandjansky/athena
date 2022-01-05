/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class  RungeKuttaPropagator, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////

#ifndef RungeKuttaPropagator_H
#define RungeKuttaPropagator_H

#include <list>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"

// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

//
#include "CxxUtils/restrict.h"

namespace Trk {

    class Surface                  ;
    class MagneticFieldProperties  ;
    class CylinderBounds           ;
    class CylinderSurface          ;
    class PatternTrackParameters   ;

/**
   @class RungeKuttaPropagator

   Trk::RungeKuttaPropagator is algorithm for track parameters propagation through
   magnetic field with or without jacobian of transformation. This algorithm
   contains three steps.

   1.The first step of the algorithm is track parameters transformation from
   local presentation for given start surface to global Runge Kutta coordinates.

   2.The second step is propagation through magnetic field with or without jacobian.

   3.Third step is transformation from global Runge Kutta presentation to local
   presentation of given output surface.


   AtaPlane    AtaStraightLine      AtaDisc       AtaCylinder      Perigee
   |               |               |               |              |
   |               |               |               |              |
   V               V               V               V              V
   -----------------------------------------------------------------
   |              Local->Global transformation
   V
   Global position (Runge Kutta presentation)
   |
   |
   Propagation to next surface with or without jacobian
   using Nystroem algorithm
   (See Handbook Net. Bur. of Standards, procedure 25.5.20)
   |
   V              Global->Local transformation
   ----------------------------------------------------------------
   |               |               |               |              |
   |               |               |               |              |
   V               V               V               V              V
   PlaneSurface StraightLineSurface DiscSurface CylinderSurface PerigeeSurface

   For propagation using Runge Kutta method we use global coordinate, direction,
   inverse momentum and Jacobian of transformation. All this parameters we save
   in array P[42].
   /dL0    /dL1    /dPhi   /dThe   /dCM
   X  ->P[0]  dX /   P[ 7]   P[14]   P[21]   P[28]   P[35]
   Y  ->P[1]  dY /   P[ 8]   P[15]   P[22]   P[29]   P[36]
   Z  ->P[2]  dZ /   P[ 9]   P[16]   P[23]   P[30]   P[37]
   Ax ->P[3]  dAx/   P[10]   P[17]   P[24]   P[31]   P[38]
   Ay ->P[4]  dAy/   P[11]   P[18]   P[25]   P[32]   P[39]
   Az ->P[5]  dAz/   P[12]   P[19]   P[26]   P[33]   P[40]
   CM ->P[6]  dCM/   P[13]   P[20]   P[27]   P[34]   P[41]

   where
   in case local presentation

   L0  - first  local coordinate  (surface dependent)
   L1  - second local coordinate  (surface dependent)
   Phi - Azimuthal angle
   The - Polar     angle
   CM  - charge/momentum

   in case global presentation

   X   - global x-coordinate        = surface dependent
   Y   - global y-coordinate        = surface dependent
   Z   - global z-coordinate        = sutface dependent
   Ax  - direction cosine to x-axis = Sin(The)*Cos(Phi)
   Ay  - direction cosine to y-axis = Sin(The)*Sin(Phi)
   Az  - direction cosine to z-axis = Cos(The)
   CM  - charge/momentum            = local CM

   Comment:
   if pointer to const *  = 0 algorithm will propagate track
   parameters and jacobian of transformation according straight line model

   @author Igor.Gavrilenko@cern.ch

   @authors AthenaMT modifications RD Schaffer C Anastopoulos
*/

    class RungeKuttaPropagator final
      : public AthAlgTool
      , virtual public IPropagator
      , virtual public IPatternParametersPropagator
    {
        /////////////////////////////////////////////////////////////////////////////////
        // Public methods:
        /////////////////////////////////////////////////////////////////////////////////

    public:

        using IPropagator::propagate;
        using IPropagator::propagateT;

        RungeKuttaPropagator(const std::string&,const std::string&,const IInterface*);

        virtual ~RungeKuttaPropagator ();

        /** AlgTool initailize method.*/
        virtual StatusCode initialize() override final;

        /** Main propagation method NeutralParameters */
        virtual std::unique_ptr<NeutralParameters> propagate
        (const NeutralParameters        &,
         const Surface                  &,
         PropDirection                   ,
         const BoundaryCheck            &,
         bool                            ) const override final;

        /** Main propagation method without transport jacobian production*/
        virtual  std::unique_ptr<TrackParameters>           propagate
        (const EventContext&          ctx,
         const TrackParameters          &,
         const Surface                  &,
         const PropDirection             ,
         const BoundaryCheck            &,
         const MagneticFieldProperties  &,
         ParticleHypothesis              ,
         bool                            ,
         const TrackingVolume*           ) const override final;

        /** Main propagation method with transport jacobian production*/
        virtual  std::unique_ptr<TrackParameters>           propagate
        (const EventContext&          ctx,
         const TrackParameters          &,
         const Surface                  &,
         const PropDirection             ,
         const BoundaryCheck            &,
         const MagneticFieldProperties  &,
         TransportJacobian             *&,
         double                         &,
         ParticleHypothesis              ,
         bool                            ,
         const TrackingVolume*            ) const override final;

        /** The propagation method finds the closest surface */
        virtual std::unique_ptr<TrackParameters>           propagate
        (const EventContext&          ctx,
         const TrackParameters         &,
         std::vector<DestSurf>         &,
         PropDirection                  ,
         const MagneticFieldProperties &,
         ParticleHypothesis             ,
         std::vector<unsigned int>     &,
         double                        &,
         bool                           ,
         bool                           ,
         const TrackingVolume*          ) const override final;

        /** Main propagation method for parameters only without transport jacobian productio*/
        virtual  std::unique_ptr<TrackParameters>           propagateParameters
        (const EventContext&          ctx,
         const TrackParameters          &,
         const Surface                  &,
         const PropDirection             ,
         const BoundaryCheck            &,
         const MagneticFieldProperties  &,
         ParticleHypothesis              ,
         bool                            ,
         const TrackingVolume*          ) const override final;


        /** Main propagation method for parameters only with transport jacobian productio*/
        virtual  std::unique_ptr<TrackParameters>           propagateParameters
        (const EventContext&          ctx,
         const TrackParameters          &,
         const Surface                  &,
         const PropDirection             ,
         const BoundaryCheck            &,
         const MagneticFieldProperties  &,
         TransportJacobian             *&,
         ParticleHypothesis              ,
         bool                            ,
         const TrackingVolume*           ) const override final;

        /** Global position together with direction of the trajectory on the surface */
        virtual const IntersectionSolution*      intersect
        (const EventContext&          ctx,
         const TrackParameters          &,
         const Surface                  &,
         const MagneticFieldProperties  &,
         ParticleHypothesis particle=pion,
         const TrackingVolume*   tvol=nullptr  ) const override final;

        /** GlobalPositions list interface:*/
        virtual void globalPositions
        (const EventContext&          ctx,
         std::list<Amg::Vector3D>       &,
         const TrackParameters          &,
         const MagneticFieldProperties  &,
         const CylinderBounds&           ,
         double                          ,
         ParticleHypothesis particle=pion,
         const TrackingVolume* tvol=nullptr    ) const override final;

        /////////////////////////////////////////////////////////////////////////////////
        // Public methods for Trk::PatternTrackParameters (from IPattern'Propagator)
        /////////////////////////////////////////////////////////////////////////////////

        /** Main propagation method */
        virtual bool propagate
        (const EventContext&          ctx,
         PatternTrackParameters         &,
         const Surface                  &,
         PatternTrackParameters         &,
         PropDirection                   ,
         const MagneticFieldProperties  &,
         ParticleHypothesis particle=pion) const  override final;

        /** Main propagation method with step to surface calculation*/
        virtual bool propagate
        (const EventContext&          ctx,
         PatternTrackParameters         &,
         const Surface                  &,
         PatternTrackParameters         &,
         PropDirection                   ,
         const MagneticFieldProperties  &,
         double                         &,
         ParticleHypothesis particle=pion) const  override final;

        /** Main propagation method for parameters only */
        virtual bool propagateParameters
        (const EventContext&          ctx,
         PatternTrackParameters         &,
         const Surface                  &,
         PatternTrackParameters         &,
         PropDirection                   ,
         const MagneticFieldProperties  &,
         ParticleHypothesis particle=pion) const  override final;

        /** Main propagation method for parameters only with step to surface calculation*/
        virtual bool propagateParameters
        (const EventContext&          ctx,
         PatternTrackParameters         &,
         const Surface                  &,
         PatternTrackParameters         &,
         PropDirection                   ,
         const MagneticFieldProperties  &,
         double                         &,
         ParticleHypothesis particle=pion) const  override final;

        /** GlobalPositions list interface:*/
        virtual void globalPositions
        (const EventContext&          ctx,
         std::list<Amg::Vector3D>       &,
         const PatternTrackParameters   &,
         const MagneticFieldProperties  &,
         const CylinderBounds           &,
         double                          ,
         ParticleHypothesis particle=pion) const  override final;

        /** GlobalPostions and steps for set surfaces */
        virtual void globalPositions
        (const EventContext&          ctx,
         const PatternTrackParameters                 &,
         std::list<const Surface*>                    &,
         std::list< std::pair<Amg::Vector3D,double> > &,
         const MagneticFieldProperties                &,
         ParticleHypothesis particle=pion              ) const  override final;

        struct Cache
        {
          MagField::AtlasFieldCache m_fieldCache;
          double m_field[3];
          double m_direction;
          double m_step;
          double m_maxPath = 10000;
          double m_dlt = .000200;
          double m_helixStep = 1;
          double m_straightStep = 0.01;
          bool m_maxPathLimit = false;
          bool m_mcondition = false;
          bool m_solenoid = true;
          bool m_needgradient = false;
          bool m_newfield = true;
        };

      private:

        /////////////////////////////////////////////////////////////////////////////////
        // Private methods:
        /////////////////////////////////////////////////////////////////////////////////
        /** Test quality Jacobian calculation */
        void JacobianTest
        (const TrackParameters        &,
         const Surface                &,
         const MagneticFieldProperties&) const;


        /** Internal RungeKutta propagation method for charge track parameters*/

        std::unique_ptr<TrackParameters>      propagateRungeKutta
        (Cache& cache                  ,
         bool                          ,
         const TrackParameters        &,
         const Surface                &,
         const PropDirection           ,
         const BoundaryCheck&          ,
         const MagneticFieldProperties&,
         double                       *,
         bool                returnCurv) const;

        /** Internal RungeKutta propagation method */

        bool propagateRungeKutta
        (Cache& cache                  ,
         bool                          ,
         PatternTrackParameters       &,
         const Surface                &,
         PatternTrackParameters       &,
         PropDirection                 ,
         const MagneticFieldProperties&,
         double                       &) const;

        void getFieldCacheObject(
          Cache& cache,
          const EventContext& ctx) const;

        // Private data members:
        // Read handle for conditions object to get the field cache
        SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey{
          this,
          "AtlasFieldCacheCondObj",
          "fieldCondObj",
          "Name of the Magnetic Field conditions object key"
        };

        double m_dlt                                               ;  // accuracy parameter
        double m_helixStep                                         ;  // max step whith helix model
        double m_straightStep                                      ;  // max step whith srtaight line model
        bool   m_usegradient                                       ;  // use magnetif field gradient
    };

    /////////////////////////////////////////////////////////////////////////////////
    // Inline methods for magnetic field information
    /////////////////////////////////////////////////////////////////////////////////

}//end namespace Trk
#endif // RungeKuttaPropagator_H
