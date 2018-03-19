/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include <cmath>
#include <utility>

#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1CaloCalibTools/L1CaloPprPlotManager.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloxAODOfflineTriggerTowerTools.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

// constructor for offline monitoring
L1CaloPprPlotManager::L1CaloPprPlotManager(ITHistSvc* histSvc,
        ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> &ttTool_offline,
        const int ADC_cut,
        unsigned int lumiMax,
        const std::string& pathInRootFile,
        const std::string& monitoringPath,
        const std::string& monitoringName,
        const std::string& monitoringTitle,
        const std::string& monitoringDimension,
        bool doRunHistograms):
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_ttToolOffline(ttTool_offline),
    m_eventInfo(0),
    m_histoSvc(histSvc),
    m_monObj(0),
    m_pathInRootFile(pathInRootFile),
    m_monitoringPath(monitoringPath),
    m_monitoringName(monitoringName),
    m_monitoringTitle(monitoringTitle),
    m_monitoringDimension(monitoringDimension),
    m_p_online_em_valueVsLumi(0),
    m_p_online_had_valueVsLumi(0),
    m_p_online_em_valueVsBCN(0),
    m_p_online_had_valueVsBCN(0),
    m_h_ppm_em_2d_value_BCN_Lumi(0),
    m_h_ppm_had_2d_value_BCN_Lumi(0),
    m_p_offline_em_valueVsLumi(0),
    m_p_offline_had_valueVsLumi(0),
    m_p_online_em_etaPhiValue(0),
    m_p_online_had_etaPhiValue(0),
    m_h_online_em_etaPhiValueRMS(0),
    m_h_online_had_etaPhiValueRMS(0),
    m_h_offline_em_etaPhiValueRMS(0),
    m_h_offline_had_etaPhiValueRMS(0),
    m_p_offline_em_valueVsRunNumber(0),
    m_p_offline_had_valueVsRunNumber(0),
    m_isOnline(false),
    m_firstRun(true),
    m_doRunHistograms(doRunHistograms),
    m_ppmAdcMinValue(ADC_cut),
    m_lumiNo(0),
    m_bunchCrossing(0),
    m_lumiMax(lumiMax),
    m_sampleInterval(25), //25 nano second sampling interval
    m_currentRunNo(0),
    m_lastRunNo(0)
{
    m_log = new Athena::MsgStreamMember(Athena::Options::Eager, Form("L1CaloPpr%sPlotManager", m_monitoringPath.data()));
}

// --------------------------------------------------------------------------

// Overloaded constructor for online monitoring
L1CaloPprPlotManager::L1CaloPprPlotManager(ManagedMonitorToolBase* aMonObj,
        ToolHandle<LVL1::IL1TriggerTowerTool>&ttTool_online,
        int ADC_cut,
        unsigned int lumiMax,
        const std::string& pathInRootFile,
        const std::string& monitoringPath,
        const std::string& monitoringName,
        const std::string& monitoringTitle,
        const std::string& monitoringDimension):
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_ttToolOnline(ttTool_online),
    m_eventInfo(0),
    m_histoSvc(0),
    m_monObj(aMonObj),
    m_pathInRootFile(pathInRootFile),
    m_monitoringPath(monitoringPath),
    m_monitoringName(monitoringName),
    m_monitoringTitle(monitoringTitle),
    m_monitoringDimension(monitoringDimension),
    m_p_online_em_valueVsLumi(0),
    m_p_online_had_valueVsLumi(0),
    m_p_online_em_valueVsBCN(0),
    m_p_online_had_valueVsBCN(0),
    m_h_ppm_em_2d_value_BCN_Lumi(0),
    m_h_ppm_had_2d_value_BCN_Lumi(0),
    m_p_offline_em_valueVsLumi(0),
    m_p_offline_had_valueVsLumi(0),
    m_p_online_em_etaPhiValue(0),
    m_p_online_had_etaPhiValue(0),
    m_h_online_em_etaPhiValueRMS(0),
    m_h_online_had_etaPhiValueRMS(0),
    m_h_offline_em_etaPhiValueRMS(0),
    m_h_offline_had_etaPhiValueRMS(0),
    m_p_offline_em_valueVsRunNumber(0),
    m_p_offline_had_valueVsRunNumber(0),
    m_isOnline(true),
    m_firstRun(true),
    m_doRunHistograms(false),
    m_ppmAdcMinValue(ADC_cut),
    m_lumiNo(0),
    m_bunchCrossing(0),
    m_lumiMax(lumiMax),
    m_sampleInterval(25), // 25 nano second sampling interval
    m_currentRunNo(0),
    m_lastRunNo(0)
{
    m_log = new Athena::MsgStreamMember(Athena::Options::Eager, Form("L1CaloPpr%sPlotManager", m_monitoringPath.data()));
    TH1::AddDirectory(0);
}

