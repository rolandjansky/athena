// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODMETADATA_XAODMETADATADICT_H
#define XAODMETADATA_XAODMETADATADICT_H

// Local include(s).
#include "xAODMetaData/FileMetaData.h"
#include "xAODMetaData/FileMetaDataAuxInfo.h"
#include "xAODMetaData/versions/FileMetaData_v1.h"
#include "xAODMetaData/versions/FileMetaDataAuxInfo_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODMETADATA {
      // Local type(s).
      XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, FileMetaData_v1 );
   };
}

#endif // XAODMETADATA_XAODMETADATADICT_H
