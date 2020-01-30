/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackCaloClusterRecTools/TrackCaloClusterTool.h"
#include "TrackCaloClusterRecTools/IParticleToCaloExtensionMap.h"
#include "TrackCaloClusterRecTools/TrackCaloClusterInfo.h"

#include "CxxUtils/sincos.h"
#include <cmath>


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
}


//*******************************************************************************

TrackCaloClusterBaseTool::TrackCaloClusterBaseTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p),
  m_loosetrackvertexassoTool("LooseTrackVertexAssociationTool"),
  m_useEnergy(false),
  m_caloEntryMapName("ParticleToCaloExtensionMap")
{

  declareInterface<ITrackCaloClusterTool>(this);
  declareProperty("ParticleCaloEntryMapName", m_caloEntryMapName );
  declareProperty("LooseTrackVertexAssoTool", m_loosetrackvertexassoTool);
  declareProperty("UseEnergy"               , m_useEnergy);
  
  declareProperty("SaveDetectorEta"      , m_saveDetectorEta);  
    
}
TrackCaloClusterBaseTool::~TrackCaloClusterBaseTool() {}


StatusCode TrackCaloClusterBaseTool::initialize() {
  ATH_CHECK(m_loosetrackvertexassoTool.retrieve());
  return StatusCode::SUCCESS;
}







//*******************************************************************************

TCCCombinedTool::TCCCombinedTool(const std::string& t, const std::string& n, const IInterface*  p )
  : TrackCaloClusterBaseTool(t,n,p),
  m_doOriginCorrection(false),
  m_storeCorrectedPosition(false)
{
  declareProperty("DoOriginCorrection"    , m_doOriginCorrection);
  declareProperty("StoreCorrectedPosition", m_storeCorrectedPosition );
}


StatusCode TCCCombinedTool::fillTCC(xAOD::TrackCaloClusterContainer* tccContainer, const TrackCaloClusterInfo & tccInfo ) const {

  if(tccInfo.pv0==nullptr){
    ATH_MSG_ERROR ("No PV0 available ! ");
    return StatusCode::FAILURE;
  }

  // declare Decorator in case we want to save out corrected positions
  static SG::AuxElement::Decorator<int> dec_isCorrected("Corrected");
  static SG::AuxElement::Decorator<float> dec_calEntryEta("CaloEntryPosEtaCorr") ;
  static SG::AuxElement::Decorator<float> dec_calEntryPhi("CaloEntryPosPhiCorr") ;
  static SG::AuxElement::Decorator<float> dec_detEta("DetectorEta") ;

  
  // Loop over the TrackParticleClusterAssociationContainer to retrieve the (track, vector<cluster>) associations
  for ( const xAOD::TrackParticleClusterAssociation * assocClusters : *tccInfo.assocContainer ) {
    ATH_MSG_VERBOSE ("InDetTrackParticlesClusterAssociations index = " << assocClusters->index());

    if (!assocClusters->trackParticleLink().isValid()){ATH_MSG_ERROR ("trackParticleLink is not valid! " ); continue;}
    
    // flollow the link to the track particle
    const xAOD::TrackParticle* trk = *(assocClusters->trackParticleLink());
    // verify the track is compatible with PV0
    if (! m_loosetrackvertexassoTool->isCompatible(*trk, *tccInfo.pv0) ) continue ;


    IParticleToCaloExtensionMap * caloExtensionMap = 0;
    if (m_doOriginCorrection || m_saveDetectorEta) {
      if(evtStore()->retrieve(caloExtensionMap,m_caloEntryMapName).isFailure()){
        ATH_MSG_ERROR( "CaloExtensionMap requested, but unable to retrieve " << m_caloEntryMapName << " will leak the ParticleCaloExtension" );
        return StatusCode::FAILURE;
      }      
    }
    
    
	FourMom_t tcc_4p(0.,0.,0.,0.); // will be the TCC 4-vector
    // follow the link to the calorimeter clusters
    ATH_MSG_VERBOSE ("#(CaloCluster) = " << assocClusters->caloClusterLinks().size());

    // Loop over associated clusters to sum the 4-vectors x weigths
    for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
      const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));

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
	  
	  const Trk::TrackParameters* pars = caloExtensionMap->readCaloEntry(trk);
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
	xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
	tccContainer->push_back(tcc);
	tcc->setParameters(tcc_4p.Pt(),eta,phi,tcc_4p.M(),xAOD::TrackCaloCluster::Taste::Combined,assocClusters->trackParticleLink(),assocClusters->caloClusterLinks());
	
    ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->taste());
	
	if(m_saveDetectorEta) {	  
	  const Trk::TrackParameters* pars = caloExtensionMap->readCaloEntry(trk);
	  double det_eta = pars->position().eta();      
      dec_detEta(*tcc) = det_eta;
	}
  } // for assoc clusters

  
  return StatusCode::SUCCESS;
}







