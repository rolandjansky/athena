/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Geometry/AFP_GeometryTool.h"

AFP_GeometryTool::AFP_GeometryTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

AFP_GeometryTool::~AFP_GeometryTool()
{
}

StatusCode AFP_GeometryTool::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode AFP_GeometryTool::finalize()
{
  return StatusCode::SUCCESS;
}

void AFP_GeometryTool::SetCfgParams(AFP_CONFIGURATION * pCfgParams)
{
  // coppy of constructor of AFP_Geometry
  m_CfgParams=*pCfgParams;
  
  setupLBarsDims(EAS_AFP00);
  setupLBarsDims(EAS_AFP03);

  for(int nStationID=0; nStationID<=3; ++nStationID)
  {
    eAFPStation eStation=(eAFPStation)nStationID;
    for(int nPlateID=0; nPlateID<getSIDPlatesCnt(eStation); ++nPlateID)
    {
      if(!(m_MapSIDTransToGlobal.find(eStation)!=m_MapSIDTransToGlobal.end() && m_MapSIDTransToGlobal[eStation].find(nPlateID)!=m_MapSIDTransToGlobal[eStation].end()))
      {
        char szStationName[8];
        sprintf(szStationName,"AFP%02i",(int)eStation);
HepGeom::Transform3D Aux=getSIDTransform(ESTT_SENSORGLOBAL,szStationName,nPlateID);
        m_MapSIDTransToGlobal[eStation][nPlateID]=Aux;
      }

      if(!(m_MapSIDTransToLocal.find(eStation)!=m_MapSIDTransToLocal.end() && m_MapSIDTransToLocal[eStation].find(nPlateID)!=m_MapSIDTransToLocal[eStation].end()))
      {
        char szStationName[8];
        sprintf(szStationName,"AFP%02i",(int)eStation);
HepGeom::Transform3D Aux=getSIDTransform(ESTT_SENSORLOCAL,szStationName,nPlateID);
        m_MapSIDTransToLocal[eStation][nPlateID]=Aux;
      }
    }
  }
}
