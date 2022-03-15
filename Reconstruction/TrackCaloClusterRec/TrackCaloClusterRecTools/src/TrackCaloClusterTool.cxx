/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "StoreGate/ReadDecorHandle.h"

#include "AthContainers/AuxElement.h"

#include "TrackCaloClusterRecTools/TrackCaloClusterTool.h"
#include "TrackCaloClusterRecTools/TrackCaloClusterInfo.h"


#include "CxxUtils/sincos.h"

#include "TrackCaloClusterRecTools/TCCHelpers.h"


namespace {
  // helper functions needed only in this file are defined in this anonymous namespace
  
  typedef xAOD::IParticle::FourMom_t  FourMom_t; // this is actually 

  // update the given eta and phi coordinates by shifting the origin to the position of vertex
  void computeVertexCorr(double& eta, double& phi, const Amg::Vector3D& vertex, double radius) {
  
    if (radius<1.) return;
    
    if (std::fabs(eta)>10. || std::fabs(phi)>10.) return;
  
    CxxUtils::sincos sc (phi);
    double iradius = 1 / radius;
    eta += (-vertex[2]/std::cosh(eta) + sc.apply (vertex[1], vertex[0])*std::tanh(eta)) * iradius;
    phi += sc.apply (vertex[0], -vertex[1]) * iradius;
  }



  template<typename T>
  void setParameters(xAOD::FlowElement* fe, float pt, float eta, float phi, float m,
		     xAOD::FlowElement::SignalType stype,
		     const ElementLink<xAOD::IParticleContainer> &trackLink,
		     const std::vector<ElementLink<T> >& neutralLinks){
    std::vector<ElementLink<xAOD::IParticleContainer> > ipLinks(neutralLinks.size());
    for(size_t i=0;i<neutralLinks.size();i++) ipLinks[i] = neutralLinks[i];
    setParameters(fe,pt,eta,phi, m, stype,trackLink, ipLinks);
  }

  template<>
  void setParameters<xAOD::IParticleContainer>(xAOD::FlowElement* fe, float pt, float eta, float phi, float m,
		     xAOD::FlowElement::SignalType stype,
		     const ElementLink<xAOD::IParticleContainer> &trackLink,
		     const std::vector<ElementLink<xAOD::IParticleContainer> >& neutralLinks){
    fe->setP4(pt,eta,phi,m);
    fe->setSignalType(stype);
    fe->setChargedObjectLinks( {trackLink} );
    fe->setOtherObjectLinks( neutralLinks);
  }
  

  /// help work around limitation of ReadDecorHandle
  template<typename DTYPE, typename CTYPE>
  SG::AuxElement::ConstAccessor<DTYPE> asConstAccessor(const SG::ReadDecorHandleKey<CTYPE> & wh){
    static const std::string ts = typeid(DTYPE).name();
    if(wh.empty() ) return SG::AuxElement::ConstAccessor<DTYPE>( typeid(DTYPE).name() );
    const auto split = wh.key().rfind ('.');
    if (split == std::string::npos)
      throw std::runtime_error ("decor key does not contain a .: " + wh.key());
    return SG::AuxElement::ConstAccessor<DTYPE>( wh.key().substr (split + 1) );
  }
  
  

}


//*******************************************************************************

TrackCaloClusterBaseTool::TrackCaloClusterBaseTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p)
{
  declareInterface<ITrackCaloClusterTool>(this);
}
TrackCaloClusterBaseTool::~TrackCaloClusterBaseTool() {}


StatusCode TrackCaloClusterBaseTool::initialize() {
  if(!m_trackVertexAssoTool.empty()) ATH_CHECK(m_trackVertexAssoTool.retrieve());

  ATH_CHECK(m_assoClustersKey.initialize());

  ATH_CHECK(m_caloEntryParsDecor.initialize(!m_caloEntryParsDecor.empty()) );
  return StatusCode::SUCCESS;
}







//*******************************************************************************

