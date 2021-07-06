/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JPSIPLUSV0CASCADE_H
#define JPSIPLUSV0CASCADE_H
//*********************
// JpsiPlusV0Cascade header file
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

    static const InterfaceID IID_JpsiPlusV0Cascade("JpsiPlusV0Cascade", 1, 0);

    class JpsiPlusV0Cascade : virtual public AthAlgTool, public IAugmentationTool
    {

        std::string m_vertexContainerKey;
        std::string m_vertexV0ContainerKey;
        std::vector<std::string> m_cascadeOutputsKeys;

        std::string   m_VxPrimaryCandidateName;   //!< Name of primary vertex container

        double m_jpsiMassLower;
        double m_jpsiMassUpper;
        double m_V0MassLower;
        double m_V0MassUpper;
        double m_MassLower;
        double m_MassUpper;

        double m_mass_muon;
        double m_mass_pion;
        double m_mass_proton;
        double m_mass_lambda;
        double m_mass_ks;
        double m_mass_jpsi;
        double m_mass_b0;
        double m_mass_lambdaB;
        int m_v0_pid;
        bool m_constrV0;
        bool m_constrJpsi;

        SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
        ToolHandle < Trk::TrkVKalVrtFitter > m_iVertexFitter;
        ToolHandle < Analysis::PrimaryVertexRefitter > m_pvRefitter;
        ToolHandle < Trk::V0Tools > m_V0Tools;
        ToolHandle < DerivationFramework::CascadeTools > m_CascadeTools;

        bool        m_refitPV;
        std::string m_refPVContainerName;
        std::string m_hypoName;               //!< name of the mass hypothesis. E.g. Jpis, Upsi, etc. Will be used as a prefix for decorations
        //This parameter will allow us to optimize the number of PVs under consideration as the probability
        //of a useful primary vertex drops significantly the higher you go
        int         m_PV_max;
        int         m_DoVertexType;
        size_t      m_PV_minNTracks;

    public:
        static const InterfaceID& interfaceID() { return IID_JpsiPlusV0Cascade;}
        JpsiPlusV0Cascade(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiPlusV0Cascade();
        StatusCode initialize() override;
        StatusCode performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer ) const;
        virtual StatusCode addBranches() const override;
    };
}


#endif

