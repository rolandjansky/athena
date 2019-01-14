///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
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

// EDM includes
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "JetInterface/IJetModifier.h"
#include "AsgTools/IAsgTool.h"

//ASG_TOOL_INTERFACE(IJetUpdateJvt)

  class JetForwardJvtTool
  : public asg::AsgTool,
    virtual public IJetModifier{
    ASG_TOOL_CLASS(JetForwardJvtTool,IJetModifier)

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
    StatusCode  initialize();
    StatusCode  finalize();

    virtual int modify(xAOD::JetContainer& jetCont) const;

    float getFJVT(const xAOD::Jet *jet) const;
    bool forwardJet(const xAOD::Jet *jet) const;
    bool centralJet(const xAOD::Jet *jet) const;
    float getDrpt(const xAOD::Jet *jet) const;
    int getJetVertex(const xAOD::Jet *jet) const;

    static StatusCode tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets);
    void calculateVertexMomenta(const xAOD::JetContainer *jets) const;
    float getCombinedWidth(const xAOD::Jet *jet) const;

  private:

    std::string m_orLabel;
    std::string m_outLabel;
    double m_etaThresh;
    double m_forwardMinPt;
    double m_forwardMaxPt;
    double m_centerMinPt;
    double m_centerMaxPt;
    double m_centerJvtThresh;
    std::string m_jvtMomentName;
    double m_centerDrptThresh;
    double m_maxStochPt;
    double m_jetScaleFactor;
    double m_fjvtThresh;
    bool m_tightOP;
    mutable std::vector<TVector2> m_pileupMomenta;
    mutable size_t m_pvind;
    SG::AuxElement::Decorator<char>* m_Dec_OR = NULL;
    SG::AuxElement::Decorator<char>* m_Dec_out = NULL;
    void getPV() const;

    /// Default constructor:
    JetForwardJvtTool();

    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key;
    SG::ReadHandleKey<xAOD::MissingETContainer> m_trkMET_key;

  };
#endif //> !FORWARDJVTTOOL_JVT_FORWARDJVTTOOL_H