TCCCombinedTool::TCCCombinedTool(const std::string& t, const std::string& n, const IInterface*  p )
  : TrackCaloClusterBaseTool(t,n,p) { }


StatusCode TCCCombinedTool::fillTCC(xAOD::FlowElementContainer* tccContainer, const TrackCaloClusterInfo & tccInfo ) const {

  if(tccInfo.pv0==nullptr){
    ATH_MSG_ERROR ("No PV0 available ! ");
    return StatusCode::FAILURE;
  }

  // declare Decorator in case we want to save out corrected positions
  static SG::AuxElement::Decorator<int> dec_isCorrected("Corrected");
  static SG::AuxElement::Decorator<float> dec_calEntryEta("CaloEntryPosEtaCorr") ;
  static SG::AuxElement::Decorator<float> dec_calEntryPhi("CaloEntryPosPhiCorr") ;
  static SG::AuxElement::Decorator<float> dec_detEta("DetectorEta") ;

  // it is not possible to prepare a blank ReadDecorHandle (which we need if !m_caloEntryParsDecor.empty()), so instead or re-instantiating a ReadDecorHandle on each
  // track in the loop below, we just instantiate a ConstAccessor  
  SG::ReadDecorHandle<xAOD::TrackParticleContainer, std::vector<ElementLink<xAOD::CaloClusterContainer>> > clusterLinksH(m_assoClustersKey);
  auto caloEntryParams = asConstAccessor<const Trk::TrackParameters*>(m_caloEntryParsDecor);
  
  
  for ( const xAOD::TrackParticle* trk : *tccInfo.allTracks ) {

    if (! m_trackVertexAssoTool->isCompatible(*trk, *tccInfo.pv0) ) continue ;
   
    const auto & clusterLinks = clusterLinksH(*trk);
    if( clusterLinks.empty() ) continue;
    
    FourMom_t tcc_4p(0.,0.,0.,0.); // will be the TCC 4-vector

    // Loop over associated clusters to sum the 4-vectors x weigths
    for( const ElementLink<xAOD::CaloClusterContainer> & clLink : clusterLinks){
      const xAOD::CaloCluster* cluster = *clLink;

      double cluster_pt       = m_useEnergy ? cluster->e() : cluster->pt();
      double totalcluster_pt  = m_useEnergy ? tccInfo.trackTotalClusterPt.at(trk).E() : tccInfo.trackTotalClusterPt.at(trk).Pt();
      
      tcc_4p += cluster->p4()*(( trk->pt() * cluster_pt / totalcluster_pt) / ((tccInfo.clusterToTracksWeightMap.at(cluster)).Pt()));
      
      ATH_MSG_VERBOSE ("cluster->pt() " << cluster_pt << " cluster->eta() " << cluster->eta() << " cluster->phi() " 
                       << cluster->phi() << " track pt " << trk->pt() << " (tccInfo.clusterToTracksWeightMap.at(cluster)).Pt() " << (tccInfo.clusterToTracksWeightMap.at(cluster)).Pt());
    } // for caloClusterLinks
    

    // get angular position from tracks
    double eta = trk->eta();
    double phi = trk->phi();
    
    if (m_doOriginCorrection) {
      // retrieve the caloExtensionContainer to get the track direction at the calo entrance
      
      const Trk::TrackParameters* pars = caloEntryParams(*trk);
      eta = pars->position().eta();
      phi = pars->position().phi();
	  
      computeVertexCorr(eta, phi, tccInfo.pv0->position(), pars->position().perp());
      if (m_storeCorrectedPosition) {
        dec_isCorrected(*trk) = 1;
	dec_calEntryEta(*trk) = eta;
	dec_calEntryPhi(*trk) = phi;
      }
    }
	
    // Build the final TCC
    xAOD::FlowElement* tcc = new xAOD::FlowElement;
    tccContainer->push_back(tcc);
    setParameters(tcc, tcc_4p.Pt(),eta,phi,tcc_4p.M(),xAOD::FlowElement::SignalType::Combined,
		  ElementLink<xAOD::TrackParticleContainer>(*tccInfo.allTracks, trk->index() ),clusterLinks);
    
    ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " signalType= " << tcc->signalType());
    
    if(m_saveDetectorEta) {	  
      const Trk::TrackParameters* pars = caloEntryParams(*trk);
      double det_eta = pars->position().eta();      
      dec_detEta(*tcc) = det_eta;
    }
  } // for assoc clusters

  
  return StatusCode::SUCCESS;
}







