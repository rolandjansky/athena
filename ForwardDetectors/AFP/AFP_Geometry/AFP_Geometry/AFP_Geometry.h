/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GEOMETRY_H
#define AFP_GEOMETRY_H 1

#include "AFP_Geometry/AFP_constants.h"
#include "AFP_Geometry/AFP_ConfigParams.h"

#include "CLHEP/Geometry/Point3D.h"
#include "GeoModelKernel/GeoTransform.h"
#include "CLIDSvc/CLASS_DEF.h"
#ifndef __CINT__
#include "CLIDSvc/CLASS_DEF.h"
#endif

#include <list>
#include <map>
#include <vector>
#include <string>

class AFP_Geometry
{

private:
	AFP_CONFIGURATION m_CfgParams;
	std::map<eAFPStation, std::map<int, HepGeom::Transform3D> > m_MapSIDTransToLocal;
	std::map<eAFPStation, std::map<int, HepGeom::Transform3D> > m_MapSIDTransToGlobal;

public:
	AFP_Geometry(const AFP_CONFIGURATION* pCfgParams);
	~AFP_Geometry();

public:
	void getCfgParams(AFP_CONFIGURATION* pCfgParams) const { *pCfgParams=m_CfgParams; }
	HepGeom::Transform3D getStationTransform(const char* pszStationName); //in world
	HepGeom::Transform3D getStationElementTransform(const char* pszStationName, eStationElement eElement, const int nPlateID=-1); //in station

public:
	//SID detector
	int getSIDPlatesCnt(const eAFPStation eStation) { return m_CfgParams.sidcfg[eStation].fLayerCount; }
	HepGeom::Transform3D getSIDTransform(const eSIDTransformType eType, const char* pszStationName, const int nPlateID); // in station
	StatusCode getPointInSIDSensorLocalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& GlobalPoint, HepGeom::Point3D<double>& LocalPoint);
	StatusCode getPointInSIDSensorGlobalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& LocalPoint, HepGeom::Point3D<double>& GlobalPoint);
	eAFPStation parseStationName(const char* pszStationName);

	//TOF detector
	void setupLBarsDims(const eAFPStation eStation);
	void getPixelLocalPosition(const eAFPStation eStation, const int nPixelID, double* pfX1Pos, double* pfX2Pos);
	int getPixelRow(const int nPixelID) const { return (nPixelID-(nPixelID%10))/10; }
	int getPixelColumn(const int nPixelID) const { return nPixelID%10; }

};

#ifndef __CINT__
CLASS_DEF(AFP_Geometry, 160718034, 1)
#endif

#endif //AFP_GEOMETRY_H
