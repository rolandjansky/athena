/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"

#include <cassert>
#include <iostream>

Identifier 
SCT_Identifier::getWaferId() 
{
  assert (m_idHelper);
  return m_idHelper->wafer_id(m_barrelEC, m_layerDisk, m_phiModule, m_etaModule, m_side);
}

void SCT_Identifier::print()
{
  std::cout << "2/2/" << m_barrelEC << "/" 
            << m_layerDisk << "/" 
            << m_phiModule << "/"
            << m_etaModule << "/"
            << m_side << std::endl;
}
