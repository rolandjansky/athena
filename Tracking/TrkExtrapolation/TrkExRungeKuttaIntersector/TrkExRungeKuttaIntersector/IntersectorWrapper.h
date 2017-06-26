/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

    StatusCode			initialize();
    StatusCode			finalize();

          

    /** N 0) <b>Neutral parameters method </b>
	- returns a ParametersBase object as well, 0 if the extrapolation did not succeed
    */
    /// implemented
    const NeutralParameters* propagate(const NeutralParameters&,
				       const Surface&,
				       PropDirection,
				       BoundaryCheck,
				       bool) const;

    /** [TrackParameters] --------------------------------------------------------- */

    /** Propagation interface:
    
	The propagation method called by the TrkExtrapolator. The extrapolator
	is responsible for the underlying logic of which surface to go to.
    */
    /// implemented
    const TrackParameters*      propagate( const TrackParameters& parm,
					   const Surface& sf,
					   PropDirection dir,
					   BoundaryCheck bcheck,
					   const MagneticFieldProperties& mprop,
					   ParticleHypothesis particle,
					   bool returnCurv,
					   const TrackingVolume*) const;
  

    /** Propagation interface:
    
	The propagation method called by the TrkExtrapolator. The propagator
	finds the closest surface.
    */
    const TrackParameters* propagate( const TrackParameters&,
				      std::vector<DestSurf>&,
				      PropDirection,
				      const MagneticFieldProperties&,
				      ParticleHypothesis,
				      std::vector<unsigned int>&,
				      double&,
				      bool,
				      bool,
				      const TrackingVolume*) const { return 0; }
  
    /** Propagation interface:
    
	The propagation method called by the TrkExtrapolator. The propagator
	finds the closest surface. Timing included.
    */
    const TrackParameters* propagateT( const TrackParameters&,
				       std::vector<DestSurf>&,
				       PropDirection,
				       const MagneticFieldProperties&,
				       ParticleHypothesis,
				       std::vector<unsigned int>&,
				       PathLimit&, TimeLimit&,
				       bool,
				       const TrackingVolume*,
				       std::vector<Trk::HitInfo>*&) const { return 0; }
    

    /** Propagation interface:
    
	The propagation method including the return of the TransportJacobian matrix.

    */
    /// implemented
    const TrackParameters*      propagate( const TrackParameters&,
					   const Surface&,
					   PropDirection,
					   BoundaryCheck,
					   const MagneticFieldProperties&,
					   TransportJacobian*&,
					   double&,
					   ParticleHypothesis,
					   bool,
					   const TrackingVolume*) const;
  

    /** Propagation interface without Covariance matrix propagation
	the pathlength has to be returned for eventual following propagateCovariance
    */
    /// implemented
    const TrackParameters*      propagateParameters( const TrackParameters& parm,
						     const Surface& sf,
						     PropDirection dir,
						     BoundaryCheck bcheck,
						     const MagneticFieldProperties& mprop,
						     ParticleHypothesis particle=pion,
						     bool returnCurv = false,
						     const TrackingVolume* tVol=0) const;
  
    /// implemented
    const TrackParameters*      propagateParameters( const TrackParameters& parm,
						     const Surface& sf,
						     PropDirection dir,
						     BoundaryCheck bcheck,
						     const MagneticFieldProperties& mprop,
						     TransportJacobian*&,
						     ParticleHypothesis particle=pion,
						     bool returnCurv = false,
						     const TrackingVolume* tVol=0) const;
  

    /** Intersection interface:
    
	The intersection interface might be used by the material service as well to estimate
	the surfaces (sensitive and nonesensitive) while propagation
    */
    const IntersectionSolution* intersect( const TrackParameters& parm,
					   const Surface& sf,
					   const MagneticFieldProperties& mprop,
					   ParticleHypothesis particle=pion,
					   const TrackingVolume* tVol=0) const;
                                                
    /** GlobalPositions list interface:
	This is used mostly in pattern recognition in the road finder, the propagation direction is intrinsically given
	by the sign of the stepSize.
    
	To avoid memory fragmentation in multiple use of pattern recognition processes and respecting the possible iterative
	filling of the positions list, the list of GlobalPositions is given by reference through the signature and a void
	method has been chosen.
    */
    void globalPositions(std::list<Amg::Vector3D>& positionslist, 
			 const TrackParameters& parm,
			 const MagneticFieldProperties& mprop,
			 const CylinderBounds& cylbo,
			 double stepSize,
			 ParticleHypothesis particle=pion,
			 const TrackingVolume* tVol=0) const;

    //placeholder for compatibility with new interface                                                                                                                        
    const TrackSurfaceIntersection* intersectSurface(const Surface&,
						     const TrackSurfaceIntersection*,
						     const double,
						     const MagneticFieldProperties&,
						     ParticleHypothesis) const {return 0;}

    /** Validation Action:
	Can be implemented optionally, outside access to internal validation steps */
    void validationAction() const {}

    

private:
    
    // private methods
    void			
    createParameters (const Surface&	surface,
						  BoundaryCheck		boundsCheck,
						  bool			curvilinear) const;
    void			
    findIntersection (const TrackParameters& parameters,
						  const Surface&	surface,
						  PropDirection		dir = Trk::anyDirection) const;
        
    // helpers, managers, tools
    ToolHandle<IIntersector>		m_intersector;
    ToolHandle<IPropagator>		m_linePropagator;

    // current parameter cache
    mutable double			    m_charge;
    mutable const TrackSurfaceIntersection* m_intersection;
    mutable Amg::Vector3D		    m_momentum;
    mutable const TrackParameters*  	    m_parameters;
    mutable Amg::Vector3D		    m_position;
    mutable double			    m_qOverP;
};

} // end of namespace


#endif // TRKEXRUNGEKUTTAINTERSECTOR_INTERSECTORWRAPPER_H
