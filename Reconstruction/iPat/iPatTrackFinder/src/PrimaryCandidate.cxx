/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 a PrimaryCandidate is a track with approximate parameters formed from 
 2 SpacePoints with a vertex region
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "iPatInterfaces/IFinderConfiguration.h"
#include "iPatTrackFinder/Point.h"
#include "iPatTrackFinder/Direction.h"
#include "iPatTrackFinder/PrimaryCandidate.h"
#include "iPatTrackParameters/PerigeeParameters.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

PrimaryCandidate::PrimaryCandidate (ToolHandle<IFinderConfiguration>&	tolerances,
				    const Amg::Vector3D&		vertexRegionCentre)
    :	m_inBarrel		(false),
	m_outerPosition 	(Amg::Vector3D(0.,0.,0.)),
	m_outerR		(0.),  
	m_outerPhi		(0.), 
	m_outerZ		(0.),
	m_innerPosition		(Amg::Vector3D(0.,0.,0.)),
	m_innerR		(0.),
	m_innerPhi		(0.),
	m_innerZ		(0.),
	m_cacheValid		(false),
	m_etaTolerance		(0.),
	m_phiTolerance		(0.),
	m_tolerances		(tolerances),
	m_phi0			(0.),
	m_phiSlope		(0.),
	m_cosPhi		(0.),
	m_cotTheta		(0.),
	m_inversePt		(0.),
	m_sinPhi		(0.),
	m_transverseImpact	(0.),
	m_vertex       		(vertexRegionCentre),
	m_z			(0.)
{}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

bool
PrimaryCandidate::addInnerPixel (const Point& innerPoint)
{
    // for doublePixel search (inner and intermediate points from pixels)
    // check inside z vertex region
    double invDeltaR	= 1./(m_outerR - innerPoint.r());
    m_cotTheta		= invDeltaR*(m_outerZ - innerPoint.z());
    m_z			= m_outerZ - m_outerR * m_cotTheta;
    if (m_z > m_tolerances->maxVertexZ()
	|| m_z < m_tolerances->minVertexZ())			return false;
    
    // check below maximum curvature
    m_phiSlope		= invDeltaR*(m_outerPhi - innerPoint.phi());
    if (std::abs(m_phiSlope) > m_tolerances->maxPhiSlope())	return false;
    
    m_innerPosition     = innerPoint.globalPosition();
    m_innerPhi          = innerPoint.phi();
    m_innerR            = innerPoint.r();
    m_innerZ            = innerPoint.globalPosition().z();
    m_phi0		= m_outerPhi - m_outerR * m_phiSlope;

    // invalidate cache
    m_cacheValid	= false;

    if (m_tolerances->printLevel() > 5)
    {
	std::cout << "     innerPixel: " << std::setiosflags(std::ios::fixed)
		  << " z0 " << std::setw(6) << std::setprecision(1) << m_z
		  << " cotTheta " << std::setw(8) << std::setprecision(3) << m_cotTheta
		  << " phiSlope " << std::setw(8) << std::setprecision(5) << m_phiSlope << "   ";
	innerPoint.print();
    }

    return true;    
}

bool
PrimaryCandidate::addInnerPixelAtNewVtx (const Point& innerPoint,
					 std::vector<double>::const_iterator zBegin,
					 std::vector<double>::const_iterator zEnd)
{
    // overload for vertexDefinitionSearch
    // check inside z vertex region
    double invDeltaR	= 1./(m_outerR - innerPoint.r());
    m_cotTheta		= invDeltaR*(m_outerZ - innerPoint.z());
    m_z			= m_outerZ - m_outerR * m_cotTheta;
    if (m_z > m_tolerances->maxVertexZ() || m_z < m_tolerances->minVertexZ())	return false;

    // veto if vertex already found
    double sinThetaSq	= 1./(1. + m_cotTheta*m_cotTheta);
    for (std::vector<double>::const_iterator z = zBegin; z != zEnd; ++z)
    {
	double zImpactSq = (m_z-*z)*(m_z-*z)*sinThetaSq;
	if (zImpactSq < 1.*Gaudi::Units::mm*Gaudi::Units::mm) return false; // TODO!! rm hardwired number	
    }
    
    // check below maximum curvature
    m_phiSlope		= invDeltaR*(m_outerPhi - innerPoint.phi());
    if (std::abs(m_phiSlope) > m_tolerances->maxPhiSlope())		return false;

    m_innerPosition     = innerPoint.globalPosition();
    m_innerPhi          = innerPoint.phi();
    m_innerR            = innerPoint.r();
    m_innerZ            = innerPoint.globalPosition().z();
    m_phi0		= m_outerPhi - m_outerR * m_phiSlope;
    m_cacheValid       	= true;
    double invSinTheta	= std::sqrt(1.+m_cotTheta*m_cotTheta);
    m_etaTolerance	= m_tolerances->maxPrimaryDeltaEtaVtx()*invSinTheta;
    m_phiTolerance	= m_tolerances->maxPrimaryDeltaPhiVtx()*std::sqrt(invSinTheta);

    if (m_tolerances->printLevel() > 5)
    {
	std::cout << "     innerPoint: " << std::setiosflags(std::ios::fixed)
		  << " z0 " << std::setw(6) << std::setprecision(1) << m_z
		  << " cotTheta " << std::setw(8) << std::setprecision(3) << m_cotTheta
		  << "   ";
	innerPoint.print();
    }
        
    return true;
}

