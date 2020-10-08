/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGeoModel_PixelGeoModelAthenaComps_H
#define PixelGeoModel_PixelGeoModelAthenaComps_H

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GaudiKernel/ServiceHandle.h"

class IGeoSubDetTool;
class PixelID;
class IInDetServMatBuilderTool;

/// Class to hold various Athena components
class PixelGeoModelAthenaComps : public InDetDD::AthenaComps {

public:

  PixelGeoModelAthenaComps();

  //Add BCM
  void setBCM(IGeoSubDetTool * bcmTool);
  IGeoSubDetTool *bcm();

  //Add BLM
  void setBLM(IGeoSubDetTool * blmTool);
  IGeoSubDetTool *blm();

 //Add Builder Tool
  void setServiceBuilderTool(IInDetServMatBuilderTool * serviceBuilderTool);
  IInDetServMatBuilderTool *serviceBuilderTool();

  //Id helper
  void setIdHelper(const PixelID* idHelper);
  const PixelID* getIdHelper() const;

private:

  // Tool for building BCM
  IGeoSubDetTool * m_bcmTool;

  // Tool for building BLM
  IGeoSubDetTool * m_blmTool;

  // Tool for building service
  IInDetServMatBuilderTool * m_serviceBuilderTool;

  // Id helper
  const PixelID* m_idHelper;

};

#endif // PixelGeoModel_PixelGeoModelAthenaComps_H