// --------------------------------------------------------------------------

L1CaloPprPlotManager::~L1CaloPprPlotManager()
{
    delete m_log;
}

// --------------------------------------------------------------------------

void L1CaloPprPlotManager::Analyze(const EventInfo* evtInfo, const xAOD::TriggerTower* trigTower, bool channelDisabled)
{
    unsigned int coolID = trigTower->coolId();

    m_eventInfo = evtInfo;
    m_lumiNo = evtInfo->event_ID()->lumi_block();
    m_currentRunNo = evtInfo->event_ID()->run_number();
    m_bunchCrossing = evtInfo->event_ID()->bunch_crossing_id();

    if (!m_isOnline && m_doRunHistograms)
    {
        if (m_firstRun)
        {
            this->bookRunHistograms();
            m_firstRun = false;
        }
        if (this->isNewRun())
        {

            StatusCode sc =  this->MakeSummary();
            if (sc.isFailure()) {*m_log << MSG::FATAL << "Could not make Summary" << endmsg;}

            sc = this->bookRunHistograms();
            if (sc.isFailure()) {*m_log << MSG::FATAL << "Could not book Run Histograms" << endmsg;}
        }
    }
    if (m_isOnline && this->isNewRun())
    {
        m_p_online_em_valueVsLumi = 0;
        m_p_online_had_valueVsLumi = 0;
        m_p_online_em_valueVsBCN = 0;
        m_p_online_had_valueVsBCN = 0;
        m_p_online_em_etaPhiValue = 0;
        m_p_online_had_etaPhiValue = 0;
        m_h_online_em_etaPhiValueRMS = 0;
        m_h_online_had_etaPhiValueRMS = 0;
        m_map_online_partitionProfile_ValueVsLumi.clear();
        m_map_online_partitionProfile_ValueVsBCN.clear();
        m_map_online_partitionProfile_ValueVsLumiVsBCN.clear();	
        m_map_online_coolIDProfile_ValueVsLumi.clear();
        m_map_online_coolIDProfile_ValueVsBCN.clear();
    }
    //Check if the TriggerTowers are disabled or not before analysis
    if (!channelDisabled)
    {
        if ( m_isOnline) {
            if ( trigTower->layer() == 0 ) { this->fillOnlineHistos(trigTower, coolID, Emlayer); }
            if ( trigTower->layer() == 1 ) { this->fillOnlineHistos(trigTower, coolID, Hadlayer); }
        }
        else {
            if ( trigTower->layer() == 0 ) { this->fillOfflineHistos(trigTower, coolID, Emlayer); }
            if ( trigTower->layer() == 1 ) { this->fillOfflineHistos(trigTower, coolID, Hadlayer); }
        }
    }
}

// --------------------------------------------------------------------------

