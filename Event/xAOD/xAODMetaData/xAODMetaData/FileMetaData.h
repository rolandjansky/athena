// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#endif // XAODMETADATA_FILEMETADATA_H
