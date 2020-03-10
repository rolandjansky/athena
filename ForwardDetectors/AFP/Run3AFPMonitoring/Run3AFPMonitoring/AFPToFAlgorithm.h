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


#include "TRandom3.h"

class AFPToFAlgorithm : public AthMonitorAlgorithm {
public:
    AFPToFAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~AFPToFAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
private:
    Gaudi::Property<bool> m_doRandom {this,"RandomHist",false};
    std::vector<int> m_abGroups1;
    std::vector<std::vector<int>> m_abGroups2;
    std::map<std::string,int> m_cGroups1;
    //std::map<std::string,std::map<std::string,std::map<std::string,int>>> m_HitmapGroups;
    std::map<std::string,std::map<std::string,int>> m_HitmapGroups;
    SG::ReadHandleKey<xAOD::AFPSiHitContainer> m_afpHitContainerKey;
    
//      m_HitmapGroups = buildToolMap<std::string,std::map<std::string,int>>
protected:
   std::vector<std::string> m_pixlayers = { "P0", "P1", "P2", "P3"};
   std::vector<std::string> m_stationnames = { "farAside", "nearAside" , "nearCside" , "farCside"};



  static const int s_cNearStationIndex;
  static const int s_cFarStationIndex;
  const std::string m_histsDirectoryName;

};
#endif

