/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHIEventShapeJetIteration.h"
#include "xAODHIEvent/HIEventShape.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/HIEventShapeSummaryUtils.h"
#include "xAODJet/JetContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

TrigHIEventShapeJetIteration::TrigHIEventShapeJetIteration(std::string name) : AsgTool(name),
								       m_isInit(false)
{

  declareProperty("InputClustersKey",m_input_clusters_key="HIClusters");
  declareProperty("InputEventShapeKey",m_input_event_shape_key="HIEventShape");
  declareProperty("OutputEventShapeKey",m_output_event_shape_key="HIEventShape_mod");
  declareProperty("SeedContainerKey",m_seed_key,"Names of seed collections");
  declareProperty("ExclusionRadius",m_exclude_DR=0.4,"Exclude all calo regions w/in this DR to jet");
  declareProperty("ExcludeConstituents",m_exclude_constituents=false,"Only exclude constituents of jets");
  declareProperty("AssociationKey",m_association_key,"Name of jet attribute containing jet-cluster association");
  declareProperty("Subtractor",m_subtractor_tool);
}

int TrigHIEventShapeJetIteration::execute() const
{
  ///
  const xAOD::CaloClusterContainer* clusColl=0;
  if(! (evtStore()->retrieve(clusColl, m_input_clusters_key).isFailure()) )
    {ATH_MSG_DEBUG("Clusters successfully retrieved as CaloClusterContainer");
    }
  else
    {ATH_MSG_WARNING("Clusters could not be retrieved as CaloClusterContainer!");
    }

  const xAOD::HIEventShapeContainer* input_shape=0;
  ATH_MSG_DEBUG("In " << (name() ).c_str() << ": ");
  ATH_MSG_DEBUG("Retrieving event shape: " << m_input_event_shape_key.c_str() << " ... in TrigHIEventShapeJetIteration");
  CHECK(evtStore()->retrieve(input_shape,m_input_event_shape_key), 1);

  const xAOD::JetContainer* theJets=0;
  ATH_MSG_DEBUG("Retrieving seeds: " << m_seed_key.c_str() << " ... in TrigHIEventShapeJetIteration");
  CHECK(evtStore()->retrieve(theJets,m_seed_key), 1);


   //shallow copy shares unaltered auxilliary data with the original container.
  ATH_MSG_DEBUG("Recording shallow copy of event shape: " << m_output_event_shape_key.c_str() << " ... in TrigHIEventShapeJetIteration");
  auto shape_copy=xAOD::shallowCopyContainer(*input_shape);
  CHECK(evtStore()->record(shape_copy.first,m_output_event_shape_key), 1);
  CHECK(evtStore()->record(shape_copy.second,m_output_event_shape_key + "Aux."), 1);
  xAOD::HIEventShapeContainer* output_shape=shape_copy.first;

  std::string unaltered_input_event_shape_key = "TrigHIEventShape";

  const HIEventShapeIndex* es_index=HIEventShapeMap::getIndex(unaltered_input_event_shape_key); //m_input_event_shape_key);
  if(es_index==nullptr)
  {
    ATH_MSG(ERROR) << "No HIEventShapeIndex w/ name " << m_input_event_shape_key << endmsg;
    return 1;
  }
  if(!m_isInit)
  {
    HIEventShapeMap::getMap()->insert(m_output_event_shape_key,*es_index);
    m_isInit=true;
  }

  std::set<unsigned int> used_indices;
  std::set<unsigned int> used_eta_bins;
  for(xAOD::JetContainer::const_iterator jItr=theJets->begin(); jItr!=theJets->end(); jItr++)
  {
    const xAOD::Jet* theJet=(*jItr);
    xAOD::IParticle::FourMom_t jet4mom=theJet->p4();
    
    /*
    std::vector<const xAOD::IParticle*> assoc_clusters;
    //only use jet constituents
    if(m_exclude_constituents) assoc_clusters=theJet->getConstituents().asIParticleVector();
    
    //use associations w/ name m_association_key
    else
    {
      if(! theJet->getAssociatedObjects<xAOD::IParticle>(m_association_key,assoc_clusters))
      {
	//this should only happen if SOME of the jets in the collection are missing the association
	//technically possible, but should NOT happen
	ATH_MSG(ERROR) << "Individual jet missing association " << m_association_key << endmsg;
	return StatusCode::FAILURE;
      }
    }
    
    //Now loop over associated objects
    ATH_MSG(DEBUG) << "Jet " 
		  << std::setw(10) << jet4mom.Pt()*1e-3
		  << std::setw(10) << jet4mom.Eta()
		  << std::setw(10) << jet4mom.Phi()
		  << std::setw(10) << jet4mom.E()*1e-3
		  << std::setw(25) << " has " << assoc_clusters.size() << " assoc. clusters"
		  << endmsg;
    */

    /*
    for(auto pItr = assoc_clusters.begin(); pItr!=assoc_clusters.end(); pItr++)
    {
      if( jet4mom.DeltaR( (*pItr)->p4() ) > m_exclude_DR ) continue;
      const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(*pItr);
      m_subtractor_tool->UpdateUsingCluster(output_shape,es_index,cl,used_indices,used_eta_bins);
    }
    */

    for(xAOD::CaloClusterContainer::const_iterator clusCollIter= clusColl->begin();
        clusCollIter!= clusColl->end(); clusCollIter++)
    {
      const xAOD::CaloCluster* cl = (*clusCollIter);

      if( jet4mom.DeltaR( cl->p4() ) > m_exclude_DR ) continue;

      m_subtractor_tool->UpdateUsingCluster(output_shape,es_index,cl); //,used_indices,used_eta_bins);
    }

  }//end jet loop

  return 0;
}




  //if configured, define layer-depedent ET-weighted, eta-independent v2 and necessary event plane angles
  //loop again on bins and manually set flow part?
  

  // xAOD::HIEventShapeContainer* output_flow_shape=new xAOD::HIEventShapeContainer;
  // xAOD::HIEventShapeAuxContainer *output_flow_shape_aux = new xAOD::HIEventShapeAuxContainer;
  // output_flow_shape->setStore( output_flow_shape_aux );
  // CHECK(evtStore()->record(output_flow_shape,m_output_event_flow_shape_key));
  // CHECK(evtStore()->record(output_flow_shape_aux,m_output_event_flow_shape_key + std::string("Aux.")));
  
  // //FIX remove hard coded value
  // int N_CALO_LAYERS = 24;
  // output_flow_shape->reserve(N_CALO_LAYERS);
  // for(unsigned int i=0; i<input_shape->size(); i++)
  // {
  //   const xAOD::HIEventShape* slice=(*input_shape)[slices];
  //   unsigned int eb=HICaloCellHelper::FindBinEta(0.5*(slice->eta_min()+slice->eta_max()));
  //   if(used_eta_bins.find(eb)!=used_eta_bins.end()) continue;
  //   //
  //   unsigned int layer=slice->layer();
  //   xAOD::HIEventShape* flow_slice=(*output_flow_shape)[layer];
  //   //do averaging

  // }


