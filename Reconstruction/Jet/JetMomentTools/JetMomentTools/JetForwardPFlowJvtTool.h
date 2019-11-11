///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetForwardPFlowJvtTool.h
// Header file for class JetForwardPFlowJvtTool
// Author: Anastasia Kotsokechagia <anastasia.kotsokechagia@cern.ch>

// Tool for calculating fjvt values for pflow jets. 
// Short describtion of the tool;
//   First central PU jets are built per vertex.
//   Reconstructed calibrated jets are then used to calculate the per vertex missing momentum (miss-mom). 
//   The per vertex missing momentum is defined as: The vector some of the calibrated jet momenta (for jets with pt>20GeV && Rpt>0.1 wrt to the vertex) + tracks assosiated to the vertex (otherwise). 
//   PU Jets closeby (dR<0.3) to a HS jet are not considered.
//   The fJVT value for every forward jet (fj) of the event is then calculated choosing the vertex with the largest negative miss-mom projection on the fj.  
//   User action: After initializing the tool the user has to call the modify(xAOD::JetContainer& jetCont) function. Argument in this fuction is the PFlow jet container of the event. 
//   The fjvt value for every forward jet of the container is then calculated and can be retrieved.
///////////////////////////////////////////////////////////////////
    //Parameters
    // m_orLabel:  ""  
    // m_jetsName : "Container name for the output reconstructed PU jets " 
    // m_tightOP: "If true a tight fjvt threshold value is applied" 
    // m_outLabelFjvt: "Decorator for passing fJVT threshold (tight or loose)"                            
    // m_verticesName: "Container name of vertices to be retrieved" 
    // m_jetchargedp4: "Name of the jet charged momentum 4-vector" 
    // m_etaThresh: "Maximum eta value for considering a jet as central" 
    // m_forwardMinPt: "Minimum forward jet pt" 
    // m_forwardMaxPt: "Maximum forward jet pt. If -1 no threshold is applied" 
    // m_centerMinPt: "Minimum central jet pt"  
    // m_centerMaxPt: "Maximum central jet pt. If -1 no threshold is applied" 
    // m_pvind: "Hard-Scatter primary vertex index of the event. If -1 it's automatically retrieved from the event"
    // m_rptCut: "Rpt cut value for central PU jets contributing in the missing momentum calculation" 
    // m_jvtCut: "JVT threshold value for considering a central PU jet as HS" 
    // m_dzCut: "Dz=z-z0 cut value for pfo objects participating in the HS vertex jet reco" 
    // m_vertices: "Number of vertices for which the missing momentum is calculated"
    // m_maxRap: "Maximum rapidity value in fastjet::AreaDefinition" 
    // m_neutMaxRap: "Maximum rapidity value for neutral pfos participating in jet reco"
    // m_weight: "PFO weight value"
    // m_pfoToolName: "Name of PFO retriever tool"
    // m_wpfoToolName: "Name of PFO weighting tool"
    // m_pfoJESName: "Name of jet calibration tool"
    // m_jetAlgo: "Jet calibration collection name"
    // m_caliconfig: "Calibration config for PFlow jets, need to be updated with latest one"
    // m_calibSeq: "Calibration sequence to be applied"
    // m_calibArea: "Calibration area" 
    // m_isdata: "True if data"


#ifndef FORWARDPFLOWJVTTOOL_JVT_FORWARDPFLOWJVTTOOL_H
#define FORWARDPFLOWJVTTOOL_JVT_FORWARDPFLOWJVTTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

// Pflow tools 
#include "PFlowUtils/IRetrievePFOTool.h"
#include "PFlowUtils/IWeightPFOTool.h"
#include "PFlowUtils/RetrievePFOTool.h"
#include "PFlowUtils/WeightPFOTool.h"
#include "JetCalibTools/JetCalibrationTool.h"

#include "AsgTools/AnaToolHandle.h"
#include "JetCalibTools/IJetCalibrationTool.h"


  class JetForwardPFlowJvtTool
  : public asg::AsgTool,
    virtual public IJetModifier{
    ASG_TOOL_CLASS(JetForwardPFlowJvtTool,IJetModifier)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// Constructor with parameters:
    JetForwardPFlowJvtTool(const std::string& name);

    /// Destructor:
    virtual ~JetForwardPFlowJvtTool();

    virtual StatusCode  initialize() override;
 

    virtual int modify(xAOD::JetContainer& jetCont) const override;

    float getFJVT(const xAOD::Jet *jet,std::vector<TVector2> pileupMomenta) const;
    bool isForwardJet(const xAOD::Jet *jet) const;
    bool isCentralJet(const xAOD::Jet *jet) const;

    static StatusCode tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets);
    std::vector<TVector2> calculateVertexMomenta(const xAOD::JetContainer *jets,int pvind, int vertices) const;
//  void buildPFlowPUjets(const xAOD::Vertex &vx, const xAOD::PFOContainer &pfos) const;
    StatusCode buildPFlowPUjets(const xAOD::Vertex &vx, const xAOD::PFOContainer &pfos) const;
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
    float  m_weight;
    bool m_tightOP;
    std::unique_ptr<SG::AuxElement::Decorator<char> > Dec_OR;
    std::unique_ptr<SG::AuxElement::Decorator<char> > Dec_outFjvt;
    asg::AnaToolHandle<CP::RetrievePFOTool> m_pfotool;
    asg::AnaToolHandle<CP::WeightPFOTool> m_wpfotool;
    asg::AnaToolHandle<IJetCalibrationTool> m_pfoJES;

    int getPV() const;

  };
#endif //> !FORWARDJVTTOOL_JVT_FORWARDJVTTOOL_H
