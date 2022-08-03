/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecisionMaker_TrigDecisionMakerValidator_H
#define TrigDecisionMaker_TrigDecisionMakerValidator_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

namespace TrigDec {

  /**
   * @class TrigDecisionMakerValidator
   * @breif Algorithm to perform sanity-checks on the output of the TrigDecisionMaker using the TrigDecisionTool
   */
  class TrigDecisionMakerValidator : public ::AthReentrantAlgorithm
  {

  public:

    TrigDecisionMakerValidator(const std::string &name, ISvcLocator *pSvcLocator);
    ~TrigDecisionMakerValidator();

    virtual StatusCode initialize() override;
    virtual StatusCode execute( const EventContext& context ) const override;

  private:

    StatusCode reportError(const std::string& item, const std::string& msg) const;

    PublicToolHandle<Trig::TrigDecisionTool> m_tdt {this, "TrigDecisionTool", "", "Handle to the public trigger decision tool."};

    Gaudi::Property<int> m_edmVersion{this, "EDMVersion", 3, "Is the input from the Run 1, 2 trigger or the Run 3 trigger?"};
    Gaudi::Property<int> m_samplingFrequence{this, "samplingFrequency", 10, "Sample one in every N events."};
    Gaudi::Property<bool> m_doL1{this, "doL1",  true, "Read & verify L1 trigger information."};
    Gaudi::Property<bool> m_doHLT{this, "doHLT", true, "Read & verify HLT trigger information."};
    Gaudi::Property<bool> m_errorOnFailure{this, "errorOnFailure", false, "If false, issues found are just WARNINGs rather than ERRORs."};

    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_navigationReadHandleKey{this, "NavigationKey", "For the Run 3 trigger, what is the primary navigation container?"};

  };
}

#endif
