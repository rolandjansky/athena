/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes geometry of a ring of silicon detector elements
 (phi symmetric - forming part of barrel layer or endcap wheel)
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "iPatGeometry/DetectorShape.h"
#include "iPatGeometry/SiliconDetector.h"
#include "iPatGeometry/SiliconLayer.h"
#include "iPatGeometry/SiliconRing.h"

//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>

const double    SiliconRing::s_shapeTolerance	= 0.1*Gaudi::Units::mm;
const double    SiliconRing::s_sortTolerance	= 1.0*Gaudi::Units::mm;
const double    SiliconRing::s_stereoTolerance	= 0.010;

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SiliconRing::SiliconRing(SiliconDetector*	detector,
			 const DetectorShape*	shape)
    :	m_isPixel	(detector->isPixel()),
	m_isStereo	(detector->isStereo()),
	m_parent	(0),
	m_parity	(false),
	m_phiOffset	(0.),
	m_phiPeriod	(M_PI),
	m_shape		(shape),
	m_sinStereo	(detector->sinStereo())
{
    // reserve at half upper limit for as-built detector
    m_detectors.reserve(29);
    m_detectors.push_back(detector);
    detector->setParent(this);
    detector->rzLimits(m_rMax,m_rMin,m_zMax,m_zMin);
    if (detector->straddlesPhiBoundary())
	m_detectors.push_back(detector->copyThenWrapPhi());
}

SiliconRing::SiliconRing(const SiliconRing&	ring,
			 bool			parity)
    :	m_isPixel	(ring.m_isPixel),
	m_isStereo	(ring.m_isStereo),
	m_parent	(ring.m_parent),
	m_parity	(parity),
	m_phiOffset	(0.),
	m_phiPeriod	(M_PI),
	m_rMax		(ring.m_rMax),
	m_rMin		(ring.m_rMin),
	m_shape		(ring.m_shape),
	m_sinStereo	(ring.m_sinStereo),
	m_zMax		(ring.m_zMax),
	m_zMin		(ring.m_zMin)
{
    // reserve at upper limit for as-built detector
    m_detectors.reserve(29);
}

SiliconRing::~SiliconRing(void)
{}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

void
SiliconRing::addDetector(SiliconDetector* detector)
{
    m_detectors.push_back(detector);
    detector->setParent(this);
}

bool
SiliconRing::addDetector(SiliconDetector*	detector,
			 const DetectorShape*	shape)
{
    // demand similar shape, rz coordinates and stereo angle
    double deltaR	= (**(m_detectors.begin())).centre().perp() - detector->centre().perp();
    double deltaZ	= (**(m_detectors.begin())).centre().z() - detector->centre().z();
    double deltaStereo	= detector->sinStereo() - m_sinStereo;
    if (shape				== m_shape
	&& std::abs(deltaR)		<  s_shapeTolerance
	&& std::abs(deltaZ)		<  s_shapeTolerance
	&& std::abs(deltaStereo)	<  s_stereoTolerance)
    {
	m_detectors.push_back(detector);
	detector->setParent(this);
	if (detector->straddlesPhiBoundary())
	    m_detectors.push_back(detector->copyThenWrapPhi());
	return true;
    }
    else
    {
	return false;
    }
}

SiliconRing*
SiliconRing::copyWithParity(bool parity) const
{
    SiliconRing* ring = new SiliconRing(*this, parity);
    std::vector<SiliconDetector*>::const_iterator i = m_detectors.begin();
    if (m_parity && parity) --i;    
    while (i != m_detectors.end() && ++i != m_detectors.end()) ring->addDetector(*i++);
    ring->determinePhiProperties();
    return ring;
}

const SiliconDetector*
SiliconRing::detector(double phi) const
{
    int 			phiNumber	= static_cast<int>
						  ((phi + m_phiOffset)/m_phiPeriod);
    const SiliconDetector* 	detector	= *(m_detectors.begin() + phiNumber);
    //      cout << " phiNumber " << phiNumber
    //  	 << std::setiosflags(std::ios::fixed)
    //  	 << "  centre " << std::setw(7) << std::setprecision(3)
    //  	 << -m_phiOffset + m_phiPeriod*phiNumber + 0.5*m_phiPeriod
    //  	 << "  parity " << std::setw(2) << m_parity
    //  	 << endl;
    
    if (phi	> detector->phiCentre() - 0.6*m_phiPeriod
      	&& phi	< detector->phiCentre() + 0.6*m_phiPeriod)
    {
	return detector;
    }
    else
    {
      	return 0;
    }
}

const std::vector<SiliconDetector*>&
SiliconRing::detectors(void) const
{ return m_detectors; }

