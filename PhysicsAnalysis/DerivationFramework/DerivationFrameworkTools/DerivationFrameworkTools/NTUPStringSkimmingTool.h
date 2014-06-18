/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NTUPStringSkimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_NTUPSTRINGSKIMMINGTOOL_H
#define DERIVATIONFRAMEWORK_NTUPSTRINGSKIMMINGTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

namespace DerivationFramework {

  class NTUPStringSkimmingTool : public AthAlgTool, public ISkimmingTool {
    public: 
      NTUPStringSkimmingTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual bool eventPassesFilter() const;

    private:
      std::string m_expression;
      ExpressionParsing::ExpressionParser *m_parser;
  }; 
}

#endif // DERIVATIONFRAMEWORK_NTUPSTRINGSKIMMINGTOOL_H
