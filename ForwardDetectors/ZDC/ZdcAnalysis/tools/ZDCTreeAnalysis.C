/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define ZDCTreeAnalysis_cxx
#include "ZDCTreeAnalysis.h"

#include <algorithm>
#include <cmath>

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TH1.h>
#include <TF1.h>
#include <TLatex.h>
#include <TPaveStats.h>

template <typename T> T Sqr(const T& inT) {return inT*inT;}

void ZDCTreeAnalysis::InitInternal() 
{
  for (int iside = 0; iside < 2; iside++) {
    for (int imod = 0; imod < 4; imod++) {
      _fixTau1[iside][imod] = false;
      _fixTau2[iside][imod] = false;
      _moduleTau1[iside][imod] = 4;
      _moduleTau2[iside][imod] = 20;

      _chisqDivAmpCutHG[iside][imod] = 1e6;
      _chisqDivAmpCutLG[iside][imod] = 1e6;
      _DeltaT0CutLow[iside][imod] = -100;
      _DeltaT0CutHigh[iside][imod] = 100;
      _HGOverFlowADC[iside][imod] = 800;
      _HGUnderFlowADC[iside][imod] = 25;

      for (size_t ipar = 0; ipar < 3; ipar++) {
	_T0SlewCoeffHG[iside][imod][ipar] = 0;
	_T0SlewCoeffLG[iside][imod][ipar] = 0;
      }

      _modECalib[iside][imod] = 1;
      _haveLBDepECalib[iside][imod] = false;
      _haveLBDepT0[iside][imod] = false;
    }
  }

  zdc_samplesSub = new std::vector<float>(7, 0);
  zdc_samplesDeriv = new std::vector<float>(7, 0);
  zdc_samplesDeriv2nd = new std::vector<float>(7, 0);
}


void ZDCTreeAnalysis::SetupBunchTrains()
{
  GetEntry(0);

  _BCIDGap.assign(3300, 0);
  _BCIDPosInTrain.assign(3300, 0);

  std::cout << "Run = " << runNumber << std::endl;

  if (runNumber >= 287706) {
  
    int trainStartBCIDS[21] = {5, 155, 305, 446, 596, 746, 896, 1046, 1196, 1487, 1637, 1787, 1937, 2087, 2228, 2378, 2528, 2678, 2828, 2978, 3119}; 
    int trainNumBCIDS[21] = {24, 24, 20, 24, 24, 24, 24, 24, 20, 24, 24, 24, 24, 22, 24, 24, 24, 24, 24, 22, 24};

    int lastBCID = -1;
    int numBCID = 0;

    for (int train = 0; train < 21; train++) {
      int bcid = trainStartBCIDS[train];

      std::cout << "Train " << train << ", bcids = ";

      for (int PIT = 0; PIT < trainNumBCIDS[train]; PIT++) {
	int gapNext = PIT % 2 == 0 ? 4 : 6;

	_BCIDGap[bcid] = bcid - lastBCID;
	_BCIDPosInTrain[bcid] = PIT;

	std::cout << bcid << ", ";

	numBCID++;
	lastBCID = bcid;
	bcid += gapNext;
      }

      std::cout << std::endl;
    }
    
    std::cout << "Found " << numBCID << "bcids" << std::endl;
  }
}

