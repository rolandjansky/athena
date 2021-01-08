///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    // m_calibconfig: "Calibration config for PFlow jets, need to be updated with latest one"
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
#include "AsgTools/PropertyWrapper.h"
#include "JetInterface/IJetDecorator.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/ReadDecorHandle.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandle.h"

// Pflow tools 
#include "PFlowUtils/IWeightPFOTool.h"
#include "PFlowUtils/WeightPFOTool.h"
#include "JetCalibTools/IJetCalibrationTool.h"

#include "AsgTools/ToolHandle.h"
#include "JetCalibTools/IJetCalibrationTool.h"

namespace pflow {
  struct puJets {
    std::shared_ptr<xAOD::JetContainer> jetCont;
    std::shared_ptr<xAOD::JetAuxContainer> jetAuxCont;
  };
}

  class JetForwardPFlowJvtTool
  : public asg::AsgTool,
    virtual public IJetDecorator{
    ASG_TOOL_CLASS(JetForwardPFlowJvtTool,IJetDecorator)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// Constructor with parameters:
    JetForwardPFlowJvtTool(const std::string& name);

    /// Destructor:
    virtual ~JetForwardPFlowJvtTool();

    virtual StatusCode  initialize() override;
 

    virtual StatusCode decorate(const xAOD::JetContainer& jetCont) const override;

    float getFJVT(const xAOD::Jet *jet,std::vector<TVector2> pileupMomenta) const;
    bool isForwardJet(const xAOD::Jet *jet) const;
    bool isCentralJet(const xAOD::Jet *jet) const;

    StatusCode tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets);
    std::vector<TVector2> calculateVertexMomenta(const xAOD::JetContainer *jets,int pvind, int vertices) const;
    pflow::puJets buildPFlowPUjets(const xAOD::Vertex &vx) const;
    bool hasCloseByHSjet(const xAOD::Jet *jet, const xAOD::JetContainer *pjets ) const;
    double getRpt(const xAOD::Jet *jet) const;
    fastjet::PseudoJet pfoToPseudoJet(const xAOD::PFO* pfo, const CP::PFO_JetMETConfig_charge& theCharge, const xAOD::Vertex *vx) const;

  private:

    SG::ReadHandleKey<jet::TrackVertexAssociation> m_tvaKey{this, "TrackVertexAssociation", "", "Input track-vertex association"};
    Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};
    Gaudi::Property<std::string> m_jetsName{this, "jetsName", "AntiKt4PUPFlowJets", "Container name for the output reconstructed PU jets"};
    Gaudi::Property<std::string> m_jetchargedp4{this, "jetchargedp4", "JetChargedScaleMomentum", "Name of the jet charged momentum 4-vector"};
    Gaudi::Property<std::string> m_pfoToolName{this, "pfoToolName", "PFOTool", "Name of PFO retriever tool"};
    Gaudi::Property<std::string> m_wpfoToolName{this, "wpfoToolName", "WPFOTool", "Name of PFO weighting tool"};
    Gaudi::Property<std::string> m_pfoJESName{this, "pfoJESName", "pfoJES", "Name of jet claibration tool"};
    Gaudi::Property<std::string> m_jetAlgo{this, "jetAlgo", "AntiKt4EMPFlow", "Jet calibration collection name"};
    Gaudi::Property<std::string> m_calibconfig{this, "calibconfig", "JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config", "Calibration config for PFlow jets, need to be updated with latest one"};
    Gaudi::Property<std::string> m_calibSeq{this, "calibSeq", "JetArea_Residual_EtaJES", "Calibration sequence to be applied"};
    Gaudi::Property<std::string> m_calibArea{this, "calibArea", "00-04-82", "Calibration area"};
    
    Gaudi::Property<bool> m_isdata{this, "isdata", false, "True if data"};
    Gaudi::Property<int> m_pvind{this, "pvind", -1, "Hard-Scatter primary vertex index of the event. If -1 it will be automatically retrieved from the event"};
    Gaudi::Property<int> m_vertices{this, "vertices", 10, "Number of vertices for which the missing momentum is calculated"};
    Gaudi::Property<bool> m_includePV{this, "includePV", false, "Flag to include jets and tracks associated to PV in the calculation"};
    Gaudi::Property<double> m_etaThresh{this, "etaThresh", 2.5, "Maximum eta value for considering a jet as central"};
    Gaudi::Property<double> m_forwardMinPt{this, "forwardMinPt", 20e3, "Minimum forward jet pt"};
    Gaudi::Property<double> m_forwardMaxPt{this, "forwardMaxPt", -1, "Maximum forward jet pt. If -1 no threshold is applied"};
    Gaudi::Property<double> m_centerMinPt{this, "centralMinPt", 20e3, "Minimum central jet pt"};
    Gaudi::Property<double> m_centerMaxPt{this, "centralMaxPt", -1, "Maximum central jet pt. If -1 no threshold is applied"};
    Gaudi::Property<double> m_fjvtThresh{this, "fjvtThresh", 15e3, "fjvt threshold value"};
    Gaudi::Property<double> m_rptCut{this, "rptCut", 0.1, "Rpt cut value for central PU jets contributing in the missing momentum calculation"};
    Gaudi::Property<double> m_jvtCut{this, "jvtCut", 0.2, "JVT threshold value for considering a central PU jet as HS"};
    Gaudi::Property<double> m_dzCut{this, "dzCut", 2.0, "Dz=z=-z0 cut for pfo objects participating in the HS vertex jet reco"};
    Gaudi::Property<double> m_maxRap{this, "maxRap", 2.5, "Maximum rapidity value in fastjet::AreaDefinition"};
    Gaudi::Property<double> m_neutMaxRap{this, "neutMaxRap", 2.5, "Maximum rapidity value for neutral pfos participating in jet reco"};
    Gaudi::Property<float>  m_weight{this, "weight", 0, "PFO weight value"};
    Gaudi::Property<bool> m_tightOP{this, "tightOP", false, "If true a tight fjvt threshold value is applied"};

    // not used?
    //Gaudi::Property<std::string> m_jvtMomentName{"jvtMomentName", "", ""};
    //Gaudi::Property<double> m_centerJvtThresh{"", 0, ""};

    SG::ReadHandleKey<xAOD::VertexContainer> m_vxContKey{this, "verticesName", "PrimaryVertices", "Container name of vertices to be retrieved"};
    SG::ReadHandleKey<xAOD::PFOContainer> m_PFOKey{this, "PFOName", "CHSParticleFlowObjects", "SG Key for CHS PFO Container"};

    SG::ReadDecorHandleKey<xAOD::JetContainer> m_jvtKey{this, "jvtName", "Jvt", "SG key for the Jvt decoration"};
    SG::ReadDecorHandleKey<xAOD::PFO> m_orKey{this, "ORName", "", "OR label"};
    
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_fjvtKey{this, "FjvtName", "passOnlyFJVT", "Decorator for passing fJVT threshold (tight or loose)"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_fjvtRawKey{this, "FjvtRawName", "fJvt", "Decorator for raw fJVT variable"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_isHSKey{this, "isHSName", "isJVTHS", "SG key for output isJVTHS decoration"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_isPUKey{this, "isPUName", "isJvtPU", "SG key for output isJVTPU decoration"};
    
    ToolHandle<CP::WeightPFOTool> m_wpfotool{this,"WeightPFOTool", "", "Weight PFO tool name"};
    ToolHandle<IJetCalibrationTool> m_pfoJES{this,"JetCalibrationTool", "", "Jet calibration tool name"};

    std::size_t getPV() const;

  };
#endif //> !FORWARDJVTTOOL_JVT_FORWARDJVTTOOL_H
