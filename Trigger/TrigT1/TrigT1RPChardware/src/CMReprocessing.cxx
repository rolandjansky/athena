/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigT1RPChardware/CMReprocessing.h"

using namespace std;

//----------------------------------------------------------------------------//
CMReprocessing::CMReprocessing(MatrixReadOut *hard, Matrix *CMsimu) 
                                    : BaseObject(Hardware,"CMROReprocessing"){
//
// load and run the Matrix object "CMsimu" using as RPC input the data available
// in the MatrixReadOut object "hard". CMsimu is created outside this method
// before this method is used.
//
  CMsimu->reset();
  hard->doMatrix(CMsimu);
  MatrixReadOut simu(CMsimu,0,MatrixReadOut::Atlas);
//
// overwite header and subheader of the simulated readout fragment
// with the records from the hardware
//
  simu.overwriteHeader(hard->readHeader());
  simu.overwriteSubHeader(hard->readSubHeader());
//
// recompute the footer
//
  simu.reComputeFooter();
//
  CMROCompare CMROcmp(hard,&simu);
  int outCompare = CMROcmp.diffOut();
  cout<<" Comparison flag = "<<outCompare<<endl
      <<" check Fragment = "<<simu.checkFragment()<<endl;

  if(outCompare) {
   CMsimu->display();
   cout<<endl;
  }//
  
}//end-of-CMReprocessing
//----------------------------------------------------------------------------//
CMReprocessing::~CMReprocessing() {
}//end-of-~CMReprocessing()
