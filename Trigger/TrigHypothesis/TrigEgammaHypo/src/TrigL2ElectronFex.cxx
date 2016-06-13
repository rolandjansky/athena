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
  //  declareProperty( "SaveTrigElectrons",    m_saveTrigEl = true );
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
//  declareProperty( "TrackExtrapolator",    m_trackExtrapolator);
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
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Initialization:" << endreq;

  // initialize error counter
  m_extrapolator_failed = 0;

  // retrieve extrapolation tool
  /*
  StatusCode sc = toolSvc()->retrieveTool(m_trackExtrapolatorName,
					  m_trackExtrapolatorName,
					  m_trackExtrapolator);
  StatusCode sc = m_trackExtrapolator.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrackExtrapolator tool " << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  */
  if ( m_caloExtensionTool.retrieve().isFailure() ) {  
        msg() << MSG::FATAL << "Unable to locate TrackExtrapolator tool " << endreq; 
        return HLT::BAD_JOB_SETUP; 
  }  

  // print out settings
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
    msg() << MSG::DEBUG << "AcceptAll            = "
	<< (m_acceptAll==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "TrackPt              = " << m_trackPtthr           << endreq;
    msg() << MSG::DEBUG << "EtaBins              = " << m_etabin               << endreq; 
    msg() << MSG::DEBUG << "CaloTrackdETA        = " << m_calotrackdeta        << endreq; 
    msg() << MSG::DEBUG << "CaloTrackdPHI        = " << m_calotrackdphi        << endreq; 
    msg() << MSG::DEBUG << "CaloTrackdEoverPLow  = " << m_calotrackdeoverp_low << endreq; 
    msg() << MSG::DEBUG << "CaloTrackdEoverPHigh = " << m_calotrackdeoverp_high<< endreq; 
    msg() << MSG::DEBUG << "TrackAlgoId          = " << m_trackalgoID          << endreq; 
    msg() << MSG::DEBUG << "CaloAlgoId           = " << m_calo_algoID          << endreq; 
    msg() << MSG::DEBUG << "dETACLUSTERthr       = " << m_detacluster          << endreq; 
    msg() << MSG::DEBUG << "dPHICLUSTERthr       = " << m_dphicluster          << endreq; 

  }

  return HLT::OK;
}


