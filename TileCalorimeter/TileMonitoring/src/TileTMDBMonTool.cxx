/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTMDBMonTool
// PACKAGE:  TileMonitoring
//
//           Produce TMDB folder in tilemon.root for the monitoring of the TMDB
//           Input is RAW
//           Output are the pulse shape plots, pulse peak map, calibration error
//             plots, calibration coeficients plots, and noise histograms (per 
//             channel and partition)
//
// AUTHOR:   Amanda Camacho  (largely based on TileTMDBDigitsMonTool.cxx)
//
// TODO
// -- Fix TileCondToolTMDB automatic calibration
// -- Add flags "isPhysics" and "isPedestal"
//
// ********************************************************************

#include "xAODEventInfo/EventInfo.h"

#include "TileMonitoring/TileTMDBMonTool.h"


#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCondToolTMDB.h"

#include "TProfile2D.h"
#include "TPaveStats.h"
#include "TF1.h"
#include "TLatex.h"
#include "TList.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>


/*---------------------------------------------------------*/
TileTMDBMonTool::TileTMDBMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_nEventsProcessed(0)
  , m_tileToolTMDB("TileCondToolTMDB")
  , m_peakPos_map{}
  , m_coefCalib_map{}
  , m_noiseEnergy_map{}
/*---------------------------------------------------------*/
{
    declareInterface<IMonitorToolBase>(this);

    declareProperty("TileDigitsContainer", m_digitsContainerName = "MuRcvDigitsCnt");
    declareProperty("TileRawChannelContainer", m_dspContainerName = "TileRawChannelCnt");
    declareProperty("TileMuonRawChannelContainer", m_energyContainerName = "MuRcvRawChCnt");
    declareProperty("SummaryUpdateFrequency", m_summaryUpdateFrequency = 0);
    declareProperty("TileCondToolTMDB", m_tileToolTMDB);
    declareProperty("doAllPlots", m_doAllPlots = false);

    m_path = "/Tile/TMDB"; //ROOT File relative directory
    m_hasDsp = true;
}


/*---------------------------------------------------------*/
TileTMDBMonTool::~TileTMDBMonTool() {}
/*---------------------------------------------------------*/


/*---------------------------------------------------------*/
StatusCode TileTMDBMonTool::initialize()
/*---------------------------------------------------------*/
{
    ATH_MSG_INFO( "in initialize()" );

    ATH_CHECK(m_tileToolTMDB.retrieve());

    return TileFatherMonTool::initialize();
}


