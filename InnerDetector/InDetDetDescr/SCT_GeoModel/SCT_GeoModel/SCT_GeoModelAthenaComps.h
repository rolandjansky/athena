/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_GeoModelAthenaComps_H
#define SCT_GeoModel_SCT_GeoModelAthenaComps_H

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

class SCT_ID;


/// Class to hold various Athena components
class SCT_GeoModelAthenaComps : public InDetDD::AthenaComps {

public:

  SCT_GeoModelAthenaComps();

  void setIdHelper(const SCT_ID* idHelper);

  const SCT_ID* getIdHelper() const;

private:
  const SCT_ID* m_idHelper;

};

#endif // SCT_GeoModel_SCT_GeoModelAthenaComps_H

