/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Simple alg to wrap the selectiontool
//outputs a view container of the selected muons, if m_outputMuons different to m_inputMuons

//author: will buttinger <will@cern.ch>


#ifndef MUONSELECTORTOOLS_MUONSELECTIONALG
#define MUONSELECTORTOOLS_MUONSELECTIONALG

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

namespace CP {



class MuonSelectionAlg : public AthAlgorithm {

   public:
      MuonSelectionAlg( const std::string& name, ISvcLocator* svcloc);

      virtual StatusCode initialize();
      virtual StatusCode execute();

   private:
      std::string m_inputMuons;
      std::string m_outputMuons;
      ToolHandle<CP::IMuonSelectionTool> m_tool;

};

}

#endif