bool
PrimaryCandidate::isPixelCompatible (const Point& point)
{
    // check eta compatible
    if (std::abs(point.z() - m_z - m_cotTheta*point.r()) > m_tolerances->maxPrimaryDeltaEtaPix())
	return false;

    // check phi compatible
    if (std::abs(point.phi() - m_phi0 - m_phiSlope*point.r()) > m_tolerances->maxPrimaryDeltaPhiPix())
	return false;
    
    return true;    
}

bool
PrimaryCandidate::isPointCompatible (const Point& point)
{
    // cache tolerances
    if (! m_cacheValid)
    {
	m_cacheValid		= true;
	double invSinTheta	= std::sqrt(1.+m_cotTheta*m_cotTheta);
	m_etaTolerance		= m_tolerances->maxPrimaryDeltaEtaSct()*invSinTheta;
	m_phiTolerance		= m_tolerances->maxPrimaryDeltaPhiSct()*std::sqrt(invSinTheta);
    }
    
    // check eta compatible
    if (m_inBarrel)
    {
	if (std::abs(point.z() - m_z - m_cotTheta*point.r()) > m_etaTolerance)		return false;
    }
    else
    {
	if (std::abs(point.r() - (point.z() - m_z)/m_cotTheta) > m_etaTolerance)	return false;
    }

    // check phi compatible
    if (std::abs(m_phi0 + m_phiSlope*point.r() -point.phi()) > m_phiTolerance)		return false;
    
    return true;
}

PerigeeParameters*
PrimaryCandidate::perigee (void)
{
    Direction	in_direction (m_innerPosition,
			      m_innerR);
    Direction	out_direction(m_innerPosition,
			      m_outerPosition);
    m_inversePt		= out_direction.sin_diff_phi(in_direction) /
			  (m_outerR*m_tolerances->halfField());
    double	sin_phi	= m_inversePt*m_innerR*m_tolerances->halfField();
    Direction	phi_correction (std::sqrt(1.0-sin_phi*sin_phi), sin_phi, 0.);
    m_cosPhi		= in_direction.cos_diff_phi(phi_correction);
    m_sinPhi		= in_direction.sin_diff_phi(phi_correction);
    double r		= m_vertex.x()*m_cosPhi + m_vertex.y()*m_sinPhi;
    sin_phi		= m_inversePt*(r+m_innerR)*m_tolerances->halfField();
    Direction   phi_correction2 (std::sqrt(1.0-sin_phi*sin_phi), sin_phi, 0.);
    m_cosPhi            = in_direction.cos_diff_phi(phi_correction2);
    m_sinPhi            = in_direction.sin_diff_phi(phi_correction2);
    m_transverseImpact	= m_vertex.y()* m_cosPhi - m_vertex.x()*m_sinPhi;
    //    m_transverseImpact  *= -1.;
    m_z                 = m_innerZ -
      (m_innerR + m_vertex.x()*m_cosPhi + m_vertex.y()*m_sinPhi)*m_cotTheta;
//     std::cout << std::endl << " PrimaryCandidate: impact " << 10.*m_transverseImpact 
// 	      << "  dr " << 10.*(m_vertex.x()*m_cosPhi + m_vertex.y()*m_sinPhi) 
// 	      << "   pt " << 1./m_inversePt << std::endl;
    
    //    m_cotTheta		= out_direction.cot_theta();

    return new PerigeeParameters(m_cosPhi,
				 m_cotTheta,
				 m_inversePt,
				 m_sinPhi,
				 m_transverseImpact,
				 Amg::Vector3D(0.,0.,0.),
				 m_z); 
}

void
PrimaryCandidate::print (void) const
{
    double momentum = 0.;
    if (m_inversePt != 0.) momentum = 1./m_inversePt;
    std::cout << std::setiosflags(std::ios::fixed)
	      << " PrimaryCandidate: " 
	      << "   zo" 	<< std::setw(6) << std::setprecision(1) << m_z
	      << "   phio" 	<< std::setw(8) << std::setprecision(4) << m_phi0
	      << "   pt" 	<< std::setw(8) << std::setprecision(2) << momentum/Gaudi::Units::GeV
	      << std::resetiosflags(std::ios::fixed) << std::endl;
}

void
PrimaryCandidate::setOuterPoint (const Point& outerPoint)
{
    // save point parameters 
    m_inBarrel          = outerPoint.inBarrel();
    m_outerPosition     = outerPoint.globalPosition();
    m_outerPhi		= outerPoint.phi();
    m_outerR            = outerPoint.r();
    m_outerZ            = outerPoint.globalPosition().z();

    if (m_tolerances->printLevel() > 4)
    {
	std::cout << std::endl << "     new outerPoint: ";
        outerPoint.print();
    }
}


