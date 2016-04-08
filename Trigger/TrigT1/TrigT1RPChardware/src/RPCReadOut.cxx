/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include "TrigT1RPChardware/RPCReadOut.h"

using namespace std;

  // initialize some fixed binary pattern
const ubit16 RPCReadOut::last2bitsON =0xc000;
const ubit16 RPCReadOut::last4bitsON =0xf000;
//---------------------------------------------------------------------------//
RPCReadOut::RPCReadOut() {
}//end-of-RPCReadOut::RPCReadOut()
//---------------------------------------------------------------------------//
RPCReadOut::~RPCReadOut() {
}//end-of-RPCReadOut::~RPCReadOut
//---------------------------------------------------------------------------//
