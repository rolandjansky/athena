/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMPrimeGmxInterface.h"

#include <BCMPrimeReadoutGeometry/BCMPrimeDetectorManager.h>
#include <InDetSimEvent/SiHitIdHelper.h>

namespace InDetDD
{

BCMPrimeGmxInterface::BCMPrimeGmxInterface(BCMPrimeDetectorManager *detectorManager)
  : AthMessaging(Athena::getMessageSvc(), "BCMPrimeGmxInterface"),
    m_detectorManager(detectorManager)
{}

int BCMPrimeGmxInterface::sensorId(std::map<std::string, int> &index) const
{
  // Return the Simulation HitID (nothing to do with "ATLAS Identifiers" aka "Offline Identifiers")
  int hitIdOfModule = SiHitIdHelper::GetHelper()->buildHitId(0, 0, index["diamond_number"], index["module_number"], 0, 0);

  ATH_MSG_DEBUG("Index list: " << index["diamond_number"] << " " << index["module_number"]);
  ATH_MSG_DEBUG("hitIdOfModule = " << std::hex << hitIdOfModule << std::dec);
  ATH_MSG_DEBUG(" dia = " << SiHitIdHelper::GetHelper()->getLayerDisk(hitIdOfModule) <<
                " mod = " << SiHitIdHelper::GetHelper()->getEtaModule(hitIdOfModule));
  return hitIdOfModule;
}

void BCMPrimeGmxInterface::addAlignable(int /*level*/, std::map<std::string, int> &index,
                                        GeoVFullPhysVol *fpv, GeoAlignableTransform *transform)
{
  ATH_MSG_DEBUG("alignable transform added for indices: " << index["diamond_number"] << " " << index["module_number"]);

  // A preliminary id scheme
  int id = index["diamond_number"] + 8*index["module_number"];

  m_detectorManager->addAlignableTransform(id, transform, fpv);
}

} // namespace InDetDD
