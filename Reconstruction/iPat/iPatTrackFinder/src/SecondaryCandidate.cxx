/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 a SecondaryCandidate is a track with approximate parameters formed from 
 3 SpacePoints or from 2 SpacePoints with a vertex region
 ------------------------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "iPatInterfaces/IFinderConfiguration.h"
#include "iPatTrackFinder/Point.h"
#include "iPatTrackFinder/SecondaryCandidate.h"
#include "iPatTrackParameters/PerigeeParameters.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SecondaryCandidate::SecondaryCandidate (const Amg::Vector3D&			perigeeVertex,
					ToolHandle<IFinderConfiguration>&	tolerances,
					std::vector<double>::const_iterator	zBegin,
					std::vector<double>::const_iterator	zEnd)
    :	m_perigeeVertex		(perigeeVertex),
	m_tolerances		(tolerances),
	m_inBarrel		(false),
	m_outerPosition 	(Amg::Vector3D(0.,0.,0.)),
	m_outerR		(0.),  
	m_innerPosition		(Amg::Vector3D(0.,0.,0.)),
	m_a1			(0.),
	m_a2			(0.),
	m_cosAxis		(0.),
	m_sinAxis		(0.),
	m_x			(0.),
	m_cosPhi		(0.),
	m_cotTheta		(0.),
	m_inversePt		(0.),
	m_sinPhi		(0.),
	m_transverseImpact	(0.),
	m_z			(0.),
	m_zBegin		(zBegin),
	m_zEnd			(zEnd)
{
//     std::cout << "   m_half_field " << tolerances.halfField()
// 	      << "   m_phi_slope_max " << tolerances.maxPhiSlope()
// 	      << "   m_tolerances->maxVertexZ() " << m_tolerances->maxVertexZ()
// 	      << std::endl;
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

bool
SecondaryCandidate::addInnerPoint (const Point& innerPoint)
{
    m_innerPosition		= innerPoint.globalPosition();
    Amg::Vector3D	dist	= m_innerPosition - m_outerPosition;
    m_x				= dist.perp();
    Amg::Vector3D	axis	= -dist/m_x;
    m_cotTheta			= axis.z();
    m_z				= m_outerPosition.z() - m_cotTheta*m_outerR;
    
    // inside z vertex region
    if (m_z > m_tolerances->maxVertexZ() || m_z < m_tolerances->minVertexZ())	return false;
    
    // acceptable longitudinal impact at already found vertices (cut on deltaZ*sinTheta)
    double maxImpactSq	= m_tolerances->maxSecondaryImpact()*m_tolerances->maxSecondaryImpact()*
			  (1. + m_cotTheta*m_cotTheta);
    std::vector<double>::const_iterator z = m_zBegin;
    while (z != m_zEnd)
    {
	if ((m_z-*z)*(m_z-*z) < maxImpactSq)	break;
	if (++z == m_zEnd)			return false;
    }

    m_cosAxis		= axis.x();
    m_sinAxis		= axis.y();
  
    if (m_tolerances->printLevel() > 5)
    {
	std::cout << "     innerPoint: ";
	innerPoint.print();
    }

    return true;
}

bool
SecondaryCandidate::addIntermediatePoint (const Point& point)
{
    if (m_inBarrel)
    {
	if (std::abs(point.z() - m_z - m_cotTheta * point.r())
	    > m_tolerances->maxDeltaRZ())  					return false;
    }
    else
    {
	double	r	= (point.z() - m_z) / m_cotTheta;
	if (std::abs(point.r() - r) > m_tolerances->maxDeltaRZ())		return false;
    }
    double deltaX	= point.globalPosition().x() - m_innerPosition.x();
    double deltaY	= point.globalPosition().y() - m_innerPosition.y();
    double x2		= deltaX*m_cosAxis + deltaY*m_sinAxis;
    double y2		= deltaY*m_cosAxis - deltaX*m_sinAxis;
    m_a2		= y2/(x2*(x2 - m_x));
    if (std::abs(m_a2) > m_tolerances->maxPhiSlope())				return false;
    m_a1		= -m_a2*m_x;
    deltaX		= m_perigeeVertex.x() - m_innerPosition.x();
    deltaY		= m_perigeeVertex.y() - m_innerPosition.y();
    double xv		= deltaX*m_cosAxis + deltaY*m_sinAxis;
    double yv		= deltaY*m_cosAxis - deltaX*m_sinAxis;
    m_transverseImpact	= xv*(m_a1 + m_a2*xv) - yv;
    m_z			= m_innerPosition.z() + m_cotTheta*xv;
    if (std::abs(m_transverseImpact) > m_tolerances->maxSecondaryImpact())	return false;
    double sinDeltaPhi	= m_a1 + 2.0*xv*m_a2;
    double cosDeltaPhi	= std::sqrt(1.0 - sinDeltaPhi*sinDeltaPhi);
    m_cosPhi		= m_cosAxis*cosDeltaPhi - m_sinAxis*sinDeltaPhi;
    m_sinPhi		= m_sinAxis*cosDeltaPhi + m_cosAxis*sinDeltaPhi;
    m_inversePt		= m_a2/m_tolerances->halfField();
    //  TODO: add Pt dependent impact cut
//     print();
    return true;
}

PerigeeParameters*
SecondaryCandidate::perigee (void)
{
    return new PerigeeParameters(m_cosPhi,
				 m_cotTheta,
				 m_inversePt,
				 m_sinPhi,
				 m_transverseImpact,
				 m_perigeeVertex,
				 m_z); 
}

void
SecondaryCandidate::print (void) const
{
    std::cout << std::setiosflags(std::ios::fixed)
	      << " SecondaryCandidate:   ";
    PerigeeParameters	perigee(m_cosPhi,
				m_cotTheta,
				m_inversePt,
				m_sinPhi,
				m_transverseImpact,
				m_perigeeVertex,
				m_z); 
    perigee.print();
    std::cout << std::endl;
}

void
SecondaryCandidate::setOuterPoint (const Point& outerPoint)
{
    m_inBarrel		= outerPoint.inBarrel();
    m_outerPosition	= outerPoint.globalPosition();
    m_outerR		= outerPoint.r();
    
    if (m_tolerances->printLevel() > 4)
    {
	std::cout << std::endl << " new outerPoint: ";
	outerPoint.print();
    }
}

