/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLOCRECO_h
#define AFP_SIDLOCRECO_h

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <fstream>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/errorcheck.h" // CHECK( )
#include "AthenaBaseComps/AthMsgStreamMacros.h" 

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "AFP_Geometry/AFP_GeometryTool.h"
#include "CLHEP/Geometry/Point3D.h"

#include "AFP_LocReco/AFP_UserObjects.h" // for SIDHIT
#include "AFP_LocReco/AFP_SIDBasicKalman.h"

// xAOD
#include "xAODEventInfo/EventInfo.h"
#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPTrack.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"
#include "xAODForward/AFPSiHitContainer.h"


#define SIDSTATIONID 4

class StoreGateSvc;
class ActiveStoreSvc;

class AFP_SIDLocReco : public AthReentrantAlgorithm
{
	public:
        AFP_SIDLocReco(const std::string& name, ISvcLocator* pSvcLocator);
		~AFP_SIDLocReco();

	private:
		AFP_CONFIGURATION m_Config;
		ToolHandle<AFP_GeometryTool> m_pGeometry {this, "AFP_Geometry", "AFP_GeometryTool", "Tool that provides AFP geometry"};
		ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "", "Monitoring tool"};

		SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey { this, "EventInfoKey", "EventInfo", "name of EventInfo container" };
		SG::ReadHandleKey<xAOD::AFPSiHitContainer> m_AFPSiHitContainerKey { this, "AFPSiHitsContainerName", "AFPSiHitContainer", "name of AFPSiHit container" };
		SG::WriteHandleKey<xAOD::AFPTrackContainer> m_AFPTrackContainerKey{this, "AFPTrackContainerKey", "AFPTrackContainer", "name of AFPTrack container"};
		
		Gaudi::Property<int> m_iDataType{this, "DataType",0, "data type (simulation=0, real data=1) using in the local reconstruction"};
		Gaudi::Property<float> m_AmpThresh{this, "AmpThresh",10., "TD signal amplitude threshold"};

		Gaudi::Property<std::vector<std::string>> m_vecListAlgoSID{this, "vecListAlgoSID",{"SIDBasicKalman"}, "list of AFP track reco algorithms"};

	private:

		//slope and X,Y,Z-pos for SID plates [4][6]
		Float_t m_fsSID[SIDSTATIONID][SIDCNT];
		Float_t m_fxSID[SIDSTATIONID][SIDCNT];
		Float_t m_fySID[SIDSTATIONID][SIDCNT];
		Float_t m_fzSID[SIDSTATIONID][SIDCNT];
	
	public:
		StatusCode initialize();
		StatusCode execute(const EventContext &ctx) const;
		StatusCode finalize();
		
	private:
		StatusCode ReadGeometryDetCS();
		StatusCode ClearGeometry();

		// save all Si hits
		std::list<SIDHIT> AFPCollectionReading(SG::ReadHandle<xAOD::AFPSiHitContainer> siHitContainer, SG::ReadHandle<xAOD::EventInfo> eventInfo) const;

		// perform the track reconstrution
		StatusCode ExecuteRecoMethod(const std::string strAlgo, const std::list<SIDHIT> &ListSIDHits, SG::ReadHandle<xAOD::AFPSiHitContainer> siHitContainer, const EventContext &ctx) const;
};

#endif	//AFP_TDLOCRECO_h
