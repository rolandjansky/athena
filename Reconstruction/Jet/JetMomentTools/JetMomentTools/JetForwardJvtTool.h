///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetForwardJvtTool.h
// Header file for class JetForwardJvtTool
// Author: Matt Klein<matthew.henry.klein@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef FORWARDJVTTOOL_JVT_FORWARDJVTTOOL_H
#define FORWARDJVTTOOL_JVT_FORWARDJVTTOOL_H 1

// STL includes
#include <string>


// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"

// EDM includes
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "JetInterface/IJetDecorator.h"
#include "AsgTools/IAsgTool.h"

//ASG_TOOL_INTERFACE(IJetUpdateJvt)

  class JetForwardJvtTool
  : public asg::AsgTool,
    virtual public IJetDecorator{
    ASG_TOOL_CLASS(JetForwardJvtTool,IJetDecorator)

    // This macro defines the constructor with the interface declaration
    //ASG_TOOL_CLASS(JetForwardJvtTool, IJetForwardJvtTool)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    // Copy constructor:

    /// Constructor with parameters:
    JetForwardJvtTool(const std::string& name);

    /// Destructor:
    virtual ~JetForwardJvtTool();

    // Athena algtool's Hooks
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual StatusCode decorate(const xAOD::JetContainer& jetCont) const override;

    float getFJVT(const xAOD::Jet *jet) const;
    bool forwardJet(const xAOD::Jet *jet) const;
    bool centralJet(const xAOD::Jet *jet) const;
    float getDrpt(const xAOD::Jet *jet) const;
    int getJetVertex(const xAOD::Jet *jet) const;

    StatusCode tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets);
    void calculateVertexMomenta(const xAOD::JetContainer *jets) const;
    float getCombinedWidth(const xAOD::Jet *jet) const;

  private:

    Gaudi::Property<double> m_etaThresh{this, "EtaThresh", 2.5, "Eta threshold"};
    Gaudi::Property<double> m_forwardMinPt{this, "ForwardMinPt", 20e3, "Forward minimum Pt"};
    Gaudi::Property<double> m_forwardMaxPt{this, "ForwardMaxPt", 50e3, "Forward maximum Pt"};
    Gaudi::Property<double> m_centerMinPt{this, "CentralMinPt", 20e3, "Central minimum Pt"};
    Gaudi::Property<double> m_centerMaxPt{this, "CentralMaxPt", -1, "Central maximum Pt"};
    Gaudi::Property<double> m_centerJvtThresh{this, "CentralJvtThresh", 0.14, "Central JVT threshold"};
    Gaudi::Property<double> m_centerDrptThresh{this, "CentralDrptThresh", 0.2, "Central drpt threshold"};
    Gaudi::Property<double> m_maxStochPt{this, "CentralMaxStochPt", 35e3, "Central maximum StochPt"};
    Gaudi::Property<double> m_jetScaleFactor{this, "JetScaleFactor", 0.4, "Jet scale factor"};
    Gaudi::Property<double> m_fjvtThresh{this, "FjvtThresh", 15e3, "FJVT threshold"}; //15GeV->92%,11GeV->85%
    Gaudi::Property<bool> m_tightOP{this, "UseTightOP", false, "Use tight (true) or loose (false)"};
    mutable std::vector<TVector2> m_pileupMomenta;
    mutable size_t m_pvind;
    void getPV() const;

    /// Default constructor:
    JetForwardJvtTool();

    Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_orKey{this, "OverlapDec", "", "Overlap decoration key"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_outKey{this, "OutputDec", "passFJVT", "Output decoration key"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_isHSKey{this, "IsJvtHSName", "isJvtHS", "Decoration key for isJvtHS"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_isPUKey{this, "IsJvtPUName", "isJvtPU", "Decoration key for isJvtPU"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_fjvtDecKey{this, "FJVTName", "fJvt", "Decoration key for fJvt"};
    
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerName{this, "VertexContainerName", "PrimaryVertices", "SG key for vertex container"};
    SG::ReadHandleKey<xAOD::MissingETContainer> m_trkMETName{this, "Met_TrackName", "Met_Track", "SG key for MET track container"};

    SG::ReadDecorHandleKey<xAOD::JetContainer> m_widthKey{this, "WidthName", "Width", "SG key for jet width"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_jvtMomentKey{this, "JvtMomentName", "Jvt", "JVT moment name"};
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_sumPtsKey{this, "SumPtsName", "SumPtTrkPt500", "SG key for SumPt vector"};

  };
#endif //> !FORWARDJVTTOOL_JVT_FORWARDJVTTOOL_H
