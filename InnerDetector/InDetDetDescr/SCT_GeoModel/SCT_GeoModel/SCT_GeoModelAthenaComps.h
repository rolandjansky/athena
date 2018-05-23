/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_GeoModelAthenaComps_H
#define SCT_GeoModel_SCT_GeoModelAthenaComps_H

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

class ISiLorentzAngleTool;
class SCT_ID;


/// Class to hold various Athena components
class SCT_GeoModelAthenaComps : public InDetDD::AthenaComps {

public:

  SCT_GeoModelAthenaComps();

  void setLorentzAngleTool(const ISiLorentzAngleTool*);
  void setIdHelper(const SCT_ID* idHelper);

  const ISiLorentzAngleTool* lorentzAngleTool() const;
  const SCT_ID* getIdHelper() const;

private:
  // Lorentz angle tool
  const ISiLorentzAngleTool* m_lorentzAngleTool;
  const SCT_ID* m_idHelper;

};

#endif // SCT_GeoModel_SCT_GeoModelAthenaComps_H

