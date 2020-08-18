/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#define ZDCTreeAnalysis_cxx
#include "ZDCTreeAnalysis.h"

#include <algorithm>
#include <cmath>

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TH1.h>
#include <TF1.h>
#include <TLatex.h>
#include <TPaveStats.h>
#include <TList.h>
#include <TLine.h>
#include <TGraphErrors.h>
#include "TSystem.h"

#include <cmath>

#include "CountBits.C"

int ZDCTreeAnalysis::m_DebugLevel = 5;

template <typename T> T Sqr(const T& inT) {return inT * inT;}

void ZDCTreeAnalysis::InitInternal()
{
    for (int iside = 0; iside < 2; iside++) {
        for (int imod = 0; imod < 4; imod++) {
            m_FixTau1[iside][imod] = false;
            m_FixTau2[iside][imod] = false;
            m_ModuleTau1[iside][imod] = 4;
            m_ModuleTau2[iside][imod] = 20;

            m_ChisqDivAmpCutHG[iside][imod] = 1e6;
            m_ChisqDivAmpCutLG[iside][imod] = 1e6;
            m_DeltaT0CutLow[iside][imod] = -100;
            m_DeltaT0CutHigh[iside][imod] = 100;
            m_HGOverFlowADC[iside][imod] = 800;
            m_HGUnderFlowADC[iside][imod] = 25;

            for (size_t ipar = 0; ipar < 3; ipar++) {
                m_T0SlewCoeffHG[iside][imod][ipar] = 0;
                m_T0SlewCoeffLG[iside][imod][ipar] = 0;
            }

            m_ModECalib[iside][imod] = 1;
            m_HaveLBDepECalib[iside][imod] = false;
            m_HaveLBDepT0[iside][imod] = false;
        }
    }

    zdc_side = new std::vector<int>;
    zdc_module = new std::vector<int>;

    zdc_samplesSub = new std::vector<std::vector<float> >;
    zdc_samplesDeriv = new std::vector<std::vector<float> >;
    zdc_samplesDeriv2nd = new std::vector<std::vector<float> >;
}

void ZDCTreeAnalysis::SetupBunchTrains()
{
    GetEntry(0);

    m_BCIDGap.assign(3600, 0);
    m_BCIDPosInTrain.assign(3600, 0);

    std::cout << "Run = " << runNumber << std::endl;

    TH1F *bunch_dhis = new TH1F("bunch_dhis", "bunch_dhis", 3599, 1, 3600);
    fChain->Draw("bcid>>bunch_dhis", "", "goff");

    int count_gap = 999;
    int numBCID = 0;
    int trainID = 0;
    int pos     = -1;

    for (int i = 1; i <= 3420; i++)
    {
        int content = bunch_dhis->GetBinContent(i);
        if (content < 10) {count_gap++;}
        else {
            m_BCIDGap[i] = count_gap;
            numBCID++;

            if (count_gap > 30) {
                pos = -1;
                trainID++;
                if (i != 1 ) std::cout << std::endl;
                std::cout << "Train " << trainID << ", bcids = ";
                std::cout << i << ", ";
            }
            else std::cout << i << ", ";

            pos++;
            m_BCIDPosInTrain[i] = pos;
            count_gap  = 0;
        }
    }
    std::cout << std::endl << "Found " << numBCID << " bcids" << std::endl;
    delete bunch_dhis;
}

