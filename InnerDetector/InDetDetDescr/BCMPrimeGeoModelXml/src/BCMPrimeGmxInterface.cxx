/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMPrimeGeoModelXml/BCMPrimeGmxInterface.h"

#include <cstdlib>
#include <sstream>

#include "InDetSimEvent/SiHitIdHelper.h" 

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/MsgStream.h"

#include "BCMPrimeReadoutGeometry/BCMPrimeDetectorManager.h"

using namespace std;

namespace ITk {

  BCMPrimeGmxInterface::BCMPrimeGmxInterface(InDetDD::BCMPrimeDetectorManager *detectorManager):
    m_detectorManager(detectorManager)
  {
    // Logging: ref https://wiki.bnl.gov/dayabay/index.php?title=Logging
    // Turn on logging in job-options with: MessageSvc.setDebug += {"BCMPrimeGmxInterface"}
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "BCMPrimeGmxInterface");
    
    m_log = std::make_unique<MsgStream>(&(*msgh), "BCMPrimeGmxInterface");

  }

  BCMPrimeGmxInterface::~BCMPrimeGmxInterface() {
  }

  int BCMPrimeGmxInterface::sensorId(map<string, int> &index) const {
    //
    //    Return the Simulation HitID (nothing to do with "ATLAS Identifiers" aka "Offline Identifiers")
    //
    
    int hitIdOfModule = SiHitIdHelper::GetHelper()->buildHitId(0, 0, index["diamond_number"], 
                  index["module_number"], 0, 0);

    *m_log << MSG::DEBUG  << "Index list: " << index["diamond_number"] << " " << index["module_number"] << endmsg;
    *m_log << MSG::DEBUG << "hitIdOfModule = " << std::hex << hitIdOfModule << std::dec << endmsg;
    *m_log << MSG::DEBUG << " dia = " << SiHitIdHelper::GetHelper()->getLayerDisk(hitIdOfModule) <<
                            " mod = " << SiHitIdHelper::GetHelper()->getEtaModule(hitIdOfModule) << endmsg;
    return hitIdOfModule;
  }

  void BCMPrimeGmxInterface::addAlignable(int /*level*/, std::map<std::string, int> &index,
                                          GeoVFullPhysVol *fpv, GeoAlignableTransform *transform)
  {
    *m_log << MSG::DEBUG  << "alignable transform added for indices: " << index["diamond_number"] << " " << index["module_number"] << endmsg;

    // A preliminary id scheme
    int id = index["diamond_number"] + 8*index["module_number"];

    m_detectorManager->addAlignableTransform(id, transform, fpv);
  }

} // namespace ITk
