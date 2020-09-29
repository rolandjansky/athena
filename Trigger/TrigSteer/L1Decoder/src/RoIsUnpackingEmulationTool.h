/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_ROISUNPACKINGEMULATIONTOOL_H
#define L1DECODER_ROISUNPACKINGEMULATIONTOOL_H 1

// STL includes
#include <string>

#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

// L1Decoder includes
#include "RoIsUnpackingToolBase.h"


class RoIsUnpackingEmulationTool : public RoIsUnpackingToolBase { 


 public: 
  RoIsUnpackingEmulationTool( const std::string& type,
                              const std::string& name, 
                              const IInterface* parent );

  virtual StatusCode unpack(const EventContext& ctx,
                            const ROIB::RoIBResult& roib,
                            const HLT::IDSet& activeChains) const override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
  
 private: 

  ///@{ @name Properties
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigRoIsKey{
    this, "OutputTrigRoIs", "HLT_EMRoIs", "Name of the RoIs object produced by the unpacker"};

  Gaudi::Property<float> m_roIWidth{"RoIWidth", 0.1, "Size of RoI in eta/ phi"};

  Gaudi::Property<std::string> m_inputFilename{this, "InputFilename", "RoIEmulation.dat", "FakeROI input filename"};
  Gaudi::Property<std::string> m_thresholdPrefix{this, "ThresholdPrefix", "EM", "Activate chains that are seed from threshold that have this prefix in the name"};
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
  
  std::vector<TrigConf::TriggerThreshold*> m_emtauThresholds;
  
  std::vector<TrigConf::TriggerThreshold*> m_emThresholds;
  std::vector<std::vector<RoIsUnpackingEmulationTool::FakeRoI>> m_inputData;
  unsigned m_currentRowNumber;

  StatusCode decodeThresholdToChainMapping();

}; 

#endif //> !L1DECODER_EMROISUNPACKINGTOOL_H
