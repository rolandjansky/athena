/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowPi0Cuts.h"
#include <cmath>

void eflowPi0Cuts::isPi0(bool& result, float eta, float et, float pi0Score, int numProng){

  if (1 == numProng){
    if (fabs(eta) < 0.8) {
      if (et > 2500 && pi0Score > 0.09) result = true;
    }
    else if (fabs(eta) < 1.4){
      if (et > 2500 && pi0Score > 0.09) result = true;
    }
    else if (fabs(eta) < 1.5){
      if (et > 1900 && pi0Score > 0.09) result = true;
    }
    else if (fabs(eta) < 1.9){
      if (et > 2500 && pi0Score > 0.08) result = true;
    }
    else{
      if (et > 2300 && pi0Score > 0.05) result = true;
    }
  }
  else if (3 == numProng){
    if (fabs(eta) < 0.8) {
      if (et > 2500 && pi0Score > 0.09) result = true;
    }
    else if (fabs(eta) < 1.4){
      if (et > 2500 && pi0Score > 0.09) result = true;
    }
    else if (fabs(eta) < 1.5){
      if (et > 2500 && pi0Score > 0.09) result = true;
    }
    else if (fabs(eta) < 1.9){
      if (et > 2500 && pi0Score > 0.09) result = true;
    }
    else{
      if (et > 2500 && pi0Score > 0.07) result = true;
    }
  }

}
