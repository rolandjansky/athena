/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaTopoClusterCopier.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include <stdexcept>

class greater{
public:
  bool operator()(xAOD::CaloCluster const *a, xAOD::CaloCluster const *b) const { 
    static const SG::AuxElement::Accessor<float> acc("EMFraction");
    //Order with EM Et (descending)
    double emfrac_a = acc(*a);
    double emfrac_b = acc(*b);
    return (a->et()*emfrac_a) > (b->et()*emfrac_b); 
  }
};

egammaTopoClusterCopier::egammaTopoClusterCopier(const std::string& name, 
                                                 ISvcLocator* pSvcLocator): 
  AthReentrantAlgorithm(name, pSvcLocator),
  m_AllClusters{},                                          
  m_PassPreSelection{},                                     
  m_PassSelection{} 
{
}

StatusCode egammaTopoClusterCopier::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  ATH_CHECK(m_inputTopoCollection.initialize());
  ATH_CHECK(m_outputTopoCollection.initialize());
  ATH_CHECK(m_outputTopoCollectionShallow.initialize());

  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

StatusCode egammaTopoClusterCopier::finalize() {
  
  ATH_MSG_INFO( name() << " All Clusters " << m_AllClusters );
  ATH_MSG_INFO( name() << " Pass Preselection Clusters " << m_PassPreSelection );
  ATH_MSG_INFO( name() << " Pass Selection " << m_PassSelection );
  
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTopoClusterCopier::execute(const EventContext& ctx) const {

  SG::ReadHandle<xAOD::CaloClusterContainer> inputTopoclusters(m_inputTopoCollection, ctx);
  SG::WriteHandle<xAOD::CaloClusterContainer> outputTopoclustersShallow(m_outputTopoCollectionShallow, ctx);
  SG::WriteHandle<ConstDataVector <xAOD::CaloClusterContainer> > outputTopoclusters(m_outputTopoCollection, ctx);

  /*Create a shallow copy, the elements of this can be modified , 
   * but no need to recreate the cluster
   */
  std::pair<xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer* > 
    inputShallowcopy = xAOD::shallowCopyContainer(*inputTopoclusters );

  ATH_CHECK( outputTopoclustersShallow.record(std::unique_ptr<xAOD::CaloClusterContainer>(inputShallowcopy.first), 
                                              std::unique_ptr<xAOD::ShallowAuxContainer>(inputShallowcopy.second)) );
  /*
   * Here it just needs to be a view copy , 
   * i.e the collection we create does not really 
   * own its elements
   */
  auto viewCopy =  std::make_unique<ConstDataVector <xAOD::CaloClusterContainer> >(SG::VIEW_ELEMENTS );
  /*
   * Special egamma EMFraction which includes presampler
   */
  static const SG::AuxElement::Accessor<float> acc("EMFraction");

  auto buff_AllClusters=m_AllClusters.buffer();                                                                                                       
  auto buff_PassPreSelection=m_PassPreSelection.buffer();                                                                                                       
  auto buff_PassSelection=m_PassSelection.buffer();                                                                                                       
  //Loop over the shallow copy
  for (xAOD::CaloCluster* clus :  *outputTopoclustersShallow) {
    ++buff_AllClusters;
    //Always decorate
    acc(*clus)=0.0;
    double clusterE=clus->e();
    //First pass using basic checks   
    ATH_MSG_DEBUG("->CHECKING Cluster at eta,phi,et " << clus->eta() << " , "
                  << clus->phi() << " , " << clus->et());
    if( fabs(clus->eta()) > m_etaCut || // if it falls outside eta region
        clusterE < m_ECut               // Total energy below threshold
      ) {            
      continue;
    }
    ++buff_PassPreSelection;
    //Calculate Kine for the cluster 
    CaloClusterKineHelper::calculateKine(clus,true,true);
    float emfrac= (clus->energyBE(0) + clus->energyBE(1) +clus->energyBE(2) +clus->energyBE(3))/clusterE;
    acc(*clus)=emfrac; 
    if( emfrac > m_EMFracCut && 
        (clusterE*emfrac) > m_ECut ){
      ++buff_PassSelection;   
      ATH_MSG_DEBUG("-->Selected Cluster at eta,phi,et,EMFraction " << clus->eta() 
                  << " , "<< clus->phi() << " , " << clus->et()<<" , "<< emfrac);
      viewCopy->push_back(clus);
   }
  }
  //sort in descenting em energy
  std::sort(viewCopy->begin(),viewCopy->end(), greater());
  ATH_MSG_DEBUG("Cloned container has size: " << viewCopy->size()<<  " selected out of : " 
                <<inputTopoclusters->size());
  ATH_CHECK( outputTopoclusters.record(std::move(viewCopy)) );

  return StatusCode::SUCCESS;
}

