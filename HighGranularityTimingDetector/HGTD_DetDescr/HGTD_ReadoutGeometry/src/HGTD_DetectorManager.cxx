/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

using InDetDD::HGTD_DetectorElementCollection;
using InDetDD::HGTD_DetectorElement;
using InDetDD::SiCommonItems;

HGTD_DetectorManager::HGTD_DetectorManager(StoreGateSvc* detStore)
    : AthMessaging(Athena::getMessageSvc(), "HGTD_DetectorManager"),
      m_idHelper(0)
{
    setName("HGTD");

    //
    // Initialize the Identifier helper
    //
    StatusCode sc = detStore->retrieve(m_idHelper,"HGTD_ID");
    if (sc.isFailure() ) {
        ATH_MSG_ERROR ("Could not retrieve HGTD id helper");
    }

    // Initialize the collections
    if (m_idHelper) {
        m_elementCollection.resize(m_idHelper->wafer_hash_max());
    }
}

HGTD_DetectorManager::~HGTD_DetectorManager()
{
    // Clean up
    HGTD_DetectorElementCollection::iterator iter;
    for (iter = m_elementCollection.begin(); iter != m_elementCollection.end(); ++iter){
        delete *iter;
    }

    for (size_t i=0; i < m_volume.size(); i++) {
        m_volume[i]->unref();
    }
}

unsigned int HGTD_DetectorManager::getNumTreeTops() const
{
    return m_volume.size();
}

PVConstLink HGTD_DetectorManager::getTreeTop(unsigned int i) const
{
    return m_volume[i];
}

void HGTD_DetectorManager::addTreeTop(PVLink vol){
    vol->ref();
    m_volume.push_back(vol);
}

HGTD_DetectorElement* HGTD_DetectorManager::getDetectorElement(const Identifier & id) const
{
    // Make sure it is a wafer Id
    Identifier waferId =  m_idHelper->wafer_id(id);
    IdentifierHash idHash = m_idHelper->wafer_hash(waferId);
    if (idHash.is_valid()) {
        return m_elementCollection[idHash];
    } else {
        return 0;
    }
}

HGTD_DetectorElement* HGTD_DetectorManager::getDetectorElement(const IdentifierHash & idHash) const
{
    return m_elementCollection[idHash];
}

HGTD_DetectorElement* HGTD_DetectorManager::getDetectorElement(int endcap, int layer, int phi_module, int eta_module) const
{
    return getDetectorElement(m_idHelper->wafer_id(endcap, layer, phi_module, eta_module));
}

const HGTD_DetectorElementCollection* HGTD_DetectorManager::getDetectorElementCollection() const
{
    return &m_elementCollection;
}

HGTD_DetectorElementCollection::const_iterator HGTD_DetectorManager::getDetectorElementBegin() const
{
    return m_elementCollection.begin();
}

HGTD_DetectorElementCollection::const_iterator HGTD_DetectorManager::getDetectorElementEnd() const
{
    return m_elementCollection.end();
}

void HGTD_DetectorManager::addDetectorElement(HGTD_DetectorElement * element)
{
    IdentifierHash idHash = element->identifyHash();
    if (idHash >=  m_elementCollection.size())
        ATH_MSG_ERROR ("HGTD_DetectorManager: Error adding detector element.");
    m_elementCollection[idHash] = element;
}

const HGTD_ID* HGTD_DetectorManager::getIdHelper() const
{
    return m_idHelper;
}

void HGTD_DetectorManager::invalidateAll() const
{
    for (HGTD_DetectorElementCollection::const_iterator element_iter = getDetectorElementBegin();
    element_iter != getDetectorElementEnd();
    ++element_iter) {
        if (*element_iter) {
            (*element_iter)->invalidate();
        }
    }
}

void HGTD_DetectorManager::updateAll() const
{
    for (HGTD_DetectorElementCollection::const_iterator element_iter = getDetectorElementBegin();
    element_iter != getDetectorElementEnd();
    ++element_iter) {
        if (*element_iter) {
            (*element_iter)->setCache();
        }
    }
}

void HGTD_DetectorManager::setCommonItems(std::unique_ptr<const SiCommonItems>&& commonItems)
{
    m_commonItems = std::move(commonItems);
}
