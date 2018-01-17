/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIEventShapeJetIteration.h"
#include "xAODHIEvent/HIEventShape.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/HIEventShapeSummaryUtils.h"
#include "xAODCore/ShallowCopy.h"


namespace
{
  struct SelectByList{

    SelectByList() : used_set(nullptr) {};
    const std::set<unsigned int>* used_set;
    bool operator()(const xAOD::HIEventShape* in_slice)
    {
      unsigned int eb=HI::TowerBins::findBinEta(0.5*(in_slice->etaMin()+in_slice->etaMax()));
      return (used_set->find(eb)==used_set->end());
    }

  };
} //annonymous namespace


HIEventShapeJetIteration::HIEventShapeJetIteration(std::string name) : AsgTool(name),
								       m_isInit(false)
{
  declareProperty("InputEventShapeKey",m_input_event_shape_key="HIEventShape");
  declareProperty("OutputEventShapeKey",m_output_event_shape_key="HIEventShape_iter");
  declareProperty("CaloJetSeedContainerKey",m_calo_jet_seed_key,"Names of seed collections");
  declareProperty("TrackJetSeedContainerKey",m_track_jet_seed_key,"Names of seed collections");
  declareProperty("ExclusionRadius",m_exclude_DR=0.4,"Exclude all calo regions w/in this DR to jet");
  declareProperty("ExcludeConstituents",m_exclude_constituents=false,"Only exclude constituents of jets");
  declareProperty("AssociationKey",m_association_key,"Name of jet attribute containing jet-cluster association");
  declareProperty("Subtractor",m_subtractor_tool);
  declareProperty("Modulator",m_modulator_tool);
  declareProperty("RemodulateUE",m_do_remodulation=false,"Correct UE for incomplete cancellation of flow harmonics when iterating");
  declareProperty("ModulationScheme",m_modulation_scheme=0,"Scheme to build separate ES object for flow modulation");
  declareProperty("ModulationEventShapeKey",m_modulation_key="HIEventShape_itr_mod");

}

int HIEventShapeJetIteration::execute() const
{
  ///
  const xAOD::HIEventShapeContainer* input_shape=nullptr;
  xAOD::HIEventShapeContainer* output_shape=nullptr;
  getShapes(input_shape,output_shape,true).ignore();


  const HIEventShapeIndex* es_index=HIEventShapeMap::getIndex(m_input_event_shape_key);
  
  const xAOD::JetContainer* theCaloJets=0;
  const xAOD::JetContainer* theTrackJets=0;

  if(m_calo_jet_seed_key.compare("")!=0) ATH_CHECK(evtStore()->retrieve(theCaloJets,m_calo_jet_seed_key), 1);
  if(m_track_jet_seed_key.compare("")!=0) ATH_CHECK(evtStore()->retrieve(theTrackJets,m_track_jet_seed_key), 1);

  if(theTrackJets && m_exclude_constituents) 
  {
    ATH_MSG_ERROR("Incompatible options. Cannot track jets and constituents together.");
    return 1;
  }

  std::set<unsigned int> used_indices;
  std::set<unsigned int> used_eta_bins;

  std::vector<const xAOD::CaloCluster*> assoc_clusters;
  assoc_clusters.reserve(6400);
  if(theCaloJets) ATH_CHECK(makeClusterList(assoc_clusters,theCaloJets,used_indices,used_eta_bins), 1);
  if(theTrackJets) ATH_CHECK(makeClusterList(assoc_clusters,theTrackJets,used_indices,used_eta_bins), 1);
  
  updateShape(output_shape,assoc_clusters,es_index);
  
  //compute ES for modulation
  if(m_modulation_scheme)
  {
    xAOD::HIEventShapeContainer* modShape=new xAOD::HIEventShapeContainer;
    xAOD::HIEventShapeAuxContainer *modShapeAux = new xAOD::HIEventShapeAuxContainer;
    modShape->setStore( modShapeAux );
    ATH_CHECK( evtStore()->record(modShape,m_modulation_key), 1 );
    ATH_CHECK( evtStore()->record(modShapeAux,m_modulation_key+std::string("Aux.")), 1 );
    
    xAOD::HIEventShape* ms=new xAOD::HIEventShape();
    modShape->push_back(ms);
    ATH_CHECK( fillModulatorShape(ms,output_shape,used_indices,m_modulation_scheme), 1 );
    if(m_do_remodulation) ATH_CHECK( remodulate(output_shape,ms,used_indices), 1 );
  }
  return 0;
}

