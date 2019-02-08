/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************
// ----------------------------------------------------------------------------
// JpsiPlus2Tracks header file
//
// James Catmore <James.Catmore@cern.ch>

// ----------------------------------------------------------------------------
// ****************************************************************************
#ifndef JPSIPLUS2TRACKS_H
#define JPSIPLUS2TRACKS_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include <vector>
#include <string>
#include "JpsiUpsilonTools/ICandidateSearch.h"
/////////////////////////////////////////////////////////////////////////////

namespace Trk {
    class IVertexFitter;
    class VxTrackAtVertex;
    class ITrackSelectorTool;
}
namespace InDet { class VertexPointEstimator; }

namespace xAOD{
   class BPhysHelper;
}

namespace Analysis {
    
    static const InterfaceID IID_JpsiPlus2Tracks("JpsiPlus2Tracks", 1, 0);
    
    class JpsiPlus2Tracks:  public Analysis::ICandidateSearch, public AthAlgTool
    {
    public:
        JpsiPlus2Tracks(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiPlus2Tracks();
        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;
        
        static const InterfaceID& interfaceID() { return IID_JpsiPlus2Tracks;};
        
        //-------------------------------------------------------------------------------------
        //Doing Calculation and inline functions
        virtual StatusCode performSearch(xAOD::VertexContainer*& , xAOD::VertexAuxContainer*& ) override;

        static double getInvariantMass(const xAOD::TrackParticle*, double, const xAOD::TrackParticle*, double);
        static double getInvariantMass(const std::vector<const xAOD::TrackParticle*> &trk, const std::vector<double>&);
        static bool   oppositeCharges(const xAOD::TrackParticle*, const xAOD::TrackParticle*);

        bool  passCuts(xAOD::BPhysHelper &bHelper, const std::vector<double> &masses, const std::string &str) const;
        bool  vertexCuts(xAOD::BPhysHelper &bHelper) const;
        xAOD::Vertex* fit(const std::vector<const xAOD::TrackParticle*>&,
                          const xAOD::TrackParticleContainer*, const xAOD::Vertex* pv, const xAOD::TrackParticleContainer* GSL) const;
        //-------------------------------------------------------------------------------------
        
    private:
        bool m_pipiMassHyp;
        bool m_kkMassHyp;
        bool m_kpiMassHyp;
        bool m_kpMassHyp;
        bool m_oppChargesOnly;
        double m_trkThresholdPt;
        double m_trkMaxEta;
        double m_BThresholdPt;
        double m_BMassUpper;
        double m_BMassLower;
        std::string m_jpsiCollectionKey;
        double m_jpsiMassUpper;
        double m_jpsiMassLower;
        std::string m_TrkParticleCollection;
        std::string m_TrkParticleGSFCollection;
        std::string m_MuonsUsedInJpsi;
        bool m_excludeJpsiMuonsOnly; //Add by Matt Klein
        bool m_excludeCrossJpsiTracks; //Added by Matteo Bedognetti
        ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
        ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
        ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
        Trk::TrkVKalVrtFitter* m_VKVFitter;
        bool m_useMassConst;
        double m_altMassConst;
        double m_diTrackMassUpper;
        double m_diTrackMassLower;
        
        // additional cuts (Daniel Scheirich)
        double m_chi2cut;                 // chi2/Ndof of the final veretx
        double m_diTrackPt;               // pT of the hadronic track pair before fit
        double m_trkQuadrupletMassUpper;  // invariant mass of all the 4 tracks before fit
        double m_trkQuadrupletMassLower;
        double m_trkQuadrupletPt;         // combined pT of all 4 tracks  before fit
        double m_finalDiTrackMassUpper;   // inveriant mass of the hadronic track pair after the fit
        double m_finalDiTrackMassLower;
        double m_finalDiTrackPt;          // pT of the hadronic track after fit
        double m_trkDeltaZ;               // DeltaZ between the JPsi vertex and hadronic tracks Z0
        // (to reduce the pileup contribution before vertexing)
        std::vector<double> m_manualMassHypo;
        int m_requiredNMuons;
        // fit with PV
        bool m_vertexFittingWithPV;
        std::string m_PVerticesCollection;
        std::vector<double> m_altMassMuonTracks;
        std::vector<double>  m_mumukkMasses;
        std::vector<double>  m_mumupipiMasses;
        std::vector<double>  m_mumukpiMasses;
        std::vector<double>  m_mumupikMasses;
        std::vector<double>  m_mumukpMasses;
        std::vector<double>  m_mumupkMasses;
        std::vector<int>     m_useGSFTrackIndices;
        std::bitset<4>       m_useGSFTrack;

    };
} // end of namespace
#endif
