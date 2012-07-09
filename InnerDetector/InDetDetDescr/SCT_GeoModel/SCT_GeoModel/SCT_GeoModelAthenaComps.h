/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_GeoModelAthenaComps_H
#define SCT_GeoModel_SCT_GeoModelAthenaComps_H

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

#include "GaudiKernel/ServiceHandle.h"
class ISiLorentzAngleSvc;
class SCT_ID;


/// Class to hold various Athena components
class SCT_GeoModelAthenaComps : public InDetDD::AthenaComps {

public:

  SCT_GeoModelAthenaComps();

  void setLorentzAngleSvc(const ServiceHandle<ISiLorentzAngleSvc> &);
  void setIdHelper(const SCT_ID* idHelper);

  const ServiceHandle<ISiLorentzAngleSvc> & lorentzAngleSvc() const;
  const SCT_ID* getIdHelper() const;

private:
  // Lorentz angle service
  ServiceHandle<ISiLorentzAngleSvc> m_lorentzAngleSvc;
  const SCT_ID* m_idHelper;

};

#endif // SCT_GeoModel_SCT_GeoModelAthenaComps_H

