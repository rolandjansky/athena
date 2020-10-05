/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventShapeJetIteration.h"
#include "xAODHIEvent/HIEventShape.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/HIEventShapeSummaryUtils.h"
#include "xAODCore/ShallowCopy.h"
#include "HIEventUtils/HIEventShapeMapTool.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

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


HIEventShapeJetIteration::HIEventShapeJetIteration(std::string name) : AsgTool(name)
{
}

StatusCode HIEventShapeJetIteration::initialize()
{
   ATH_MSG_VERBOSE("HIEventShapeJetIteration initialize");
   ATH_CHECK( m_inputEventShapeKey.initialize( !m_inputEventShapeKey.key().empty()) );
   ATH_CHECK( m_outputEventShapeKey.initialize( !m_outputEventShapeKey.key().empty()) );
   ATH_CHECK( m_caloJetSeedKey.initialize( SG::AllowEmpty ) );
   ATH_CHECK( m_trackJetSeedKey.initialize( SG::AllowEmpty ) );
   ATH_CHECK( m_modulationKey.initialize( m_modulationScheme ));

   return StatusCode::SUCCESS;
}

int HIEventShapeJetIteration::execute() const
{
  ///
  ATH_MSG_INFO( "Starting HIEventShapeJetIteration execution" );
  const xAOD::HIEventShapeContainer* input_shape=nullptr;
  xAOD::HIEventShapeContainer* output_shape=nullptr;
  getShapes(input_shape,output_shape,true).ignore();

  const HIEventShapeIndex* es_index = m_eventShapeMapTool->getIndexFromShape(input_shape);
  //New implementation after moving away from mutable
  ATH_MSG_INFO("HIEventShapeJetIteration: found index for  " << m_inputEventShapeKey.key());
  if(es_index==nullptr)
  {
    ATH_MSG_FATAL("No HIEventShapeIndex w/ name " << m_inputEventShapeKey.key() << ". Shape not TOWER nor COMPACT");
  }

  const HIEventShapeIndex* other_index = m_eventShapeMapTool->getIndexFromShape(output_shape);
  if(!other_index) {
   ATH_MSG_FATAL("No HIEventShapeIndex w/ name " << m_outputEventShapeKey.key() << ". Shape not TOWER nor COMPACT");
  }
  //End of new implementation

  const xAOD::JetContainer* theCaloJets=0;
  const xAOD::JetContainer* theTrackJets=0;

  if(!m_caloJetSeedKey.empty()){
    SG::ReadHandle<xAOD::JetContainer>  readHandleCaloJets  ( m_caloJetSeedKey );
    theCaloJets = readHandleCaloJets.cptr();
  }
  if(!m_trackJetSeedKey.empty()){
    SG::ReadHandle<xAOD::JetContainer>  readHandleTrackJets ( m_trackJetSeedKey );
    theTrackJets = readHandleTrackJets.cptr();
  }

  if(theTrackJets && m_excludeConstituents)
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

  ATH_MSG_INFO( "Checking Modulation Scheme" );

  //compute ES for modulation
  if(m_modulationScheme)
  {
    SG::WriteHandle<xAOD::HIEventShapeContainer> write_handle_modShape ( m_modulationKey );
    ATH_MSG_DEBUG( "Key HIEventShapeJetIteration: " << m_modulationKey.key() );

    auto modShape = std::make_unique<xAOD::HIEventShapeContainer> ();
    auto modShapeAux = std::make_unique<xAOD::HIEventShapeAuxContainer> ();
    modShape->setStore( modShapeAux.get() );

    xAOD::HIEventShape* ms=new xAOD::HIEventShape();
    modShape->push_back(ms);
    ATH_CHECK( fillModulatorShape(ms, output_shape, used_eta_bins, m_modulationScheme), 1 );
    if(m_doRemodulation) ATH_CHECK( remodulate(output_shape, ms, used_indices), 1 );

    if(write_handle_modShape.record ( std::move(modShape), std::move(modShapeAux)).isFailure() ){
      ATH_MSG_ERROR("Unable to write newHIEventShapeContainer for modulated shape with key: " << m_modulationKey.key());
      return 0;
    }
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
    if(m_excludeConstituents) assoc_clusters=theJet->getConstituents().asIParticleVector();
    else
    {
      if(! (theJet->getAssociatedObjects<xAOD::IParticle>(m_associationKey, assoc_clusters)) )
      {
      	//this should only happen if SOME of the jets in the collection are missing the association
      	//technically possible, but should NOT happen
      	ATH_MSG_ERROR("Individual jet missing association " << m_associationKey);
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
      if( jet4mom.DeltaR( (*pItr)->p4() ) >  m_excludeDR ) continue;
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
    ATH_MSG_INFO("Problem, null pointer");

    es_index=m_eventShapeMapTool->getIndexFromShape(output_shape);
    if(es_index==nullptr)
    {
      ATH_MSG_FATAL("Can't find correspondent index for map " << m_inputEventShapeKey.key() << " in the HIEventShapeMapTool");
    }
  }

  for(auto pItr = assoc_clusters.begin(); pItr!=assoc_clusters.end(); pItr++)
  {
     m_subtractorTool->updateUsingCluster(output_shape,es_index,(*pItr));
  }

  if(!m_subtractorTool->usesCells())
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

    SG::ReadHandle<xAOD::HIEventShapeContainer>  read_handle_cont ( "CaloSums" );
    if (!read_handle_cont.isValid()) {
       ATH_MSG_FATAL( "Could not find HI event shape! CaloSums" );
       return(StatusCode::FAILURE);
    }
    summary_container = read_handle_cont.cptr();

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
    unsigned int phi_bin=(*sItr) % HI::TowerBins::numPhiBins();
    unsigned int eta_bin=(*sItr) / HI::TowerBins::numPhiBins();
    mod_counts[eta_bin]++;
    mod_factors[eta_bin]+=(m_modulatorTool->getModulation(HI::TowerBins::getBinCenterPhi(phi_bin),ms)-1.);
  }
  double nphibins=HI::TowerBins::numPhiBins();
  //now loop on shape and correct;
  for(unsigned int i=0; i<output_shape->size(); i++)
  {
    xAOD::HIEventShape* s=output_shape->at(i);
    float eta0=0.5*(s->etaMin()+s->etaMax());
    unsigned int eb=HI::TowerBins::findBinEta(eta0);
    double neff=nphibins-mod_counts[eb];
    double cf=neff/(neff-mod_factors[eb]);
    //check on value of cf;
    if(cf < 0.) cf =1;
    if(cf > 2.) cf=2;

    s->setEt(s->et()*cf);
    s->setRho(s->rho()*cf);
  }

  return StatusCode::SUCCESS;
}

StatusCode HIEventShapeJetIteration::getShapes(const xAOD::HIEventShapeContainer*& input_shape, xAOD::HIEventShapeContainer*& output_shape, bool record) const
{
  SG::ReadHandle<xAOD::HIEventShapeContainer>  readHandleEvtShape ( m_inputEventShapeKey );
  SG::WriteHandle<xAOD::HIEventShapeContainer> writeHandleEvtShape ( m_outputEventShapeKey );

  input_shape = readHandleEvtShape.ptr();

  if(m_shallowCopy)
  {
    auto shape_copy=xAOD::shallowCopyContainer(*input_shape);
    auto unique_first_copy = xAOD::prepareElementForShallowCopy(shape_copy.first);
    auto unique_second_copy = xAOD::prepareElementForShallowCopy(shape_copy.second);
    output_shape=shape_copy.first;
    if(record)
    {
      ATH_MSG_DEBUG( "Write Handle current key is : " <<  m_outputEventShapeKey.key() );
      if(writeHandleEvtShape.record ( std::move(unique_first_copy), std::move(unique_second_copy)).isFailure() ){
        ATH_MSG_ERROR("Unable to write Shallow Copy containers for event shape with key: " << m_outputEventShapeKey.key());
        return(StatusCode::FAILURE);
      }
    }

  }
  else
  {
    auto output_shapex=std::make_unique<xAOD::HIEventShapeContainer> ();
    auto output_Aux=std::make_unique<xAOD::HIEventShapeAuxContainer> ();
    output_shapex->setStore( output_Aux.get() );
    output_shapex->reserve( input_shape->size() );

    for(auto s=input_shape->begin(); s!=input_shape->end(); s++)
    {
      xAOD::HIEventShape* s_copy=new xAOD::HIEventShape();
      output_shapex->push_back(s_copy);
      *s_copy=**s;
    }
    if(record)
    {
        if(writeHandleEvtShape.record ( std::move(output_shapex), std::move(output_Aux)).isFailure() ){
        ATH_MSG_ERROR("Unable to write Output Shape containers for event shape with key: " << m_outputEventShapeKey.key());
        return(StatusCode::FAILURE);
      }
      output_shape = writeHandleEvtShape.ptr();
    }

  }

  return StatusCode::SUCCESS;
}
