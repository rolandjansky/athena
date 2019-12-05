/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_BOOKKEEPER_TOOLS__BOOKKEEPER_DUMPER_TOOL_H
#define EVENT_BOOKKEEPER_TOOLS__BOOKKEEPER_DUMPER_TOOL_H

/**
 * @file BookkeeperDumperTool.h
 * @brief class definition for BookkeeperDumperTool
 */

/**
 * @class BookkeeperDumper
 * @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
 * Created October 2017
 */

#include <AsgTools/AsgMetadataTool.h>
#include <AthenaKernel/IMetaDataTool.h>

class BookkeeperDumperTool : public asg::AsgMetadataTool,
                             public virtual IMetaDataTool
{
  /// Declare the correct constructor for Athena
  ASG_TOOL_CLASS( BookkeeperDumperTool, IMetaDataTool )

public:
  BookkeeperDumperTool(const std::string &name = "BookkeeperDumperTool");

  virtual StatusCode beginInputFile() final;
  virtual StatusCode beginInputFile(const SG::SourceID &) final { return StatusCode::SUCCESS; }
  virtual StatusCode endInputFile() final { return StatusCode::SUCCESS; }
  virtual StatusCode endInputFile(const SG::SourceID &) final { return StatusCode::SUCCESS; }
  virtual StatusCode metaDataStop() final { return StatusCode::SUCCESS; }
};

#endif
