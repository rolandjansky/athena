/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSELECTORTOOLS_MUONQUALITYUPDATERALG
#define MUONSELECTORTOOLS_MUONQUALITYUPDATERALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

namespace CP {



  class MuonQualityUpdaterAlg : public AthAlgorithm {

  public:
    MuonQualityUpdaterAlg( const std::string& name, ISvcLocator* svcloc);

    virtual StatusCode initialize();
    virtual StatusCode execute();

  private:
    std::string m_input_muons;
    std::string m_output_muons;
    ToolHandle<CP::IMuonSelectionTool> m_tool;

  };

}

#endif