//*******************************************************************************

TCCChargedTool::TCCChargedTool(const std::string& t, const std::string& n, const IInterface*  p )
  : TrackCaloClusterBaseTool(t,n,p)
{
}


    
StatusCode TCCChargedTool::fillTCC(xAOD::TrackCaloClusterContainer* tccContainer, const TrackCaloClusterInfo & tccInfo ) const {
      

  IParticleToCaloExtensionMap * caloExtensionMap = 0;
  if (m_saveDetectorEta) {
    if(evtStore()->retrieve(caloExtensionMap,m_caloEntryMapName).isFailure()){
      ATH_MSG_ERROR( "CaloExtensionMap requested, but unable to retrieve " << m_caloEntryMapName << " will leak the ParticleCaloExtension" );
        return StatusCode::FAILURE;
    }      
  }
  
  // declare Decorator in case we want to save out corrected positions  
  static SG::AuxElement::Decorator<float> dec_detEta("DetectorEta") ;
  
  unsigned int i = 0;
  // Loop over ALL tracks at the source of TCC
  for ( const xAOD::TrackParticle* track : *tccInfo.allTracks ) {
    // considre ONLY tracks NOT matched to a cluster :
    if(tccInfo.trackTotalClusterPt.find(track)==tccInfo.trackTotalClusterPt.end()){
      bool isMatched = m_loosetrackvertexassoTool->isCompatible(*track, *tccInfo.pv0 );
      if (!isMatched) continue;
      
      xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
      tccContainer->push_back(tcc);
      ElementLink< xAOD::TrackParticleContainer > trkLink(*tccInfo.allTracks,i);
      tcc->setParameters(track->pt(),track->eta(),track->phi(),track->m(),xAOD::TrackCaloCluster::Taste::Charged,trkLink,std::vector<ElementLink<xAOD::CaloClusterContainer>>());
      ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->taste());

      if(m_saveDetectorEta) {
        // retrieve the caloExtensionContainer to get the track direction at the calo entrance
        double det_eta = track->eta();
        const Trk::CurvilinearParameters* pars = caloExtensionMap->readCaloEntry(track);
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
  : TrackCaloClusterBaseTool(t,n,p)
  , m_clusterFilterTool("ClusterFilterTool", this)
    
{
  declareProperty("ClusterFilterTool", m_clusterFilterTool);
  declareProperty("ApplyClusterFilter" , m_applyFilter);
}

StatusCode TCCNeutralTool::initialize() {
  ATH_CHECK(TrackCaloClusterBaseTool::initialize());
  if (m_applyFilter)
    ATH_CHECK(m_clusterFilterTool.retrieve());
  return StatusCode::SUCCESS;
}


    
StatusCode TCCNeutralTool::fillTCC(xAOD::TrackCaloClusterContainer* tccContainer, const TrackCaloClusterInfo & tccInfo ) const {

  unsigned int i = 0;
  // declare Decorator in case we want to save out corrected positions  
  static SG::AuxElement::Decorator<float> dec_detEta("DetectorEta") ;

  // Loop over ALL clusters 
  for ( const xAOD::CaloCluster* cluster : *tccInfo.allClusters ) {
    // consider only clusters NOT matched to a track :
    if(tccInfo.clusterToTracksWeightMap.find(cluster)==tccInfo.clusterToTracksWeightMap.end()){
	  if (m_applyFilter and m_clusterFilterTool->rejectCluster(*cluster)) continue;

      // create a neutral TCC
      xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
      tccContainer->push_back(tcc);
	  ElementLink< xAOD::CaloClusterContainer > clusterLink(*tccInfo.allClusters,i);
	  const std::vector< ElementLink<xAOD::CaloClusterContainer> > ClusterLink {clusterLink};
	  tcc->setParameters(cluster->pt(),cluster->eta(),cluster->phi(),cluster->m(),xAOD::TrackCaloCluster::Taste::Neutral,ElementLink<xAOD::TrackParticleContainer>(),ClusterLink);
      ATH_MSG_VERBOSE ("Created TCC with pt " << tcc->pt() << " eta " << tcc->eta() << " phi " << tcc->phi() << " mass " << tcc->m() << " taste " << tcc->taste());
      
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

UFOTool::UFOTool(const std::string& t, const std::string& n, const IInterface*  p )
  : TrackCaloClusterBaseTool(t,n,p)
    
{
  declareProperty("PFOPrefix", m_pfoPrefix="CSSKJetETMiss");
  declareProperty("ClusterECut", m_clusterEcut=0.); // WARNING this property must be the same as the one in TrackCaloClusterInfoUFOAlg
}


StatusCode UFOTool::fillTCC(xAOD::TrackCaloClusterContainer* tccContainer, const TrackCaloClusterInfo & tccInfo ) const {// createCombinedTCCs(xAOD::TrackCaloClusterContainer* tccContainer,

  // These are the PFO containers that we actually want to use to create the Unholy Alliance
  const xAOD::PFOContainer* chPFO = nullptr;
  ATH_CHECK( evtStore()->retrieve(chPFO, m_pfoPrefix+"ChargedParticleFlowObjects") );

  const xAOD::PFOContainer* nPFO = nullptr;
  ATH_CHECK( evtStore()->retrieve(nPFO, m_pfoPrefix+"NeutralParticleFlowObjects") );

  
  // These are the unmodified PFO containers. Currently, we need these to do a cluster matching, since the modified
  // containers change the eta and phi of each PFO
  const xAOD::PFOContainer* chPFO_orig = nullptr;
  ATH_CHECK( evtStore()->retrieve(chPFO_orig, "JetETMissChargedParticleFlowObjects") );

  const xAOD::PFOContainer* nPFO_orig = nullptr;
  ATH_CHECK( evtStore()->retrieve(nPFO_orig, "JetETMissNeutralParticleFlowObjects") );


  
  // Loop over the TrackParticleClusterAssociationContainer to retrieve the (track, vector<cluster>) associations
  for ( const xAOD::TrackParticleClusterAssociation* assocClusters : *tccInfo.assocContainer ) {
    const ElementLink<xAOD::TrackParticleContainer> trackLinks;
    ATH_MSG_VERBOSE ("InDetTrackParticlesClusterAssociations index = " << assocClusters->index());

    if (!assocClusters->trackParticleLink().isValid()){ATH_MSG_ERROR ("trackParticleLink is not valid! " ); continue;}
    const xAOD::TrackParticle* trk = *(assocClusters->trackParticleLink());
      
    // if not matched to PV0, ignore track
    if (! m_loosetrackvertexassoTool->isCompatible(*trk, *tccInfo.pv0) ) continue ;

    std::vector< ElementLink< xAOD::PFOContainer > > pfoLinks;
      
    // Checking if the track is matched to a charged PFO
    // We only use tracks which have not already been associated to a PFO (unless that PFO is in a dense environment)
    bool isTrackMatchedToPFO = false;
    for ( const xAOD::PFO* cpfo : *chPFO ){
      int isInDenseEnvironment = false;
      cpfo->attribute(xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment,isInDenseEnvironment);
      if(isInDenseEnvironment) continue;

      if(cpfo->track(0) == trk) {
        isTrackMatchedToPFO = true;
        break;
      }
    }
    if(isTrackMatchedToPFO) continue;

    FourMom_t tcc_4p(0.,0.,0.,0.);
    // follow the link to the calorimeter clusters
    ATH_MSG_VERBOSE ("#(PFO) = " << assocClusters->caloClusterLinks().size());
    for (size_t c = 0; c < assocClusters->caloClusterLinks().size(); ++c) {
      const xAOD::CaloCluster* cluster = *(assocClusters->caloClusterLinks().at(c));
      //bool isClusterMatched = false;
      int i=-1;
      // Check if this is linked to any pfo
      for ( const xAOD::PFO* cpfo : *nPFO ){
        i++;
        if(cpfo->pt()<=0.) continue;
          
        // Only want to include if this PFO is associated to this cluster
        ElementLink< xAOD::PFOContainer > pfoLink(*nPFO,i);
        // Compares result to 
        if( !(fabs((nPFO_orig->at(i))->eta()  - cluster->rawEta() )<0.01 && fabs((nPFO_orig->at(i))->phi()  - cluster->rawPhi() )<0.01)) continue;
        if(cluster->rawE() < m_clusterEcut) continue;

        pfoLinks.push_back(pfoLink);
        double pfo_pt       = m_useEnergy ? cpfo->e() : cpfo->pt();
        const FourMom_t & totalP = tccInfo.trackTotalClusterPt.at(trk);
        double totalpfo_pt  = m_useEnergy ? totalP.E() : totalP.Pt();
        tcc_4p += cpfo->p4()*(( trk->pt() * pfo_pt / totalpfo_pt) / ((tccInfo.clusterToTracksWeightMap.at(cpfo)).Pt()));
      } // end loop over neutral PFO

      i=-1;
      for ( const xAOD::PFO* cpfo : *chPFO ){
        i++;
        if(cpfo->pt()<=0.) continue;
        // Only want to include if this PFO is associated to this cluster
        if( !(fabs( (chPFO_orig->at(i))->eta()  - cluster->rawEta() )<0.01 && fabs( (chPFO_orig->at(i))->phi()  - cluster->rawPhi() )<0.01)) continue;
            
        // Only want to use charged tracks which haven't been properly matched
        int isInDenseEnvironment = false;
        cpfo->attribute(xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment,isInDenseEnvironment);
        if(!isInDenseEnvironment) continue;

        ElementLink< xAOD::PFOContainer > pfoLink(m_pfoPrefix+"ChargedParticleFlowObjects",i);
        pfoLinks.push_back(pfoLink);
        double pfo_pt       = m_useEnergy ? cpfo->e() : cpfo->pt();
        const FourMom_t & totalP = tccInfo.trackTotalClusterPt.at(trk);
        double totalpfo_pt  = m_useEnergy ? totalP.E() : totalP.Pt();
        tcc_4p += cpfo->p4()*(( trk->pt() * pfo_pt / totalpfo_pt) / ((tccInfo.clusterToTracksWeightMap.at(cpfo)).Pt()));
      } // end loop over charged PFO
    } // for caloClusterLinks
      
    if(tcc_4p.Pt() <=0) continue;
    xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
    tccContainer->push_back(tcc);
    tcc->setParameters(tcc_4p.Pt(), trk->eta(), trk->phi(),tcc_4p.M(),xAOD::TrackCaloCluster::Taste::Combined,assocClusters->trackParticleLink(),pfoLinks);
  } // for assoc clusters
  
  // Create a TCC for all neutral PFO which are not matched to any tracks
  unsigned int i = -1;
  for ( const xAOD::PFO* pfo : *nPFO ) {
    i++;
    if(pfo->pt() <= 0) continue;
    
    if(tccInfo.clusterToTracksWeightMap.find(pfo)==tccInfo.clusterToTracksWeightMap.end()){
      xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
      tccContainer->push_back(tcc);
      ElementLink< xAOD::PFOContainer > pfoLink(m_pfoPrefix+"NeutralParticleFlowObjects",i);
      const std::vector< ElementLink<xAOD::PFOContainer> > PFOLink {pfoLink};
      tcc->setParameters(pfo->pt(),pfo->eta(),pfo->phi(),pfo->m(),xAOD::TrackCaloCluster::Taste::Neutral,ElementLink<xAOD::TrackParticleContainer>(),PFOLink);
    }
  } // for all neutral PFO
    

  // Create a TCC for all charged PFO which are not matched to any tracks (unless they are PU)
  i = -1;
  for ( const xAOD::PFO* pfo : *chPFO ) {
    i++;
    if(pfo->pt() <= 0) continue;
    if(tccInfo.clusterToTracksWeightMap.find(pfo)==tccInfo.clusterToTracksWeightMap.end()){
      const static SG::AuxElement::Accessor<char> PVMatchedAcc("matchedToPV"); // this decoration is set by JetRecTools/Root/ChargedHadronSubtractionTool.cxx !
      if(!PVMatchedAcc(*pfo)) continue;
      xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
      tccContainer->push_back(tcc);
      ElementLink< xAOD::PFOContainer > pfoLink(m_pfoPrefix+"ChargedParticleFlowObjects",i);
      const std::vector< ElementLink<xAOD::PFOContainer> > PFOLink {pfoLink};
      tcc->setParameters(pfo->pt(), pfo->eta(), pfo->phi(), pfo->m(), xAOD::TrackCaloCluster::Taste::Charged, ElementLink<xAOD::TrackParticleContainer>(), PFOLink);
    }
  } // for all charged PFO

  return StatusCode::SUCCESS;
}
