/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Template_h
#define Template_h

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

#include "TSystem.h"
#include "TStopwatch.h"
#include "TMath.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

#include "RooGlobalFunc.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooCBShape.h"
#include "RooGaussian.h"
#include "RooChebychev.h"
#include "RooExponential.h"
#include "RooAddPdf.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "RooWorkspace.h"

#include "Math/ProbFunc.h"

#include "MuonCalibrationFit/Tools.h"

class Template {
private:
  TString               m_Name;
  TString               m_Title;
  TString               m_Bkg;
  int                   m_Splitting;
  std::vector< TH1F* >* m_VectorHistograms;
  //::: 
  float* m_MassJpsiContentBuffer_Data;
  float* m_MassZContentBuffer_Data;
  float* m_RhoZContentBuffer_Data;
  //:::
  float* m_MassJpsiContentBuffer_MonteCarlo;
  float* m_MassZContentBuffer_MonteCarlo;
  float* m_RhoZContentBuffer_MonteCarlo;
  //:::
  float* m_MassJpsiErrorBuffer_MonteCarlo;
  float* m_MassZErrorBuffer_MonteCarlo;
  float* m_RhoZErrorBuffer_MonteCarlo;
  //:::
  float* m_MassJpsiAddWeightBuffer_MonteCarlo;
  float* m_MassZAddWeightBuffer_MonteCarlo;
  float* m_RhoZAddWeightBuffer_MonteCarlo;
  //:::
  float* m_MassJpsiContentBuffer_Background;
  float* m_MassZContentBuffer_Background;
  float* m_RhoZContentBuffer_Background;
  //:::
  float* m_MassJpsiErrorBuffer_Background;
  float* m_MassZErrorBuffer_Background;
  float* m_RhoZErrorBuffer_Background;
  //:::
  std::vector< Tools::FitInfo* >* m_FitInfo[ Type::N ];
  //:::
  Tools::BoundsInfo* m_MassJpsi;
  Tools::BoundsInfo* m_MassZ;
  Tools::BoundsInfo* m_RhoZ;
  Tools::BoundsInfo* m_EtaZ;
  Tools::BoundsInfo* m_PtZ;
  //:::
  Tools::FloatBoundsInfo* m_MassJpsiPtBins;
  Tools::FloatBoundsInfo* m_MassZPtBins;
  Tools::FloatBoundsInfo* m_RhoZPtBins;
  //:::
  TH1F* m_Weights;
  //:::
  bool  m_SetGlobalWeights;
  bool  m_SetAverageWeight;
  bool  m_EvaluatedBackground;
  float m_AverageWeight;
  //:::
  bool m_EmptyMonitoredPlots;
  std::map< std::string, Tools::MonitoredVariable > m_MonVars;

public:
  Template( TString name, TString title, int splitting, std::string bkg );
  ~Template();
  //:::
  void Add( int nBins, float xMin, float xMax, std::vector< float >* ptBins, Tools::BoundsInfo* &dummyBin, Tools::FloatBoundsInfo* &dummyBinPt, float* &data_contentBuffer, float* &mc_contentBuffer, float* &mc_errorBuffer, float* &mc_addweightBuffer, float* &bkg_contentBuffer, float* &bkg_errorBuffer );
  void AddMassJpsi( int nBins, float xMin, float xMax, std::vector< float >* ptBins ); 
  void AddMassZ( int nBins, float xMin, float xMax, std::vector< float >* ptBins ); 
  void AddRhoZ( int nBins, float xMin, float xMax, std::vector< float >* ptBins ); 
  //:::
  void AddMonitoredVariable( std::string name, std::string title, std::string axistitle, int n_bins, float min, float max ) { AddMonitoredVariable( name, title, axistitle, n_bins, min, max, n_bins, min, max ); } 
  void AddMonitoredVariable( std::string name, std::string title, std::string axistitle, int jpsi_n_bins, float jpsi_min, float jpsi_max, int z_n_bins, float z_min, float z_max ); 
  //:::
  void Clear();
  void ClearData();
  void ClearMonteCarlo();
  void ClearBackground();
  void ClearVector( float* Vec, int n );
  void Delete( float* Vec );
  void SetAverageWeight();
  void SetGlobalWeights();
  void EvaluateBackground();
  //:::
  void FillMass( int type, float mass, float pt, float weight = 1. );
  void FillRho(  int type, float rho,  float pt, float weight = 1. );
  //:::
  void FillMonitoredVariable( int type, std::string name, float var, float pt, float mass, bool os, float weight, bool inRoF ); 
  std::vector< std::string > GetMonitoredVariables() { std::vector< std::string > res; for( std::map< std::string, Tools::MonitoredVariable >::iterator it = m_MonVars.begin(); it != m_MonVars.end(); ++it ) res.push_back( it->first ); return res; } 
  void MonitoredPlotsAreEmpty( bool val ) { m_EmptyMonitoredPlots = val; }
  //:::
  void Write( TString PreFix = "" );
  void Print();
  void Print( float* Vec, int n );
  //:::
  double GetLogLikelihood( double nRandom );
  double GetChiSquare( double nRandom );
  double GetDegreesOfFreedom();
  //:::
  std::vector< TH1F* >* GetAllHistograms( TString PreFix = "", bool recalculate = false );
  //:::
  TString GetName() { return m_Name; }
};

#endif
