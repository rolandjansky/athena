/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xAODFileMetaData_MetaDataType_test.cxx 670157 2015-05-27 11:52:51Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODMetaData/FileMetaData.h"

int main() {

   // Print some known types:
   std::cout << xAOD::FileMetaData::productionRelease << std::endl;
   std::cout << xAOD::FileMetaData::AODCalibVersion << std::endl;

   // Print an unknown type:
   std::cout << static_cast< xAOD::FileMetaData::MetaDataType >( 100 )
             << std::endl;

   // Return gracefully:
   return 0;
}
