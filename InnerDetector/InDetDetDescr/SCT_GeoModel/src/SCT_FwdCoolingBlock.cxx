/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdCoolingBlock.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

#include <iostream>

SCT_FwdCoolingBlock::SCT_FwdCoolingBlock(const std::string & name, int hiLo, int mainOrSecondary,
                                         InDetDD::SCT_DetectorManager* detectorManager,
                                         const SCT_GeometryManager* geometryManager,
                                         SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_hiLo(hiLo), m_mainSec(mainOrSecondary)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_FwdCoolingBlock::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
    
  m_coolingBlockIndex = -1;
  for (int i = 0; i < 4; i++){
    if (parameters->fwdCoolingBlockHiLo(i) == m_hiLo && parameters->fwdCoolingBlockMainOrSecondary(i) == m_mainSec) {
      m_coolingBlockIndex = i;
    }
  }

  if (m_coolingBlockIndex < 0){
    std::cout << "SCT_FwdCoolingBlock: ERROR. Cooling block type is missing. HiLo = " << m_hiLo 
              << ", MainSecondary = " << m_mainSec << std::endl;
    // Will crash or give unpredictable results
  } 

  m_materialName = parameters->fwdCoolingBlockMaterial(m_coolingBlockIndex);
  m_thickness = parameters->fwdCoolingBlockThickness(m_coolingBlockIndex);
  m_deltaR    = parameters->fwdCoolingBlockDeltaR(m_coolingBlockIndex);
  m_rphi      = parameters->fwdCoolingBlockRPhi(m_coolingBlockIndex);
  m_offset    = parameters->fwdCoolingBlockOffsetFromDisc(m_coolingBlockIndex);


}

GeoVPhysVol * 
SCT_FwdCoolingBlock::build()
{

  // Build the CoolingBlock. Just a simple box.
  const GeoBox * coolingBlockShape = new GeoBox(0.5*m_deltaR, 0.5*m_rphi, 0.5*m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName, coolingBlockShape->volume());
  const GeoLogVol *coolingBlockLog = 
    new GeoLogVol(getName(), coolingBlockShape, m_material);
  GeoPhysVol * coolingBlock = new GeoPhysVol(coolingBlockLog);

  return coolingBlock;
}