/*---------------------------------------------------------*/
StatusCode TileTMDBMonTool::bookHistograms()
/*---------------------------------------------------------*/
{
    ATH_MSG_DEBUG( "::bookHistograms() :  m_path =  " << m_path  );

    return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBMonTool::fillHistograms()
/*---------------------------------------------------------*/
{
    ATH_MSG_DEBUG( "in fillHistograms()" );

    // fill event info like L1 trigger type, run number, etc...
    fillEvtInfo();

    if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Run = " << m_runNum
                        << " LB = " << m_lumiBlock
                        << " Evt = " << m_evtNum
                        << " BCID = " << m_evtBCID
                        << " lvl1 = 0x" << std::hex << m_lvl1info << std::dec;

        const DataHandle<xAOD::EventInfo> eventInfo;
        if (evtStore()->retrieve(eventInfo).isSuccess()) {
            const std::vector<xAOD::EventInfo::StreamTag>& evtStreamTags = eventInfo->streamTags();
            if (!evtStreamTags.empty()) {
                msg(MSG::DEBUG) << " stream name/type:";
                for (const auto& evtStreamTag : evtStreamTags) {
                    msg(MSG::DEBUG) << " " << evtStreamTag.name() << "/" << evtStreamTag.type();
                }
            }
        }

        msg(MSG::DEBUG) << endmsg;
    }


    // -------------------------------------------------------------
    // Getting the eDsp value for the entry
    float eDspValues[5][64][8];

    const TileRawChannelContainer* dspContainer;
    if ( evtStore()->retrieve(dspContainer, m_dspContainerName).isFailure() ){
        m_hasDsp = false;
        ATH_MSG_WARNING( "Could not get DSP values. Plots that require this data won't be filled" );
    } else {
        m_hasDsp = true;
        TileRawChannelUnit::UNIT rChUnitDsp = dspContainer->get_unit();
        ATH_MSG_VERBOSE( "RawChannel unit is " << rChUnitDsp );
        if (rChUnitDsp != TileRawChannelUnit::OnlineMegaElectronVolts)
            ATH_MSG_WARNING( "eDsp is not in MeV! Pulse plots won't be correctly adjusted!" );

        TileRawChannelContainer::const_iterator itColl = (*dspContainer).begin();
        TileRawChannelContainer::const_iterator itCollEnd = (*dspContainer).end();
        TileRawChannelCollection::const_iterator it, itEnd;

        for(; itColl != itCollEnd; ++itColl) {
            int fragId = (*itColl)->identify();
            int drawer = fragId & 0x3F;
            int ros = (fragId>>8);

            it = (*itColl)->begin();
            itEnd = (*itColl)->end();

            for(; it != itEnd; ++it) {
                const TileRawChannel* rch = (*it);

                HWIdentifier hwid = rch->adc_HWID();
                int channel = m_tileHWID->channel(hwid);

                float energy = rch->amplitude();

                switch (channel){
                    case 0: // D0 cell
                        if (ros > 0 && ros < 3){
                            if (drawer % 2 == 0) eDspValues[ros][drawer][1] = energy;
                            else eDspValues[ros][drawer][0] = energy;
                        }
                        break;
                    case 13: // D1L cell
                        if (ros > 0 && ros < 3){
                            if (drawer % 2 == 0) eDspValues[ros][drawer][2] = energy;
                            else eDspValues[ros][drawer][1] = energy;
                        }
                        break;
                    case 14: // D1R cell
                        if (ros > 0 && ros < 3){
                            if (drawer % 2 == 0) eDspValues[ros][drawer][3] = energy;
                            else eDspValues[ros][drawer][2] = energy;
                        }
                        break;
                    case 25: // D2L cell
                        if (ros > 0 && ros < 3){
                            if (drawer % 2 == 0) eDspValues[ros][drawer][4] = energy;
                            else eDspValues[ros][drawer][3] = energy;
                        }
                        break;
                    case 24: // D2R cell
                        if (ros > 0 && ros < 3){
                            if (drawer % 2 == 0) eDspValues[ros][drawer][5] = energy;
                            else eDspValues[ros][drawer][4] = energy;
                        }
                        break;
                    case 41: // D3L cell
                        if (ros > 0 && ros < 3){
                            if (drawer % 2 == 0) eDspValues[ros][drawer][6] = energy;
                            else eDspValues[ros][drawer][5] = energy;
                        }
                        break;
                    case 44: // D3R cell
                        if (ros > 0 && ros < 3){
                            if (drawer % 2 == 0) eDspValues[ros][drawer][7] = energy;
                            else eDspValues[ros][drawer][6] = energy;
                        }
                        break;
                    case 17: // D5L cell
                        if (ros > 2) eDspValues[ros][drawer][0] = energy;
                        break;
                    case 16: // D5R cell
                        if (ros > 2) eDspValues[ros][drawer][1] = energy;
                        break;
                    case 37: // D6L cell
                        if (ros > 2) eDspValues[ros][drawer][2] = energy;
                        break;
                    case 38: // D6R cell
                        if (ros > 2) eDspValues[ros][drawer][3] = energy;
                        break;
                }
            }
        }
    }
    // -------------------------------------------------------------

    
 
    // -------------------------------------------------------------
    // Getting sampleTMDB value for the entry
    const TileDigitsContainer* digitsContainer;
    ATH_CHECK( evtStore()->retrieve(digitsContainer, m_digitsContainerName) );

    for (const TileDigitsCollection* digitsCollection : *digitsContainer) {

        if (digitsCollection->empty()) continue;

        HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
        int ros = m_tileHWID->ros(adc_id);
        int drawer = m_tileHWID->drawer(adc_id);

        if (m_TMDB_names[ros].empty()) ATH_CHECK(bookTMDBHistograms(digitsCollection));

        if (m_hasDsp) {
            for (const TileDigits* tile_digits : *digitsCollection) {

                adc_id = tile_digits->adc_HWID();
                int channel = m_tileHWID->channel(adc_id);

                std::vector<float> digits = tile_digits->samples();

                unsigned int n_digits = digits.size();

                bool savePulse = (eDspValues[ros][drawer][channel] > 1000) && (eDspValues[ros][drawer][channel] < 5000);

                if ( savePulse ){
                    int dig_idx = 0;
                    for (double digit : digits) { 
                        m_mPulseDig[ros][drawer][channel][dig_idx]->Fill(digit);
                        m_mPulse[ros][drawer][channel]->SetBinContent( dig_idx+1, m_mPulseDig[ros][drawer][channel][dig_idx]->GetMean() );
                        dig_idx++;
                    }
                }

                if (n_digits > 0) {
                    if (m_mPulse[ros][drawer][channel]->GetEntries() != 0) {
                        int binmax = m_mPulse[ros][drawer][channel]->GetMaximumBin();
                        m_peakPos_map[ros]->Fill(drawer + 1, channel, binmax);
                    }
                }
            } // digits
        }
    }
    // -------------------------------------------------------------



    // -------------------------------------------------------------
    // Getting eTMDB value for the entry
    const TileRawChannelContainer* energyContainer;
    ATH_CHECK( evtStore()->retrieve(energyContainer, m_energyContainerName) ); 

    TileRawChannelUnit::UNIT rChUnit = energyContainer->get_unit();
    ATH_MSG_VERBOSE( "TMDBRawChannel unit is " << rChUnit );

    TileRawChannelContainer::const_iterator itColl = (*energyContainer).begin();
    TileRawChannelContainer::const_iterator itCollEnd = (*energyContainer).end();
    TileRawChannelCollection::const_iterator it, itEnd;

    for(; itColl != itCollEnd; ++itColl) {
        int fragId = (*itColl)->identify();
        int drawer = fragId & 0x3F;
        int ros = (fragId>>8);

        it = (*itColl)->begin();
        itEnd = (*itColl)->end();

        for(; it != itEnd; ++it) {
            const TileRawChannel* rch = (*it);

            HWIdentifier hwid = rch->adc_HWID();
            int channel = m_tileHWID->channel(hwid);

            double energy = rch->amplitude(); 

            if (ros > 2) {
                TMDB::CHANNEL chName = TMDB::D5L;
                switch (channel){
                    case 0:
                        chName = TMDB::D5L;
                        break;
                    case 1:
                        chName = TMDB::D5R;
                        break;
                    case 2:
                        chName = TMDB::D6L;
                        break;
                    case 3:
                        chName = TMDB::D6R;
                        break;
                }
                
                int rosdrawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

                // FIXME: Automatic calibration is not correct (as of 9/10/18)! Must change coeficients in channelCalib function
                // double noise_MeV = m_tileToolTMDB->channelCalib(rosdrawerIdx, chName, energy);
                
                float a, b;
                m_tileToolTMDB->getCalib(rosdrawerIdx, chName, a, b);
                double noise_MeV = (b) + (a)*(energy);
                m_chanNoiseEn[ros][drawer][channel]->Fill(noise_MeV);
   
                if (m_aux_coef[ros][drawer][channel][0] == 0){
                    m_aux_coef[ros][drawer][channel][0] = a;
                    m_aux_coef[ros][drawer][channel][1] = b;
                }

                if (m_hasDsp){
                    if (m_doAllPlots)
                        m_calibError[ros][drawer][channel]->Fill(eDspValues[ros][drawer][channel] - noise_MeV);

                    bool saveEvent = (eDspValues[ros][drawer][channel] > 100) && (eDspValues[ros][drawer][channel] < 10000);
                    if (saveEvent){
                        m_aux_eTMDB[ros][drawer][channel].push_back( energy );
                        m_aux_eDsp[ros][drawer][channel].push_back( eDspValues[ros][drawer][channel] );
                        m_aux_Nevents[ros][drawer][channel]++;
                    }
                }
            } 
        }
    }
    // -------------------------------------------------------------


    ++m_nEventsProcessed;

    if ((m_summaryUpdateFrequency > 0) && (m_nEvents % m_summaryUpdateFrequency == 0)) return updateSummaryHistograms();

    return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBMonTool::updateSummaryHistograms()
/*---------------------------------------------------------*/
{
    ATH_MSG_DEBUG( "in updateSummaryHistograms()" );

    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {

        if (ros != 1 && ros != 2) { // Only have calibration for EB
            if (m_doAllPlots)
                m_noiseEnergy_summ[ros]->Reset();

            unsigned int TMDB_max_channel = m_TMDB_names[ros].size();
            for (unsigned int channel = 0; channel < TMDB_max_channel; ++channel) {
                for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
 
                        if (m_doAllPlots)
                            m_noiseEnergy_summ[ros]->Fill( m_chanNoiseEn[ros][drawer][channel]->GetRMS() );
                        m_noiseEnergy_map[ros]->Fill(drawer + 1, channel, m_chanNoiseEn[ros][drawer][channel]->GetRMS());
                        m_noiseEnergy_map[ros]->SetEntries(m_nEventsProcessed);

                } // channnel loop for (int channel = 0; channel < 48; ++channel) {
            } //loop over drawer
        }
    } //loop over ros

    return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBMonTool::bookTMDBHistograms(const TileDigitsCollection* digitsCollection)
/*---------------------------------------------------------*/
{
    std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};

    if (digitsCollection->empty()) return StatusCode::SUCCESS;

    HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);

    for (const TileDigits* digits: *digitsCollection) {

        adc_id = digits->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);

        std::string TMDB_name(ros_names[ros] + "_" + getTMDBCellName(ros, channel));

        m_TMDB_names[ros].push_back(TMDB_name);

        for (int mod=0; mod < TMDB_DRAWERS; mod++){

            std::string fullTMDBname( ros_names[ros] + std::to_string(mod+1) + "_" + getTMDBCellName(ros, channel) );
            if ( (ros == 1 || ros == 2) && (mod < 16 || mod > 27) ) { // There are no TMDBs outside of mod 16-27 for the LB
                m_mPulse[ros][mod].push_back( new TH1F() );
                m_chanNoiseEn[ros][mod].push_back( new TH1F() );
                m_calibError[ros][mod].push_back( new TH1F() ); 
            
            } else {
                if (m_doAllPlots) {
                    m_mPulse[ros][mod].push_back( book1F("MeanPulse", "TMDB_" + fullTMDBname + "_meanPulse", "Mean pulse of " + fullTMDBname, 7, 0.5, 7.5) );
                    m_mPulse[ros][mod].back()->GetYaxis()->SetTitle("[ADC]");
                    m_mPulse[ros][mod].back()->GetXaxis()->SetTitle("Sample");
                } else {
                    std::string auxTitle( "TMDB_" + fullTMDBname + "_meanPulse" );
                    m_mPulse[ros][mod].push_back( new TH1F( auxTitle.c_str(), "", 7, 0.5, 7.5) );
                }

                if (ros != 1 && ros != 2) { // There is no point in making these plots for LB, since there is no updated energy calibration
                    if (m_doAllPlots) { 
                        m_chanNoiseEn[ros][mod].push_back( book1F("NoiseAnalysis", "TMDB_" + fullTMDBname + "_noiseEne", "Energy of noise for " + fullTMDBname + " [MeV]", 101, -1010, 1010) );
                        m_chanNoiseEn[ros][mod].back()->GetXaxis()->SetTitle("[MeV]");

                        m_calibError[ros][mod].push_back( book1F("CalibErrors", "TMDB_" + fullTMDBname + "_calibError", "Calibration error in " + fullTMDBname, 101, -1010, 1010) );
                        m_calibError[ros][mod].back()->GetXaxis()->SetTitle("[MeV]");
                    } else {
                        std::string auxTitle( "TMDB_" + fullTMDBname + "_noiseEne" );
                        m_chanNoiseEn[ros][mod].push_back( new TH1F( auxTitle.c_str(), "", 101, -1010, 1010) );
                    }
                }

            }

            for (int samp=0; samp < TMDB_SAMPLES; samp++){
                std::string plotTitle( "TMDB_" + fullTMDBname + "_" + std::to_string(samp) + "_meanPulse" );
                std::string plotName( "Sample[" + std::to_string(samp) + "] in " + fullTMDBname );
                m_mPulseDig[ros][mod][channel][samp] = new TH1F( plotTitle.c_str(), plotName.c_str(), 200, -0.5, 200 );

                m_aux_Nevents[ros][mod][channel] = 0;
            }
        }
    }

    if (!m_TMDB_names[ros].empty()) ATH_CHECK( bookTMDBSummaryHistograms(ros) );

    return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBMonTool::bookTMDBSummaryHistograms(unsigned int ros)
