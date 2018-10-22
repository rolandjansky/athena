/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERING_RESULTBUILDERMT_H
#define TRIGSTEERING_RESULTBUILDERMT_H

#include "TrigSteeringEvent/HLTResultMT.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/WriteHandle.h"

namespace HLT {
  /** @class ResultBuilderMT
   *  @brief Tool to create the HLTResultMT at the end of each event
   **/
  class ResultBuilderMT : public AthAlgTool {
  public:
    /// Standard constructor
    ResultBuilderMT(const std::string& type, const std::string& name, const IInterface* parent);
    /// Standard destructor
    virtual ~ResultBuilderMT();

    // ------------------------- IStateful methods -----------------------------
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    // ------------------------- Specific methods of this tool -----------------
    /// Builds the HLTResultMT and records it in the event store
    StatusCode buildResult(const EventContext& eventContext) const;
    /// Return name of the HLTResultMT
    const std::string& resultName() const {return m_hltResultWHKey.key();}

  private:
    /// StoreGate key for the HLTResultMT
    SG::WriteHandleKey<HLT::HLTResultMT> m_hltResultWHKey;
  };
} // namespace HLT

#endif // TRIGSTEERING_RESULTBUILDERMT_H
