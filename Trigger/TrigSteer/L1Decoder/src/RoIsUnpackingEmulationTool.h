/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_ROISUNPACKINGEMULATIONTOOL_H
#define L1DECODER_ROISUNPACKINGEMULATIONTOOL_H 1

// STL includes
#include <string>

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// L1Decoder includes
#include "./IRoIsUnpackingTool.h"


class RoIsUnpackingEmulationTool : virtual public AthAlgTool, virtual public IRoIsUnpackingTool { 


 public: 
  RoIsUnpackingEmulationTool( const std::string& type,
		       const std::string& name, 
		       const IInterface* parent );

  virtual ~RoIsUnpackingEmulationTool(); 

  StatusCode unpack(const EventContext& ctx,
		    const ROIB::RoIBResult& roib,
		    const HLT::IDSet& activeChains) const override;
  
  // Athena algtool's Hooks
  StatusCode  initialize() override;
  StatusCode  updateConfiguration() override;
  StatusCode  finalize() override;
  
 private: 
  RoIsUnpackingEmulationTool();
  std::vector<TrigConf::TriggerThreshold*> m_emThresholds;
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_trigRoIsKey;
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  float m_roIWidth;

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
  
  std::string m_inputFilename;
  std::vector<std::vector<RoIsUnpackingEmulationTool::FakeRoI>> m_inputData;
  unsigned m_currentRowNumber;

}; 

#endif //> !L1DECODER_EMROISUNPACKINGTOOL_H
