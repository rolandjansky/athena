/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_CLinkEvent_h
#define ALFA_CLinkEvent_h

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DataModel/DataLink.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <map>

#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_RawEv/ALFA_DigitCollection.h"
#include "ALFA_RawEv/ALFA_ODDigitCollection.h"
#include "ALFA_LocRecEv/ALFA_LocRecEvCollection.h"
#include "ALFA_LocRecEv/ALFA_LocRecODEvCollection.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvCollection.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvCollection.h"
// #include "ALFA_GloRecEv/ALFA_GloRecEvCollection.h"

using namespace std;

#define EVCOLLNAME_RAWDATA      "ALFA_RawData"
#define EVCOLLNAME_DIGIT	    "ALFA_DigitCollection"
#define EVCOLLNAME_ODDIGIT	    "ALFA_ODDigitCollection"
#define EVCOLLNAME_LOCREC       "ALFA_LocRecEvCollection"
#define EVCOLLNAME_LOCRECOD     "ALFA_LocRecODEvCollection"
#define EVCOLLNAME_LOCRECCORR   "ALFA_LocRecCorrEvCollection"
#define EVCOLLNAME_LOCRECCORROD "ALFA_LocRecCorrODEvCollection"
// #define EVCOLLNAME_GLOREC       "ALFA_GloRecEvCollection"

#define DCSCOLLNAME_BLM				"/RPO/DCS/BLM"
#define DCSCOLLNAME_HVCHANNEL		"/RPO/DCS/HVCHANNEL"
#define DCSCOLLNAME_LOCALMONITORING "/RPO/DCS/LOCALMONITORING"
#define DCSCOLLNAME_MOVEMENT		"/RPO/DCS/MOVEMENT"
#define DCSCOLLNAME_RADMON			"/RPO/DCS/RADMON"
#define DCSCOLLNAME_TRIGGERRATES	"/RPO/DCS/TRIGGERRATES"
#define DCSCOLLNAME_FECONFIGURATION "/RPO/DCS/FECONFIGURATION"
#define DCSCOLLNAME_TRIGGERSETTINGS "/RPO/DCS/TRIGGERSETTINGS"

#define DCSUPDATENOTIFICATION_BLM "DCSUPDATE_BLM"

enum eDVType { EDVT_UNDEFINED, EDVT_RAWDATAEVCOLLECTION, EDVT_DIGITCOLLECTION, EDVT_ODDIGITCOLLECTION, EDVT_LOCRECEVCOLLECTION, EDVT_LOCRECODEVCOLLECTION,
			   EDVT_LOCRECCORREVCOLLECTION, EDVT_LOCRECCORRODEVCOLLECTION, EDVT_GLORECEVCOLLECTION};

enum eDCSItem { EDCSI_UNDEFINED, EDCSI_BLM, EDCSI_HVCHANNEL, EDCSI_LOCALMONITORING, EDCSI_MOVEMENT, EDCSI_RADMON,
				EDCSI_TRIGGERRATES, EDCSI_FECONFIGURATION, EDCSI_TRIGGERSETTINGS };

typedef struct _DCSID {
	unsigned long long ullBlmID;
	unsigned long long ullHVChannelID;
	unsigned long long ullLocalMonitoringID;
	unsigned long long ullMovementID;
	unsigned long long ullRadMonID;
	unsigned long long ullTriggerRatesID;
	unsigned long long ullFEConfigurationID;
	unsigned long long ullTriggerSettingsID;
} DCSID, *PDCSID;

class ALFA_CLinkEvent {
  
public:
  
	ALFA_CLinkEvent();
	~ALFA_CLinkEvent();

public:
	StatusCode AddLink(eDVType eType, const void* pObj);
	const void* GetLinkedObject(eDVType eType) const;

	void SetDCSFolderID(eDCSItem eItem, const unsigned long long ullID);
	void SetDCSFolderIDs(const PDCSID pDCSIds);
	unsigned long long GetDCSFolderID(eDCSItem eItem) const;
  
private:
	DCSID m_DCSId;
	DataLink<ALFA_RawDataContainer> m_RawDataEvColl;
	DataLink<ALFA_DigitCollection> m_DigitEvColl;
	DataLink<ALFA_ODDigitCollection> m_ODDigitEvColl;
	DataLink<ALFA_LocRecEvCollection> m_LocRecEvColl;
	DataLink<ALFA_LocRecODEvCollection> m_LocRecODEvColl;
	DataLink<ALFA_LocRecCorrEvCollection> m_LocRecCorrEvColl;
	DataLink<ALFA_LocRecCorrODEvCollection> m_LocRecCorrODEvColl;
//	DataLink<ALFA_GloRecEvCollection> m_GloRecEvColl;
};

//#ifndef __CINT__
CLASS_DEF( ALFA_CLinkEvent , 106511747 , 1 )
//#endif

#endif
