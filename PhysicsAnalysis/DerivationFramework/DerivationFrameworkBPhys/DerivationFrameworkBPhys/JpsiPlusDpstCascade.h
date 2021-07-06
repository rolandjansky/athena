/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JPSIPLUSDPSTCASCADE_H
#define JPSIPLUSDPSTCASCADE_H
//*********************
// JpsiPlusDpstCascade header file
//
// Eva Bouhova <e.bouhova@cern.ch>
// Adam Barton <abarton@cern.ch>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include <vector>
#include "BeamSpotConditionsData/BeamSpotData.h"

namespace Trk {
    class IVertexFitter;
    class TrkVKalVrtFitter;
    class IVertexCascadeFitter;
    class VxCascadeInfo;
    class V0Tools;
    class ParticleDataTable;
}

namespace DerivationFramework {
    class CascadeTools;
}

namespace DerivationFramework {

    static const InterfaceID IID_JpsiPlusDpstCascade("JpsiPlusDpstCascade", 1, 0);

    class JpsiPlusDpstCascade : virtual public AthAlgTool, public IAugmentationTool
    {
      public:
        static const InterfaceID& interfaceID() { return IID_JpsiPlusDpstCascade;}
        JpsiPlusDpstCascade(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiPlusDpstCascade();
        virtual StatusCode initialize() override;
        StatusCode performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer ) const;
        virtual StatusCode addBranches() const override;

      private:
        std::string m_vertexContainerKey;
        std::string m_vertexD0ContainerKey;
        std::vector<std::string> m_cascadeOutputsKeys;

        std::string m_VxPrimaryCandidateName;   //!< Name of primary vertex container

        double m_jpsiMassLower;
        double m_jpsiMassUpper;
        double m_jpsipiMassLower;
        double m_jpsipiMassUpper;
        double m_D0MassLower;
        double m_D0MassUpper;
        double m_DstMassLower;
        double m_DstMassUpper;
        double m_MassLower;
        double m_MassUpper;
        double m_vtx0MassHypo;      // mass hypothesis of vertex 0
        double m_vtx1MassHypo;      // mass hypothesis of vertex 1
        double m_vtx0Daug1MassHypo; // mass hypothesis of 1st daughter from vertex 0
        double m_vtx0Daug2MassHypo; // mass hypothesis of 2nd daughter from vertex 0
        double m_vtx0Daug3MassHypo; // mass hypothesis of 3rd daughter from vertex 0
        double m_vtx1Daug1MassHypo; // mass hypothesis of 1st daughter from vertex 1
        double m_vtx1Daug2MassHypo; // mass hypothesis of 2nd daughter from vertex 1


        double m_mass_jpsi;
        int    m_Dx_pid;
        bool   m_constrD0;
        bool   m_constrJpsi;
        double m_chi2cut;

        SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
        ToolHandle < Trk::TrkVKalVrtFitter >             m_iVertexFitter;
        ToolHandle < Analysis::PrimaryVertexRefitter >   m_pvRefitter;
        ToolHandle < Trk::V0Tools >                      m_V0Tools;
        ToolHandle < DerivationFramework::CascadeTools > m_CascadeTools;

        bool        m_refitPV;
        std::string m_refPVContainerName;
        std::string m_hypoName;               //!< name of the mass hypothesis. E.g. Jpsi, Upsi, etc. Will be used as a prefix for decorations
        //This parameter will allow us to optimize the number of PVs under consideration as the probability
        //of a useful primary vertex drops significantly the higher you go
        int         m_PV_max;
        int         m_DoVertexType;
        size_t      m_PV_minNTracks;

    };
}

#endif
