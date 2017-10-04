/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ALFA_GeometryReader_h
#define ALFA_GeometryReader_h 1

#include "ALFA_constants.h"

#include "CLHEP/Geometry/Point3D.h"
#include "GeoModelKernel/GeoTransform.h"
#include "CLIDSvc/CLASS_DEF.h"
#ifndef __CINT__
	#include "CLIDSvc/CLASS_DEF.h"
#endif

#include "ALFA_Geometry/ALFA_ConfigParams.h"

#include <list>
#include <map>
#include <vector>
#include <string>
//using namespace std;
//using namespace HepGeom;

enum eGeoSourceType { EGST_UNDEFINED = 0, EGST_IDEALGEOMETRY = 1, EGST_FILE = 2, EGST_DATABASE = 3 };
enum eMetrologyType { EMT_UNDEFINED = 0, EMT_NOMINAL = 1, EMT_METROLOGY = 2, EMT_SWCORRECTIONS = 3 };
enum eFiberType { EFT_UNDEFINED=-1, EFT_FIBERMD=0, EFT_FIBEROD, EFT_UFIBER, EFT_VFIBER, EFT_ODFIBERU0, EFT_ODFIBERV0, EFT_ODFIBERU1, EFT_ODFIBERV1};
enum eAStationName { EASN_UNDEFINED=0, EASN_B7L1=1, EASN_A7L1, EASN_A7R1, EASN_B7R1 };
enum eRPotName { ERPN_UNDEFINED=0, ERPN_B7L1U=1, ERPN_B7L1L=2, ERPN_A7L1U=3, ERPN_A7L1L=4, ERPN_A7R1U=5, ERPN_A7R1L=6, ERPN_B7R1U=7, ERPN_B7R1L=8};
enum eRPPositionType { ERPPT_UNSET=-1, ERPPT_INACTIVE=0, ERPPT_ACTIVE=1 };
enum eFiberCoordSystem { EFCS_UNDEFINED=-1, EFCS_CLADDING, EFCS_ATLAS };
enum eRefPointType { ERPT_IDEAL, ERPT_REAL, ERPT_DETIDEAL, ERPT_DETREAL };
enum eMetrologyCoordSystem { EMCS_ATLAS, EMCS_STATION, EMCS_ROMANPOT, EMCS_DETPIN1};
enum eTransformElement { ETE_A1, ETE_A2, ETE_A3, ETE_T1, ETE_T2, ETE_T3 };
//enum eOwnerType { EOT_UNDEFINED=0, EOT_GEOMODEL, EOT_RECO };

struct RPPINS{
	// points in station CS
	HepGeom::Point3D<double> IdealRPPin1, RealRPPin1;
	HepGeom::Point3D<double> IdealRPPin2, RealRPPin2;
	HepGeom::Point3D<double> IdealRPPin3, RealRPPin3;

	// points in ALFA det CS
	HepGeom::Point3D<double> DTPInAlfaCS,DTPInRPotCS,DTPInAtlasCS;
	HepGeom::Point3D<double> DCPInAlfaCS,DCPInRPotCS,DCPInAtlasCS;

	void clear();
};
typedef RPPINS* PRPPINS;

struct RPPOSPARAMS {
	char szLabel[8];
	HepGeom::Point3D<double> IdealMainPoint;
	HepGeom::Point3D<double> IdealMainPointInStation;
	HepGeom::Point3D<double> IdealRefPoint; //reference point of RP's detector
	bool bIsLow;
	eAStationName eASName;
	RPPINS RefPins;
	double fCurrentLVDTmm;
	HepGeom::Vector3D<double> DetectorNormal;

	//RP transform matrix in main point of the station (which ideally lies on the beam axis) - positioning of the RP's main point
	std::vector<HepGeom::Point3D<double> > VecIdealRPRefPoints;
	std::vector<HepGeom::Point3D<double> > VecRealRPRefPoints;
	HepGeom::Transform3D RPIdealTransformInStation;
	HepGeom::Transform3D RPSWTransformInStation;
	HepGeom::Transform3D RPTransformInStation;
	HepGeom::Scale3D RPScaleInStation;

