/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelHitDiscCnfgData.h"

PixelHitDiscCnfgData::PixelHitDiscCnfgData()
{
}

PixelHitDiscCnfgData::~PixelHitDiscCnfgData() { }

void PixelHitDiscCnfgData::setHitDiscCnfgPL(int hitDiscCnfgPL) { m_hitDiscCnfgPL=hitDiscCnfgPL; }
void PixelHitDiscCnfgData::setHitDiscCnfg3D(int hitDiscCnfg3D) { m_hitDiscCnfg3D=hitDiscCnfg3D; }
int PixelHitDiscCnfgData::getHitDiscCnfgPL() const { return m_hitDiscCnfgPL; }
int PixelHitDiscCnfgData::getHitDiscCnfg3D() const { return m_hitDiscCnfg3D; }

