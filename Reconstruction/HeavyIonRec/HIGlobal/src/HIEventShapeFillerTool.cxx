/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/HIEventShapeFillerTool.h"
#include <Navigation/NavigationToken.h>
#include <CaloEvent/CaloCell.h>
#include <CaloEvent/CaloCellContainer.h>
#include <xAODHIEvent/HIEventShape.h>
#include "HIEventUtils/HIEventDefs.h"
#include "HIEventUtils/HIEventShapeMapTool.h"

#include <iostream>
#include <iomanip>

HIEventShapeFillerTool::HIEventShapeFillerTool(const std::string& myname)  : asg::AsgTool(myname),
									     m_evtShape(nullptr),
									     m_index(nullptr)
{
}

StatusCode  HIEventShapeFillerTool::setNumOrders(int Norders)
{
   m_numOrders=Norders;
   return StatusCode::SUCCESS;
}


StatusCode HIEventShapeFillerTool::initializeCollection(xAOD::HIEventShapeContainer *evtShape_)
{
   //change m_evtShape to m_evtShape
   m_evtShape=evtShape_;
   //tool is initialized only once
   if(!m_index)
   {
     m_index=m_eventShapeMapTool->getIndex( HI::BinningScheme::COMPACT );
   }
   //fix this to have proper name passing
   //use tool to initialize event shape object
   if(!m_useClusters) m_index->initializeEventShapeContainer(m_evtShape,m_numOrders);
   return StatusCode::SUCCESS;
}



StatusCode HIEventShapeFillerTool::fillCollectionFromTowers(const SG::ReadHandleKey<xAOD::CaloClusterContainer>  &tower_container_key)
{
   //retrieve the tower container from store
  if(m_useClusters)
  {
		SG::ReadHandle<xAOD::CaloClusterContainer>  readHandleCaloClus ( tower_container_key );
    const xAOD::CaloClusterContainer* clusters=nullptr;
		clusters = readHandleCaloClus.get();
    return fillCollectionFromClusterContainer(clusters);
  }
	SG::ReadHandle<INavigable4MomentumCollection>  readHandleINav ( tower_container_key.key() );
  const INavigable4MomentumCollection* navInColl = 0;
	navInColl = readHandleINav.get();
  return fillCollectionFromTowerContainer(navInColl);
}
StatusCode HIEventShapeFillerTool::fillCollectionFromTowerContainer(const INavigable4MomentumCollection* navInColl)
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
		cellItr != cellToken.end(); cellItr++ ) updateShape(m_evtShape,m_index,*cellItr,cellToken.getParameter(*cellItr),eta0,phi0);
  }//end tower loop
  return StatusCode::SUCCESS;
}

StatusCode HIEventShapeFillerTool::fillCollectionFromClusterContainer(const xAOD::CaloClusterContainer* theClusters)
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
    e->etCos().assign(m_numOrders,0);
    e->etSin().assign(m_numOrders,0);
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

		//HIMag back in rel 22 (removed by mistake in 21)
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

    for(unsigned int i=0; i<static_cast<unsigned int>(m_numOrders); i++)
    {
      float nn=i+1;
      float tmp_cos = slice->etCos().at(i);
      slice->etCos()[i] = tmp_cos + weight*ET*std::cos(nn*phi);
      float tmp_sin = slice->etSin().at(i);
      slice->etSin()[i] = tmp_sin + weight*ET*std::sin(nn*phi);
    }
  }
  ATH_MSG_DEBUG("DUMPING HIEVENTSHAPE");
  for(auto es : *m_evtShape)
  {
     ATH_MSG_DEBUG ( std::setw(10) << es->etaMin()
   	      << std::setw(10) << es->etaMax()
   	      << std::setw(15) << es->et()*1e-3 );

     for(unsigned int i=0; i<static_cast<unsigned int>(m_numOrders); i++)
     {
        ATH_MSG_DEBUG( std::setw(40) << i
    		<< std::setw(15) << es->etCos().at(i)
    		<< std::setw(15) << es->etSin().at(i) );
     }

  }
  return StatusCode::SUCCESS;
}

StatusCode HIEventShapeFillerTool::fillCollectionFromCells(const SG::ReadHandleKey<CaloCellContainer> &cell_container_key)
{
   //retrieve the cell container from store
	 SG::ReadHandle<CaloCellContainer>  read_handle_caloCell ( cell_container_key );
   const CaloCellContainer* CellContainer = read_handle_caloCell.get();
   return fillCollectionFromCellContainer(CellContainer);
}
StatusCode HIEventShapeFillerTool::fillCollectionFromCellContainer(const CaloCellContainer* CellContainer)
{
   //loop on Cells
   for(const auto cellItr : *CellContainer) updateShape(m_evtShape,m_index,cellItr,1.,cellItr->eta(),cellItr->phi());
   return StatusCode::SUCCESS;
}


void HIEventShapeFillerTool::updateShape(xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const CaloCell* theCell, float geoWeight, float eta0, float phi0, bool isNeg) const
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
