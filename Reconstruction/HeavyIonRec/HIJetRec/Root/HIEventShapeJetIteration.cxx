/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIEventShapeJetIteration.h"
#include "xAODHIEvent/HIEventShape.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/HIEventShapeSummaryUtils.h"
#include "xAODJet/JetContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloCluster.h"


namespace
{
  struct SelectByList{

    SelectByList() {};
    std::set<unsigned int>* used_set;
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
  ATH_MSG_INFO("HIEventShapeJetIteration constructor declaring properties");
  declareProperty("InputEventShapeKey",m_input_event_shape_key="HIEventShape");
  declareProperty("OutputEventShapeKey",m_output_event_shape_key="HIEventShape_iter");
  declareProperty("SeedContainerKey",m_seed_key,"Names of seed collections");
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
  const xAOD::HIEventShapeContainer* input_shape=0;
  CHECK(evtStore()->retrieve(input_shape,m_input_event_shape_key));
  
  ATH_MSG_INFO("In execute");
  const xAOD::JetContainer* theJets=0;
  CHECK(evtStore()->retrieve(theJets,m_seed_key));

   //shallow copy shares unaltered auxilliary data with the original container.
  auto shape_copy=xAOD::shallowCopyContainer(*input_shape);
  CHECK(evtStore()->record(shape_copy.first,m_output_event_shape_key));
  CHECK(evtStore()->record(shape_copy.second,m_output_event_shape_key + "Aux."));  
  xAOD::HIEventShapeContainer* output_shape=shape_copy.first;

  const HIEventShapeIndex* es_index=HIEventShapeMap::getIndex(m_input_event_shape_key);
  if(es_index==nullptr)
  {
    ATH_MSG(ERROR) << "No HIEventShapeIndex w/ name " << m_input_event_shape_key << endreq;
    return StatusCode::FAILURE;
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
	ATH_MSG(ERROR) << "Individual jet missing association " << m_association_key << endreq;
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
      m_subtractor_tool->UpdateUsingCluster(output_shape,es_index,cl,used_indices,used_eta_bins);
    }
  }//end jet loop

  //compute ES for modulation
  if(m_modulation_scheme==0) return 0;

  xAOD::HIEventShapeContainer* modShape=new xAOD::HIEventShapeContainer;
  xAOD::HIEventShapeAuxContainer *modShapeAux = new xAOD::HIEventShapeAuxContainer;
  modShape->setStore( modShapeAux );
  CHECK(evtStore()->record(modShape,m_modulation_key));
  CHECK(evtStore()->record(modShapeAux,m_modulation_key+std::string("Aux.")));
  xAOD::HIEventShape* ms=new xAOD::HIEventShape();
  modShape->push_back(ms);

  const xAOD::HIEventShape* s_fcal=0;
  if(m_modulation_scheme > 1)
  {
    const xAOD::HIEventShapeContainer* summary_container=0;
    CHECK(evtStore()->retrieve(summary_container,"CaloSums"));

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
  }
  if(m_modulation_scheme==3) (*ms)=(*s_fcal);
  else
  {
    SelectByList selector;
    selector.used_set=&used_eta_bins;
    HI::fillSummary(output_shape,ms,selector);
    //correct ms for FCal phase.
    if(m_modulation_scheme==2)
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
  } //end else


  if(m_do_remodulation)
  {
    if(m_modulator_tool->setEventShapeForModulation(ms).isFailure())
    {
      ATH_MSG_ERROR("Cannot set shape for modulation using key");
      return 1;
    }
    //now correct averages in each slice for effect of modulation
    std::vector<float> mod_factors(HI::TowerBins::numEtaBins(),0);
    for(std::set<unsigned int>::const_iterator sItr=used_indices.begin(); sItr!=used_indices.end(); sItr++)
    {
      unsigned int phi_bin=(*sItr) % HI::TowerBins::numEtaBins();
      unsigned int eta_bin=(*sItr) / HI::TowerBins::numPhiBins();
      mod_factors[eta_bin] += m_modulator_tool->getModulation(HI::TowerBins::getBinCenterPhi(phi_bin));
    }

    float deta_dphi=HI::TowerBins::getBinArea();
    //now loop on shape and correct;
    for(unsigned int i=0; i<output_shape->size(); i++)
    {
      xAOD::HIEventShape* s=output_shape->at(i);
      float eta0=0.5*(s->etaMin()+s->etaMax());
      unsigned int eb=HI::TowerBins::findBinEta(eta0);
      float area=s->area();
      float cf=area/(area-deta_dphi*mod_factors[eb]);
      //check on value of cf;
      s->setEt(s->et()*cf);
      s->setRho(s->rho()*cf);
    }
  }
  return 0;
}

