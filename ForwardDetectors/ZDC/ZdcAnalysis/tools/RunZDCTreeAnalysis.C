/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <iostream>
#include <sstream>

#include "TChain.h"
#include "TString.h"
#include "TStopwatch.h"

std::string calibFile = "";
bool doECalib = false;
bool doTCalib = false;

void EnableCalibrations(std::string file, bool input_ECalib, bool input_TCalib)
{
    if (input_ECalib || input_TCalib) {
        calibFile = file;
        doECalib = input_ECalib;
        doTCalib = input_TCalib;
    }
    else {
        calibFile = "";
        doECalib = false;
        doTCalib = false;
    }
}

void PrintConfig(ZDCDataAnalyzer::ZDCModuleFloatArray array, std::string name, ofstream &file)
{
    file << name << " = " << std::endl;
    for (size_t side : {0, 1}) {
        file << "side " << side << ": ";
        for (size_t module : {0, 1, 2, 3}) {
            file << array[side][module] << ", ";
        }
        file << std::endl;
    }
    file << std::endl << std::endl;
}

void LoadCalibrations(ZDCTreeAnalysis* ana, std::string filename, int runNumber, bool eCalib = true, bool t0Calib = true)
{
    TFile* file = TFile::Open(filename.c_str());
    if (!file->IsOpen()) {
        std::cout << "Error opening calibration file, quitting" << std::endl;
    }
    char name[128];

    if (eCalib) {
        std::array<std::array<std::unique_ptr<TSpline>, 4>, 2>  ECalibSplinePtrs;
        std::unique_ptr<TSpline3> spline_p;
        for (int side : {0, 1}) {
            for (int module : {0, 1, 2, 3}) {
                sprintf(name, "ZDC_Ecalib_run%d_s%d_m%d", runNumber, side, module);
                spline_p.reset (static_cast<TSpline3*>(file->GetObjectChecked(name, "TSpline3")));

                ECalibSplinePtrs[side][module] = std::move (spline_p);
            }
        }

        ana->LoadEnergyCalibrations(ECalibSplinePtrs);
    }

    if (t0Calib) {

        std::array<std::array<std::unique_ptr<TSpline>, 4>, 2> t0CalibSplinePtrsHG, t0CalibSplinePtrsLG;
        std::unique_ptr<TSpline3> HGSpline_p, LGSpline_p;
        for (int side : {0, 1}) {
            for (int module : {0, 1, 2, 3}) {

                sprintf(name, "ZDC_T0calib_run%d_HG_s%d_m%d", runNumber, side, module);
                HGSpline_p.reset (static_cast<TSpline3*>(file->GetObjectChecked(name, "TSpline3")));
                sprintf(name, "ZDC_T0calib_run%d_LG_s%d_m%d", runNumber, side, module);
                LGSpline_p.reset (static_cast<TSpline3*>(file->GetObjectChecked(name, "TSpline3")));

                t0CalibSplinePtrsHG[side][module] = std::move (HGSpline_p);
                t0CalibSplinePtrsLG[side][module] = std::move (LGSpline_p);
            }
        }

        ana->LoadT0Calibrations(t0CalibSplinePtrsHG, t0CalibSplinePtrsLG);
    }

    file->Close();
}

