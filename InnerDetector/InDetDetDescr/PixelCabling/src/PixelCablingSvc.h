/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCABLINGSVC_H
#define PIXELCABLINGSVC_H

#include "PixelCabling/IPixelCablingSvc.h"
#include "AthenaBaseComps/AthService.h"

namespace InDetDD {
  class PixelDetectorManager;
}  

class PixelCablingSvc: virtual public IPixelCablingSvc, public AthService {

  public:
    PixelCablingSvc(const std::string& name, ISvcLocator* svc ) ;

    ~PixelCablingSvc();
    StatusCode initialize();
    StatusCode finalize();

    StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );

    Identifier getPixelIdfromHash(IdentifierHash offlineIdHash, uint32_t FE, uint32_t row, uint32_t column);
    Identifier getPixelId(Identifier offlineId, uint32_t FE, uint32_t row, uint32_t column);

    uint32_t getFE(Identifier* pixelId, Identifier offlineId);
    uint32_t getColumn(Identifier* pixelId, Identifier offlineId);
    uint32_t getRow(Identifier* pixelId, Identifier offlineId);
    unsigned int getLocalFEI4(const uint32_t fe, const uint64_t onlineId);

    moduletype getModuleType(const Identifier& id);
    pixeltype  getPixelType(const Identifier& id);

  private:
    ServiceHandle< StoreGateSvc > m_detStore;
    StringProperty m_detManagerName{this, "DetManagerName", "Pixel", "Name of the DeterctorManager to retrieve"};
    const InDetDD::PixelDetectorManager *m_detManager;
    const PixelID* m_idHelper;

};

#endif
