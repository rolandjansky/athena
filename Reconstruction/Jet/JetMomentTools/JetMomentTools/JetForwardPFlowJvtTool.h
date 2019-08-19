///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetForwardPFlowJvtTool.h
// Header file for class JetForwardPFlowJvtTool
// Author: Matt Klein<matthew.henry.klein@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef FORWARDPFLOWJVTTOOL_JVT_FORWARDPFLOWJVTTOOL_H
#define FORWARDPFLOWJVTTOOL_JVT_FORWARDPFLOWJVTTOOL_H 1

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

// Pflow tools
#include "PFlowUtils/RetrievePFOTool.h"
#include "PFlowUtils/WeightPFOTool.h"
#include "JetRec/PseudoJetGetter.h"

//ASG_TOOL_INTERFACE(IJetUpdateJvt)

  class JetForwardPFlowJvtTool
  : public asg::AsgTool,
    virtual public IJetModifier{
    ASG_TOOL_CLASS(JetForwardPFlowJvtTool,IJetModifier)

    // This macro defines the constructor with the interface declaration
    //ASG_TOOL_CLASS(JetForwardPFlowJvtTool, IJetForwardPFlowJvtTool)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    // Copy constructor:

    /// Constructor with parameters:
    JetForwardPFlowJvtTool(const std::string& name);

    /// Destructor:
    virtual ~JetForwardPFlowJvtTool();

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
    void calculateVertexMomenta(const xAOD::JetContainer *jets,int m_pvind, int m_vertices) const;
    void buildPFlowPUjets(const xAOD::Vertex &vx, const xAOD::PFOContainer &pfos) const;
    fastjet::PseudoJet pfoToPseudoJet(const xAOD::PFO* pfo, const CP::PFO_JetMETConfig_charge& theCharge, const xAOD::Vertex *vx) const;
    float getCombinedWidth(const xAOD::Jet *jet) const;


  private:

    std::string m_orLabel;
    std::string m_outLabel;
    std::string m_outLabelFjvt;
    std::string m_outLabelTiming;
    std::string m_verticesName;
    std::string m_jetsName;
    std::string m_jetchargedpt;
    int m_pvind;
    int m_vertices;
    double m_etaThresh;
    double m_timingCut;
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
    double m_RptCut;
    bool m_tightOP;
    mutable std::vector<TVector2> m_pileupMomenta;
    std::unique_ptr<SG::AuxElement::Decorator<char> > Dec_OR;
    std::unique_ptr<SG::AuxElement::Decorator<char> > Dec_out;
    std::unique_ptr<SG::AuxElement::Decorator<char> > Dec_outFjvt;
    std::unique_ptr<SG::AuxElement::Decorator<char> > Dec_outTiming;
    CP::RetrievePFOTool *m_pfotool;
    CP::WeightPFOTool *m_wpfotool;
    PseudoJetGetter *m_pjetget;

    int getPV() const;
    /// Default constructor:
    JetForwardPFlowJvtTool();

  };
#endif //> !FORWARDJVTTOOL_JVT_FORWARDJVTTOOL_H
