
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
	AFP_Geometry(const PAFP_CONFIGURATION pCfgParams);
	~AFP_Geometry();

public:
	void GetCfgParams(PAFP_CONFIGURATION pCfgParams) const { *pCfgParams=m_CfgParams; }
	HepGeom::Transform3D GetStationTransform(const char* pszStationName); //in world
	HepGeom::Transform3D GetStationElementTransform(const char* pszStationName, eStationElement eElement, const int nPlateID=-1); //in station

public:
	//SID detector
	int GetSIDPlatesCnt(const eAFPStation eStation) { return m_CfgParams.sidcfg[eStation].fLayerCount; }
	HepGeom::Transform3D GetSIDTransform(const eSIDTransformType eType, const char* pszStationName, const int nPlateID); // in station
	StatusCode GetPointInSIDSensorLocalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& GlobalPoint, HepGeom::Point3D<double>& LocalPoint);
	StatusCode GetPointInSIDSensorGlobalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& LocalPoint, HepGeom::Point3D<double>& GlobalPoint);
	eAFPStation ParseStationName(const char* pszStationName);

	//TOF detector
	void SetupLBarsDims(const eAFPStation eStation);
	void GetPixelLocalPosition(const eAFPStation eStation, const int nPixelID, double* pfX1Pos, double* pfX2Pos);
	int GetPixelRow(const int nPixelID) { return (nPixelID-(nPixelID%10))/10; }
	int GetPixelColumn(const int nPixelID) { return nPixelID%10; }

};

#ifndef __CINT__
CLASS_DEF(AFP_Geometry, 160718034, 1)
#endif

#endif //AFP_GEOMETRY_H
