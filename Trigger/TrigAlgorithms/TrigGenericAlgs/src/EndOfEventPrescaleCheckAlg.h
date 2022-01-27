/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGGENERICALGS_EndOfEventPrescaleCheckAlg_h
#define TRIGGENERICALGS_EndOfEventPrescaleCheckAlg_h

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

/**
 * @class EndOfEventPrescaleCheckAlg
 * @brief Filter alg passing if the chain given by the @c ChainName property is active
 *        (seeded and not prescaled) in a given event
 **/
class EndOfEventPrescaleCheckAlg : public AthReentrantAlgorithm {
  public:
    EndOfEventPrescaleCheckAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& context) const override;

  private:
    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_hltSeedingSummaryKey {
      this, "HLTSeedingSummaryKey", "HLTSeedingSummary", "All chain decisions after L1 and prescaling"};

    Gaudi::Property<std::string> m_chainName {
      this, "ChainName", "", "Name of the chain to check the prescale"};

    HLT::Identifier m_chainId{0};
};

#endif // TRIGGENERICALGS_EndOfEventPrescaleCheckAlg_h
