// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ONLINE // This tool is only meant for special online monitoring at Point 1

#ifndef TRT_HITS_MONITORING_TOOL_H
#define TRT_HITS_MONITORING_TOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "is/infodictionary.h"
#include "is/infoT.h"

#include "StoreGate/ReadHandleKey.h"

#include <string>

class TRT_ID;

class TRT_Hits_Monitoring_Tool: public ManagedMonitorToolBase {
public:
	TRT_Hits_Monitoring_Tool(const std::string &type, const std::string &name, const IInterface *parent);
	virtual ~TRT_Hits_Monitoring_Tool();

	virtual StatusCode initialize();
	virtual StatusCode bookHistogramsRecurrent();
	virtual StatusCode fillHistograms();
	virtual StatusCode procHistograms();

private:
	SG::ReadHandleKey<TRT_RDO_Container> m_rdoContainerKey{this, "TRTRawDataObjectName", "TRT_RDOs", "Name of TRT Raw Data Object container"};

	const TRT_ID *m_pTRTHelper;

	ISInfoDictionary m_dict;

	ISInfoFloat m_occ_longToT;
	ISInfoFloat m_occBR_longToT;
	ISInfoFloat m_occEA_longToT;
	ISInfoFloat m_occEC_longToT;

	std::string m_name_longToT;
	std::string m_nameBR_longToT;
	std::string m_nameEA_longToT;
	std::string m_nameEC_longToT;

	std::string m_partition;
	std::string m_server;
	std::string m_name;

	float m_longToTCut;
	unsigned int m_sleepTime; // milliseconds
	boost::posix_time::ptime m_lastPublishTime;
};

#endif // TRT_HITS_MONITORING_TOOL_H
#endif // ONLINE
