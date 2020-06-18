/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetServMatGeoModel_InDetServMatAthenaComps_H
#define InDetServMatGeoModel_InDetServMatAthenaComps_H

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

class IInDetServMatBuilderTool;

/// Class to hold various Athena components
class InDetServMatAthenaComps : public InDetDD::AthenaComps {

public:

  InDetServMatAthenaComps();

  //Add Builder Tool
  void setBuilderTool(IInDetServMatBuilderTool * builderTool);
  const IInDetServMatBuilderTool *builderTool() const;

private:


  // Tool for building service
  IInDetServMatBuilderTool * m_builderTool;

};

#endif // InDetServMatGeoModel_InDetServMatAthenaComps_H

