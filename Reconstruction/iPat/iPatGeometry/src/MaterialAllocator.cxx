/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 hard-wired material properties for silicon layers, inert structure and Trt
 (radiation thickness, energy loss coefficients)
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "iPatGeometry/MaterialAllocator.h"
#include "iPatGeometry/SiliconLayer.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

MaterialAllocator::MaterialAllocator (std::string versionName)
    :	m_useTDR_Material	(false),
	m_versionName		(versionName)
{
    if (m_versionName != "DC1"
	&& m_versionName != "DC2"
	&& m_versionName != "DC3")
    {
	// TODO: should flag problem
	m_versionName == "DC3";
    }
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

std::pair<double,double>
MaterialAllocator::layerMaterial (const SiliconLayer&	layer)
{
    double energyLoss	= 0.;
    double radThickness	= 0.;
    if (layer.name() == "pixel")
    {
	// DC1 estimates from Dario Barberis
        // adjustment from tuning pulls
	if (layer.isBarrel())
	{
	    if (m_useTDR_Material)
	    {
		if (layer.rzCentre() < 60.0)	// b-layer is thinner
		{
		    energyLoss		= 0.68*Gaudi::Units::MeV;
		    radThickness	= 0.0135;
		}
		else
		{
		    energyLoss		= 0.75*Gaudi::Units::MeV;
		    radThickness	= 0.0150;
		}
	    }
	    else if (m_versionName == "DC3")
	    {
		energyLoss	= 2.0*Gaudi::Units::MeV;
		radThickness	= 0.030;
	    }
	    else 	// DC1 or DC2
	    {
		energyLoss	= 1.15*Gaudi::Units::MeV;
		radThickness	= 0.020;
	    }
	}
	else
	{
	    if (m_useTDR_Material)
	    {
		energyLoss    	= 0.74*Gaudi::Units::MeV;
		radThickness	= 0.0148;
	    }
	    else if (m_versionName == "DC3")
	    {
		energyLoss	= 1.44*Gaudi::Units::MeV;
		radThickness	= 0.025;
	    }
	    else 	// DC1 or DC2
	    {
		energyLoss    	= 0.62*Gaudi::Units::MeV;
		radThickness	= 0.0123;
	    }
	}
    }
    else if (layer.name() == "SCT")
    {
	// SCT DC1 estimates from Grant Gorfine (barrel = endcap = 2.2%)
        // with subsequent adjustment from tuning pulls
	if (layer.isBarrel())
	{
	    energyLoss     = 1.10*Gaudi::Units::MeV;
	    radThickness   = 0.0190;  // GG 2.2%
	}
	else
	{
	    energyLoss     = 1.10*Gaudi::Units::MeV;
	    radThickness   = 0.0240;  // GG 2.2%
	}
    }
    else if (layer.name() == "beamPipe")
    {
	// double skin beam pipe (per skin)
	energyLoss	= 0.106*Gaudi::Units::MeV;
	radThickness	= 0.0023;
    }
    else if (layer.name() == "pixelServiceDisk")
    {
	energyLoss	= 2.50*Gaudi::Units::MeV;
	radThickness	= 0.050;
    }
    else if (layer.name() == "pixelSupportCylinder")
    {
	energyLoss     	= 1.50*Gaudi::Units::MeV;
	radThickness	= 0.030;
    }
    else if (layer.name() == "pixelEndCapServiceCylinder")
    {
	energyLoss	= 2.0*Gaudi::Units::MeV;
	radThickness	= 0.040;
    }
    else if (layer.name() == "pixelBarrelServiceCylinder")
    {
	energyLoss	= 0.36*Gaudi::Units::MeV;
	radThickness	= 0.0072;
    }
    else if (layer.name() == "outerSupportCylinder")
    {
	energyLoss	= 0.36*Gaudi::Units::MeV;
	radThickness	= 0.0072;
    }
    else if (layer.name() == "sctServiceDisk0")
    {
	energyLoss	= 2.35*Gaudi::Units::MeV;
	radThickness	= 0.047;
    }
    else if (layer.name() == "sctServiceDisk1")
    {
	energyLoss	= 1.20*Gaudi::Units::MeV;
	radThickness	= 0.023;
    }
    else if (layer.name() == "sctServiceDisk2")
    {
	energyLoss	= 5.0*Gaudi::Units::MeV;
	radThickness	= 0.10;
    }
    return std::make_pair(energyLoss,radThickness);
}