StatusCode L1CaloPprPlotManager::bookRunHistograms()
{
    StatusCode sc;

    m_p_offline_em_valueVsRunNumber = new TProfile(Form("ppm_em_1d_profile_adc_%sVsRunNumber", m_monitoringName.data()),
            Form("%s Vs Run No: Em TriggerTower; Run No; %s %s",
                 m_monitoringTitle.data(),
                 m_monitoringTitle.data(),
                 m_monitoringDimension.data()),
            100000,
            150000,
            200000);
    m_p_offline_em_valueVsRunNumber->Sumw2();
    sc = m_histoSvc->regHist(Form("/AANT/ADC/%s/ppm_em_1d_profile_adc_%sVsRunNumber", m_monitoringPath.data(), m_monitoringName.data()),
                             m_p_offline_em_valueVsRunNumber);
    if (sc.isFailure()) {*m_log << MSG::ERROR << "could not register histogram: " << m_p_offline_em_valueVsRunNumber->GetName() << endmsg;}
    else {*m_log << MSG::INFO << Form("EM%s VS RUN NO HISTOGRAM booked", m_monitoringName.data()) << endmsg;}

    m_p_offline_had_valueVsRunNumber = new TProfile(Form("ppm_had_1d_profile_adc_%sVsRunNumber", m_monitoringName.data()),
            Form("%s Vs Run No: Had TriggerTower; Run No; %s %s",
                 m_monitoringTitle.data(),
                 m_monitoringTitle.data(),
                 m_monitoringDimension.data()),
            100000,
            150000,
            200000);
    m_p_offline_had_valueVsRunNumber->Sumw2();
    sc = m_histoSvc->regHist(Form("/AANT/ADC/%s/ppm_had_1d_profile_adc_%sVsRunNumber", m_monitoringPath.data(), m_monitoringName.data()),
                             m_p_offline_had_valueVsRunNumber);
    if (sc.isFailure()) {*m_log << MSG::ERROR << "could not register histogram: " << m_p_offline_had_valueVsRunNumber->GetName() << endmsg;}
    else {*m_log << MSG::INFO << Form("HAD%s VS RUN NO HISTOGRAM booked", m_monitoringName.data()) << endmsg;}

    m_p_offline_em_valueVsLumi = new TProfile(Form("ppm_em_1d_profile_adc_%sVsLumi", m_monitoringName.data()),
            Form("Profile Histogram of %s vs Luminosity Block: Em TriggerTower; LumiBlock; %s %s",
                 m_monitoringTitle.data(),
                 m_monitoringTitle.data(),
                 m_monitoringDimension.data()),
            1000,
            0,
            m_lumiMax);
    sc = m_histoSvc->regHist(Form("/AANT/ADC/%s/run#%d/ppm_em_1d_profile_adc_%sVsLumi",
                                  m_monitoringPath.data(),
                                  m_currentRunNo,
                                  m_monitoringName.data()),
                             m_p_offline_em_valueVsLumi);
    m_p_offline_em_valueVsLumi->Sumw2();
    if (sc.isFailure()) {*m_log << MSG::ERROR << "could not register histogram: " << m_p_offline_em_valueVsLumi->GetName() << endmsg;}

    m_p_offline_had_valueVsLumi = new TProfile(Form("ppm_had_1d_profile_adc_%sVsLumi", m_monitoringName.data()),
            Form("Profile Histogram of %s vs Luminosity Block: Had TriggerTower; LumiBlock; %s %s",
                 m_monitoringTitle.data(),
                 m_monitoringTitle.data(),
                 m_monitoringDimension.data()),
            1000,
            0,
            m_lumiMax);
    sc = m_histoSvc->regHist(Form("/AANT/ADC/%s/run#%d/ppm_had_1d_profile_adc_%sVsLumi",
                                  m_monitoringPath.data(),
                                  m_currentRunNo,
                                  m_monitoringName.data()),
                             m_p_offline_had_valueVsLumi);
    m_p_offline_had_valueVsLumi->Sumw2();
    if (sc.isFailure()) {*m_log << MSG::ERROR << "could not register histogram: " << m_p_offline_had_valueVsLumi->GetName() << endmsg;}

    //Eta Phi Maps
    m_p_online_em_etaPhiValue    = m_histTool->bookProfilePPMEmEtaVsPhi(Form("ppm_em_2d_profile_etaPhi_adc_%s", m_monitoringName.data()),
                                   Form("#eta-#phi profile map of %s: Em Trigger Tower", m_monitoringTitle.data()));
    m_p_online_had_etaPhiValue   = m_histTool->bookProfilePPMHadEtaVsPhi(Form("ppm_had_2d_profile_etaPhi_adc_%s", m_monitoringName.data()),
                                   Form("#eta-#phi profile map of %s: Had Trigger Tower", m_monitoringTitle.data()));

    m_h_online_em_etaPhiValueRMS     = m_histTool->bookPPMEmEtaVsPhi(Form("ppm_em_2d_etaPhi_adc_%sRMS,", m_monitoringName.data()),
                                       Form("#eta-#phi Histogram of %s RMS: Em Trigger Tower", m_monitoringTitle.data()));
    m_h_online_had_etaPhiValueRMS    = m_histTool->bookPPMHadEtaVsPhi(Form("ppm_had_2d_etaPhi_adc_%sRMS", m_monitoringName.data()),
                                       Form("#eta-#phi Histogram of %s RMS: Had Trigger Tower", m_monitoringTitle.data()));

    return sc;

}

// --------------------------------------------------------------------------

/*virtual*/
void L1CaloPprPlotManager::fillOnlineHistos(const xAOD::TriggerTower* trigTower, unsigned int &mapCoolid, CalLayerEnum theLayer)
{
    // get value to be filled in histograms
    double value = this->getMonitoringValue(trigTower, theLayer);

    if ( doMonitoring(value) )
    {
        this->fillPartitionOnlineHistos(trigTower, theLayer, value);
        if (AthenaMonManager::environment() == AthenaMonManager::online) { // Don't run on Tier0
            this->fillDifferentialOnlineHistos(trigTower, mapCoolid, theLayer, value);
        }
        this->fillGlobalOnlineHistos(trigTower, theLayer, value);
    }

}

// --------------------------------------------------------------------------

