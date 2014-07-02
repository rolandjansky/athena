/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_JETCONVERTERBASE_H
#define JETEVENTTPCNV_JETCONVERTERBASE_H

#include <cmath>

namespace JetConverterTypes
{
  typedef   std::vector<unsigned short> signalState_pers_t;
  
  struct momentum
  {
    typedef float dtype;
    momentum() : m_px(0), m_py(0), m_pz(0), m_m(0) { };
    momentum(dtype x, dtype y, dtype z, dtype mm) : m_px(x), m_py(y), m_pz(z), m_m(mm) { };
    dtype m_px, m_py, m_pz, m_m;
    
    dtype eta() const 
    { 
      dtype p = std::sqrt( m_px * m_px + m_py * m_py + m_pz * m_pz );
      return 0.5*std::log( ( p + m_pz ) / ( p - m_pz ) );
    };
  };
}


template<class PERS>
class JetConverterBase
{ };

#endif
