/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

//////////////////////////////////////////////////////////////////////
// wrapper to IIntersector tool to provide IPropagator functionality
//   default configuration wraps the RungeKutta intersector
// (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////////

#ifndef TRKEXRUNGEKUTTAINTERSECTOR_INTERSECTORWRAPPER_H
#define TRKEXRUNGEKUTTAINTERSECTOR_INTERSECTORWRAPPER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkExInterfaces/IPropagator.h"

namespace Trk
{
class IIntersector;
class TrackSurfaceIntersection;

class IntersectorWrapper: public AthAlgTool,
  virtual public IPropagator
  {

  public:
    using IPropagator::propagate;
    using IPropagator::propagateT;

    IntersectorWrapper	(const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);
    ~IntersectorWrapper	(void); 	// destructor

    virtual StatusCode		initialize() override;
    virtual StatusCode		finalize() override;



    /** N 0) <b>Neutral parameters method </b>
      - returns a ParametersBase object as well, 0 if the extrapolation did not succeed
      */
    /// implemented
    virtual  NeutralParameters* propagate(const NeutralParameters&,
                                       const Surface&,
                                       PropDirection,
                                       BoundaryCheck,
                                       bool) const override;

    /** [TrackParameters] --------------------------------------------------------- */

    /** Propagation interface:

      The propagation method called by the TrkExtrapolator. The extrapolator
      is responsible for the underlying logic of which surface to go to.
      */
    /// implemented
    virtual  TrackParameters*      propagate( const TrackParameters& parm,
                                              const Surface& sf,
                                              PropDirection dir,
                                              BoundaryCheck bcheck,
                                              const MagneticFieldProperties& mprop,
                                              ParticleHypothesis particle,
                                              bool returnCurv,
                                              const TrackingVolume*) const override;


    /** Propagation interface:

      The propagation method called by the TrkExtrapolator. The propagator
      finds the closest surface.
      */
    virtual TrackParameters* propagate( const TrackParameters&,
                                      std::vector<DestSurf>&,
                                      PropDirection,
                                      const MagneticFieldProperties&,
                                      ParticleHypothesis,
                                      std::vector<unsigned int>&,
                                      double&,
                                      bool,
                                      bool,
                                      const TrackingVolume*) const override{ return 0; }

    /** Propagation interface:

      The propagation method called by the TrkExtrapolator. The propagator
      finds the closest surface. Timing included.
      */
    virtual  TrackParameters* propagateT( const TrackParameters&,
                                       std::vector<DestSurf>&,
                                       PropDirection,
                                       const MagneticFieldProperties&,
                                       ParticleHypothesis,
                                       std::vector<unsigned int>&,
                                       PathLimit&, TimeLimit&,
                                       bool,
                                       const TrackingVolume*,
                                       std::vector<Trk::HitInfo>*&) const override{ return 0; }


    /** Propagation interface:

      The propagation method including the return of the TransportJacobian matrix.

*/
    /// implemented
    virtual  TrackParameters*      propagate( const TrackParameters&,
                                           const Surface&,
                                           PropDirection,
                                           BoundaryCheck,
                                           const MagneticFieldProperties&,
                                           TransportJacobian*&,
                                           double&,
                                           ParticleHypothesis,
                                           bool,
                                           const TrackingVolume*) const override;


    /** Propagation interface without Covariance matrix propagation
      the pathlength has to be returned for eventual following propagateCovariance
      */
    /// implemented
    virtual  TrackParameters*      propagateParameters( const TrackParameters& parm,
                                                     const Surface& sf,
                                                     PropDirection dir,
                                                     BoundaryCheck bcheck,
                                                     const MagneticFieldProperties& mprop,
                                                     ParticleHypothesis particle=pion,
                                                     bool returnCurv = false,
                                                     const TrackingVolume* tVol=0) const override;

    /// implemented
    virtual  TrackParameters*      propagateParameters( const TrackParameters& parm,
                                                     const Surface& sf,
                                                     PropDirection dir,
                                                     BoundaryCheck bcheck,
                                                     const MagneticFieldProperties& mprop,
                                                     TransportJacobian*&,
                                                     ParticleHypothesis particle=pion,
                                                     bool returnCurv = false,
                                                     const TrackingVolume* tVol=0) const override;


    /** Intersection interface:

      The intersection interface might be used by the material service as well to estimate
      the surfaces (sensitive and nonesensitive) while propagation
      */
    virtual const IntersectionSolution* intersect( const TrackParameters& parm,
                                           const Surface& sf,
                                           const MagneticFieldProperties& mprop,
                                           ParticleHypothesis particle=pion,
                                           const TrackingVolume* tVol=0) const override;

    /** GlobalPositions list interface:
      This is used mostly in pattern recognition in the road finder, the propagation direction is intrinsically given
      by the sign of the stepSize.

      To avoid memory fragmentation in multiple use of pattern recognition processes and respecting the possible iterative
      filling of the positions list, the list of GlobalPositions is given by reference through the signature and a void
      method has been chosen.
      */
    virtual void globalPositions(std::list<Amg::Vector3D>& positionslist, 
                         const TrackParameters& parm,
                         const MagneticFieldProperties& mprop,
                         const CylinderBounds& cylbo,
                         double stepSize,
                         ParticleHypothesis particle=pion,
                         const TrackingVolume* tVol=0) const override;

    //placeholder for compatibility with new interface                                                                                                                        
    virtual const TrackSurfaceIntersection* intersectSurface(const Surface&,
                                                     const TrackSurfaceIntersection*,
                                                     const double,
                                                     const MagneticFieldProperties&,
                                                     ParticleHypothesis) const override{return 0;}

    /** Validation Action:
      Can be implemented optionally, outside access to internal validation steps */
    virtual void validationAction() const override{}



  private:
    struct Cache{
      double			                                    m_charge;
      double			                                    m_qOverP;
      std::unique_ptr<const TrackSurfaceIntersection> m_intersection;
      TrackParameters*                         	      m_parameters;
      Amg::Vector3D		                                m_position;
      Amg::Vector3D		                                m_momentum;

      Cache():	
        m_charge{},
        m_qOverP{},
        m_intersection{nullptr},
        m_parameters{nullptr},
        m_position{},
        m_momentum{}{
        }
    };
    // private methods
    void			
      createParameters (Cache& cache, 
                        const Surface&	surface,
                        BoundaryCheck		boundsCheck,
                        bool			curvilinear) const;
    void			
      findIntersection (Cache& cache, 
                        const TrackParameters& parameters,
                        const Surface&	surface,
                        PropDirection		dir = Trk::anyDirection) const;

    // helpers, managers, tools
    ToolHandle<IIntersector>		m_intersector;
    ToolHandle<IPropagator>		  m_linePropagator;
  };

} // end of namespace


#endif // TRKEXRUNGEKUTTAINTERSECTOR_INTERSECTORWRAPPER_H
