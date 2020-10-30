/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_BOOKKEEPER_TOOLS__BOOKKEEPER_TOOL_H
#define EVENT_BOOKKEEPER_TOOLS__BOOKKEEPER_TOOL_H

/** @file BookkeeperTool.h
 *  @brief This class is an implementation of the AsgMetadataTool
 *  for the xAOD::CutBookkeeperContainer.
 *  @author Tadej Novak <tadej@cern.ch>
 *  @author Jack Cranshaw <cranshaw@anl.gov>
 **/

#include <memory>

#include <AsgTools/AsgMetadataTool.h>
#include <AsgTools/PropertyWrapper.h>
#include <AsgTools/SourceID.h>

#ifndef XAOD_STANDALONE
#include <AthenaKernel/ICutFlowSvc.h>
#include <AthenaKernel/IMetaDataTool.h>
#include <GaudiKernel/ServiceHandle.h>
#endif // XAOD_STANDALONE

#include <xAODCutFlow/CutBookkeeperContainer.h>
#include <xAODCutFlow/CutBookkeeperAuxContainer.h>

#include <EventBookkeeperTools/CutBookkeepersLocalCache.h>


#ifndef XAOD_STANDALONE

class CutFlowSvc;

#endif // XAOD_STANDALONE


class BookkeeperTool : public asg::AsgMetadataTool
#ifndef XAOD_STANDALONE
                     , public virtual ::IMetaDataTool
#endif // XAOD_STANDALONE
{
#ifndef XAOD_STANDALONE
  /// Declare the correct constructor for Athena
  ASG_TOOL_CLASS( BookkeeperTool, IMetaDataTool )
#endif

public: // Constructor and Destructor
  /// Standard Service Constructor
  BookkeeperTool(const std::string& name = "BookkeeperTool");
  /// Destructor
  virtual ~BookkeeperTool() = default;

  /// Standard reimplemented functions
public:
  virtual StatusCode initialize() override;
  virtual StatusCode metaDataStop() override;
#ifdef XAOD_STANDALONE
  virtual StatusCode beginInputFile() override;
  virtual StatusCode endInputFile() override;
#else
  virtual StatusCode beginInputFile() override {return StatusCode::SUCCESS;}
  virtual StatusCode endInputFile() override {return StatusCode::SUCCESS;}
  virtual StatusCode beginInputFile(const SG::SourceID &source) override;
  virtual StatusCode endInputFile(const SG::SourceID &source) override;
#endif

private:
  /// Copy input containers to the output
  StatusCode copyInputContainersToOutput(CutBookkeepersLocalCache &target,
                                         const SG::SourceID &source = "");

  /// Fill Cutflow information from the service
  StatusCode copyCutflowFromService();

  /// Initialisation helpers
  StatusCode loadXAODMetaData();
  StatusCode loadPOOLMetaData();

#ifndef XAOD_STANDALONE
  /// Pointer to the public CutFlowSvc interface
  ServiceHandle<ICutFlowSvc> m_cutFlowSvc{ this, "CutFlowSvc", "CutFlowSvc/CutFlowSvc", "Pointer to the CutFlowSvc"};
  /// Direct pointer to the CutFlowSvc for "private" methods access
  CutFlowSvc *m_cutFlowSvcPrivate;
#endif // XAOD_STANDALONE

  /// The name of the input CutBookkeeperContainer
  Gaudi::Property<std::string> m_inputCollName{this, "InputCollName", "CutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for input files"};

  /// The name of the output CutBookkeeperContainer
  Gaudi::Property<std::string> m_outputCollName{this, "OutputCollName", "CutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for output files"};

  /// Number of event weight variations
  size_t m_numberOfWeightVariations{};

  /// Input CutBookkeeperContainers
  std::unordered_map<SG::SourceID, CutBookkeepersLocalCache> m_inputContainers;

  /// Local CutBookkeeperContainers
  CutBookkeepersLocalCache m_completeContainers;
  CutBookkeepersLocalCache m_incompleteContainers;
};

#endif // EVENT_BOOKKEEPER_TOOLS__BOOKKEEPER_TOOL_H
