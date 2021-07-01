/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_CUSTOMTOPOCLUSTER_H
#define TRIGL0GEPPERF_CUSTOMTOPOCLUSTER_H

#include <vector>
#include "TLorentzVector.h"

namespace Gep{

  class CustomTopoCluster
  {
    
  public:
    CustomTopoCluster() {}
    ~CustomTopoCluster() {}
    
    void erase() {
      ncells = 0;
      time = 0.;
      vec.SetPxPyPzE(0.,0.,0.,0.);
      cell_id = {};
    }
    
    bool isEmptyCluster() const{
      if (ncells == 0) return true;
      return false;
    }
    
    int ncells;
    float time;
    TLorentzVector vec;
    std::vector<unsigned int> cell_id;

    void setEtEtaPhi(double et, double eta, double phi) {
      vec.SetPtEtaPhiM(et, eta, phi, 0.0);
    }

    float et() {
      return vec.Et();
    }
    
  private:
    
  };
}

#endif //TRIGL0GEPPERF_CUSTOMTOPOCLUSTER_H
