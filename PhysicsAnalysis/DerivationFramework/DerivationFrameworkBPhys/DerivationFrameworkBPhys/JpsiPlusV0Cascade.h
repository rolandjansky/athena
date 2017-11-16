#ifndef JPSIPLUSV0CASCADE_H
#define JPSIPLUSV0CASCADE_H
//*********************
// JpsiPlusV0Cascade header file
//
// Eva Bouhova <e.bouhova@cern.ch>
// Adam Barton <abarton@cern.ch>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/VertexContainer.h"
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

    static const InterfaceID IID_JpsiPlusV0Cascade("JpsiPlusV0Cascade", 1, 0);

    class JpsiPlusV0Cascade : virtual public AthAlgTool, public IAugmentationTool
    {

        SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerKey;
        SG::ReadHandleKey<xAOD::VertexContainer> m_vertexV0ContainerKey;
        SG::WriteHandleKeyArray<xAOD::VertexContainer> m_cascadeOutputsKeys;

        SG::ReadHandleKey<xAOD::VertexContainer>   m_VxPrimaryCandidateName;   //!< Name of primary vertex container

        double m_jpsiMassLower;
        double m_jpsiMassUpper;
        double m_V0MassLower;
        double m_V0MassUpper;
        double m_MassLower;
        double m_MassUpper;

        const HepPDT::ParticleDataTable*        m_particleDataTable;
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
        //bool m_pvConstraint;

        ToolHandle < Trk::TrkVKalVrtFitter > m_iVertexFitter;
        ToolHandle < Trk::V0Tools > m_V0Tools;
        ToolHandle < DerivationFramework::CascadeTools > m_CascadeTools;
        ToolHandle<Analysis::PrimaryVertexRefitter> m_pvRefitter;
        ServiceHandle<IBeamCondSvc>                 m_beamSpotSvc;

        bool        m_refitPV;
        std::string m_refPVContainerName;
        //This parameter will allow us to optimize the number of PVs under consideration as the probability
        //of a useful primary vertex drops significantly the higher you go
        int         m_PV_max;
        int         m_DoVertexType;
        size_t      m_PV_minNTracks;

        double getParticleMass(int particlecode) const;

    public:
        static const InterfaceID& interfaceID() { return IID_JpsiPlusV0Cascade;}
        JpsiPlusV0Cascade(const std::string& t, const std::string& n, const IInterface*  p);
        ~JpsiPlusV0Cascade();
        StatusCode initialize() override;
        StatusCode finalize() override;
        StatusCode performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer ) const;
        virtual StatusCode addBranches() const override;
    };
}


#endif