void ZDCTreeAnalysis::OpenOutputTree(std::string file)
{
    TFile* outputFile = new TFile(file.c_str(), "recreate");
    if (!outputFile->IsOpen()) {
        std::cout << "Error opening output file " << file << ", quitting" << std::endl;
        return;
    }

    m_OutTFile = outputFile;

    m_OutTree = fChain->CloneTree(0);
    m_OutTree->SetBranchStatus("zdc_raw", 0);
    m_OutTree->Branch("bcidGap"       , &bcidGap       , "bcidGap/I"       );
    m_OutTree->Branch("bcidPosInTrain", &bcidPosInTrain, "bcidPosInTrain/I");

    m_OutTree->Branch("zdc_SumAmp"   , zdc_SumAmp   , "zdc_SumAmp[2]/F"   );
    m_OutTree->Branch("zdc_SumAmpErr", zdc_SumAmpErr, "zdc_SumAmpErr[2]/F");

    m_OutTree->Branch("zdc_SumCalibAmp"   , zdc_SumCalibAmp   , "zdc_SumCalibAmp[2]/F"   );
    m_OutTree->Branch("zdc_SumCalibAmpErr", zdc_SumCalibAmpErr, "zdc_SumCalibAmpErr[2]/F");

    m_OutTree->Branch("zdc_SumPreSampleAmp", zdc_SumPreSampleAmp, "zdc_SumPreSampleAmp[2]/F");

    m_OutTree->Branch("zdc_AvgTime"   , zdc_AvgTime    , "zdc_AvgTime[2]/F");
    m_OutTree->Branch("zdc_ModuleMask", &zdc_ModuleMask, "zdc_ModuleMask/I");

    m_OutTree->Branch("zdc_Status"    , zdc_Status    , "zdc_Status[2][4]/I");
    m_OutTree->Branch("zdc_Amp"       , zdc_Amp       , "zdc_Amp[2][4]/F");
    m_OutTree->Branch("zdc_Presample" , zdc_Presample , "zdc_Presample[2][4]/F");
    m_OutTree->Branch("zdc_FitT0"     , zdc_FitT0     , "zdc_FitT0[2][4]/F");
    m_OutTree->Branch("zdc_FitPreAmp" , zdc_FitPreAmp , "zdc_FitPreAmp[2][4]/F");
    m_OutTree->Branch("zdc_FitPreT0"  , zdc_FitPreT0  , "zdc_FitPreT0[2][4]/F");
    m_OutTree->Branch("zdc_FitPostAmp", zdc_FitPostAmp, "zdc_FitPostAmp[2][4]/F");
    m_OutTree->Branch("zdc_FitPostT0" , zdc_FitPostT0 , "zdc_FitPostT0[2][4]/F");
    m_OutTree->Branch("zdc_FitExpAmp" , zdc_FitExpAmp , "zdc_FitExpAmp[2][4]/F");
    m_OutTree->Branch("zdc_FitTau1"   , zdc_FitTau1   , "zdc_FitTau1[2][4]/F");
    m_OutTree->Branch("zdc_FitTau2"   , zdc_FitTau2   , "zdc_FitTau2[2][4]/F");
    m_OutTree->Branch("zdc_T0Corr"    , zdc_T0Corr    , "zdc_T0Corr[2][4]/F");
    m_OutTree->Branch("zdc_FitChisq"  , zdc_FitChisq  , "zdc_FitChisq[2][4]/F");

    m_OutTree->Branch("zdc_AmpError"       , zdc_AmpError       , "zdc_AmpError[2][4]/F");
    m_OutTree->Branch("zdc_BkgdMaxFraction", zdc_BkgdMaxFraction, "zdc_BkgdMaxFraction[2][4]/F");

    m_OutTree->Branch("zdc_ModuleQuality", zdc_ModuleQuality, "zdc_ModuleQuality[2][4]/F");
    m_OutTree->Branch("zdc_Quality"      , zdc_Quality      , "zdc_Quality[2]/F");

    m_OutTree->Branch("zdc_CalibAmp" , zdc_CalibAmp, "zdc_CalibAmp[2][4]/F");
    m_OutTree->Branch("zdc_CalibTime", zdc_CalibTime, "zdc_CalibTime[2][4]/F");

    m_OutTree->Branch("zdc_MaxADC", zdc_MaxADC, "zdc_MaxADC[2][4]/f");
    m_OutTree->Branch("zdc_MinADC", zdc_MinADC, "zdc_MinADC[2][4]/f");
    m_OutTree->Branch("zdc_MaxADCSample", zdc_MaxADCSample, "zdc_MaxADCSample[2][4]/I");
    m_OutTree->Branch("zdc_MinADCSample", zdc_MaxADCSample, "zdc_MinADCSample[2][4]/I");
    m_OutTree->Branch("zdc_Min2ndDeriv", zdc_Min2ndDeriv, "zdc_Min2ndDeriv[2][4]/F");
    m_OutTree->Branch("zdc_Min2ndDerivSample", zdc_Min2ndDerivSample, "zdc_Min2ndDerivSample[2][4]/I");

    m_OutTree->Branch("zdc_samplesDeriv", &zdc_samplesDeriv);
    m_OutTree->Branch("zdc_samplesDeriv2nd", &zdc_samplesDeriv2nd);

    m_OutTree->Branch("zdc_maxAmpModule", zdc_maxAmpModule, "zdc_maxAmpModule[2]/I");
    m_OutTree->Branch("zdc_maxAmp"      , zdc_maxAmp      , "zdc_maxAmp[2]/F"      );

    m_OutTree->Branch("zdc_delayedBS", zdc_delayedBS, "zdc_delayedBS[2][4]/F");

    if (mcBranches) {
        m_OutTree->Branch("zdc_maxSumTruthEnergyModule"   , zdc_maxSumTruthEnergyModule   , "zdc_maxSumTruthEnergyModule[2]/I"   );
        m_OutTree->Branch("zdc_maxSumTruthEnergy"         , zdc_maxSumTruthEnergy         , "zdc_maxSumTruthEnergy[2]/F"         );
        m_OutTree->Branch("zdc_maxSumTruthEnergyGapModule", zdc_maxSumTruthEnergyGapModule, "zdc_maxSumTruthEnergyGapModule[2]/I");
        m_OutTree->Branch("zdc_maxSumTruthEnergyGap"      , zdc_maxSumTruthEnergyGap      , "zdc_maxSumTruthEnergyGap[2]/F"      );
        m_OutTree->Branch("zdc_sumTruthEnergy"            , zdc_sumTruthEnergy            , "zdc_sumTruthEnergy[2][4]/F"         );
        m_OutTree->Branch("zdc_sumTruthEnergyGap"         , zdc_sumTruthEnergyGap         , "zdc_sumTruthEnergyGap[2][4]/F"      );
    }

    m_DoOutput = true;
}

