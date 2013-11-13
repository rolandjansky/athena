/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SLTrueInfo_P1_H
#define JETTAGINFOTPCNV_SLTrueInfo_P1_H

#include "EventPrimitives/EventPrimitives.h"

namespace Analysis {

  class SLTrueInfo_p1 {
  public:

    int m_barcode;
    int m_pdgCode;
    int m_pdgCodeMother;
    bool m_isFromBhadron;
    bool m_isFromDhadron;
    bool m_isFromGHboson;
    
    
    Eigen::Vector3d m_Momentum;
    Eigen::Vector3d m_ProductionVertex;
  };

}
#endif
