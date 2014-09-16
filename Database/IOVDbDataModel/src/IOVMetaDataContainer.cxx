/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"

IOVMetaDataContainer::~IOVMetaDataContainer()
{
    // delete payload
    delete (m_payload);
}

bool
IOVMetaDataContainer::merge(CondAttrListCollection* payload)
{ 
    // If payload container doesn't exist, create it
    bool result;
    if (m_payload) {
        // Exists
//         std::cout << "IOVMetaDataContainer::merge - folder name " << m_folderName << std::endl;
        result = m_payload->merge(payload);
    }
    else {
        // Must make a new payload container
        m_payload = new IOVPayloadContainer;
        result = m_payload->merge(payload);
    }
    return (result);
}
