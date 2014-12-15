// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TestElectronIsoAthena.h 622214 2014-10-16 22:10:48Z morrisj $
#ifndef ELECTRONISOLATIONSELECTION_TESTELECTRONISOATHENA_H
#define ELECTRONISOLATIONSELECTION_TESTELECTRONISOATHENA_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "ElectronIsolationSelection/IElectronIsolationSelectionTool.h"

namespace CP
{
  class TestElectronIsoAthena : public AthAlgorithm{
    public:
      TestElectronIsoAthena( const std::string& name, ISvcLocator* svcLoc );
      
      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();   
      
    private:
      std::string m_sgKey;
      ToolHandle< CP::IElectronIsolationSelectionTool > m_isoTool;
  };
}
#endif
