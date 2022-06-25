/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JPSIPLUSPSICASCADE_H
#define JPSIPLUSPSICASCADE_H
// Xin Chen <xin.chen@cern.ch>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
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

  static const InterfaceID IID_JpsiPlusPsiCascade("JpsiPlusPsiCascade", 1, 0);

  class JpsiPlusPsiCascade : virtual public AthAlgTool, public IAugmentationTool
  {
  public:
    static const InterfaceID& interfaceID() { return IID_JpsiPlusPsiCascade;}
    JpsiPlusPsiCascade(const std::string& t, const std::string& n, const IInterface* p);
    ~JpsiPlusPsiCascade();
    virtual StatusCode initialize() override;
    StatusCode performSearch(std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer, std::vector<Trk::VxCascadeInfo*> *cascadeinfoContainer_noConstr) const;
    virtual StatusCode addBranches() const override;

  private:
    std::string m_vertexContainerKey;
    std::string m_vertexPsiContainerKey;
    std::string m_vertexJpsiHypoName;
    std::string m_vertexPsiHypoName;
    std::vector<std::string> m_cascadeOutputsKeys;
    std::string m_VxPrimaryCandidateName;   //!< Name of primary vertex container

    double m_jpsiMassLower;
    double m_jpsiMassUpper;
    double m_jpsi2MassLower;
    double m_jpsi2MassUpper;
    double m_psiMassLower;
    double m_psiMassUpper;
    double m_MassLower;
    double m_MassUpper;
    double m_vtx1Daug1MassHypo; // mass hypothesis of 1st daughter from vertex 1
    double m_vtx1Daug2MassHypo; // mass hypothesis of 2nd daughter from vertex 1
    double m_vtx1Daug3MassHypo; // mass hypothesis of 3rd daughter from vertex 1
    double m_vtx1Daug4MassHypo; // mass hypothesis of 4th daughter from vertex 1
    double m_vtx2Daug1MassHypo; // mass hypothesis of 1st daughter from vertex 2
    double m_vtx2Daug2MassHypo; // mass hypothesis of 2nd daughter from vertex 2

    double m_mass_jpsi;
    double m_mass_jpsi2;
    double m_mass_psi;
    bool   m_constrPsi;
    bool   m_constrJpsi;
    bool   m_constrJpsi2;
    double m_chi2cut;

    ServiceHandle<IBeamCondSvc>                      m_beamSpotSvc;
    ToolHandle < Trk::TrkVKalVrtFitter >             m_iVertexFitter;
    ToolHandle < Analysis::PrimaryVertexRefitter >   m_pvRefitter;
    ToolHandle < Trk::V0Tools >                      m_V0Tools;
    ToolHandle < DerivationFramework::CascadeTools > m_CascadeTools;

    bool        m_refitPV;
    std::string m_refPVContainerName;
    std::string m_hypoName;
    int         m_PV_max;
    int         m_DoVertexType;
    size_t      m_PV_minNTracks;
  };
}

#endif
