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

#include <vector>
#include <cmath>
#include <string>
#include <map>
/////////////////////////////////////////////////////////////////////////////

namespace Trk {
    class IVertexFitter;
    class VxCandidate;
    class TrackParticleBase;
    class VxTrackAtVertex;
    class RecVertex;
    class TrkV0VertexFitter;
    class ITrackSelectorTool;
    class V0Tools;
    class ExtendedVxCandidate;
}
namespace Rec { class TrackParticle; }
namespace InDet { class VertexPointEstimator; }
namespace Analysis { class Muon; }

namespace Analysis {
    
    static const InterfaceID IID_JpsiFinder("JpsiFinder", 1, 0);

    // Struct and enum to associate muon pairs with track pairs
    // and make the program flow more straightforward
    enum PairType{ MUMU=0, MUTRK=1, TRKTRK=2};
    enum MuonTypes{ CC=0, CT=1, TT=2};
    struct JpsiCandidate
    {
        const Rec::TrackParticle* trackParticle1;
        const Rec::TrackParticle* trackParticle2;
        const Analysis::Muon* muon1;
        const Analysis::Muon* muon2;
        const Trk::TrackParticleBaseCollection* collection1;
        const Trk::TrackParticleBaseCollection* collection2;
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
        
        static const InterfaceID& interfaceID() { return IID_JpsiFinder;};
        
        //-------------------------------------------------------------------------------------
        //Doing Calculation and inline functions
        int nGoodPairs(void) {return m_nGoodPairs;}
        std::vector<std::vector<const Rec::TrackParticle*> > getGoodPairs(void) {return m_goodPairs;}
        VxContainer* performSearch();
        std::vector<JpsiCandidate> getPairs(std::vector<const Rec::TrackParticle*>);
        std::vector<JpsiCandidate> getPairs(std::vector<const Analysis::Muon*>);
        std::vector<JpsiCandidate> getPairs2Colls(std::vector<const Rec::TrackParticle*>,std::vector<const Analysis::Muon*>, bool);
        double getPt(const Rec::TrackParticle*);
        double getEta(const Rec::TrackParticle*);
        double getInvariantMass(JpsiCandidate,std::vector<double> );
        std::vector<JpsiCandidate> selectCharges(std::vector<JpsiCandidate> , std::string);
        Trk::VxCandidate* fit(std::vector<const Rec::TrackParticle*>);
        bool passesMCPCuts(const Rec::TrackParticle*);
        bool isContainedIn(const Trk::TrackParticleBase*, const Trk::TrackParticleBaseCollection*);
        bool isContainedIn(const Rec::TrackParticle*, std::vector<const Rec::TrackParticle*>);
        
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
        int m_nGoodPairs;
        std::vector<std::vector<const Rec::TrackParticle*> > m_goodPairs;
        std::string m_muonCollectionKey;
        std::string m_TrkParticleCollection;
        std::vector<std::string> m_MuonTrackKeys;
        ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
        ToolHandle < Trk::IVertexFitter > m_iV0VertexFitter;
        ToolHandle < Trk::V0Tools > m_V0Tools;
        ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
        ToolHandle < InDet::ConversionFinderUtils > m_helpertool;
        ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
        bool m_mcpCuts;
        bool m_doTagAndProbe;
    };
} // end of namespace
#endif

