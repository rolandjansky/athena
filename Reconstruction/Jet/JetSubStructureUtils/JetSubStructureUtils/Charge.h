/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_charge_header
#define jetsubstructureutils_charge_header

#include "xAODJet/Jet.h"
namespace JetSubStructureUtils {
  class Charge { 
    public:
      Charge(double k = 1.0) : m_k(k) {};
      virtual double result(const xAOD::Jet &jet) const;
    private:
      double m_k;
  };
}

#endif
