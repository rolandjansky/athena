// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2ElectronFex.cxx
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

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h" 
#include "TrkCaloExtension/CaloExtensionHelpers.h" 
#include "TrigEgammaHypo/TrigL2ElectronFex.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

template <class SRC>
inline const DataVector<xAOD::TrigElectron>** dvec_cast(SRC** ptr) { 
  return (const DataVector<xAOD::TrigElectron>**)(ptr); 
} 


TrigL2ElectronFex::TrigL2ElectronFex(const std::string & name, ISvcLocator* pSvcLocator)
    : HLT::FexAlgo(name, pSvcLocator),
      m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool", this)
{
    declareProperty( "AcceptAll",            m_acceptAll  = false );
    declareProperty( "ClusEt",              m_clusEtthr = 20.0*CLHEP::GeV );
    declareProperty( "TrackPt",              m_trackPtthr = 5.0*CLHEP::GeV );
    declareProperty( "CaloTrackdEtaNoExtrap",        m_calotrkdeta_noextrap );
    declareProperty( "TrackPtHighEt",              m_trackPtthr_highet = 2.0*CLHEP::GeV );
    declareProperty( "CaloTrackdEtaNoExtrapHighEt",        m_calotrkdeta_noextrap_highet = 0);
    declareProperty( "CaloTrackdETA",        m_calotrackdeta = 0);
    declareProperty( "CaloTrackdPHI",        m_calotrackdphi = 0); 
    declareProperty( "CaloTrackdEoverPLow",  m_calotrackdeoverp_low = 0);
    declareProperty( "CaloTrackdEoverPHigh", m_calotrackdeoverp_high = 0);
    declareProperty( "RCalBarrelFace",       m_RCAL = 1470.0*CLHEP::mm );
    declareProperty( "ZCalEndcapFace",       m_ZCAL = 3800.0*CLHEP::mm );
    declareProperty( "ParticleCaloExtensionTool",    m_caloExtensionTool);

    declareMonitoredStdContainer("PtCalo",m_calopt_mon);
    declareMonitoredStdContainer("PtTrack",m_trackpt_mon);
    declareMonitoredStdContainer("CaloTrackdEta",m_calotrackdeta_mon); 
    declareMonitoredStdContainer("CaloTrackdPhi",m_calotrackdphi_mon); 
    declareMonitoredStdContainer("CaloTrackEoverP",m_calotrackdeoverp_mon);
    declareMonitoredStdContainer("CaloTrackdEtaNoExtrapMon",m_calotrkdeta_noextrap_mon);
    // initialize error counter
    m_extrapolator_failed = 0;
}


TrigL2ElectronFex::~TrigL2ElectronFex()
{}