StatusCode HIEventShapeJetIteration::makeClusterList(std::vector<const xAOD::CaloCluster*>& particleList, const xAOD::JetContainer* theJets,
						     std::set<unsigned int>& used_indices, std::set<unsigned int>& used_eta_bins) const
{
  for(xAOD::JetContainer::const_iterator jItr=theJets->begin(); jItr!=theJets->end(); jItr++)
  {
    const xAOD::Jet* theJet=(*jItr);
    xAOD::IParticle::FourMom_t jet4mom=theJet->p4();
    std::vector<const xAOD::IParticle*> assoc_clusters;
    //only use jet constituents
    if(m_exclude_constituents) assoc_clusters=theJet->getConstituents().asIParticleVector();
    else
    {
      if(! (theJet->getAssociatedObjects<xAOD::IParticle>(m_association_key,assoc_clusters)) )
      {
	//this should only happen if SOME of the jets in the collection are missing the association
	//technically possible, but should NOT happen
	ATH_MSG_ERROR("Individual jet missing association " << m_association_key);
	return StatusCode::FAILURE;
      }
    }
    ATH_MSG_DEBUG( "Jet " << std::setw(10) << jet4mom.Pt()*1e-3
		   << std::setw(10) << jet4mom.Eta()
		   << std::setw(10) << jet4mom.Phi()
		   << std::setw(10) << jet4mom.E()*1e-3
		   << std::setw(25) << " has " << assoc_clusters.size() << " assoc. clusters");
    
    for(auto pItr = assoc_clusters.begin(); pItr!=assoc_clusters.end(); pItr++)
    {
      if( jet4mom.DeltaR( (*pItr)->p4() ) >  m_exclude_DR ) continue;
      const xAOD::CaloCluster* cl_ptr=static_cast<const xAOD::CaloCluster*>(*pItr);
      unsigned int tower_index=HI::TowerBins::findEtaPhiBin(cl_ptr->eta0(),cl_ptr->phi0());
      
      if(used_indices.insert(tower_index).second)
      {
	particleList.push_back(cl_ptr);
	used_eta_bins.insert(HI::TowerBins::findBinEta(cl_ptr->eta0()));
      }
    }
  }
  return StatusCode::SUCCESS;

}

StatusCode HIEventShapeJetIteration::makeClusterList(std::vector<const xAOD::CaloCluster*>& particleList, const xAOD::JetContainer* theJets) const
{
  std::set<unsigned int> used_indices;
  std::set<unsigned int> used_eta_bins;
  return makeClusterList(particleList,theJets,used_indices,used_eta_bins);
}
StatusCode HIEventShapeJetIteration::makeClusterList(std::vector<const xAOD::CaloCluster*>& particleList, const std::vector<const xAOD::JetContainer*>& theJets_vector) const
{
  std::set<unsigned int> used_indices;
  std::set<unsigned int> used_eta_bins;
  for(auto theJets : theJets_vector) 
  {
    ATH_CHECK(makeClusterList(particleList,theJets,used_indices,used_eta_bins));
  }
  return StatusCode::SUCCESS;
}

void HIEventShapeJetIteration::updateShape(xAOD::HIEventShapeContainer* output_shape, const std::vector<const xAOD::CaloCluster*>& assoc_clusters, const HIEventShapeIndex* es_index ) const
{
  if(es_index==nullptr)
  {
    es_index=HIEventShapeMap::getIndex(m_input_event_shape_key);
    if(es_index==nullptr)
    {
      ATH_MSG_INFO("No HIEventShapeIndex w/ name " << m_input_event_shape_key << " adding it to the map");
      HIEventShapeIndex* h=new HIEventShapeIndex();
      h->setBinning(output_shape);
      es_index=HIEventShapeMap::insert(m_input_event_shape_key,*h);
      
    }
  }
  for(auto pItr = assoc_clusters.begin(); pItr!=assoc_clusters.end(); pItr++) 
    m_subtractor_tool->UpdateUsingCluster(output_shape,es_index,(*pItr));

  if(!m_subtractor_tool->usesCells())
  {
    for(auto s : *output_shape)
    {
      float sum_et=s->et();
      float sum_nc=0.;
      if(sum_et!=0.) sum_nc=s->rho()*s->area()/sum_et;
      s->setNCells(sum_nc);
    }
  }
}

