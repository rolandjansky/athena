/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BJetTwoValueCheck_h
#define BJetTwoValueCheck_h

#include "IBJetHypoDiscriminantCheck.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

class BJetTwoValueCheck: virtual public ::AthAlgTool, IBJetHypoDiscriminantCheck {
public:
  BJetTwoValueCheck(
    const std::string& type,
    const std::string& name,
    const IInterface* parent);
  virtual StatusCode initialize() override;
  virtual bool passThreshold(const SG::AuxElement&) const override;
private:
  struct Accessors {
    SG::AuxElement::ConstAccessor<float> n;
    SG::AuxElement::ConstAccessor<float> d;
  };
  Gaudi::Property<std::string> m_numName { this, "numerator", "",
      "numerator name in flavor log likelihood"};
  Gaudi::Property<std::string> m_denomName { this, "denominator", "",
    "denominator name in flavor log likelihood"};
  Gaudi::Property<float> m_threshold { this, "threshold", NAN,
    "minimum value to pass"};
  std::unique_ptr<Accessors> m_acc;

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "",
    "Monitoring tool" };
  Gaudi::Property<std::string> m_llrName {this, "llrName", "bbtag_llr",
    "name to give log likelihood in monitoring"};
};

#endif
