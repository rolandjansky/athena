/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EventInfoPixelDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EVENTINFOPIXELDECORATOR_H
#define DERIVATIONFRAMEWORK_EVENTINFOPIXELDECORATOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

namespace DerivationFramework {

  class EventInfoPixelDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      EventInfoPixelDecorator(const std::string& type, const std::string& name, const IInterface* parent);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_expression;
      ExpressionParsing::ExpressionParser *m_parser;
      std::string m_selectionString;

      mutable unsigned int m_ntot;
      mutable unsigned int m_npass;
    
      std::string m_eventInfoKey;
      std::string m_decorationPrefix;
      std::string m_pixelKey;
  }; 
}

#endif // DERIVATIONFRAMEWORK_EVENTINFOPIXELDECORATOR_H
