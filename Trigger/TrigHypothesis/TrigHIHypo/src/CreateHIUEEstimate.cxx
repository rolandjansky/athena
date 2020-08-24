/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODHIEvent/HIEventShapeAuxContainer.h"

#include "CreateHIUEEstimate.h"


CreateHIUEEstimate::CreateHIUEEstimate(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_fillerTool("HIEventShapeFillerTool"),
    m_hasRun(false)
{
  declareProperty("HIEventShapeContainerKey", m_HIEventShapeContainerKey ="HIUE", "Nmae of the output HIUE container");
  declareProperty("CaloCellContainerKey",     m_CaloCellContainerKey="HLT_CaloCellContainer_TrigCaloCellMaker", "CaloCellContainer to work on");
  declareProperty("FillerTool", m_fillerTool, "Filler tool to use");
}

HLT::ErrorCode CreateHIUEEstimate::hltInitialize() {
  ATH_MSG_INFO( m_fillerTool.name() );
  if( m_fillerTool.retrieve().isFailure() ) {
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }
  return HLT::OK;
}

HLT::ErrorCode CreateHIUEEstimate::hltEndEvent() {
  m_hasRun = false;
  return HLT::OK;
}

HLT::ErrorCode CreateHIUEEstimate::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& ,
					      unsigned int type_out) {

  HLT::TEVec empty;
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(empty, type_out);
  if ( m_hasRun ) {
    return HLT::OK;
  }
  m_hasRun = true;
  // for now we have this code which does what offline alg does
  // but in future we have to change it to be trigger specific
  //ATH_MSG_INFO("Executing HIUE maker tool");
  //  xAOD::HIEventShapeContainer* theUEContainer = new xAOD::HIEventShapeContainer();
  //  xAOD::HIEventShapeAuxContainer* aux = new xAOD::HIEventShapeAuxContainer();
  //  theUEContainer->setStore(aux);

  //  ATH_MSG_INFO( evtStore()->dump() );
  xAOD::HIEventShapeContainer* shape = new xAOD::HIEventShapeContainer();
  xAOD::HIEventShapeAuxContainer shapeAux = xAOD::HIEventShapeAuxContainer();
  shape->setStore(&shapeAux);

  if( m_fillerTool->initializeCollection(shape).isFailure() ) {
    return HLT::ERROR;
  }

  if( m_fillerTool->fillCollectionFromCells(m_CaloCellContainerKey).isFailure() ) {
    return HLT::ERROR;
    }

  {

    // ATH_MSG_DEBUG( "UE Container size" << theUEContainer->size() );
    // for ( auto eshape: *theUEContainer ) {
    //   ATH_MSG_INFO("Eshape layer " << eshape->layer() << " etamin: " << eshape->etaMin() << " etamax: " << eshape->etaMax() << " energy " << eshape->Et() << " area " << eshape->area() );
    // }



    auto status = attachFeature(outputTE, shape, m_HIEventShapeContainerKey);
    if ( status != HLT::OK ) {
      return status;
    }

  }


return HLT::OK;
}
