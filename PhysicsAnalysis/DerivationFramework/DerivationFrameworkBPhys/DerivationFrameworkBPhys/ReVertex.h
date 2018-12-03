/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// ReVertex.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ReVertex_H
#define DERIVATIONFRAMEWORK_ReVertex_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
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


    std::vector<double> m_MassConstraints;
    std::vector<int> m_indices;
    double m_massConst;

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
    
};
}

#endif
