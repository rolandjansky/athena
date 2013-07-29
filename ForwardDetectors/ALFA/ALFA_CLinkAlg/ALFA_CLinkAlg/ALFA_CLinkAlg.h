/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_CLINKALG_H
#define ALFA_CLINKALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "DataModel/DataModel/DataLink.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "ALFA_CLinkEv/ALFA_CLinkEvent.h"

#include  <string>
using namespace std;

class ALFA_CLinkAlg : public AthAlgorithm
{
public:
	ALFA_CLinkAlg (const std::string& name, ISvcLocator* pSvcLocator);
	~ALFA_CLinkAlg();

private:
	DCSID m_CurrentDCSId;
	ServiceHandle< IIOVDbSvc > m_iovSvc;
	//ServiceHandle<IIncidentSvc> m_incidentSvc;

	int m_nDataType; //data type (simulation or real data) using in the local reconstruction
	int m_nProcessingMode; //1=offline, 2=online

public:
	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();

public:
	StatusCode LoadAllEventData(ALFA_CLinkEvent* pDataEvent);
private:
	StatusCode COOLUpdate(IOVSVC_CALLBACK_ARGS_P(I, keys));
	StatusCode AddCOOLFolderCallback(const string& szFolder);
	unsigned long long CalcDCSId(eDCSItem eItem);
	StatusCode CalcAllDCSIds(PDCSID pDCSIds);
};

#endif // ALFA_CLINKALG_H
