//-*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// Author: Clement Camincher (ccaminch@cern.ch)
///////////////////////////////////////////////////////////////////////

#include "JetRecTools/JetInputElRemovalTool.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"



JetInputElRemovalTool::JetInputElRemovalTool(const std::string& t)
  :AsgTool(t)
   
  ,m_elIDname("DFCommonElectronsLHTight")
  ,m_elPt(25000)
  ,m_useOnlyclInJets(false)
  ,m_clRemovRadius(0.15)
  ,m_clEMFrac(0.8)
{
  declareProperty("ElectronQuality",m_elIDname);
  declareProperty("ElectronMinpT",m_elPt);
  declareProperty("ClusterRemovRadius",m_clRemovRadius);
  declareProperty("ClusterEMFrac",m_clEMFrac);
  declareProperty("UseOnlyclInJets",m_useOnlyclInJets);

  declareProperty("TrkInputContainer",
                  m_trkInputContainer_key="InDetTrackParticles");

  declareProperty("JetINputContainer",
                  m_jetInputContainer_key="AntiKt4EMTopoJets");


  declareProperty("ClusterContainerName",
                  m_clInputContainer_key="CaloCalTopoClusters");

  declareProperty("ElectronContainerName",
                  m_elInputContainer_key="Electrons");

  declareProperty("ClusterNoElName",
                  m_clOutputContainer_key="CaloCalTopoClustersNoEl");

  declareProperty("TrkOutputContainer",
                  m_trkOutputContainer_key="InDetTrackParticlesNoEl");

  
}

JetInputElRemovalTool::~JetInputElRemovalTool(){
  
}


StatusCode JetInputElRemovalTool::initialize(){
  ATH_MSG_INFO("Initializing tool " << name() << "...");
  ATH_MSG_DEBUG("initializing version with data handles");

  ATH_CHECK(m_trkInputContainer_key.initialize());
  ATH_CHECK(m_jetInputContainer_key.initialize());
  ATH_CHECK(m_clInputContainer_key.initialize());
  ATH_CHECK(m_elInputContainer_key.initialize());
  ATH_CHECK(m_clOutputContainer_key.initialize()); 
  ATH_CHECK(m_trkOutputContainer_key.initialize()); 
  
  return StatusCode::SUCCESS;
}


int JetInputElRemovalTool::execute() const{

  using OutContTypeCl = ConstDataVector<xAOD::CaloClusterContainer>;
  OutContTypeCl* filtered_clusters = new OutContTypeCl(SG::VIEW_ELEMENTS);
  
  //Select the electrons with given properties
  std::vector<const xAOD::Electron*> el_vector=selectElectron();
  
  //Select the clusters away from electrons
  
  //Use all the clusters in the event
  if(!m_useOnlyclInJets){
    fillSelectedClusters(el_vector,*filtered_clusters);
  }
  //Use only the cluster from jets in the event
  else{
    fillSelectedClustersInJets(el_vector,*filtered_clusters);
  }
  
  //Record clusters vector
  {
    auto handle = SG::makeHandle(m_clOutputContainer_key);
    if(!handle.record(std::unique_ptr<OutContTypeCl>(filtered_clusters))){
      ATH_MSG_WARNING("Unable to record new clusters vector");
      return 1;
    }
  }
  
  //Store vector only if data vector name not empty 
  if (!m_trkInputContainer_key.key().empty()){

    using OutContTypeTr = ConstDataVector<xAOD::TrackParticleContainer>;
 
    OutContTypeTr* filtered_tracks = new OutContTypeTr(SG::VIEW_ELEMENTS) ;

    fillSelectedTracks(el_vector,*filtered_tracks);
  
    auto handle_out = SG::makeHandle(m_trkOutputContainer_key);
    if(!handle_out.record(std::unique_ptr<OutContTypeTr>(filtered_tracks))){
      ATH_MSG_WARNING("Unable to record new tracks vector");
      return 1;
    }
  }

  return 0;
}

StatusCode JetInputElRemovalTool::finalize(){
  return StatusCode::SUCCESS;
}



std::vector<const xAOD::Electron*> JetInputElRemovalTool::selectElectron()const{

  std::vector<const xAOD::Electron*> selected_electrons_v;  

  auto handle = SG::makeHandle(m_elInputContainer_key);
  if(!handle.isValid()){
    ATH_MSG_WARNING("Unable to retrieve electrons");
    return selected_electrons_v;
  }
    
  auto electrons = handle.cptr();

  selected_electrons_v.clear();
  bool isTight=false;
  
  for (auto electron_itr : *electrons){
    
    isTight=false;
    
    //Select only el with given quality
    if (electron_itr->auxdecor< char >(m_elIDname)==1){
      isTight=true;
    }
    
    if (! isTight) continue ;
    
    
    //Select only el with pt>25GeV
    if (electron_itr->pt()<m_elPt) continue;
    
    selected_electrons_v.push_back(dynamic_cast<const xAOD::Electron*>(electron_itr));
    
    
  }
  
  return selected_electrons_v ;
}