//*******************************************************************************

TCCChargedTool::TCCChargedTool(const std::string& t, const std::string& n, const IInterface*  p )
  : TrackCaloClusterBaseTool(t,n,p) { }


    
StatusCode TCCChargedTool::fillTCC(xAOD::FlowElementContainer* tccContainer, const TrackCaloClusterInfo & tccInfo ) const {
      

  // it is not possible to prepare a blank ReadDecorHandle (which we need if !m_caloEntryParsDecor.empty()), so instead or re-instantiating a ReadDecorHandle on each
  // track in the loop below, we just instantiate a ConstAccessor  
  auto caloEntryParams = asConstAccessor<const Trk::TrackParameters*>(m_caloEntryParsDecor);
  
  SG::ReadDecorHandle<xAOD::TrackParticleContainer, std::vector<ElementLink<xAOD::CaloClusterContainer>> > clusterLinksH(m_assoClustersKey);
  
  // declare Decorator in case we want to save out corrected positions  
  static SG::AuxElement::Decorator<float> dec_detEta("DetectorEta") ;
  
  unsigned int i = 0;
  // Loop over ALL tracks at the source of TCC
  for ( const xAOD::TrackParticle* track : *tccInfo.allTracks ) {
    if( ! clusterLinksH(*track).empty() ) continue; // because if not empty, it is matched to a cluster
    // considre ONLY tracks NOT matched to a cluster :
    if(tccInfo.trackTotalClusterPt.find(track)==tccInfo.trackTotalClusterPt.end()){
      bool isMatched = m_trackVertexAssoTool->isCompatible(*track, *tccInfo.pv0 );
      if (!isMatched) continue;
      
      xAOD::FlowElement* tcc = new xAOD::FlowElement;
      tccContainer->push_back(tcc);
      ElementLink< xAOD::TrackParticleContainer > trkLink(*tccInfo.allTracks,i);
      setParameters(tcc, track->pt(),track->eta(),track->phi(),track->m(),xAOD::FlowElement::SignalType::Charged,trkLink,std::vector<ElementLink<xAOD::CaloClusterContainer>>());
      ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->signalType());

      if(m_saveDetectorEta) {
        // retrieve the caloExtensionContainer to get the track direction at the calo entrance
        double det_eta = track->eta();
	const Trk::TrackParameters* pars = caloEntryParams(*track);
        if(pars) det_eta = pars->position().eta();
        dec_detEta(*tcc) = det_eta;
      }
    }
    i++;
  }
  return StatusCode::SUCCESS;
}
    




//*******************************************************************************

TCCNeutralTool::TCCNeutralTool(const std::string& t, const std::string& n, const IInterface*  p )
  : TrackCaloClusterBaseTool(t,n,p) {}

