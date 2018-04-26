/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************
// Name: L1CaloPprPlotManager.h
//
// Authors: Rohin T Narayan (narayan@mail.cern.ch)
//          Patrick Rieck -  rieck@physik.hu-berlin.de
//
// Abstract base class used for handling online and offline monitoring histograms.
// This class provides a basic structure for booking and filling of histograms.
// Derived classes are needed to provide values to be monitored, e.g.
// ADC peak sampling jitter (aka fine time) or non signal behaviour (pedestals).
//
// The class uses two different histogram services in order to handle offline and online monitoring.
// All the histograms are mapped to the coolID using stl maps.
//
// An instance of the class should be created in the initialize member function of
// the athena class which uses it.
//
// In the finalize() member function of the athena class calling a plotter one should
// call the MakeSummary() member function to make the summary plots. MakeSummary() creates
// two dimensional plots in eta and phi, and fills the monitored value in
// corresponding TriggerTower bins.
//
#ifndef TRIGT1CALOCALIBTOOLS_L1CALOPPRPLOTMANAGER
#define TRIGT1CALOCALIBTOOLS_L1CALOPPRPLOTMANAGER

#include <map>
#include <string>

#include "GaudiKernel/ToolHandle.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

class TH2F;
class TH2F_LW;
class TProfile;
class TProfile2D;
class TProfile_LW;
class TProfile2D_LW;

class EventInfo;
class ITHistSvc;
class ManagedMonitorToolBase;
namespace Athena {
class MsgStreamMember;
}



namespace LVL1 {
class IL1TriggerTowerTool;
class IL1CaloOfflineTriggerTowerTools;
class IL1CaloxAODOfflineTriggerTowerTools;
class TriggerTower;
}

enum CaloDivisionEnum
{
    LArFCAL1C = 0,
    LArEMECC,
    LArOverlapC,
    LArEMBC,
    LArEMBA,
    LArOverlapA,
    LArEMECA,
    LArFCAL1A,
    LArFCAL23C,
    LArHECC,
    TileEBC,
    TileLBC,
    TileLBA,
    TileEBA,
    LArHECA,
    LArFCAL23A,
    bound
};

enum CalLayerEnum
{
    Emlayer = 0,
    Hadlayer,
    undef
};

class L1CaloPprPlotManager
{
protected:
    // constructor for offline monitoring
    // monitoring strings must be provided when concstructing
    // derived classes
    L1CaloPprPlotManager(ITHistSvc* histoSvc,
                         ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
                         const int ADC_cut,
                         const unsigned int lumimax,
                         const std::string& pathInRootFile,
                         const std::string& monitoringPath,
                         const std::string& monitoringName,
                         const std::string& monitoringTitle,
                         const std::string& monitoringDimension,
                         bool doRunHistograms);
    // constructor for online monitoring
    // monitoring strings must be provided when concstructing
    // derived classes
    L1CaloPprPlotManager(ManagedMonitorToolBase* aMonObj,
                         ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
                         const int ADC_cut,
                         const unsigned int lumimax,
                         const std::string& pathInRootFile,
                         const std::string& monitoringPath,
                         const std::string& monitoringName,
                         const std::string& monitoringTitle,
                         const std::string& monitoringDimension);
    virtual ~L1CaloPprPlotManager();

    // Getting the value that is supposed to be monitored,
    // e.g. fine time, pedestal, ...
    // makes this class an abstract base class to be further defined
    // in the derived class
    virtual double getMonitoringValue(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer) = 0;

    // decide whether the given value that is supposed to be monitored
    // is acctually plotted
    // to be implemented in derived class
    virtual bool doMonitoring(double &value) = 0;

public:

    // start the actual monitoring of the considered trigger tower
    void Analyze(const EventInfo* evtInfo, const xAOD::TriggerTower* trigTower, bool channelDisabled);
    // possibility to summarize plots at the end of a run
    virtual StatusCode MakeSummary();

protected:

    virtual StatusCode bookRunHistograms();
    void SetHistogramLimits(TProfile2D_LW* hist, const int min, const int max);
    TProfile* ConvertToOfflineHist(TProfile_LW* hist);
    TProfile2D* ConvertToOfflineHist(TProfile2D_LW* hist, unsigned int binCountour);

