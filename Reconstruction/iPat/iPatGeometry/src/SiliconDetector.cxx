/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
  unique properties of a silicon detector element
 (within a specific SiliconRing)
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "iPatGeometry/DetectorShape.h"
#include "iPatGeometry/SiliconDetector.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SiliconDetector::SiliconDetector(const InDetDD::SiDetectorElement&	element,
				 const DetectorShape&			shape)
    :	m_element		(element),
	m_identifier    	(new Identifier(element.identify())),
	m_identifierHash	(static_cast<int>(element.identifyHash())),
	m_parent		(0),
	m_sigmaAlignment	(false),
	m_sigmaAlignmentR	(0.),
	m_sigmaAlignmentRPhi	(0.),
	m_sigmaAlignmentZ	(0.),
	m_straddlesInPhi	(false)
{
    // cache phi for ring creation
    m_phiCentre	= m_element.center().phi();
    
    // note asymmetry:
    //   catch any straddle at -PI, but demand significant straddle at +PI,
    //   to avoid double straddle bug when detector boundary aligned to phi=0.
    if (m_phiCentre - shape.localPhiWidth()/m_element.center().perp()		< -M_PI
	|| m_phiCentre + 0.5*shape.localPhiWidth()/m_element.center().perp()	>  M_PI)
	m_straddlesInPhi = true;
}

SiliconDetector::SiliconDetector(const SiliconDetector& detector)
    :	m_deadRegions		(detector.m_deadRegions),
	m_element		(detector.m_element),
	m_identifier    	(new Identifier(*detector.m_identifier)),
	m_identifierHash	(detector.m_identifierHash),
	m_parent		(detector.m_parent),
	m_phiCentre		(detector.m_phiCentre),
	m_sigmaAlignment	(detector.m_sigmaAlignment),
	m_sigmaAlignmentR	(detector.m_sigmaAlignmentR),
	m_sigmaAlignmentRPhi	(detector.m_sigmaAlignmentRPhi),
	m_sigmaAlignmentZ	(detector.m_sigmaAlignmentZ),
	m_straddlesInPhi	(detector.m_straddlesInPhi)
{}

SiliconDetector::~SiliconDetector(void)
{
    delete m_identifier;
    // also must delete std::vector<DeadRegion*>
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

SiliconDetector*
SiliconDetector::copyThenWrapPhi(void)
{
    SiliconDetector* detectorCopy = new SiliconDetector(*this);
    detectorCopy->setParent(m_parent);
    if (m_phiCentre > 0.)
    {
	m_phiCentre	-= 2.*M_PI;
    }
    else
    {
	m_phiCentre	+= 2.*M_PI;
    }
    return detectorCopy;
}

DetectorIntersect*
SiliconDetector::intersect (Amg::Vector3D	position,
			    Amg::Vector3D	direction,
			    double		rphiTol,
			    double		rzTol) const
{
    double distanceToPlane	=  m_element.normal().dot(m_element.center() - position) /
				   m_element.normal().dot(direction);
    position			+= distanceToPlane*direction;
    Amg::Vector3D	offset	=  position - m_element.center();
    double localEta		=  m_element.etaAxis().dot(offset);
    if (m_parent->shape().insideEta(localEta))
    {
	double 	localPhi		= m_element.phiAxis().dot(offset);
	DetectorIntersect* intersect	= m_parent->shape().intersect(localEta,
								      localPhi,
								      rzTol,
								      rphiTol,
								      std::abs(sinStereo()));
	if (intersect != 0)
	{
	    intersect->position(position);
	    // TODO: check for DeadRegion's 
	    return intersect;
	}
    }
    return 0;
}

void
SiliconDetector::refresh (void)
{
    // refresh phiCentre - taking care of straddling
    double oldPhiCentre	= m_phiCentre;
    m_phiCentre	= m_element.center().phi();
    if (oldPhiCentre > M_PI)
    {
	m_phiCentre	+= 2.*M_PI;
    }
    else if (oldPhiCentre < -M_PI)
    {
	m_phiCentre	-= 2.*M_PI;
    }
}

void
SiliconDetector::rzLimits (double& rMax, double& rMin, double& zMax, double& zMin) const
{
    // take limits from 4 corners and eta-axis of shape:
    Amg::Vector3D	etaOffset	= m_element.etaAxis()*m_parent->shape().localEtaWidth();
    Amg::Vector3D	phiOffset	= m_element.phiAxis()*m_parent->shape().localPhiWidth();
    Amg::Vector3D	offset		= m_element.center() + etaOffset + phiOffset;
    rMax				= offset.perp();
    rMin				= offset.perp();
    zMax				= offset.z();
    zMin				= offset.z();
    offset	= m_element.center() + etaOffset - phiOffset;
    if (offset.z()	> zMax)  zMax = offset.z();
    if (offset.perp()	> rMax)  rMax = offset.perp();
    if (offset.z()	< zMin)  zMin = offset.z();
    if (offset.perp()	< rMin)  rMin = offset.perp();
    offset	= m_element.center() - etaOffset + phiOffset;
    if (offset.z()	> zMax)  zMax = offset.z();
    if (offset.perp()	> rMax)  rMax = offset.perp();
    if (offset.z()	< zMin)  zMin = offset.z();
    if (offset.perp()	< rMin)  rMin = offset.perp();
    offset	= m_element.center() - etaOffset - phiOffset;
    if (offset.z()	> zMax)  zMax = offset.z();
    if (offset.perp()	> rMax)  rMax = offset.perp();
    if (offset.z()	< zMin)  zMin = offset.z();
    if (offset.perp()	< rMin)  rMin = offset.perp();
    offset	= m_element.center() - etaOffset;
    if (offset.z()	> zMax)  zMax = offset.z();
    if (offset.perp()	> rMax)  rMax = offset.perp();
    if (offset.z()	< zMin)  zMin = offset.z();
    if (offset.perp()	< rMin)  rMin = offset.perp();
}

void
SiliconDetector::setSigmaAlignment (std::vector<double>& sigma)
{ 
    m_sigmaAlignmentR		= sigma[0];
    m_sigmaAlignmentRPhi	= sigma[1];
    m_sigmaAlignmentZ		= sigma[2];
    if (sigma[0] > 0. || sigma[1] > 0. ||sigma[2] > 0.)
    {
	m_sigmaAlignment = true;
    }
    else
    {
	m_sigmaAlignment = false;
    }
}

double
SiliconDetector::sinStereo(void) const
{
    double sinStereo;
    // barrel definition
    if (std::abs(m_element.etaAxis().z()) > 0.5)
    {
        sinStereo = m_element.phiAxis().z();
    }
    
    // endcap definition
    else
    {
        Amg::Vector3D stereoAxis = m_element.center().cross(m_element.etaAxis());
        if (m_element.center().z() > 0.)
        {
            sinStereo   =  stereoAxis.z()/m_element.center().perp();
        }
        else
        {
            sinStereo   = -stereoAxis.z()/m_element.center().perp();
        }
    }
    return sinStereo;
}
