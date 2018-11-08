/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGOUTPUTHANDLING_HLTRESULTMTMAKER_H
#define TRIGOUTPUTHANDLING_HLTRESULTMTMAKER_H

#include "TrigSteeringEvent/HLTResultMT.h"
#include "TrigOutputHandling/HLTResultMTMakerTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "StoreGate/WriteHandle.h"

/** @class HLTResultMTMaker
 *  @brief Tool to create the HLTResultMT at the end of each event
 **/
class HLTResultMTMaker : public AthAlgTool {
public:
  /// Standard constructor
  HLTResultMTMaker(const std::string& type, const std::string& name, const IInterface* parent);
  /// Standard destructor
  virtual ~HLTResultMTMaker();

  // ------------------------- IStateful methods -----------------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ------------------------- Specific methods of this tool -----------------
  /// Builds the HLTResultMT and records it in the event store
  StatusCode makeResult(const EventContext& eventContext) const;
  /// Return name of the HLTResultMT
  const std::string& resultName() const {return m_hltResultWHKey.key();}

private:
  /// StoreGate key for the HLTResultMT
  SG::WriteHandleKey<HLT::HLTResultMT> m_hltResultWHKey {this, "HLTResultWHKey", "HLTResultMT",
                                                         "Key of the output HLTResultMT object"};
  /// Tools filling the HLTResultMT object
  ToolHandleArray<HLTResultMTMakerTool> m_makerTools {this, "MakerTools", {},
                                                      "Set of tools that fill content of the HLTResultMT"};
  /// Monitoring tool
  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "",
                                               "Monitoring tool"};
};

#endif // TRIGOUTPUTHANDLING_HLTRESULTMTMAKER_H
