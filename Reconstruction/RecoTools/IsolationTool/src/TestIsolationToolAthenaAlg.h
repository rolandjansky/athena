// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONTOOL_TESTISOLATIONTOOLATHENAALG_H
#define ISOLATIONTOOL_TESTISOLATIONTOOLATHENAALG_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "IsolationTool/IIsolationHelper.h"

namespace CP
{
  class TestIsolationToolAthenaAlg : public AthAlgorithm{
    public:
      TestIsolationToolAthenaAlg( const std::string& name, ISvcLocator* svcLoc );
      
      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();   
      
    private:
      ToolHandle< CP::IIsolationHelper > m_isoHelper;
  };
}
#endif
