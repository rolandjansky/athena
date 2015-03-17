/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HICaloCellCorectionTool.h"

HICaloCellCorectionTool::HICaloCellCorectionTool(const std::string& type, 
						 const std::string& name, 
						 const IInterface* parent)
  :AthAlgTool(type,name,parent),
   m_avgEnergy(0)
{
  declareInterface<ICaloCellMakerTool>(this);
  declareProperty("CaloCellContainerName",m_CaloCellContainerName="SubtractedCells", "Name of corrected collection of cells");
  declareProperty("HIEventShapeContainerName",m_HIEventShapeContainerName="HLT_xAOD__HIEventShapeContainer_HIUE", "This HIEventShapeContainer object will be used");
  //  declareProperty("EventShapeFillerTool", m_eventShapeTool,"EventShapeFillerTool ");
}

StatusCode HICaloCellCorectionTool::initialize(){
  //  msg(MSG::DEBUG) << "Retrieved the eventShape tool: "<<endreq;
  //  msg(MSG::DEBUG) << m_eventShapeTool->name()<<endreq;
  //  StatusCode evntShape = m_eventShapeTool.retrieve();
  //  if(evntShape.isFailure()){
  //    msg(MSG::FATAL) << "Unable to retrieve the IHIEventShapeFiller"<<endreq;
  //    return evntShape;
  //  }
  
  return StatusCode::SUCCESS;
}

StatusCode HICaloCellCorectionTool::process(CaloCellContainer* cells){
  //get EventShapeContainer
  //  m_eventShape = m_eventShapeTool->GetHIEventShapeContainer();  

  //  const CaloCellContainer* allcels(nullptr);
  //  CHECK(evtStore()->retrieve(allcels, "HLT_CaloCellContainer_TrigCaloCellMaker"));
  //  ATH_MSG_INFO("Size of full calo cells collection" << allcels->size());


  ATH_MSG_INFO( evtStore()->dump() );  

  m_eventShape = nullptr;
  CHECK( evtStore()->retrieve(m_eventShape, m_HIEventShapeContainerName) );


  //get cells from storegate
  /*
  StatusCode sc = evtStore()->retrieve(cells, m_CaloCellContainerName);
  if(sc.isFailure()){
    msg(MSG::WARNING) << "Couldn't retrieve CaloCellContainer" << m_CaloCellContainerName <<endreq;
    return sc;
  }
  */

  //loop over cells 
  typedef CaloCellContainer::const_iterator cellItr;
  cellItr cIter = cells->begin();

  for( ;cIter !=cells->end(); cIter++){
    
    int cellLayer = (*cIter)->caloDDE()->getSampling();
    double cellEta = (*cIter)->eta();
    double cellsinTheta = (*cIter)->sinTh();
    
    m_avgEnergy = getAvgEnergy(cellLayer, cellEta);
    double cellEt = (*cIter)->et();
    
    //ATH_MSG_INFO( "Correction " << cellEt << " " << m_avgEnergy);

    double totEnergy = cellEt - m_avgEnergy;
    totEnergy /= cellsinTheta;

    (*cIter)->setEnergy(totEnergy);

  }//end loop over cells

  return StatusCode::SUCCESS;
}


double HICaloCellCorectionTool::getAvgEnergy(const int layer, const double eta){
  unsigned int size = m_eventShape->size();
  
  for(unsigned int i =0; i<size; i++){
    const xAOD::HIEventShape* ev = m_eventShape->at(i);
    int evLayer = ev->layer();
    
    if(evLayer == layer && (eta < ev->etaMax() && eta > ev->etaMin())){
      double et = ev->Et();
      return et /= ev->nCells();
    }
    else continue;
  }
  return 0.;
  
  
}