/*virtual*/
void L1CaloPprPlotManager::fillGlobalOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum layer, double &value)
{
    /* Create global partition integrated plots */
    /********************************************************************************************************/
    double eta  = trigTower->eta();
    double phi  = trigTower->phi();

    if (!m_p_online_em_valueVsLumi) {

        std::string mergeMethod("");
        int nbins = m_lumiMax;
        double xmin = 0.;
        double xmax = m_lumiMax;
        if (AthenaMonManager::environment() != AthenaMonManager::online) {
            // Tier0 - assumes each job processes only one lumiblock
            mergeMethod = "mergeRebinned";
            nbins = 1;
            xmin  = m_lumiNo;
            xmax  = m_lumiNo + 1.;
        }

        ManagedMonitorToolBase::MgmtAttr_t attr = ManagedMonitorToolBase::ATTRIB_UNMANAGED;
        ManagedMonitorToolBase::MonGroup ADC_GlobalMrg(m_monObj, m_pathInRootFile, ManagedMonitorToolBase::run, attr, "", mergeMethod);
        m_histTool->setMonGroup(&ADC_GlobalMrg);

        m_p_online_em_valueVsLumi =
            m_histTool->bookProfile(Form("ppm_em_1d_profile_adc_%s", m_monitoringName.data()),
                                    Form("Profile plot of %s Vs Lumi Block: Em TriggerTower; Lumi Block; %s %s",
                                         m_monitoringTitle.data(),
                                         m_monitoringTitle.data(),
                                         m_monitoringDimension.data()),
                                    nbins, xmin, xmax);
        m_p_online_had_valueVsLumi
            = m_histTool->bookProfile(Form("ppm_had_1d_profile_adc_%s", m_monitoringName.data()),
                                      Form("Profile plot of %s Vs Lumi Block: Had TriggerTower; Lumi Block; %s %s",
                                           m_monitoringTitle.data(),
                                           m_monitoringTitle.data(),
                                           m_monitoringDimension.data()),
                                      nbins, xmin, xmax);

        ManagedMonitorToolBase::MonGroup ADC_Global(m_monObj, m_pathInRootFile, ManagedMonitorToolBase::run, attr);
        m_histTool->setMonGroup(&ADC_Global);

        m_p_online_em_etaPhiValue
            = m_histTool->bookProfilePPMEmEtaVsPhi(Form("ppm_em_2d_profile_etaPhi_adc_%s", m_monitoringName.data()),
                    Form("#eta-#phi profile map of %s: Em Trigger Tower", m_monitoringTitle.data()));

        m_p_online_had_etaPhiValue
            = m_histTool->bookProfilePPMHadEtaVsPhi(Form("ppm_had_2d_profile_etaPhi_adc_%s", m_monitoringName.data()),
                    Form("#eta-#phi profile map of %s: Had Trigger Tower", m_monitoringTitle.data()));

        m_h_online_em_etaPhiValueRMS
            = m_histTool->bookPPMEmEtaVsPhi(Form("ppm_em_2d_etaPhi_adc_%sRMS", m_monitoringName.data()),
                                            Form("#eta-#phi map of %s RMS: Em Trigger Tower", m_monitoringTitle.data()));

        m_h_online_had_etaPhiValueRMS
            = m_histTool->bookPPMHadEtaVsPhi(Form("ppm_had_2d_etaPhi_adc_%sRMS", m_monitoringName.data()),
                                             Form("#eta-#phi map of %s RMS: Had Trigger Tower", m_monitoringTitle.data()));
        m_histTool->unsetMonGroup();
    }
    //Fill Channel and detector partition integrated plots.
    if ( layer == 0 ) {m_p_online_em_valueVsLumi->Fill(m_lumiNo, value);}
    else { m_p_online_had_valueVsLumi->Fill(m_lumiNo, value);}

    //Fill Etaphi Plots
    int binX(0), binY(0);
    double entries = 0;
    double val = 0;
    double rms = 0;

    if ( layer == 0 )
    {
        m_histTool->fillPPMEmEtaVsPhi(m_p_online_em_etaPhiValue, eta, phi, value, 1);
        if (AthenaMonManager::environment() == AthenaMonManager::online) { // Needs post-processing algorithm on Tier0
            m_histTool->findBinPPMEmEtaVsPhi(m_p_online_em_etaPhiValue, eta, phi, binX, binY);
            m_p_online_em_etaPhiValue->GetBinInfo(binX, binY, entries, val, rms);
            m_histTool->setBinPPMEmEtaVsPhi(m_h_online_em_etaPhiValueRMS,
                                            binX, binY, rms, rms / sqrt(2 * entries));
        }
    }
    else
    {
        m_histTool->fillPPMHadEtaVsPhi(m_p_online_had_etaPhiValue, eta, phi, value, 1);
        if (AthenaMonManager::environment() == AthenaMonManager::online) {
            m_histTool->findBinPPMHadEtaVsPhi(m_p_online_had_etaPhiValue, eta, phi, binX, binY);
            m_p_online_had_etaPhiValue->GetBinInfo(binX, binY, entries, val, rms);
            m_histTool->setBinPPMHadEtaVsPhi(m_h_online_had_etaPhiValueRMS,
                                             binX, binY, rms, rms / sqrt(2 * entries));
        }
    }

}

