/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_ROISUNPACKINGEMULATIONTOOL_H
#define HLTSEEDING_ROISUNPACKINGEMULATIONTOOL_H 1

#include "RoIsUnpackingToolBase.h"

#include <string>

class RoIsUnpackingEmulationTool : public RoIsUnpackingToolBase { 
 public: 
  RoIsUnpackingEmulationTool( const std::string& type,
                              const std::string& name, 
                              const IInterface* parent );

  using RoIsUnpackingToolBase::unpack;
  virtual StatusCode unpack(const EventContext& ctx,
                            const ROIB::RoIBResult& roib,
                            const HLT::IDSet& activeChains) const override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
  
 private: 
  ///@{ @name Properties
  Gaudi::Property<float> m_roIWidth{"RoIWidth", 0.1, "Size of RoI in eta/ phi"};

  Gaudi::Property<std::string> m_inputFilename{
    this, "InputFilename", "RoIEmulation.dat", "FakeROI input filename"};

  Gaudi::Property<std::string> m_thresholdPrefix{
    this, "ThresholdPrefix", "EM",
    "Activate chains that are seed from threshold that have this prefix in the name"};
  ///@}

  // emulation
  struct FakeRoI {
    double eta;
    double phi;
    uint32_t roIWord;
    std::vector<std::string> passedThresholdIDs;
  };

  StatusCode readEmulatedData();
  std::vector<std::vector<FakeRoI>> parseInputFile();
  std::vector<FakeRoI> parseInputFileLine(const std::string& line, unsigned lineNumber);
  FakeRoI parseInputRoI(const std::string& roi, unsigned lineNumber, unsigned roiNumber);
  std::vector<std::vector<RoIsUnpackingEmulationTool::FakeRoI>> m_inputData;
  unsigned m_currentRowNumber;
}; 

#endif //> !HLTSEEDING_EMROISUNPACKINGTOOL_H