StatusCode TCCNeutralTool::initialize() {
  ATH_CHECK(TrackCaloClusterBaseTool::initialize());
  if (!m_clusterFilterTool.empty()){
    ATH_CHECK(m_clusterFilterTool.retrieve());
    m_applyFilter=true;
  } else m_applyFilter=false;
  return StatusCode::SUCCESS;
}


    
StatusCode TCCNeutralTool::fillTCC(xAOD::FlowElementContainer* tccContainer, const TrackCaloClusterInfo & tccInfo ) const {
  
  unsigned int i = 0;
  // declare Decorator in case we want to save out corrected positions  
  static SG::AuxElement::Decorator<float> dec_detEta("DetectorEta") ;

  // Loop over ALL clusters 
  for ( const xAOD::CaloCluster* cluster : *tccInfo.allClusters ) {
    // consider only clusters NOT matched to a track :
    if(tccInfo.clusterToTracksWeightMap.find(cluster)==tccInfo.clusterToTracksWeightMap.end()){
      if (m_applyFilter and m_clusterFilterTool->rejectCluster(*cluster)) continue;
      
      // create a neutral TCC
      xAOD::FlowElement* tcc = new xAOD::FlowElement;
      tccContainer->push_back(tcc);
      ElementLink< xAOD::CaloClusterContainer > clusterLink(*tccInfo.allClusters,i);
      const std::vector< ElementLink<xAOD::CaloClusterContainer> > ClusterLink {clusterLink};
      setParameters(tcc, cluster->pt(),cluster->eta(),cluster->phi(),cluster->m(),xAOD::FlowElement::SignalType::Neutral,ElementLink<xAOD::TrackParticleContainer>(),ClusterLink);
      ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->signalType());
      
      static SG::AuxElement::Accessor< float > acc_det_eta ( "DetectorEta" );        
      if(m_saveDetectorEta && acc_det_eta.isAvailable(*cluster)) {
        dec_detEta(*tcc) = dec_detEta(*cluster);
      }
    }
    i++;
  } // for all clusters
  return StatusCode::SUCCESS;
}










//*******************************************************************************



namespace TCCHelpers{
  ///////////////////////////////////////////////////////
  /// Implement a concrete CombinedUFOLoop dedicated to building UFO
  ///  see TCCHelpers.h in TrackCaloClusterRecTools/
  struct UFOBuilder : public CombinedUFOLoop {

    const xAOD::FlowElementContainer * m_pfoContainer;
    const TrackCaloClusterInfo* m_tccInfo;
    xAOD::FlowElementContainer * m_tccContainer;
    
    std::vector<ElementLink< xAOD::FlowElementContainer > > m_pfoLinks;
    FourMom_t m_tcc_4p = {0.,0.,0.,0.};
    
    virtual void processPFO(const xAOD::TrackParticle* trk, const xAOD::FlowElement* pfo) {
      /// accumulate the total P4 and the pfos linked to trk
      
      ElementLink< xAOD::FlowElementContainer > pfoLink(*m_pfoContainer,pfo->index());
      m_pfoLinks.push_back(pfoLink);
      double pfo_pt       = m_useEnergy ? pfo->e() : pfo->pt();
      const FourMom_t & totalP = m_tccInfo->trackTotalClusterPt.at(trk);
      double totalpfo_pt  = m_useEnergy ? totalP.E() : totalP.Pt();
      m_tcc_4p += pfo->p4()*(( trk->pt() * pfo_pt / totalpfo_pt) / ((m_tccInfo->clusterToTracksWeightMap.at(pfo)).Pt()));
    }
    
    virtual void processTrk(const xAOD::TrackParticle* trk ) {
      // build the actual combined UFO
      if(m_tcc_4p.Pt() <=0) return;
      
      xAOD::FlowElement* tcc = new xAOD::FlowElement;
      m_tccContainer->push_back(tcc);
      setParameters(tcc, m_tcc_4p.Pt(), trk->eta(), trk->phi(),m_tcc_4p.M(),xAOD::FlowElement::SignalType::Combined,
		    ElementLink<xAOD::TrackParticleContainer>(*m_tccInfo->allTracks,trk->index()),m_pfoLinks);

      // reset accumulators for next track :
      m_pfoLinks.clear();
      m_tcc_4p = {0.,0.,0.,0.};
    }
  };
}



UFOTool::UFOTool(const std::string& t, const std::string& n, const IInterface*  p )
  : TrackCaloClusterBaseTool(t,n,p) {}

