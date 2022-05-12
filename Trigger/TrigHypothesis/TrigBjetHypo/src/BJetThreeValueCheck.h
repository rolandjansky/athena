/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BJetThreeValueCheck_h
#define BJetThreeValueCheck_h

#include "IBJetHypoDiscriminantCheck.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

class BJetThreeValueCheck: virtual public ::AthAlgTool, IBJetHypoDiscriminantCheck {
public:
  BJetThreeValueCheck(
    const std::string& type,
    const std::string& name,
    const IInterface* parent);
  virtual StatusCode initialize() override;
  virtual bool passThreshold(const SG::AuxElement&) const override;
private:
  struct Accessors {
    SG::AuxElement::ConstAccessor<float> b;
    SG::AuxElement::ConstAccessor<float> c;
    SG::AuxElement::ConstAccessor<float> u;
  };
  Gaudi::Property<std::string> m_bName { this, "b", "",
      "numerator name in flavor log likelihood"};
  Gaudi::Property<std::string> m_cName { this, "c", "",
    "first denominator name in flavor log likelihood"};
  Gaudi::Property<std::string> m_uName { this, "u", "",
    "second denominator name in flavor log likelihood"};
  Gaudi::Property<float> m_cFraction { this, "cFraction", 0,
    "weight for first denominator component"};
  Gaudi::Property<float> m_threshold { this, "threshold", NAN,
    "minimum value to pass"};
  std::unique_ptr<Accessors> m_acc;

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "",
    "Monitoring tool" };
  Gaudi::Property<std::string> m_llrName {this, "llrName", "btag_llr",
    "name to give log likelihood in monitoring"};
};

#endif
