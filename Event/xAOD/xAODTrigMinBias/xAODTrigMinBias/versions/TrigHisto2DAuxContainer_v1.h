/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigHisto2DAUXCONTAINTER_V1_H
#define xAODTrigMinBias_TrigHisto2DAUXCONTAINTER_V1_H

#include "xAODCore/AuxContainerBase.h"

#include <vector>

namespace xAOD {
  
  class TrigHisto2DAuxContainer_v1 : public AuxContainerBase {
    
  public:
    TrigHisto2DAuxContainer_v1();
    
  private:
    //contents of trighisto
    std::vector<std::vector<float> > contents;
    
    std::vector<unsigned int>  nbinsX;
    std::vector<float> minX;
    std::vector<float> maxX;

    std::vector<unsigned int> nbinsY;
    std::vector<float> minY;
    std::vector<float> maxY;
  };
  
}

#include "xAODCore/BaseInfo.h"
SG_BASE(   xAOD::TrigHisto2DAuxContainer_v1, xAOD::AuxContainerBase );

#endif