    virtual void fillOnlineHistos(const xAOD::TriggerTower* trigTower, unsigned int& coolID, CalLayerEnum layer);
    virtual void fillGlobalOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum layer, double &value);
    virtual void fillPartitionOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer, double &value);
    virtual void fillDifferentialOnlineHistos(const xAOD::TriggerTower* trigTower, unsigned int &coolId, CalLayerEnum theLayer, double &value);
    virtual void fillOfflineHistos(const xAOD::TriggerTower* trigTower, unsigned int& coolID, CalLayerEnum layer);
    virtual void fillRunHistograms(double &eta, double &phi, CalLayerEnum theLayer, double &value);
    virtual void fillPartitionOfflineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer, double &value);
    virtual void fillDifferentialOfflineHistos(const xAOD::TriggerTower* trigTower, unsigned int &coolId, CalLayerEnum theLayer, double &value);

protected:

    CaloDivisionEnum GetDetectorRegion(double Eta, CalLayerEnum Layer);
    std::string GetDetectorRegionString(CaloDivisionEnum detectorRegion);
    std::string GetDetectorLayerString(CalLayerEnum layer);

    bool isNewRun();

protected:

    ToolHandle<LVL1::TrigT1CaloLWHistogramTool> m_histTool;
    ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_ttToolOffline;
    ToolHandle<LVL1::IL1CaloxAODOfflineTriggerTowerTools> m_xAODttToolOffline;
    ToolHandle<LVL1::IL1TriggerTowerTool> m_ttToolOnline;

    const EventInfo* m_eventInfo;
    ITHistSvc* m_histoSvc;
    Athena::MsgStreamMember* m_log;
    ManagedMonitorToolBase* m_monObj;

    // basic path for ppr monitoring histograms
    std::string m_pathInRootFile;

    // variables to be provided by derived class
    // these strings are supposed to be set in the constructor
    std::string m_monitoringPath;        // path of histograms
    std::string m_monitoringName;        // name of histograms
    std::string m_monitoringTitle;       // title of histograms
    std::string m_monitoringDimension;   // dimension of plotted value printed in histogram

    // global histograms
    TProfile_LW* m_p_online_em_valueVsLumi;
    TProfile_LW* m_p_online_had_valueVsLumi;

    TProfile_LW* m_p_online_em_valueVsBCN;
    TProfile_LW* m_p_online_had_valueVsBCN;
    
    TProfile2D_LW* m_h_ppm_em_2d_value_BCN_Lumi;
    TProfile2D_LW* m_h_ppm_had_2d_value_BCN_Lumi;

    TProfile* m_p_offline_em_valueVsLumi;
    TProfile* m_p_offline_had_valueVsLumi;

    TProfile2D_LW* m_p_online_em_etaPhiValue;
    TProfile2D_LW* m_p_online_had_etaPhiValue;

    TH2F_LW* m_h_online_em_etaPhiValueRMS;
    TH2F_LW* m_h_online_had_etaPhiValueRMS;

    TH2F* m_h_offline_em_etaPhiValueRMS;
    TH2F* m_h_offline_had_etaPhiValueRMS;

    /* global summary histograms */
    TProfile* m_p_offline_em_valueVsRunNumber;
    TProfile* m_p_offline_had_valueVsRunNumber;

    // histogram calorimeter partition maps
    std::map<CaloDivisionEnum, TProfile_LW*> m_map_online_partitionProfile_ValueVsLumi;
    std::map<CaloDivisionEnum, TProfile_LW*> m_map_online_partitionProfile_ValueVsBCN;
    std::map<CaloDivisionEnum, TProfile2D_LW*> m_map_online_partitionProfile_ValueVsLumiVsBCN;

    std::map<CaloDivisionEnum, TProfile*> m_map_offline_partitionProfile_ValueVsLumi;

    // histogram coolChannelID maps
    std::map<unsigned int, TProfile_LW*> m_map_online_coolIDProfile_ValueVsLumi;
    std::map<unsigned int, TProfile_LW*> m_map_online_coolIDProfile_ValueVsBCN;

    std::map<unsigned int, TProfile*> m_map_offline_coolIDProfile_ValueVsLumi;

    bool m_isOnline;
    bool m_firstRun;
    bool m_doRunHistograms;
    int m_ppmAdcMinValue;
    unsigned int m_lumiNo;
    unsigned int m_bunchCrossing;
    unsigned int m_lumiMax;
    unsigned int m_sampleInterval;
    unsigned int m_currentRunNo;
    unsigned int m_lastRunNo;

};

#endif