void ZDCTreeAnalysis::CloseOutputTree()
{
    if (!m_DoOutput) return;

    m_OutTree->AutoSave();
    delete m_OutTFile;
    m_OutTFile = 0;
    m_OutTree  = 0;
    m_DoOutput = false;
}

int ZDCTreeAnalysis::SetSelection(TCut selection)
{
    if (fChain == 0) return -1;

    fChain->Draw(">>zdcTreeAnalysisEL", selection, "entrylist");
    TEntryList* entryList = (TEntryList*) gDirectory->Get("zdcTreeAnalysisEL");
    if (entryList) {
        m_entryList = entryList;
        m_haveEntryList = true;
        m_currentSelected = 0;
        return m_entryList->GetN();
    }

    return -1;
}

int ZDCTreeAnalysis::LoadSelected(unsigned int entry)
{
    if (!m_haveEntryList) return -1;
    if (entry > m_entryList->GetN()) return -1;

    unsigned int treeEntry = m_entryList->GetEntry(entry);
    LoadEntry(treeEntry);

    m_currentSelected = entry;
    return 0;
}

int ZDCTreeAnalysis::LoadNextSelected()
{
    if (!m_haveEntryList) return -1;
    if (m_currentSelected >= m_entryList->GetN()) return -1;

    unsigned int treeEntry = m_entryList->GetEntry(++m_currentSelected);
    LoadEntry(treeEntry);

    return 0;
}

