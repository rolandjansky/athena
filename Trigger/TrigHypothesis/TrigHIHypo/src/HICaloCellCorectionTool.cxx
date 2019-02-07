/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "HICaloCellCorectionTool.h"
#include "StoreGate/ReadHandle.h"


HICaloCellCorectionTool::HICaloCellCorectionTool(const std::string& type, 
						 const std::string& name, 
						 const IInterface* parent)
  : base_class(type,name,parent)
{
  declareProperty("HIEventShapeContainerName",m_eventShapeKey="HLT_xAOD__HIEventShapeContainer_HIUE", "This HIEventShapeContainer object will be used");
}

StatusCode HICaloCellCorectionTool::initialize()
{
  ATH_CHECK( m_eventShapeKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode HICaloCellCorectionTool::process (CaloCellContainer* cells,
                                             const EventContext& ctx) const
{
  SG::ReadHandle<xAOD::HIEventShapeContainer> eventShape (m_eventShapeKey, ctx);

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
    
    double avgEnergy = getAvgEnergy(*eventShape, cellLayer, cellEta);
    double cellEt = cell->et();
    
    //ATH_MSG_INFO( "Correction " << cellEt << " " << avgEnergy);

    double totEnergy = cellEt - avgEnergy;
    totEnergy /= cellsinTheta;
    CaloCell* cell_clone = cell->clone();
    cell_clone->setEnergy(totEnergy);
    cells->push_back(cell_clone);

  }//end loop over cells
  //  ATH_MSG_INFO( "Renewed size " << cells->size());
  return StatusCode::SUCCESS;
}


double
HICaloCellCorectionTool::getAvgEnergy(const xAOD::HIEventShapeContainer& eventShape,
                                      const int layer,
                                      const double eta) const
{
  unsigned int size = eventShape.size();
  
  for(unsigned int i =0; i<size; i++){
    const xAOD::HIEventShape* ev = eventShape.at(i);
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
