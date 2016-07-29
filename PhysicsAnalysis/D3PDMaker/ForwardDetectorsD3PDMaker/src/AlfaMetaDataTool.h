/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_ALFAMETADATATOOL_H
#define FORWARDDETECTORSD3PDMAKER_ALFAMETADATATOOL_H

// STL include(s):
#include <map>
#include <string>
#include <set>

// Gaudi/Athena include(s):
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IIOVSvc.h"
#include "StoreGate/StoreGateSvc.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/IMetadataTool.h"
#include "D3PDMakerInterfaces/ID3PDSvc.h"

#include "AlfaCommonHdr.h"

namespace D3PD
{

	// Forward declaration(s):
	class ID3PD;

	class AlfaMetaDataTool : public AthAlgTool, public IMetadataTool
	{

	public:
		// Regular AlgTool constructor
		AlfaMetaDataTool( const std::string& type, const std::string& name, const IInterface* parent );

		// AlgTool initialization function
		virtual StatusCode initialize();
		// Gaudi function describing which interfaces the tool implements
		virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

		// Function writing the configuration information to the D3PD
		virtual StatusCode writeMetadata( ID3PD* d3pd );

	private:
		void ClearDCSData(eDCSItem eItem);
		StatusCode COOLUpdate(IOVSVC_CALLBACK_ARGS);
		StatusCode AddCOOLFolderCallback(const std::string& szFolder);
		unsigned long long CalcDCSId(eDCSItem eItem);

	private:
		std::string m_COOLDir;
		ServiceHandle<ID3PDSvc> m_d3pdSvc;
		ServiceHandle< IIOVDbSvc > m_iovSvc;

		//DCS BLM
		ID3PD* m_DCSTreeBLM;
		unsigned long long* m_pullBLMId;
		std::vector<float>* m_pvecBLM;

		//DCS HVCHANNEL
		ID3PD* m_DCSTreeHVChannel;
		unsigned long long* m_pullHVChannelId;
		std::vector<float>* m_pvecActualVMeas;
		std::vector<float>* m_pvecActualIMeas;

		//DCS LOCALMONITORING
		ID3PD* m_DCSTreeLocalMonitoring;
		unsigned long long* m_pullLocalMonitoringId;
		std::vector<float>* m_pvecTempSensor1;
		std::vector<float>* m_pvecTempSensor2;
		std::vector<float>* m_pvecTempSensor3;
		std::vector<float>* m_pvecTempSensor4;
		std::vector<float>* m_pvecTempSensor5;

		//DCS MOVEMENT
		ID3PD* m_DCSTreeMovement;
		unsigned long long* m_pullMovementId;
		std::vector<float>* m_pvecPosLVDT;
		std::vector<float>* m_pvecPosMotor;

		//DCS RADMON
		ID3PD* m_DCSTreeRadMon;
		unsigned long long* m_pullRadMonId;
		std::vector<float>* m_pvecDose;
		std::vector<float>* m_pvecFluence;
		std::vector<float>* m_pvecTemp;

		//DCS TRIGGERRATES
		ID3PD* m_DCSTreeTriggerRates;
		unsigned long long* m_pullTriggerRatesId;
		std::vector<int>* m_pvecTriggerRates;

		//DCS FECONFIGURATION
		ID3PD* m_DCSTreeFEConfiguration;
		unsigned long long* m_pullFEConfigurationId;
		std::vector<int>* m_pvecThreshold;
		std::vector<int>* m_pvecGain;
		std::vector<std::string>* m_pvecMDGainMode;

		//DCS TRIGGERSETTINGS
		ID3PD* m_DCSTreeTriggerSettings;
		unsigned long long* m_pullTriggerSettingsId;
		std::vector<int>* m_pvecTriggerSet;
		std::vector<int>* m_pvecLatency;
   };

} // namespace D3PD

#endif // FORWARDDETECTORSD3PDMAKER_ALFAMETADATATOOL_H
