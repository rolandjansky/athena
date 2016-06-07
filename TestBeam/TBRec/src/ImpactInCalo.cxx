/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TBRec/ImpactInCalo.h"
#include <iostream>


void ImpactInCalo::print() const {

  std::cout << "Impact in PS: x,y,z=" << m_x_0 << " " << m_y_0 
	    << " " << m_z_0
	    << " local eta,phi=" << m_etaCaloLocal_0 << " " 
	    << m_phiCaloLocal_0
	    << std::endl;

  std::cout << "Impact in Strip: x,y,z=" << m_x_1 << " " << m_y_1 
	    << " " << m_z_1
	    << " local eta,phi=" << m_etaCaloLocal_1 << " " 
	    << m_phiCaloLocal_1
	    << std::endl;

  std::cout << "Impact in Middle: x,y,z=" << m_x_2 << " " << m_y_2 
	    << " " << m_z_2
	    << " local eta,phi=" << m_etaCaloLocal_2 
	    << " " << m_phiCaloLocal_2
	    << std::endl;

  std::cout << "Impact in Back: x,y,z=" << m_x_3 << " " << m_y_3 
	    << " " << m_z_3
	    << " local eta,phi=" << m_etaCaloLocal_3 << " " 
	    << m_phiCaloLocal_3
	    << std::endl;

  std::cout << "Impact in Tiles: x,y,z=" << m_x_tile << " " 
	    << m_y_tile << " " << m_z_tile
	    << " local eta,phi=" << m_etaCaloLocal_tile 
	    << " " << m_phiCaloLocal_tile
	    << std::endl;
}
