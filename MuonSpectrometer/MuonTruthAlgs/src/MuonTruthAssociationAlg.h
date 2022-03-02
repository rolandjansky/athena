/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHPARTICLEALGS_MUONTRUTHASSOCIATIONALG_H
#define TRUTHPARTICLEALGS_MUONTRUTHASSOCIATIONALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "StoreGate/ReadDecorHandleKeyArray.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

class MuonTruthAssociationAlg : public AthReentrantAlgorithm {
public:
    // Constructor with parameters:
    MuonTruthAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator);

    // Basic algorithm methods:
    StatusCode initialize() override;
    StatusCode execute(const EventContext& ctx) const override;

private:
   SG::ReadHandleKey<xAOD::TruthParticleContainer> m_muonTruthParticleContainerName{
        this, "MuonTruthParticleContainerName", "MuonTruthParticles",
        "container name for muon truth particles; the full handle name, including the reco muon link auxdata, is set in initialize()"};
   
    SG::WriteDecorHandleKey<xAOD::TruthParticleContainer> m_muonTruthRecoLink{
        this, "MuonTruthParticleRecoLink", "",
        "container name for muon truth particles; the full handle name, including the reco muon link auxdata, is set in initialize()"};
    SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonTruthParticleLink{
        this, "MuonTruthParticleLink", "Muons.truthParticleLink",
        "muon truth particle link auxdata name; name will be reset in initialize() based on m_muonName"};
    SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonTruthParticleType{
        this, "MuonTruthParticleType", "Muons.truthType",
        "muon truth type auxdata name; name will be reset in initialize() based on m_muonName"};
    SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonTruthParticleOrigin{
        this, "MuonTruthParticleOrigin", "Muons.truthOrigin",
        "muon truth origin auxdata name; name will be reset in initialize() based on m_muonName"};
    SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonTruthParticleNPrecMatched{
        this, "MuonTruthParticleNPrecMatched", "Muons.nprecMatchedHitsPerChamberLayer",
        "muon vector of number of precision matched hits per chamber layer auxdata name; name will be reset in initialize() based on "
        "m_muonName"};
    SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonTruthParticleNPhiMatched{
        this, "MuonTruthParticleNPhiMatched", "Muons.nphiMatchedHitsPerChamberLayer",
        "muon vector of number of phi matched hits per chamber layer auxdata name; name will be reset in initialize() based on m_muonName"};
    SG::WriteDecorHandleKey<xAOD::MuonContainer> m_muonTruthParticleNTrigEtaMatched{
        this, "MuonTruthParticleNTrigEtaMatched", "Muons.ntrigEtaMatchedHitsPerChamberLayer",
        "muon vector of number of phi matched hits per chamber layer auxdata name; name will be reset in initialize() based on m_muonName"};
    
   
    SG::ReadDecorHandleKeyArray<xAOD::TrackParticleContainer> m_trkTruthKeys{
        this, "TrkTruthLinkKeys", {},
        "Declare the decoration dependencies of this algorithm. Steer via TrackContainers property"};
    Gaudi::Property<std::string> m_muonName{this, "MuonContainerName", "Muons", "muon container name"};
    Gaudi::Property<std::vector<std::string>> m_assocTrkContainers{this, "TrackContainers", {"CombinedMuonTrackParticles",
                                                                                            "ExtrapolatedMuonTrackParticles",
                                                                                            "InDetTrackParticles",
                                                                                            "MSOnlyExtrapolatedMuonTrackParticles"}, 
                        "Collection of track containers to be decorated before this alg can be scheduled. truthLink decoration exploited "};
    Gaudi::Property<std::string> m_recoLink{this, "RecoLinkName", "recoMuonLink" , "Decoration to the truth particle pointing to the muon"};
    Gaudi::Property<bool> m_associateWithInDetTP{this, "AssociateWithInDetTP", false, "force use of ID track particles for association"};
    Gaudi::Property<int> m_barcodeOffset{this, "BarcodeOffset", 1000000, "barcode offset for truth particles"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    void count_chamber_layers(const xAOD::IParticle* truth_particle, const Trk::Track* ptrk,
                              std::vector<unsigned int>& nprecHitsPerChamberLayer, std::vector<unsigned int>& nphiHitsPerChamberLayer,
                              std::vector<unsigned int>& ntrigEtaHitsPerChamberLayer) const;
    void clear_dummys(const std::vector<unsigned long long>& identifiers, std::vector<unsigned int>& vec) const;
  
};

#endif  // TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H
