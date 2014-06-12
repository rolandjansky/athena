/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCELLCORRECTION_CALOCELLRANDOMIZER_H
#define CALOCELLCORRECTION_CALOCELLRANDOMIZER_H

#include "CaloUtils/CaloCellCorrection.h"

#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "GaudiKernel/ToolHandle.h"

//class CaloCell;
//class ICalorimeterNoiseTool;

class CaloCellRandomizer : public CaloCellCorrection

{

public:

  CaloCellRandomizer(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~CaloCellRandomizer() {};

  virtual StatusCode initialize();
  
  void MakeCorrection(CaloCell* theCell);


private:


  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;

  int m_corrSampleMin;
  int m_corrSampleMax;
  float m_fractionSigma;
  bool m_GaussRand;
  bool m_GaussRandGroupedSamples;
  bool m_GaussRandShifted;
  bool m_GaussRandShifted_Custom;
  float m_shift_EMB;
  float m_shift_EMEC;
  float m_shift_HEC;
  float m_shift_TileBar;
  float m_shift_TileGap;
  float m_shift_TileExt;
  float m_shift_FCAL;
 
};

#endif
