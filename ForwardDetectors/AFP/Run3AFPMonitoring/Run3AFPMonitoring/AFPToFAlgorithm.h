/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPTOFALGORITHM_H
#define AFPTOFALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHit.h"
#include "LumiBlockData/BunchCrossingCondData.h"

#include "TRandom3.h"

class AFPToFAlgorithm : public AthMonitorAlgorithm {
public:
	AFPToFAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~AFPToFAlgorithm();
	virtual StatusCode initialize() override;
	virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:
	std::map<std::string,int> m_StationNamesGroup;
	std::map<std::string,int> m_TrainsToFGroup;
	std::map<std::string,std::map<std::string,int>> m_BarsInTrainsA;
	std::map<std::string,std::map<std::string,int>> m_BarsInTrainsC;
	SG::ReadHandleKey<xAOD::AFPToFHitContainer> m_afpToFHitContainerKey;
	SG::ReadCondHandleKey<BunchCrossingCondData> m_bunchCrossingKeyToF{this, "BunchCrossingKey", "BunchCrossingData", "Key BunchCrossing CDO" };

protected:
	// Only 0 and 3 are ToF stations (farAside and farCside)
	std::vector<std::string> m_stationNamesToF = { "farAside", "nearAside" , "nearCside" , "farCside" };
	std::vector<std::string> m_trainsToF = { "train0", "train1" , "train2" , "train3" };
	
	std::vector<std::string> m_trainsToFA = { "T0", "T1" , "T2" , "T3" };
	std::vector<std::string> m_trainsToFC = { "T0", "T1" , "T2" , "T3" };
	std::vector<std::string> m_barsToF = { "A", "B" , "C" , "D" };
	
};
#endif

