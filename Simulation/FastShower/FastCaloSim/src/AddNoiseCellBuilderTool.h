/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTCALOSIM_ADDNOISE_CELLBUILDERTOOL_H
#define FASTCALOSIM_ADDNOISE_CELLBUILDERTOOL_H

/**
 * @file   AddNoiseCellBuilderTool.h
 * @class  AddNoiseCellBuilderTool
 * @brief  Building Cells objects from Atlfast
 * @author Michael Duehrssen 
 */

#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloEstimatedGainTool.h"
#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class AddNoiseCellBuilderTool: public extends<AthAlgTool, ICaloCellMakerTool>
{
public:
  AddNoiseCellBuilderTool(
                          const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  ~AddNoiseCellBuilderTool();

  virtual StatusCode initialize() override;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;
private:

  SG::ReadCondHandleKey<CaloNoise> m_noiseKey
  { this, "NoiseKey", "electronicNoise", "Calo noise CDO" };

  ToolHandle<ICaloEstimatedGainTool> m_estimatedGain
  { this, "CaloEstimatedGainTool", "CaloEstimatedGainTool", "Estimated gain tool." };

  ServiceHandle<IAthRNGSvc> m_rndmGenSvc{this, "RandomSvc", "AthRNGSvc", ""};
  Gaudi::Property<std::string> m_randomEngineName{this, "RandomStreamName", "FastCaloSimNoiseRnd", "Name of the random number stream"};
  Gaudi::Property<bool> m_donoise{this, "doNoise", true};
};

#endif