HLT::ErrorCode TrigL2ElectronFex::hltInitialize()
{
  ATH_MSG_DEBUG("Initialization:");

  // initialize error counter
  m_extrapolator_failed = 0;

  if ( m_caloExtensionTool.retrieve().isFailure() ) {  
        ATH_MSG_ERROR("Unable to locate TrackExtrapolator tool ");
        return HLT::BAD_JOB_SETUP; 
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

  return HLT::OK;
}


HLT::ErrorCode TrigL2ElectronFex::hltFinalize()
{
    ATH_MSG_INFO("in finalize()");

    if (m_extrapolator_failed) 
        ATH_MSG_INFO("track extrapolation failed " << m_extrapolator_failed << " times");

    return HLT::OK;
}


HLT::ErrorCode TrigL2ElectronFex::hltExecute(const HLT::TriggerElement* inputTE,
					   HLT::TriggerElement* outputTE) {
  HLT::ErrorCode stat = HLT::OK;

  // Collection may be never used. Better only create if necessary
  // NULL value is specially important to avoid crashs in monitoring
  //m_trigElecColl = NULL;
  xAOD::TrigElectronContainer *trigElecColl = new xAOD::TrigElectronContainer();
  xAOD::TrigElectronAuxContainer trigElecAuxContainer;
  trigElecColl->setStore(&trigElecAuxContainer);

  bool pass = false;
  bool result = false;
  // Accept-All mode: temporary patch; should be done with force-accept
  if (m_acceptAll) {
      pass = true;
      ATH_MSG_DEBUG("AcceptAll property is set: taking all events");
  }
  else {
      pass = false;
      ATH_MSG_DEBUG("AcceptAll property not set: applying selection");
  }

  // get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(inputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    ATH_MSG_WARNING("No RoI for this Trigger Element! ");
    return HLT::NAV_ERROR;
  }
  
  // get the initial RoI descriptor 
  const TrigRoiDescriptor* initialRoI = 0;
  if (getFeature(inputTE,  initialRoI, "initialRoI") != HLT::OK) {
    initialRoI = roiDescriptor;
    ATH_MSG_WARNING("Initial RoI was not found for this Trigger Element! ");
  }

  ATH_MSG_DEBUG("Using inputTE("<< inputTE <<")->getId(): " << inputTE->getId()
          << "; RoI ID = "   << roiDescriptor->roiId()
          << "; RoI word="   << initialRoI->roiWord()
          << ": Eta = "      << roiDescriptor->eta()
          << ", Phi = "      << roiDescriptor->phi());
  
  float calo_eta(999), calo_phi(999), calo_et(-1);
  // the ElementLink pointing to a TrigEMCluster needs to be declared in this scope
  ElementLink<xAOD::TrigEMClusterContainer> el_t2calo_clus;
  
  // retrieve TrigEMCluster using the Navigation methods
  stat = getFeatureLink<xAOD::TrigEMClusterContainer,xAOD::TrigEMCluster>(inputTE, el_t2calo_clus);
    
  if ( stat == HLT::OK && el_t2calo_clus.isValid() ) {
    // copy relevant quantities to local variables
    calo_eta = (*el_t2calo_clus)->eta();
    calo_phi = (*el_t2calo_clus)->phi();
    calo_et  = (*el_t2calo_clus)->et();
  } 
  else {
      ATH_MSG_WARNING("Failed to get TrigEMCluster");
      return HLT::MISSING_FEATURE;
  }

  // Transverse em energy
  ATH_MSG_DEBUG("Cluster: ET=" << calo_et);
  ATH_MSG_DEBUG("searching a matching track: loop over tracks");

  // get vector of TrigInDetTrackCollections
  ElementLinkVector< xAOD::TrackParticleContainer > v_inputTracks;
  stat = getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE, v_inputTracks, "");

  if ( stat != HLT::OK){ 
      ATH_MSG_WARNING("Failed to retrieve track EL collection");
      return HLT::MISSING_FEATURE;
  }

  ATH_MSG_DEBUG("Got vector of " << v_inputTracks.size()
          << " InDetTrackCollections");
  std::vector<const xAOD::TrackParticleContainer*> vectorTrackParticleContainer;
  stat = getFeatures(inputTE, vectorTrackParticleContainer);
  if (stat != HLT::OK){
      ATH_MSG_WARNING("Failed to retrieve track collection");
      return HLT::MISSING_FEATURE;
  }
  if (vectorTrackParticleContainer.size() < 1){
      ATH_MSG_ERROR("No track collection, vector < 1");
      return HLT::MISSING_FEATURE;
  }
  const xAOD::TrackParticleContainer *tracks = vectorTrackParticleContainer.back();
  if(!tracks){
      ATH_MSG_WARNING("Track collection is null");
      return HLT::MISSING_FEATURE;
  }
  
  size_t coll_size = tracks->size();
  trigElecColl->reserve(coll_size);

  // loop over tracks
  for(const ElementLink<xAOD::TrackParticleContainer> trkLink:v_inputTracks){
      const xAOD::TrackParticle* trkIter = *trkLink;
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
          if(!extrapolate(*el_t2calo_clus,trkIter,etaAtCalo,phiAtCalo)){
              ATH_MSG_VERBOSE("extrapolator failed");
              continue; 
          }
          else{
              ATH_MSG_VERBOSE("REGTEST: TrigElectron: cluster = " <<
                      el_t2calo_clus.getStorableObjectPointer() << " index = " << el_t2calo_clus.index() <<
                      " track = "     << trkIter << " eta = " << etaAtCalo << " phi = " << phiAtCalo); 
              xAOD::TrigElectron* trigElec = new xAOD::TrigElectron();
              trigElecColl->push_back(trigElec);
              trigElec->init(  initialRoI->roiWord(),
                      etaAtCalo, phiAtCalo,  etoverpt,        
                      el_t2calo_clus,
                      trkLink);
              m_calotrackdeta_mon.push_back(trigElec->trkClusDeta()); 
              m_calotrackdphi_mon.push_back(trigElec->trkClusDphi()); 
              m_calotrackdeoverp_mon.push_back(trigElec->etOverPt());
              m_trackpt_mon.push_back(getTkPt(trigElec));
              m_calopt_mon.push_back(getCaloPt(trigElec));
              m_calotrkdeta_noextrap_mon.push_back(calotrkdeta_noextrap);
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
          if(!extrapolate(*el_t2calo_clus,trkIter,etaAtCalo,phiAtCalo)){
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
          result = true;
          /** Create a TrigElectron corresponding to this candidate
            assume cluster quantities give better estimate of transverse energy
            (probably a safe assumption for large pT) and that track parameters
            at perigee give better estimates of angular quantities */

          ATH_MSG_DEBUG("REGTEST: TrigElectron: cluster = " <<
                  el_t2calo_clus.getStorableObjectPointer() << 
                  " index = " << el_t2calo_clus.index() <<
                  " track = "     << trkIter << " eta = " << 
                  etaAtCalo << " phi = " << phiAtCalo << 
                  " deta = " << dEtaCalo << "dphi = " << dPhiCalo);

          xAOD::TrigElectron* trigElec = new xAOD::TrigElectron();
          trigElecColl->push_back(trigElec);
          trigElec->init(  initialRoI->roiWord(),
                  etaAtCalo, phiAtCalo,  etoverpt,        
                  el_t2calo_clus,
                  trkLink);
          ATH_MSG_DEBUG(" deta = " << dEtaCalo << " deta = " << trigElec->trkClusDeta() 
                  << " dphi = " << dPhiCalo << " dphi = " << trigElec->trkClusDphi()
                  << " caloEta = " << calo_eta << " caloEta = " << trigElec->caloEta()
                  << " caloPhi = " << calo_phi << " calophi = " << trigElec->caloPhi()
                  << " etaAtCalo = " << etaAtCalo << " etaAtCalo = " << trigElec->trkEtaAtCalo()
                  << " phiAtCalo = " << phiAtCalo << " phiAtCalo = " << trigElec->trkPhiAtCalo()
                  );

          m_calotrackdeta_mon.push_back(trigElec->trkClusDeta()); 
          m_calotrackdphi_mon.push_back(trigElec->trkClusDphi()); 
          m_calotrackdeoverp_mon.push_back(trigElec->etOverPt());
          m_trackpt_mon.push_back(getTkPt(trigElec));
          m_calopt_mon.push_back(getCaloPt(trigElec));
          m_calotrkdeta_noextrap_mon.push_back(calotrkdeta_noextrap);
      }
  }

  // set output TriggerElement unless acceptAll is set
  if (!m_acceptAll) pass = result;
  ATH_MSG_DEBUG("acceptInput = " << pass);

  if (!trigElecColl) {
      ATH_MSG_DEBUG("Execute called will NULL pointer to TrigElectronContainer");
      trigElecColl = nullptr;
      return  HLT::OK;
  }

  // attach the TrigElectronCollection to the TriggerElement so
  // it is accessible from trigger navigation
  stat = attachFeature(outputTE, trigElecColl, "L2ElectronFex");
  if ( stat != HLT::OK ) {
      ATH_MSG_WARNING("Failed to attach TrigElectronContainer to navigation");
      return stat;
  }
  // print debug info
  ATH_MSG_DEBUG("REGTEST:  storing a collection with size "<< trigElecColl->size() << ".");
  return HLT::OK;
}

bool TrigL2ElectronFex::extrapolate(const xAOD::TrigEMCluster *clus, const xAOD::TrackParticle *trk, double &etaAtCalo, double &phiAtCalo){
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