	//transform matrix of detection part of the RP in the RP's main point
	std::vector<HepGeom::Point3D<double> > VecIdealDetRefPoints;
	std::vector<HepGeom::Point3D<double> > VecRealDetRefPoints;
	HepGeom::Transform3D DetIdealTransformInMainPoint;
	HepGeom::Transform3D DetSWTransformInMainPoint;

	HepGeom::Transform3D DetSWTransform; //relative to a given origin
	HepGeom::Point3D<double> OriginOfDetSWTransform; //relative to RRPin1
	HepGeom::Transform3D DetTransformInMainPoint; //relative to MainPoint (computed)
	HepGeom::Scale3D DetScaleInRP;

	void clear();
};
typedef RPPOSPARAMS* PRPPOSPARAMS;

struct ASPOSPARAMS {
	char szLabel[8];
	HepGeom::Point3D<double> IdealMainPoint;

	//station transform matrix in the ATLAS coord. system and ideal ref points
	//station itself is placed ideally, transformation is dedicated for RPs positioning
	HepGeom::Vector3D<double> ShiftE;
	HepGeom::Vector3D<double> ShiftS;
	HepGeom::Transform3D ASTransformInMainPoint;
	HepGeom::Transform3D ASTransformInATLAS;

	void clear();
};
typedef ASPOSPARAMS* PASPOSPARAMS;

struct FIBERPARAMS {
	int nPlateID = 0;
	int nFiberID = 0;
	int nLayerID = 0;

	struct {
			union {
				double fCentreXPos;
				double fCentreYPos;
			};
			double fAngle = 0;
	} fcs_cladding;

	struct {
			double fSlope = 0;
			double fOffset = 0;
			double fZPos = 0;
	} fcs_atlas;

	struct {
		double fOriginX = 0, fOriginY = 0, fOriginZ = 0;
		double fDirX = 0, fDirY = 0, fDirZ = 0;
	} fcs_atlas_full;
	
	double fSlope = 0;
	double fOffset = 0;
	double fZPos = 0;
	
	double fMainRefPointSlope = 0;
	HepGeom::Point3D<float> MainRefPointPos;

        FIBERPARAMS() { fcs_cladding.fCentreXPos = 0; }
};
typedef FIBERPARAMS* PFIBERPARAMS;

struct PLATEPARAMS {
	double fUCladdingSizeX;
	double fVCladdingSizeX;
};
typedef PLATEPARAMS* PPLATEPARAMS;

struct ROMAPOT {

	ROMAPOT()
	{
		clear();
	}

	eGeoSourceType eMDGeometryType;
	eGeoSourceType eODGeometryType;
	std::list<FIBERPARAMS> ListUFibers;
	std::list<FIBERPARAMS> ListVFibers;
	std::list<FIBERPARAMS> ListODFibersU0;
	std::list<FIBERPARAMS> ListODFibersV0;
	std::list<FIBERPARAMS> ListODFibersU1;
	std::list<FIBERPARAMS> ListODFibersV1;
	std::map<int,PLATEPARAMS> MapPlates;
	std::map<int,PLATEPARAMS> MapODPlates;

	void clear()
	{
		eMDGeometryType=EGST_IDEALGEOMETRY;
		eODGeometryType=EGST_IDEALGEOMETRY;
		ListUFibers.clear();
		ListVFibers.clear();
		ListODFibersU0.clear();
		ListODFibersV0.clear();
		ListODFibersU1.clear();
		ListODFibersV1.clear();
		MapPlates.clear();
		MapODPlates.clear();
	}
};
typedef ROMAPOT PROMAPOT;

struct CFGRPPOSPARAMS {
	int /*eRPPositionType*/ eRPPosType;
	int /*eGeoSourceType*/ eMDGeoType;
	int /*eGeoSourceType*/ eODGeoType;
	std::string strMDConnString;
	std::string strODConnString;

	double fCurrentLVDTmm;
	struct {
		double fYOffset;
		double fXOffset;
		double fTheta;
	} swcorr;

	struct {
		bool bIsEnabledUserTranform;
		HepGeom::Point3D<double> UserOriginOfDetTransInRPot;
		HepGeom::Transform3D UserTransformOfDetInRPot;
		HepGeom::Transform3D UserTransformOfRPInStation;
	} usercorr;
};
typedef CFGRPPOSPARAMS* PCFGRPPOSPARAMS;

