/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************************************
// Filename : TileCorrelation.h
// Author   : C Cuenca
// Created  : May 2004

// DESCRIPTION
// Implementation of a class for correlation among digits calculation.

// ************************************************************************************

#include "GaudiKernel/MsgStream.h"

class TileHWID;

#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

class TileCorrelation
{
 public:
  TileCorrelation();
  ~TileCorrelation(); 
  void SetCorrelationZero(MsgStream & log, int dignum);
  void SetCorrelationDelta(MsgStream & log, int dignum);
  void Sum(vector<double> &digits, int ros, int drawer, int channel, int gain, MsgStream & log, bool m_debug, int &dignum);
  void RunningCorrelation(vector<double> &digits, int ros, int drawer, int channel, int gain,
			  MsgStream & log, bool m_debug, int &dignum, int chthres);
  void CalcCorrelation(MsgStream & log, int dignum);
  void CalcRunningCorrelation(MsgStream & log, int dignum, int chthres, bool m_7to9);
  void PrintCorrelation(int dignum);
  void SaveCorrelationSumm(bool m_deltaCorrelation, string m_OptFilterFile_CorrelationSumm, const TileHWID *m_tileHWID,
		       MsgStream & log, int dignum);
  void SaveCorrelationMatrix(bool m_deltaCorrelation, string m_OptFilterFile_CorrelationMatrix, const TileHWID *m_tileHWID,
		       MsgStream & log, int dignum);
  void CalcWeights(bool m_deltaCorrelation,
		   vector<double> m_LshapeForm, vector<double> m_HshapeForm,
		   vector<double> m_LdshapeForm, vector<double> m_HdshapeForm,
		   string m_OptFilterFile_ai_lo, string m_OptFilterFile_bi_lo,
		   string m_OptFilterFile_ai_hi, string m_OptFilterFile_bi_hi,
		   const TileHWID *m_tileHWID, MsgStream & log, int dignum);
  void BuildPulseShape(vector<double> &m_pulseShape, vector<double> &m_pulseShapeX, vector<double> &m_pulseShapeT,
		       int dignum, MsgStream &log);
  
 private:
  //double SS(9,9);
  double SS[4][64][48][2][9][9];
  double S[4][64][48][2][9];
  double R[4][64][48][2][9][9];
  //int N_events[4][64][48][2];  

  double corr[9], corr_sum[4][64][48][2][9], corr_sum_sq[4][64][48][2][9], ncorr;
  int N[4][64][48][2], jentry, lag, N_pairs[4][64][48][2][9];
  double N_d;
  double S1[4][64][48][2][9], S2[4][64][48][2][9], S11[4][64][48][2][9], S12[4][64][48][2][9], S22[4][64][48][2][9];
  


  //  const TileInfo *m_tileInfo;
};

//#ifndef TILERECALGS_TILELOF_H
//#define TILERECALGS_TILELOF_H
//#endif
