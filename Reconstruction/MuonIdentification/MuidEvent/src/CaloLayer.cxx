/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "CLHEP/Units/SystemOfUnits.h"
#include "MuidEvent/CaloLayer.h"
#include "TrkSurfaces/Surface.h"

namespace Rec
{

CaloLayer::CaloLayer(const Trk::Surface*	surface,
		     int			geom,
		     double			r,
		     double			z,
		     double			radLen,
		     bool			isBarrel)
    :	m_igeom		(geom),
	m_isBarrel	(isBarrel),
	m_R		(r),		
	m_Z		(z),
	m_surface	(surface),
	m_X0		(radLen)
{}

CaloLayer::~CaloLayer()
{}

void
CaloLayer::print(void) const
{
//     std::cout << " Layer geom " <<  m_igeom << std::endl;
    std::cout << " R " <<  m_R << std::endl;
    std::cout << " Z " <<  m_Z << std::endl;
//     std::cout << " Rmax " <<  m_Rmax << std::endl;
//     std::cout << " Rmin " <<  m_Rmin << std::endl;
//     std::cout << " Zmax " <<  m_Zmax << std::endl;
//     std::cout << " Zmin " <<  m_Zmin << std::endl;
    std::cout << " Rad Len " <<  m_X0<< std::endl;
//     std::cout << " isBarrel " <<  m_isBarrel<< std::endl;
//     std::cout << " CaloRegion " <<  m_caloregion<< std::endl;
    std::cout << std::endl;
}
 
}	// end of namespace
 
