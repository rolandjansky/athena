// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaDataMarkUpTool.h 676522 2015-06-18 22:17:03Z cranshaw $
#ifndef XAODMETADATACNV_FILEMETADATAMARKUPTOOL_H
#define XAODMETADATACNV_FILEMETADATAMARKUPTOOL_H

// System include(s):
#include <string>
#include <memory>

// Infrastructure include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AthenaKernel/IAthenaOutputTool.h"

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
   class FileMetaDataMarkUpTool : public asg::AsgMetadataTool,
                                   public virtual ::IAthenaOutputTool {

      /// Declare the correct constructor for Athena
      ASG_TOOL_CLASS( FileMetaDataMarkUpTool, IAthenaOutputTool )

   public:
      /// Regular AsgTool constructor
      FileMetaDataMarkUpTool( const std::string& name =
                               "FileMetaDataMarkUpTool" );

      /// Required of all IAthenaOutputTools:
      /// Called by AthenaOutputStream::initialize() (via ToolSvc retrieve()).
      StatusCode initialize();
      /// Called at the end of AthenaOutputStream::initialize().
      StatusCode postInitialize();
      /// Called at the beginning of AthenaOutputStream::execute().
      StatusCode preExecute();
      /// Called at the end of AthenaOutputStream::execute().
      StatusCode postExecute();
      /// Called at the beginning of AthenaOutputStream::finalize().
      StatusCode preFinalize();
      /// Called at the end of AthenaOutputStream::finalize() (via release()).
      StatusCode finalize();
      virtual StatusCode start();

   protected:
      /// @name Functions called by the AsgMetadataTool base class
      /// @{

      /// Function collecting the metadata from a new input file
      virtual StatusCode beginInputFile();

      /// Function writing the collected metadata to the output
      virtual StatusCode metaDataStop();

      /// @}

   private:
      /// Key of the metadata object for the output file
      std::string m_outputKey;

   }; // class FileMetaDataMarkUpTool

} // namespace xAODMaker

#endif // XAODMETADATACNV_FILEMETADATAMARKUPTOOL_H
