// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//#include "TrigInDetToolInterfaces/ITrigInDetTrackExtrapolator.h"
//#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h" 
#include "TrkCaloExtension/CaloExtensionHelpers.h" 
#include "TrigEgammaHypo/TrigL2ElectronFex.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

class ISvcLocator;

template <class SRC>
inline const DataVector<xAOD::TrigElectron>** dvec_cast(SRC** ptr) { 
  return (const DataVector<xAOD::TrigElectron>**)(ptr); 
} 


TrigL2ElectronFex::TrigL2ElectronFex(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
    m_trigElecColl(0)
{
  // Read cuts - should probably get these from an xml file
  declareProperty( "AcceptAll",            m_acceptAll  = false );
  declareProperty( "EtaBins",              m_etabin );
  declareProperty( "TrackPt",              m_trackPtthr = 5.0*CLHEP::GeV );
  declareProperty( "CaloTrackdETA",        m_calotrackdeta );
  declareProperty( "CaloTrackdPHI",        m_calotrackdphi ); 
  declareProperty( "CaloTrackdEoverPLow",  m_calotrackdeoverp_low );
  declareProperty( "CaloTrackdEoverPHigh", m_calotrackdeoverp_high );
  declareProperty( "TrackAlgoId",          m_trackalgoID = 0 );
  declareProperty( "CaloAlgoId",           m_calo_algoID = 1 ); // 0=all (N/A); 1=T2Calo; 2=CaloRinger
  declareProperty( "dETACLUSTERthr",       m_detacluster = 0.1 );
  declareProperty( "dPHICLUSTERthr",       m_dphicluster = 0.1 );
  declareProperty( "RCalBarrelFace",       m_RCAL = 1470.0*CLHEP::mm );
  declareProperty( "ZCalEndcapFace",       m_ZCAL = 3800.0*CLHEP::mm );
  declareProperty( "ParticleCaloExtensionTool",    m_caloExtensionTool);
  
  declareProperty( "TrackPtTRT",              m_trackPtthrTRT = 5.0*CLHEP::GeV );
  declareProperty( "CaloTrackdETATRT",        m_calotrackdetaTRT ); 
  declareProperty( "CaloTrackdPHITRT",        m_calotrackdphiTRT );
  
  declareMonitoredCollection("PtCalo",*dvec_cast(&m_trigElecColl),&TrigL2ElectronFex::getCaloPt);
  declareMonitoredCollection("PtTrack",*dvec_cast(&m_trigElecColl),&TrigL2ElectronFex::getTkPt);
  declareMonitoredCollection("CaloTrackdEta",*dvec_cast(&m_trigElecColl),&xAOD::TrigElectron::trkClusDeta);
  declareMonitoredCollection("CaloTrackdPhi",*dvec_cast(&m_trigElecColl),&xAOD::TrigElectron::trkClusDphi);
  declareMonitoredCollection("CaloTrackEoverP",*dvec_cast(&m_trigElecColl),&xAOD::TrigElectron::etOverPt);

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
  ATH_MSG_DEBUG("EtaBins              = " << m_etabin);               
  ATH_MSG_DEBUG("CaloTrackdETA        = " << m_calotrackdeta);        
  ATH_MSG_DEBUG("CaloTrackdPHI        = " << m_calotrackdphi);        
  ATH_MSG_DEBUG("CaloTrackdEoverPLow  = " << m_calotrackdeoverp_low); 
  ATH_MSG_DEBUG("CaloTrackdEoverPHigh = " << m_calotrackdeoverp_high);
  ATH_MSG_DEBUG("TrackAlgoId          = " << m_trackalgoID);          
  ATH_MSG_DEBUG("CaloAlgoId           = " << m_calo_algoID);          
  ATH_MSG_DEBUG("dETACLUSTERthr       = " << m_detacluster);          
  ATH_MSG_DEBUG("dPHICLUSTERthr       = " << m_dphicluster);          

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
  xAOD::TrigElectronAuxContainer trigElecAuxContainer;
  if ( !m_trigElecColl ) {
      m_trigElecColl = new xAOD::TrigElectronContainer();
      m_trigElecColl->setStore(&trigElecAuxContainer);
  } else {
      m_trigElecColl->clear();
  }

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
  
  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();

  // CaloAlgoId now isn't used anymore, as TrigCaloRinger also dumps a TrigEMCluster
  // It is kept for backward compability.
  // Eventually it can be passed to the next algos for information?!
  
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

  
  // figure out what eta bin this cluster is in
  float absEta = fabs(calo_eta);
  int etaBin = 0;
  
  if(m_etabin.size()<2 ) {
    ATH_MSG_WARNING("etaBin not configured correctly");
    return HLT::OK;//eta bins not configured correctly
  }

  for (unsigned int iBin = 0; iBin < (m_etabin.size()-1); iBin++ )
      if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) etaBin = iBin;

  ATH_MSG_DEBUG("eta bin used for cuts: " << etaBin);


  // calculate eta and phi distances (deal with angle diferences > Pi)
  float dPhi =  calo_phi - phiRef;
  dPhi = ( fabs(dPhi) < M_PI ? dPhi : 2*M_PI - dPhi );
  
  float dEta = fabs( etaRef - calo_eta );


  // Following code was previously commented out
  // VD already cutting on TriL2CaloHypo
  // Accept only L2 em clusters which are "close" to the one provided by L1

  ATH_MSG_DEBUG("Not cutting on dEta=" << dEta << " or dPhi=" << dPhi);

  //if ( dEta > m_detacluster || dPhi > m_dphicluster ) {
  //  if ( msgLvl() <= MSG::DEBUG ) {
  //    msg() << MSG::DEBUG << "Cluster doesn't match RoI! Leaving." << endmsg;
  //  }
  //  return HLT::OK;
  //}

  
  // Transverse em energy
  ATH_MSG_DEBUG("Cluster: ET=" << calo_et);
  ATH_MSG_DEBUG("searching a matching track: loop over tracks");

  // get vector of TrigInDetTrackCollections
  ElementLinkVector< xAOD::TrackParticleContainer > v_inputTracks;
  stat = getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE, v_inputTracks, "");

  if ( stat != HLT::OK){ 
      ATH_MSG_WARNING("Failed to retrieve track collection");
      return HLT::MISSING_FEATURE;
  }

  ATH_MSG_DEBUG("Got vector of " << v_inputTracks.size()
          << " InDetTrackCollections");

  CaloExtensionHelpers::LayersToSelect layersToSelect; 
  layersToSelect.insert(CaloSampling::CaloSample::EMB2); 
  layersToSelect.insert(CaloSampling::CaloSample::EME2); 


  // loop over tracks
  for(const auto &trkLink:v_inputTracks){
      const xAOD::TrackParticle* trkIter = *trkLink;
      ATH_MSG_VERBOSE("AlgoId = " << (trkIter)->patternRecoInfo());
      ATH_MSG_VERBOSE("At perigee:");
      ATH_MSG_VERBOSE(" Pt  = " << fabs((trkIter)->pt())); 
      ATH_MSG_VERBOSE(" phi = " << fabs((trkIter)->phi0()));
      ATH_MSG_VERBOSE(" eta = " << fabs((trkIter)->eta())); 
      ATH_MSG_VERBOSE(" z0  = " << fabs((trkIter)->z0()));  

      // algorithm ID
      int algoId = 0; 
      if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::FastTrackFinderSeed] ) algoId=9;
      if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyA] ) algoId=5;
      if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyB] ) algoId=6;
      if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyC] ) algoId=7;

      // ======== Following to be checked ============== //
      // do not try track/cluster match if produced by wrong algo (0=all algos)
      // Following code needs to be checked for rel21
      // Is it required to check for track algorithm ID?
      if (m_trackalgoID == 0 || (unsigned int)algoId == m_trackalgoID || m_acceptAll  || 
              (m_trackalgoID == 5 &&  (unsigned int)algoId <= 2 )) {
          // =============================== ============== //
          
          ATH_MSG_VERBOSE("good track AlgoId");
          
          //Use SiTrack/IDScan cuts or TRT cuts
          std::vector<float> temp_calotrackdeta;
          std::vector<float> temp_calotrackdphi;
          float temp_trackPtthr;
          temp_calotrackdeta = m_calotrackdeta;
          temp_calotrackdphi = m_calotrackdphi;
          temp_trackPtthr = m_trackPtthr;

          // ======== Following to be checked ============== //
          // The following should be checked at initialize or before loop 
          // throw a warning/error and quit chain due to misconfiguration
          //
          //ignore tracks that don't have cuts 
          if(temp_calotrackdeta.size()<1 || temp_calotrackdphi.size()<1 ){
              ATH_MSG_DEBUG("Track type does not have corresponding cut configuration");
              continue;
          }
          //ignore incorrect cut configurations
          if(temp_calotrackdeta.size()!=(m_etabin.size()-1) || temp_calotrackdphi.size()!=(m_etabin.size()-1) ){
              ATH_MSG_DEBUG("Track type has inconsistent cut configuration");
              continue;
          }
          // ============================================= //
          // Pt cut
          float trkPt = fabs((trkIter)->pt());
          if (trkPt > temp_trackPtthr || m_acceptAll) {
              ATH_MSG_VERBOSE("passed pT cut");

              // match in ET/PT
              float etoverpt = fabs(calo_et/trkPt);
              if (etoverpt > m_calotrackdeoverp_low[etaBin] || m_acceptAll) {
                  ATH_MSG_VERBOSE("passed low cut on ET/PT");

                  if (etoverpt < m_calotrackdeoverp_high[etaBin] || m_acceptAll) {
                      ATH_MSG_VERBOSE("passed high cut on ET/PT");

                      // extrapolate track using tool
                      // get calo extension 
                      const Trk::CaloExtension* caloExtension = 0; 
                      bool useCaching = false; 

                      if( !m_caloExtensionTool->caloExtension(*trkIter,caloExtension,useCaching) || caloExtension->caloLayerIntersections().empty() ) { 
                          ATH_MSG_VERBOSE("extrapolator failed 1");
                          m_extrapolator_failed++;  
                          continue; 
                      }  
                      // extract eta/phi in EM2 
                      CaloExtensionHelpers::EtaPhiPerLayerVector intersections; 
                      CaloExtensionHelpers::midPointEtaPhiPerLayerVector( *caloExtension, intersections, &layersToSelect ); 
                      if( intersections.empty() ) { 
                          ATH_MSG_VERBOSE("extrapolator failed 2");
                          m_extrapolator_failed++; 
                          continue; 
                      }  
                      // pick the correct sample in case of ambiguity 
                      std::tuple<CaloSampling::CaloSample, double, double> etaPhiTuple = intersections.front(); 
                      if( intersections.size() == 2 )  
                          if ( (*el_t2calo_clus)->energy(CaloSampling::CaloSample::EME2) > (*el_t2calo_clus)->energy(CaloSampling::CaloSample::EMB2) ) 
                              etaPhiTuple=intersections.back(); 
                      double etaAtCalo = std::get<1>(etaPhiTuple); 
                      double phiAtCalo = std::get<2>(etaPhiTuple); 
                      
                      // all ok: do track-matching cuts
                      ATH_MSG_VERBOSE("extrapolated eta/phi=" << etaAtCalo << "/" << phiAtCalo);

                      // match in eta
                      float dEtaCalo = fabs(etaAtCalo - calo_eta);
                      if ( dEtaCalo < temp_calotrackdeta[etaBin] || m_acceptAll) {
                          ATH_MSG_VERBOSE("passed eta match cut");
                          
                          // match in phi: deal with differences larger than Pi
                          float dPhiCalo =  fabs(phiAtCalo - calo_phi);
                          dPhiCalo       = ( dPhiCalo < M_PI ? dPhiCalo : 2*M_PI - dPhiCalo );
                          if ( dPhiCalo < temp_calotrackdphi[etaBin] || m_acceptAll) {
                              ATH_MSG_VERBOSE("passed phi match cut");
                              // all cuts passed
                              result = true;
                              /** Create a TrigElectron corresponding to this candidate
                                assume cluster quantities give better estimate of transverse energy
                                (probably a safe assumption for large pT) and that track parameters
                                at perigee give better estimates of angular quantities */

                              ATH_MSG_DEBUG("REGTEST: TrigElectron: cluster = " <<
                                      el_t2calo_clus.getStorableObjectPointer() << " index = " << el_t2calo_clus.index() <<
                                      " track = "     << trkIter << " eta = " << etaAtCalo << " phi = " << phiAtCalo); 
                              xAOD::TrigElectron* trigElec = new xAOD::TrigElectron();
                              m_trigElecColl->push_back(trigElec);
                              trigElec->init(  initialRoI->roiWord(),
                                      etaAtCalo, phiAtCalo,  etoverpt,        
                                      el_t2calo_clus,
                                      trkLink);
                          } // dphi
                      } // deta
                  } //eoverPhi
              } //eoverPlow
          } //tmp track pt
      } // track type
  }

  // set output TriggerElement unless acceptAll is set
  if (!m_acceptAll) pass = result;
  ATH_MSG_DEBUG("acceptInput = " << pass);

  if (!m_trigElecColl) {
      ATH_MSG_DEBUG("Execute called will NULL pointer to TrigElectronContainer");
      m_trigElecColl = nullptr;
      return  HLT::OK;
  }

  // attach the TrigElectronCollection to the TriggerElement so
  // it is accessible from trigger navigation
  stat = attachFeature(outputTE, m_trigElecColl, "L2ElectronFex");
  if ( stat != HLT::OK ) {
      ATH_MSG_WARNING("Failed to attach TrigElectronContainer to navigation");
      return stat;
  }
  // print debug info
  ATH_MSG_DEBUG("REGTEST:  storing a collection with size "<< m_trigElecColl->size() << ".");
  // Requires NULL pointer for monitoring?
  m_trigElecColl = nullptr;
  return HLT::OK;
}