struct GEOMETRYCONFIGURATION {
	int /*eMetrologyType*/ eRPMetrologyGeoType;
	std::string strRPMetrologyConnString;
	bool bShiftToX97Pos;

	double fNominalZPosA7L1;
	double fNominalZPosB7L1;
	double fNominalZPosA7R1;
	double fNominalZPosB7R1;
	CFGRPPOSPARAMS CfgRPosParams[RPOTSCNT];

	//bool bEnableUserLVDT;
	void clear();
};
typedef GEOMETRYCONFIGURATION* PGEOMETRYCONFIGURATION;

class ALFA_RDBAccess;

class ALFA_GeometryReader
{	
	public:
		static HepGeom::Point3D<double> ms_NominalRPPin1; //in station CS
		static HepGeom::Point3D<double> ms_NominalRPMainPoint;  //in station CS
		static HepGeom::Point3D<double> ms_NominalAlfaRefPoint;  //in RP CS
		static HepGeom::Point3D<double> ms_NominalDetPin1; //in RP CS

	private:
		eFiberCoordSystem m_eFCoordSystem;
		GEOMETRYCONFIGURATION m_ConfigOpts;
		eMetrologyType m_eMetrologyType;

	public:
		std::map<eRPotName,RPPOSPARAMS> m_RPPosParams;
		std::map<eAStationName,ASPOSPARAMS> m_ASPosParams;
		
	private:
		std::map<eRPotName,ROMAPOT> m_MapRPot;
		std::list<eRPotName> m_ListExistingRPots;
		
	public:
		ALFA_GeometryReader();
		~ALFA_GeometryReader();
		
	private:
		bool InitializeDefault(const PGEOMETRYCONFIGURATION pConfig);
		bool ReadSource(const eGeoSourceType eSourceType, const eRPotName eRPName, const eFiberType eFType, const char* szDataSource);
		bool SetIdealGeometry(const eRPotName eRPName, const eFiberType eFType);
		void UpdateGeometry();
		void UpdateStationsPosParams();
		void UpdateSimRPPos(const eRPotName eRPName);
		bool ReadFile(const eRPotName eRPName, const eFiberType eFType, const char* szFilename);
		bool ReadDatabase(const eRPotName eRPName, const eFiberType eFType, const char* szDataSource);
		void TransformFiberPositions(PFIBERPARAMS pFiberParams,eRPotName eRPName, const eFiberType eType, const eGeoSourceType eSourceType);
		void TransformFiberPositionsFCSCladding(PFIBERPARAMS pFiberParams,eRPotName eRPName, const eFiberType eType, const eGeoSourceType eSourceType);
		void TransformFiberPositionsFCSAtlas(PFIBERPARAMS pFiberParams,eRPotName eRPName, const eFiberType eType, const eGeoSourceType eSourceType);
		HepGeom::Transform3D ComputeTransformMatrix(const std::vector<HepGeom::Point3D<double> >& VecIdealRefPoints,const std::vector<HepGeom::Point3D<double> >& VecRealRefPoints, const int nPointCnt, HepGeom::Scale3D& Scale, bool bForceUseSVD=false);
		bool ParseRPMetrology(eGeoSourceType eSourceType, const char* szDataSource);
		bool ResolveRPotRefPoints(const char* szvalue, eRPotName eRPName, eRefPointType eRPointType);
		bool ParseRefPoints(const char* szvalue, std::vector<HepGeom::Point3D<double> >& vecRefPoints, eMetrologyCoordSystem eCSystem);
		bool ParseArrayOfValues(const char* szvalue, std::vector<double>& vecValues);
		bool SetupRPMetrologyPoints(ALFA_ConfigParams& CfgParams, eRPotName eRPName);
		bool SetupDetMetrologyPoints(ALFA_ConfigParams& CfgParams, eRPotName eRPName);
		bool SetupStationMetrologyPoints(ALFA_ConfigParams& CfgParams, eAStationName eASName);
		double GetPolyFitValue(const double fInputValue, const std::vector<double>& vecPolyFitParams);
		void SetupCurrentLVDT(const PGEOMETRYCONFIGURATION pConfig);
		void SetupSWCorrections(const PGEOMETRYCONFIGURATION pConfig);
		void SetupUserCorrections(const PGEOMETRYCONFIGURATION pConfig);