HLT::ErrorCode TrigL2ElectronFex::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  if (m_extrapolator_failed) 
    msg() << MSG::INFO << "track extrapolation failed " << m_extrapolator_failed << " times" << endreq;

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
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"
	    << endreq;
    }
  } else {
    pass = false;
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
	    << endreq;
    }
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

    // cluster found: print debug information
    //if ( msgLvl() <= MSG::DEBUG ) (*el_t2calo_clus)->print(msg());

  } else {
    if ( msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Failed to get TrigEMCluster" << endreq;
    return HLT::MISSING_FEATURE;
  }

  
  // figure out what eta bin this cluster is in
  float absEta = fabs(calo_eta);
  int etaBin = 0;
  
  if(m_etabin.size()<2 ) {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "etaBin not configured correctly" << endreq;
    return HLT::OK;//eta bins not configured correctly
  }

  for (unsigned int iBin = 0; iBin < (m_etabin.size()-1); iBin++ )
    if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) etaBin = iBin;

  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "eta bin used for cuts: " << etaBin << endreq;


  // calculate eta and phi distances (deal with angle diferences > Pi)
  float dPhi =  calo_phi - phiRef;
  dPhi = ( fabs(dPhi) < M_PI ? dPhi : 2*M_PI - dPhi );
  
  float dEta = fabs( etaRef - calo_eta );


  // VD already cutting on TriL2CaloHypo
  // Accept only L2 em clusters which are "close" to the one provided by L1

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Not cutting on dEta=" << dEta << " or dPhi=" << dPhi << endreq;
  }

  //if ( dEta > m_detacluster || dPhi > m_dphicluster ) {
  //  if ( msgLvl() <= MSG::DEBUG ) {
  //    msg() << MSG::DEBUG << "Cluster doesn't match RoI! Leaving." << endreq;
  //  }
  //  return HLT::OK;
  //}

  
  // Transverse em energy
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Cluster: ET=" << calo_et << endreq;
    msg() << MSG::DEBUG << "searching a matching track: loop over tracks"
	<< endreq;
  }


  // get vector of TrigInDetTrackCollections
  ElementLinkVector< xAOD::TrackParticleContainer > v_inputTracks;
  stat = getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE, v_inputTracks, "");

  if ( stat != HLT::OK || v_inputTracks.size() < 1) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No track collections found! Leaving." << endreq;
    return HLT::OK;
  }

  if (  msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Got vector of " << v_inputTracks.size()
	  << " InDetTrackCollections" << endreq;

  CaloExtensionHelpers::LayersToSelect layersToSelect; 
  layersToSelect.insert(CaloSampling::CaloSample::EMB2); 
  layersToSelect.insert(CaloSampling::CaloSample::EME2); 


  // loop over track collections
  int n_coll = 0;
  ElementLinkVector< xAOD::TrackParticleContainer >::const_iterator collIter,collEnd=(v_inputTracks).end();
  for ( collIter = v_inputTracks.begin(); collIter != collEnd; ++n_coll, ++collIter) {

    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "Starting on collection nr." << n_coll <<endreq;

    // loop over tracks
    int n_trk = 0;
    const xAOD::TrackParticle* trkIter = *(collIter->cptr()); //,trkEnd = (collIter)->end();
    //for ( trkIter = (collIter)->begin(); trkIter != trkEnd; ++n_trk, ++trkIter) {

      if ( msgLvl() <= MSG::DEBUG ) {
	msg() << MSG::DEBUG << "Track " << n_trk << " in collection " << n_coll << endreq;
	msg() << MSG::DEBUG << "AlgoId = " << (trkIter)->patternRecoInfo() << endreq;
	msg() << MSG::DEBUG << "At perigee:" << endreq;
	msg() << MSG::DEBUG << " Pt  = " << fabs((trkIter)->pt())  << endreq;
	msg() << MSG::DEBUG << " phi = " << fabs((trkIter)->phi0())<< endreq;
	msg() << MSG::DEBUG << " eta = " << fabs((trkIter)->eta()) << endreq;
//	msg() << MSG::DEBUG << " a0  = " << fabs((trkIter)->a0())  << endreq;
	msg() << MSG::DEBUG << " z0  = " << fabs((trkIter)->z0())  << endreq;
      }

      // algorithm ID
      //TrackParticle::AlgoId algoId = (*trkIter)->algorithmId();
      //TrackParticle::AlgoId algoId = 0; //(*trkIter)->algorithmId();
      int algoId = 0; //(*trkIter)->algorithmId();
      if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::FastTrackFinderSeed] ) algoId=9;
      if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyA] ) algoId=5;
      if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyB] ) algoId=6;
      if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyC] ) algoId=7;

      // do not try track/cluster match if produced by wrong algo (0=all algos)
      if (m_trackalgoID == 0 || (unsigned int)algoId == m_trackalgoID || m_acceptAll  || 
	  (m_trackalgoID == 5 &&  (unsigned int)algoId <= 2 )) {

	if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "good track AlgoId" << endreq;

	//Use SiTrack/IDScan cuts or TRT cuts
	
	std::vector<float> temp_calotrackdeta;
	std::vector<float> temp_calotrackdphi;
	float temp_trackPtthr;
	temp_calotrackdeta = m_calotrackdeta;
	temp_calotrackdphi = m_calotrackdphi;
	temp_trackPtthr = m_trackPtthr;

	//ignore tracks that don't have cuts 
	if(temp_calotrackdeta.size()<1 || temp_calotrackdphi.size()<1 ){
	  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Track type does not have corresponding cut configuration" << endreq;
	  continue;
	}
	//ignore incorrect cut configurations
	if(temp_calotrackdeta.size()!=(m_etabin.size()-1) || temp_calotrackdphi.size()!=(m_etabin.size()-1) ){
	  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Track type has inconsistent cut configuration" << endreq;
	  continue;
	}

	// Pt cut
	float trkPt = fabs((trkIter)->pt());

	if (trkPt > temp_trackPtthr || m_acceptAll) {
	  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "passed pT cut" << endreq;

	  // match in ET/PT
	  float etoverpt = fabs(calo_et/trkPt);

	  if (etoverpt > m_calotrackdeoverp_low[etaBin] || m_acceptAll) {
	    msg() << MSG::DEBUG << "passed low cut on ET/PT" << endreq;

	    if (etoverpt < m_calotrackdeoverp_high[etaBin] || m_acceptAll) {
	      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "passed high cut on ET/PT" << endreq;

	      // extrapolate track using tool
	      //double phiAtCalo = 0, etaAtCalo = 0;
	      //double offset = 0.
                // get calo extension 
                const Trk::CaloExtension* caloExtension = 0; 
                bool useCaching = false; 
 
                if( !m_caloExtensionTool->caloExtension(*trkIter,caloExtension,useCaching) || caloExtension->caloLayerIntersections().empty() ) { 
                         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "extrapolator failed 1"<<endreq;  
                         m_extrapolator_failed++;  continue; 
                }  
                // extract eta/phi in EM2 
                CaloExtensionHelpers::EtaPhiPerLayerVector intersections; 
                CaloExtensionHelpers::midPointEtaPhiPerLayerVector( *caloExtension, intersections, &layersToSelect ); 
                if( intersections.empty() ) { 
                        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "extrapolator failed 2"<<endreq; 
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
		if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "extrapolated eta/phi=" << etaAtCalo
						    << "/" << phiAtCalo << endreq;

		// match in eta
		float dEtaCalo = fabs(etaAtCalo - calo_eta);

		if ( dEtaCalo < temp_calotrackdeta[etaBin] || m_acceptAll) {
		  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "passed eta match cut" << endreq;

		  // match in phi: deal with differences larger than Pi
		  float dPhiCalo =  fabs(phiAtCalo - calo_phi);
		  dPhiCalo       = ( dPhiCalo < M_PI ? dPhiCalo : 2*M_PI - dPhiCalo );

		  if ( dPhiCalo < temp_calotrackdphi[etaBin] || m_acceptAll) {
		    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "passed phi match cut" << endreq;

		    // all cuts passed
		    result = true;
		    
		    /** Create a TrigElectron corresponding to this candidate
			assume cluster quantities give better estimate of transverse energy
			(probably a safe assumption for large pT) and that track parameters
			at perigee give better estimates of angular quantities */
		    
		    msg() << MSG::DEBUG << "cluster pointer=" <<
			el_t2calo_clus.getStorableObjectPointer() << " and index=" << el_t2calo_clus.index()  << endreq;
		    msg() << MSG::DEBUG << "track pointer="     << (*collIter) << " and index=" << n_trk  << endreq;
								//roirDescriptor->roiWord(),
                    //ElementLink<const xAOD::TrackParticleContainer> el_t2trk (*collIter);
                    //if ( n_trk < el_t2trk->size() ) el_t2trk.toIndexredElement( collIter, n_trk );
                    //  ElementLinkVector< xAOD::TrackParticleContainer >::const_iterator collIter,collEnd=(v_inputTracks).end();
                    ElementLink< xAOD::TrackParticleContainer > trkLink ( *collIter );
		    trkLink.toIndexedElement( (*collIter).getStorableObjectRef(), n_trk );
		    xAOD::TrigElectron* trigElec = new xAOD::TrigElectron();
		    m_trigElecColl->push_back(trigElec);
			trigElec->init(  initialRoI->roiWord(),
								etaAtCalo, phiAtCalo,  etoverpt,        
								//el_t2calo_clus.getStorableObjectPointer(), el_t2calo_clus.index(),
								el_t2calo_clus,
								trkLink);
								//(*collIter), n_trk);
		      // obsolete: the constructor used to take dEtaCalo, dPhiCalo, etoverpt,
		    // Ok. If collection does not exist, build it for the first time
//		    if ( !m_trigElecColl ) m_trigElecColl = new xAOD::TrigElectronContainer();
		    // electron passed all cuts: push into collection
//		    m_trigElecColl->push_back(trigElec);
		    //if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << (*trigElec) << endreq;
		  } // dphi
		} // deta
	      //}// track extrapolation
	    }
	  } //eoverP
	} //tmp track pt
      } // track type
    //}
  }

  // set output TriggerElement unless acceptAll is set
  if (!m_acceptAll) pass = result;

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "acceptInput = " << pass << endreq;

  // return HLT::OK;


  if (!m_trigElecColl) {
    if ( msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG
	    << "Execute called will NULL pointer to TrigElectronContainer" << endreq;
      m_trigElecColl = NULL;
    return  HLT::OK;
  }

  // attach the TrigElectronCollection to the TriggerElement so
  // it is accessible from trigger navigation
  stat = attachFeature(outputTE, m_trigElecColl, "L2ElectronFex");
  if ( stat != HLT::OK ) {

    if ( msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING
	    << "Failed to attach TrigElectronContainer to navigation" << endreq;

    return stat;
  }

  // print debug info
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "* Summary:  storing a collection with size "
	  << m_trigElecColl->size() << "." <<endreq;

  int i = 0;

  for (xAOD::TrigElectronContainer::iterator elecIter = m_trigElecColl->begin();
       elecIter !=  m_trigElecColl->end(); ++i, ++elecIter) {

#ifdef DONTDO
    msg() << MSG::DEBUG
	  << "TrigElec[" << i << "]->isValid()=" << (*elecIter)->isValid()
	  << endreq;

    /* IMPORTANT: pt() is the 4-momentum base class method; returns cluster ET */
    msg() << MSG::DEBUG << (**elecIter) << endreq;
// 	  << ">eta0= " << (*elecIter)->eta()
// 	  << " phi0= " << (*elecIter)->phi()
// 	  << " calo: Et= "  << (*elecIter)->pt()
// 	  << " track: Pt= "  << (*elecIter)->Pt()
// 	  << " eta at calo= " << (*elecIter)->trkEtaAtCalo()
// 	  << " phi at calo= " << (*elecIter)->trkPhiAtCalo()
// 	  << " match: Deta= "  << (*elecIter)->trkClusDeta()
// 	  << " Dphi= "  << (*elecIter)->trkClusDphi()
// 	  << endreq;


    msg() << MSG::DEBUG
	  << ">Tracking algo " << (*elecIter)->trackAlgo()
	  << endreq;
#endif

  }
  // Requires NULL pointer for monitoring?
  m_trigElecColl = NULL;
  return HLT::OK;
}
