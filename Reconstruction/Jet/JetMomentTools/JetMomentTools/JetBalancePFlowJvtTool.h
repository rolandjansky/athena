///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// JetBalancePFlowJvtTool.h
// Header file for class JetBalancePFlowJvtTool
// Author: Eimear Conroy <eimear.isobel.conroy@cern.ch>
// Inherits from JetForwardPFlowJvtTool - Author: Anastasia Kotsokechagia <anastasia.kotsokechagia@cern.ch>

// Tool for calculating bjvt values for pflow jets. 
// Short describtion of the tool;
//   First central PU jets are built per vertex.
//   Reconstructed calibrated jets are then used to calculate the per vertex missing momentum (miss-mom). 
//   The per vertex missing momentum is defined as: The vector some of the calibrated jet momenta (for jets with pt>20GeV && Rpt>0.1 wrt to the vertex) + tracks assosiated to the vertex (otherwise). 
//   PU Jets closeby (dR<0.3) to a HS jet are not considered.
//   The bJVT value for every central jet (cj) of the event is then calculated choosing the vertex with the largest negative miss-mom projection on the cj.  
//   User action: After initializing the tool the user has to call the modify(xAOD::JetContainer& jetCont) function. Argument in this fuction is the PFlow jet container of the event. 
//   The bjvt value for every central jet of the container is then calculated and can be retrieved.
///////////////////////////////////////////////////////////////////

    //Parameters inherited from JetForwardPFlowJvtTool
    // m_orLabel:  "" (redundant here)
    // m_jetsName : "Container name for the output reconstructed PU jets " 
    // m_tightOP: "If true a tight fjvt threshold value is applied" (redundant here)
    // m_outLabelFjvt: "Decorator for passing fJVT threshold (tight or loose)" (redundant here)                            
    // m_jetchargedp4: "Name of the jet charged momentum 4-vector" 
    // m_etaThresh: "Maximum eta value for considering a jet as central" 
    // m_forwardMinPt: "Minimum forward jet pt" (redundant here)
    // m_forwardMaxPt: "Maximum forward jet pt. If -1 no threshold is applied" (redundant here)
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
    // m_pfoToolName: "Name of PFO retriever tool" (redundant here)
    // m_wpfoToolName: "Name of PFO weighting tool"
    // m_pfoJESName: "Name of jet calibration tool" (redundant here)
    // m_jetAlgo: "Jet calibration collection name" (redundant here)
    // m_calibconfig: "Calibration config for PFlow jets, need to be updated with latest one" (redundant here)
    // m_calibSeq: "Calibration sequence to be applied" (redundant here)
    // m_calibArea: "Calibration area" (redundant here)
    // m_isdata: "True if data" (redundant here)
    //
    //New parameters for bJVT
    // m_QCDPUMinPt: "Minimum pt cut value for central PU jets contributing in the missing momentum calculation"
    // m_QCDPUMaxPt: "Maximum pt cut value for central PU jets contributing in the missing momentum calculation. If -1 no threshold is applied"
    // m_bjvtKey: "Decorator for passing bJVT threshold (tight or loose)"
    // m_bjvtRawKey: "Decorator for raw bJVT variable"
    // m_isQCDPUKey: "SG key for output isJVTQCDPU decoration"
    // m_isStochPUKey: "SG key for output isJVTStochPU decoration"


#ifndef BALANCEPFLOWJVTTOOL_JVT_BALANCEPFLOWJVTTOOL_H
#define BALANCEPFLOWJVTTOOL_JVT_BALANCEPFLOWJVTTOOL_H 1

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
#include "xAODPFlow/FlowElementContainer.h"

#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/ReadDecorHandle.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandle.h"

// Pflow / FE tools 
#include "PFlowUtils/FEHelpers.h"
#include "PFlowUtils/IWeightPFOTool.h"
#include "PFlowUtils/WeightPFOTool.h"
#include "JetCalibTools/IJetCalibrationTool.h"

#include "AsgTools/ToolHandle.h"
#include "JetCalibTools/IJetCalibrationTool.h"

#include "JetMomentTools/JetForwardPFlowJvtTool.h"



class JetBalancePFlowJvtTool: public JetForwardPFlowJvtTool{

  ASG_TOOL_CLASS(JetBalancePFlowJvtTool,IJetDecorator)

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
  public:

    /// Constructor with parameters:
    JetBalancePFlowJvtTool(const std::string& name);

    /// Destructor:
    virtual ~JetBalancePFlowJvtTool();

    virtual StatusCode initialize() override;
    virtual StatusCode decorate(const xAOD::JetContainer& jetCont) const override;

    //New methods
    bool isQCDPUJet(const xAOD::Jet *jet) const;
  
    //Overridden/overloaded methods from JetForwardPFlowJvtTool
    virtual std::vector<TVector2> calculateVertexMomenta(const xAOD::JetContainer *jets,int pvind, int vertices) const override; 
    StatusCode tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets,const xAOD::JetContainer *itpujets,const xAOD::JetContainer *ootpujets);
  
  protected:
    //New properties for bJVT, JetForwardPFlowJvtTool properties also inherited
    
    Gaudi::Property<float> m_QCDPUMinPt{this, "QCDPUMinPt", 20e3, "Minimum pt cut value for central PU jets contributing in the missing momentum calculation"};
    Gaudi::Property<float> m_QCDPUMaxPt{this, "QCDPUMaxPt", -1, "Maximum pt cut value for central PU jets contributing in the missing momentum calculation. If -1 no threshold is applied"};
    //EMTopo tool also had selections on JVF and JVT

    SG::WriteDecorHandleKey<xAOD::JetContainer> m_bjvtKey{this, "BjvtName", "passOnlyBJVT", "Decorator for passing bJVT threshold (tight or loose)"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_bjvtRawKey{this, "BjvtRawName", "bJvt", "Decorator for raw bJVT variable"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_isQCDPUKey{this, "isQCDPUName", "isJvtQCDPU", "SG key for output isJVTQCDPU decoration"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_isStochPUKey{this, "isStochPUName", "isJvtStochPU", "SG key for output isJVTStochPU decoration"};
    
    float m_bjvtThresh;

  };
#endif //> !BALANCEJVTTOOL_JVT_BALANCEJVTTOOL_H
