/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigPartialEventBuilding_PEBInfoWriterToolBase_h
#define TrigPartialEventBuilding_PEBInfoWriterToolBase_h

// Trigger includes
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

/** @class PEBInfoWriterToolBase
 *  @brief Base class for tools used by PEBInfoWriterAlg
 * 
 *  Derived classes have to implement \c createPEBInfo, which is used in \c decide
 **/
class PEBInfoWriterToolBase : public AthAlgTool {
public:
  // ------------------------- Public types ------------------------------------
  /// Input to the tool's \c decide method
  struct Input {
    Input(TrigCompositeUtils::Decision* d,
          const EventContext& ctx,
          const TrigRoiDescriptor* r,
          const TrigCompositeUtils::Decision* pd)
    : decision(d),
      eventContext(ctx),
      roi(r),
      previousDecisionIDs(TrigCompositeUtils::decisionIDs(pd).begin(), TrigCompositeUtils::decisionIDs(pd).end()) {}
    TrigCompositeUtils::Decision* decision;
    const EventContext& eventContext;
    const TrigRoiDescriptor* roi;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };
  /// Structure holding the list of ROBs and SubDets
  struct PEBInfo {
    std::set<uint32_t> robs;
    std::set<uint32_t> subdets; // uint8_t is sufficient to represent eformat::SubDetector, but uint32_t makes the
                                // implementation and bookkeeping easier
  };

  // ------------------------- Public methods ----------------------------------
  /// Standard constructor
  PEBInfoWriterToolBase(const std::string& type, const std::string& name, const IInterface* parent);
  /// Standard destructor
  virtual ~PEBInfoWriterToolBase();

  /// Adds a pass-through decision with attached PEB information created by \c createPEBInfo for each input
  StatusCode decide(std::vector<Input>& inputs) const;
  /// Returns the decision id
  HLT::Identifier id() const {return m_decisionId;}
  /// Returns the key used to record/retrieve the ROB list
  static std::string robListKey() {return "PEBROBList";}
  /// Returns the key used to record/retrieve the SubDet list
  static std::string subDetListKey() {return "PEBSubDetList";}

protected:
  /// Creates the PEBInfo which is attached to the decision in \c decide. Has to be implemented by the derived class.
  virtual PEBInfo createPEBInfo(const Input& input) const = 0;

private:
  /// The decision id of the tool instance
  HLT::Identifier m_decisionId;
};

std::ostream& operator<< (std::ostream& str, const PEBInfoWriterToolBase::PEBInfo& pebi);

#endif // TrigPartialEventBuilding_PEBInfoWriterToolBase_h
