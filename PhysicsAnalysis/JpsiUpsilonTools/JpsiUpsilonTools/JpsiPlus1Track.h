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
#ifndef JPSIPLUS1TRACK_H
#define JPSIPLUS1TRACK_H
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
    class ITrackSelectorTool;
}
namespace InDet { class VertexPointEstimator; }

namespace Analysis {
    
    static const InterfaceID IID_JpsiPlus1Track("JpsiPlus1Track", 1, 0);
    
    class JpsiPlus1Track:  virtual public AthAlgTool
    {
    public:
        JpsiPlus1Track(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiPlus1Track();
        StatusCode initialize();
        StatusCode finalize();
        
        static const InterfaceID& interfaceID() { return IID_JpsiPlus1Track;};
        static double getInvariantMass(const xAOD::TrackParticle* trk1, double mass1,
                                             const xAOD::TrackParticle* trk2, double mass2,
                                             const xAOD::TrackParticle* trk3, double mass3);
 
        static double getPt(const xAOD::TrackParticle* trk1,
                                  const xAOD::TrackParticle* trk2,
                                  const xAOD::TrackParticle* trk3);
      
        //-------------------------------------------------------------------------------------
        //Doing Calculation and inline functions
        StatusCode performSearch(xAOD::VertexContainer*& , xAOD::VertexAuxContainer*& );
        static bool isContainedIn(const xAOD::TrackParticle*, std::vector<const xAOD::TrackParticle*>);
        static bool isContainedIn(const xAOD::TrackParticle*, const xAOD::MuonContainer*);
        xAOD::Vertex* fit(const xAOD::TrackParticle*, const xAOD::TrackParticle*, const xAOD::TrackParticle*, bool, double, const xAOD::TrackParticleContainer*);
        //-------------------------------------------------------------------------------------
        
    private:
        const HepPDT::ParticleDataTable *m_particleDataTable;
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
	bool m_excludeCrossJpsiTracks; //Added by Matteo Bedognetti
        std::vector<Trk::VxCandidate*> m_vxc;
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
        
    };
} // end of namespace
#endif