void
SiliconRing::determinePhiProperties(void)
{
    // order detectors by increasing phi
    std::sort (m_detectors.begin(), m_detectors.end(), SiliconDetector::PhiOrder());

    // even parity (true) if first detector straddles 0/2PI boundary
    // with protection for incomplete (TB) geometry => parity defined true
    std::vector<SiliconDetector*>::iterator i = m_detectors.begin();
    if ((**i).straddlesPhiBoundary() || m_detectors.size() < 3)
    {
	m_parity		= true;
    }
    else
    {
	// odd parity - put a phi-wrapped copy of the first detector at the end of the vector
	m_parity		= false;
	SiliconDetector* first	= (**i).copyThenWrapPhi();
	m_detectors.push_back(*i);
	i			= m_detectors.begin();
	*i			= first;
    }
    if (m_detectors.size() > 2)
    {
	m_phiPeriod = 2.*M_PI/static_cast<double>(m_detectors.size() - 1);
    }
    else if (m_detectors.size() == 2)
    {
	m_phiPeriod = (**m_detectors.rbegin()).phiCentre() - (**i).phiCentre();
    }
    else
    {
	m_phiPeriod = M_PI;
    }
    m_phiOffset = 0.5*m_phiPeriod - (**i).phiCentre();

    // for complete geometry verify that the hashed detector phi look-up lies within range
    if (m_detectors.size() > 2)
    {
	assert (static_cast<int> ((m_phiOffset - M_PI)/m_phiPeriod) >= 0);
	assert (static_cast<unsigned int> ((m_phiOffset + M_PI)/m_phiPeriod) < m_detectors.size());
    }
}

bool
SiliconRing::hasPhiParity(void) const
{
    // a ring has defined phiParity if it is phi-hermetic (i.e. has overlapping detectors).
    // It will be split into 2 rings with alternating detectors, the ring containing the
    // detector straddling the +-PI boundary is defined to have even parity (boolean true).
    return (m_shape->localPhiWidth() < 0.4*m_phiPeriod*m_rMin);
}

void
SiliconRing::print(void) const
{
    if (isBarrel())
    {
	std::cout << std::setiosflags(std::ios::fixed)
		  << " barrel ring: " << m_detectors.size()
		  << " dets   rMin,rMax"
		  << std::setw(7) << std::setprecision(1) << m_rMin
		  << std::setw(7) << std::setprecision(1) << m_rMax
		  << "  zMin,zMax"
		  << std::setw(8) << std::setprecision(2) << m_zMin
		  << std::setw(8) << std::setprecision(2) << m_zMax
		  << std::resetiosflags(std::ios::fixed) << std::endl;
    }
    else
    {
	std::cout << std::setiosflags(std::ios::fixed)
		  << " endcap ring: " << m_detectors.size()
		  << " dets   rMin,rMax"
		  << std::setw(7) << std::setprecision(1) << m_rMin
		  << std::setw(7) << std::setprecision(1) << m_rMax
		  << "  zMin,zMax"
		  << std::setw(8) << std::setprecision(2) << m_zMin
		  << std::setw(8) << std::setprecision(2) << m_zMax
		  << std::resetiosflags(std::ios::fixed) << std::endl;
    }
    
    for (std::vector<SiliconDetector*>::const_iterator i = m_detectors.begin();
	 i != m_detectors.end();
	 ++i)
    {
	int phiNumber	= static_cast<int> (((**i).phiCentre()+m_phiOffset)/m_phiPeriod);
	std::cout << std::setiosflags(std::ios::fixed)
		  << "                  centre,offset,period "
		  << std::setw(8) << std::setprecision(4) << (**i).phiCentre()
		  << std::setw(8) << std::setprecision(4) << m_phiOffset
		  << std::setw(8) << std::setprecision(4) << m_phiPeriod
	          << "   phiNumber " << std::setw(4) << phiNumber
		  << "   parity " << std::setw(2) << m_parity << std::endl;
    }
}

void
SiliconRing::refresh (void)
{
    // refresh detectotors then update limits
    std::vector<SiliconDetector*>::const_iterator d = m_detectors.begin();
    (**d).rzLimits(m_rMax,m_rMin,m_zMax,m_zMin);
    for (++d; d != m_detectors.end(); ++d)
    {
	(**d).refresh();
	double rMax;
	double rMin;
	double zMax;
	double zMin;
	(**d).rzLimits(rMax,rMin,zMax,zMin);
	if (rMax > m_rMax) m_rMax = rMax;
	if (rMin < m_rMin) m_rMin = rMin;
	if (zMax > m_zMax) m_zMax = zMax;
	if (zMin < m_zMin) m_zMin = zMin;
    }
}
