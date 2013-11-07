/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_GeoModelAthenaComps_H
#define SCT_SLHC_GeoModel_SCT_GeoModelAthenaComps_H

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

#include "GaudiKernel/ServiceHandle.h"
class ISiLorentzAngleSvc;

class SCT_ID;
class IInDetServMatBuilderTool;

namespace InDetDDSLHC {

/// Class to hold various Athena components
class SCT_GeoModelAthenaComps : public InDetDD::AthenaComps {

public:

  SCT_GeoModelAthenaComps();

  void setLorentzAngleSvc(const ServiceHandle<ISiLorentzAngleSvc> &);
  void setIdHelper(const SCT_ID* idHelper);

  //Add Builder Tool
  void setServiceBuilderTool(IInDetServMatBuilderTool * serviceBuilderTool);
  IInDetServMatBuilderTool *serviceBuilderTool() const;

  const ServiceHandle<ISiLorentzAngleSvc> & lorentzAngleSvc() const;
  const SCT_ID* getIdHelper() const;

private:
  // Lorentz angle service
  ServiceHandle<ISiLorentzAngleSvc> m_lorentzAngleSvc;
  IInDetServMatBuilderTool * m_serviceBuilderTool;
  const SCT_ID* m_idHelper;

};
}
#endif // SCT_SLHC_GeoModel_SCT_GeoModelAthenaComps_H

