/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_ParametersBase_H
#define SCT_SLHC_GeoModel_SCT_ParametersBase_H

#include "SCT_SLHC_GeoModel/SCT_GeoModelAthenaComps.h"

namespace InDetDDSLHC {

class SCT_ParametersBase{

public:

  // Constructor 
  SCT_ParametersBase(const SCT_GeoModelAthenaComps * athenaComps)
    : m_athenaComps(athenaComps) {}
  virtual ~SCT_ParametersBase() = default;

  // Geometry DB Interface
  const IGeometryDBSvc * db() const {return m_athenaComps->geomDB();}

  //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_athenaComps->msg(lvl); }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_athenaComps->msgLvl(lvl); }

  const SCT_GeoModelAthenaComps * athenaComps() const {return m_athenaComps;}

private:

  const SCT_GeoModelAthenaComps * m_athenaComps;
};
} 
#endif // SCT_SLHC_GeoModel_SCT_ParametersBase_H
