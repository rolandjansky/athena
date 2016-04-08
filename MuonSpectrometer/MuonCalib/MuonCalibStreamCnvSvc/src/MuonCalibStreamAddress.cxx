/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//own 
#include "MuonCalibStreamCnvSvc/MuonCalibStreamAddress.h"

// Framework include files
#include "GaudiKernel/GenericAddress.h"
//#include <iostream>

// Externals
extern long MuonCalibStream_StorageType;

/// Standard Constructor
MuonCalibStreamAddress::MuonCalibStreamAddress(const CLID& clid, 
const std::string& fname , const std::string& cname, int p1, int p2)
 : GenericAddress(MuonCalibStream_StorageType, clid,fname,cname, p1, p2 ) {
  //std::cout << "MuonCalibStreamAddress::MuonCalibStreamAddress 1 "<<fname<<std::endl;
}
MuonCalibStreamAddress::MuonCalibStreamAddress(const CLID& clid) 
  : GenericAddress(MuonCalibStream_StorageType, clid,"","") {
  //std::cout << "MuonCalibStreamAddress::MuonCalibStreamAddress 2"<<std::endl;
}
