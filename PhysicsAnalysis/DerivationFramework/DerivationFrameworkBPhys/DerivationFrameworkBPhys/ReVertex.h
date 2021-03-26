/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// ****************************************************************************
// ----------------------------------------------------------------------------
// ReVertex header file
//
// Konstantin Beloborodov <Konstantin.Beloborodov@cern.ch>
//
// ----------------------------------------------------------------------------
// ****************************************************************************

#ifndef DERIVATIONFRAMEWORK_ReVertex_H
#define DERIVATIONFRAMEWORK_ReVertex_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODTracking/Vertex.h"
/** forward declarations
 */
namespace Trk {
    class IVertexFitter;
    class TrkV0VertexFitter;
    class ITrackSelectorTool;
    class V0Tools;
    class TrkVKalVrtFitter;
}

namespace Analysis {
    class PrimaryVertexRefitter;
}

namespace InDet { class VertexPointEstimator; }
class IBeamCondSvc;

namespace DerivationFramework {

class ReVertex : public AthAlgTool, public IAugmentationTool {
public:

    ReVertex(const std::string& t, const std::string& n, const IInterface* p);

    virtual StatusCode initialize() override;
    //virtual StatusCode finalize() override;

    virtual StatusCode addBranches() const override;

   void fitAndStore(xAOD::VertexContainer* vtxContainer,
		    const xAOD::Vertex* v,
		    const xAOD::VertexContainer    *InVtxContainer,
		    const std::vector<const xAOD::TrackParticle*> &inputTracks,
		    const xAOD::TrackParticleContainer* importedTrackCollection,
		    const xAOD::VertexContainer* pvContainer) const;
   xAOD::Vertex* fit(const std::vector<const xAOD::TrackParticle*> &inputTracks,
		     const xAOD::TrackParticleContainer* importedTrackCollection,
		     const xAOD::Vertex* pv) const;
private:
    std::vector<int> m_TrackIndices;
    ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
    ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
    Trk::TrkVKalVrtFitter* m_VKVFitter;
    std::string m_OutputContainerName;
    std::string m_inputContainerName;
    std::string m_trackContainer;
    std::string m_refPVContainerName;
    std::string m_pvContainerName;


    std::vector<double> m_trkMasses;
    std::vector<int> m_indices;
    double m_massConst;
    double m_totalMassConst;
    std::vector<std::string> m_hypoNames;

    ToolHandle<Trk::V0Tools>                    m_v0Tools;
    ToolHandle<Analysis::PrimaryVertexRefitter> m_pvRefitter;
    ServiceHandle<IBeamCondSvc>                 m_beamSpotSvc;
    int         m_PV_max;
    int         m_DoVertexType;
    size_t      m_PV_minNTracks;
    bool        m_do3d;
    bool        m_AddPVData;
    bool        m_refitPV;
    bool        m_doMassConst;
    bool        m_startingpoint0;
    
    bool m_vertexFittingWithPV;

   double m_BMassUpper;
   double m_BMassLower;
   double m_chi2cut;                 // chi2/Ndof of the final veretx
   double m_trkDeltaZ;               // DeltaZ between the JPsi vertex and hadronic tracks Z0

   bool m_useAdditionalTrack;
};
}

#endif
