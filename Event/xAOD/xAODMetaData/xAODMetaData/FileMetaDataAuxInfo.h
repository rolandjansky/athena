// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaDataAuxInfo.h 670157 2015-05-27 11:52:51Z krasznaa $
#ifndef XAODMETADATA_FILEMETADATAAUXINFO_H
#define XAODMETADATA_FILEMETADATAAUXINFO_H

// Local include(s):
#include "xAODMetaData/versions/FileMetaDataAuxInfo_v1.h"

namespace xAOD {
   /// Declare the latest version of the class
   typedef FileMetaDataAuxInfo_v1 FileMetaDataAuxInfo;
} // namespace xAOD

// Declare a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::FileMetaDataAuxInfo, 73252552, 1 )
#ifndef XAOD_STANDALONE
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<xAOD::FileMetaDataAuxInfo> , 233059795, 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( MetaCont<xAOD::FileMetaDataAuxInfo>, MetaContBase );
#endif // not XAOD_STANDALONE

#endif // XAODMETADATA_FILEMETADATAAUXINFO_H
