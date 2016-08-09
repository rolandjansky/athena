/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEOPTFILTERWEIGHTS_H
#define TILECONDITIONS_TILEOPTFILTERWEIGHTS_H

#include "GaudiKernel/MsgStream.h"

typedef struct 
{
  bool m_DeltaWeights;
  double a_phys_simp[2][9][201], b_phys_simp[2][9][201], c_phys_simp[2][9][201];
  double a_cis_simp[2][7][201], b_cis_simp[2][7][201], c_cis_simp[2][7][201];
  
} TileOptFilterWeightsStruct;


typedef struct 
{
  
  bool m_DeltaCorr;
  double corr_delta[7];
  //ros dra chn gain sample
  double corr_noise[4][64][48][2][7];
  
  
} TileOptFilterCorrelationStruct;


//typedef double TileOptFilterCorrelationStruct;

class  TileOptFilterWeights {
  
  friend class TileInfoLoader;
  //  friend class TileInfo;
  
 public:
 
  TileOptFilterWeights();
  ~TileOptFilterWeights();
  
  void loadCorrelation(MsgStream &log);
  void loadWeights(MsgStream &log);

  TileOptFilterWeightsStruct * TileOFWstruct() { return m_weights; }
  TileOptFilterCorrelationStruct * TileOFCorrstruct() { return m_correlation; }
  
  bool DeltaConf() { return m_DeltaConf; }  
  bool loaded()    { return m_loaded; }
  
 private:

  //variables
  bool m_DeltaConf;
  int  m_NSamples_Phys;
  std::string m_noiseCISSuffix;
  std::string m_noisePhysicsSuffix;
  std::string m_deltaCISSuffix;
  std::string m_deltaPhysicsSuffix;
  std::string m_noiseCorrSuffix;
  std::string m_deltaCorrSuffix;
 
  bool m_loaded;
  TileOptFilterWeightsStruct * m_weights;
  TileOptFilterCorrelationStruct * m_correlation; 
  
};

#endif // TILECONDITIONS_TILEOPTFILTERWEIGHTS_H
