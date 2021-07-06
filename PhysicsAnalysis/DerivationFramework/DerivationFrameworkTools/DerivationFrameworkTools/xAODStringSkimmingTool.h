/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// xAODStringSkimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_XAODSTRINGSKIMMINGTOOL_H
#define DERIVATIONFRAMEWORK_XAODSTRINGSKIMMINGTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

#include "ExpressionEvaluation/ExpressionParserUserWithTrigSupport.h"

namespace DerivationFramework {

  class xAODStringSkimmingTool : public ExpressionParserUserWithTrigSupport<AthAlgTool>, public ISkimmingTool {
    public: 
      xAODStringSkimmingTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual bool eventPassesFilter() const;
    private:
      std::string m_expression;
  };
}

#endif // DERIVATIONFRAMEWORK_XAODSTRINGSKIMMINGTOOL_H
