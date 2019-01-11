/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/HIEventShapeFillerTool.h"
#include <Navigation/NavigationToken.h>
#include <CaloEvent/CaloCell.h>
#include <CaloEvent/CaloCellContainer.h>
#include <xAODHIEvent/HIEventShape.h>
#include "HIEventUtils/HIEventShapeMap.h"

#include <iostream>
#include <iomanip>

HIEventShapeFillerTool::HIEventShapeFillerTool(const std::string& myname)  : asg::AsgTool(myname),
									     m_evtShape(nullptr),
									     m_NumOrders(7),
									     m_index(nullptr),
									     m_useClusters(false)
{
  declareProperty("UseClusters", m_useClusters);
  declareProperty("TowerWeightTool",m_towerWeightTool);
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
   if(!m_useClusters) m_index->initializeEventShapeContainer(m_evtShape,m_NumOrders);
   return StatusCode::SUCCESS;
}



StatusCode HIEventShapeFillerTool::FillCollectionFromTowers(const std::string &tower_container_key)     
{
   //retrieve the tower container from store
  if(m_useClusters)
  {
    const xAOD::CaloClusterContainer* clusters=nullptr;
    CHECK(evtStore()->retrieve(clusters,tower_container_key));
    return FillCollectionFromClusterContainer(clusters);
  }
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
StatusCode HIEventShapeFillerTool::FillCollectionFromClusterContainer(const xAOD::CaloClusterContainer* theClusters)
{
  constexpr float area_slice=HI::TowerBins::getBinArea()*HI::TowerBins::numPhiBins();
  m_evtShape->reserve(HI::TowerBins::numEtaBins());
  for(unsigned int eb=0; eb< HI::TowerBins::numEtaBins(); eb++) 
  {
    xAOD::HIEventShape* e=new xAOD::HIEventShape();
    m_evtShape->push_back(e);
    e->setLayer(0);
    e->setEtaMin(HI::TowerBins::getBinLowEdgeEta(eb));
    e->setEtaMax(HI::TowerBins::getBinUpEdgeEta(eb));
    e->etCos().assign(m_NumOrders,0);
    e->etSin().assign(m_NumOrders,0);
    e->setArea(area_slice);
    e->setNCells(HI::TowerBins::numPhiBins());
  }
    

  std::unique_ptr<std::vector<float> > weight_vector(new std::vector<float>());
  weight_vector->reserve(theClusters->size());
  SG::AuxElement::Decorator< float > decorator("HIEtaPhiWeight");
  
  
  std::unique_ptr<std::vector<float> > cm_vector(new std::vector<float>());
  cm_vector->reserve(theClusters->size());
  SG::AuxElement::Decorator< float > cm_decorator("HIMag");
  
  constexpr float area_cluster=HI::TowerBins::getBinArea();
  
  
  if(m_towerWeightTool) CHECK(m_towerWeightTool->configureEvent());
  
  for(auto cl : *theClusters)
  {
    double ET=cl->e()/std::cosh(cl->eta0());
    double phi=cl->phi0();
    double eta=cl->eta0();
    unsigned int eb=HI::TowerBins::findBinEta(eta);
    xAOD::HIEventShape* slice=m_evtShape->at(eb);
    float weight=1;
    if(m_towerWeightTool) 
    {
      float recip=m_towerWeightTool->getEtaPhiResponse(eta,phi);
      if(recip!=0.) weight=1./recip;
    }
    weight_vector->push_back(weight);
    decorator(*cl)=weight;
    
    
    float etot2=0;
    float er2=0;

    for(unsigned int sample=0; sample<24; sample++)
    {
      CaloSampling::CaloSample s=static_cast<CaloSampling::CaloSample>(sample);
      if(!cl->hasSampling(s)) continue;
      float esamp=std::abs(cl->eSample(s));
      float w1=m_towerWeightTool->getWeight(eta,phi,s);
      float wr=m_towerWeightTool->getWeightMag(eta,phi,s);
      etot2+=esamp*w1;
      er2+=esamp*wr;
      
    }
    float cm=er2/etot2;
    cm_vector->push_back(cm);
    cm_decorator(*cl)=cm;
    
    //update members
    slice->setEt(slice->et()+weight*ET);
    slice->setRho(slice->rho() + weight*ET/area_cluster);

    for(unsigned int i=0; i<static_cast<unsigned int>(m_NumOrders); i++)
    {
      float nn=i+1;
      float tmp_cos = slice->etCos().at(i);
      slice->etCos()[i] = tmp_cos + weight*ET*std::cos(nn*phi);
      float tmp_sin = slice->etSin().at(i);
      slice->etSin()[i] = tmp_sin + weight*ET*std::sin(nn*phi);
    }
  }
  // std::cout << "DUMPING HIEVENTSHAPE" << std::endl;
  // for(auto es : *m_evtShape)
  // {
  //   std::cout << std::setw(10) << es->etaMin()
  // 	      << std::setw(10) << es->etaMax()
  // 	      << std::setw(15) << es->et()*1e-3
  // 	      << std::endl;
  //   for(unsigned int i=0; i<static_cast<unsigned int>(m_NumOrders); i++)
  //   {
  //     std::cout << std::setw(40) << i
  // 		<< std::setw(15) << es->etCos().at(i)
  // 		<< std::setw(15) << es->etSin().at(i)
  // 		<< std::endl;
  //   }

  // }
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

