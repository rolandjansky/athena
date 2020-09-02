/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

	#ifndef ALFA_LOCREC_h
#define ALFA_LOCREC_h

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <fstream>

#include "AthenaBaseComps/AthAlgorithm.h"

//#include "GaudiKernel/Algorithm.h"
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

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

#include "ALFA_LocRec/ALFA_UserObjects.h"
#include "ALFA_LocRec/ALFA_CenterGravity.h"
#include "ALFA_LocRec/ALFA_MDOverlap.h"
#include "ALFA_LocRec/ALFA_MDTracking.h"
#include "ALFA_LocRec/ALFA_MDMultiple.h"
#include "ALFA_LocRec/ALFA_MDGap.h"
#include "ALFA_LocRec/ALFA_HalfReco.h"
#include "ALFA_LocRec/ALFA_ODTracking.h"
#include "ALFA_LocRec/ALFA_EdgeMethod.h"

#include "TROOT.h"

//for truth particles
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

typedef struct _USERTRANSFORM
{
	Int_t iRPot;
	Float_t fAngle;
        CLHEP::Hep3Vector vecRotation;
        CLHEP::Hep3Vector vecTranslation;

} USERTRANSFORM, *PUSERTRANSFORM;


#define NSIDE  3
#define NTRACK 10
#define NTRIG  4
#define NBPM   4
//#define MAXNUMTRACKS 100

class StoreGateSvc;
class ActiveStoreSvc;

class ALFA_LocRec : public AthAlgorithm
{
	public:
		ALFA_LocRec(const std::string& name, ISvcLocator* pSvcLocator);
		~ALFA_LocRec();

	private:
		GEOMETRYCONFIGURATION m_Config;
		ALFA_GeometryReader* m_pGeometryReader;
		// a handle on Store Gate
		//StoreGateSvc* m_storeGate;
		//StoreGateSvc* m_pDetStore;

		ALFA_LocRecEvCollection*	m_pLocRecEvCollection;
		ALFA_LocRecEvent*			m_pLocRecEvent;

		ALFA_LocRecODEvCollection*	m_pLocRecODEvCollection;
		ALFA_LocRecODEvent*			m_pLocRecODEvent;

	private:
		std::list<eRPotName> m_ListExistingRPots;

		UInt_t m_eventNum;					// real event number
		Int_t m_iRunNum;
		Int_t m_iDataType;					// data type (simulation or real data) using in the local reconstruction
		Int_t m_iEvent;						// event number from zero value
		Int_t m_iMultiplicityCutMD;			// for Main Detector
		Int_t m_iNumLayerCutMD;				// for Main Detector
		Int_t m_iUVCutMD;					// for Main Detector
		Int_t m_iUVCutMDHalfReco;			// for Main Detector HalfReco algorithm
		Int_t m_iMultiplicityCutOD;			// for Overlap Detector
		Float_t m_fOverlapCutMD;			// for Main Detector
		Float_t m_fDistanceCutOD;			// for Overlap Detector
		Float_t m_iLayerCutOD;				// for Overlap Detector
		Bool_t m_bEdgeMethod_Opt_Sisters;	// for EdgeMethod
		Bool_t m_bEdgeMethod_Opt_UseGaps;	// for EdgeMethod

		//slope, offset and Z-pos for MD fibers [8][2*10][64]
		Float_t m_faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
		Float_t m_fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
		Float_t m_fzMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];

		//slope, offset and Z-pos for OD fibers [8][3][2][2*15], side 0 = right; side 1 = left (in +z direction)
		Float_t m_faOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT];
		Float_t m_fbOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT];
		Float_t m_fzOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT];

		std::string m_strKeyGeometryForReco;
		std::vector<std::string> m_vecListAlgoMD;
		std::vector<std::string> m_vecListAlgoOD;
		std::string m_strAlgoOD;
		std::string m_strAlgoMD;
		std::string m_strKeyLocRecEvCollection;
		std::string m_strKeyLocRecODEvCollection;
		std::string m_strCollectionName;
		std::string m_strODCollectionName;

	public:
		StatusCode initialize();
		StatusCode execute();
		StatusCode finalize();
		
	private:
		bool ReadGeometryDetCS();
		bool StoreReconstructionGeometry(const eRPotName eRPName, const eFiberType eFType, const char* szDataDestination);
		void SaveGeometry();
		void ClearGeometry();

		StatusCode ALFACollectionReading(std::list<MDHIT> &ListMDHits, std::list<ODHIT> &ListODHits);	

		StatusCode RecordCollection();
		StatusCode RecordODCollection();
		StatusCode ExecuteRecoMethod(const std::string strAlgo, const eRPotName eRPName, const std::list<MDHIT> &ListMDHits, const std::list<ODHIT> &ListODHits);
};

#endif	//ALFA_LOCREC_h
