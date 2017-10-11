/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_TOOLTESTER_H
#define MCAST_TOOLTESTER_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

// Root include(s)
#include "TTree.h"
#include "TFile.h"

namespace CP {

class TestMCASTTool : public AthAlgorithm {

public:
  //::: Regular Algorithm constructor
  TestMCASTTool( const std::string& name, ISvcLocator* svcLoc );
  //::: Function initialising the algorithm
  virtual StatusCode initialize();
  //::: Function executing the algorithm
  virtual StatusCode execute();
  //::: Function finalizing the algoritm
  virtual StatusCode finalize();

private:
  //::: StoreGate key for the muon container to investigate
  std::string m_sgKey;
  //::: Connection to the smearing tool
  ToolHandle< CP::IMuonCalibrationAndSmearingTool > m_Tool;

  TFile* m_smearfile;
  TTree* m_smeartree;
  Float_t m_eta, m_phi, m_pt, m_ptcorr, m_ptdiff, m_ptdiffabs, m_ptid, m_ptms;


}; // class TestMCASTTool

} // namespace CP

#endif // MCAST_TOOLTESTER_H
