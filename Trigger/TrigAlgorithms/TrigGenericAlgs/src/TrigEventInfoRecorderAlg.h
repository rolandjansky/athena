 // -*- C++ -*- 
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTINFOALG_H
#define TRIGEVENTINFOALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODEventShape/EventShape.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "xAODTracking/VertexContainer.h"


class TrigEventInfoRecorderAlg : public AthReentrantAlgorithm {

    public:
    
    TrigEventInfoRecorderAlg(const std::string & name, ISvcLocator* pSvcLocator);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    
    // Custom function for TLA functionalities
    StatusCode decorateTLA(const EventContext& ctx, xAOD::TrigComposite* trigEI) const;
 
    // Other functionalities may be added adding similar functions or defining a derived class

    private:
    
    // WRITE HANDLE KEY FOR OUTPUT CONTAINER
    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_TrigEventInfoKey{this, "trigEventInfoKey","Undefined", ""};


    // TLA AREA
    Gaudi::Property<bool> m_decorateTLA {
      this, "decorateTLA", false, "Flag to enable TLA variables decoration"};
    // TLA ReadHandleKeys
    ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool{this, "LuminosityTool", "LumiBlockMuTool/LumiBlockMuTool", "Luminosity Tool"};
    SG::ReadHandleKey<xAOD::EventShape> m_rhoKeyPF{this, "RhoKey_PFlow", "HLT_Kt4EMPFlowEventShape"}; // name of the density key: TOPO, PFLOW, etc
    SG::ReadHandleKey<xAOD::EventShape> m_rhoKeyEMT{this, "RhoKey_EMTopo", "HLT_Kt4EMTopoEventShape"}; // name of the density key: TOPO, PFLOW, etc
    SG::ReadHandleKey<xAOD::VertexContainer> m_PrimaryVxInputName{ this,"primaryVertexInputName","PrimaryVertices","Input Vertex Collection" }; 
    
    // OTHER PURPOSES to follow
     

};
#endif