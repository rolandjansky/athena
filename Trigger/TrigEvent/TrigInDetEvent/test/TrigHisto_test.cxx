/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Following the example of
// Trigger/TrigEvent/TrigSteeringEvent/test/Operators_test.cxx 

// Need to modify requirements file to compile this code once the HEAD
// of TrigInDetEvent compiles against a nightly.  Waiting for Tomasz et al..

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

#include "TrigInDetEvent/TrigHisto.h"

int main() {
  MsgStream log(Athena::getMessageSvc(), "TrigHisto_test");
 
  TrigHisto1D<unsigned char> trigHisto1D(5,-2.5,2.5);

  trigHisto1D.fill(-3.0, 2);
  trigHisto1D.fill(-2.5, 4);
  trigHisto1D.fill(1., 1);

  TrigHisto2D<unsigned int> trigHisto2D(20,-10.,10.,10,-5.,5.);
  for(int i=0;i<22;i++) {
    for(int j=0;j<12;j++) {
      trigHisto2D.fill(i-11,j-6);
    }
  }

  return 0;
}
  
