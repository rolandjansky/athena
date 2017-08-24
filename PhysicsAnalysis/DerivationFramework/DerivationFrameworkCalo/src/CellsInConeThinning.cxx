/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "xAODEgamma/EgammaContainer.h"

#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"

#include "DerivationFrameworkCalo/CellsInConeThinning.h"
#include "DerivationFrameworkCalo/CellsInCone.h"

DerivationFramework::CellsInConeThinning::CellsInConeThinning(const std::string& type, 
							      const std::string& name, 
							      const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_SGKey("Electrons"),
  m_InputCellsSGKey("AllCalo"),
  m_OutputClusterSGKey("EgammaDummyClusters"),
  m_selectionString(""),
  m_dr(0.5),
  m_parser(0)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("InputSGKey",  m_SGKey="Electrons");		
  declareProperty("InputCellsSGKey",m_InputCellsSGKey="AllCalo");		
  declareProperty("OutputClusterSGKey",m_OutputClusterSGKey="dummyCluster");		
  declareProperty("deltaR",m_dr=0.5);		
}

StatusCode DerivationFramework::CellsInConeThinning::initialize(){

  if (m_selectionString!="") {
    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
    if (m_selectionString!="") {
      m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
      m_parser->loadExpression(m_selectionString);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CellsInConeThinning::finalize(){
  if (m_selectionString!="") {
    delete m_parser;
    m_parser = 0;
  }
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CellsInConeThinning::addBranches() const{

  ///Make new container
  xAOD::CaloClusterContainer* dummyClusterContainer = CaloClusterStoreHelper::makeContainer(&*evtStore(),                                                                        
											    m_OutputClusterSGKey,                                                         
											    msg());                                                                              
  /// Input objects
  const xAOD::EgammaContainer* egammas = evtStore()->retrieve< const xAOD::EgammaContainer >(m_SGKey);
  if(!egammas ) {
    ATH_MSG_ERROR( "Couldn't retrieve egamma container with key: " <<m_SGKey);
    return StatusCode::FAILURE;
  }

  unsigned int nEgammas(egammas->size());
  if (nEgammas==0) {
    return StatusCode::SUCCESS;
  }

  const CaloCellContainer* cells = evtStore()->retrieve< const CaloCellContainer >(m_InputCellsSGKey);
  if(!cells) {
      ATH_MSG_ERROR( "Couldn't retrieve cell container with key: " <<m_InputCellsSGKey);
      return StatusCode::FAILURE;
  }
  //We have a selection string 
  if (m_selectionString!="") {
    std::vector<int> entries =  m_parser->evaluateAsVector();
    unsigned int nEntries = entries.size();
    // check the sizes are compatible
    if (nEgammas!= nEntries ) {
      ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used e-gamma objects");
      return StatusCode::FAILURE;
    }
    //Loop over the egammas, pick the selected ones and fill the cluster
    int index(0);
    for (const xAOD::Egamma* eg : *egammas){
      if(entries.at(index)==true){
	xAOD::CaloCluster *dummy = CaloClusterStoreHelper::makeCluster(cells);
	DerivationFramework::CellsInCone::egammaSelect(dummy,cells,eg,m_dr);
	dummyClusterContainer->push_back(dummy);
      }
      ++index;
    }
  }
  /// Fill clusters without selection string
  else{
    for (const xAOD::Egamma* eg : *egammas){
      xAOD::CaloCluster *dummy = CaloClusterStoreHelper::makeCluster(cells);
      DerivationFramework::CellsInCone::egammaSelect(dummy,cells,eg,m_dr);
      dummyClusterContainer->push_back(dummy);
    }
  }
  ///Finalize clusters
  CHECK( CaloClusterStoreHelper::finalizeClusters(&*evtStore(),                                                                                                                    
						  dummyClusterContainer,                                           
						  m_OutputClusterSGKey,                                          
						  msg()));               
  ///Return
  return StatusCode::SUCCESS;
}
////

