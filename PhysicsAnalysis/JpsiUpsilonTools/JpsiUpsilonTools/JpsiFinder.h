/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "DataModel/DataVector.h"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "HepPDT/ParticleDataTable.hh"

#include "xAODMuon/Muon.h"

#include <vector>
#include <cmath>
#include <string>
#include <map>
/////////////////////////////////////////////////////////////////////////////

namespace Trk {
    class IVertexFitter;
//    class VxCandidate;
//    class TrackParticleBase;
//    class VxTrackAtVertex;
//    class RecVertex;
    class TrkV0VertexFitter;
    class ITrackSelectorTool;
    class V0Tools;
//    class ExtendedVxCandidate;
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
        const xAOD::TrackParticle* trackParticle1;
        const xAOD::TrackParticle* trackParticle2;
        const xAOD::Muon* muon1;
        const xAOD::Muon* muon2;
        const xAOD::TrackParticleContainer* collection1;
        const xAOD::TrackParticleContainer* collection2;
        PairType pairType;
        MuonTypes muonTypes;
    };
    
    class JpsiFinder:  virtual public AthAlgTool
    {
    public:
        JpsiFinder(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiFinder();
        StatusCode initialize();
        StatusCode finalize();
        
        static const InterfaceID& interfaceID() { return IID_JpsiFinder;}
        
        //-------------------------------------------------------------------------------------
        //Doing Calculation and inline functions
        StatusCode performSearch(xAOD::VertexContainer*& vxContainer, xAOD::VertexAuxContainer*& vxAuxContainer);
        std::vector<JpsiCandidate> getPairs(const std::vector<const xAOD::TrackParticle*>&);
        std::vector<JpsiCandidate> getPairs(const std::vector<const xAOD::Muon*>&);
        std::vector<JpsiCandidate> getPairs2Colls(const std::vector<const xAOD::TrackParticle*>&, const std::vector<const xAOD::Muon*>&, bool);
        double getInvariantMass(const JpsiCandidate&, const std::vector<double>& );
        std::vector<JpsiCandidate> selectCharges(const std::vector<JpsiCandidate>& , const std::string&);
        xAOD::Vertex* fit(const std::vector<const xAOD::TrackParticle*>&, const xAOD::TrackParticleContainer* importedTrackCollection);
        bool passesMCPCuts(const xAOD::Muon*);
        bool isContainedIn(const xAOD::TrackParticle*, const xAOD::TrackParticleContainer*);
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
        std::string m_muonCollectionKey;
        std::string m_TrkParticleCollection;
        std::vector<std::string> m_MuonTrackKeys;
        ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
        ToolHandle < Trk::IVertexFitter > m_iV0VertexFitter;
        ToolHandle < Trk::V0Tools > m_V0Tools;
        ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
        ToolHandle < InDet::ConversionFinderUtils > m_helpertool;//unused remove later
        ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
        bool m_mcpCuts;
        bool m_doTagAndProbe;
    };
} // end of namespace
#endif

