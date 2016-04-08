/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericObjectThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_GENERICOBJECTTHINNING_H
#define DERIVATIONFRAMEWORK_GENERICOBJECTTHINNING_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODBase/IParticleContainer.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

class IThinningSvc;

namespace DerivationFramework {

  class GenericObjectThinning : public AthAlgTool, public IThinningTool {
    public: 
      GenericObjectThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~GenericObjectThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      //Main thinning service
      ServiceHandle<IThinningSvc> m_thinningSvc;

      //Expression for object thinning selection
      std::string m_expression;
      ExpressionParsing::ExpressionParser *m_parser;
      std::string m_selectionString;

      //Counters and key
      mutable unsigned int m_ntot, m_npass;
      std::string m_SGKey;

      //logic
      bool m_and;

  }; 
}

#endif // DERIVATIONFRAMEWORK_GENERICOBJECTTHINNING_H
