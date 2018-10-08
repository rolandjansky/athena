/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBphysTrackRoiMaker.h"

// RoI include
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

/**
 * Standard constructor for the algorithm
 */
TrigBphysTrackRoiMaker::TrigBphysTrackRoiMaker(const std::string& name, ISvcLocator* pSvcLocator) :
  FexAlgo(name, pSvcLocator),
  m_inputTrackCollectionKey("MultiTrkFex")

{

  declareProperty("InputRoiName",m_inputTrackCollectionKey="MultiTrkFex"); 

  declareMonitoredVariable("nTrksIn", m_nTrksIn, IMonitoredAlgo::AutoClear);
  declareMonitoredVariable("nRoiOut", m_nRoiOut, IMonitoredAlgo::AutoClear);
  declareMonitoredVariable("nRoiToTrkRatio", m_nRoiToTrkRatio, IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("ptTrkIn", m_ptTrkIn, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("etaTrkIn", m_etaTrkIn, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("phiTrkIn", m_phiTrkIn, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("ptRoiOut", m_ptRoiOut, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("etaRoiOut", m_etaRoiOut, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("phiRoiOut", m_phiRoiOut, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("dEtaTrkSel", m_dEtaTrkSel, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("dPhiTrkSel", m_dPhiTrkSel, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("dRTrkSel", m_dRTrkSel, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("logdRTrkSel", m_logdRTrkSel, IMonitoredAlgo::AutoClear);
  

}

/**
 * Destructor
 */
TrigBphysTrackRoiMaker::~TrigBphysTrackRoiMaker()
{

}

/**
 * Initialize the algo
 */
HLT::ErrorCode TrigBphysTrackRoiMaker::hltInitialize() {
  // print out the configuration
  ATH_MSG_DEBUG("Initializing TrigBphysTrackRoiMaker[" << name() << "]");
  ATH_MSG_DEBUG("inputTrackCollectionKey  = " << m_inputTrackCollectionKey);

  return HLT::OK;
}//hltInitialize

/** 
 * Finalise the algo - nothing to do yet
 */
HLT::ErrorCode TrigBphysTrackRoiMaker::hltFinalize() {
  return HLT::OK;
}

/**
 * Execute function - called for each roi
 */
HLT::ErrorCode TrigBphysTrackRoiMaker::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

  ATH_MSG_DEBUG("Executing TrigBphysTrackRoiMaker::hltExecute");
  beforeExecMonitors().ignore();
  outputTE->setActiveState(true);

  // get the RoI for the ID tracking
  const xAOD::TrackParticleContainer*  tracks = 0;
  if (HLT::OK != getFeature(inputTE, tracks, m_inputTrackCollectionKey) ) {
    ATH_MSG_WARNING("Could not get " << m_inputTrackCollectionKey << " trackParticles");
    return HLT::OK;
  }


  TrigRoiDescriptor* superRoI = new TrigRoiDescriptor(true); // create fullscan RoI
  superRoI->setComposite(true);
  int nIDtracks(0);
  int nFound = 0;
  for( auto track : *tracks){
    
    //if (!(track->perigeeParameters())) continue;   //Must have helix parameters
    double pt_id    = track->pt(); //perigeeParameters()->momentum().perp()*0.001; //in GeV/c
    double phi_id   = track->phi();//perigeeParameters()->parameters()[Trk::phi0];
    //double theta    = track->perigeeParameters()->parameters()[Trk::theta] ;
    double eta_id   = track->eta();//-log(fabs(tan(theta*0.5)));
    
    // FIXME: MS what are the eta and phi resolutions? are these set correctly?
    //        Use these for the roi sizes
    // double deta_id = track->perigeeParameters()->parameters()[Trk::eeta];

    //double zPos_id  = track->perigeeParameters()->parameters()[Trk::z0];
    //double chi2_id  = track->fitQuality()->chiSquared();

    double dphi_id =  0.1;
    double deta_id = 0.1;

    ATH_MSG_DEBUG( "Found track: "
                           << "  with pt= " << pt_id
                           << ", eta=" << eta_id
                           << ", phi=" << phi_id
		   //<< ", Zid=" << zPos_id
		   //      << ", Chi2=" << chi2_id
                           );

    // monitoring
    m_ptTrkIn.push_back( std::abs(pt_id) );
    m_etaTrkIn.push_back( eta_id );
    m_phiTrkIn.push_back( phi_id );
    
    ++nFound;

    TrigRoiDescriptor*  newRoi = createSingleTrigRoiDescriptor( eta_id, phi_id, deta_id, dphi_id, superRoI->roiId() + nFound );
    superRoI->push_back( newRoi );
    ATH_MSG_DEBUG("Added RoI from selected track: " << *newRoi );

    // difference w.r.t already found tracks
    int n = m_etaRoiOut.size();
    for ( int i = 0; i < n; ++i ) {
      double dEta = eta_id - m_etaRoiOut[i];
      double dPhi = phi_id - m_phiRoiOut[i];
      // put phi in range -PI,PI
      while ( dPhi  >  M_PI  ) dPhi -= 2*M_PI;
      while ( dPhi <= -M_PI  ) dPhi += 2*M_PI;
      double dR = std::sqrt( dEta*dEta + dPhi*dPhi );
      m_dEtaTrkSel.push_back( dEta );
      m_dPhiTrkSel.push_back( dPhi );
      m_dRTrkSel.push_back( dR );
      if ( dR > 0.0 ) m_logdRTrkSel.push_back( std::log10(dR) );
    }
    
    // monitoring
    m_ptRoiOut.push_back( std::abs(pt_id) );
    m_etaRoiOut.push_back( eta_id );
    m_phiRoiOut.push_back( phi_id );
    
   
  }
  nIDtracks =  tracks->size();
  m_nTrksIn = nIDtracks;

  if ( m_nTrksIn > 0 ) {
    m_nRoiToTrkRatio = (double)m_nRoiOut/(double)m_nTrksIn;
  } else {
    m_nRoiToTrkRatio = 0.0;
  }
  
  ATH_MSG_DEBUG("Made " << nFound << " ROIs from " << tracks->size() << " tracks.");
  attachFeature(outputTE,superRoI,"forMSFS");// forMS");
  outputTE->setActiveState(true);
  // fill monitor variables "by hand" because this is an allTE algo
  afterExecMonitors().ignore();
  return HLT::OK;


  
  return HLT::OK;
  
}//hltExecute


TrigRoiDescriptor* TrigBphysTrackRoiMaker::createSingleTrigRoiDescriptor( double eta, double phi, double etaWidth, double phiWidth, int roiNum ) {
  double minphi = phi - std::abs(phiWidth);
  double maxphi = phi + std::abs(phiWidth);
  if(minphi <  -M_PI) minphi += 2.0*M_PI;
  if(minphi >=  M_PI) minphi -= 2.0*M_PI;
  if(maxphi <= -M_PI) maxphi += 2.0*M_PI;
  if(maxphi >   M_PI) maxphi -= 2.0*M_PI;
  double minEta = eta - std::abs(etaWidth);
    double maxEta = eta + std::abs(etaWidth);

  /// Oh no!! will setting the roiword and the l1id to 0 have any serious consequences?
  /// do we really need these identifiers? Does anything actually ever use them for
  /// anything except printout? I guess not, since by default they are set to 0 anyhow.
  TrigRoiDescriptor* trigRoI = new TrigRoiDescriptor( unsigned(0), unsigned(roiNum), eta, minEta, maxEta, phi, minphi, maxphi);
  //  trigRoI->set_roiId(roiNum);

  ATH_MSG_DEBUG( "createSingleTrigRoiDescriptor: Creating Roi with center " << " (eta,phi) = (" << eta << "," << phi <<
                   ") and size (dEta,dPhi) = (" << etaWidth << "," << phiWidth << ")\n" <<
                   "  createSingleTrigRoiDescriptor: Roi num = " << roiNum << " etaRange = " << minEta << " -> " << maxEta <<
                   " phiRange = " << minphi << " -> " << maxphi );

  return trigRoI;
}
