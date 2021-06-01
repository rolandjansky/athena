// -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **   Description: Hypothesis algorithms to be run at Level 2 after
 **                tracking algorithms: creates TrigElectrons
 **
 **   Author: R.Goncalo <r.goncalo@rhul.ac.uk> Thu Jan 20 22:02:43 BST 2005
 **            P.Urquijo <Phillip.Urquijo@cern.ch>
 **
 **   Created:      Sat Mar  1 19:55:56 GMT 2005
 **   Modified:     RG 18 Mar 06 - fix to always generate TrigElectrons
 **                              - use key2keyStore for collection names
 **                 N.Berger Dec.06 - migrate to new steering (RG)
 **************************************************************************/

#include "TrkCaloExtension/CaloExtensionHelpers.h" 
#include "TrigEgammaFastElectronReAlgo.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"




TrigEgammaFastElectronReAlgo::TrigEgammaFastElectronReAlgo(const std::string & name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
{}


StatusCode TrigEgammaFastElectronReAlgo::initialize()
{
  ATH_MSG_DEBUG("Initialization:");

  if (!m_monTool.empty()) {
    ATH_MSG_DEBUG("Retrieving monTool");
    CHECK(m_monTool.retrieve());
  } else {
    ATH_MSG_DEBUG("No monTool configured => NO MONITOING");
  }
 
  if ( m_caloExtensionTool.retrieve().isFailure() ) {  
        ATH_MSG_ERROR("Unable to locate TrackExtrapolator tool ");
        return StatusCode::FAILURE; 
  }  

  // print out settings
  ATH_MSG_DEBUG("Initialization completed successfully:"); 
  ATH_MSG_DEBUG( "AcceptAll            = "<< (m_acceptAll==true ? "True" : "False")); 
  ATH_MSG_DEBUG("TrackPt              = " << m_trackPtthr);          
  ATH_MSG_DEBUG("TrackPtHighEt        = " << m_trackPtthr_highet);          
  ATH_MSG_DEBUG("ClusEt               = " << m_clusEtthr);          
  ATH_MSG_DEBUG("CaloTrackdEtaNoExtrap= " << m_calotrkdeta_noextrap);        
  ATH_MSG_DEBUG("CaloTrackdEtaNoExtrapHighEt= " << m_calotrkdeta_noextrap_highet);        
  ATH_MSG_DEBUG("CaloTrackdETA        = " << m_calotrackdeta);        
  ATH_MSG_DEBUG("CaloTrackdPHI        = " << m_calotrackdphi);        
  ATH_MSG_DEBUG("CaloTrackdEoverPLow  = " << m_calotrackdeoverp_low); 
  ATH_MSG_DEBUG("CaloTrackdEoverPHigh = " << m_calotrackdeoverp_high);

  ATH_CHECK( m_roiCollectionKey.initialize() );
  ATH_CHECK( m_TrigEMClusterContainerKey.initialize() );
  ATH_CHECK( m_TrackParticleContainerKey.initialize() );
  ATH_CHECK( m_outputElectronsKey.initialize() );
  ATH_CHECK( m_outputDummyElectronsKey.initialize() );

  return StatusCode::SUCCESS;
}



StatusCode TrigEgammaFastElectronReAlgo::execute(const EventContext& ctx) const 
{
  using namespace xAOD;   

  ATH_MSG_DEBUG( "Executing " <<name());

  auto trigElecColl =   SG::makeHandle (m_outputElectronsKey, ctx);  
  ATH_CHECK( trigElecColl.record (std::make_unique<xAOD::TrigElectronContainer>(),
                           std::make_unique<xAOD::TrigEMClusterAuxContainer>()) );
  
  ATH_MSG_DEBUG( "Made WriteHandle " << m_outputElectronsKey );

  auto trigDummyElecColl =   SG::makeHandle (m_outputDummyElectronsKey, ctx);
 
  ATH_MSG_DEBUG( "Made Dummy WriteHandle " << m_outputDummyElectronsKey );
  ATH_CHECK( trigDummyElecColl.record (std::make_unique<xAOD::TrigElectronContainer>(),
                           std::make_unique<xAOD::TrigEMClusterAuxContainer>()) );  

  auto roiCollection = SG::makeHandle(m_roiCollectionKey, ctx);
  ATH_MSG_DEBUG( "Made handle " << m_roiCollectionKey  );
  

  if (roiCollection->size()==0) {
    ATH_MSG_DEBUG(" RoI collection size = 0");
    return StatusCode::SUCCESS;
  }

  const TrigRoiDescriptor* roiDescriptor = *(roiCollection->begin());

  ATH_MSG_DEBUG(" RoI ID = "   << (roiDescriptor)->roiId() << ": Eta = "      << (roiDescriptor)->eta()
                << ", Phi = "      << (roiDescriptor)->phi());
  
  float calo_eta(999), calo_phi(999), calo_et(-1);

  auto clusterCont = SG::makeHandle (m_TrigEMClusterContainerKey, ctx);
  ATH_MSG_DEBUG( "Made handle " << m_TrigEMClusterContainerKey  );
  
  //JTB Should only be 1 cluster in each RoI 

  const xAOD::TrigEMCluster *emCluster=(*clusterCont->begin());
  ElementLink<xAOD::TrigEMClusterContainer> clusEL=ElementLink<xAOD::TrigEMClusterContainer> (*clusterCont,0);

  // copy relevant quantities to local variables
  calo_eta = emCluster->eta();
  calo_phi = emCluster->phi();
  calo_et  = emCluster->et();


  // Transverse em energy
  ATH_MSG_DEBUG("Cluster: ET=" << calo_et);
  ATH_MSG_DEBUG("searching a matching track: loop over tracks");

  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_TrackParticleContainerKey, ctx);
  ATH_MSG_DEBUG( "Made handle " << m_TrackParticleContainerKey  );


  if (tracks->size() == 0){
      return StatusCode::SUCCESS; // Exit early if there are no tracks
  }

  size_t coll_size = tracks->size();
  trigElecColl->reserve(coll_size);

  // monitoring
  std::vector<float> calotrkdeta_noextrap_mon; //!< monitor preselection between track eta and cluster before extrapolation 

  auto caloPtMon = Monitored::Collection("PtCalo",   *trigElecColl,  getCaloPt );
  auto trackPtMon = Monitored::Collection("PtTrack", *trigElecColl, getTkPt );
  auto caloTrackDEtaMon = Monitored::Collection("CaloTrackdEta", *trigElecColl, &xAOD::TrigElectron::trkClusDeta );
  auto caloTrackDPhiMon = Monitored::Collection("CaloTrackdPhi", *trigElecColl, &xAOD::TrigElectron::trkClusDphi );
  auto etOverPtMon = Monitored::Collection("CaloTrackEoverP", *trigElecColl, &xAOD::TrigElectron::etOverPt );
  auto caloTrackDEtaNoExtrapMon = Monitored::Collection("CaloTrackdEtaNoExtrapMon", calotrkdeta_noextrap_mon );

  auto mon = Monitored::Group(m_monTool,  caloPtMon, trackPtMon, caloTrackDEtaMon, caloTrackDPhiMon, etOverPtMon, caloTrackDEtaNoExtrapMon );

  // Make Dummy Electron
  xAOD::TrigElectron* trigDummyElec = new xAOD::TrigElectron();

  ElementLink<xAOD::TrackParticleContainer> trackDummyEL = ElementLink<xAOD::TrackParticleContainer> (*tracks, 0);
     
  trigDummyElecColl->push_back(trigDummyElec);
  trigDummyElec->init( 0, 0, 0,  0, clusEL, trackDummyEL);
 


  // loop over tracks
  for(unsigned int track_index = 0; track_index < tracks->size(); track_index++)
  {
    const xAOD::TrackParticle_v1* trkIter = (*tracks)[track_index];
    ATH_MSG_DEBUG("Track loop starts");
    ATH_MSG_VERBOSE("AlgoId = " << (trkIter)->patternRecoInfo());
    ATH_MSG_VERBOSE("At perigee:");
    ATH_MSG_DEBUG(" Pt  = " << std::abs((trkIter)->pt())); 
    ATH_MSG_DEBUG(" phi = " << std::abs((trkIter)->phi0()));
    ATH_MSG_DEBUG(" eta = " << std::abs((trkIter)->eta())); 
    ATH_MSG_DEBUG(" z0  = " << std::abs((trkIter)->z0()));
    ATH_MSG_DEBUG(" d0  = " << std::abs((trkIter)->d0()));  
    // ============================================= //
    // Pt cut
    float trkPt = std::abs((trkIter)->pt());
    float etoverpt = std::abs(calo_et/trkPt);
    float calotrkdeta_noextrap = (trkIter)->eta() - calo_eta;
    
    double etaAtCalo=999.;
    double phiAtCalo=999.;
    if(m_acceptAll){
      if(!extrapolate(emCluster,trkIter,etaAtCalo,phiAtCalo)){
        ATH_MSG_VERBOSE("extrapolator failed");
        continue; 
      }
      else{
        ATH_MSG_VERBOSE("REGTEST: TrigElectron: cluster index = " << clusEL.index() <<
               " eta = " << etaAtCalo << " phi = " << phiAtCalo);

        xAOD::TrigElectron* trigElec = new xAOD::TrigElectron();
        trigElecColl->push_back(trigElec);
        ElementLink<xAOD::TrackParticleContainer> trackEL = ElementLink<xAOD::TrackParticleContainer> (*tracks, track_index);
        trigElec->init(  roiDescriptor->roiWord(),
                    etaAtCalo, phiAtCalo,  etoverpt,        
                    clusEL,
                    trackEL);
        calotrkdeta_noextrap_mon.push_back(calotrkdeta_noextrap);
      }
    }else {

      ATH_MSG_DEBUG("Apply cuts on track with index: "<<track_index);
      if(trkPt < m_trackPtthr){
        ATH_MSG_DEBUG("Failed track pt cut " << trkPt);
        continue;
      }
      if(std::abs(calotrkdeta_noextrap) > m_calotrkdeta_noextrap){
        ATH_MSG_DEBUG("Failed pre extrapolation calo track deta " << calotrkdeta_noextrap);
        continue;
      }
      if(calo_et > m_clusEtthr){
        if(trkPt < m_trackPtthr_highet){
          ATH_MSG_DEBUG("Failed track pt cut for high et cluster");
          continue;
        }
        if(calotrkdeta_noextrap > m_calotrkdeta_noextrap_highet){
          ATH_MSG_DEBUG("Failed pre extrapolation calo track deta for high et");
          continue;
        }
      }
      if (etoverpt < m_calotrackdeoverp_low){ 
        ATH_MSG_DEBUG("failed low cut on ET/PT");
        continue;
      }
      if (etoverpt > m_calotrackdeoverp_high){ 
        ATH_MSG_DEBUG("failed high cut on ET/PT");
        continue;
      }
      if(!extrapolate(emCluster,trkIter,etaAtCalo,phiAtCalo)){
        ATH_MSG_DEBUG("extrapolator failed 1");
        continue; 
      }
      // all ok: do track-matching cuts
      ATH_MSG_DEBUG("extrapolated eta/phi=" << etaAtCalo << "/" << phiAtCalo);
      // match in eta
      float dEtaCalo = std::abs(etaAtCalo - calo_eta);
      ATH_MSG_DEBUG("deta = " << dEtaCalo); 
      if ( dEtaCalo > m_calotrackdeta){ 
        ATH_MSG_DEBUG("failed eta match cut " << dEtaCalo);
        continue;
      }
      // match in phi: deal with differences larger than Pi
      float dPhiCalo =  std::abs(phiAtCalo - calo_phi);
      dPhiCalo = ( dPhiCalo < M_PI ? dPhiCalo : 2*M_PI - dPhiCalo );
      ATH_MSG_DEBUG("dphi = " << dPhiCalo); 
      if ( dPhiCalo > m_calotrackdphi) {
        ATH_MSG_DEBUG("failed phi match cut " << dPhiCalo);
        continue;
      }
      // all cuts passed
      /** Create a TrigElectron corresponding to this candidate
        assume cluster quantities give better estimate of transverse energy
        (probably a safe assumption for large pT) and that track parameters
        at perigee give better estimates of angular quantities */
      ElementLink<xAOD::TrackParticleContainer> trackEL = ElementLink<xAOD::TrackParticleContainer> (*tracks, track_index);
      ATH_MSG_DEBUG("pT of trackEL: "<<(*trackEL)->pt());     

      ATH_MSG_DEBUG("REGTEST: TrigElectron: cluster index = " << clusEL.index() <<
                " track = "     << trkIter << " eta = " <<  etaAtCalo << " phi = " << phiAtCalo << 
                " deta = " << dEtaCalo << "dphi = " << dPhiCalo);
      xAOD::TrigElectron* trigElec = new xAOD::TrigElectron();
      trigElecColl->push_back(trigElec);
      trigElec->init( roiDescriptor->roiWord(),
                etaAtCalo, phiAtCalo,  etoverpt,        
                clusEL,
                trackEL);
      
      ATH_MSG_DEBUG(" deta = " << dEtaCalo << " deta = " << trigElec->trkClusDeta() 
                << " dphi = " << dPhiCalo << " dphi = " << trigElec->trkClusDphi()
                << " caloEta = " << calo_eta << " caloEta = " << trigElec->caloEta()
                << " caloPhi = " << calo_phi << " calophi = " << trigElec->caloPhi()
                << " etaAtCalo = " << etaAtCalo << " etaAtCalo = " << trigElec->trkEtaAtCalo()
                << " phiAtCalo = " << phiAtCalo << " phiAtCalo = " << trigElec->trkPhiAtCalo()
                );
      calotrkdeta_noextrap_mon.push_back(calotrkdeta_noextrap);
     
    }
    
  }


  ATH_MSG_DEBUG("REGTEST:  returning an xAOD::TrigElectronContainer with size "<< trigElecColl->size() << ".");
  
  return StatusCode::SUCCESS;
}

