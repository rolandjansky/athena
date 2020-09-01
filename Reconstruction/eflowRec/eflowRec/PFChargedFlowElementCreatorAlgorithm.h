#ifndef PFCHARGEDFLOWELEMENTCREATORALGORITHM_H
#define PFCHARGEDFLOWELEMENTCREATORALGORITHM_H

#include "eflowRec/eflowCaloObject.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "xAODPFlow/FlowElementContainer.h"


class PFChargedFlowElementCreatorAlgorithm : public  AthReentrantAlgorithm {

  public:
  
    using AthReentrantAlgorithm::AthReentrantAlgorithm;

    StatusCode initialize();
    StatusCode execute(const EventContext&) const;

  private:

    /** Create the charged PFO */ 
    void createChargedFlowElements(const eflowCaloObject& energyFlowCaloObject, bool addClusters, SG::WriteHandle<xAOD::FlowElementContainer>& chargedFlowElementContainerWriteHandle) const;

    /** Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed */
    Gaudi::Property<bool> m_eOverPMode{this,"EOverPMode",false,"Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed"};

    /** ReadHandleKey for eflowCaloObjectContainer */
    SG::ReadHandleKey<eflowCaloObjectContainer> m_eflowCaloObjectContainerReadHandleKey{this,"eflowCaloObjectContainerName","eflowCaloObjects","ReadHandleKey for eflowCaloObjectContainer"};
  
    /** WriteHandleKey for charged PFO */
    SG::WriteHandleKey<xAOD::FlowElementContainer> m_chargedFlowElementContainerWriteHandleKey{this,"FlowElementOutputName","JetETMissChargedFlowElements","WriteHandleKey for charged FlowElements"};
    
};

#endif
