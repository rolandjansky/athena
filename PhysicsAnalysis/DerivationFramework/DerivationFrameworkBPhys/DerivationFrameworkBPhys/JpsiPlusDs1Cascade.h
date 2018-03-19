/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JPSIPLUSDS1CASCADE_H
#define JPSIPLUSDS1CASCADE_H
//*********************
// JpsiPlusDs1Cascade header file
//
// Eva Bouhova <e.bouhova@cern.ch>
// Adam Barton <abarton@cern.ch>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include <vector>


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
class IBeamCondSvc;

namespace DerivationFramework {

    static const InterfaceID IID_JpsiPlusDs1Cascade("JpsiPlusDs1Cascade", 1, 0);

    class JpsiPlusDs1Cascade : virtual public AthAlgTool, public IAugmentationTool
    {

        std::string m_vertexContainerKey;
        std::string m_vertexD0ContainerKey;
        std::string m_vertexK0ContainerKey;
        std::vector<std::string> m_cascadeOutputsKeys;

        std::string   m_VxPrimaryCandidateName;   //!< Name of primary vertex container

        double m_jpsiMassLower;
        double m_jpsiMassUpper;
        double m_jpsipiMassLower;
        double m_jpsipiMassUpper;
        double m_D0MassLower;
        double m_D0MassUpper;
        double m_K0MassLower;
        double m_K0MassUpper;
        double m_DpstMassLower;
        double m_DpstMassUpper;
        double m_MassLower;
        double m_MassUpper;

        const HepPDT::ParticleDataTable*           m_particleDataTable;
        double m_mass_muon;
        double m_mass_pion;
        double m_mass_proton;
        double m_mass_lambda;
        double m_mass_ks;
        double m_mass_jpsi;
        double m_mass_b0;
        double m_mass_lambdaB;
        double m_mass_kaon;
        double m_mass_Dpst;
        double m_mass_D0;
        double m_mass_Bc;
        int m_v0_pid;
        int m_k0_pid;
        bool m_constrD0;
        bool m_constrK0;
        bool m_constrJpsi;

        ServiceHandle<IBeamCondSvc>                m_beamSpotSvc;
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

        double getParticleMass(int particlecode) const;

    public:
        static const InterfaceID& interfaceID() { return IID_JpsiPlusDs1Cascade;}
        JpsiPlusDs1Cascade(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiPlusDs1Cascade();
        StatusCode initialize() override;
        StatusCode performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer ) const;
        virtual StatusCode addBranches() const override;
    };
}


#endif

