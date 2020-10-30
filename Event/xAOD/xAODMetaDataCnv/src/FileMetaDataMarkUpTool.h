// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
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
class FileMetaDataMarkUpTool
    : public asg::AsgMetadataTool
    , public virtual ::IAthenaOutputTool {
  /// Declare the correct constructor for Athena
  ASG_TOOL_CLASS(FileMetaDataMarkUpTool, IAthenaOutputTool)

 public:
      /// Regular AsgTool constructor
      explicit FileMetaDataMarkUpTool(
          const std::string& name = "FileMetaDataMarkUpTool");

      /// Required of all IAthenaOutputTools:
      /// Called by AthenaOutputStream::initialize() (via ToolSvc retrieve()).
      StatusCode initialize() override;
      /// Called at the end of AthenaOutputStream::initialize().
      StatusCode postInitialize() override;
      /// Called at the beginning of AthenaOutputStream::execute().
      StatusCode preExecute() override;
      /// Called before actually streaming objects.
      StatusCode preStream() override;
      /// Called at the end of AthenaOutputStream::execute().
      StatusCode postExecute() override;
      /// Called at the beginning of AthenaOutputStream::finalize().
      StatusCode preFinalize() override;
      /// Called at the end of AthenaOutputStream::finalize() (via release()).
      StatusCode finalize() override;
      StatusCode start() override;

 protected:
      /// @name Functions called by the AsgMetadataTool base class
      /// @{

      /// Function collecting the metadata from a new input file
      StatusCode beginInputFile() override;

      /// Function writing the collected metadata to the output
      StatusCode metaDataStop() override;

      /// @}

 private:
      /// Key of the metadata object for the output file
      std::string m_outputKey;
};  // class FileMetaDataMarkUpTool

}  // namespace xAODMaker

#endif  // XAODMETADATACNV_FILEMETADATAMARKUPTOOL_H
