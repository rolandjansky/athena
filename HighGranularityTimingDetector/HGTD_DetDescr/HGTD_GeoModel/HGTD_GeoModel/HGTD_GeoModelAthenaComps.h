/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GeoModel_HGTD_GeoModelAthenaComps_H
#define HGTD_GeoModel_HGTD_GeoModelAthenaComps_H

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

class HGTD_ID;


/// Class to hold various Athena components
class HGTD_GeoModelAthenaComps : public InDetDD::AthenaComps {

public:

  HGTD_GeoModelAthenaComps();

  void setIdHelper(const HGTD_ID* idHelper);

  const HGTD_ID* getIdHelper() const;

private:
  const HGTD_ID* m_idHelper;

};

#endif // HGTD_GeoModel_HGTD_GeoModelAthenaComps_H