// --------------------------------------------------------------------------

/*virtual*/
void L1CaloPprPlotManager::fillPartitionOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer, double &value)
{
    //Create Channel Integrated, Partition differentiated plots
    /****************************************************************************************************/
    double eta  = trigTower->eta();
    CaloDivisionEnum detectorRegion = this->GetDetectorRegion(eta, theLayer);

    std::map<CaloDivisionEnum, TProfile_LW*>::iterator part_itr = m_map_online_partitionProfile_ValueVsLumi.find(detectorRegion);
    if (part_itr == m_map_online_partitionProfile_ValueVsLumi.end())
    {
        std::string plotType = this->GetDetectorLayerString(theLayer);
        std::string detectorRegionString = this->GetDetectorRegionString(detectorRegion);
        std::string mergeMethod("");
        int nbins = m_lumiMax;
        double xmin = 0.;
        double xmax = m_lumiMax;
        if (AthenaMonManager::environment() != AthenaMonManager::online) {
            mergeMethod = "mergeRebinned";
            nbins = 1;
            xmin  = m_lumiNo;
            xmax  = m_lumiNo + 1.;
        }

        ManagedMonitorToolBase::MgmtAttr_t attr = ManagedMonitorToolBase::ATTRIB_UNMANAGED;
        ManagedMonitorToolBase::MonGroup ADC_Partitions(m_monObj, m_pathInRootFile , ManagedMonitorToolBase::run, attr, "", mergeMethod);
        m_histTool->setMonGroup(&ADC_Partitions);

        TProfile_LW* anLWProfileHist = m_histTool->bookProfile(Form("ppm_%s_1d_profile_adc_%s_%sVsLumi",
                                       plotType.data(),
                                       detectorRegionString.data(),
                                       m_monitoringName.data()),
                                       Form("Run:%d, %s Vs Lumi Profile for partition %s",
                                            m_currentRunNo,
                                            m_monitoringTitle.data(),
                                            detectorRegionString.data()),
                                       nbins, xmin, xmax);
        anLWProfileHist->Fill(m_lumiNo, value);

        m_map_online_partitionProfile_ValueVsLumi.insert( std::pair<CaloDivisionEnum, TProfile_LW*> (detectorRegion, anLWProfileHist));

        m_histTool->unsetMonGroup();
    }
    else
    {
        part_itr->second->Fill(m_lumiNo, value);
    }
}

// --------------------------------------------------------------------------

/*virtual*/
void L1CaloPprPlotManager::fillDifferentialOnlineHistos(const xAOD::TriggerTower* trigTower, unsigned int &coolId, CalLayerEnum theLayer, double &value)
{
    //Create fully differential plots
    /****************************************************************************************************/

    std::map<unsigned int, TProfile_LW*>::iterator p_itr;
    p_itr = m_map_online_coolIDProfile_ValueVsLumi.find(coolId);

    if (p_itr == m_map_online_coolIDProfile_ValueVsLumi.end())
    {
        double eta  = trigTower->eta();
        std::string plotType = this->GetDetectorLayerString(theLayer);
        CaloDivisionEnum detectorRegion = this->GetDetectorRegion(eta, theLayer);
        std::string detectorRegionString = this->GetDetectorRegionString(detectorRegion);

        ManagedMonitorToolBase::MgmtAttr_t attr = ManagedMonitorToolBase::ATTRIB_UNMANAGED;
        ManagedMonitorToolBase::MonGroup ADC_Channels(m_monObj,
                m_pathInRootFile + Form("/%s", detectorRegionString.data()),
                ManagedMonitorToolBase::run,
                attr);
        m_histTool->setMonGroup(&ADC_Channels);

        std::string titles;
        if ( m_ppmAdcMinValue > 0 ) {
            titles = Form("Run:%d, %s Vs Lumi Profile for %sFADC >%d, Channel:%08x ;Lumi Block; %s %s",
                          m_currentRunNo,
                          m_monitoringTitle.data(),
                          plotType.data(),
                          m_ppmAdcMinValue,
                          coolId,
                          m_monitoringTitle.data(),
                          m_monitoringDimension.data());
        }
        else {
            titles = Form("Run:%d, %s Vs Lumi Profile, Channel:%08x ;Lumi Block; %s %s",
                          m_currentRunNo,
                          m_monitoringTitle.data(),
                          coolId,
                          m_monitoringTitle.data(),
                          m_monitoringDimension.data());
        }
        TProfile_LW* anLWProfileHist = m_histTool->bookProfile(Form("ppm_%s_1d_profile_adc_%08x_%sVsLumi",
                                       plotType.data(),
                                       coolId,
                                       m_monitoringName.data()),
                                       titles,
                                       m_lumiMax,
                                       0,
                                       m_lumiMax);
        anLWProfileHist->Fill(m_lumiNo, value);
        m_map_online_coolIDProfile_ValueVsLumi.insert ( std::pair<unsigned int, TProfile_LW*>(coolId, anLWProfileHist) );

        m_histTool->unsetMonGroup();

    }
    else
    {
        p_itr->second->Fill(m_lumiNo, value);
    }

}


