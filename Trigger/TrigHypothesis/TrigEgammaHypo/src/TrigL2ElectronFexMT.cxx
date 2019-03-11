// -*- C++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2ElectronFexMT.cxx
 **
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
#include "TrigL2ElectronFexMT.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "AthenaMonitoring/Monitored.h"
class ISvcLocator;

template <class SRC>
inline const DataVector<xAOD::TrigElectron>** dvec_cast(SRC** ptr) { 
  return (const DataVector<xAOD::TrigElectron>**)(ptr); 
} 


TrigL2ElectronFexMT::TrigL2ElectronFexMT(const std::string & name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
{

    m_extrapolator_failed = 0;
}


TrigL2ElectronFexMT::~TrigL2ElectronFexMT()
{}


StatusCode TrigL2ElectronFexMT::initialize()
{
  ATH_MSG_DEBUG("Initialization:");

  if (!m_monTool.empty()) {
    ATH_MSG_DEBUG("Retrieving monTool");
    CHECK(m_monTool.retrieve());
  } else {
    ATH_MSG_INFO("No monTool configured => NO MONITOING");
  }
  // initialize error counter
  m_extrapolator_failed = 0;

  if ( m_caloExtensionTool.retrieve().isFailure() ) {  
        ATH_MSG_ERROR("Unable to locate TrackExtrapolator tool ");
        return StatusCode::FAILURE; 
  }  

  // print out settings
  ATH_MSG_DEBUG("Initialization completed successfully:"); 
  ATH_MSG_DEBUG( "AcceptAll            = "<< 
          (m_acceptAll==true ? "True" : "False")); 
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

  return StatusCode::SUCCESS;
}


StatusCode TrigL2ElectronFexMT::finalize()
{
    ATH_MSG_INFO("in finalize()");

    if (m_extrapolator_failed) 
        ATH_MSG_INFO("track extrapolation failed " << m_extrapolator_failed << " times");

    return  StatusCode::SUCCESS;
}


StatusCode TrigL2ElectronFexMT::execute() {
 using namespace xAOD;   

 ATH_MSG_DEBUG( "Executing " <<name());
  auto ctx = getContext();

  auto trigElecColl =   SG::makeHandle (m_outputElectronsKey, ctx);  
  ATH_CHECK( trigElecColl.record (std::make_unique<xAOD::TrigElectronContainer>(),
                           std::make_unique<xAOD::TrigEMClusterAuxContainer>()) );

  ATH_MSG_DEBUG( "Made WriteHandle " << m_outputElectronsKey );
  ATH_MSG_INFO( name() << " running with store " <<  getContext().getExtension<Atlas::ExtendedEventContext>().proxy()->name() );
 

  auto roiCollection = SG::makeHandle(m_roiCollectionKey, ctx);
  ATH_MSG_DEBUG( "Made handle " << m_roiCollectionKey  );
  
  //JTB For the moment assume 1 RoI (as in TrigL2ElectronFex) - could change to SuperRoI later

  //TrigRoiDescriptorCollection::const_iterator roiDescriptor = roiCollection->begin();
  //TrigRoiDescriptorCollection::const_iterator roiE = roiCollection->end();

  if (roiCollection->size()==0) {
    ATH_MSG_DEBUG(" RoI collection size = 0");
    return StatusCode::SUCCESS;
  }

  const TrigRoiDescriptor* roiDescriptor = *(roiCollection->begin());

  ATH_MSG_DEBUG(" RoI ID = "   << (roiDescriptor)->roiId()
		<< ": Eta = "      << (roiDescriptor)->eta()
		<< ", Phi = "      << (roiDescriptor)->phi());
  
  float calo_eta(999), calo_phi(999), calo_et(-1);

  auto clusContainer = SG::makeHandle (m_TrigEMClusterContainerKey, ctx);
  ATH_MSG_DEBUG( "Made handle " << m_TrigEMClusterContainerKey  );
  
  
  //JTB Should only be 1 cluster in each RoI 

  const xAOD::TrigEMCluster *el_t2calo_clus=(*clusContainer->begin());
  ElementLink<xAOD::TrigEMClusterContainer> clusEL=ElementLink<xAOD::TrigEMClusterContainer> (*clusContainer,0);

  // copy relevant quantities to local variables
  calo_eta = el_t2calo_clus->eta();
  calo_phi = el_t2calo_clus->phi();
  calo_et  = el_t2calo_clus->et();


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



  // loop over tracks

  unsigned int track_index=0;
  for(const auto trkIter:(*tracks)){
      ATH_MSG_VERBOSE("AlgoId = " << (trkIter)->patternRecoInfo());
      ATH_MSG_VERBOSE("At perigee:");
      ATH_MSG_VERBOSE(" Pt  = " << fabs((trkIter)->pt())); 
      ATH_MSG_VERBOSE(" phi = " << fabs((trkIter)->phi0()));
      ATH_MSG_VERBOSE(" eta = " << fabs((trkIter)->eta())); 
      ATH_MSG_VERBOSE(" z0  = " << fabs((trkIter)->z0()));  

      // ============================================= //
      // Pt cut
      float trkPt = fabs((trkIter)->pt());
      float etoverpt = fabs(calo_et/trkPt);
      float calotrkdeta_noextrap = (trkIter)->eta() - calo_eta;
      
      double etaAtCalo=999.;
      double phiAtCalo=999.;
      if(m_acceptAll){
	if(!extrapolate(el_t2calo_clus,trkIter,etaAtCalo,phiAtCalo)){
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
      }
      else {  
          ATH_MSG_DEBUG("Apply cuts");
          if(trkPt < m_trackPtthr){
              ATH_MSG_DEBUG("Failed track pt cut " << trkPt);
              continue;
          }
          if(fabs(calotrkdeta_noextrap) > m_calotrkdeta_noextrap){
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
          if(!extrapolate(el_t2calo_clus,trkIter,etaAtCalo,phiAtCalo)){
              ATH_MSG_DEBUG("extrapolator failed 1");
              continue; 
          }
          // all ok: do track-matching cuts
          ATH_MSG_DEBUG("extrapolated eta/phi=" << etaAtCalo << "/" << phiAtCalo);
          // match in eta
          float dEtaCalo = fabs(etaAtCalo - calo_eta);
          ATH_MSG_DEBUG("deta = " << dEtaCalo); 
          if ( dEtaCalo > m_calotrackdeta){ 
              ATH_MSG_DEBUG("failed eta match cut " << dEtaCalo);
              continue;
          }

          // match in phi: deal with differences larger than Pi
          float dPhiCalo =  fabs(phiAtCalo - calo_phi);
          dPhiCalo       = ( dPhiCalo < M_PI ? dPhiCalo : 2*M_PI - dPhiCalo );
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
          ATH_MSG_DEBUG("REGTEST: TrigElectron: cluster index = " << clusEL.index() <<
                  " track = "     << trkIter << " eta = " << 
                  etaAtCalo << " phi = " << phiAtCalo << 
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
      track_index++;
  }


  ATH_MSG_DEBUG("REGTEST:  returning an xAOD::TrigElectronContainer with size "<< trigElecColl->size() << ".");
  
  return StatusCode::SUCCESS;
}

bool TrigL2ElectronFexMT::extrapolate(const xAOD::TrigEMCluster *clus, const xAOD::TrackParticle *trk, double &etaAtCalo, double &phiAtCalo){
    CaloExtensionHelpers::LayersToSelect layersToSelect; 
    layersToSelect.insert(CaloSampling::CaloSample::EMB2); 
    layersToSelect.insert(CaloSampling::CaloSample::EME2); 
    // extrapolate track using tool
    // get calo extension 
    std::unique_ptr<Trk::CaloExtension> caloExtension = m_caloExtensionTool->caloExtension(*trk); 
    if( !caloExtension || caloExtension->caloLayerIntersections().empty() ) {
        ATH_MSG_VERBOSE("extrapolator failed 1");
        m_extrapolator_failed++;  
        return false;
    }  
    // extract eta/phi in EM2 
    CaloExtensionHelpers::EtaPhiPerLayerVector intersections; 
    CaloExtensionHelpers::midPointEtaPhiPerLayerVector( *caloExtension, intersections, &layersToSelect ); 
    if( intersections.empty() ) { 
        ATH_MSG_VERBOSE("extrapolator failed 2");
        m_extrapolator_failed++;  
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
