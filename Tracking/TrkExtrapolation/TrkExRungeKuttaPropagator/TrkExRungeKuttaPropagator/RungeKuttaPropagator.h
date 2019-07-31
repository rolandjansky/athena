/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class  RungeKuttaPropagator, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////

#ifndef RungeKuttaPropagator_H
#define RungeKuttaPropagator_H

#include <list>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"

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
  */

  class RungeKuttaPropagator final: public AthAlgTool, virtual public IPropagator,
  virtual public IPatternParametersPropagator
{
  /////////////////////////////////////////////////////////////////////////////////
  // Public methods:
  /////////////////////////////////////////////////////////////////////////////////

public:

  RungeKuttaPropagator(const std::string&,const std::string&,const IInterface*);

  virtual ~RungeKuttaPropagator ();

  /** AlgTool initailize method.*/
  virtual StatusCode initialize() override final;

  /** AlgTool finalize method */
  virtual StatusCode finalize() override final;

  /** Main propagation mehtod NeutralParameters */

 virtual const NeutralParameters* propagate
    (const NeutralParameters        &,
     const Surface                  &,
     PropDirection                   ,
     BoundaryCheck                   ,
     bool                            ) const override final;

  /** Main propagation mehtod without transport jacobian production*/

  virtual const TrackParameters*           propagate
    (const TrackParameters          &,
     const Surface                  &,
     const PropDirection             ,
     BoundaryCheck                   ,
     const MagneticFieldProperties  &, 
     ParticleHypothesis              ,
     bool                            ,
     const TrackingVolume*           ) const override final;

  /** Main propagation mehtod with transport jacobian production*/

  virtual const TrackParameters*           propagate
    (const TrackParameters          &,
     const Surface                  &,
     const PropDirection             ,
     BoundaryCheck                   ,
     const MagneticFieldProperties  &, 
     TransportJacobian             *&,
     double                         &,
     ParticleHypothesis              ,
     bool                            ,
     const TrackingVolume*            ) const override final;

  /** The propagation method finds the closest surface */

  virtual const TrackParameters*           propagate
    (const TrackParameters         &,
     std::vector<DestSurf>         &,
     PropDirection                  ,
     const MagneticFieldProperties &,
     ParticleHypothesis             ,
     std::vector<unsigned int>     &,
     double                        &,
     bool                           ,
     bool                           ,
     const TrackingVolume*          ) const override final;

  /** Main propagation mehtod for parameters only without transport jacobian productio*/

  virtual const TrackParameters*           propagateParameters
    (const TrackParameters          &,
     const Surface                  &,
     const PropDirection             ,
     BoundaryCheck                   ,
     const MagneticFieldProperties  &, 
     ParticleHypothesis              ,
     bool                            ,
     const TrackingVolume*          ) const override final;


  /** Main propagation mehtod for parameters only with transport jacobian productio*/

  virtual const TrackParameters*           propagateParameters
    (const TrackParameters          &,
     const Surface                  &,
     const PropDirection             ,
     BoundaryCheck                   ,
     const MagneticFieldProperties  &, 
     TransportJacobian             *&,
     ParticleHypothesis              ,
     bool                            ,
     const TrackingVolume*           ) const override final;

  /** Global position together with direction of the trajectory on the surface */

  virtual const IntersectionSolution*      intersect
    (const TrackParameters          &,
     const Surface                  &,
     const MagneticFieldProperties  &, 
     ParticleHypothesis particle=pion, 
     const TrackingVolume*   tvol=0  ) const override final;

  /** GlobalPositions list interface:*/

  virtual void globalPositions
    (std::list<Amg::Vector3D>       &,
     const TrackParameters          &,
     const MagneticFieldProperties  &,
     const CylinderBounds&           ,
     double                          ,
     ParticleHypothesis particle=pion,
     const TrackingVolume* tvol=0    ) const override final;

 /////////////////////////////////////////////////////////////////////////////////
  // Public methods for Trk::PatternTrackParameters (from IPattern'Propagator)
  /////////////////////////////////////////////////////////////////////////////////

  /** Main propagation method */

  using IPropagator::propagate;
  virtual bool propagate
    (PatternTrackParameters         &,
     const Surface                  &,
     PatternTrackParameters         &,
     PropDirection                   ,
     const MagneticFieldProperties  &, 
     ParticleHypothesis particle=pion) const  override final;

  /** Main propagation mehtod with step to surface calculation*/

  virtual bool propagate
    (PatternTrackParameters         &,
     const Surface                  &,
     PatternTrackParameters         &,
     PropDirection                   ,
     const MagneticFieldProperties  &,
     double                         &,
     ParticleHypothesis particle=pion) const  override final;

  /** Main propagation mehtod for parameters only */

  virtual bool propagateParameters
    (PatternTrackParameters         &,
     const Surface                  &,
     PatternTrackParameters         &,
     PropDirection                   ,
     const MagneticFieldProperties  &,
     ParticleHypothesis particle=pion) const  override final;

  /** Main propagation mehtod for parameters only with step to surface calculation*/

  virtual bool propagateParameters
    (PatternTrackParameters         &,
     const Surface                  &,
     PatternTrackParameters         &,
     PropDirection                   ,
     const MagneticFieldProperties  &,
     double                         &,
     ParticleHypothesis particle=pion) const  override final;

  /** GlobalPositions list interface:*/

  virtual void globalPositions
    (std::list<Amg::Vector3D>       &,
     const PatternTrackParameters   &,
     const MagneticFieldProperties  &,
     const CylinderBounds           &,
     double                          ,
     ParticleHypothesis particle=pion) const  override final;

  /** GlobalPostions and steps for set surfaces */

  virtual void globalPositions
    (const PatternTrackParameters                 &,
     std::list<const Surface*>                    &,
     std::list< std::pair<Amg::Vector3D,double> > &,
     const MagneticFieldProperties                &,
     ParticleHypothesis particle=pion              ) const  override final;

  /** a very simple propagation along a given path length */

  virtual void propagateStep(const Amg::Vector3D& inputPosition, 
                             const Amg::Vector3D& inputMomentum, 
                             double charge, 
                             double step,
                             Amg::Vector3D& outputPosition, 
                             Amg::Vector3D& outputMomentum,
                             const MagneticFieldProperties& mprop) const override final;

private:

  struct Cache{
    double  m_direction                                ;
    double  m_step                                     ;
    double  m_maxPath                            =10000;
    double  m_field[3]                                 ;
    bool    m_maxPathLimit                       =false;
    bool    m_mcondition                         =false;
    bool    m_solenoid                            =true;
    bool    m_needgradient                       =false;  
    bool    m_newfield                            =true;
  };

  /////////////////////////////////////////////////////////////////////////////////
  // Private methods:
  /////////////////////////////////////////////////////////////////////////////////

  /** Test quality Jacobian calculation */

  void JacobianTest
    (const TrackParameters        &,
     const Surface                &,
     const MagneticFieldProperties&) const;

 
  /** Internal RungeKutta propagation method for charge track parameters*/   

  const TrackParameters*      propagateRungeKutta
    (Cache& cache                  ,
     bool                          ,
     const TrackParameters        &,
     const Surface                &,
     const PropDirection           ,
     BoundaryCheck                 ,
     const MagneticFieldProperties&,
     double                       *,
     bool                returnCurv) const;

  /** Internal RungeKutta propagation method for neutral track parameters*/   

  const NeutralParameters*    propagateStraightLine
    (Cache& cache                  ,
     bool                          ,
     const NeutralParameters      &,
     const Surface                &,
     const PropDirection           ,
     BoundaryCheck                 ,
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

  /** Internal RungeKutta propagation method for propation with jacobian*/     

  bool propagateWithJacobian
    (Cache& cache                  , 
     bool                          ,
     int                           ,
     double                       *,
     double                       *,
     double                       &) const;

  /** Propagation methods runge kutta step*/

  double rungeKuttaStep  
    (Cache& cache,
     bool                          ,
     double                        ,
     double                       *,
     bool                         &) const;

  /** Propagation methods runge kutta step*/

  double rungeKuttaStepWithGradient  
    (Cache& cache                  ,
     double                        ,
     double                       *,
     bool                         &) const;

  /** Propagation methods straight line step*/

  double straightLineStep
    (bool   ,     
     double ,
     double*) const;

  /** Test new propagation to cylinder boundary */

  bool newCrossPoint
    (const CylinderSurface&,
     const double         *,
     const double         *) const;

  /** Step estimator with directions correction */

  double stepEstimatorWithCurvature(Cache& cache,int,double*,const double*,bool&) const;

  /** Step reduction */
  double stepReduction(const double*) const;

  /** Build new track parameters without propagation */

  const TrackParameters*  buildTrackParametersWithoutPropagation
    (const TrackParameters &,double*) const;

  const NeutralParameters*  buildTrackParametersWithoutPropagation
    (const NeutralParameters&,double*) const;

  void globalOneSidePositions
    (Cache& cache                   ,
     std::list<Amg::Vector3D>       &,
     const double                   *,
     const MagneticFieldProperties  &,
     const CylinderBounds           &,
     double                          ,
     ParticleHypothesis particle=pion) const;

  void globalTwoSidePositions
    (Cache& cache                   ,
     std::list<Amg::Vector3D>       &,
     const double                   *,
     const MagneticFieldProperties  &,
     const CylinderBounds           &,
     double                          ,
     ParticleHypothesis particle=pion) const;

  const Trk::TrackParameters* crossPoint
    (const TrackParameters    &,
     std::vector<DestSurf>    &,
     std::vector<unsigned int>&,
     double                   *,
     std::pair<double,int>    &) const;

  void getField        (Cache& cache, double*,double*        ) const;
  void getFieldGradient(Cache& cache, double*,double*,double*) const;

  //placeholder for compatibility with new interface
  virtual
  const TrackSurfaceIntersection* intersectSurface(const Surface&,
                                                   const TrackSurfaceIntersection*,
                                                   const double,
                                                   const MagneticFieldProperties&,
                                                   ParticleHypothesis) const override
  {return 0;}

  /////////////////////////////////////////////////////////////////////////////////
  // Private data members: 
  /////////////////////////////////////////////////////////////////////////////////

  double m_dlt                                               ;  // accuracy parameter
  double m_helixStep                                         ;  // max step whith helix model
  double m_straightStep                                      ;  // max step whith srtaight line model
  bool   m_usegradient                                       ;  // use magnetif field gradient
  ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;
  MagField::IMagFieldSvc*                m_fieldService      ;
};

/////////////////////////////////////////////////////////////////////////////////
// Inline methods for magnetic field information
/////////////////////////////////////////////////////////////////////////////////

inline void RungeKuttaPropagator::getField(Cache& cache,double* R,double* H) const
{
  if(cache.m_solenoid) return m_fieldService->getFieldZR(R,H);
  return                m_fieldService->getField  (R,H);
}

inline void RungeKuttaPropagator::getFieldGradient(Cache& cache,double* R,double* H,double* dH) const
{
  if(cache.m_solenoid) return m_fieldService->getFieldZR(R,H,dH);
  return                m_fieldService->getField  (R,H,dH);
}
}

#endif // RungeKuttaPropagator_H
