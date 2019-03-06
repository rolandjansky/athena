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
#ifndef JPSIPLUS1TRACK_H
#define JPSIPLUS1TRACK_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include <vector>
#include <string>
#include "JpsiUpsilonTools/ICandidateSearch.h"
/////////////////////////////////////////////////////////////////////////////

namespace Trk {
    class IVertexFitter;
    class ITrackSelectorTool;
}
namespace InDet { class VertexPointEstimator; }

namespace Analysis {
    
    static const InterfaceID IID_JpsiPlus1Track("JpsiPlus1Track", 1, 0);
    
    class JpsiPlus1Track:  public Analysis::ICandidateSearch, public AthAlgTool
    {
    public:
        JpsiPlus1Track(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiPlus1Track();
        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;
        
        static const InterfaceID& interfaceID() { return IID_JpsiPlus1Track;};
        static double getInvariantMass(const std::vector<const xAOD::TrackParticle*> &trk, double mass1,
                                             double mass2, double mass3);
 

      
        //-------------------------------------------------------------------------------------
        //Doing Calculation and inline functions
        virtual StatusCode performSearch(xAOD::VertexContainer*& , xAOD::VertexAuxContainer*& ) override;
        xAOD::Vertex* fit(const std::vector<const xAOD::TrackParticle*>&, const xAOD::TrackParticleContainer*, const xAOD::TrackParticleContainer*) const;
        //-------------------------------------------------------------------------------------
        
    private:
        bool m_piMassHyp;
        bool m_kMassHyp;
        double m_trkThresholdPt;
        double m_trkMaxEta;
        double m_BThresholdPt;
        double m_BMassUpper;
        double m_BMassLower;
        std::string m_jpsiCollectionKey;
        double m_jpsiMassUpper;
        double m_jpsiMassLower;
        std::string m_TrkParticleCollection;
        std::string m_MuonsUsedInJpsi;
        bool m_excludeJpsiMuonsOnly;
        bool m_excludeCrossJpsiTracks; //Added by Matteo Bedognetti
        ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
        ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
        ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
        Trk::TrkVKalVrtFitter* m_VKVFitter;
        bool m_useMassConst;
        double m_altMassConst;
        double m_chi2cut;
        double m_trkTrippletMassUpper;
        double m_trkTrippletMassLower;
        double m_trkTrippletPt       ;
        double m_trkDeltaZ           ;
        int m_requiredNMuons;
        std::vector<double> m_muonMasses;
        std::vector<int>    m_useGSFTrackIndices;
        std::string         m_TrkParticleGSFCollection;
        std::bitset<3>      m_useGSFTrack;

    };
} // end of namespace
#endif

