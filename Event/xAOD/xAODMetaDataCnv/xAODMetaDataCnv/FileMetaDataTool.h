// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODMETADATACNV_FILEMETADATATOOL_H
#define XAODMETADATACNV_FILEMETADATATOOL_H

// System include(s):
#include <string>

// Infrastructure include(s):
#include "AsgTools/AsgMetadataTool.h"
#ifndef XAOD_STANDALONE
# include "GaudiKernel/ServiceHandle.h"
# include "AthenaKernel/IMetaDataSvc.h"
# include "AthenaKernel/IMetaDataTool.h"
#endif

namespace xAODMaker {

/// Tool propagating xAOD::FileMetaData from input to output
///
/// This tool propogates the xAOD::FileMetaData object from the input files to
/// the MetaDataStore in Athena. It requires the input to contain the
/// information in an xAOD format. The tool will emit a warning if the file
/// metadata between inputs does not match.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
/// @author Frank Berghaus <fberghaus@anl.gov>
///
class FileMetaDataTool
#ifdef XAOD_STANDALONE
    : public asg::AsgMetadataTool {
#else
    : public asg::AsgMetadataTool, virtual public IMetaDataTool {
#endif
 public:
  //using extends::extends;
  ASG_TOOL_CLASS0(FileMetaDataTool)

  /// Regular AsgTool constructor
  explicit FileMetaDataTool(const std::string& name = "FileMetaDataTool");

  /// Function initialising the tool
  StatusCode initialize() override;

  /// @name Functions called by the IMetaDataTool base class
  /// @{

  /// Collecting file metadata from input and write to output
  StatusCode beginInputFile() override;

  /// Does nothing
  StatusCode endInputFile() override;

#ifndef XAOD_STANDALONE
  /// Collecting file metadata from input and write to output
  StatusCode beginInputFile(const SG::SourceID&) override;

  /// Does nothing
  StatusCode endInputFile(const SG::SourceID&) override;
#endif

  /// Does nothing
  StatusCode metaDataStop() override;

  /// @}

 private:
  /// Key of the metadata object in the input file
  std::string m_inputKey;

  /// Key of the metadata object for the output file
  std::string m_outputKey;

#ifndef XAOD_STANDALONE
  /// Get a handle on the metadata store for the job
  ServiceHandle< IMetaDataSvc > m_metaDataSvc{"MetaDataSvc", name()};
#endif

  // To lock/unlock the tool
  std::mutex m_toolMutex;
};  // class FileMetaDataTool

}  // namespace xAODMaker

#endif  // XAODMETADATACNV_FILEMETADATATOOL_H