void ZDCTreeAnalysis::OpenOutputTree(std::string file)
{
  TFile* outputFile = new TFile(file.c_str(), "recreate");
  if (!outputFile->IsOpen()) {
    std::cout << "Error opening output file " << file << ", quitting" << std::endl;
    return;
  }

  _outTFile = outputFile;

  _outTree = fChain->CloneTree(0);
  _outTree->Branch("bcidGap", &bcidGap, "bcidGap/I");
  _outTree->Branch("bcidPosInTrain", &bcidPosInTrain, "bcidPosInTrain/I");

  _outTree->Branch("zdc_SumAmp", zdc_SumAmp, "zdc_SumAmp[2]/F");
  _outTree->Branch("zdc_SumCalibAmp", zdc_SumCalibAmp, "zdc_SumCalibAmp[2]/F");
  _outTree->Branch("zdc_AvgTime", zdc_AvgTime, "zdc_AvgTime[2]/F");
  _outTree->Branch("zdc_ModuleMask", &zdc_ModuleMask, "zdc_ModuleMask/I");

  _outTree->Branch("zdc_Status", zdc_Status, "zdc_Status[2][4]/I");
  _outTree->Branch("zdc_Amp", zdc_Amp, "zdc_Amp[2][4]/F");
  _outTree->Branch("zdc_FitT0", zdc_FitT0, "zdc_FitT0[2][4]/F");
  _outTree->Branch("zdc_T0Corr", zdc_T0Corr, "zdc_T0Corr[2][4]/F");
  _outTree->Branch("zdc_FitChisq", zdc_FitChisq, "zdc_FitChisq[2][4]/F");

  _outTree->Branch("zdc_CalibAmp", zdc_CalibAmp, "zdc_CalibAmp[2][4]/F");
  _outTree->Branch("zdc_CalibTime", zdc_CalibTime, "zdc_CalibTime[2][4]/F");

  _outTree->Branch("zdc_MaxADC", zdc_MaxADC, "zdc_MaxADC[2][4]/f");
  _outTree->Branch("zdc_MinADC", zdc_MinADC, "zdc_MinADC[2][4]/f");
  _outTree->Branch("zdc_MaxADCSample", zdc_MaxADCSample, "zdc_MaxADCSample[2][4]/I");
  _outTree->Branch("zdc_MinADCSample", zdc_MaxADCSample, "zdc_MinADCSample[2][4]/I");
  _outTree->Branch("zdc_Min2ndDeriv", zdc_Min2ndDeriv, "zdc_Min2ndDeriv[2][4]/F");
  _outTree->Branch("zdc_Min2ndDerivSample", zdc_Min2ndDerivSample, "zdc_Min2ndDerivSample[2][4]/I");

  _outTree->Branch("zdc_samplesSub", &zdc_samplesSub);
  _outTree->Branch("zdc_samplesDeriv", &zdc_samplesDeriv);
  _outTree->Branch("zdc_samplesDeriv2nd", &zdc_samplesDeriv2nd);

  _doOutput = true;
}

void ZDCTreeAnalysis::CloseOutputTree()
{
  if (!_doOutput) return;

  _outTree->AutoSave();
  delete _outTFile;
  _outTFile = 0;
  _outTree = 0;
  _doOutput = false;
}

void ZDCTreeAnalysis::Loop(int numEntries, int startEntry)
{
   if (fChain == 0) return;

   int loopStart = (startEntry == -1 ? _currentEntry : startEntry);

   Long64_t nentries = fChain->GetEntriesFast() - loopStart;
   
   Long64_t numLoopEntry = (numEntries != -1 ? numEntries : nentries);

   Long64_t nbytes = 0, nb = 0;

   _inLoop = true;

   for (Long64_t jentry = loopStart; jentry < loopStart + numLoopEntry; jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);
      _currentEntry = jentry;
      nbytes += nb;

      DoAnalysis();

      if (jentry > 1) {
	float log10Entry = std::floor(std::log ((float) jentry)/std::log(10));
	int modEntry = jentry % (int) pow(10, log10Entry);
	
	if (modEntry == 0) {
	  std::cout << "Processed event " << jentry << std::endl;
	}
      }
   }

   _inLoop = false;
}

