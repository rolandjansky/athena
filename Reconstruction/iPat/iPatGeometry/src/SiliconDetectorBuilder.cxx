/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 build one silicon detector element in athena (from InDetDD)
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "iPatGeometry/PixelTrapezoid.h"
#include "iPatGeometry/SiliconDetectorBuilder.h"
#include "iPatGeometry/SiliconRectangle.h"
#include "iPatGeometry/SiliconTrapezoid.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SiliconDetectorBuilder::SiliconDetectorBuilder(void)
{}

SiliconDetectorBuilder::~SiliconDetectorBuilder (void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

SiliconDetector*
SiliconDetectorBuilder::detector(const InDetDD::SiDetectorElement& element)
{
    DetectorShape* shape	= 0;
    double localEtaWidth	= 0.5*element.length();
    double localPhiWidth	= 0.5*element.width();
    if (element.isPixel())
    {
	shape = new SiliconRectangle(localEtaWidth,localPhiWidth);
    }
    else
    {
	double rzDeadRegionMax	= 0.;
	double rzDeadRegionMin	= 0.;
	const InDetDD::SCT_ModuleSideDesign* design =
	    dynamic_cast<const InDetDD::SCT_ModuleSideDesign*>(&element.design());
	if (design)
	{
	    rzDeadRegionMax	= design->deadAreaUpperBoundary();
	    rzDeadRegionMin	= design->deadAreaLowerBoundary();
	}
	
	// fixed (Tom) for the combined Test Beam with "End Cap modules in Barrel"
	if (dynamic_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(&element.design()))
	{
	    shape = new SiliconRectangle(localEtaWidth,
					 localPhiWidth,
					 rzDeadRegionMax);
	}
	else
	{
	    if (rzDeadRegionMax > rzDeadRegionMin)
	    {
		shape = new SiliconTrapezoid(localEtaWidth,
					     localPhiWidth,
					     element.center().perp(),
					     rzDeadRegionMin,
					     rzDeadRegionMax);
	    }
	    else
	    {
		shape = new SiliconTrapezoid(localEtaWidth,
					     localPhiWidth,
					     element.center().perp());
	    }
	}
    }
    
    m_shapeRiterator = m_shapes.rbegin();
    while (m_shapeRiterator != m_shapes.rend()
	   && !(**m_shapeRiterator).sameShape(*shape)) 
	++m_shapeRiterator;
    
    if (m_shapeRiterator == m_shapes.rend())
    {
	m_shapes.push_back(shape);
	m_shapeRiterator = m_shapes.rbegin();
	shape->print();
    }
    else
    {
	delete shape;
    }
    SiliconDetector* detector = new SiliconDetector(element,**m_shapeRiterator);
    return detector;
}
