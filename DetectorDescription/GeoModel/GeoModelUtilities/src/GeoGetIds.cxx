/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeoModelUtilities/src/GeoGetIds.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Visitor to collect all IDs under a GeoModel node.
 */


#include "GeoModelUtilities/GeoGetIds.h"


GeoGetIds::GeoGetIds (IGeoGetIdsAction& action,
                      int depthLimit /*= 1*/)
  : m_action (action)
{
  setDepthLimit (depthLimit);
}


void GeoGetIds::handleSerialTransformer (const GeoSerialTransformer  *sT)
{
  if (m_serialIdentifier) {
    int idbase = m_serialIdentifier->getBaseId();
    for (unsigned int i=0; i < sT->getNCopies(); i++) {
      m_action (idbase + i);
    }
  }
}


void GeoGetIds::handleIdentifierTag (const GeoIdentifierTag *idTag)
{
  m_serialIdentifier = nullptr;
  m_action (idTag->getIdentifier());
}


void GeoGetIds::handleSerialIdentifier(const GeoSerialIdentifier *sI)
{
  m_serialIdentifier = sI;
}
