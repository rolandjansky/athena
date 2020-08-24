/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODHIEvent/HIEventShape.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"

#include "TrigHIEventShapeMaker.h"


TrigHIEventShapeMaker::TrigHIEventShapeMaker(const std::string& name,
                               ISvcLocator* pSvcLocator) :
   HLT::FexAlgo(name, pSvcLocator),
    m_fillerTool("HIEventShapeFillerTool")
{
  declareProperty("HIEventShapeContainerKey", m_HIEventShapeContainerKey ="HIUE", "Name of the output HIUE container");
  //declareProperty("CaloCellContainerKey",     m_cell_container_key="HLT_CaloCellContainer_TrigCaloCellMaker", "CaloCellContainer to work on");
  declareProperty("UseCaloCell"           ,m_use_calo_cell	= false                     );
  declareProperty("InputCellKey"          ,m_cell_container_key = "HLT_CaloCellContainer_TrigCaloCellMaker" );
  declareProperty("InputTowerKey"         ,m_tower_container_key = "HLT_CaloTowerContainer_TrigCaloTowerMaker" );
  declareProperty("FillerTool", m_fillerTool, "Filler tool to use");
}


TrigHIEventShapeMaker::~TrigHIEventShapeMaker()
{ }


HLT::ErrorCode TrigHIEventShapeMaker::hltInitialize() {
  ATH_MSG_DEBUG( m_fillerTool.name() );
  if( m_fillerTool.retrieve().isFailure() ) {
    return HLT::ERROR;
  }
  m_fillerTool->setContainerName(m_HIEventShapeContainerKey);
  return HLT::OK;
}


HLT::ErrorCode TrigHIEventShapeMaker::hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE)
{
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
  ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());

  xAOD::HIEventShapeContainer* shape = new xAOD::HIEventShapeContainer();
  xAOD::HIEventShapeAuxContainer shapeAux = xAOD::HIEventShapeAuxContainer();
  shape->setStore(&shapeAux);

  /*
  if ( (evtStore()->record(shape,m_HIEventShapeContainerKey)).isFailure() )
    {ATH_MSG_ERROR("EE failing to record container");
    }
  if ( (evtStore()->record(shapeAux,m_HIEventShapeContainerKey+std::string("Aux"))).isFailure() )
    {ATH_MSG_ERROR("EE failing to record Aux container");
    }
  */

  if( m_fillerTool->initializeCollection(shape).isFailure() ) {
    ATH_MSG_ERROR("TrigHIEventShapeMaker failing on init");
    return HLT::ERROR;
  }


  if (m_use_calo_cell)
    {ATH_MSG_DEBUG("TrigHIEventShapeMaker will use cells");
     if (m_fillerTool->fillCollectionFromCells(m_cell_container_key).isFailure() )
       {ATH_MSG_ERROR("TrigHIEventShapeMaker failing on filling from cells");
        return HLT::ERROR;
       }
    }
  else
    {ATH_MSG_DEBUG("TrigHIEventShapeMaker will use towers");
     if (m_fillerTool->fillCollectionFromTowers(m_tower_container_key).isFailure() )
       {ATH_MSG_ERROR("TrigHIEventShapeMaker failing on filling from towers");
        return HLT::ERROR;
       }
    }



  ATH_MSG_DEBUG("Container name in TrigHIEventShapeMaker: " << m_fillerTool->getContainerName() );

  auto status = attachFeature(outputTE, shape, m_HIEventShapeContainerKey);
  if ( status != HLT::OK ) {
    ATH_MSG_ERROR("TrigHIEventShapeMaker could not attach the event shape container");
    return status;
  }
  ATH_MSG_DEBUG("The event shape collection was attached as: " << m_HIEventShapeContainerKey);



return HLT::OK;
}


HLT::ErrorCode TrigHIEventShapeMaker::hltFinalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");
  return HLT::OK;
}
