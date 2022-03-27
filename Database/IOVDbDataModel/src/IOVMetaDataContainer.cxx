/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"

IOVMetaDataContainer::~IOVMetaDataContainer()
{
  delete (m_payload);
}

bool
IOVMetaDataContainer::merge(CondAttrListCollection* payload)
{
  return m_payload->merge(payload);
}

void IOVMetaDataContainer::dump(std::ostringstream& stream) const
{
  stream << "IOVMetaDataContainer ::" << std::endl;
  stream << "Payload size : " << m_payload->size() << std::endl;
  stream << "IOVs and attribute lists : " << std::endl;

  for(CondAttrListCollection* attListCol : *m_payload ) {
    attListCol->dump(stream);
  }
}