/*---------------------------------------------------------*/
{
    std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};

    unsigned int n_TMDB = m_TMDB_names[ros].size();

    m_peakPos_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_peakPos_map", "Position of peak sample in TMDB " + ros_names[ros], 64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB -0.5, 0, 100);
    SetBinLabel(m_peakPos_map[ros]->GetYaxis(), m_TMDB_names[ros]);
    m_peakPos_map[ros]->GetZaxis()->SetTitle("[sample #]");
    m_peakPos_map[ros]->SetMinimum(0.5);
    m_peakPos_map[ros]->SetMaximum(7.5);
    m_peakPos_map[ros]->SetContour(7);

    if (ros != 1 && ros != 2) { // No point in doing these plots for LB, since there is no calibration
        m_coefCalib_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_coefCalib_map", "Divergence of angular coefficient in calibration of TMDB " + ros_names[ros], 64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB -0.5, 0, 50);
        SetBinLabel(m_coefCalib_map[ros]->GetYaxis(), m_TMDB_names[ros]);
        m_coefCalib_map[ros]->SetContour(100);

        m_noiseEnergy_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_noiseEnergy_map", "RMS of noise [MeV] in TMDB " + ros_names[ros], 64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB -0.5, 0, 200);
        SetBinLabel(m_noiseEnergy_map[ros]->GetYaxis(), m_TMDB_names[ros]);
        m_noiseEnergy_map[ros]->SetContour(100);
     
        if (m_doAllPlots){
            m_noiseEnergy_summ[ros] = book1F("", "TMDB_" + ros_names[ros] + "_noiseEnergy_summ", "RMS of noise [MeV] in TMDB " + ros_names[ros], 31, 47.5, 202.5);
            m_noiseEnergy_summ[ros]->GetXaxis()->SetTitle("[MeV]");
        }
    }

    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; drawer += 2) {
        int module = drawer + 1;
        std::string module_name = TileCalibUtils::getDrawerString(ros, drawer);

        m_peakPos_map[ros]->GetXaxis()->SetBinLabel(module, module_name.c_str());
       
        if ( ros != 1 && ros != 2 ) { // No such plots for LB
            m_coefCalib_map[ros]->GetXaxis()->SetBinLabel(module, module_name.c_str());
            m_noiseEnergy_map[ros]->GetXaxis()->SetBinLabel(module, module_name.c_str());
        }
    }

    return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBMonTool::procHistograms()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG( "in procHistograms()" );

  return updateSummaryHistograms();
}


