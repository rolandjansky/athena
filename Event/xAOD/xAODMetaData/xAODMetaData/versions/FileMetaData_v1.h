// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaData_v1.h 685184 2015-07-23 20:25:43Z cranshaw $
#ifndef XAODMETADATA_VERSIONS_FILEMETADATA_V1_H
#define XAODMETADATA_VERSIONS_FILEMETADATA_V1_H

// System include(s):
#include <iosfwd>
#include <string>
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class holding file-level metadata about an xAOD file
   ///
   /// Each xAOD file is expected to hold exactly one object of this type
   /// to describe much of the simple metadata about the file. So, metadata
   /// that might change event-by-event in the file is not stored in such
   /// objects.
   ///
   /// @param Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 685184 $
   /// $Date: 2015-07-23 22:25:43 +0200 (Thu, 23 Jul 2015) $
   ///
   class FileMetaData_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      FileMetaData_v1();

      /// @name Comparison operators
      /// @{

      /// Operator testing the equality of two objects
      bool operator==( const FileMetaData_v1& rhs ) const;
      /// Operator testing the inequality of two objects
      bool operator!=( const FileMetaData_v1& rhs ) const;

      /// @}

      /// Pre-defined metadata value types
      enum MetaDataType {
         /// Release that was used to make the file [string]
         productionRelease = 0,
         /// AMI tag used to process the file the last time [string]
         amiTag = 1,
         /// Version of AODFix that was used on the file last [string]
         AODFixVersion = 2,
         /// Version of AODCalib that was used on the file last [string]
         AODCalibVersion = 3,
         /// Data type that's in the file [string]
         dataType = 4,
         /// Geometry version [string]
         geometryVersion = 5,
         /// Conditions version used for simulation/reconstruction [string]
         conditionsTag = 6,
         /// Beam energy [float]
         beamEnergy = 7,
         /// Beam type [string]
         beamType = 8,
         /// Same as mc_channel_number [float]
         mcProcID = 9,
         /// Fast or Full sim [string]
         simFlavour = 10,
         /// Used data overlay for backgrounds [bool]
         isDataOverlay = 11,
         /// End marker
         END = 12
      }; // enum MetaDataType

      /// Get a pre-defined string value out of the object
      bool value( MetaDataType type, std::string& val ) const;
      /// Get a generic string value out of the object
      bool value( const std::string& type, std::string& val ) const;

      /// Set a pre-defined string value on the object
      bool setValue( MetaDataType type, const std::string& val );
      /// Set a generic string value on the object
      bool setValue( const std::string& type, const std::string& val );

      /// Get a pre-defined float value out of the object
      bool value( MetaDataType type, float& val ) const;
      /// Get a generic float value out of the object
      bool value( const std::string& type, float& val ) const;

      /// Set a pre-defined float value on the object
      bool setValue( MetaDataType type, float val );
      /// Set a generic float value on the object
      bool setValue( const std::string& type, float val );

      /// Get a pre-defined bool value out of the object
      bool value( MetaDataType type, bool& val ) const;
      /// Get a generic bool value out of the object
      bool value( const std::string& type, bool& val ) const;

      /// Set a pre-defined bool value on the object
      bool setValue( MetaDataType type, bool val );
      /// Set a generic bool value on the object
      bool setValue( const std::string& type, bool val );

      /// Get a generic list value out of the object
      bool value(const std::string& type, std::vector< uint32_t >& val) const;

      /// Set a generic list value on the object
      bool setValue(const std::string& type, const std::vector< uint32_t >& val);

   }; // class FileMetaData_v1

   /// A convenience print operator for xAOD::FileMetaData_v1::MetaDataType
   std::ostream& operator<< ( std::ostream& out,
                              xAOD::FileMetaData_v1::MetaDataType type );

} // namespace xAOD

// Declare a base class for the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::FileMetaData_v1, SG::AuxElement );

#endif // XAODMETADATA_VERSIONS_FILEMETADATA_V1_H
