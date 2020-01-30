#include <string>
#include <iostream>
#include <sstream>

#include "TChain.h"
#include "TString.h"
#include "ZdcAnalysis/ZDCMsg.h"
#include "TStopwatch.h"

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

ZDCTreeAnalysis* InitZDCAnalysis(TChain *chain, int year, int nSamples, bool frq80MHz, bool useDelayed, bool fixTau1, bool fixTau2, bool doSlew)
{
    ZDCDataAnalyzer::ZDCModuleFloatArray tau1, tau2, t0HG, t0LG, peak2ndDerivMinSamples;

    if (year == 1111) peak2ndDerivMinSamples = {4, 4, 4, 4,
                                                4, 4, 4, 4};
    if (year == 2016 || year == 2018) peak2ndDerivMinSamples = {5, 5, 5, 5,
                                                                5, 5, 5, 5};

    ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsHG = { -12, -12, -12, -12,
                                                                         -12, -12, -12, -12,
                                                                       };

    ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsLG = { -10, -10, -10, -10,
                                                                         -10, -10, -10, -10,
                                                                       };


    float deltaT = 25;
    if (frq80MHz) deltaT = 12.5;

    // ComplexPrePulse, FermiExpLinear, GeneralPulse
    ZDCTreeAnalysis* ana = new ZDCTreeAnalysis(chain, nSamples, deltaT, 0, "FermiExp", peak2ndDerivMinSamples, peak2ndDerivMinThresholdsHG, peak2ndDerivMinThresholdsLG);

    ana->SetDebugLevel(5);

    if (year == 2016) ana->DisableModule(0, 0);

    if (useDelayed) {
        // ZDCDataAnalyzer::ZDCModuleFloatArray delayUndelPedestalDiff = {0, 0, 0, 0, 3.22, -0.77, -25.85, -28.68};
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

    if (year == 1111) {
        // -------------------------------------
        // fix tau1 and tau2 value for MC
        // tau1 = {3.000, 3.000, 3.000, 3.000,
        //         3.000, 3.000, 3.000, 3.000
        //        };
        // tau1 = {3.500, 3.500, 3.500, 3.500,
        //         3.500, 3.500, 3.500, 3.500
        //        };

        // tau1 = {4.500, 4.500, 4.500, 4.500,
        //         4.500, 4.500, 4.500, 4.500
        //        };

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
        // fix tau1 and tau2 value                   03/05/2019   tau2 then tau1     // 2016 p+Pb
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
        // fix tau1 and tau2 value                   05/04/2019   tau2 then tau1     // 2018 Pb+Pb
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

        slewingParamsHG[0][0] = {0, -7.904e-02, 4.686e-02, 1.530e-03 };
        slewingParamsHG[0][1] = { -0.517444, -0.0644051, 0.0496465, 0.0012064};
        slewingParamsHG[0][2] = { -0.575125, -0.0473179, 0.0564227, 0.0025134};
        slewingParamsHG[0][3] = { -0.600446, -0.0574149, 0.0467646, 0.0017492};

        slewingParamsHG[1][0] = {  0.670605, -0.0454974, 0.0878822, 0.0110132};
        slewingParamsHG[1][1] = { -0.016475, -0.0076705, 0.0868254, 0.0078555};
        slewingParamsHG[1][2] = { -0.006359, -0.0284119, 0.0626615, 0.0038606};
        slewingParamsHG[1][3] = {  0.169817, -0.0304877, 0.0674829, 0.005483};

        slewingParamsLG[0][0] = { 0, 1.708e-02, 7.929e-02, 5.079e-03   };
        slewingParamsLG[0][1] = { -1.44148, -0.214415, -0.03659, -0.0133405};
        slewingParamsLG[0][2] = { -9.71674, -7.580210, -2.23929, -0.233969};
        slewingParamsLG[0][3] = { -22.8444, -18.7901 , -5.36434, -0.517689};

        slewingParamsLG[1][0] = {  0.08029, -0.0406899,  0.100793, -0.0033816};
        slewingParamsLG[1][1] = { -0.60508, -0.0184704,  0.085549, -0.0047334};
        slewingParamsLG[1][2] = { -0.76608, -0.0250434,  0.083663, -0.0053537};
        slewingParamsLG[1][3] = { -1.4581 , -0.503884 , -0.161387, -0.0379136};

        // 4 sigma
        slew2ndHG[0][0] = {0., 0., 0., 0.};
        slew2ndHG[0][1] = {5.523101, 2.735484e-3, -3.045758e-6,  2.398731e-9 };
        slew2ndHG[0][2] = {5.401597, 2.664528e-3, -1.656913e-6,  6.526235e-10};
        slew2ndHG[0][3] = {5.068347, 2.934560e-3, -6.166999e-7, -9.100381e-10};

        slew2ndHG[1][0] = {6.285770, 1.364173e-3, -5.582632e-7,  4.050092e-10};
        slew2ndHG[1][1] = {6.388118, 1.059508e-3, -1.162300e-7,  2.072714e-10};
        slew2ndHG[1][2] = {6.327466, 9.578267e-4, -5.096097e-7,  8.635932e-10};
        slew2ndHG[1][3] = {5.827046, 1.638310e-3,  1.926675e-7, -4.908247e-10};

        slew2ndLG[0][0] = {0., 0., 0., 0.};
        slew2ndLG[0][1] = {6.961994, -1.224803e-4,  3.792120e-7, -8.476456e-11};
        slew2ndLG[0][2] = {0.518704,  1.387013e-2, -9.458857e-6,  2.191948e-9 };
        slew2ndLG[0][3] = {1.909640,  1.250902e-2, -1.041146e-5,  3.010049e-9 };

        slew2ndLG[1][0] = {6.610375, 8.510258e-4, -1.199924e-7,  5.905106e-12};
        slew2ndLG[1][1] = {6.590451, 9.164059e-4, -1.626631e-7,  1.059131e-11};
        slew2ndLG[1][2] = {6.930768, 7.651640e-4, -1.401897e-7,  9.679413e-12};
        slew2ndLG[1][3] = {6.594080, 1.096790e-3, -3.630054e-7,  4.884000e-11};

        ana->SetSlewingCoeff(slewingParamsHG, slewingParamsLG, slew2ndHG, slew2ndLG);
    }

    ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutHG = {15, 15, 15, 15, 15, 15, 15, 15};
    ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutLG = {15, 15, 15, 15, 15, 15, 15, 15};   // increased from 10 to 15 then to 25
    // ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowHG  = { -6, -5, -5, -5, -5, -5, -5, -5};
    // ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighHG = { 8,  8,  8, 11,  8, 10,  8, 12};
    // ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowLG  = { -6, -5, -5, -5, -5, -5, -5, -5};
    // ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighLG = { 8,  8,  8, 11,  8, 10,  8, 12};

    // ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowHG  = { -15, -15, -15, -15, -15, -15, -15, -15};
    // ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighHG = {  15,  15,  15,  15,  15,  15,  15,  15};
    // ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowLG  = { -15, -15, -15, -15, -15, -15, -15, -15};
    // ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighLG = {  15,  15,  15,  15,  15,  15,  15,  15};

    // ----------------------
    // current one
    ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowHG, DeltaT0CutHighHG, DeltaT0CutLowLG, DeltaT0CutHighLG;
    if (year == 1111) {
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
        DeltaT0CutLowHG  = { -10, -10, -10, -10, -10, -10, -10, -10};
        DeltaT0CutHighHG = {  10,  10,  10,  10,  10,  10,  10,  10};
        DeltaT0CutLowLG  = { -10, -10, -10, -10, -10, -10, -10, -10};
        DeltaT0CutHighLG = {  10,  10,  10,  10,  10,  10,  10,  10};
    }

    ana->SetCutValues(chisqDivAmpCutHG, chisqDivAmpCutLG, DeltaT0CutLowHG, DeltaT0CutHighHG, DeltaT0CutLowLG, DeltaT0CutHighLG);

    //  if (eCalib || t0Calib) LoadCalibrations(ana, calibFile, runNumber, eCalib, t0Calib);

    return ana;
}

TChain *inputFiles(int year)
{
    TChain *chain = new TChain("zdcTree");

    if (year == 1111) {
        // chain->Add("ZDC_10kNeutrons_Merged_157TeV_80MHz.root");
        chain->Add("ZDC_10kNeutrons_Merged_251TeV_80MHz.root");
    }
    else if (year == 2016) {
        chain->Add("data16_hip8TeV_00313187_87z4/user.steinber.17047360._000001.ANALYSIS.root");
        chain->Add("data16_hip8TeV_00313187_87z4/user.steinber.17047360._000002.ANALYSIS.root");
    }
    else if (year == 2018) {
        chain->Add("data2018/data18_hi.00367365.calibration_zdcCalib.merge.AOD.c1319_m2048.89_ANALYSIS.root");
        chain->Add("data2018/user.steinber.18605473._000006.ANALYSIS.root");
    }
    else throw;

    // chain->Add("debug.root");

    return chain;
}

void RunZDCAnalysis(std::string outputFile, int nevent = -1, int year = 2016,
                    int nsamples = 7, bool freq80Mhz = false, bool useDelayed = false, bool fixTau1 = true, bool fixTau2 = true, bool doSlew = false, float FitTMax = 162.5)
{
    TStopwatch timer;
    timer.Start();
    TChain *chain = inputFiles(year);

    ZDCTreeAnalysis* ana = InitZDCAnalysis(chain, year, nsamples, freq80Mhz, useDelayed, fixTau1, fixTau2, doSlew);

    ana->SetFitTimeMax(FitTMax);

    ana->OpenOutputTree(outputFile);
    ana->Dump_setting();
    ana->Loop(nevent);
    ana->Dump_setting();
    ana->CloseOutputTree();

    timer.Stop();
    timer.Print();
}