/*---------------------------------------------------------*/
StatusCode TileTMDBMonTool::finalize()
/*---------------------------------------------------------*/
{
    ATH_MSG_INFO( "in finalize()" );

    std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};

    // Getting calibration plots
    for (int ros = 3; ros < TMDB_ROS; ros++){ // As of yet we only have calibration for EB (ros 3 ad 4)
        for (int drawer = 0; drawer < TMDB_DRAWERS; drawer++){
            for (int channel = 0; channel < 4; channel++){
                // In the EB (ros 3 and 4), one only has 4 instead of 8 channels in TMDB_CHANNELS

                std::string fullTMDBname( ros_names[ros] + std::to_string(drawer+1) + "_" + getTMDBCellName(ros, channel) ); 
                if ( m_aux_Nevents[ros][drawer][channel] != 0 ){
                    float* e_dsp_singleCh = m_aux_eDsp[ros][drawer][channel].data();
                    float* e_tmdb_singleCh = m_aux_eTMDB[ros][drawer][channel].data(); 

                    float angCoef_ref = m_aux_coef[ros][drawer][channel][0];
                    float linCoef_ref = m_aux_coef[ros][drawer][channel][1];

                    float angCoef_old = 1/angCoef_ref;
                    float linCoef = -linCoef_ref/angCoef_ref;

                    float aux_up = 0, aux_down = 0; 
                    for (int i=0; i<m_aux_Nevents[ros][drawer][channel]; i++){
                        aux_up += e_dsp_singleCh[i]*(e_tmdb_singleCh[i] - linCoef);
                        aux_down += e_dsp_singleCh[i]*e_dsp_singleCh[i];
                    }
                    float angCoef_new = aux_up/aux_down;
                    ATH_MSG_VERBOSE( "Calculated calibration for " << fullTMDBname << ": angular coeficient = " << angCoef_new << "(database value = " << angCoef_old << ")" );

                    double relDiff = fabs((angCoef_old - angCoef_new)/angCoef_old);
                    m_coefCalib_map[ros]->Fill(drawer + 1, channel, relDiff);

                    if (m_doAllPlots) {
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(4) << angCoef_new << " (ref " << angCoef_old << ")";
                        std::string title( "Ang Coef " + ss.str() );
                        m_calibPlots[ros][drawer][channel] = bookGraph( "Calibration", 
                                                                        "TMDB_" + fullTMDBname + "_calibration", 
                                                                        "Calibration of " + fullTMDBname + ". " + title, 
                                                                        m_aux_Nevents[ros][drawer][channel], 
                                                                        e_dsp_singleCh,
                                                                        e_tmdb_singleCh );
                        m_calibPlots[ros][drawer][channel]->GetXaxis()->SetTitle("eDsp (MeV)");
                        m_calibPlots[ros][drawer][channel]->GetYaxis()->SetTitle("eTMDB (ADC counts)");
                        m_calibPlots[ros][drawer][channel]->SetLineWidth(0);
                        m_calibPlots[ros][drawer][channel]->SetMarkerSize(3);
                        m_calibPlots[ros][drawer][channel]->SetMarkerStyle(3);
                        m_calibPlots[ros][drawer][channel]->SetMarkerColor(2);

                        std::string strTitle( "TMDB_" + fullTMDBname + "_fit" );
                        TF1* f = new TF1(strTitle.c_str(), "[0] + [1]*x", 0, 220000);
                        f->SetLineColor(1);
                        f->SetLineWidth(2);
                        f->SetParameter(0,linCoef);
                        f->SetParLimits(0,linCoef,linCoef);
                        m_calibPlots[ros][drawer][channel]->Fit(f);

                        delete f;
                    }
                }

            }
        }
    }

    return StatusCode::SUCCESS;
}
