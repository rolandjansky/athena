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

class AFP_Geometry
{

protected:
	AFP_CONFIGURATION m_CfgParams;

	std::map<eAFPStation, std::map<int, HepGeom::Transform3D> > m_MapSIDTransToLocal;
	std::map<eAFPStation, std::map<int, HepGeom::Transform3D> > m_MapSIDTransToGlobal;

public:
	AFP_Geometry() {}
	AFP_Geometry(const AFP_CONFIGURATION* pCfgParams);
	~AFP_Geometry();

public:
	void getCfgParams(AFP_CONFIGURATION* pCfgParams) const { *pCfgParams=m_CfgParams; }
	HepGeom::Transform3D getStationTransform(const char* pszStationName) const; //in world
	HepGeom::Transform3D getStationElementTransform(const char* pszStationName, eStationElement eElement, const int nPlateID=-1) const; //in station

public:
	//SID detector
	int getSIDPlatesCnt(const eAFPStation eStation) const{
		std::map<eAFPStation, AFP_SIDCONFIGURATION> sidcfg(m_CfgParams.sidcfg.begin(),m_CfgParams.sidcfg.end());
		return sidcfg[eStation].fLayerCount;
	}
	HepGeom::Transform3D getSIDTransform(const eSIDTransformType eType, const char* pszStationName, const int nPlateID) const; // in station
	StatusCode getPointInSIDSensorLocalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& GlobalPoint, HepGeom::Point3D<double>& LocalPoint) const;
	StatusCode getPointInSIDSensorGlobalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& LocalPoint, HepGeom::Point3D<double>& GlobalPoint) const;
	eAFPStation parseStationName(const char* pszStationName) const;

	//TOF detector
	void setupLBarsDims(const eAFPStation eStation);
	void getPixelLocalPosition(const eAFPStation eStation, const int nPixelID, double* pfX1Pos, double* pfX2Pos) const;
	int getPixelRow(const int nPixelID) const { return (nPixelID-(nPixelID%10))/10; }
	int getPixelColumn(const int nPixelID) const { return nPixelID%10; }

};

#ifndef __CINT__
CLASS_DEF(AFP_Geometry, 160718034, 1)
#endif

#endif //AFP_GEOMETRY_H