void ZDCTreeAnalysis::Loop(int numEntries, int startEntry)
{
    if (fChain == 0) return;

    int loopStart = (startEntry == -1 ? m_currentEntry : startEntry);

    Long64_t nentries = fChain->GetEntriesFast() - loopStart;

    Long64_t numLoopEntry = (numEntries != -1 ? numEntries : nentries);

    Long64_t nbytes = 0, nb = 0;

    m_inLoop = true;
    for (Long64_t jentry = loopStart; jentry < loopStart + numLoopEntry; jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);
        m_currentEntry = jentry;
        nbytes += nb;

        if (jentry > 1) {
            float log10Entry = std::floor(std::log ((float) jentry) / std::log(10));
            int   modEntry   = jentry % (int) pow(10, log10Entry);

            if (modEntry == 0) {
                std::cout << "Processed event " << jentry << std::endl;
            }
        }

        if (doSavePlot) {
            if (!saveEvent) {
                continue;
            }
        }

        DoAnalysis();

        if (mcBranches) {
            for (size_t side : {0, 1}) {
                int   m_maxSumTruthEnergyModule    = 0;
                float m_maxSumTruthEnergy          = 0;
                int   m_maxSumTruthEnergyGapModule = 0;
                float m_maxSumTruthEnergyGap       = 0;
                for (size_t module : {0, 1, 2, 3}) {
                    zdc_sumTruthEnergy   [side][module] = zdc_TotalTruthEnergySum[side][module][0] + zdc_TotalTruthEnergySum[side][module][1];
                    zdc_sumTruthEnergyGap[side][module] = zdc_TotalTruthEnergySum[side][module][0];

                    if (zdc_sumTruthEnergy[side][module] > m_maxSumTruthEnergy) {
                        m_maxSumTruthEnergy       = zdc_sumTruthEnergy[side][module];
                        m_maxSumTruthEnergyModule = module;
                    }

                    if (zdc_sumTruthEnergyGap[side][module] > m_maxSumTruthEnergyGap) {
                        m_maxSumTruthEnergyGap       = zdc_sumTruthEnergyGap[side][module];
                        m_maxSumTruthEnergyGapModule = module;
                    }
                }

                zdc_maxSumTruthEnergyModule   [side] = m_maxSumTruthEnergyModule;
                zdc_maxSumTruthEnergy         [side] = m_maxSumTruthEnergy;
                zdc_maxSumTruthEnergyGapModule[side] = m_maxSumTruthEnergyGapModule;
                zdc_maxSumTruthEnergyGap      [side] = m_maxSumTruthEnergyGap;
            }
        }

        if (doSavePlot) {
            if (saveEvent) {
                index++;
                eventIndex = jentry;
                PlotFits(0);
                saveEvent = false;
            }

            if (index == nSave) {
                std::cout << "-----> Saved " << nSave << " events!" << std::endl;
                break;
            }
        }
    }

    m_inLoop = false;
}