	public:
		bool Initialize(const PGEOMETRYCONFIGURATION pConfig, eFiberCoordSystem eFCoordSystem);
		bool ReadFiberGeometry(const PGEOMETRYCONFIGURATION pConfig);
		int GetRPotCount() const { return (int)m_MapRPot.size(); }
		void PrintFiberGeometry(std::ostream &OutStream);
		void PrintFiberGeometry(const char* szOutFilename);
		void GetListOfRPotIDs(std::map<eRPotName,std::string>* pMapRPotName);
		void GetListOfExistingRPotIDs(std::list<eRPotName>* pListRPotName);
		bool GetUFiberParams(PFIBERPARAMS pFiberParams, const eRPotName eRPName, const int nPlateID, const int nFiberID);
		bool GetVFiberParams(PFIBERPARAMS pFiberParams, const eRPotName eRPName, const int nPlateID, const int nFiberID);
		bool StoreReconstructionGeometry(const eRPotName eRPName, const eFiberType eFType, const char* szDataDestination);
		bool GetPlateParams(PPLATEPARAMS pPlateParams, const eRPotName eRPName, const int nPlateID);
		bool GetRPPosParams(PRPPOSPARAMS pRPosParams, const eRPotName eRPName);
		bool GetASPosParams(PASPOSPARAMS pRPosParams, const eAStationName eASName);
		const char* GetRPotLabel(const eRPotName eRPName) { return m_RPPosParams[eRPName].szLabel; }
		const char* GetAStationLabel(const eAStationName eASName) { return m_ASPosParams[eASName].szLabel; }
		eGeoSourceType GetRPGeometryType(const eRPotName eRPName, eFiberType eFType);
		bool GetMDFiberParams(PFIBERPARAMS pFiberParams, const eFiberType eFType, const eRPotName eRPName, const int nPlateID, const int nFiberID);
		bool GetODFiberParams(PFIBERPARAMS pFiberParams, const eFiberType eFType, const eRPotName eRPName, const int nPlateID, const int nFiberID);
		bool SaveRPGeometryParams(const eRPotName eRPName, const char* szDataDestination);

		HepGeom::Point3D<double> GetDetPointInAtlas(eRPotName eRPName, const HepGeom::Point3D<double>& PointInDetCS);
		HepGeom::Point3D<double> GetDetPointInRPot(eRPotName eRPName, const HepGeom::Point3D<double>& PointInDetCS);
		HepGeom::Transform3D GetTransformMatrix(const eRPotName eRPName, const eTransformElement eMatrixType);
		double GetRPotZPosInAtlas(const eRPotName eRPName);

	public:
		//fiber properties relevant to cladding coordinate system
		double GetUFiberCentreXPos(const eRPotName eRPName, const int nPlateID, const int nFiberID);
		double GetVFiberCentreXPos(const eRPotName eRPName, const int nPlateID, const int nFiberID);
		double GetUFiberAngle(const eRPotName eRPName, const int nPlateID, const int nFiberID);
		double GetVFiberAngle(const eRPotName eRPName, const int nPlateID, const int nFiberID);
		double GetODFiberCentreYPos(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID);
		double GetODFiberAngle(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID);

		void SetUFiberPositionToMainReference(const eRPotName eRPName, const int nPlateID, const int nFiberID, const HepGeom::Point3D<float>& TransPoint,const double fTransSlope);
		void SetVFiberPositionToMainReference(const eRPotName eRPName, const int nPlateID, const int nFiberID, const HepGeom::Point3D<float>& TransPoint,const double fTransSlope);
		void SetODFiberPositionToMainReference(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID, const HepGeom::Point3D<float>& TransPoint,const double fTransSlope);

	public:
		//fiber properties relevant to ATLAS coordinate system
		double GetMDFiberSlope(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID);
		double GetMDFiberOffset(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID);
		double GetMDFiberZPos(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID);
		double GetODFiberSlope(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID);
		double GetODFiberOffset(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID);
		double GetODFiberZPos(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID);
		
};

#ifndef __CINT__
	CLASS_DEF(ALFA_GeometryReader, 223218229, 1)
#endif

#endif
