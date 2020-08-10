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

  return StatusCode::SUCCESS;
}
