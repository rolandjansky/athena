/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MuPlusDsCascade_H
#define MuPlusDsCascade_H
//*********************
// MuPlusDsCascade header file
//
// Eva Bouhova <e.bouhova@cern.ch>
// Adam Barton <abarton@cern.ch>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include <vector>
#include "xAODMuon/MuonContainer.h"


namespace Trk {
    class IVertexFitter;
    class TrkVKalVrtFitter;
    class IVertexCascadeFitter;
    class VxCascadeInfo;
    class V0Tools;
    class ParticleDataTable;
    class ITrackSelectorTool;

}

namespace DerivationFramework {
    class CascadeTools;
}
class IBeamCondSvc;

namespace DerivationFramework {

    static const InterfaceID IID_MuPlusDsCascade("MuPlusDsCascade", 1, 0);

    class MuPlusDsCascade : virtual public AthAlgTool, public IAugmentationTool
    {
      public:
        static const InterfaceID& interfaceID() { return IID_MuPlusDsCascade;}
        MuPlusDsCascade(const std::string& t, const std::string& n, const IInterface*  p);
        ~MuPlusDsCascade();
        virtual StatusCode initialize() override;
        StatusCode performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer ) const;
        virtual StatusCode addBranches() const override;

        static xAOD::Vertex* FindVertexTrack(const xAOD::MuonContainer* c,  xAOD::Vertex* v);


      private:
        
        std::string m_vertexContainerKey;
        std::string m_vertexDxContainerKey;
        std::vector<std::string> m_cascadeOutputsKeys;

        std::string m_VxPrimaryCandidateName;   //!< Name of primary vertex container

        double m_DxMassLower;
        double m_DxMassUpper;
        double m_MassLower;
        double m_MassUpper;
        double m_vtx0MassHypo; // 1st vertex mass hypothesis
        double m_vtx1MassHypo; // 2nd vertex mass hypothesis
        double m_vtx0Daug1MassHypo; // mass hypothesis of 1st daughter from vertex 0
        double m_vtx1Daug1MassHypo; // mass hypothesis of 1st daughter from vertex 1
        double m_vtx1Daug2MassHypo; // mass hypothesis of 2nd daughter from vertex 1
        double m_vtx1Daug3MassHypo; // mass hypothesis of 3rd daughter from vertex 1

        int    m_Dx_pid;
        bool   m_constrDx;
        double m_chi2cut;

        ServiceHandle<IBeamCondSvc>                      m_beamSpotSvc;
        ToolHandle < Trk::TrkVKalVrtFitter >             m_iVertexFitter;
        ToolHandle < Analysis::PrimaryVertexRefitter >   m_pvRefitter;
        ToolHandle < Trk::V0Tools >                      m_V0Tools;
        ToolHandle < DerivationFramework::CascadeTools > m_CascadeTools;

        bool        m_refitPV;
        std::string m_refPVContainerName;
        std::string m_hypoName;               //!< name of the mass hypothesis. E.g. Jpis, Upsi, etc. Will be used as a prefix for decorations
        //This parameter will allow us to optimize the number of PVs under consideration as the probability
        //of a useful primary vertex drops significantly the higher you go
        int         m_PV_max;
        int         m_DoVertexType;
        size_t      m_PV_minNTracks;
        
        std::string m_muonCollectionKey;
        ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
        double m_thresholdPt;
        bool m_mcpCuts;
        bool m_combOnly;
        bool m_useCombMeasurement;
        


    };
}

#endif
