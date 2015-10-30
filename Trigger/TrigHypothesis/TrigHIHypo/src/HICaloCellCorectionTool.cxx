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
  //  declareProperty("CaloCellContainerName",m_CaloCellContainerName="SubtractedCells", "Name of corrected collection of cells");
  declareProperty("HIEventShapeContainerName",m_HIEventShapeContainerName="HLT_xAOD__HIEventShapeContainer_HIUE", "This HIEventShapeContainer object will be used");
  //  declareProperty("EventShapeFillerTool", m_eventShapeTool,"EventShapeFillerTool ");
}

StatusCode HICaloCellCorectionTool::initialize(){
  
  return StatusCode::SUCCESS;
}

StatusCode HICaloCellCorectionTool::process(CaloCellContainer* cells){


  //  ATH_MSG_INFO( evtStore()->dump() );  
  
  m_eventShape = nullptr;
  CHECK( evtStore()->retrieve(m_eventShape, m_HIEventShapeContainerName) );

  CaloCellContainer clone(SG::VIEW_ELEMENTS); // this is not newed as it will be disposed at the return from this fucntion
  clone.insert(clone.end(), cells->begin(), cells->end());
  //ATH_MSG_INFO( "Cloned cells size " << clone.size());
  cells->clear();  
  cells->clear(SG::OWN_ELEMENTS);

  //loop over cells 
  for (CaloCell* cell : clone) {

    int cellLayer = cell->caloDDE()->getSampling();
    double cellEta = cell->eta();
    double cellsinTheta = cell->sinTh();
    
    m_avgEnergy = getAvgEnergy(cellLayer, cellEta);
    double cellEt = cell->et();
    
    //ATH_MSG_INFO( "Correction " << cellEt << " " << m_avgEnergy);

    double totEnergy = cellEt - m_avgEnergy;
    totEnergy /= cellsinTheta;
    CaloCell* cell_clone = cell->clone();
    cell_clone->setEnergy(totEnergy);
    cells->push_back(cell_clone);

  }//end loop over cells
  //  ATH_MSG_INFO( "Renewed size " << cells->size());
  return StatusCode::SUCCESS;
}


double HICaloCellCorectionTool::getAvgEnergy(const int layer, const double eta){
  unsigned int size = m_eventShape->size();
  
  for(unsigned int i =0; i<size; i++){
    const xAOD::HIEventShape* ev = m_eventShape->at(i);
    int evLayer = ev->layer();
    
    if(evLayer == layer && (eta <= ev->etaMax() && eta > ev->etaMin())){
      double et = ev->et();
      if ( std::abs(et) > 0.1 ) // energy in MeV so this small value essentially zero
	return et /= ev->nCells();
    }
    else continue;
  }
  return 0.;
  
  
}
