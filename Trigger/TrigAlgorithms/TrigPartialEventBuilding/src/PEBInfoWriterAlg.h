/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigPartialEventBuilding_PEBInfoWriterAlg_h
#define TrigPartialEventBuilding_PEBInfoWriterAlg_h

#include "DecisionHandling/HypoBase.h"
#include "TrigPartialEventBuilding/PEBInfoWriterToolBase.h"

/** @class PEBInfoWriterAlg
 *  @brief Pass-through hypo algorithm writing decisions with attached partial event building information
 **/
class PEBInfoWriterAlg : public HypoBase {
public:
  /// Standard constructor
  PEBInfoWriterAlg(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~PEBInfoWriterAlg();

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;

private:
  ToolHandleArray<PEBInfoWriterToolBase> m_hypoTools {this, "HypoTools", {}, "Tools to create the PEB Info"};
};

#endif // TrigPartialEventBuilding_PEBInfoWriterAlg_h
