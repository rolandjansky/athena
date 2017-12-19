/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaTopoClusterCopier.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

class greater{
public:
  bool operator()(xAOD::CaloCluster const *a, xAOD::CaloCluster const *b) const { 

    //TieGap Recovered (true 1) should after (ascending) the no recovered (false 0) 
    static const SG::AuxElement::Accessor<bool> acc("isCrackRecovered");
    if(acc.isAvailable(*a) && acc.isAvailable(*b)) {
      if(acc(*a)!=acc(*b)){
	return acc(*a) < acc(*b);
      }
    }
    //
    //Order with EM Pt (descending)
    double emfrac_a = 0;
    double emfrac_b = 0;
    if(a->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emfrac_a)&&
       b->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emfrac_b)){
      return (a->pt()*emfrac_a) > (b->pt()*emfrac_b); 
    }
    //Generic catch all case
    return a->pt() > b->pt();   
  }
};

// =============================================================
egammaTopoClusterCopier::egammaTopoClusterCopier(const std::string& name, 
						 ISvcLocator* pSvcLocator): 
  AthReentrantAlgorithm(name, pSvcLocator) 
{
}

// =============================================================
StatusCode egammaTopoClusterCopier::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");
  
  ATH_CHECK(m_inputTopoCollection.initialize());
  ATH_CHECK(m_outputTopoCollection.initialize());
  ATH_CHECK(m_outputTopoCollectionShallow.initialize());

  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTopoClusterCopier::finalize() {
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTopoClusterCopier::execute_r(const EventContext& ctx) const {

  SG::ReadHandle<xAOD::CaloClusterContainer> inputTopoclusters(m_inputTopoCollection, ctx);
  SG::WriteHandle<xAOD::CaloClusterContainer> outputTopoclustersShallow(m_outputTopoCollectionShallow, ctx);
  SG::WriteHandle<ConstDataVector <xAOD::CaloClusterContainer> > outputTopoclusters(m_outputTopoCollection, ctx);

  //Create a shallow copy, the elements of this can be modified , 
  //but no need to recreate the cluster
  std::pair<xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer* > inputShallowcopy = xAOD::shallowCopyContainer(*inputTopoclusters );

  ATH_CHECK( outputTopoclustersShallow.record(std::unique_ptr<xAOD::CaloClusterContainer>(inputShallowcopy.first), 
					      std::unique_ptr<xAOD::ShallowAuxContainer>(inputShallowcopy.second)) );

  //Here it just needs to be a view copy , 
  //i.e the collection we create does not really 
  //own its elements
  auto viewCopy =  std::make_unique<ConstDataVector <xAOD::CaloClusterContainer> >(SG::VIEW_ELEMENTS );

  //Loop over the shallow copy
  xAOD::CaloClusterContainer::iterator cciter = outputTopoclustersShallow->begin();
  xAOD::CaloClusterContainer::iterator ccend  = outputTopoclustersShallow->end();
  for (; cciter != ccend; ++cciter) {

    ATH_MSG_DEBUG("->CHECKING Cluster at eta,phi,et " << (*cciter)->eta() << " , "<< (*cciter)->phi() << " , " << (*cciter)->et());
    if( fabs((*cciter)->eta()) > m_etaCut  // if it falls outside eta region
	|| (*cciter)->e() < m_ECut      //Total energy below threshold
	) {            
      continue;
    }

    //Check if it passes the cuts
    float emfrac(0.);
    bool  passesCuts(checkEMFraction(*cciter, emfrac));

    //Special condition for crack on EM fraction OR EM Et due to observed
    //fragmentation of showers.
    bool isCrack(fabs((*cciter)->eta()) > 1.37 && fabs((*cciter)->eta()) < 1.52);
    if(isCrack){
      passesCuts = (passesCuts || (emfrac*(*cciter)->et()) > m_EMCrackEtCut);
    }
    if (!passesCuts){
      continue;
    }
    //Clone the cluster 
    CaloClusterKineHelper::calculateKine(*cciter,true,true);
    ATH_MSG_DEBUG("-->SELECTED Cluster at eta,phi,et " << (*cciter)->eta() << " , "<< (*cciter)->phi() << " , " << (*cciter)->et());
    viewCopy->push_back((*cciter));
  }
  //sort in descenting em energy
  std::sort(viewCopy->begin(),viewCopy->end(), greater());
  ATH_MSG_DEBUG("Cloned container has size: " << viewCopy->size()<<  " selected out of : " <<inputTopoclusters->size());
  ATH_CHECK( outputTopoclusters.record(std::move(viewCopy)) );

  return StatusCode::SUCCESS;
}

StatusCode egammaTopoClusterCopier::checkEMFraction (const xAOD::CaloCluster *clus, float &emFrac) const{
  
  double emfrac(0);
  if(!clus->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emfrac)){
    ATH_MSG_ERROR("No EM fraction momement stored");
    return StatusCode::FAILURE;
  } 
  ///
  double clusterE= clus->e();
  const bool pass_no_correction= ( (emfrac>m_EMFracCut) &&  ( (clusterE*emfrac) > m_ECut) );
  ATH_MSG_DEBUG("Initial emfrac: " <<emfrac);
  //
  double aeta= fabs(clus->eta());
  // Try to add the TileGap cells to its EM energy 
  // For crack clusters, also want to consider EME0, EMB0
  if(aeta>1.37 && aeta<1.63 && clusterE>0){
    double EMEnergy= clusterE*emfrac;
    xAOD::CaloCluster::const_cell_iterator cell_itr = clus->begin();
    xAOD::CaloCluster::const_cell_iterator cell_end = clus->end();   

    for (; cell_itr != cell_end; ++cell_itr) { 
      const CaloCell* cell = *cell_itr; 
      if (!cell){
	continue;
      }    

      const CaloDetDescrElement *dde = cell->caloDDE();
      if(!dde){
	continue;
      }
      //Add TileGap3. Consider only E4 cell
      if (CaloCell_ID::TileGap3 == dde->getSampling()) {
	if( fabs(cell->eta()) >1.4 && fabs(cell->eta()) < 1.6 ){	  
	  EMEnergy += cell->e()*cell_itr.weight();
	}
      }

      //Add also cells from EMB0, EME0 for crack.
      if (CaloCell_ID::PreSamplerE == dde->getSampling()) {
	EMEnergy += cell->e()*cell_itr.weight();
      }
      if (CaloCell_ID::PreSamplerB == dde->getSampling()) {
	EMEnergy += cell->e()*cell_itr.weight();
      }

      emfrac = EMEnergy/clusterE;
    }
    ATH_MSG_DEBUG("Corrected emfrac for E4 in TileGap3: " <<emfrac);
  }
  //
  static const  SG::AuxElement::Decorator<float> acc("EMFraction");
  acc(*clus)=emfrac;
  //Did it pass after correction
  const bool pass_after_correction= ((emfrac>m_EMFracCut) &&  ( (clusterE*emfrac) > m_ECut));
  static const SG::AuxElement::Decorator<bool> acc1("isCrackRecovered");
  acc1(*clus)= (pass_no_correction!=pass_after_correction);
  ATH_MSG_DEBUG("Cluster need to be recovered " << (pass_no_correction!=pass_after_correction));
  //
  emFrac = emfrac;
  //Could re
  return StatusCode(pass_after_correction);
}
  


