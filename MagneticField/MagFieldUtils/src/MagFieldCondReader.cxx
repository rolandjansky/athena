/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MagFieldCondReader.h"

MagField::CondReader::CondReader(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode MagField::CondReader::initialize()
{
  ATH_CHECK(m_fieldCacheKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode MagField::CondReader::execute(const EventContext& ctx) const
{
  SG::ReadCondHandle<AtlasFieldCacheCondObj> rh{m_fieldCacheKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*rh};
  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheKey.key());
    return StatusCode::FAILURE;
  }

  MagField::AtlasFieldCache fieldCache;
  fieldCondObj->getInitializedCache(fieldCache);

  ATH_MSG_INFO("Field status: solenoid=" << fieldCache.solenoidOn() << ", toroids=" << fieldCache.toroidOn());

  // get field at 0,0,0
  double xyz[3] = { 100, 100, 100 };
  double bxyz[3];
  fieldCache. getField(xyz, bxyz);

  ATH_MSG_INFO("Field xyz: " << xyz[0] << ", " << xyz[1] << ", " << xyz[2] << ", " << bxyz[0] << ", " << bxyz[1] << ", " << bxyz[2]);
  


  
  return StatusCode::SUCCESS;
}
