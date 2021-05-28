//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLNOISEMASKINGTOOL_H
#define LARCELLREC_LARCELLNOISEMASKINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "LArRecConditions/LArBadChannelMask.h"
#include "LArRecConditions/LArBadChannelCont.h"

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
  Gaudi::Property<std::vector<std::string> > m_problemsToMask{this,"ProblemsToMask",{},
      "Bad-Channel categories to mask entirly"}; 
  Gaudi::Property<std::vector<std::string> > m_sporadicProblemsToMask{this,"SporadicProblemsToMask",{},
      "Bad-Channel categories to mask in case of sporadic noise"}; 
  SG::ReadCondHandleKey<LArBadChannelCont> m_bcContKey {this, "BadChanKey", "LArBadChannel", 
      "SG key for LArBadChan object"};
  
  
  LArBadChannelMask m_noiseMask;
  LArBadChannelMask m_sporadicNoiseMask;
  
  int m_qualityCut;
  std::vector<int> m_caloNums ;
  bool m_maskNoise;
  bool m_maskSporadic;
};

#endif

