/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaDataAuxInfo_v1.cxx 670157 2015-05-27 11:52:51Z krasznaa $

// Local include(s):
#include "xAODMetaData/versions/FileMetaDataAuxInfo_v1.h"

namespace xAOD {

   FileMetaDataAuxInfo_v1::FileMetaDataAuxInfo_v1()
      : AuxInfoBase() {

      // Declare the auxiliary variables of the class:
      AUX_VARIABLE( productionRelease );
      AUX_VARIABLE( dataType );
      AUX_VARIABLE( runNumbers );
      AUX_VARIABLE( lumiBlocks );
   }

} // namespace xAOD
