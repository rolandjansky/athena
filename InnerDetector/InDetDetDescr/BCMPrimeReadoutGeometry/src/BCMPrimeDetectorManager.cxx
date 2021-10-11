/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMPrimeReadoutGeometry/BCMPrimeDetectorManager.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace InDetDD {

    BCMPrimeDetectorManager::BCMPrimeDetectorManager(StoreGateSvc* detStore, const std::string & name)
                        : m_detStore(detStore)
    {
        setName(name);
    }

    BCMPrimeDetectorManager::~BCMPrimeDetectorManager() {

        //
        // Clean up
        //
        for (size_t i=0; i < m_volume.size(); i++) {
            m_volume[i]->unref();
        }
    }

    unsigned int BCMPrimeDetectorManager::getNumTreeTops() const {
        return m_volume.size();
    }

    PVConstLink BCMPrimeDetectorManager::getTreeTop(unsigned int i) const {
        return m_volume[i];
    }

    void BCMPrimeDetectorManager::addTreeTop(PVLink vol) {
        vol->ref();
        m_volume.push_back(vol);
    }

    void BCMPrimeDetectorManager::addAlignableTransform(int /*id*/, 
                                                        GeoAlignableTransform * /*transform*/,
                                                        const GeoVPhysVol * /*child*/)
    {
        // Here alignment transforms will be added
    }

    StatusCode BCMPrimeDetectorManager::align( IOVSVC_CALLBACK_ARGS_P( /*I*/, /*keys*/) ) const {
        // Here alignment transform deltas will be set
        return StatusCode::SUCCESS;
    }

} // namespace InDetDD
