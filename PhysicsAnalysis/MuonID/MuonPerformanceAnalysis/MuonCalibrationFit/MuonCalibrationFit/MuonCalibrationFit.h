/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibrationFit_h
#define MuonCalibrationFit_h

#include <array>
#include <iostream>
#include <iomanip>
#include <random>
#include <fstream>
#include <sstream> 
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <strings.h>
#include <unistd.h>
#include <cstdlib>
#include <getopt.h>
#include <cmath>

#include <TChain.h>
#include <TF1.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTreeCache.h>
#include <TStyle.h>
#include <TColor.h>
#include <TError.h>
#include <TRandom2.h>
#include <TRandom3.h>
#include <TVectorT.h>
#include <TStopwatch.h>

#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>

#include <AthenaBaseComps/AthAlgorithm.h>
#include <GaudiKernel/MsgStream.h>

#include <boost/range/irange.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include "MuonCalibrationFit/Regions.h"
#include "MuonCalibrationFit/Reader.h"
#include "MuonCalibrationFit/Template.h"

class MuonCalibrationFit: public AthAlgorithm {

public:
  MuonCalibrationFit( const std::string& name, ISvcLocator* pSvcLocator );
  //:::
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  //:::
  std::vector< float >* GetVector( TString all_values ); 
  double GetFunction( const double* par ) { if( m_DoNLL ) return GetNegLogLikelihood( par ); else if( m_DoChi2  ) return GetChiSquare( par ); else return -666.; }
  double GetNegLogLikelihood( const double *par );
  double GetChiSquare( const double *par );
  // float GetTruth( float eta, int par ); 
  //:::
  void SetParameters( ROOT::Math::Minimizer* Minim, bool forScan = false );
  void ResetParameters();
  void ActivateParameters( std::string active_pars );
  void UpdateTemplates( const double* par );
  void UpdateGlobalValues( double min, const double* pars, const double* errs_up, const double* errs_low );
  void UpdateGlobalValues( double min, const double* pars, const double* errs );
  void FoldParameters( const double* fit_pars, const double* fit_errs, double* pars, double* errs );
  void DoMonitoringPlots( const double* par );
  void SaveParameters( TString name, const double* pars, const double* errs_up, const double* errs_low );
  void SaveParameters( TString name, const double* pars, const double* errs );
  void CopyToBestFitParameters();
  void SaveTime( TString name, float time );
  bool RunMinimizationStep( std::string what, int times );
  void Stop( Tools::RegionInfo* fitRegion );
  void FillVectors( std::vector< Tools::RegionInfo* > RegInfos, std::vector< Tools::RegionInfo* > RegInfosForID, Tools::RegionInfo* RegInfo, bool can_correct_all );
  void GetMemoryUsage();

private:
  bool     g_FilledData;
  TFile*   g_Output;
  TTree*   g_Tree;
  //::: Running parameters
  double   g_pars[ Par::N ];
  double   g_errs_up[ Par::N ];
  double   g_errs_low[ Par::N ];
  double   g_NLL; 
  double   g_Chi2; 
  //::: Best (so far) parameters
  double   g_best_pars[ Par::N ];
  double   g_best_errs_up[ Par::N ];
  double   g_best_errs_low[ Par::N ];
  double   g_best_NLL; 
  double   g_best_Chi2; 
  //:::
  double   g_GlobalMinimum; 
  int      g_Index;
  int      g_ScanPar;
  int      g_ScanIter;
  int      g_FitTrial;
  int      g_UpdatedTemplates; //::: How many times have we recaclulated the templates?
  bool     g_DumpFlag;
  TRandom3 g_Random;
  //:::
  std::vector< Tools::Info >* g_DataSingleInfo;
  std::vector< Tools::Info >* g_DataBothInfo;
  std::vector< Tools::Info >* g_BackgroundSingleInfo;
  std::vector< Tools::Info >* g_BackgroundBothInfo;
  std::vector< Tools::Info >* g_MonteCarloSingleInfo;
  std::vector< Tools::Info >* g_MonteCarloBothInfo; 
  //:::
  Template* g_CurrentTemplate; 
  //:::
  std::string m_Detector;
  std::string m_FitType;
  std::string m_Bkg;
  std::string m_RunMode;
  int         m_MaxRetrials;
  float       m_ProbThreshold;
  std::string m_GRL_Name;
  std::string m_Input;
  std::string m_InputForID;
  std::string m_Output;
  std::string m_DataFile;
  std::string m_BackgroundFile;
  std::string m_MonteCarloFile;
  std::string m_InputRegionsFile;
  std::string m_InputRegionsFileForID;
  std::string m_OutputRegionsFile;
  std::string m_JpsiReweightingFile;
  std::string m_ZReweightingFile;
  bool        m_BatchMode;
  bool        m_MonitoringPlots;
  bool        m_RequireErrors;
  bool        m_DoNLL;
  bool        m_DoChi2;
  double      m_Error;
  int         m_MaxEvents;
  bool        m_Use[ Par::N ];
  bool        m_UseLocally[ Par::N ];
  float       m_Prev[ Par::N ];
  float       m_Init[ Par::N ], m_MinInit[ Par::N ], m_MaxInit[ Par::N ];
  float       m_Step[ Par::N ], m_MinStep[ Par::N ], m_MaxStep[ Par::N ];
  bool        m_PosDef[ Par::N ];
  std::string m_ParNames[ Par::N ];
  int         m_Splitting;
  float       m_NumRandomValues;
  float       m_MinMuonsPt;
  float       m_MinMuonsPtForJpsi;
  float       m_MaxMuonsPtForJpsi;
  float       m_MinLeadingMuonPtForZ;
  float       m_MinSubLeadingMuonPtForZ;
  int         m_JpsiMass_Bins;
  float       m_JpsiMass_Min;
  float       m_JpsiMass_Max;
  std::string m_JpsiMass_Pts;
  int         m_ZMass_Bins;
  float       m_ZMass_Min;
  float       m_ZMass_Max;
  std::string m_ZMass_Pts;
  int         m_ZRho_Bins;
  float       m_ZRho_Min;
  float       m_ZRho_Max;
  std::string m_ZRho_Pts;
  //:::
};

#endif
