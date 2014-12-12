/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LArCellNoiseMaskingTool_H
#define LARCELLREC_LArCellNoiseMaskingTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "LArRecConditions/ILArBadChannelMasker.h"

class LArCellNoiseMaskingTool: public AthAlgTool,
  virtual public ICaloCellMakerTool 
{
public:    
  LArCellNoiseMaskingTool(const std::string& type, 
                          const std::string& name, 
                          const IInterface* parent) ;


  virtual StatusCode initialize() ; 

  // update theCellContainer
  virtual StatusCode process( CaloCellContainer * theCellContainer) ;


 private:
  ToolHandle<ILArBadChannelMasker> m_maskingTool;
  ToolHandle<ILArBadChannelMasker> m_maskingSporadicTool;
  int m_qualityCut;
  std::vector<int> m_caloNums ;
  bool m_maskNoise;
  bool m_maskSporadic;
};

#endif

