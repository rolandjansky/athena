/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************************************
// Filename : TileOFCorrelation.h
//  Author   : Valencia TileCal group, cristobal.cuenca@cern.ch,
//  Mantained by Ximo Poveda, jpoveda@cern.ch
// 
//  Created  : May, 2004
//  Moved to TileCalibAlgs on Dec'06
//
// DESCRIPTION
// Implementation of a class for correlation among digits calculation.

// ************************************************************************************
#ifndef TILECALIBALGS_TILELOFCORRELATION_H
#define TILECALIBALGS_TILELOFCORRELATION_H

#include "GaudiKernel/MsgStream.h"

#include <vector>
#include <string>

class TileHWID;

class TileOFCorrelation
{
 public:

  TileOFCorrelation();
  ~TileOFCorrelation(); 

  void SetCorrelationZero(MsgStream & log, int dignum);
  void SetCorrelationDelta(MsgStream & log, int dignum);

  void Sum(std::vector<float> &digits, int ros, int drawer, int channel, int gain,
           MsgStream & log, bool m_debug, int &dignum);

  void RunningCorrelation(std::vector<float> &digits, int ros, int drawer, int channel, int gain,
			  MsgStream & log, bool m_debug, int &dignum, int chthres);

  void CalcCorrelation(MsgStream & log, int dignum, bool m_7to9);

  void CalcRunningCorrelation(MsgStream & log, int dignum, int chthres, bool m_7to9);

  void GetCorrelation(int dignum, float tmpCorr[][9], int ros, int drawer, int chan, int gain);

  void PrintCorrelation(int dignum);

  void SaveCorrelationSumm(bool m_deltaCorrelation, std::string m_OptFilterFile_CorrelationSumm, 
                           const TileHWID *m_tileHWID, MsgStream & log, int dignum);

  void SaveCorrelationMatrix(bool m_deltaCorrelation, std::string m_OptFilterFile_CorrelationMatrix,
                             const TileHWID *m_tileHWID, MsgStream & log, int dignum);

  void CalcWeights(bool m_of2,
		   bool m_deltaCorrelation,
		   std::vector<double> m_LshapeForm, std::vector<double> m_HshapeForm,
		   std::vector<double> m_LdshapeForm, std::vector<double> m_HdshapeForm,
		   MsgStream & log, int ros, int drawer, int channel, int gain, int dignum);

  void BuildPulseShape(std::vector<double> &m_pulseShape, std::vector<double> &m_pulseShapeX, 
                       std::vector<double> &m_pulseShapeT, int dignum, MsgStream &log);

  void WriteWeightsToFile(bool m_deltaCorrelation, int dignum, std::string m_OptFilterFile_ai_lo,
			  std::string m_OptFilterFile_bi_lo, std::string m_OptFilterFile_ai_hi,
			  std::string m_OptFilterFile_bi_hi, int ros, int drawer, int channel, int gain,
			  const TileHWID *m_tileHWID, MsgStream & log);

  void CloseWeightsFile(std::string m_OptFilterFile_ai_lo, std::string m_OptFilterFile_bi_lo,
                        std::string m_OptFilterFile_ai_hi, std::string m_OptFilterFile_bi_hi, 
			MsgStream & log);

  void OpenWeightsFile(std::string m_OptFilterFile_ai_lo, std::string m_OptFilterFile_bi_lo,
                       std::string m_OptFilterFile_ai_hi, std::string m_OptFilterFile_bi_hi, 
                       MsgStream & log);

  float* getCorrelationSumm(bool m_deltaCorrelation, int ros, int drawer, int channel, int gain, int dignum);

  double w_a[201][9];
  double w_b[201][9];
  double w_c[201][9];
  double g[201][9];
  
  
 private:

  //double SS(9,9);
  double SS[4][64][48][2][9][9];
  double S[4][64][48][2][9];
  double R[4][64][48][2][9][9];
  //int N_events[4][64][48][2];  

  double corr[9], corr_sum[4][64][48][2][9], corr_sum_sq[4][64][48][2][9], ncorr;
  float  m_corr[8];
  int N[4][64][48][2], jentry, lag, N_pairs[4][64][48][2][9];
  double N_d;
  double S1[4][64][48][2][9], S2[4][64][48][2][9], S11[4][64][48][2][9], S12[4][64][48][2][9], S22[4][64][48][2][9];

};

#endif
