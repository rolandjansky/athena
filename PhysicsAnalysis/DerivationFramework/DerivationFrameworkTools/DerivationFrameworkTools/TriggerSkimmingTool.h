/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TriggerSkimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRIGGERSKIMMINGTOOL_H
#define DERIVATIONFRAMEWORK_TRIGGERSKIMMINGTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

namespace DerivationFramework {

  class TriggerSkimmingTool : public AthAlgTool, public ISkimmingTool {
    public: 
      TriggerSkimmingTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual bool eventPassesFilter() const;

    private:
      std::vector<std::string> m_triggerListAND;
      std::vector<std::string> m_triggerListOR;
      ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  }; 
}

#endif // DERIVATIONFRAMEWORK_TRIGGERSKIMMINGTOOL_H
