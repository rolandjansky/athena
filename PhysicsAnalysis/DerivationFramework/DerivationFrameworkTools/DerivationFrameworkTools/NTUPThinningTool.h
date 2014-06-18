/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NTUPThinningTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_NTUPTHINNINGTOOL_H
#define DERIVATIONFRAMEWORK_NTUPTHINNINGTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

namespace DerivationFramework {

  class NTUPThinningTool : public AthAlgTool, public IThinningTool {
    public: 
      NTUPThinningTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      std::string m_expression;
      ExpressionParsing::ExpressionParser *m_parser;
      std::string m_suffix;
      std::vector<std::string> m_branches;
      template<typename T> void thinner(std::vector<T>*,std::vector<int>,std::vector<T>*&) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_NTUPTHINNINGTOOL_H
