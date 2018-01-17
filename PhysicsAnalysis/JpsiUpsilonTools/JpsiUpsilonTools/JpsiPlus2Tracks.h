/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "AthContainers/DataVector.h"
#include "HepPDT/ParticleDataTable.hh"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include <vector>
#include <cmath>
#include <string>
/////////////////////////////////////////////////////////////////////////////

namespace Trk {
    class IVertexFitter;
    class VxTrackAtVertex;
    class ITrackSelectorTool;
}
namespace InDet { class VertexPointEstimator; }


namespace Analysis {
    
    static const InterfaceID IID_JpsiPlus2Tracks("JpsiPlus2Tracks", 1, 0);
    
    class JpsiPlus2Tracks:  virtual public AthAlgTool
    {
    public:
        JpsiPlus2Tracks(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiPlus2Tracks();
        StatusCode initialize();
        StatusCode finalize();
        
        static const InterfaceID& interfaceID() { return IID_JpsiPlus2Tracks;};
        
        //-------------------------------------------------------------------------------------
        //Doing Calculation and inline functions
        StatusCode performSearch(xAOD::VertexContainer*& , xAOD::VertexAuxContainer*& );
        static double getPt(const xAOD::TrackParticle*, const xAOD::TrackParticle*);
        static double getPt(const xAOD::TrackParticle*, const xAOD::TrackParticle*, const xAOD::TrackParticle*, const xAOD::TrackParticle*);
        static double getInvariantMass(const xAOD::TrackParticle*, double, const xAOD::TrackParticle*, double);
        static double getInvariantMass(const xAOD::TrackParticle*, double, const xAOD::TrackParticle*, double,
                                const xAOD::TrackParticle*, double, const xAOD::TrackParticle*, double);
        static bool   oppositeCharges(const xAOD::TrackParticle*, const xAOD::TrackParticle*);
        static bool   isContainedIn(const xAOD::TrackParticle*, std::vector<const xAOD::TrackParticle*>);
        static bool   isContainedIn(const xAOD::TrackParticle*, const xAOD::MuonContainer*);
        xAOD::Vertex* fit(const std::vector<const xAOD::TrackParticle*>&,
                          bool, double, const xAOD::TrackParticleContainer*);
        //-------------------------------------------------------------------------------------
        
    private:
        //const HepPDT::ParticleDataTable *m_particleDataTable;
        bool m_pipiMassHyp;
        bool m_kkMassHyp;
        bool m_kpiMassHyp;
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
	bool m_excludeCrossJpsiTracks; //Added by Matteo Bedognetti
        std::vector<xAOD::Vertex*> m_vxc;
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
        
    };
} // end of namespace
#endif
