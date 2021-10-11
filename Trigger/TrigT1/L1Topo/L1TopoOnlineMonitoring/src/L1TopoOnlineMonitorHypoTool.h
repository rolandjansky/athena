/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOONLINEMONITORING_L1TopoOnlineMonitorHypoTool_h
#define L1TOPOONLINEMONITORING_L1TopoOnlineMonitorHypoTool_h

// Trigger includes
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"

/**
 *  @class L1TopoOnlineMonitorHypoTool
 *  @brief Hypo tool using the xAOD::TrigComposite with information on potential L1Topo hardware,
           decoding or simulation issues to decide whether an event should be accepted or not
 **/
class L1TopoOnlineMonitorHypoTool : public AthAlgTool {
public:
  // ------------------------- Public types ------------------------------------
  using InputVector = std::vector<std::pair<TrigCompositeUtils::Decision*, const TrigCompositeUtils::Decision*>>;

  // ------------------------- Public methods ----------------------------------
  /// Standard constructor
  L1TopoOnlineMonitorHypoTool(const std::string& type, const std::string& name, const IInterface* parent);

  /// Implementation of AthAlgTool::initialize()
  virtual StatusCode initialize() override;

  /// The main method of the tool
  StatusCode decide(const InputVector& input, const EventContext& eventContext) const;

private:
  // ------------------------- Private members ---------------------------------
  /// The decision id of the tool instance
  HLT::Identifier m_decisionId;
  /// The input ErrorFlags
  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_errorFlagsKey {
    this, "ErrorFlagsKey", "L1TopoErrorFlags",
    "Key of the input TrigCompositeContainer with L1Topo error flags"};
  
  // ------------------------- Accept properties -------------------------------
  Gaudi::Property<bool> m_acceptGenericRoiError {
    this, "AcceptOnGenericRoiError", false, "Accept events with the hasGenericRoiError flag set"};
  Gaudi::Property<bool> m_acceptGenericDaqError {
    this, "AcceptOnGenericDaqError", false, "Accept events with the hasGenericDaqError flag set"};
  Gaudi::Property<bool> m_acceptCrcTobError {
    this, "AcceptOnCrcTobError", false, "Accept events with the hasCrcTobError flag set"};
  Gaudi::Property<bool> m_acceptCrcFibreError {
    this, "AcceptOnCrcFibreError", false, "Accept events with the hasCrcFibreError flag set"};
  Gaudi::Property<bool> m_acceptCrcDaqError {
    this, "AcceptOnCrcDaqError", false, "Accept events with the hasCrcDaqError flag set"};
  Gaudi::Property<bool> m_acceptRoibDaqDifference {
    this, "AcceptOnRoibDaqDifference", false, "Accept events with the hasRoibDaqDifference flag set"};
  Gaudi::Property<bool> m_acceptRoibCtpDifference {
    this, "AcceptOnRoibCtpDifference", false, "Accept events with the hasRoibCtpDifference flag set"};
  Gaudi::Property<bool> m_acceptDaqCtpDifference {
    this, "AcceptOnDaqCtpDifference", false, "Accept events with the hasDaqCtpDifference flag set"};
};

#endif // L1TOPOONLINEMONITORING_L1TopoOnlineMonitorHypoTool_h