bool TrigEgammaFastElectronReAlgo::extrapolate(const xAOD::TrigEMCluster *clus, const xAOD::TrackParticle *trk, 
                                              double &etaAtCalo, double &phiAtCalo) const
{
    CaloExtensionHelpers::LayersToSelect layersToSelect; 
    layersToSelect.insert(CaloSampling::CaloSample::EMB2); 
    layersToSelect.insert(CaloSampling::CaloSample::EME2); 
    // extrapolate track using tool
    // get calo extension
    std::unique_ptr<Trk::CaloExtension> caloExtension =
      m_caloExtensionTool->caloExtension(Gaudi::Hive::currentContext(), *trk);
    if (!caloExtension || caloExtension->caloLayerIntersections().empty()) {
      ATH_MSG_VERBOSE("extrapolator failed 1");
      return false;
    }
    // extract eta/phi in EM2 
    CaloExtensionHelpers::EtaPhiPerLayerVector intersections; 
    CaloExtensionHelpers::midPointEtaPhiPerLayerVector( *caloExtension, intersections, &layersToSelect ); 
    if( intersections.empty() ) { 
        ATH_MSG_VERBOSE("extrapolator failed 2");
        return false;
    }  
    // pick the correct sample in case of ambiguity 
    std::tuple<CaloSampling::CaloSample, double, double> etaPhiTuple = intersections.front(); 
    if( intersections.size() == 2 )  
        if ( clus->energy(CaloSampling::CaloSample::EME2) > clus->energy(CaloSampling::CaloSample::EMB2) ) 
            etaPhiTuple=intersections.back(); 
    etaAtCalo = std::get<1>(etaPhiTuple); 
    phiAtCalo = std::get<2>(etaPhiTuple); 
    return true;
}
