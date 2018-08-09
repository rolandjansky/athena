/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_DetectorFactory_h
#define ALFA_DetectorFactory_h 1

#include "ALFA_Geometry/ALFA_constants.h"
#include "ALFA_Geometry/ALFA_GeometryReader.h"
//#include "ALFA_GeoModel/ALFA_DetectorTool.h"
#include "GeoModelKernel/GeoAlignableTransform.h"

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "ALFA_GeoModel/ALFA_DetectorManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
//#include "GeoModelSvc/StoredMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include <string>
#include <vector>

class StoreGateSvc;
class AbsMaterialManager;
class IRDBRecordset;
class GeoShape;
class GeoPhysVol;
class GeoFullPhysVol;

typedef struct _ALFAPHYSVOLUME {
	GeoFullPhysVol* pPhysVolume;
	HepGeom::Transform3D Transform;
} ALFAPHYSVOLUME, *PALFAPHYSVOLUME;

typedef struct _ALIGNPARAMETERS {
	double fYOffset[RPOTSCNT];
	double fXOffset[RPOTSCNT];
	double fTheta[RPOTSCNT];
} ALIGNPARAMETERS, *PALIGNPARAMETERS;

enum eTransformDeltaType { ETDT_RPTRANSFORM };

typedef struct _CONFIGURATION {
	GEOMETRYCONFIGURATION GeometryConfig;

	bool bConstructBeampipe;
	bool bAddIBP;

	std::vector<bool> bIsTransformInStation;
	std::vector<bool> bIsTransformInDetector;
	std::vector<double> pointTransformInDetectorB7L1U;
	std::vector<double> pointTransformInDetectorB7L1L;
	std::vector<double> pointTransformInDetectorA7L1U;
	std::vector<double> pointTransformInDetectorA7L1L;
	std::vector<double> pointTransformInDetectorB7R1U;
	std::vector<double> pointTransformInDetectorB7R1L;
	std::vector<double> pointTransformInDetectorA7R1U;
	std::vector<double> pointTransformInDetectorA7R1L;
	std::vector<double> vecTransformInDetectorB7L1U;
	std::vector<double> vecTransformInDetectorB7L1L;
	std::vector<double> vecTransformInDetectorA7L1U;
	std::vector<double> vecTransformInDetectorA7L1L;
	std::vector<double> vecTransformInDetectorB7R1U;
	std::vector<double> vecTransformInDetectorB7R1L;
	std::vector<double> vecTransformInDetectorA7R1U;
	std::vector<double> vecTransformInDetectorA7R1L;
	std::vector<double> vecTransformInStationB7L1U;
	std::vector<double> vecTransformInStationB7L1L;
	std::vector<double> vecTransformInStationA7L1U;
	std::vector<double> vecTransformInStationA7L1L;
	std::vector<double> vecTransformInStationB7R1U;
	std::vector<double> vecTransformInStationB7R1L;
	std::vector<double> vecTransformInStationA7R1U;
	std::vector<double> vecTransformInStationA7R1L;

	void clear();
} CONFIGURATION, *PCONFIGURATION;

class ALFA_DetectorFactory : public GeoVDetectorFactory  
{
	private:
		// The managers:
		ALFA_DetectorManager* m_pDetectorManager;   
		//const AbsMaterialManager* m_pMaterialManager;
		DataHandle<StoredMaterialManager> m_pMaterialManager;
		StoreGateSvc* m_pDetectorStore;
		IRDBAccessSvc* m_pIRDBAccess;

		eMetrologyType m_eRequestedMetrologyType;
		CONFIGURATION m_Config;
		ALFA_GeometryReader* m_pGeoReader;
		std::list<eRPotName> m_ListExistingRPots;
		std::map<std::string,const GeoMaterial*> m_MapMaterials;

		
	public:
		ALFA_DetectorFactory(StoreGateSvc *pDetStore,IRDBAccessSvc *pAccess, const PCONFIGURATION pConfig);
		~ALFA_DetectorFactory();
			  
		// Creation of geometry:
		virtual void create(GeoPhysVol *pWorld);
		// Access to the results:
		virtual const ALFA_DetectorManager * getDetectorManager() const { return m_pDetectorManager; }

		void UpdateTransforms(PALIGNPARAMETERS pAlignParams);
		bool ReadGeometry(bool bAlignCorrections=false);

	private:  
		// Illegal operations:
		const ALFA_DetectorFactory & operator=(const ALFA_DetectorFactory &right);
		ALFA_DetectorFactory(const ALFA_DetectorFactory &right);
  
	private:
		void SaveGeometry();
		void DefineMaterials();
		void ConstructUFiberCladdings(const eRPotName eRPName, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform, GeoAlignableTransform* pDetTransform);
		void ConstructVFiberCladdings(const eRPotName eRPName, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform, GeoAlignableTransform* pDetTransform);
		void ConstructODFiberCladdings(const eRPotName eRPName, GeoFullPhysVol* pPhysMotherVolume, const HepGeom::Transform3D& MotherTransform, GeoAlignableTransform* pDetTransform);

		void ConstructODFibers00(const eRPotName eRPName, const int iODPlate, eFiberType eFType, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform, const HepGeom::Transform3D& TransODCladding);
		void ConstructODFibers01(const eRPotName eRPName, const int iODPlate, eFiberType eFType, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform, const HepGeom::Transform3D& TransODCladding);
		void SelectRPots();
		void CreateAxes(GeoPhysVol* pMotherVolume);
		void ConstructAlfaStations(std::map<eAStationName,ALFAPHYSVOLUME>* pmapActiveStations, GeoPhysVol* pWorld);
		void ConstructBeampipe(GeoPhysVol* pWorld);
		void AddBeamPipeInStation(GeoFullPhysVol* pPhysStation, const char* pszStationLabel);
		void AddGlobalVacuumSensorInStation(GeoFullPhysVol* pPhysStation, eAStationName eStatName);
	
		GeoShape* CreateSolidRP();
		GeoShape* CreateSolidAir();
		GeoShape* CreateSolidTrigger();
		GeoShape* CreateSolidRPSupport();
		GeoShape* CreateSolidG10Shapes();
		std::map<int,GeoShape*>* CreateSolidTiPlates();
		std::map<int,GeoShape*>* CreateSolidODPlates();

	private:
		HepGeom::Transform3D UserTransformInDetector(eRPotName eRPName);
		HepGeom::Transform3D UserTransformInStation(eRPotName eRPName);
		HepGeom::Point3D<double> Point3DInDetector(eRPotName eRPName);
};

#endif
