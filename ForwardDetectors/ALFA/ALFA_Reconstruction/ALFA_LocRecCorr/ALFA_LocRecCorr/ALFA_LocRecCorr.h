/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LOCRECCORR_h
#define ALFA_LOCRECCORR_h

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>

#include "Riostream.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMinuit.h"
#include "TF1.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TMath.h"
#include "TLine.h"
#include "TProfile.h"
#include "TPad.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "ALFA_RawEv/ALFA_RawData.h"
#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_RawEv/ALFA_RawDataCollection.h"
#include "ALFA_RawEv/ALFA_DigitCollection.h"
#include "ALFA_RawEv/ALFA_ODDigitCollection.h"
#include "ALFA_Geometry/ALFA_GeometryReader.h"
#include "ALFA_Geometry/ALFA_constants.h"
#include "ALFA_LocRecEv/ALFA_LocRecEvCollection.h"
#include "ALFA_LocRecEv/ALFA_LocRecODEvCollection.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvCollection.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvCollection.h"
#include "ALFA_LocRecCorr/ALFA_UserObjects.h"

#define MAXNUMTRACKS 100

typedef struct _USERTRANSFORM
{
	int iRPot;
	double fAngle;
	CLHEP::Hep3Vector vecRotation;
	CLHEP::Hep3Vector vecTranslation;

} USERTRANSFORM, *PUSERTRANSFORM;

using namespace std;

class StoreGateSvc;
class ActiveStoreSvc;

class ALFA_LocRecCorr : public Algorithm
{
public:
	ALFA_LocRecCorr(const string& name, ISvcLocator* pSvcLocator);
	~ALFA_LocRecCorr();

private:
	GEOMETRYCONFIGURATION m_Config;
	ALFA_GeometryReader* m_pGeometryReader;

	// a handle on Store Gate
	StoreGateSvc* m_storeGate;
	StoreGateSvc* m_pDetStore;

	ALFA_LocRecCorrEvCollection*	m_pLocRecCorrEvCollection;
	ALFA_LocRecCorrODEvCollection*	m_pLocRecCorrODEvCollection;

private:
	bool m_bCoolData;
	list<eRPotName> m_ListExistingRPots;

	string m_strLocRecCollectionName;
	string m_strLocRecODCollectionName;

	Int_t m_iDataType;			//data type (simulation or real data) using in the local reconstruction
	Int_t m_iEvt;

	//slope, offset and Z-pos for MD fibers [8][2*10][64]
	double m_faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
	double m_fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
	double m_fzMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];

	//slope, offset and Z-pos for OD fibers [8][3][2][2*15], side 0 = right; side 1 = left (in +z direction)
	double m_faOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT];
	double m_fbOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT];
	double m_fzOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT];

private:
	vector<bool> m_bIsTransformInStation;
	vector<bool> m_bIsTransformInDetector;
	vector<double> m_pointTransformInDetectorB7L1U;
	vector<double> m_pointTransformInDetectorB7L1L;
	vector<double> m_pointTransformInDetectorA7L1U;
	vector<double> m_pointTransformInDetectorA7L1L;
	vector<double> m_pointTransformInDetectorB7R1U;
	vector<double> m_pointTransformInDetectorB7R1L;
	vector<double> m_pointTransformInDetectorA7R1U;
	vector<double> m_pointTransformInDetectorA7R1L;
	vector<double> m_vecTransformInDetectorB7L1U;
	vector<double> m_vecTransformInDetectorB7L1L;
	vector<double> m_vecTransformInDetectorA7L1U;
	vector<double> m_vecTransformInDetectorA7L1L;
	vector<double> m_vecTransformInDetectorB7R1U;
	vector<double> m_vecTransformInDetectorB7R1L;
	vector<double> m_vecTransformInDetectorA7R1U;
	vector<double> m_vecTransformInDetectorA7R1L;
	vector<double> m_vecTransformInStationB7L1U;
	vector<double> m_vecTransformInStationB7L1L;
	vector<double> m_vecTransformInStationA7L1U;
	vector<double> m_vecTransformInStationA7L1L;
	vector<double> m_vecTransformInStationB7R1U;
	vector<double> m_vecTransformInStationB7R1L;
	vector<double> m_vecTransformInStationA7R1U;
	vector<double> m_vecTransformInStationA7R1L;

private:
	string m_strKeyGeometryForReco;
	string m_strKeyLocRecEvCollection;
	string m_strKeyLocRecODEvCollection;
	string m_strKeyLocRecCorrEvCollection;
	string m_strKeyLocRecCorrODEvCollection;
	string m_strCollectionName;
	string m_strODCollectionName;
	string m_strTruthCollectionName;
	string m_strKeyRawDataCollection;
	string m_rootInputFileName;

private:
	HepGeom::Transform3D m_TransMatrixSt[RPOTSCNT];
	HepGeom::Transform3D m_TransMatrixLHC[RPOTSCNT];

public:
	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();

private:
	bool ReadGeometryAtlas();
	bool UpdateGeometryAtlas();
	bool SetNominalUserCorr();
	void SaveGeometry();
	void ClearGeometry();

	StatusCode RecordCollection();
	StatusCode RecordODCollection();

	HepGeom::Transform3D UserTransform3DInDetector(eRPotName eRPName);
	HepGeom::Transform3D UserTransform3DInStation(eRPotName eRPName);
	HepGeom::Point3D<double> Point3DInDetector(eRPotName eRPName);

	StatusCode AddCOOLFolderCallback(const string& szFolder);
	StatusCode COOLUpdate(IOVSVC_CALLBACK_ARGS_P(/*I*/, keys));
};

#endif	//ALFA_LOCRECCORR_h