// --------------------------------------------------------------------------

/*virtual*/
void L1CaloPprPlotManager::fillOfflineHistos(const xAOD::TriggerTower* trigTower, unsigned int &mapCoolid, CalLayerEnum theLayer)
{
    // compute value to be monitored
    double value = this->getMonitoringValue(trigTower, theLayer);


    // check if histograms need to be filled
    if ( this->doMonitoring(value) )
    {

        double eta = trigTower->eta();
        double phi = trigTower->phi();


        if ( m_doRunHistograms ) { this->fillRunHistograms(eta, phi, theLayer, value); }
        // fill histograms per calorimeter partition and per channel
        this->fillPartitionOfflineHistos(trigTower, theLayer, value);
        this->fillDifferentialOfflineHistos(trigTower, mapCoolid, theLayer, value);


    }
}

// --------------------------------------------------------------------------

void L1CaloPprPlotManager::fillRunHistograms(double &eta, double &phi, CalLayerEnum theLayer, double &value)
{

    if (theLayer == Emlayer)
    {
        m_p_offline_em_valueVsRunNumber->Fill(m_currentRunNo, value);
        m_p_offline_em_valueVsLumi->Fill(m_lumiNo, value);
        m_histTool->fillPPMEmEtaVsPhi(m_p_online_em_etaPhiValue, eta, phi, value, 1);
    }
    else if (theLayer == Hadlayer)
    {
        m_p_offline_had_valueVsRunNumber->Fill(m_currentRunNo, value);
        m_p_offline_had_valueVsLumi->Fill(m_lumiNo, value);
        m_histTool->fillPPMHadEtaVsPhi(m_p_online_had_etaPhiValue, eta, phi, value);
    }
}

// --------------------------------------------------------------------------

/*virtual*/
void L1CaloPprPlotManager::fillPartitionOfflineHistos(const xAOD::TriggerTower* trigTower,
        CalLayerEnum theLayer,
        double &value)
{
    // fill offline histograms for the given calorimeter partition with the given value

    double eta = trigTower->eta();

    std::string plotType = this->GetDetectorLayerString(theLayer);
    CaloDivisionEnum detectorRegion  = this->GetDetectorRegion(eta, theLayer);
    std::string detectorRegionString = this->GetDetectorRegionString(detectorRegion);

    std::map<CaloDivisionEnum, TProfile*>::iterator part_itr = m_map_offline_partitionProfile_ValueVsLumi.find(detectorRegion);
    if ( part_itr == m_map_offline_partitionProfile_ValueVsLumi.end() )
    {
        // create new histogram for given detector region with one bin per lumi block
        TProfile* aProfileHist = new TProfile(Form("ppm_%s_1d_profile_adc_%s_%sVsLumi",
                                              plotType.data(),
                                              detectorRegionString.data(),
                                              m_monitoringName.data()),
                                              Form("Run:%d, %s Vs Lumi Profile for partition %s;Lumi Block; %s %s",
                                                      m_currentRunNo,
                                                      m_monitoringTitle.data(),
                                                      detectorRegionString.data(),
                                                      m_monitoringTitle.data(),
                                                      m_monitoringDimension.data()),
                                              m_lumiMax,
                                              0,
                                              m_lumiMax);
        aProfileHist->Sumw2();
        StatusCode sc = m_histoSvc->regHist(Form("/AANT/ADC/%s/run#%d/ppm_%s_1d_profile_adc_%s_%sVsLumi",
                                            m_monitoringPath.data(),
                                            m_currentRunNo,
                                            plotType.data(),
                                            detectorRegionString.data(),
                                            m_monitoringName.data()),
                                            aProfileHist);
        if ( sc.isFailure() ) { *m_log << MSG::ERROR << "could not register histogram: " << aProfileHist->GetName() << endmsg; }

        aProfileHist->Fill(m_lumiNo, value);
        m_map_offline_partitionProfile_ValueVsLumi.insert( std::pair<CaloDivisionEnum, TProfile*> (detectorRegion, aProfileHist) );
    }
    else
    {
        part_itr->second->Fill(m_lumiNo, value);
    }
}

