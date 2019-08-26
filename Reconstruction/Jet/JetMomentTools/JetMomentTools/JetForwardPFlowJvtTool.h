///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetForwardPFlowJvtTool.h
// Header file for class JetForwardPFlowJvtTool
// Author: Anastasia Kotsokechagia <anastasia.kotsokechagia@cern.ch>
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
#include "JetCalibTools/JetCalibrationTool.h"

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

    static StatusCode tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets);
    void calculateVertexMomenta(const xAOD::JetContainer *jets,int m_pvind, int m_vertices) const;
    void buildPFlowPUjets(const xAOD::Vertex &vx, const xAOD::PFOContainer &pfos) const;
    bool hasCloseByHSjet(const xAOD::Jet *jet, const xAOD::JetContainer *pjets ) const;
    double getRpt(const xAOD::Jet *jet) const;
    fastjet::PseudoJet pfoToPseudoJet(const xAOD::PFO* pfo, const CP::PFO_JetMETConfig_charge& theCharge, const xAOD::Vertex *vx) const;

  private:

    std::string m_orLabel;
    std::string m_outLabel;
    std::string m_outLabelFjvt;
    std::string m_verticesName;
    std::string m_jetsName;
    std::string m_jetchargedp4;
    std::string m_pfoToolName;
    std::string m_wpfoToolName;
    std::string m_pfoJESName;   
    std::string m_jetAlgo;
    std::string m_caliconfig;
    std::string m_calibSeq;
    std::string m_calibArea;
    bool m_isdata;
    int m_pvind;
    int m_vertices;
    double m_etaThresh;
    double m_forwardMinPt;
    double m_forwardMaxPt;
    double m_centerMinPt;
    double m_centerMaxPt;
    double m_centerJvtThresh;
    std::string m_jvtMomentName;
    double m_fjvtThresh;
    double m_rptCut;
    double m_jvtCut;
    double m_dzCut;
    double m_maxRap;
    double m_neutMaxRap;
    bool m_tightOP;
    mutable std::vector<TVector2> m_pileupMomenta;
    std::unique_ptr<SG::AuxElement::Decorator<char> > Dec_OR;
    //std::unique_ptr<SG::AuxElement::Decorator<char> > Dec_out;
    std::unique_ptr<SG::AuxElement::Decorator<char> > Dec_outFjvt;
    std::unique_ptr<CP::RetrievePFOTool> m_pfotool;
    std::unique_ptr<CP::WeightPFOTool> m_wpfotool;
    std::unique_ptr<JetCalibrationTool> m_pfoJES;

    int getPV() const;
    /// Default constructor:
    JetForwardPFlowJvtTool();

  };
#endif //> !FORWARDJVTTOOL_JVT_FORWARDJVTTOOL_H
