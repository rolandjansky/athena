/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHPARTICLEALGS_MUONTRUTHASSOCIATIONALG_H
#define TRUTHPARTICLEALGS_MUONTRUTHASSOCIATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class MuonTruthAssociationAlg : public AthAlgorithm  {

public:
  // Constructor with parameters:
  MuonTruthAssociationAlg(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::TruthParticleContainer> >m_muonTruthParticleContainerName{this,"MuonTruthParticleContainerName","MuonTruthParticles","container name for muon truth particles; the full handle name, including the reco muon link auxdata, is set in initialize()"};
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::MuonContainer> >m_muonTruthParticleLink{this,"MuonTruthParticleLink","Muons.truthParticleLink","muon truth particle link auxdata name; name will be reset in initialize() based on m_muonName"};
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::MuonContainer> >m_muonTruthParticleType{this,"MuonTruthParticleType","Muons.truthType","muon truth type auxdata name; name will be reset in initialize() based on m_muonName"};
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::MuonContainer> >m_muonTruthParticleOrigin{this,"MuonTruthParticleOrigin","Muons.truthOrigin","muon truth origin auxdata name; name will be reset in initialize() based on m_muonName"};
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::MuonContainer> >m_muonTruthParticleNPrecMatched{this,"MuonTruthParticleNPrecMatched","Muons.nprecMatchedHitsPerChamberLayer","muon vector of number of precision matched hits per chamber layer auxdata name; name will be reset in initialize() based on m_muonName"};
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::MuonContainer> >m_muonTruthParticleNPhiMatched{this,"MuonTruthParticleNPhiMatched","Muons.nphiMatchedHitsPerChamberLayer","muon vector of number of phi matched hits per chamber layer auxdata name; name will be reset in initialize() based on m_muonName"};
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::MuonContainer> >m_muonTruthParticleNTrigEtaMatched{this,"MuonTruthParticleNTrigEtaMatched","Muons.ntrigEtaMatchedHitsPerChamberLayer","muon vector of number of phi matched hits per chamber layer auxdata name; name will be reset in initialize() based on m_muonName"};
  Gaudi::Property<std::string> m_muonName{this,"MuonContainerName","Muons","muon container name"};
  Gaudi::Property<bool>m_associateWithInDetTP{this,"AssociateWithInDetTP",false,"force use of ID track particles for association"};
  Gaudi::Property<int>m_barcodeOffset{this,"BarcodeOffset",1000000 ,"barcode offset for truth particles"};

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
};


#endif //TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H
