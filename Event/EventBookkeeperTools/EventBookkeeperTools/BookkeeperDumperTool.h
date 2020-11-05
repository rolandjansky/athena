/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include <AsgTools/PropertyWrapper.h>
#ifndef XAOD_STANDALONE
#include <AthenaKernel/IMetaDataTool.h>
#endif

class BookkeeperDumperTool : public asg::AsgMetadataTool
#ifndef XAOD_STANDALONE
                           , public virtual IMetaDataTool
#endif
{
#ifndef XAOD_STANDALONE
  /// Declare the correct constructor for Athena
  ASG_TOOL_CLASS( BookkeeperDumperTool, IMetaDataTool )
#endif

public:
  BookkeeperDumperTool(const std::string &name = "BookkeeperDumperTool");

  virtual StatusCode initialize() final;

  virtual StatusCode beginInputFile() final;
  virtual StatusCode endInputFile() final { return StatusCode::SUCCESS; }
  virtual StatusCode metaDataStop() final { return StatusCode::SUCCESS; }

#ifndef XAOD_STANDALONE
  virtual StatusCode beginInputFile(const SG::SourceID &) final { return StatusCode::SUCCESS; }
  virtual StatusCode endInputFile(const SG::SourceID &) final { return StatusCode::SUCCESS; }
#endif

  Gaudi::Property<bool> m_allVariations{this, "AllVariations", false, "Dump all variations"};
  Gaudi::Property<bool> m_standaloneMode{this, "StandaloneMode", false, "Dump on initialize when running standalone"};
};

#endif
