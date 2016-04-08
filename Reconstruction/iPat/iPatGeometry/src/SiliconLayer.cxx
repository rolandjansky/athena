/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes the geometry of a silicon barrel layer or endcap wheel.
 The layer is decomposed into a vector of rings representing phi symmetry.
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "iPatGeometry/MaterialAllocator.h"
#include "iPatGeometry/SiliconDetector.h"
#include "iPatGeometry/SiliconLayer.h"
#include "iPatGeometry/SiliconRing.h"

//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>

const double	SiliconLayer::s_safetyDistance	= 1.0*Gaudi::Units::mm;  // allow 1 mm around edge

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SiliconLayer::SiliconLayer(SiliconRing*	ring)
    :	m_isActive		(true),
	m_isBarrel		(ring->isBarrel()),
	m_isPixel		(ring->isPixel()),
	m_isStrip		(!ring->isPixel()),
	m_negativeSurface	(0),
	m_positiveSurface	(0),
	m_rMax			(ring->rMax()),
	m_rMin			(ring->rMin()),
	m_rzCentre            	(0.),
	m_zrBegin             	(0.),
	m_zrCentre            	(0.),
	m_zrEnd      		(0.),
	m_zrPeriod		(0.)
{
    // set name
    if (m_isPixel)
    {
	m_name = "pixel";
    }
    else
    {
	m_name = "SCT";
    }
    
    m_rings.push_back(ring);
    ring->setParent(this);
    if (m_isBarrel)
    {
	m_tolerance    	= 10.0*Gaudi::Units::mm;
	m_zMax 		= ring->zMax();
	m_zMin		= ring->zMin();
    }
    else
    {
	m_tolerance	= 30.0*Gaudi::Units::mm;
	m_zMax		= std::abs(ring->zMax());
	m_zMin		= std::abs(ring->zMin());
    }
    if (m_isPixel)
    {
	m_multiple      = 2;
    }
    else
    {
	m_multiple      = 4;
    }
}

SiliconLayer::SiliconLayer(std::string	name,
			   double	rzCentre,
			   double	zrMin,
			   double	zrMax)
    :	m_isActive		(false),
	m_isPixel		(false),
	m_isStrip		(false),
	m_multiple		(1),
	m_name			(name),
	m_negativeSurface	(0),
	m_positiveSurface	(0),
	m_rMax			(zrMax),
	m_rMin			(zrMin),
	m_rzCentre            	(rzCentre),
	m_zMax			(zrMax),
	m_zMin			(zrMin),
	m_zrBegin             	(0.),
	m_zrCentre            	(0.),
	m_zrEnd      		(0.),
	m_zrPeriod		(0.)
{
    if (name == "beamPipe"
	|| name == "pixelBarrelServiceCylinder"
	|| name == "pixelEndCapServiceCylinder"
	|| name == "pixelSupportCylinder")
    {
	m_isBarrel	= true;
	m_isPixel 	= true;
    }
    else if (name == "outerSupportCylinder")
    {
	m_isBarrel	= true;
    }
    else
    {
	m_isBarrel	= false;
	if (name == "pixelServiceDisk") m_isPixel = true;
    }
    
    m_tolerance = 1.0*Gaudi::Units::mm;
    if (m_isBarrel)
    {
	m_rMax				= m_rzCentre + m_tolerance;
	m_rMin				= m_rzCentre - m_tolerance;
	m_zrCentre    			= 0.5*(m_zMax + m_zMin);
	Amg::Transform3D* transform	= new Amg::Transform3D;
	(*transform)			= Amg::Translation3D(0.,0.,m_zrCentre);
	m_negativeSurface 		= new Trk::CylinderSurface(transform,
								   m_rzCentre,
								   std::abs(m_zMax));
	m_positiveSurface		= m_negativeSurface;
    }
    else
    {
	m_zMax				= m_rzCentre + m_tolerance;
	m_zMin				= m_rzCentre - m_tolerance;
	m_zrCentre			= 0.5*(m_rMax + m_rMin);
	Amg::Transform3D* transformNeg	= new Amg::Transform3D;
	(*transformNeg)			= Amg::Translation3D(0.,0.,-std::abs(m_rzCentre));
	m_negativeSurface		= new Trk::DiscSurface(transformNeg, m_rMin, m_rMax);
	Amg::Transform3D* transformPos	= new Amg::Transform3D;
	(*transformPos)			= Amg::Translation3D(0.,0.,+std::abs(m_rzCentre));
	m_positiveSurface		= new Trk::DiscSurface(transformPos, m_rMin, m_rMax);
    }
}