ZDCTreeAnalysis* InitZDCAnalysis(TChain *chain, int year, int nSamples, bool frq80MHz, bool useTwoPass, bool useDelayed, bool fixTau1, bool fixTau2, bool doSlew)
{
    ZDCDataAnalyzer::ZDCModuleFloatArray tau1, tau2, t0HG, t0LG, peak2ndDerivMinSamples;

    if (year == 1111) { // year 1111 is for MC samples
        peak2ndDerivMinSamples = {4, 4, 4, 4,
                                  4, 4, 4, 4
                                 };
    }
    if (year == 2016 || year == 2018) {
        peak2ndDerivMinSamples = {5, 5, 5, 5,
                                  5, 5, 5, 5
                                 };
    }

    ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsHG = { -12, -12, -12, -12,
                                                                         -12, -12, -12, -12
                                                                       };

    ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsLG = { -10, -10, -10, -10,
                                                                         -10, -10, -10, -10
                                                                       };

    ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinRepassHG = { -10, -10, -10, -10,
                                                                     -10, -10, -10, -10
                                                                   };

    ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinRepassLG = { -6, -6, -6, -6,
                                                                     -6, -6, -6, -6
                                                                   };

    if (useTwoPass) {
        peak2ndDerivMinThresholdsHG = { -35, -35, -35, -35,
                                        -35, -35, -35, -35
                                      };

        peak2ndDerivMinThresholdsLG = { -20, -20, -20, -20,
                                        -20, -20, -20, -20
                                      };
    }

    float deltaT = 25;
    if (frq80MHz) deltaT = 12.5;

    // ComplexPrePulse, FermiExpLinear, GeneralPulse, FermiExp
    ZDCTreeAnalysis* ana;
    if      (year == 2016) {ana = new ZDCTreeAnalysis(chain, nSamples, deltaT, 0, "FermiExpLinear", peak2ndDerivMinSamples, peak2ndDerivMinThresholdsHG, peak2ndDerivMinThresholdsLG);}
    else if (year == 2018) {ana = new ZDCTreeAnalysis(chain, nSamples, deltaT, 0, "FermiExpLinear", peak2ndDerivMinSamples, peak2ndDerivMinThresholdsHG, peak2ndDerivMinThresholdsLG);}
    else                   {ana = new ZDCTreeAnalysis(chain, nSamples, deltaT, 0, "FermiExp"      , peak2ndDerivMinSamples, peak2ndDerivMinThresholdsHG, peak2ndDerivMinThresholdsLG);}

    ana->SetDebugLevel(5);

    if (useTwoPass) {
        ana->EnableRepass(peak2ndDerivMinRepassHG, peak2ndDerivMinRepassLG);
    }

    if (year == 1111) ana->enableMCBranches(1); // year 1111 is for MC samples
    if (year == 2016) ana->DisableModule(0, 0);

    if (useDelayed) {
        ZDCDataAnalyzer::ZDCModuleFloatArray delayUndelPedestalDiff = {0, 0, 0, 0, 0, 0, 0, 0};
        if (year == 2016) ana->EnableDelayed(-12.5, delayUndelPedestalDiff);
        if (year == 2018) {
            ZDCDataAnalyzer::ZDCModuleFloatArray delayDeltaTs = {{{{12.5, -12.5, -12.5, -12.5}}, {{ -12.5, -12.5, -12.5, -12.5}}}};
            ana->EnableDelayed(delayDeltaTs, delayUndelPedestalDiff);
        }
    }

    //  ADC over/underflow thresholds
    //
    ZDCDataAnalyzer::ZDCModuleFloatArray HGOverFlowADC  = { 800,  800,  800,  800,  800,  800,  800,  800};
    ZDCDataAnalyzer::ZDCModuleFloatArray HGUnderFlowADC = {  10,   10,   10,   10,   10,   10,   10,   10};
    ZDCDataAnalyzer::ZDCModuleFloatArray LGOverFlowADC  = {1020, 1020, 1020, 1020, 1020, 1020, 1020, 1020};

    ana->SetPeak2ndDerivMinTolerances(1);
    ana->SetADCOverUnderflowValues(HGOverFlowADC, HGUnderFlowADC, LGOverFlowADC);

    // Set Tau and nominal timing offsets
    //
    ZDCDataAnalyzer::ZDCModuleBoolArray fixTau1Arr, fixTau2Arr;

    for (size_t side : {0, 1}) {
        for (size_t module : {0, 1, 2, 3}) {
            fixTau1Arr[side][module] = fixTau1;
            fixTau2Arr[side][module] = fixTau2;
        }
    }

    if (year == 1111) { // year 1111 is for MC samples
        // -------------------------------------
        // tau1 = 4.0
        tau1 = {4.000, 4.000, 4.000, 4.000,
                4.000, 4.000, 4.000, 4.000
               };
        tau2 = {25.36, 25.05, 25.43, 25.60,
                25.11, 25.08, 25.18, 25.48
               };
        t0HG = {57.31, 57.28, 57.30, 57.28,
                57.28, 57.29, 57.31, 57.33
               };
        t0LG = {57.31, 57.28, 57.30, 57.28,
                57.28, 57.29, 57.31, 57.33
               };
        // -------------------------------------
    }

    if (year == 2016) {
        // -------------------------------------
        // fix tau1 and tau2 value                   03/05/2019   tau2 then tau1     // 2016 p+Pb   FEL
        tau1 = {4.000, 3.380, 3.661, 3.679,
                4.472, 4.656, 3.871, 4.061
               };

        tau2 = {22,    24.81, 24.48, 24.45,
                24.17, 24.22, 25.46, 24.45
               };

        t0HG = {70.00, 72.74, 73.09, 72.25,
                75.11, 74.94, 73.93, 74.45
               };
        t0LG = {70.00, 73.41, 74.27, 73.30,
                76.28, 76.07, 74.98, 76.54
               };
        // -------------------------------------
    }

    if (year == 2018) {
        // -------------------------------------
        // fix tau1 and tau2 value                   05/04/2019   tau2 then tau1     // 2018 Pb+Pb  FE
        tau1 = {3.877, 3.998, 3.821, 3.858,
                4.296, 4.064, 3.497, 3.642
               };

        tau2 = {24.40, 25.28, 25.66, 24.12,
                24.42, 24.99, 25.72, 25.29
               };

        t0HG = {70.51, 70.57, 70.13, 69.98,
                74.18, 72.79, 71.77, 72.62
               };
        t0LG = {70.70, 70.78, 70.76, 70.91,
                75.16, 73.71, 72.25, 73.61
               };
        // -------------------------------------
    }


    ana->SetTauT0Values(fixTau1Arr, fixTau2Arr, tau1, tau2, t0HG, t0LG);

    int runNumber = ana->GetRunNumber();

    if (doSlew) {

        std::array<std::array<std::vector<float>, 4>, 2> slewingParamsHG, slewingParamsLG, slew2ndHG, slew2ndLG;

        // ---------------------
        // 2016 FermiExpLinear
        //
        if (year == 2016) {
            slewingParamsHG[0][0] = {0, 0, 0, 0};
            slewingParamsHG[0][1] = { -4.780244e-01, -7.150874e-02, 4.614585e-02,  8.015731e-04};
            slewingParamsHG[0][2] = { -5.253412e-01, -5.718167e-02, 5.243121e-02,  2.128398e-03};
            slewingParamsHG[0][3] = { -5.773952e-01, -5.687478e-02, 4.564267e-02,  1.462294e-03};

            slewingParamsHG[1][0] = { 7.105115e-01, -3.686143e-02, 7.727447e-02,  5.924152e-03};
            slewingParamsHG[1][1] = { 4.052120e-02,  4.450686e-03, 8.031615e-02,  4.038097e-03};
            slewingParamsHG[1][2] = { 3.389476e-02, -2.056782e-02, 4.805321e-02, -2.627999e-03};
            slewingParamsHG[1][3] = { 2.069765e-01, -2.890419e-02, 6.084375e-02,  3.742011e-03};

            slewingParamsLG[0][0] = {0, 0, 0, 0};
            slewingParamsLG[0][1] = { -1.632547e+00, -4.827813e-01, -1.379131e-01, -2.522607e-02};
            slewingParamsLG[0][2] = { -7.254288e+00, -5.454064e+00, -1.619126e+00, -1.739665e-01};
            slewingParamsLG[0][3] = { -1.548400e+01, -1.277708e+01, -3.729333e+00, -3.700458e-01};

            slewingParamsLG[1][0] = {  1.142767e-01, -3.608906e-02,  9.642735e-02, -3.097043e-03};
            slewingParamsLG[1][1] = { -5.615388e-01, -1.655047e-02,  8.327350e-02, -4.231348e-03};
            slewingParamsLG[1][2] = { -7.370728e-01, -2.887482e-02,  8.293875e-02, -4.482743e-03};
            slewingParamsLG[1][3] = { -1.270636e+00, -2.791777e-01, -5.807295e-02, -2.332612e-02};
        }

        // ---------------------
        // 2018 FE
        //
        if (year == 2018) {
            slewingParamsHG[0][0] = { -1.335560e-01, -6.071869e-03, 5.858193e-02,  2.473300e-03};
            slewingParamsHG[0][1] = { -1.223062e-01, -4.379469e-02, 4.452285e-02,  2.130210e-03};
            slewingParamsHG[0][2] = { -1.021415e-01, -4.254239e-02, 4.939866e-02,  3.849738e-03};
            slewingParamsHG[0][3] = { -8.234056e-02, -3.938803e-02, 4.689029e-02,  2.784816e-03};

            slewingParamsHG[1][0] = { -1.640979e-01, -2.780350e-02, 5.755065e-02, -4.244651e-04};
            slewingParamsHG[1][1] = { -1.422324e-01,  2.663803e-02, 7.295366e-02,  3.740496e-03};
            slewingParamsHG[1][2] = { -9.858124e-02, -2.426132e-02, 4.895967e-02,  2.291393e-03};
            slewingParamsHG[1][3] = { -1.070401e-01, -2.256383e-03, 5.833770e-02,  2.255208e-03};

            slewingParamsLG[0][0] = { -2.588446e-01, -3.241086e-02, 7.828661e-02,  1.945547e-03};
            slewingParamsLG[0][1] = { -3.112495e-01, -7.419508e-02, 6.825776e-02,  2.148860e-03};
            slewingParamsLG[0][2] = { -3.470650e-01, -5.836748e-02, 6.204396e-02,  1.550421e-03};
            slewingParamsLG[0][3] = { -4.485435e-01, -4.603790e-02, 5.944799e-02, -1.174585e-03};

            slewingParamsLG[1][0] = { -3.291676e-01, -4.023732e-02, 8.608755e-02, -3.958167e-03};
            slewingParamsLG[1][1] = { -2.608969e-01, -2.129786e-03, 6.930791e-02, -4.141910e-03};
            slewingParamsLG[1][2] = { -2.505712e-01, -2.195804e-02, 5.137261e-02, -4.058378e-03};
            slewingParamsLG[1][3] = { -5.083206e-01,  3.776601e-02, 1.284275e-01,  1.014067e-02};
        }

        ana->SetSlewingCoeff(slewingParamsHG, slewingParamsLG);
    }

    ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutHG = {15, 15, 15, 15, 15, 15, 15, 15};
    ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutLG = {15, 15, 15, 15, 15, 15, 15, 15};

    // ----------------------
    // current one
    ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowHG, DeltaT0CutHighHG, DeltaT0CutLowLG, DeltaT0CutHighLG;
    if (year == 1111) { // year 1111 is for MC samples
        DeltaT0CutLowHG  = { -10, -10, -10, -10, -10, -10, -10, -10};
        DeltaT0CutHighHG = {  10,  10,  10,  10,  10,  10,  10,  10};
        DeltaT0CutLowLG  = { -10, -10, -10, -10, -10, -10, -10, -10};
        DeltaT0CutHighLG = {  10,  10,  10,  10,  10,  10,  10,  10};
    }
    if (year == 2016) {
        DeltaT0CutLowHG  = { -5, -5, -5, -5, -5, -5, -5, -5};
        DeltaT0CutHighHG = {  5,  5,  5,  5,  5,  5,  5,  5};
        DeltaT0CutLowLG  = { -5, -5, -5, -5, -5, -5, -5, -5};
        DeltaT0CutHighLG = {  5,  5,  5,  5,  5,  5,  5,  5};
    }
    if (year == 2018) {
        DeltaT0CutLowHG  = { -6, -5, -5, -5, -5, -5, -5, -5};
        DeltaT0CutHighHG = {  8,  8,  8, 11,  8, 10,  8, 12};
        DeltaT0CutLowLG  = { -6, -5, -5, -5, -5, -5, -5, -5};
        DeltaT0CutHighLG = {  8,  8,  8, 11,  8, 10,  8, 12};
    }

    ana->SetCutValues(chisqDivAmpCutHG, chisqDivAmpCutLG, DeltaT0CutLowHG, DeltaT0CutHighHG, DeltaT0CutLowLG, DeltaT0CutHighLG);

    if (doECalib || doTCalib) LoadCalibrations(ana, calibFile, runNumber, doECalib, doTCalib);

    return ana;
}

TChain *inputFiles(int year)
{
    TChain *chain = new TChain("zdcTree");

    chain->Add("file_path");    // add data file path here.

    return chain;
}

void RunZDCAnalysis(std::string outputFile, int nevent = -1, int year = 2016,
                    int nsamples = 7, bool freq80Mhz = false, bool useTwoPass = false, bool useDelayed = false, bool fixTau1 = true, bool fixTau2 = true, bool doSlew = false, float FitTMax = 162.5)
{
    TStopwatch timer;
    timer.Start();
    TChain *chain = inputFiles(year);

    ZDCTreeAnalysis* ana = InitZDCAnalysis(chain, year, nsamples, freq80Mhz, useTwoPass, useDelayed, fixTau1, fixTau2, doSlew);

    ana->SetFitTimeMax(FitTMax);
    ana->SetSaveFitFunc(false);

    ana->OpenOutputTree(outputFile);
    ana->Dump_setting();
    ana->Loop(nevent);
    ana->Dump_setting();
    ana->CloseOutputTree();

    timer.Stop();
    timer.Print();
}
