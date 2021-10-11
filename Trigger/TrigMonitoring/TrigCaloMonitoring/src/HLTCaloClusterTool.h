/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HLTCALOCLUSTERTOOL_H
#define HLTCALOCLUSTERTOOL_H

// Simple description to come

class StoreGateSvc;

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TMath.h"
#include <cmath>

class HLTCaloClusterTool : public IHLTMonTool {

public:
	// Constructor
	HLTCaloClusterTool (const std::string & type, const std::string & name, const IInterface* parent);
	virtual ~HLTCaloClusterTool();
	StatusCode init(); // Empty for the moment.
#ifdef ManagedMonitorToolBase_Uses_API_201401
	StatusCode book(); // called by bookHistograms
#else
	StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun); // called by bookHistograms
#endif
	StatusCode fill(); // called by fillHistograms
#ifdef ManagedMonitorToolBase_Uses_API_201401
        StatusCode proc() { return StatusCode::SUCCESS; } // called by procHistograms
#else
	StatusCode proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool /*endOfRun*/) { return StatusCode::SUCCESS; } // called by procHistograms
#endif

private:
	/** use or not ntuples */
	bool m_ntuple;
        std::string m_CaloClusterOfflineContainerName;
        std::string m_CaloClusterEFContainerName;
        std::string m_CaloClusterL2ContainerName;
        double m_L2EtCut;
        double m_L2dRCut;
        double m_EFEtCut;
        double m_EFdRCut;

        inline float deltaR( const float Phi1, const float Phi2, const float Eta1, const float Eta2) {
          float dphi = deltaPhi(Phi1,Phi2);
	  return sqrt((Eta1-Eta2)*(Eta1-Eta2)+dphi*dphi);
	}
        inline float deltaPhi( const float PhiA, const float PhiB){
	  float deltaphi=std::abs(PhiA-PhiB);
          deltaphi=std::abs(TMath::Pi()-deltaphi);
          return std::abs(TMath::Pi()-deltaphi);
	}

};

#endif