SiliconLayer::~SiliconLayer(void)
{
    if (m_negativeSurface != m_positiveSurface) delete m_negativeSurface;
    delete m_positiveSurface;
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

bool
SiliconLayer::addRing(SiliconRing* ring)
{
    // don't mix barrel with endcap or pixels with sct or rings with inert layers
    if (!m_isActive
	|| ring->isBarrel()	!= m_isBarrel 
	|| ring->isPixel()	!= m_isPixel) return false;

    // otherwise merge if overlap within r/z tolerance
    if (m_isBarrel)
    {
	if (ring->zMax() - ring->zMin()	< ring->rMax() - ring->rMin()
	    || m_rMin - ring->rMax()		> m_tolerance
	    || ring->rMin() - m_rMax		> m_tolerance)	return false;
	// redefine layer z-limits
	if (ring->zMax() > m_zMax) m_zMax = ring->zMax();
	if (ring->zMin() < m_zMin) m_zMin = ring->zMin();
    }
    else
    {
	if (ring->zMax() - ring->zMin()	> ring->rMax() - ring->rMin()
	    || m_zMin - std::abs(ring->zMax())	> m_tolerance
	    || std::abs(ring->zMin()) - m_zMax	> m_tolerance)	return false;
	// redefine layer z-limits (use abs z-values in end-cap)
	if (std::abs(ring->zMax()) > m_zMax) m_zMax = std::abs(ring->zMax());
	if (std::abs(ring->zMin()) < m_zMin) m_zMin = std::abs(ring->zMin());
    }

    // redefine layer r-limits
    if (ring->rMax() > m_rMax) m_rMax = ring->rMax();
    if (ring->rMin() < m_rMin) m_rMin = ring->rMin();
    m_rings.push_back(ring);
    ring->setParent(this);
    return true;
}
		 
void
SiliconLayer::determineMaterial(std::string versionName)
{
    // precompute material for named layers
    if (m_name == "") return;
    MaterialAllocator materialAllocator(versionName);
    m_materialProperties = materialAllocator.layerMaterial(*this);
}

void
SiliconLayer::determineRzProperties(void)
{
    // no ring concept for inactive (and don't want safety factor in overlap regions)
    if (! m_isActive) return;
    
    // order detectors by increasing rz
    std::sort (m_rings.begin(), m_rings.end(), SiliconRing::rzOrder());

    // hashed ring lookup with cached m_zrBegin, m_zrEnd, m_zrPeriod.
    // Take into account 2 parities, phi/stereo in SCT
    // and forwards/backwards in end-caps
    if (m_isBarrel)
    {
	m_rzCentre		= 0.5*(m_rMax + m_rMin);
	m_zrCentre		= 0.5*(m_zMax + m_zMin);
	if (m_rings.size() <= static_cast<unsigned>(m_multiple))
        {
	    m_multiple	= 0;
	    m_zrBegin	= m_zMin;
	    m_zrEnd	= m_zMax;
	    m_zrPeriod	= m_zMax - m_zMin;
	}
	else
	{
	    ring_const_iterator j	= m_rings.begin();
	    for (ring_const_iterator i = j + m_multiple;
		 i != m_rings.end();
		 ++i, ++j)
	    {
		double period = (**i).zMin() - (**j).zMin();
		if (period > m_zrPeriod) m_zrPeriod = period;
	    }
	    m_zrBegin  = (**m_rings.begin()).zMax() - m_zrPeriod;
	    if (m_zrBegin < (**m_rings.begin()).zMin())
		m_zrBegin = (**m_rings.begin()).zMin();
	    m_zrEnd    = (**(m_rings.end()-m_multiple)).zMin() + m_zrPeriod;
	    if (m_zrEnd > (**(m_rings.end()-m_multiple)).zMax())
		m_zrEnd = (**(m_rings.end()-m_multiple)).zMax();
	}
    }
    else
    {
	m_rzCentre		= 0.5*(m_zMax + m_zMin);
	m_zrCentre		= 0.5*(m_rMax + m_rMin);
	if (m_rings.size() <= static_cast<unsigned>(m_multiple))
        {
	    m_multiple	= 0;
	    m_zrBegin	= m_rMin;
	    m_zrEnd	= m_rMax;
	    m_zrPeriod	= m_rMax - m_rMin;
	}
	else
	{
	    ring_const_iterator j	= m_rings.begin();
	    ring_const_iterator k	= j + m_rings.size()/2;
	    for (ring_const_iterator i = j + m_multiple;
		 i != m_rings.end();
		 ++i, ++j)
	    {
		if (i < k || j >= k)
		{
		    double period = (**i).rMin() - (**j).rMin();
		    if (period > m_zrPeriod) m_zrPeriod = period;
		}
	    }
	    if (m_zrPeriod == 0.) m_zrPeriod = m_rMax - m_rMin;
	    m_zrBegin  = (**m_rings.begin()).rMax() - m_zrPeriod;
	    if (m_zrBegin < (**m_rings.begin()).rMin())
		m_zrBegin = (**m_rings.begin()).rMin();
	    m_zrEnd    = (**(m_rings.end()-m_multiple)).rMin() + m_zrPeriod;
	    if (m_zrEnd > (**(m_rings.end()-m_multiple)).rMax())
		m_zrEnd = (**(m_rings.end()-m_multiple)).rMax();
	}
    }

    // add something (~1mm) for safety to rz boundary
    m_rMax += s_safetyDistance;
    m_rMin -= s_safetyDistance;
    m_zMax += s_safetyDistance;
    m_zMin -= s_safetyDistance;

    // create surface
    if (m_isBarrel)
    {
	Amg::Transform3D* transform	= new Amg::Transform3D;
	(*transform)			= Amg::Translation3D(0.,0.,m_zrCentre);
	m_negativeSurface 		= new Trk::CylinderSurface(transform,
								   m_rzCentre,
								   std::abs(m_zMax));
	m_positiveSurface		= m_negativeSurface;
	
	// std::cout << std::setiosflags(std::ios::fixed)
	// 	  << " barrel layer: rzCentre "
	// 	  << std::setw(7) << std::setprecision(1) << m_rzCentre
	// 	  << "  zMax " << std::setw(8) << std::setprecision(2)
	// 	  << std::abs(m_zMax) << std::endl;
    }
    else
    {
	// double radius			= 0.;
	Amg::Transform3D* transformNeg	= new Amg::Transform3D;
	(*transformNeg)			= Amg::Translation3D(0.,0.,-std::abs(m_rzCentre));
	m_negativeSurface		= new Trk::DiscSurface(transformNeg, m_rMin, m_rMax);
	Amg::Transform3D* transformPos	= new Amg::Transform3D;
	(*transformPos)			= Amg::Translation3D(0.,0.,+std::abs(m_rzCentre));
	m_positiveSurface		= new Trk::DiscSurface(transformPos, m_rMin, m_rMax);
    }
}

std::pair<double,double>
SiliconLayer::materialProperties (const Amg::Vector3D&	/*intersect*/, 
				  const Amg::Vector3D&	transverseDirection) const
{ 
    // this function is used for inactive material.
    // It corrects for track crossing angle
    // projection factor is 1/sin(theta) [1./cos(theta)] in barrel [endcap]
    double projection	= std::sqrt(1.+ transverseDirection.z()*transverseDirection.z());
    if (!m_isBarrel) projection /= std::abs(transverseDirection.z());

    //std::cout << " material " << m_name << " " << intersect.perp() << std::endl;
    double energyLoss	= m_materialProperties.first;
    double radThickness	= m_materialProperties.second;

    return std::make_pair(projection*energyLoss,projection*radThickness);
}

void
SiliconLayer::print(void) const
{
    if (m_isBarrel)
    {
	std::cout << std::setiosflags(std::ios::fixed)
		  << " barrel layer: " << m_rings.size()
		  << " rings  rMin,rMax"
		  << std::setw(7) << std::setprecision(1) << m_rMin
		  << std::setw(7) << std::setprecision(1) << m_rMax
		  << "  zMin,zMax"
		  << std::setw(8) << std::setprecision(2) << m_zMin
		  << std::setw(8) << std::setprecision(2) << m_zMax
		  << "  begin,end,period"
		  << std::setw(8) << std::setprecision(2) << m_zrBegin
		  << std::setw(8) << std::setprecision(2) << m_zrEnd
		  << std::setw(8) << std::setprecision(2) << m_zrPeriod
		  << std::resetiosflags(std::ios::fixed) << std::endl;
    }
    else
    {
	std::cout << std::setiosflags(std::ios::fixed)
		  << " endcap wheel: " << m_rings.size()
		  << " rings  rMin,rMax"
		  << std::setw(7) << std::setprecision(1) << m_rMin
		  << std::setw(7) << std::setprecision(1) << m_rMax
		  << "  zMin,zMax"
		  << std::setw(8) << std::setprecision(2) << m_zMin
		  << std::setw(8) << std::setprecision(2) << m_zMax
		  << "  begin,end,period"
		  << std::setw(8) << std::setprecision(2) << m_zrBegin
		  << std::setw(8) << std::setprecision(2) << m_zrEnd
		  << std::setw(8) << std::setprecision(2) << m_zrPeriod
		  << std::resetiosflags(std::ios::fixed) << std::endl;
    }

    for (ring_const_iterator i = m_rings.begin();
	 i != m_rings.end();
	 ++i)
    {
	std::cout << std::setiosflags(std::ios::fixed)
		  << "    ring:" << std::setw(3) << (i - m_rings.begin())
		  << "  rMin,rMax"
		  << std::setw(7) << std::setprecision(1) << (**i).rMin()
		  << std::setw(7) << std::setprecision(1) << (**i).rMax()
		  << "  zMin,zMax"
		  << std::setw(8) << std::setprecision(2) << (**i).zMin()
		  << std::setw(8) << std::setprecision(2) << (**i).zMax()
		  << "   detector (with phi closest to 0): phiCentre,sinStereo,sinTilt"
		  << std::setw(8) << std::setprecision(4)
		  << (**i).detector(0.)->phiCentre()
		  << std::setw(9) << std::setprecision(4)
		  << (**i).detector(0.)->element().sinStereo()
		  << std::setw(9) << std::setprecision(4)
		  << (**i).detector(0.)->element().sinTilt()
		  << std::resetiosflags(std::ios::fixed) << std::endl;
    }
}

void
SiliconLayer::refresh (void)
{
    // do nothing for inactive material
    if (! m_isActive) return;

    // refresh rings and limits
    m_rMax = -99999.;
    m_rMin =  99999.;
    m_zMax = -99999.;
    m_zMin =  99999.;
    for (std::vector<SiliconRing*>::iterator r = m_rings.begin();
	 r != m_rings.end();
	 ++r)
    {
	(**r).refresh();
	if ((**r).rMax() > m_rMax) m_rMax = (**r).rMax();
	if ((**r).rMin() < m_rMin) m_rMin = (**r).rMin();
	if (m_isBarrel)
	{
	    if ((**r).zMax() > m_zMax) m_zMax = (**r).zMax();
	    if ((**r).zMin() < m_zMin) m_zMin = (**r).zMin();
	}
	else
	{
	    if (std::abs((**r).zMax()) > m_zMax) m_zMax = std::abs((**r).zMax());
	    if (std::abs((**r).zMin()) < m_zMin) m_zMin = std::abs((**r).zMin());
	}
    }

    // not sure that this is necessary !
    if (m_isBarrel)
    {
	m_rzCentre		= 0.5*(m_rMax + m_rMin);
	m_zrCentre		= 0.5*(m_zMax + m_zMin);
	if (m_rings.size() <= static_cast<unsigned>(m_multiple))
        {
	    m_multiple	= 0;
	    m_zrBegin	= m_zMin;
	    m_zrEnd	= m_zMax;
	    m_zrPeriod	= m_zMax - m_zMin;
	}
	else
	{
	    ring_const_iterator j	= m_rings.begin();
	    for (ring_const_iterator i = j + m_multiple;
		 i != m_rings.end();
		 ++i, ++j)
	    {
		double period = (**i).zMin() - (**j).zMin();
		if (period > m_zrPeriod) m_zrPeriod = period;
	    }
	    m_zrBegin  = (**m_rings.begin()).zMax() - m_zrPeriod;
	    if (m_zrBegin < (**m_rings.begin()).zMin())
		m_zrBegin = (**m_rings.begin()).zMin();
	    m_zrEnd    = (**(m_rings.end()-m_multiple)).zMin() + m_zrPeriod;
	    if (m_zrEnd > (**(m_rings.end()-m_multiple)).zMax())
		m_zrEnd = (**(m_rings.end()-m_multiple)).zMax();
	}
    }
    else 
    {
	m_rzCentre		= 0.5*(m_zMax + m_zMin);
	m_zrCentre		= 0.5*(m_rMax + m_rMin);
	if (m_rings.size() <= static_cast<unsigned>(m_multiple))
        {
	    m_multiple	= 0;
	    m_zrBegin	= m_rMin;
	    m_zrEnd	= m_rMax;
	    m_zrPeriod	= m_rMax - m_rMin;
	}
	else
	{
	    ring_const_iterator j	= m_rings.begin();
	    ring_const_iterator k	= j + m_rings.size()/2;
	    for (ring_const_iterator i = j + m_multiple;
		 i != m_rings.end();
		 ++i, ++j)
	    {
		if (i < k || j >= k)
		{
		    double period = (**i).rMin() - (**j).rMin();
		    if (period > m_zrPeriod) m_zrPeriod = period;
		}
	    }
	    if (m_zrPeriod == 0.) m_zrPeriod = m_rMax - m_rMin;
	    m_zrBegin  = (**m_rings.begin()).rMax() - m_zrPeriod;
	    if (m_zrBegin < (**m_rings.begin()).rMin())
		m_zrBegin = (**m_rings.begin()).rMin();
	    m_zrEnd    = (**(m_rings.end()-m_multiple)).rMin() + m_zrPeriod;
	    if (m_zrEnd > (**(m_rings.end()-m_multiple)).rMax())
		m_zrEnd = (**(m_rings.end()-m_multiple)).rMax();
	}
    }

    // finally add something (~1mm) for safety to rz boundary
    m_rMax += s_safetyDistance;
    m_rMin -= s_safetyDistance;
    m_zMax += s_safetyDistance;
    m_zMin -= s_safetyDistance;
}

SiliconLayer::ring_vector*
SiliconLayer::rings (double cotTheta, double r, double z, double zrTolerance) const
{
    // returns vector of rz-compatible rings
    ring_const_iterator iBegin	= m_rings.begin();
    ring_const_iterator iEnd	= m_rings.end();
    double		zrMax;
    double		zrMin;
    if (m_isBarrel)
    {
	if (cotTheta > 0.)
	{
	    zrMax = z + zrTolerance - cotTheta*(r - m_rMax);
	    zrMin = z - zrTolerance - cotTheta*(r - m_rMin);
	}
	else
	{
	    zrMax = z + zrTolerance - cotTheta*(r - m_rMin);
	    zrMin = z - zrTolerance - cotTheta*(r - m_rMax);
	}
    }
    else
    {
	if (cotTheta > 0.)
	{
	    iBegin	+= m_rings.size()/2;
	    zrMax	=  r + zrTolerance - (z - m_zMax)/cotTheta;
	    zrMin	=  r - zrTolerance - (z - m_zMin)/cotTheta;
	}
	else
	{
	    iEnd	-= m_rings.size()/2;
	    zrMax	=  r + zrTolerance - (z + m_zMax)/cotTheta;
	    zrMin	=  r - zrTolerance - (z + m_zMin)/cotTheta;
	}
    }

    //      // needed for validation
    //      ring_const_iterator iB	= iBegin;
    //      ring_const_iterator iE	= iEnd;

    // compute ring-iterators spanning zr range 
    if (zrMin > m_zrBegin)
	iBegin	+= m_multiple*static_cast<int> ((zrMin - m_zrBegin)/m_zrPeriod);
    if (m_zrEnd > zrMax)
	iEnd	-= m_multiple*static_cast<int> ((m_zrEnd - zrMax)/m_zrPeriod);
    // catch some asymmetric layouts (like wheel 8)
    if (iBegin >= iEnd)
    {
	if (zrMin > m_zrBegin)
	{
	    iBegin	= iEnd - m_multiple;
	}
	else
	{
	    iEnd	= iBegin + m_multiple;
	}
    }

    // create the selected rings
    ring_vector* 	rings	= new ring_vector(iBegin,iEnd);

    //      // debug
    //      std::cout << std::setiosflags(std::ios::fixed)
    //  	      << "    range:" << std::setw(3) << (iBegin - m_rings.begin())
    //  	      << std::setw(3) << (iEnd - m_rings.begin())
    //  	      << "  zrMin,zrMax"
    //  	      << std::setw(8) << std::setprecision(3) << zrMin
    //  	      << std::setw(8) << std::setprecision(3) << zrMax
    //  	      << std::resetiosflags(std::ios::fixed) << std::endl;
    //      print();

    //      // validation
    //      if (m_isBarrel)
    //      {
    //  	for (ring_const_iterator i = iB; i != iBegin; ++i)
    //  	    assert(zrMin > (**i).zMax());
    
    //  	for (ring_const_iterator i = iEnd; i != iE; ++i)
    //  	    assert(zrMax < (**i).zMin());
    //      }
    //      else
    //      {
    //  	for (ring_const_iterator i = iB; i != iBegin; ++i)
    //  	    assert(zrMin > (**i).rMax());

    //  	for (ring_const_iterator i = iEnd; i != iE; ++i)
    //  	    assert(zrMax < (**i).rMin());
    //      }

    return rings;
}
