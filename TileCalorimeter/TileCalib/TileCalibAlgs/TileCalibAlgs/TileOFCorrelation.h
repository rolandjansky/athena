/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include <vector>
#include <string>

#define NPARTITIONS 4
#define NDIG 9

class TileHWID;

class TileOFCorrelation
{
 public:

  TileOFCorrelation();
  ~TileOFCorrelation(); 

  void SetCorrelationZero(MsgStream & log, int dignum);
  void SetCorrelationDelta(MsgStream & log, int dignum);

  void Sum(std::vector<float> &digits, int ros, int drawer, int channel, int gain,
           MsgStream & log, bool debug, int &dignum, bool doRobustCov);

  void RunningCorrelation(std::vector<float> &digits, int ros, int drawer, int channel, int gain,
			  MsgStream & log, bool debug, int &dignum, int chthres);

  void CalcCorrelation(MsgStream & log, int dignum, bool flag_7to9, bool doRobustCov);

  void CalcRunningCorrelation(MsgStream & log, int dignum, int chthres, bool flag_7to9);

  void GetCorrelation(int dignum, float tmpCorr[][9], int ros, int drawer, int chan, int gain);

  void PrintCorrelation(int dignum);

  void SaveCorrelationSumm(bool deltaCorrelation, std::string OptFilterFile_CorrelationSumm, 
                           const TileHWID *tileHWID, MsgStream & log, int dignum);

  void SaveCorrelationMatrix(bool deltaCorrelation, std::string OptFilterFile_CorrelationMatrix,
                             const TileHWID *tileHWID, MsgStream & log, int dignum);

  void CalcWeights(bool of2,
		   bool deltaCorrelation,
		   const std::vector<double>& LshapeForm,
                   const std::vector<double>& HshapeForm,
		   const std::vector<double>& LdshapeForm,
                   const std::vector<double>& HdshapeForm,
		   MsgStream & log, int ros, int drawer, int channel, int gain, int dignum);

  void BuildPulseShape(std::vector<double> &pulseShape, std::vector<double> &pulseShapeX, 
                       std::vector<double> &pulseShapeT, int dignum, MsgStream &log);

  void WriteWeightsToFile(bool deltaCorrelation, int dignum, std::string m_OptFilterFile_ai_lo,
			  std::string m_OptFilterFile_bi_lo, std::string m_OptFilterFile_ai_hi,
			  std::string m_OptFilterFile_bi_hi, int ros, int drawer, int channel, int gain,
			  const TileHWID *tileHWID, MsgStream & log);

  void CloseWeightsFile(std::string OptFilterFile_ai_lo, std::string OptFilterFile_bi_lo,
                        std::string OptFilterFile_ai_hi, std::string OptFilterFile_bi_hi, 
			MsgStream & log);

  void OpenWeightsFile(std::string OptFilterFile_ai_lo, std::string OptFilterFile_bi_lo,
                       std::string OptFilterFile_ai_hi, std::string OptFilterFile_bi_hi, 
                       MsgStream & log);

  float* getCorrelationSumm(bool deltaCorrelation, int ros, int drawer, int channel, int gain, int dignum);

  double w_a[201][9];
  double w_b[201][9];
  double w_c[201][9];
  double g[201][9];
  
  
 private:

  using Tile = TileCalibUtils;
  //double SS(9,9);
  double (*m_SS)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG][NDIG];
  double (*m_S)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG];
  double (*m_R)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG][NDIG];
  //int N_events[4][64][48][2]; 
  std::vector< std::vector <float> > m_DataVector[NPARTITIONS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]; 

  double m_corr2[NDIG];
  double  (*m_corr_sum)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG];
  double  (*m_corr_sum_sq)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG];
  double  m_ncorr;
  float  m_corr[8];
  int (*m_N)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN];
  int  m_jentry, m_lag;
  int (*m_N_pairs)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG];
  double m_N_d;
  double (*m_S1)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG];
  double (*m_S2)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG];
  double (*m_S11)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG];
  double (*m_S12)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG];
  double  (*m_S22)[Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NDIG];

};

#endif
