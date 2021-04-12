/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef AFP_GEOMETRY_H
#define AFP_GEOMETRY_H 1

#include "AFP_Geometry/AFP_constants.h"
#include "AFP_Geometry/AFP_ConfigParams.h"

#include "CLHEP/Geometry/Point3D.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <list>
#include <map>
#include <vector>
#include <string>

enum eStationElement { ESE_RPOT, ESE_SID, ESE_TOF};
enum eSIDTransformType { ESTT_SUPPORT, ESTT_PLATE, ESTT_SENSOR, ESTT_VACUUMSENSOR, ESTT_SENSORLOCAL, ESTT_SENSORGLOBAL };
enum eAFPStation { EAS_UNKNOWN=-1, EAS_AFP00=0, EAS_AFP01=1, EAS_AFP02=2, EAS_AFP03=3 };


class AFP_Geometry
{

protected:
	AFP_CONFIGURATION m_CfgParams;

private:
	std::map<eAFPStation, std::map<int, HepGeom::Transform3D> > m_MapSIDTransToLocal;
	std::map<eAFPStation, std::map<int, HepGeom::Transform3D> > m_MapSIDTransToGlobal;

public:
	AFP_Geometry() {}
	AFP_Geometry(const PAFP_CONFIGURATION pCfgParams);
	~AFP_Geometry();

public:
	void GetCfgParams(PAFP_CONFIGURATION pCfgParams) const { *pCfgParams=m_CfgParams; }
	HepGeom::Transform3D GetStationTransform(const char* pszStationName); //in world
	HepGeom::Transform3D GetStationElementTransform(const char* pszStationName, eStationElement eElement); //in station

public:
	//SID detector
	int GetSIDPlatesCnt() const { return m_CfgParams.sidcfg.fLayerCount; }
	HepGeom::Transform3D GetSIDTransform(const eSIDTransformType eType, const char* pszStationName, const int nPlateIndex); // in station
	StatusCode GetPointInSIDSensorLocalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& GlobalPoint, HepGeom::Point3D<double>& LocalPoint);
	StatusCode GetPointInSIDSensorGlobalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& LocalPoint, HepGeom::Point3D<double>& GlobalPoint);

private:
	eAFPStation ParseStationName(const char* pszStationName);

};

#ifndef __CINT__
CLASS_DEF(AFP_Geometry, 160718034, 1)
#endif

#endif //AFP_GEOMETRY_H