// --------------------------------------------------------------------------

/*virtual*/
void L1CaloPprPlotManager::fillDifferentialOfflineHistos(const xAOD::TriggerTower* trigTower,
        unsigned int &coolId,
        CalLayerEnum theLayer,
        double &value)
{
    // fill offline histograms for the given trigger tower with the given value

    double eta = trigTower->eta();

    std::string plotType = this->GetDetectorLayerString(theLayer);
    CaloDivisionEnum detectorRegion  = this->GetDetectorRegion(eta, theLayer);
    std::string detectorRegionString = this->GetDetectorRegionString(detectorRegion);

    std::map<unsigned int, TProfile*>::iterator tt_itr = m_map_offline_coolIDProfile_ValueVsLumi.find(coolId);
    if ( tt_itr == m_map_offline_coolIDProfile_ValueVsLumi.end() )
    {
        std::string titles;
        if ( m_ppmAdcMinValue > 0 ) {
            titles = Form("Run:%d, %s Vs Lumi Profile for %sFADC >%d, Channel:%08x ;Lumi Block; %s %s",
                          m_currentRunNo,
                          m_monitoringTitle.data(),
                          plotType.data(),
                          m_ppmAdcMinValue,
                          coolId,
                          m_monitoringTitle.data(),
                          m_monitoringDimension.data());
        }
        else {
            titles = Form("Run:%d, %s Vs Lumi Profile, Channel:%08x ;Lumi Block; %s %s",
                          m_currentRunNo,
                          m_monitoringTitle.data(),
                          coolId,
                          m_monitoringTitle.data(),
                          m_monitoringDimension.data());
        }
        // create new histogram for given channel with one bin per lumi block
        TProfile* aProfileHist = new TProfile(Form("ppm_%s_1d_profile_adc_%08x_%sVsLumi",
                                              plotType.data(),
                                              coolId,
                                              m_monitoringName.data()),
                                              titles.c_str(),
                                              m_lumiMax,
                                              0,
                                              m_lumiMax);
        aProfileHist->Sumw2();
        StatusCode sc = m_histoSvc->regHist(Form("/AANT/ADC/%s/run#%d/%s/ppm_%s_1d_profile_adc_%08x_%sVsLumi",
                                            m_monitoringPath.data(),
                                            m_currentRunNo,
                                            detectorRegionString.data(),
                                            plotType.data(),
                                            coolId,
                                            m_monitoringName.data()),
                                            aProfileHist);
        if ( sc.isFailure() ) { *m_log << MSG::ERROR << "could not register histogram: " << aProfileHist->GetName() << endmsg; }

        aProfileHist->Fill(m_lumiNo, value);
        m_map_offline_coolIDProfile_ValueVsLumi.insert( std::pair<unsigned int, TProfile*> (coolId, aProfileHist) );

    }
    else
    {
        tt_itr->second->Fill(m_lumiNo, value);
    }
}

// --------------------------------------------------------------------------

StatusCode L1CaloPprPlotManager::MakeSummary()
{
    StatusCode sc;
    if ( !m_isOnline && m_doRunHistograms )
    {
        *m_log << MSG::INFO << "MakeSummary for " << m_monitoringName.data() << " plots" << endmsg;
        sc =  m_histoSvc->regHist(Form("/AANT/ADC/%s/run#%d/ppm_em_2d_profile_etaPhi_adc_%s",
                                       m_monitoringPath.data(),
                                       m_currentRunNo,
                                       m_monitoringName.data()),
                                  this->ConvertToOfflineHist(m_p_online_em_etaPhiValue, 20));
        if (sc.isFailure()) {*m_log << "could not register histogram: " << m_p_online_em_etaPhiValue->GetName() << endmsg;}

        sc = m_histoSvc->regHist(Form("/AANT/ADC/%s/run#%d/ppm_had_2d_profile_etaPhi_adc_%s",
                                      m_monitoringPath.data(),
                                      m_currentRunNo,
                                      m_monitoringName.data()),
                                 this->ConvertToOfflineHist(m_p_online_had_etaPhiValue, 20));
        if (sc.isFailure()) {*m_log << "could not register histogram: " << m_p_online_had_etaPhiValue->GetName() << endmsg;}
    }
    return sc;
}

// --------------------------------------------------------------------------

TProfile* L1CaloPprPlotManager::ConvertToOfflineHist(TProfile_LW* hist)
{
    TProfile* temp = hist->getROOTHist();
    return (TProfile*)temp->Clone();
}

// --------------------------------------------------------------------------

