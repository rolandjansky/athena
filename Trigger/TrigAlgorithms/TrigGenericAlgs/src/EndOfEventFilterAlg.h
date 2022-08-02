/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGGENERICALGS_EndOfEventFilterAlg_h
#define TRIGGENERICALGS_EndOfEventFilterAlg_h

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

#include <vector>
#include <string>

/**
 * @class EndOfEventFilterAlg
 * @brief Filter alg passing if the chain given by the @c ChainName property is active
 *        (seeded and not prescaled) in a given event, and optionally if the event is
 *        accepted to at least one of the streams given by the @c StreamFilter property
 **/
class EndOfEventFilterAlg : public AthReentrantAlgorithm {
  public:
    EndOfEventFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& context) const override;

  private:
    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_hltSeedingSummaryKey {
      this, "HLTSeedingSummaryKey", "HLTSeedingSummary", "All chain decisions after L1 and prescaling"};

    SG::ReadHandleKey<std::vector<std::string>> m_streamsSummaryKey{
      this, "StreamsSummaryKey", "HLTStreamsSummary", "Set of streams accepted in the event"};

    Gaudi::Property<std::string> m_chainName {
      this, "ChainName", "", "Name of the chain to check the prescale"};

    Gaudi::Property<std::vector<std::string>> m_streamFilter {
      this, "StreamFilter", {}, "Require event to be accepted to at least one of the listed streams "
      "for this filter alg to pass. Empty list means no requirement."};

    HLT::Identifier m_chainId{0};
};

#endif // TRIGGENERICALGS_EndOfEventFilterAlg_h
