/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTruthAssociationAlg.h
//   Header file for class MuonTruthAssociationAlg
///////////////////////////////////////////////////////////////////

#ifndef TRUTHPARTICLEALGS_MUONTRUTHASSOCIATIONALG_H
#define TRUTHPARTICLEALGS_MUONTRUTHASSOCIATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

class MuonTruthAssociationAlg : public AthAlgorithm  {

public:
  // Constructor with parameters:
  MuonTruthAssociationAlg(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  void addMuon( const xAOD::TruthParticleContainer& truthParticles, const xAOD::MuonContainer& muons );

  void setDummyTruthInfo(const xAOD::Muon& muon);

  std::string m_muonTruthParticleContainerName;
  std::string m_muonName;

  bool m_associateWithInDetTP;
  int m_barcodeOffset;

  ToolHandle<Muon::MuonIdHelperTool>    m_idHelper;
};


#endif //TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H