void L1CaloPprPlotManager::SetHistogramLimits( TProfile2D_LW* hist, const int min, const int max)
{
    hist->SetMinimum(min);
    hist->SetMaximum(max);
}

// --------------------------------------------------------------------------

//Overloaded function to Save offline TProfile.
TProfile2D* L1CaloPprPlotManager::ConvertToOfflineHist(TProfile2D_LW* hist, unsigned int binContours)
{
    TProfile2D* temp = hist->getROOTHist();
    temp->SetMaximum(temp->GetMaximum());
    temp->SetMinimum(temp->GetMinimum());
    temp->SetContour(binContours);
    *m_log << MSG::INFO << "ConvertToOfflineHist for " << temp->GetName() << endmsg;
    return (TProfile2D*)temp->Clone();
}

// --------------------------------------------------------------------------

CaloDivisionEnum L1CaloPprPlotManager::GetDetectorRegion(double etaIn, CalLayerEnum layer)
{
    CaloDivisionEnum part =  bound;

    if (layer == Emlayer)
    {
        if      (etaIn < -3.2 ) { part = LArFCAL1C; }
        else if (etaIn < -1.5 ) { part = LArEMECC; }
        else if (etaIn < -1.4 ) { part = LArOverlapC; }
        else if (etaIn <   0. ) { part = LArEMBC; }
        else if (etaIn <  1.4 ) { part = LArEMBA; }
        else if (etaIn <  1.5 ) { part = LArOverlapA; }
        else if (etaIn <  3.2 ) { part = LArEMECA; }
        else                    { part = LArFCAL1A; }
    }
    else
    {
        if      (etaIn < -3.2 ) { part = LArFCAL23C; }
        else if (etaIn < -1.5 ) { part = LArHECC; }
        else if (etaIn < -0.9 ) { part = TileEBC; }
        else if (etaIn <   0. ) { part = TileLBC; }
        else if (etaIn <  0.9 ) { part = TileLBA; }
        else if (etaIn <  1.5 ) { part = TileEBA; }
        else if (etaIn <  3.2 ) { part = LArHECA; }
        else                    { part = LArFCAL23A; }
    }
    return part;
}

// --------------------------------------------------------------------------

std::string L1CaloPprPlotManager::GetDetectorRegionString(CaloDivisionEnum detRegion)
{
    std::string stringOut;
    switch (detRegion)
    {
    case LArFCAL1C:
        stringOut = "LArFCAL1C";
        return stringOut;
        break;
    case LArEMECC:
        stringOut = "LArEMECC";
        return stringOut;
        break;
    case LArOverlapC:
        stringOut = "LArOverlapC";
        return stringOut;
        break;
    case LArEMBC:
        stringOut = "LArEMBC";
        return stringOut;
        break;
    case LArEMBA:
        stringOut = "LArEMBA";
        return stringOut;
        break;
    case LArOverlapA:
        stringOut = "LArOverlapA";
        return stringOut;
        break;
    case LArEMECA:
        stringOut = "LArEMECA";
        return stringOut;
        break;
    case LArFCAL1A:
        stringOut = "LArFCAL1A";
        return stringOut;
        break;
    case LArFCAL23C:
        stringOut = "LArFCAL23C";
        return stringOut;
        break;
    case LArHECC:
        stringOut = "LArHECC";
        return stringOut;
        break;
    case TileEBC:
        stringOut = "TileEBC";
        return stringOut;
        break;
    case TileLBC:
        stringOut = "TileLBC";
        return stringOut;
        break;
    case TileLBA:
        stringOut = "TileLBA";
        return stringOut;
        break;
    case TileEBA:
        stringOut = "TileEBA";
        return stringOut;
        break;
    case LArHECA:
        stringOut = "LArHECA";
        return stringOut;
        break;
    case LArFCAL23A:
        stringOut = "LArFCAL23A";
        return stringOut;
        break;
    case bound:
        stringOut = "NoRegion";
        return stringOut;
        break;
    }

    return stringOut;
}

// --------------------------------------------------------------------------

std::string L1CaloPprPlotManager::GetDetectorLayerString(CalLayerEnum layer)
{
    std::string stringOut;
    switch (layer)
    {
    case Emlayer:
        stringOut = "em";
        return stringOut;
        break;

    case Hadlayer:
        stringOut = "had";
        return stringOut;
        break;

    case undef:
        stringOut = "NoLayer";
        return stringOut;
        break;
    }

    return stringOut;
}

// --------------------------------------------------------------------------

bool L1CaloPprPlotManager::isNewRun()
{
    if (m_lastRunNo != m_currentRunNo && m_lastRunNo != 0)
    {
        m_lastRunNo = m_currentRunNo;
        return true;
    }
    else
    {
        return false;
    }
}