int JetInputElRemovalTool::fillSelectedClusters(std::vector<const xAOD::Electron*>&selected_el,ConstDataVector<xAOD::CaloClusterContainer> & selected_cl)const{
  
  //Initialize variables
  
  int countRemoved_clusters=0;
  double propEM=0;
  
  //Get the Topo clusters of the event

  auto handle = SG::makeHandle(m_clInputContainer_key);
  if(!handle.isValid()){
    ATH_MSG_WARNING("Unable to retrieve clusters");
    return 0;
  }
    
  auto clusterContainer = handle.cptr();
  
  //Loop over all the clusters
  for (const xAOD::CaloCluster* cluster_itr : *clusterContainer){
    
    //Compute the EMP proportion of the cluster
    propEM=0;
    
    double EMB_Enegy=cluster_itr->eSample(CaloSampling::CaloSample::PreSamplerB)+cluster_itr->eSample(CaloSampling::CaloSample::EMB1)+cluster_itr->eSample(CaloSampling::CaloSample::EMB2)+cluster_itr->eSample(CaloSampling::CaloSample::EMB3);
    
    double EMEC_Energy=cluster_itr->eSample(CaloSampling::CaloSample::PreSamplerE)+cluster_itr->eSample(CaloSampling::CaloSample::EME1)+cluster_itr->eSample(CaloSampling::CaloSample::EME2)+cluster_itr->eSample(CaloSampling::CaloSample::EME3);
    
    //Remove clusters w/o energy
    if (cluster_itr->rawE()==0) {
      continue;
    }
    propEM=(EMB_Enegy+EMEC_Energy)/cluster_itr->rawE();
    
    
    
    //Check if close to electron
    bool closetoel=false;
    
    std::vector<const xAOD::Electron*>::iterator it=selected_el.begin();
    std::vector<const xAOD::Electron*>::iterator itE =selected_el.end();
    //For each el in the vector
    for ( ; it != itE ;++it){
      ATH_MSG_DEBUG( "Deleta R electron cluster = "<<(*it)->p4().DeltaR(cluster_itr->p4()) );
      //Check if electron close to cluster
      //if ((*it)->p4().DeltaR(cluster_itr->p4())<m_clRemovRadius){
      if ((*it)->caloCluster()->p4().DeltaR(cluster_itr->p4())<m_clRemovRadius){
	
	//if (TMath::Abs((*it)->caloCluster()->etaBE(2))<1.52 && TMath::Abs((*it)->caloCluster()->etaBE(2))>1.37){
	if (TMath::Abs(cluster_itr->eta())<1.52 && TMath::Abs(cluster_itr->eta())>1.37){
	  closetoel=true;
	}
	
	//Check if the proportion of EM enegy above threshold
	else if(propEM>=m_clEMFrac){
	  closetoel=true;
	}
      }
      
    }
    //If not close enough or not enough EM energy the cluster is kept
    if (!closetoel){
      selected_cl.push_back(dynamic_cast<const xAOD::CaloCluster*> (cluster_itr));
    }
    else{ //else it is removed
      countRemoved_clusters+=1;
    }
  }//End loop over clusters
  
  
  //ANA_CHECK(evtStore()->record( filtered_clusters , "CaloCalTopoClustersNoElec" ));
  
  return  countRemoved_clusters;
}//End of SelectClusters()