StatusCode UFOTool::initialize(){
    //override parent class because of additional requirements on the PFOHandles etc
    if(!m_trackVertexAssoTool.empty()) ATH_CHECK(m_trackVertexAssoTool.retrieve());

    ATH_CHECK(m_assoClustersKey.initialize());
    
    ATH_CHECK(m_caloEntryParsDecor.initialize(!m_caloEntryParsDecor.empty()) );
    ATH_CHECK(m_inputPFOHandle.initialize(!m_inputPFOHandle.empty()));
    ATH_CHECK(m_orig_pfo.initialize(!m_orig_pfo.empty()));
    return StatusCode::SUCCESS;
}
StatusCode UFOTool::fillTCC(xAOD::FlowElementContainer* tccContainer, const TrackCaloClusterInfo & tccInfo ) const {

  SG::ReadHandle<xAOD::FlowElementContainer> pfos(m_inputPFOHandle);

  SG::ReadDecorHandle<xAOD::FlowElementContainer, ElementLink<xAOD::FlowElementContainer> > orig_pfo(m_orig_pfo);
  SG::ReadDecorHandle<xAOD::TrackParticleContainer, std::vector<ElementLink<xAOD::CaloClusterContainer>> > clusterLinksH(m_assoClustersKey);

  // We use a dedicated helper to build the combined UFO. Initialize it :  
  TCCHelpers::UFOBuilder ufoB;
  ufoB.m_orig_pfoK = m_orig_pfo.key();
  ufoB.m_clustersLinkK = m_assoClustersKey.key();
  ufoB.m_trackVertexAssoTool = m_trackVertexAssoTool.get();
  ufoB.m_clusterEcut = m_clusterEcut;
  ufoB.m_useEnergy = m_useEnergy;

  ufoB.m_pfoContainer = pfos.ptr();
  ufoB.m_tccInfo = &tccInfo;
  ufoB.m_tccContainer = tccContainer;
  // create a combined UFO for each track matched to some PFO 
  ufoB.combinedUFOLoop(&tccInfo, pfos.cptr());
  
  
  // Create a UFO for all neutral and charged PFO which are not matched to any tracks
  unsigned int i = -1;
  for ( const xAOD::FlowElement* pfo : *pfos ) {
    i++;
    if(pfo->pt() <= 0) continue;
    if(tccInfo.clusterToTracksWeightMap.find(pfo)!=tccInfo.clusterToTracksWeightMap.end()) continue; // combined

    if(pfo->isCharged()) {
      // this decoration is set by JetRecTools/Root/ChargedHadronSubtractionTool.cxx !
      const static SG::AuxElement::Accessor<char> PVMatchedAcc("matchedToPV"); 
      if(!PVMatchedAcc(*pfo)) continue;
    }
    
    ElementLink< xAOD::FlowElementContainer > pfoLink(*pfos,i);
    const std::vector< ElementLink<xAOD::FlowElementContainer> > PFOLink {pfoLink};    
    xAOD::FlowElement* tcc = new xAOD::FlowElement;
    tccContainer->push_back(tcc);

    if(pfo->isCharged()) {
	//retrieve the track from the charged PFO
	const xAOD::IParticle* pfo_chargedobj=pfo->chargedObjects().at(0);
	const xAOD::TrackParticle* pfo_track=dynamic_cast<const xAOD::TrackParticle*>(pfo_chargedobj);
	
	setParameters(tcc, pfo->pt(), pfo->eta(), pfo->phi(), pfo->m(), xAOD::FlowElement::SignalType::Charged, ElementLink<xAOD::TrackParticleContainer>(*tccInfo.allTracks, pfo_track->index()), PFOLink);
    }else{
      setParameters(tcc, pfo->pt(),pfo->eta(),pfo->phi(),pfo->m(),xAOD::FlowElement::SignalType::Neutral,ElementLink<xAOD::TrackParticleContainer>(),PFOLink);      
    }
  } //  PFO
    
  return StatusCode::SUCCESS;
}
