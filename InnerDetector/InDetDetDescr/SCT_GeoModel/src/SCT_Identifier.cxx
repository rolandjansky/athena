/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"

#include <cassert>
#include <iostream>

const SCT_ID * SCT_Identifier::s_idHelper = 0;  

Identifier 
SCT_Identifier::getWaferId() 
{
  assert (s_idHelper);
  return s_idHelper->wafer_id(m_barrelEC, m_layerDisk, m_phiModule, m_etaModule, m_side);
}

void SCT_Identifier::print()
{
  std::cout << "2/2/" << m_barrelEC << "/" 
	    << m_layerDisk << "/" 
	    << m_phiModule << "/"
	    << m_etaModule << "/"
	    << m_side << std::endl;
}