void ZDCTreeAnalysis::DoAnalysis()
{
    // Starting a new event
    //
    m_dataAnalyzer_p->StartEvent(lumiBlock);

    bcidGap = m_BCIDGap[bcid];
    bcidPosInTrain = m_BCIDPosInTrain[bcid];

    if (m_DoOutput) {
        // //
        // //  Initialize outout arrays
        // //
    }

    zdc_side->clear();
    zdc_module->clear();

    zdc_samplesSub->clear();
    zdc_samplesDeriv->clear();
    zdc_samplesDeriv2nd->clear();

    for (size_t side : {0, 1}) {
        for (size_t module : {0, 1, 2, 3}) {

            int zdcrawOffsetmodule = (side * 4 + module) * m_zdcrawModuleSize;

            static std::vector<float> HGADCSamples(m_nSample);
            static std::vector<float> LGADCSamples(m_nSample);

            int zdcrawLGOffset = zdcrawOffsetmodule;
            int zdcrawHGOffset = zdcrawLGOffset + 2 * m_nSample;

            std::copy(zdc_raw + zdcrawLGOffset, zdc_raw + zdcrawLGOffset + m_nSample,  LGADCSamples.begin());
            std::copy(zdc_raw + zdcrawHGOffset, zdc_raw + zdcrawHGOffset + m_nSample,  HGADCSamples.begin());

            if (m_useDelayed) {
                static std::vector<float> HGADCSamplesDelay(m_nSample);
                static std::vector<float> LGADCSamplesDelay(m_nSample);

                int zdcrawLGOffset = zdcrawOffsetmodule + m_nSample;
                int zdcrawHGOffset = zdcrawLGOffset + 2 * m_nSample;

                std::copy(zdc_raw + zdcrawLGOffset, zdc_raw + zdcrawLGOffset + m_nSample,  LGADCSamplesDelay.begin());
                std::copy(zdc_raw + zdcrawHGOffset, zdc_raw + zdcrawHGOffset + m_nSample,  HGADCSamplesDelay.begin());

                m_dataAnalyzer_p->LoadAndAnalyzeData(side, module, HGADCSamples, LGADCSamples, HGADCSamplesDelay, LGADCSamplesDelay);
            }
            else {
                // Load the data
                //
                m_dataAnalyzer_p->LoadAndAnalyzeData(side, module, HGADCSamples, LGADCSamples);
            }
            if (m_DebugLevel <= 3) m_dataAnalyzer_p->GetPulseAnalyzer(side, module)->Dump();
        }
    }


    // We're done
    //
    m_dataAnalyzer_p->FinishEvent();

    int vectorIdx = 0;

    for (size_t side : {0, 1}) {
        int   m_maxAmpModule = 0;
        float m_maxAmp       = 0;
        zdc_Quality[side] = 0;
        for (size_t module : {0, 1, 2, 3}) {

            const ZDCPulseAnalyzer* pulseAna_p = m_dataAnalyzer_p->GetPulseAnalyzer(side, module);

            zdc_side->push_back(side);
            zdc_module->push_back(module);

            zdc_samplesSub->push_back(std::vector<float>(pulseAna_p->GetSamplesSub()));
            zdc_samplesDeriv->push_back(std::vector<float>(pulseAna_p->GetSamplesDeriv()));
            zdc_samplesDeriv2nd->push_back(std::vector<float>(pulseAna_p->GetSamplesDeriv2nd()));

            zdc_Presample[side][module] = pulseAna_p->GetPresample();
            zdc_Min2ndDeriv[side][module] = pulseAna_p->GetMinDeriv2nd();
            zdc_Min2ndDerivSample[side][module] = pulseAna_p->GetMinDeriv2ndIndex();

            zdc_MaxADC[side][module] = pulseAna_p->GetMaxADC();
            zdc_MinADC[side][module] = pulseAna_p->GetMinADC();

            zdc_MaxADCSample[side][module] = pulseAna_p->GetMaxADCSample();
            zdc_MinADCSample[side][module] = pulseAna_p->GetMinADCSample();

            zdc_Status[side][module]     = pulseAna_p->GetStatusMask();
            zdc_Amp[side][module]        = pulseAna_p->GetAmplitude();
            zdc_FitT0[side][module]      = pulseAna_p->GetFitT0();
            zdc_FitPreAmp[side][module]  = pulseAna_p->GetFitPreAmp();
            zdc_FitPreT0[side][module]   = pulseAna_p->GetFitPreT0();
            zdc_FitPostAmp[side][module] = pulseAna_p->GetFitPostAmp();
            zdc_FitPostT0[side][module]  = pulseAna_p->GetFitPostT0();
            zdc_FitExpAmp[side][module]  = pulseAna_p->GetFitExpAmp();
            zdc_FitTau1[side][module]    = pulseAna_p->GetFitTau1();
            zdc_FitTau2[side][module]    = pulseAna_p->GetFitTau2();
            zdc_T0Corr[side][module]     = pulseAna_p->GetT0Corr();
            zdc_FitChisq[side][module]   = pulseAna_p->GetChisq();

            zdc_AmpError[side][module] = pulseAna_p->GetAmpError();
            zdc_BkgdMaxFraction[side][module] = pulseAna_p->GetBkgdMaxFraction();

            zdc_CalibAmp[side][module] = m_dataAnalyzer_p->GetModuleCalibAmplitude(side, module);
            zdc_CalibTime[side][module] = m_dataAnalyzer_p->GetModuleCalibTime(side, module);

            zdc_delayedBS[side][module] = m_dataAnalyzer_p->GetdelayedBS(side, module);

            // ------------------------------------------------------------
            // Find max amp module
            //
            if (zdc_Amp[side][module] > m_maxAmpModule) {
                m_maxAmpModule = module;
                m_maxAmp       = zdc_Amp[side][module];
            }


            // ------------------------------------------------------------
            // fill quality infomation
            //
            zdc_ModuleQuality[side][module] = 0;
            bool m_havePulse    = (zdc_Status[side][module] & 0x1   ) == 0x1   ;
            bool m_prePulse     = (zdc_Status[side][module] & 0x100 ) == 0x100 ;
            bool m_postPulse    = (zdc_Status[side][module] & 0x200 ) == 0x200 ;
            bool m_fitFail      = (zdc_Status[side][module] & 0x400 ) == 0x400 ;
            bool m_badChisq     = (zdc_Status[side][module] & 0x800 ) == 0x800 ;
            bool m_badT0        = (zdc_Status[side][module] & 0x1000) == 0x1000;
            bool m_excluEarlyLB = (zdc_Status[side][module] & 0x2000) == 0x2000;
            bool m_preExpTail   = (zdc_Status[side][module] & 0x8000) == 0x8000;

            if (m_havePulse && !m_fitFail && !m_badChisq && !m_badT0) {
                float tempSum = 0;
                tempSum = zdc_AmpError[side][module];

                float leftExp  = zdc_FitExpAmp[side][module] * exp(-(zdc_FitT0[side][module] - 12.5) / zdc_FitTau2[side][module]);
                float rightExp = zdc_FitExpAmp[side][module] * exp(-(zdc_FitT0[side][module] + 12.5) / zdc_FitTau2[side][module]);
                float deltaExp = std::abs(leftExp - rightExp);
                if (m_preExpTail)                                    tempSum += deltaExp;
                if (m_prePulse  && !m_excluEarlyLB)                  tempSum += zdc_FitPreAmp[side][module];
                if (m_postPulse && zdc_FitPostAmp[side][module] > 0) tempSum += zdc_FitPostAmp[side][module];

                zdc_ModuleQuality[side][module] = zdc_Amp[side][module] / tempSum;
                zdc_Quality[side] += zdc_ModuleQuality[side][module];
            }

            // ------------------------------------------------------------
        }

        // ------------------------------------------------------------
        // Save max amp module
        //
        zdc_maxAmpModule[side] = m_maxAmpModule;
        zdc_maxAmp      [side] = m_maxAmp;

        zdc_SumAmp[side]    = m_dataAnalyzer_p->GetModuleSum(side);
        zdc_SumAmpErr[side] = m_dataAnalyzer_p->GetModuleSumErr(side);

        zdc_SumCalibAmp[side]    = m_dataAnalyzer_p->GetCalibModuleSum(side);
        zdc_SumCalibAmpErr[side] = m_dataAnalyzer_p->GetCalibModuleSumErr(side);

        zdc_SumPreSampleAmp[side] = m_dataAnalyzer_p->GetModuleSumPreSample(side);

        zdc_AvgTime[side]    = m_dataAnalyzer_p->GetAverageTime(side);
        zdc_sideFailed[side] = m_dataAnalyzer_p->SideFailed(side);
    }

    // -----------------------------------------------------
    // Save event bool
    //
    zdc_ModuleMask = m_dataAnalyzer_p->GetModuleMask();
    bitset<4> bs(zdc_ModuleMask >> 4);
    int countBits = bs.count();
    // -----------------------------------------------------

    if (m_DoOutput) {
        m_OutTree->Fill();
    }
}


