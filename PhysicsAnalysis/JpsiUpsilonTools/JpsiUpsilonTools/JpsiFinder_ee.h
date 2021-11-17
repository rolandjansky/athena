/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// JpsiFinder_ee header file
//
// James Catmore <James.Catmore@cern.ch>

// ----------------------------------------------------------------------------
// ****************************************************************************
#ifndef JpsiFinder_ee_H
#define JpsiFinder_ee_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "HepPDT/ParticleDataTable.hh"

#include "xAODEgamma/Electron.h"

#include <vector>
#include <string>
#include "JpsiUpsilonTools/ICandidateSearch.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
/////////////////////////////////////////////////////////////////////////////

namespace Trk {
    class IVertexFitter;
    class TrkV0VertexFitter;
    class ITrackSelectorTool;
}

namespace InDet { class VertexPointEstimator; }

namespace Analysis {
    
    static const InterfaceID IID_JpsiFinder_ee("JpsiFinder_ee", 1, 0);

    // Struct and enum to associate muon pairs with track pairs
    // and make the program flow more straightforward
    enum PairTypeEE{ ELEL=0, ELTRK=1, TRK2=2};
    struct JpsiEECandidate
    {
        const xAOD::TrackParticle* trackParticle1{nullptr};
        const xAOD::TrackParticle* trackParticle2{nullptr};
        const xAOD::Electron* el1{nullptr};
        const xAOD::Electron* el2{nullptr};
        const xAOD::TrackParticleContainer* collection1{nullptr};
        const xAOD::TrackParticleContainer* collection2{nullptr};
        PairTypeEE pairType=PairTypeEE::ELEL;
    };
    
    class JpsiFinder_ee:  public Analysis::ICandidateSearch, public AthAlgTool
    {
    public:
        JpsiFinder_ee(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiFinder_ee();
        virtual StatusCode initialize() override;
        
        static const InterfaceID& interfaceID() { return IID_JpsiFinder_ee;}
        
        //-------------------------------------------------------------------------------------
        //Doing Calculation and inline functions
        virtual StatusCode performSearch(const EventContext& ctx, xAOD::VertexContainer& vxContainer) const override;
        std::vector<JpsiEECandidate> getPairs(const std::vector<const xAOD::TrackParticle*>&) const;
        std::vector<JpsiEECandidate> getPairs(const std::vector<const xAOD::Electron*>&) const;
        std::vector<JpsiEECandidate> getPairs2Colls(const std::vector<const xAOD::TrackParticle*>&, const std::vector<const xAOD::Electron*>&, bool) const;
        double getInvariantMass(const JpsiEECandidate&, const std::vector<double>& ) const;
        std::vector<JpsiEECandidate> selectCharges(const std::vector<JpsiEECandidate>& , const std::string&) const;
        xAOD::Vertex* fit(const std::vector<const xAOD::TrackParticle*>&, const xAOD::TrackParticleContainer* importedTrackCollection) const;
        bool passesEgammaCuts(const xAOD::Electron*) const;
        bool isContainedIn(const xAOD::TrackParticle*, const xAOD::TrackParticleContainer*) const;
        TVector3 trackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const;
        //-------------------------------------------------------------------------------------
        
    private:
        bool m_elel;
        bool m_eltrk;
        bool m_trktrk;
        bool m_allElectrons;
        bool m_useTrackMeasurement;
        bool m_useV0Fitter;
        bool m_diElectrons;
        double m_trk1M;
        double m_trk2M;
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
        SG::ReadHandleKey<xAOD::ElectronContainer>      m_electronCollectionKey;
        SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrkParticleCollection;
        ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
        ToolHandle < Trk::IVertexFitter > m_iV0VertexFitter;
        ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
        ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
        bool m_egammaCuts;
        std::string m_elSelection;
        bool m_doTagAndProbe;
    };
} // end of namespace
#endif

