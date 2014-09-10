/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGeoModel_PixelGeoModelAthenaComps_H
#define PixelGeoModel_PixelGeoModelAthenaComps_H

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GaudiKernel/ServiceHandle.h"

class IGeoSubDetTool;
class PixelID;
class ISiLorentzAngleSvc;
class IInDetServMatBuilderTool;

/// Class to hold various Athena components
class PixelGeoModelAthenaComps : public InDetDD::AthenaComps {

public:

  PixelGeoModelAthenaComps();

  // Lorentz Angle Service
  void setLorentzAngleSvc(const ServiceHandle<ISiLorentzAngleSvc> &);
  const ServiceHandle<ISiLorentzAngleSvc> & lorentzAngleSvc() const;
  
  //Add BCM
  void setBCM(IGeoSubDetTool * bcmTool);
  IGeoSubDetTool *bcm() const;

  //Add BLM
  void setBLM(IGeoSubDetTool * blmTool);
  IGeoSubDetTool *blm() const;

 //Add Builder Tool
  void setServiceBuilderTool(IInDetServMatBuilderTool * serviceBuilderTool);
  IInDetServMatBuilderTool *serviceBuilderTool() const;

  //Id helper
  void setIdHelper(const PixelID* idHelper);
  const PixelID* getIdHelper() const;

private:

  // Lorentz angle service
  ServiceHandle<ISiLorentzAngleSvc> m_lorentzAngleSvc;

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

