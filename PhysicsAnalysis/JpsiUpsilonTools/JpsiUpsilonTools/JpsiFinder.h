/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// JpsiFinder header file
//
// James Catmore <James.Catmore@cern.ch>

// ----------------------------------------------------------------------------
// ****************************************************************************
#ifndef JPSIFINDER_H
#define JPSIFINDER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "HepPDT/ParticleDataTable.hh"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "JpsiUpsilonTools/ICandidateSearch.h"
#include "StoreGate/ReadHandleKey.h"

/////////////////////////////////////////////////////////////////////////////

namespace Trk {
    class IVertexFitter;
    class TrkV0VertexFitter;
    class ITrackSelectorTool;
}

namespace InDet { class VertexPointEstimator; }

namespace Analysis {
    
    static const InterfaceID IID_JpsiFinder("JpsiFinder", 1, 0);

    // Struct and enum to associate muon pairs with track pairs
    // and make the program flow more straightforward
    enum PairType{ MUMU=0, MUTRK=1, TRKTRK=2};
    enum MuonTypes{ CC=0, CT=1, TT=2};
    struct JpsiCandidate
    {
        const xAOD::TrackParticle* trackParticle1 = nullptr;
        const xAOD::TrackParticle* trackParticle2 = nullptr;
        const xAOD::Muon* muon1 = nullptr;
        const xAOD::Muon* muon2 = nullptr;
        const xAOD::TrackParticleContainer* collection1 = nullptr;
        const xAOD::TrackParticleContainer* collection2 = nullptr;
        PairType pairType{MUMU};
        MuonTypes muonTypes{CC};
    };
    
    class JpsiFinder:  public Analysis::ICandidateSearch, public AthAlgTool
    {
    public:
        JpsiFinder(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiFinder();
        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;
        
        static const InterfaceID& interfaceID() { return IID_JpsiFinder;}
        
        //-------------------------------------------------------------------------------------
        //Doing Calculation and inline functions

        virtual StatusCode performSearch(xAOD::VertexContainer*& vxContainer, xAOD::VertexAuxContainer*& vxAuxContainer) override;
        std::vector<JpsiCandidate> getPairs(const std::vector<const xAOD::TrackParticle*>&) const;
        std::vector<JpsiCandidate> getPairs(const std::vector<const xAOD::Muon*>&) const;
        std::vector<JpsiCandidate> getPairs2Colls(const std::vector<const xAOD::TrackParticle*>&, const std::vector<const xAOD::Muon*>&, bool) const;
        double getInvariantMass(const JpsiCandidate&, const std::vector<double>& ) const;
        std::vector<JpsiCandidate> selectCharges(const std::vector<JpsiCandidate>&) const;
        xAOD::Vertex* fit(const std::vector<const xAOD::TrackParticle*>&, const xAOD::TrackParticleContainer* importedTrackCollection) const;
        bool passesMCPCuts(const xAOD::Muon*) const;
        bool isContainedIn(const xAOD::TrackParticle*, const xAOD::TrackParticleContainer*) const;
        TVector3 trackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const;
        //-------------------------------------------------------------------------------------
        
    private:
        bool m_mumu;
        bool m_mutrk;
        bool m_trktrk;
        bool m_allMuons;
        bool m_combOnly;
        bool m_atLeastOneComb;
        bool m_useCombMeasurement;
        bool m_useV0Fitter;
        bool m_diMuons;
        double m_trk1M;
        double m_trk2M;
        const HepPDT::ParticleDataTable *m_particleDataTable;
        double m_thresholdPt;
        double m_higherPt;
        double m_trkThresholdPt;
        double m_invMassUpper;
        double m_invMassLower;
        double m_collAngleTheta;
        double m_collAnglePhi;
        double m_Chi2Cut;
        bool m_oppChOnly;
        bool m_sameChOnly;
        bool m_allChCombs;
        SG::ReadHandleKey<xAOD::MuonContainer> m_muonCollectionKey{this, "muonCollectionKey", "StacoMuonCollection"};
        SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrkParticleCollection {this, "TrackParticleCollection", "TrackParticleCandidate" };
        SG::ReadHandleKeyArray<xAOD::TrackParticleContainer> m_MuonTrackKeys;
        ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
        ToolHandle < Trk::IVertexFitter > m_iV0VertexFitter;
        ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
        ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
        bool m_mcpCuts;
        bool m_doTagAndProbe;
    };
} // end of namespace
#endif

