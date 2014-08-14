/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 transient data object giving a hit prediction in a TRT detector element
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include <cmath>
#include "GaudiKernel/SystemOfUnits.h"
#include "InDetReadoutGeometry/TRT_BarrelElement.h"
#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "TrkSurfaces/Surface.h"
#include "TRT_Rec/TRT_Prediction.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TRT_Prediction::TRT_Prediction(const InDetDD::TRT_BarrelElement*	barrelElement,
			       const InDetDD::TRT_EndcapElement*	endcapElement,
			       IdentifierHash				hashId,
			       double					r,
			       double					z)
    :   m_barrelElement		(barrelElement),
	m_boundary		(false),
	m_deltaPhiElectron	(0.),
	m_deltaPhiElectronSign	(1.),
	m_deltaPhiNarrow	(0.),
	m_driftTolerance	(0.5*Gaudi::Units::mm),
	m_endcapElement		(endcapElement),
	m_expectedStraws	(0.),
	m_hashId		(hashId),
	m_layer			(0),
	m_phi			(0.),
	m_projectionFactor	(0.),
	m_r			(r),
	m_rPhiRoad		(0.),
	m_rRoad			(0.),
	m_strawTolerance	(2.0*Gaudi::Units::mm),
	m_z			(z)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

void
TRT_Prediction::print (void) const
{
    double nStraws;
    double phiStart;
    double pitch;
    double straw;
    double width;
    
    if (m_barrelElement)
    {
	// note phiStart refers to first straw (straw# 0)
	nStraws		= m_barrelElement->nStraws();
	phiStart	= atan2(m_barrelElement->strawYPos(0),
				m_barrelElement->strawXPos(0));
	int endStraw	= m_barrelElement->nStraws() - 1;
	double phiEnd	= atan2(m_barrelElement->strawYPos(endStraw),
				m_barrelElement->strawXPos(endStraw));
	pitch		= (phiEnd - phiStart)/(nStraws - 1);
	if (phiEnd < phiStart) pitch += 2.*M_PI/(nStraws - 1.);
	straw		= (m_phi - phiStart)/pitch;
	width		= m_deltaPhiNarrow/pitch;
	if (m_phi-phiStart < -M_PI) straw += 2.*M_PI/pitch;
	std::cout << std::setiosflags(std::ios::fixed)
		  << " barrel prediction: r,phi,z ";
    }
    else
    {
	// note phiStart refers to first straw (straw# 0)
	nStraws		= m_endcapElement->getDescriptor()->nStraws();
	phiStart	= m_endcapElement->getDescriptor()->startPhi();
	pitch		= m_endcapElement->getDescriptor()->strawPitch();
	straw		= (m_phi - phiStart)/pitch;
	width		= m_deltaPhiNarrow/pitch;
	if (m_phi-phiStart < -M_PI) straw += 2.*M_PI/pitch;
	std::cout << std::setiosflags(std::ios::fixed)
		  << " endcap prediction: r,phi,z ";
    }
    std::cout << std::setw(9) << std::setprecision(3) << m_r
	      << std::setw(9) << std::setprecision(4) << m_phi
	      << std::setw(9) << std::setprecision(1) << m_z
	      << "   straw#"  << std::setw(5) << std::setprecision(1) << straw
	      << " +-" << std::setw(5) << std::setprecision(1) << width;
    straw += 0.5;
    if (m_boundary || straw > nStraws || straw < 0.) std::cout << "  near boundary ";
    std::cout << std::resetiosflags(std::ios::fixed) << std::endl;
}

const Trk::Surface&
TRT_Prediction::surface (void) const
{
    if (m_barrelElement)
    {
	return m_barrelElement->surface();
    }
    else
    {
	return m_endcapElement->surface();
    }
}







