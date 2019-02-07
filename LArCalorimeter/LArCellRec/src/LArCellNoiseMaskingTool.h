/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLNOISEMASKINGTOOL_H
#define LARCELLREC_LARCELLNOISEMASKINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "LArRecConditions/ILArBadChannelMasker.h"

class LArCellNoiseMaskingTool
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:    
  LArCellNoiseMaskingTool(const std::string& type, 
                          const std::string& name, 
                          const IInterface* parent) ;


  virtual StatusCode initialize() override;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer * theCellContainer,
                              const EventContext& ctx) const override;


 private:
  ToolHandle<ILArBadChannelMasker> m_maskingTool;
  ToolHandle<ILArBadChannelMasker> m_maskingSporadicTool;
  int m_qualityCut;
  std::vector<int> m_caloNums ;
  bool m_maskNoise;
  bool m_maskSporadic;
};

#endif

