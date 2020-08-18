/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOKKEEPERTOOL_H
#define BOOKKEEPERTOOL_H

/** @file BookkeeperTool.h
 *  @brief This class is an implementation of the AsgMetadataTool
 *  for the xAOD::CutBookkeeperContainer.
 *  @author Tadej Novak <tadej@cern.ch>
 *  @author Jack Cranshaw <cranshaw@anl.gov>
 **/

#include <memory>

#include <AsgTools/AsgMetadataTool.h>
#include <AthenaBaseComps/AthAlgTool.h>
#include <AthenaKernel/ICutFlowSvc.h>
#ifndef XAOD_STANDALONE
#include <AthenaKernel/IMetaDataTool.h>
#endif // XAOD_STANDALONE
#include <GaudiKernel/ServiceHandle.h>

#include <xAODCutFlow/CutBookkeeperContainer.h>
#include <xAODCutFlow/CutBookkeeperAuxContainer.h>

class CutFlowSvc;

class BookkeeperTool : public asg::AsgMetadataTool
#ifndef XAOD_STANDALONE
                     , public virtual ::IMetaDataTool
#endif // XAOD_STANDALONE
{
  ASG_TOOL_CLASS0(BookkeeperTool)
public: // Constructor and Destructor
  /// Standard Service Constructor
  BookkeeperTool(const std::string& name = "BookkeeperTool");
  /// Destructor
  virtual ~BookkeeperTool() = default;

  /// Standard reimplemented functions
public:
  virtual StatusCode initialize() override;
  virtual StatusCode metaDataStop() override;
  virtual StatusCode beginInputFile() override {return StatusCode::SUCCESS;}
  virtual StatusCode endInputFile() override {return StatusCode::SUCCESS;}
  virtual StatusCode beginInputFile(const SG::SourceID &source) override;
  virtual StatusCode endInputFile(const SG::SourceID &source) override;

private:
  /// Helper in-memory structure
  struct LocalContainers {
    std::vector<std::unique_ptr<xAOD::CutBookkeeperContainer>> cont;
    std::vector<std::unique_ptr<xAOD::CutBookkeeperAuxContainer>> aux;

    bool empty() { return cont.empty(); }
    std::size_t size() { return cont.size(); }
    xAOD::CutBookkeeperContainer *at(std::size_t n) { return cont.at(n).get(); }
    void clear() { cont.clear(); aux.clear(); }
  };

  /// Copy input containers to the output
  StatusCode copyInputContainersToOutput(LocalContainers &target,
                                         const SG::SourceID &source = "");

  /// Fill Cutflow information from the service
  StatusCode copyCutflowFromService();

  /// Prepare containers
  StatusCode prepareContainers(LocalContainers &target);

  /// Pointer to the public CutFlowSvc interface
  ServiceHandle<ICutFlowSvc> m_cutFlowSvc{ this, "CutFlowSvc", "CutFlowSvc/CutFlowSvc", "Pointer to the CutFlowSvc"};
  /// Direct pointer to the CutFlowSvc for "private" methods access
  const CutFlowSvc *m_cutFlowSvcPrivate;

  /// The name of the input CutBookkeeperContainer
  Gaudi::Property<std::string> m_inputCollName{this, "InputCollName", "CutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for input files"};

  /// The name of the output CutBookkeeperContainer
  Gaudi::Property<std::string> m_outputCollName{this, "OutputCollName", "CutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for output files"};

  /// Input CutBookkeeperContainers
  std::unordered_map<SG::SourceID, LocalContainers> m_inputContainers;

  /// Local CutBookkeeperContainers
  LocalContainers m_completeContainers;
  LocalContainers m_incompleteContainers;
};

#endif // BOOKKEEPERTOOL_H