void ZDCTreeAnalysis::DoAnalysis()
{
  // Starting a new event
  //
  _dataAnalyzer_p->StartEvent(lumiBlock);

  bcidGap = _BCIDGap[bcid];
  bcidPosInTrain = _BCIDPosInTrain[bcid];

  if (_doOutput) {
    // //
    // //  Initialize outout arrays
    // //
  }

  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {

      static std::vector<float> HGADCSamples(7);
      static std::vector<float> LGADCSamples(7);

      // Copy data into a proper vector of floats
      //
      if (module == 0) {
	std::copy(&zdc_raw[side][module][0][1][0], &zdc_raw[side][module][0][1][7],  LGADCSamples.begin());
	std::copy(&zdc_raw[side][module][1][1][0], &zdc_raw[side][module][1][1][7],  HGADCSamples.begin());
      }
      else {
	std::copy(&zdc_raw[side][module][0][0][0], &zdc_raw[side][module][0][0][7],  LGADCSamples.begin());
	std::copy(&zdc_raw[side][module][1][0][0], &zdc_raw[side][module][1][0][7],  HGADCSamples.begin());
      }

      // Load the data
      //
      _dataAnalyzer_p->LoadAndAnalyzeData(side, module, HGADCSamples, LGADCSamples);
    }
  }


  // We're done
  //
  _dataAnalyzer_p->FinishEvent();

  zdc_ModuleMask = _dataAnalyzer_p->GetModuleMask();

  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {

      const ZDCPulseAnalyzer* pulseAna_p = _dataAnalyzer_p->GetPulseAnalyzer(side, module);

      *zdc_samplesSub = pulseAna_p->GetSamplesSub();
      *zdc_samplesDeriv = pulseAna_p->GetSamplesDeriv();
      *zdc_samplesDeriv2nd = pulseAna_p->GetSamplesDeriv2nd();

      zdc_Min2ndDeriv[side][module] = pulseAna_p->GetMinDeriv2nd();
      zdc_Min2ndDerivSample[side][module] = pulseAna_p->GetMinDeriv2ndIndex();

      zdc_MaxADC[side][module] = pulseAna_p->GetMaxADC();
      zdc_MinADC[side][module] = pulseAna_p->GetMinADC();

      zdc_MaxADCSample[side][module] = pulseAna_p->GetMaxADCSample();
      zdc_MinADCSample[side][module] = pulseAna_p->GetMinADCSample();

      zdc_Status[side][module] = pulseAna_p->GetStatusMask();
      zdc_Amp[side][module] = pulseAna_p->GetAmplitude();
      zdc_FitT0[side][module] = pulseAna_p->GetFitT0();
      zdc_T0Corr[side][module] = pulseAna_p->GetT0Corr();
      zdc_FitChisq[side][module] = pulseAna_p->GetChisq();

      zdc_CalibAmp[side][module] = _dataAnalyzer_p->GetModuleCalibAmplitude(side, module);
      zdc_CalibTime[side][module] = _dataAnalyzer_p->GetModuleCalibTime(side, module);
    }

    zdc_SumAmp[side] = _dataAnalyzer_p->GetModuleSum(side);
    zdc_SumCalibAmp[side] = _dataAnalyzer_p->GetCalibModuleSum(side);
    zdc_AvgTime[side] = _dataAnalyzer_p->GetAverageTime(side);
    zdc_sideFailed[side] = _dataAnalyzer_p->SideFailed(side);    

    
  }


  if (_doOutput) {
    _outTree->Fill();
  }
}


void ZDCTreeAnalysis::PlotFits(std::string canvasSavePath)
{
  static bool first = true;
  static std::array<TCanvas*, 2> plotCanvases;
  static TLatex* statusLabel;
  static TLatex* sideModuleLabel;
  static TLatex* highLowGainLabel;
  static TLatex* eventLabel;
  static TLatex* warningLabel;

  if (first) {
    plotCanvases[0] = new TCanvas("ZDCFitsSide0","ZDCFitsSide0", 1000, 800);
    plotCanvases[1] = new TCanvas("ZDCFitsSide1","ZDCFitsSide1", 1000, 800);

    plotCanvases[0]->Divide(2, 2, 0.001, 0.0001);
    plotCanvases[1]->Divide(2, 2, 0.001, 0.0001);

    first = false;

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
  }

  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      TVirtualPad* pad_p = plotCanvases[side]->cd(module + 1);

      pad_p->SetTopMargin(0.03);
      pad_p->SetRightMargin(0.03);

      const ZDCPulseAnalyzer* pulseAna_p = _dataAnalyzer_p->GetPulseAnalyzer(side, module);

      const TH1* hist_p = pulseAna_p->GetHistogramPtr();
      TH1* cloneHist_p = (TH1*) hist_p->Clone((std::string(hist_p->GetName()) + "_clone").c_str());
      
      //  Set the maximum to make sure we include the peak of the function
      //
      TF1* func_p = (TF1*) cloneHist_p->GetListOfFunctions()->First();
      double funcMax = func_p->GetMaximum();

      if (pulseAna_p->HavePulse()) {
	if (cloneHist_p->GetMaximum() < funcMax*1.1) cloneHist_p->SetMaximum(funcMax*1.1);
	if (cloneHist_p->GetMaximum() < 20) cloneHist_p->SetMaximum(20);
      }
      else {
	float maxADC = pulseAna_p->GetMaxADC();
	float minADC = pulseAna_p->GetMinADC();

	cloneHist_p->SetMaximum(maxADC*1.1);
	cloneHist_p->SetMinimum(std::min(0.0,minADC*1.1));
      }

      if (pulseAna_p->HavePulse()) cloneHist_p->Draw();
      else cloneHist_p->Draw("hist");

      gPad->Modified();
      gPad->Update();

      TPaveStats* stats_p = (TPaveStats *)cloneHist_p->GetListOfFunctions()->FindObject("stats");
      if (stats_p) {
	stats_p->SetX1NDC(0.135);
	stats_p->SetX2NDC(0.4);
	stats_p->SetFitFormat("5.3f");
      }

      unsigned int status = pulseAna_p->GetStatusMask();

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
  }
}
