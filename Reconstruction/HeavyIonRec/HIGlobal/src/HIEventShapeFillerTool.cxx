/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/HIEventShapeFillerTool.h"
#include <Navigation/NavigationToken.h>
#include <CaloEvent/CaloCell.h>
#include <CaloEvent/CaloCellContainer.h>
#include <xAODHIEvent/HIEventShape.h>
#include "HIEventUtils/HIEventShapeMap.h"

HIEventShapeFillerTool::HIEventShapeFillerTool(const std::string& myname)  : asg::AsgTool(myname),
									     m_evtShape(nullptr),
									     m_NumOrders(7),
									     m_index(nullptr)
{

}

StatusCode  HIEventShapeFillerTool::SetNumOrders(int Norders)
{
   m_NumOrders=Norders;
   return StatusCode::SUCCESS;
}

StatusCode HIEventShapeFillerTool::InitializeCollection(xAOD::HIEventShapeContainer *evtShape_)
{

   //change m_evtShape to m_evtShape
   m_evtShape=evtShape_;

   //tool is initialized only once
   if(!m_index)
   {
     HIEventShapeIndex index;
     index.setBinning(HIEventShapeIndex::COMPACT);
     m_index=HIEventShapeMap::getMap()->insert(GetContainerName(),index);
   }
   //fix this to have proper name passing

   //use tool to initialize event shape object
   m_index->initializeEventShapeContainer(m_evtShape,m_NumOrders);

   return StatusCode::SUCCESS;
}



StatusCode HIEventShapeFillerTool::FillCollectionFromTowers(const std::string &tower_container_key)     
{
   //retrieve the tower container from store
   const INavigable4MomentumCollection* navInColl = 0;
   CHECK(evtStore()->retrieve(navInColl,tower_container_key));
   return FillCollectionFromTowerContainer(navInColl);
}
StatusCode HIEventShapeFillerTool::FillCollectionFromTowerContainer(const INavigable4MomentumCollection* navInColl)
{
  //loop on towers
  for(INavigable4MomentumCollection::const_iterator towerItr=navInColl->begin();
      towerItr!=navInColl->end(); towerItr++)
  {
    //navigate back to cells
    //Default is to sort the cells by either pointer values leading to irreproducible output
    //CaloCellIDFcn ensures cells are ordered by their IDs
    NavigationToken<CaloCell,double,CaloCellIDFcn> cellToken;
    (*towerItr)->fillToken(cellToken,double(1.));

    // Use eta/phi of tower in shape calculation
    float eta0=(*towerItr)->eta();
    float phi0=(*towerItr)->phi();

    if ( cellToken.size() == 0 ) continue;
    for(NavigationToken<CaloCell,double,CaloCellIDFcn>::const_iterator cellItr = cellToken.begin();
	cellItr != cellToken.end(); cellItr++ ) UpdateShape(m_evtShape,m_index,*cellItr,cellToken.getParameter(*cellItr),eta0,phi0);
  }//end tower loop
  return StatusCode::SUCCESS;
}



 
StatusCode HIEventShapeFillerTool::FillCollectionFromCells(const std::string &cell_container_key)     
{
   ATH_MSG_DEBUG("INSIDE FillCollectionFromCells");
   //retrieve the cell container from store
   const CaloCellContainer* CellContainer = 0;
   if(evtStore()->retrieve(CellContainer,cell_container_key).isFailure())
   {
     ATH_MSG_ERROR(" Could not retrieve CaloCell container with key "<< cell_container_key);
     return StatusCode::FAILURE;
   }
   return FillCollectionFromCellContainer(CellContainer);
}
StatusCode HIEventShapeFillerTool::FillCollectionFromCellContainer(const CaloCellContainer* CellContainer)
{
   ATH_MSG_DEBUG("INSIDE FillCollectionFromCellContainer");
   //loop on Cells
   for(const auto cellItr : *CellContainer) UpdateShape(m_evtShape,m_index,cellItr,1.,cellItr->eta(),cellItr->phi());
   return StatusCode::SUCCESS;
} 


void HIEventShapeFillerTool::UpdateShape(xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const CaloCell* theCell, float geoWeight, float eta0, float phi0, bool isNeg) const
{
  float sgn=(isNeg) ? -1 : 1;

  int layer = theCell->caloDDE()->getSampling();
  float cell_et = theCell->et();


  xAOD::HIEventShape* slice=index->getShape(eta0,layer,shape);
  //update members
  slice->setNCells(slice->nCells()+sgn);
  slice->setEt(slice->et()+sgn*cell_et*geoWeight);
  float deta=theCell->caloDDE()->deta();
  float dphi=theCell->caloDDE()->dphi();
  float area=std::abs(deta*dphi);
  float rho=0;
  if(area!=0.) rho=cell_et/area;
  slice->setArea(slice->area() + sgn*area*geoWeight);
  slice->setRho(slice->rho() + sgn*rho);

  for(unsigned int ih=0; ih<slice->etCos().size(); ih++)
  {
    float ih_f=ih+1;
    float tmp_cos = slice->etCos().at(ih);
    slice->etCos()[ih] = tmp_cos + cell_et*cos(ih_f*phi0)*geoWeight;
    
    float tmp_sin = slice->etSin().at(ih);
    slice->etSin()[ih] = tmp_sin + cell_et*sin(ih_f*phi0)*geoWeight;
  }
}  

