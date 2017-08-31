/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSegmentTruthAssociationAlg.h
//   Header file for class MuonSegmentTruthAssociationAlg
///////////////////////////////////////////////////////////////////

#ifndef TRUTHPARTICLEALGS_MUONSEGMENTTRUTHASSOCIATION_H
#define TRUTHPARTICLEALGS_MUONSEGMENTTRUTHASSOCIATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMuonTrackTruthTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "xAODMuon/MuonSegmentContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"

namespace Muon {

class MuonSegmentTruthAssociationAlg : public AthAlgorithm  {

public:
  // Constructor with parameters:
  MuonSegmentTruthAssociationAlg(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  ToolHandle<Muon::MuonIdHelperTool>    m_idHelper;
  ToolHandle<Muon::MuonEDMPrinterTool>  m_printer;
  ToolHandle<Muon::IMuonTrackTruthTool> m_muonTrackTruthTool;
  SG::WriteDecorHandleKey<xAOD::MuonSegmentContainer> m_muonTruthSegmentContainerName;
  SG::WriteDecorHandleKey<xAOD::MuonSegmentContainer> m_muonSegmentCollectionName;
  int m_barcodeOffset;
};

} // namespace Muon


#endif //TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H
