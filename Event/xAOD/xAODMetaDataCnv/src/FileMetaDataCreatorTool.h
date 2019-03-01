// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaDataCreatorTool.h 676522 2015-06-18 22:17:03Z cranshaw $
#ifndef XAODMETADATACNV_FILEMETADATACREATORTOOL_H
#define XAODMETADATACNV_FILEMETADATACREATORTOOL_H

// System include(s):
#include <string>
#include <memory>

// Infrastructure include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AthenaKernel/IMetaDataTool.h"

// EDM include(s):
#include "xAODMetaData/FileMetaData.h"
#include "xAODMetaData/FileMetaDataAuxInfo.h"

namespace xAODMaker {

   /// Tool creating or propagating xAOD::FileMetaData information
   ///
   /// This Athena-only tool can be used to create xAOD::FileMetaData
   /// information out of the non-ROOT-readable metadata available in
   /// the input.
   ///
   /// Or, if the input file already has xAOD::FileMetaData payload,
   /// it is taken as is, and copied to the output.
   ///
   /// The class uses asg::AsgMetadataTool as a base class for convenience,
   /// but it's not a dual-use tool. (Hence the header is hidden from the
   /// outside world.)
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 676522 $
   /// $Date: 2015-06-19 00:17:03 +0200 (Fri, 19 Jun 2015) $
   ///
   class FileMetaDataCreatorTool : public asg::AsgMetadataTool,
                                   public virtual ::IMetaDataTool {

      /// Declare the correct constructor for Athena
      ASG_TOOL_CLASS( FileMetaDataCreatorTool, IMetaDataTool )

   public:
      /// Regular AsgTool constructor
      FileMetaDataCreatorTool( const std::string& name =
                               "FileMetaDataCreatorTool" );

      /// Function initialising the tool
      virtual StatusCode initialize();
      virtual StatusCode start();

   protected:
      /// @name Functions called by the AsgMetadataTool base class
      /// @{

      /// Function collecting the metadata from a new input file
      virtual StatusCode beginInputFile();

      /// Function collecting the metadata from a new input file
      virtual StatusCode endInputFile();

      /// Function writing the collected metadata to the output
      virtual StatusCode metaDataStop();

      /// Function collecting the metadata from a new input file
      virtual StatusCode beginInputFile(const SG::SourceID&) {return StatusCode::SUCCESS;}

      /// Function collecting the metadata from a new input file
      virtual StatusCode endInputFile(const SG::SourceID&) {return StatusCode::SUCCESS;}

      /// @}

   private:
      /// Function called by the DetectorStore when the metadata is updated
      StatusCode update( IOVSVC_CALLBACK_ARGS_P( I, keys ) );

      /// Key of the metadata object for the output file
      std::string m_outputKey;

      /// The output interface object
      std::unique_ptr< xAOD::FileMetaData > m_md;
      /// The output auxiliary object
      std::unique_ptr< xAOD::FileMetaDataAuxInfo > m_mdAux;

   }; // class FileMetaDataCreatorTool

} // namespace xAODMaker

#endif // XAODMETADATACNV_FILEMETADATACREATORTOOL_H
