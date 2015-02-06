/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIEventShapeJetIteration.h"
#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeAuxContainer.h>
#include <xAODJet/JetContainer.h>
#include <HIGlobal/HICaloHelper.h>
#include "CxxUtils/prefetch.h"

HIEventShapeJetIteration::HIEventShapeJetIteration(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name,pSvcLocator)
{
  declareProperty("InputEventShapeKey",m_input_event_shape_key="HIEventShape");
  declareProperty("OutputEventShapeKey",m_output_event_shape_key="HIEventShape_mod");
  declareProperty("SeedContainerKeys",m_jet_coll_keys,"Names of seed collections");
  declareProperty("ExclusionRadius",m_exclude_DR=0.4,"Exclude all calo regions w/in this DR to jet");
  declareProperty("ExcludeConstituents",m_exclude_constituents=false,"Only exclude constituents of jets");
  declareProperty("AssociationKey",m_association_key,"Name of jet attribute containing jet-cluster association");
}

StatusCode HIEventShapeJetIteration::initialize()
{
  return StatusCode::SUCCESS;
}
StatusCode HIEventShapeJetIteration::execute()
{
  const xAOD::HIEventShapeContainer* input_shape=0;
  CHECK(evtStore()->retrieve(input_shape,m_input_event_shape_key));
  
  
  //check that all collections are available
  //and that they have the association we need
  std::vector<const xAOD::JetContainer*> jet_coll_vec;
  for(std::vector<std::string>::const_iterator sItr=m_jet_coll_keys.begin();
      sItr!=m_jet_coll_keys.end(); sItr++)
  {
    std::string jetCollKey(*sItr);
    const xAOD::JetContainer* theJets=0;

    if(evtStore()->contains<xAOD::JetContainer>(jetCollKey)) 
    {
      ATH_MSG(INFO) << "Retrieved seed container " << jetCollKey << endreq;
      theJets = evtStore()->retrieve<const xAOD::JetContainer>(jetCollKey);
      //make sure jets have right association
      //ALL jets should have this attribute so checking the first is ok.
      //If only some of them have it this is checked upstream
      if(!m_exclude_constituents && theJets->size()>0)
      {
	const xAOD::Jet* theJet=*(theJets->begin());
	std::vector<const xAOD::IParticle*> assoc_clusters;
	if(! (theJet->getAssociatedObjects<xAOD::IParticle>(m_association_key,assoc_clusters)) )
	{
	  ATH_MSG(ERROR) << "Jet collection " << jetCollKey
			 << " has no attribute " << m_association_key << endreq;
	  return StatusCode::FAILURE;
	}
      }
    }
    
    else
    {
      ATH_MSG(ERROR) << "Could not retrieve " << jetCollKey << " from store" << endreq;
      return StatusCode::FAILURE;
    }
    
    jet_coll_vec.push_back(theJets);
  }
  




  //Make new HIEventShape object
  xAOD::HIEventShapeContainer* output_shape=new xAOD::HIEventShapeContainer;
  xAOD::HIEventShapeAuxContainer *output_shape_aux = new xAOD::HIEventShapeAuxContainer;
  output_shape->setStore( output_shape_aux );
  CHECK(evtStore()->record(output_shape,m_output_event_shape_key));
  CHECK(evtStore()->record(output_shape_aux,m_output_event_shape_key + std::string("Aux.")));

  //for now, deep (by hand) copy
  for(const xAOD::HIEventShape* slice : *input_shape)
  {
    xAOD::HIEventShape* slice_new=new xAOD::HIEventShape;
    output_shape->push_back(slice_new);
    *slice_new=*slice;
  }

  //HICaloHelper::CopyEventShapeContainer(input_shape,output_shape);

  //shallow copy shares unaltered auxilliary data with the original container.
  // auto shape_copy=xAOD::shallowCopyContainer(*input_shape);
  // CHECK(evtStore()->record(shape_copy.first,m_output_event_shape_key));
  // CHECK(evtStore()->record(shape_copy.second,m_output_event_shape_key + "Aux."));  
  // xAOD::HIEventShapeContainer* output_shape=shape_copy.first;


  std::set<unsigned int> used_indices;
  std::set<unsigned int> used_eta_bins;

  //now loop on collections and update event shape
  for(auto itr=jet_coll_vec.begin(); itr!=jet_coll_vec.end(); itr++)
  {
    const xAOD::JetContainer* theJets=(*itr);
    for(xAOD::JetContainer::const_iterator jItr=theJets->begin(); jItr!=theJets->end(); jItr++)
    {
      const xAOD::Jet* theJet=(*jItr);
      xAOD::IParticle::FourMom_t jet4mom=theJet->p4();

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
	  ATH_MSG(ERROR) << "Indiividual jet missing association " << m_association_key << endreq;
	  return StatusCode::FAILURE;
	}
      }

      //Now loop over associated objects
      ATH_MSG(INFO) << "Jet " 
		    << std::setw(10) << jet4mom.Pt()*1e-3
		    << std::setw(10) << jet4mom.Eta()
		    << std::setw(10) << jet4mom.Phi()
		    << std::setw(10) << jet4mom.E()*1e-3
		    << std::setw(25) << " has " << assoc_clusters.size() << " assoc. clusters"
		    << endreq;
      for(auto pItr = assoc_clusters.begin(); pItr!=assoc_clusters.end(); pItr++)
      {
	if( jet4mom.DeltaR( (*pItr)->p4() ) > m_exclude_DR ) continue;
	const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(*pItr);
	UpdateUsingCluster(output_shape,cl,used_indices,used_eta_bins);
	
      }
    
    }//end jet loop
    
  }// end loop on jet containers
	

  return StatusCode::SUCCESS;
}
StatusCode HIEventShapeJetIteration::finalize()
{
  return StatusCode::SUCCESS;
}


void HIEventShapeJetIteration::UpdateUsingCluster(xAOD::HIEventShapeContainer* shape, const xAOD::CaloCluster* cl, 
						  std::set<unsigned int>& used_indices, std::set<unsigned int>& used_eta_bins) const
{
  float eta0=cl->eta0();
  float phi0=cl->phi0();
  unsigned int index=HICaloHelper::FindEtaPhiBin(eta0,phi0);
  if(used_indices.find(index)==used_indices.end())
  {
    //update event shape!
    xAOD::CaloCluster::const_cell_iterator cellIterEnd = cl->cell_end();
    for(xAOD::CaloCluster::const_cell_iterator cellIter=cl->cell_begin(); cellIter != cellIterEnd; cellIter++ )
    {
      //ERROR SOMEWHERE IN HERE
      CxxUtils::prefetchNext(cellIter, cellIterEnd);	
      HICaloHelper::UpdateShape(shape,*cellIter,cellIter.weight(),eta0,phi0,true);
    }
    used_indices.insert(index);
    used_eta_bins.insert(HICaloHelper::FindBinEta(eta0));
  }

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
  //   unsigned int eb=HICaloHelper::FindBinEta(0.5*(slice->eta_min()+slice->eta_max()));
  //   if(used_eta_bins.find(eb)!=used_eta_bins.end()) continue;
  //   //
  //   unsigned int layer=slice->layer();
  //   xAOD::HIEventShape* flow_slice=(*output_flow_shape)[layer];
  //   //do averaging

  // }