int JetInputElRemovalTool::fillSelectedClustersInJets(std::vector<const xAOD::Electron*>&selected_el,ConstDataVector<xAOD::CaloClusterContainer> & selected_cl)const{
  
  int countRemoved_clusters=0;
  double propEM=0;
  
  auto handle = SG::makeHandle(m_jetInputContainer_key);
  if(!handle.isValid()){
    ATH_MSG_WARNING("Unable to retrieve jets");
    return 0;
  }

  auto jetsContainer = handle.cptr();

  // const xAOD::JetContainer* jetsContainer;
  // StatusCode sc=evtStore()->retrieve( jetsContainer, m_jetINputContainer );
  // if (sc.isFailure()){
  //   ATH_MSG_WARNING("Unable to retrieve jets");
  //   return 0;
  // }
  
  for (const xAOD::Jet* jet_itr : *jetsContainer){
    
    
    const xAOD::JetConstituentVector jetcons = jet_itr->getConstituents();
    
    
    if (!jetcons.isValid()) continue ;
    
    xAOD::JetConstituentVector::iterator jetcons_it = jetcons.begin();
    xAOD::JetConstituentVector::iterator jetcons_itE = jetcons.end();
    
    for( ; jetcons_it != jetcons_itE; jetcons_it++){
      
      propEM=0;
      const xAOD::CaloCluster* cluster_itr= dynamic_cast<const xAOD::CaloCluster*> (jetcons_it->rawConstituent());
      
      
      double EMB_Enegy=cluster_itr->eSample(CaloSampling::CaloSample::PreSamplerB)+cluster_itr->eSample(CaloSampling::CaloSample::EMB1)+cluster_itr->eSample(CaloSampling::CaloSample::EMB2)+cluster_itr->eSample(CaloSampling::CaloSample::EMB3);
      
      double EMEC_Energy=cluster_itr->eSample(CaloSampling::CaloSample::PreSamplerE)+cluster_itr->eSample(CaloSampling::CaloSample::EME1)+cluster_itr->eSample(CaloSampling::CaloSample::EME2)+cluster_itr->eSample(CaloSampling::CaloSample::EME3);
      
      
      //Remove Clusters w/o energy
      if (cluster_itr->rawE()==0) {
	continue;
      };
      
      propEM=(EMB_Enegy+EMEC_Energy)/cluster_itr->rawE();
      
      bool closetoel=false;
      std::vector<const xAOD::Electron*>::iterator it=selected_el.begin();
      std::vector<const xAOD::Electron*>::iterator itE =selected_el.end();
      
      for ( ; it != itE ;++it){
	ATH_MSG_DEBUG( "Deleta R electron cluster = "<<(*it)->p4().DeltaR(cluster_itr->p4()) );
	if ((*it)->p4().DeltaR(cluster_itr->p4())<m_clRemovRadius){
	  if(propEM>=m_clEMFrac){
	    closetoel=true;
	  }
	}
	
      }
      if (!closetoel){
	selected_cl.push_back(dynamic_cast<const xAOD::CaloCluster*> (cluster_itr));
      }
      else{
	countRemoved_clusters+=1;
      }
    }//End loop over clusters
    
  }//End loop over jets
  
  //ANA_CHECK(evtStore()->record( filtered_clusters , "CaloCalTopoClustersNoElec" ));
  
  return  countRemoved_clusters;
}


int JetInputElRemovalTool::fillSelectedTracks(std::vector<const xAOD::Electron*>&selected_el,ConstDataVector<xAOD::TrackParticleContainer> & selected_cl)const{
  
  int countRemoved_trk=0;
  
  auto handle = SG::makeHandle(m_trkInputContainer_key);
  if(!handle.isValid()){
    ATH_MSG_WARNING("Unable to retrieve jets");
    return 0;
  }

  auto tkPrtclContainer = handle.cptr();

  // PS 
  // const xAOD::TrackParticleContainer* tkPrtclContainer;
  // StatusCode sc=evtStore()->retrieve( tkPrtclContainer, m_trkInputContainer );
  // if (sc.isFailure()){
  //   ATH_MSG_WARNING("Unable to retrieve jets");
  //   return 0;
  // }
  
  //Loop over all the tracks
  for (const xAOD::TrackParticle* trk_itr : *tkPrtclContainer){
    
    //Check if close to electron
    bool is_eltrk=false;
    
    std::vector<const xAOD::Electron*>::iterator it=selected_el.begin();
    std::vector<const xAOD::Electron*>::iterator itE =selected_el.end();
    //For each el in the vector
    for ( ; it != itE ;++it){
      
      const xAOD::TrackParticle* el_trk=(*it)->trackParticle();
      
      ATH_MSG_DEBUG( "Deleta R electron trkack = "<<(*it)->p4().DeltaR(trk_itr->p4()) );
      //Check if electron close to cluster
      if (el_trk==trk_itr){
	is_eltrk=true;
      }
      
    }
    //If not close enough or not enough EM energy the cluster is kept
    if (!is_eltrk){
      selected_cl.push_back(dynamic_cast<const xAOD::TrackParticle*> (trk_itr));
    }
    else{ //else it is removed
      countRemoved_trk+=1;
    }
  }//End loop over tracks
  
  
  return  countRemoved_trk;
}


  