StatusCode HIEventShapeJetIteration::fillModulatorShape(xAOD::HIEventShape* ms, const xAOD::HIEventShapeContainer* output_shape, const std::set<unsigned int>& used_eta_bins, unsigned int scheme) const
{
  if(scheme==1 || scheme==2)
  {
    SelectByList selector;
    selector.used_set=&used_eta_bins;
    HI::fillSummary(output_shape,ms,selector);
  }

  if(scheme > 1)
  {
    const xAOD::HIEventShapeContainer* summary_container=0;
    ATH_CHECK(evtStore()->retrieve(summary_container,"CaloSums"));
    const xAOD::HIEventShape* s_fcal=0;
    for(unsigned int i=0; i<summary_container->size(); i++)
    {
      const xAOD::HIEventShape* sh=(*summary_container)[i];
      std::string summary;
      if(sh->isAvailable<std::string>("Summary")) summary=sh->auxdata<std::string>("Summary");
      if(summary.compare("FCal")==0) 
      {
	s_fcal=sh;
	break;
      }
    }
    if (!s_fcal) std::abort();
    if(scheme==3) (*ms)=(*s_fcal);
    else
    {
      float et_fcal=s_fcal->et();
      float et_fcal_recip=0;
      if(et_fcal > 0.) et_fcal_recip=1.0/et_fcal;
      for(unsigned int ih=0; ih < ms->etCos().size(); ih++)
      {
	float qx=ms->etCos().at(ih);
	float qy=ms->etSin().at(ih);
	
	float qx_fcal=s_fcal->etCos().at(ih);
	float qy_fcal=s_fcal->etSin().at(ih);
	
	ms->etCos()[ih]=(qx*qx_fcal+qy*qy_fcal)*et_fcal_recip;
	ms->etSin()[ih]=(qy*qx_fcal-qx*qy_fcal)*et_fcal_recip;
      } //end loop on harmonics
    } //end scheme==2
  } // end modulation > 1 case
  return StatusCode::SUCCESS;
}


StatusCode HIEventShapeJetIteration::remodulate(xAOD::HIEventShapeContainer* output_shape, const xAOD::HIEventShape* ms, const std::set<unsigned int>& used_indices) const
{
  std::vector<float> mod_factors(HI::TowerBins::numEtaBins(),0);
  std::vector<float> mod_counts(HI::TowerBins::numEtaBins(),0);
  
  for(std::set<unsigned int>::const_iterator sItr=used_indices.begin(); sItr!=used_indices.end(); sItr++)
  {
    unsigned int phi_bin=(*sItr) % HI::TowerBins::numEtaBins();
    unsigned int eta_bin=(*sItr) / HI::TowerBins::numPhiBins();
    mod_counts[eta_bin]++;
    mod_factors[eta_bin]+=m_modulator_tool->getModulation(HI::TowerBins::getBinCenterPhi(phi_bin), ms);
  }
  double nphibins=HI::TowerBins::numPhiBins();
  //now loop on shape and correct;
  for(unsigned int i=0; i<output_shape->size(); i++)
  {
    xAOD::HIEventShape* s=output_shape->at(i);
    float eta0=0.5*(s->etaMin()+s->etaMax());
    unsigned int eb=HI::TowerBins::findBinEta(eta0);
    double cf=(nphibins-mod_counts[eb])/(nphibins-mod_factors[eb]);
    //check on value of cf;
    if(cf < 0) cf =1;
    if(cf > 2.) cf=2;
     
    s->setEt(s->et()*cf);
    s->setRho(s->rho()*cf);
  }
  return StatusCode::SUCCESS;
}

StatusCode HIEventShapeJetIteration::getShapes(const xAOD::HIEventShapeContainer*& input_shape, xAOD::HIEventShapeContainer*& output_shape, bool record) const
{
  ATH_CHECK(evtStore()->retrieve(input_shape,m_input_event_shape_key));
  auto shape_copy=xAOD::shallowCopyContainer(*input_shape);
  output_shape=shape_copy.first;

  if(record)
  {
    ATH_CHECK(evtStore()->record(output_shape,m_output_event_shape_key));
    ATH_CHECK(evtStore()->record(shape_copy.second,m_output_event_shape_key + "Aux."));
  }

  const HIEventShapeIndex* es_index=HIEventShapeMap::getIndex(m_input_event_shape_key);
  if(es_index==nullptr)
  {
    if(m_isInit)
    {
      ATH_MSG_ERROR("No HIEventShapeIndex w/ name " << m_input_event_shape_key);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("No HIEventShapeIndex w/ name " << m_input_event_shape_key << " adding it to the map");
    HIEventShapeIndex* h=new HIEventShapeIndex();
    h->setBinning(input_shape);
    es_index=HIEventShapeMap::insert(m_input_event_shape_key,*h);
    m_isInit=true;
  }
  if(!m_isInit)
  {
    HIEventShapeMap::getMap()->insert(m_output_event_shape_key,*es_index);
    m_isInit=true;
  }
  return StatusCode::SUCCESS;
}
