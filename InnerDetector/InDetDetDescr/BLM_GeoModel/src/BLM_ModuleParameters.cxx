/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "BLM_GeoModel/BLM_ModuleParameters.h"

void BLM_ModuleParameters::Set(int module_number, const std::vector<double>* module_property)
{
  m_module_number = module_number;
  m_trans_X = (*module_property)[0];
  m_trans_Y = (*module_property)[1];
  m_trans_Z = (*module_property)[2];
  m_rot_X = (*module_property)[3];
  m_rot_Y = (*module_property)[4];
  m_rot_Z = (*module_property)[5];
  m_diamond_trans_X = (*module_property)[6];
  m_diamond_trans_Y = (*module_property)[7];
  m_diamond_trans_Z = (*module_property)[8];
}
