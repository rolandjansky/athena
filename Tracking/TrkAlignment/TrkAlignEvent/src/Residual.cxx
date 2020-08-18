/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignEvent/Residual.h"

namespace Trk {

  Residual::Residual(AlignResidualType resType, AlignMesType mesType, ParamDefs param, double residual, double errSq) 
    : m_resType(resType)
    , m_mesType(mesType)
    , m_param(param)
    , m_residual(residual)
    , m_errSq(errSq)
    , m_residualNorm(residual/std::sqrt(errSq)) 
  {
    
  }
}
