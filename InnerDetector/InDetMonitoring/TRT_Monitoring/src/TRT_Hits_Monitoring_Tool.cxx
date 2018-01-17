/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ONLINE // This tool is only meant for special online monitoring at Point 1

#include "TRT_Monitoring/TRT_Hits_Monitoring_Tool.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "StoreGate/ReadHandle.h"

#include "boost/date_time/posix_time/posix_time_types.hpp"

TRT_Hits_Monitoring_Tool::TRT_Hits_Monitoring_Tool(const std::string &type, const std::string &name, const IInterface *parent):
	ManagedMonitorToolBase(type, name, parent),
	m_pTRTHelper(0),
	m_lastPublishTime(boost::posix_time::min_date_time) // never
{
	declareProperty("Partition",            m_partition         = "initial");
	declareProperty("Server",               m_server            = "beamconditions");
	declareProperty("Name",                 m_name              = "hitfractionTRT");
	declareProperty("LongToTCut",           m_longToTCut        = 9.375);
	declareProperty("SleepTime",            m_sleepTime         = 500); // milliseconds
}

TRT_Hits_Monitoring_Tool::~TRT_Hits_Monitoring_Tool() {
}

StatusCode TRT_Hits_Monitoring_Tool::initialize () {
	StatusCode sc;

	ATH_CHECK(ManagedMonitoringToolBase::initialize());

	ATH_CHECK(AlgTool::initialize());

	ATH_CHECK(detStore()->retrieve(m_pTRTHelper, "TRT_ID"));

	IPCPartition partition(m_partition);
	m_dict = ISInfoDictionary(partition);

	m_name_longToT   = m_server + "." + m_name + "_longToT";
	m_nameBR_longToT = m_server + "." + m_name + "Barrel_longToT";
	m_nameEA_longToT = m_server + "." + m_name + "EndCapA_longToT";
	m_nameEC_longToT = m_server + "." + m_name + "EndCapC_longToT";

	m_dict.checkin(m_name_longToT,   m_occ_longToT);
	m_dict.checkin(m_nameBR_longToT, m_occBR_longToT);
	m_dict.checkin(m_nameEA_longToT, m_occEA_longToT);
	m_dict.checkin(m_nameEC_longToT, m_occEC_longToT);

	ATH_CHECK(m_rdoContainerKey.initialize());

	return StatusCode::SUCCESS;
}

StatusCode TRT_Hits_Monitoring_Tool::bookHistogramsRecurrent() {
	return StatusCode::SUCCESS;
}

StatusCode TRT_Hits_Monitoring_Tool::fillHistograms() {
	const static boost::posix_time::time_duration sleepTime = boost::posix_time::milliseconds(m_sleepTime);
	const boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
	if (now - m_lastPublishTime < sleepTime) return StatusCode::SUCCESS; // do not publish results too often

	SG::ReadHandle<TRT_RDO_Container> rdoContainer(m_rdoContainerKey);
	if (!rdoContainer.isValid()) {
		ATH_MSG_ERROR("Could not find TRT RDO container " << m_rdoContainerKey.key());
		return StatusCode::FAILURE;
	}

	unsigned int nHits_longToT = 0;
	unsigned int nHitsBR_longToT = 0;
	unsigned int nHitsEA_longToT = 0;
	unsigned int nHitsEC_longToT = 0;

	const TRT_RDO_Container::const_iterator containerBeg = rdoContainer->begin();
	const TRT_RDO_Container::const_iterator containerEnd = rdoContainer->end();
	for (TRT_RDO_Container::const_iterator collection = containerBeg; collection != containerEnd; ++collection) {
		const InDetRawDataCollection<TRT_RDORawData> *TRT_Collection(*collection);
		if (!TRT_Collection) continue; // select only TRT RDOs

		const DataVector<TRT_RDORawData>::const_iterator collectionBeg = TRT_Collection->begin();
		const DataVector<TRT_RDORawData>::const_iterator collectionEnd = TRT_Collection->end();
		for (DataVector<TRT_RDORawData>::const_iterator rdo = collectionBeg; rdo != collectionEnd; ++rdo) {
			const TRT_LoLumRawData *rawData = dynamic_cast<const TRT_LoLumRawData *>(*rdo);
			if (!rawData) continue; // not the right data type
			if (rawData->timeOverThreshold() <= m_longToTCut) continue; // omit hits with short ToT

			++nHits_longToT;
			switch (m_pTRTHelper->barrel_ec((*rdo)->identify())) { // which detector region?
			case -1: // ... or ...
			case +1: ++nHitsBR_longToT; break;
			case +2: ++nHitsEA_longToT; break;
			case -2: ++nHitsEC_longToT; break;
			}
		}
	}

	const unsigned int nChannelsBarrel = 105088; // 52544 straws with two channels
	const unsigned int nChannelsEndcap = 122880; // 122880 straws with one channel
	const unsigned int nChannelsTotal = nChannelsBarrel + 2 * nChannelsEndcap;

	m_occ_longToT   = (float)nHits_longToT   / nChannelsTotal;
	m_occBR_longToT = (float)nHitsBR_longToT / nChannelsBarrel;
	m_occEA_longToT = (float)nHitsEA_longToT / nChannelsEndcap;
	m_occEC_longToT = (float)nHitsEC_longToT / nChannelsEndcap;

	m_dict.update(m_name_longToT,   m_occ_longToT);
	m_dict.update(m_nameBR_longToT, m_occBR_longToT);
	m_dict.update(m_nameEA_longToT, m_occEA_longToT);
	m_dict.update(m_nameEC_longToT, m_occEC_longToT);

	m_lastPublishTime = now;
	return StatusCode::SUCCESS;
}

StatusCode TRT_Hits_Monitoring_Tool::procHistograms() {
	return StatusCode::SUCCESS;
}

#endif // ONLINE