void ZDCTreeAnalysis::PlotFits(int side)
{
    static bool first = true;
    static std::array<bool, 2> firstSide = {true, true};
    static std::array<TCanvas*, 2> plotCanvases;
    static TLatex* statusLabel;
    static TLatex* sideModuleLabel;
    static TLatex* highLowGainLabel;
    static TLatex* eventLabel;
    static TLatex* warningLabel;

    if (first) {
        gStyle->SetOptStat(0);
        gStyle->SetOptFit(111);

        sideModuleLabel = new TLatex;
        sideModuleLabel->SetTextFont(42);
        sideModuleLabel->SetTextSize(0.05);
        sideModuleLabel->SetTextAlign(12);
        sideModuleLabel->SetTextColor(2);
        sideModuleLabel->SetNDC(1);

        statusLabel = new TLatex;
        statusLabel->SetTextFont(42);
        statusLabel->SetTextSize(0.045);
        statusLabel->SetTextAlign(12);
        statusLabel->SetNDC(1);

        highLowGainLabel = new TLatex;
        highLowGainLabel->SetTextFont(42);
        highLowGainLabel->SetTextSize(0.045);
        highLowGainLabel->SetTextAlign(12);
        highLowGainLabel->SetNDC(1);

        warningLabel = new TLatex;
        warningLabel->SetTextFont(62);
        warningLabel->SetTextColor(2);
        warningLabel->SetTextSize(0.045);
        warningLabel->SetTextAlign(12);
        warningLabel->SetNDC(1);
        first = false;
    }

    if (firstSide[side]) {
        std::string canvasName = "ZDCFitsSide" + std::to_string(side);
        plotCanvases[side] = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1000, 800);

        plotCanvases[side]->Divide(2, 2, 0.001, 0.0001);
    }


    for (size_t module : {0, 1, 2, 3}) {
        TVirtualPad* pad_p = plotCanvases[side]->cd(module + 1);

        pad_p->SetTopMargin(0.03);
        pad_p->SetRightMargin(0.03);

        const ZDCPulseAnalyzer* pulseAna_p = m_dataAnalyzer_p->GetPulseAnalyzer(side, module);

        unsigned int status = pulseAna_p->GetStatusMask();

        std::cout << "Side " << side << ", module " << module << ", pulseAana_p = " << (unsigned long) pulseAna_p << ", status = " << status << std::endl;


        if (!m_useDelayed) {
            gStyle->SetOptTitle(0);
            std::shared_ptr<TGraphErrors> graph_sp = pulseAna_p->GetGraph();
            TGraphErrors* graph_p = new TGraphErrors(*graph_sp);
            float maxADC = pulseAna_p->GetMaxADC();
            float minADC = pulseAna_p->GetMinADC();
            graph_p->SetMaximum(std::max(  5.0, maxADC * 1.3));
            graph_p->SetMinimum(std::min(-10.0, minADC * 1.3));
            graph_p->Draw("ap");
        }
        else {
            gStyle->SetOptTitle(0);
            std::shared_ptr<TGraphErrors> graph_sp = pulseAna_p->GetCombinedGraph();
            TGraphErrors* graph_p = new TGraphErrors(*graph_sp);
            graph_p->Draw("ap");

            float funcMax = pulseAna_p->GetMaxADC();
            float funcMin = pulseAna_p->GetMinADC();

            float f_fitTMax = pulseAna_p->GetFitTMax();
            if (f_fitTMax < 162.5) {
                TLine *line_max = new TLine(f_fitTMax, funcMin - 0.1 * (funcMax - funcMin), f_fitTMax, funcMax + 0.1 * (funcMax - funcMin));
                line_max->SetLineStyle(2);
                line_max->Draw("same");
            }

            float f_fitTMin = pulseAna_p->GetFitTMin();
            if (f_fitTMin > 0) {
                TLine *line_min = new TLine(f_fitTMin, funcMin - 0.1 * (funcMax - funcMin), f_fitTMin, funcMax + 0.1 * (funcMax - funcMin));
                line_min->SetLineStyle(2);
                line_min->Draw("same");
            }
        }

        gPad->Modified();
        gPad->Update();


        std::ostringstream sideModule, statusHex;
        sideModule << "Side " << side << ", module " << module;
        statusHex << "0x" << std::hex << status;

        sideModuleLabel->DrawLatex(0.135, 0.6, sideModule.str().c_str());

        std::string statusString = "status = " + statusHex.str();
        statusLabel->DrawLatex(0.135, 0.54, statusString.c_str());

        if (pulseAna_p->UseLowGain()) {
            highLowGainLabel->DrawLatex(0.135, 0.48, "low gain");
        }
        else {
            highLowGainLabel->DrawLatex(0.135, 0.48, "high gain");
        }

        std::string warning = "";

        if (pulseAna_p->Failed()) warning += "Failed";

        if (pulseAna_p->BadT0()) {
            if (warning != "") warning += ",";
            warning += "BadT0";
        }

        if (pulseAna_p->BadChisq()) {
            if (warning != "") warning += ",";
            warning += "BadChisq";
        }

        if (warning != "") warningLabel->DrawLatex(0.135, 0.42, warning.c_str());

        gPad->Modified();
        gPad->Update();
    }

    plotCanvases[side]->Modified();
    plotCanvases[side]->Update();

    if (doSavePlot) {
        char name[600];
        sprintf(name, "events/%.3d_event%.4d.png", index, eventIndex);
        plotCanvases[side]->SaveAs(name);
        plotCanvases[side]->Close();
        gSystem->ProcessEvents();
        delete plotCanvases[side];
    }
}
