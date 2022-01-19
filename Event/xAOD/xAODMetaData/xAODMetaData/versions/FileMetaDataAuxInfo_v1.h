// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaDataAuxInfo_v1.h 670157 2015-05-27 11:52:51Z krasznaa $
#ifndef XAODMETADATA_VERSIONS_FILEMETADATAAUXINFO_V1_H
#define XAODMETADATA_VERSIONS_FILEMETADATAAUXINFO_V1_H

// System include(s):
#include <string>
#include <vector>

// EDM include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

   /// Auxiliary store for xAOD::FileMetaData_v1
   ///
   /// This is a fairly generic auxiliary store implementation to be
   /// used with xAOD::FileMetaData_v1. Most of the variables in that
   /// class are dynamic, so this class is fairly small.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 670157 $
   /// $Date: 2015-05-27 13:52:51 +0200 (Wed, 27 May 2015) $
   ///
   class FileMetaDataAuxInfo_v1 : public AuxInfoBase {

   public:
      /// Default constructor
      FileMetaDataAuxInfo_v1();

   private:
      /// Release that was used to make the file
      std::string productionRelease;
      /// Type of the data that's in the file
      std::string dataType;
      /// run numbers in the file
      std::vector<uint32_t> runNumbers;

   }; // class FileMetaDataAuxInfo_v1

} // namespace xAOD

// Declare a base class for the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::FileMetaDataAuxInfo_v1, xAOD::AuxInfoBase );

#endif // XAODMETADATA_VERSIONS_FILEMETADATAAUXINFO_V1_H
