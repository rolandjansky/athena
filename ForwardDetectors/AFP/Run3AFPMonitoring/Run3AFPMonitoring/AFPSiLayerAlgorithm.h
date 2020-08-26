/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPSILAYERALGORITHM_H
#define AFPSILAYERALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
//#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHit.h"
//#include "LumiBlockData/BunchCrossingCondData.h"
// 2
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

#include "TRandom3.h"

class BunchCrossingCondData;

class AFPSiLayerAlgorithm : public AthMonitorAlgorithm {
public:
	AFPSiLayerAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~AFPSiLayerAlgorithm();
	virtual StatusCode initialize() override;
	virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
	//virtual StatusCode execute(const EventContext& ctx) const override;
	
private:
	std::map<std::string,std::map<std::string,int>> m_HitmapGroups;
	std::map<std::string,int> m_TrackGroup; 
	SG::ReadHandleKey<xAOD::AFPSiHitContainer> m_afpHitContainerKey;
	// 2
	ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool;



protected:
	std::vector<std::string> m_pixlayers = { "P0", "P1", "P2", "P3"};
	std::vector<std::string> m_stationnames = { "farAside", "nearAside" , "nearCside" , "farCside"};

};
#endif

