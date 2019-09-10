// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaData.h 670157 2015-05-27 11:52:51Z krasznaa $
#ifndef XAODMETADATA_FILEMETADATA_H
#define XAODMETADATA_FILEMETADATA_H

// Local include(s):
#include "xAODMetaData/versions/FileMetaData_v1.h"

namespace xAOD {
   /// Declare the latest version of the class
   typedef FileMetaData_v1 FileMetaData;
} // namespace xAOD

// Declare a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::FileMetaData, 178309087, 1 )
#ifndef XAOD_STANDALONE
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<xAOD::FileMetaData> , 178903087 , 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( MetaCont<xAOD::FileMetaData>, MetaContBase );
#endif // not XAOD_STANDALONE

#endif // XAODMETADATA_FILEMETADATA_H
