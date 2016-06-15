/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaTopoClusterCopier.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "AthContainers/ConstDataVector.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

class greater{
public:
  bool operator()(xAOD::CaloCluster const *a, xAOD::CaloCluster const *b) const { 

    double emfrac_a = 0;
    double emfrac_b = 0;
    if(a->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emfrac_a)&&
       b->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emfrac_b)){
      return (a->pt()*emfrac_a) > (b->pt()*emfrac_b); 
    }
    return a->pt() > b->pt();   
  }
};

// =============================================================
egammaTopoClusterCopier::egammaTopoClusterCopier(const std::string& type, const std::string& name, const IInterface* parent) :
  egammaBaseTool(type, name, parent)
{
  declareProperty("InputTopoCollection", m_inputTopoCollection = "CaloTopoCluster");
  declareProperty("OutputTopoCollection", m_outputTopoCollection = "egammaTopoCluster");
  declareProperty("IsHadronic", m_isHadronic  = true);
  declareProperty("EtaCut", m_etaCut = 2.5);
  declareProperty("ECut", m_ECut = 400);
  declareProperty("EMFracCut", m_EMFracCut = 0.5);
  declareInterface<IegammaTopoClusterCopier>(this);
}

// =============================================================
StatusCode egammaTopoClusterCopier::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTopoClusterCopier::finalize() {
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTopoClusterCopier::contExecute() {
  if(m_isHadronic){
    return copyCaloTopo();
  }
  //The default should be hadronic.
  //Now job option to avoid hardcoding names.
  //Remove this method when not furhter EMTopo tests are needed
  return copyEMTopo();
}

// =========================================================================
StatusCode egammaTopoClusterCopier::copyEMTopo() const {

  //Remove this method when not furhter EMTopo tests are needed
  //This code adds the "SisterLink" also in the EM case.
  ATH_MSG_DEBUG("Making clone of EM input clusters");
  const xAOD::CaloClusterContainer* input_topoclusters(0);
  if (evtStore()->retrieve(input_topoclusters,m_inputTopoCollection).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve cluster container " <<m_inputTopoCollection);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved input cluster container " << m_inputTopoCollection);
  }
  //Register container in StoreGate
  xAOD::CaloClusterContainer *clusters  =   CaloClusterStoreHelper::makeContainer(&*evtStore(),
										  m_outputTopoCollection,
										  msg());
  //Do the actual coping 
  xAOD::CaloClusterContainer::const_iterator cciter = input_topoclusters->begin();
  xAOD::CaloClusterContainer::const_iterator ccend  = input_topoclusters->end();
  int index(0);
  for (; cciter != ccend; ++cciter,++index) {
    //Make a few basic quality cuts to ensure we're only carrying over the most needed
    if( fabs((*cciter)->eta()) > m_etaCut || (*cciter)->e() < m_ECut){	
      continue;
    }
    //Clone the cluster 
    xAOD::CaloCluster *clone_clus = new xAOD::CaloCluster(*(*cciter));

    //Set the FRAC_EM to 1 if not there
    double emfrac(0);
    if(!clone_clus->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emfrac)){
      ATH_MSG_INFO("Setting the ENG_FRAC_EM moment to 1 ");
      clone_clus->insertMoment(xAOD::CaloCluster::ENG_FRAC_EM,1.0);
    }
    //Add element link to the original.
    ElementLink< xAOD::CaloClusterContainer > clusLink(*input_topoclusters, index);
    static const SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > caloCalClusterLink("SisterCluster");
    caloCalClusterLink(*clone_clus)= clusLink;
    clusters->push_back(clone_clus);
  } 
  ATH_MSG_DEBUG("Cloned container has size: " << clusters->size()<<  " selected out of : " <<input_topoclusters->size());
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode egammaTopoClusterCopier::copyCaloTopo() const{

  ATH_MSG_DEBUG("Create " << m_outputTopoCollection << " from " << m_inputTopoCollection);

  //First read the Topo Clusters before calibration
  const xAOD::CaloClusterContainer* input_topoclusters(0);
  if (evtStore()->retrieve(input_topoclusters,m_inputTopoCollection).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve cluster container " <<m_inputTopoCollection);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved input cluster -container " << m_inputTopoCollection);
  }

  //Here it just needs to be a view copy, as all info is provided
  ConstDataVector<xAOD::CaloClusterContainer>* viewCopy =  new ConstDataVector <xAOD::CaloClusterContainer> (SG::VIEW_ELEMENTS );
  CHECK(evtStore()->record(viewCopy, m_outputTopoCollection));

  xAOD::CaloClusterContainer::const_iterator cciter = input_topoclusters->begin();
  xAOD::CaloClusterContainer::const_iterator ccend  = input_topoclusters->end();
  for (; cciter != ccend; ++cciter) {
    double emfrac(0);
    if(!(*cciter)->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emfrac)){
      ATH_MSG_ERROR("No EM fraction momement stored");
      return StatusCode::FAILURE;
    }
    //Check if it passes the cuts
    if(
       fabs((*cciter)->eta()) > m_etaCut  // if it falls outside eta region
       || emfrac<m_EMFracCut                  // EMFrac not high enough
       || ((*cciter)->e()*emfrac) < m_ECut     //EM energy below threshold
       || (*cciter)->e() < m_ECut              //Total energy below threshold
       ){
      continue;
    }
    //Clone the cluster 
    viewCopy->push_back((*cciter));
  }
  //sort in descenting em energy
  std::sort(viewCopy->begin(),viewCopy->end(), greater());
  ATH_MSG_DEBUG("Cloned container has size: " << viewCopy->size()<<  " selected out of : " <<input_topoclusters->size());
  return StatusCode::SUCCESS;
}
