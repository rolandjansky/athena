/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <cmath>

#include "InDetTrkRoiMaker.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace {
  inline std::string quote( const std::string& s ) { return "\"" + s + "\""; }
}

// static functions
InDetTrkRoiMaker::EFSMode InDetTrkRoiMaker::modeNameToEnum( const std::string& modeName ) {
  unsigned int nModes = (unsigned int)kNumberOfModes;
  for ( unsigned int iMode = 0; iMode < nModes; ++iMode ) {
    if ( modeName == s_scanModeNames[iMode] ) return EFSMode(iMode);
  }
  // not found
  return kNumberOfModes; // to signal failure
}

TrigInDetTrack::AlgoId InDetTrkRoiMaker::seedsNameToEnum( const std::string& seedsName ) {
  TrigInDetTrack::AlgoId algoId(TrigInDetTrack::STRATEGY_F_ID);

  if      (seedsName=="STRATEGY_F") algoId = TrigInDetTrack::STRATEGY_F_ID;
  else if (seedsName=="IDSCAN")     algoId = TrigInDetTrack::IDSCANID;
  else if (seedsName=="SITRACK")    algoId = TrigInDetTrack::SITRACKID;
  else if (seedsName=="STRATEGY_A") algoId = TrigInDetTrack::STRATEGY_A_ID;
  else if (seedsName=="STRATEGY_B") algoId = TrigInDetTrack::STRATEGY_B_ID;
  else if (seedsName=="STRATEGY_C") algoId = TrigInDetTrack::STRATEGY_C_ID;
  else if (seedsName=="TRTXK")      algoId = TrigInDetTrack::TRTXKID;
  else if (seedsName=="TRTSEG")     algoId = TrigInDetTrack::TRTLUTID;
  else if (seedsName=="FTF")        algoId = TrigInDetTrack::FTF;
  return algoId;
}

std::string InDetTrkRoiMaker::getScanModeHelp() {
  std::string scanModeHelp("Possible values of full-scan mode:\n");
  unsigned int nModes = (unsigned int)kNumberOfModes;
  for ( unsigned int iMode = 0; iMode < nModes; ++iMode ) {
    scanModeHelp += quote(s_scanModeNames[kFullScan]);
    scanModeHelp += ": Use full detector\n";
    scanModeHelp += quote(s_scanModeNames[kSeededScan]);
    scanModeHelp += ": Use tracks as seeds to use multiple parts of detector";
    scanModeHelp += quote(s_scanModeNames[kHybridScan]);
    scanModeHelp += ": Use SeededScan mode for |eta| <= SeedsEtaMax and FullScan mode for |eta| > SeedsEtaMax";
  }
  return scanModeHelp;
}


/**
 * Constructor - set up the algorithm
 */
InDetTrkRoiMaker::InDetTrkRoiMaker(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator)
{
  // ID algorithm to use as input
  declareProperty("FullScanMode", m_fullScanModeProperty="HybridScan", getScanModeHelp());
  // what to consider full detector eta range for end-caps in HybridScan mode
  declareProperty("FullEtaRange", m_fullEtaRange = 5.0 );

  declareProperty("SeedsIDalgo", m_ID_algo_to_use="STRATEGY_F");
  // main track cuts
  declareProperty("SeedsEtaMax", m_EtaMaxTrk = 2.5, "Only use track seeds with |eta| <= SeedsEtaMax. Only used in modes \"SeededScan\" and \"HybridScan\"");
  declareProperty("SeedsPtMin",  m_PtMinTrk = 5.0, "Only use track seeds with pT >= SeedsPtMin (GeV/c). Only used in modes \"SeededScan\" and \"HybridScan\"");
  //ID tracks quality parameters
  declareProperty("SeedsMaxAbsZ", m_ZMaxTrk=999999999.);
  declareProperty("SeedsMaxChi2",    m_Chi2MaxTrk=999999999.);
  declareProperty("SeedsMinNPixHit", m_NPIXhitMinTrk=0);
  declareProperty("SeedsMinNSCTHit", m_NSCThitMinTrk=0);

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

  // set consistent values of data members
  m_seedsAlgoId = seedsNameToEnum(m_ID_algo_to_use);
  m_fullScanMode = modeNameToEnum(m_fullScanModeProperty);
  
}

/**
 * Destructor - clean up when done
 */
InDetTrkRoiMaker::~InDetTrkRoiMaker() {

}

/**
 * Initialize the algorithm
 */
HLT::ErrorCode InDetTrkRoiMaker::hltInitialize() {

  m_seedsAlgoId  = seedsNameToEnum(m_ID_algo_to_use);  
  m_fullScanMode = modeNameToEnum(m_fullScanModeProperty);
  if ( m_fullScanMode == kNumberOfModes ) {
    ATH_MSG_FATAL("FullScanMode = \"" << m_fullScanModeProperty << "\" not supported. "
                  << getScanModeHelp() );
    return HLT::BAD_JOB_SETUP;
  }
  
  return HLT::OK;
}//hltInitialize

/**
 * Finalize the algorithm
 */
HLT::ErrorCode InDetTrkRoiMaker::hltFinalize() {

  return HLT::OK;
}//hltFinalize

HLT::ErrorCode InDetTrkRoiMaker::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {  
  // clear monitor variables
  beforeExecMonitors().ignore();
  outputTE->setActiveState(true);

  TrigRoiDescriptor* superRoI = new TrigRoiDescriptor(true); // create fullscan RoI
  //  superRoI->set_roiId(10000); /// do we need this ??

  if ( m_fullScanMode == kFullScan ) {
    ///    now use a standard TrigRoiDescriptor, these are not 
    ///    composite by default
        superRoI->setComposite(false); // so it will be treated as a full-scan

  } else if ( m_fullScanMode == kSeededScan || m_fullScanMode == kHybridScan ) {
        superRoI->setComposite(false);
    //    superRoI->manageConstituents(true);



	//    std::vector <const TrigInDetTrackCollection*> vectorOfTrackCollections;
	std::vector<const TrackCollection*> vectorOfTrackCollections;
	if( HLT::OK != getFeatures(inputTE,vectorOfTrackCollections, "") ) {
	  ATH_MSG_WARNING(" Failed to get InDetTrackCollections --> no match");
	  return HLT::MISSING_FEATURE;
	}
  
	ATH_MSG_DEBUG(" Got InDetTrackCollections "<<vectorOfTrackCollections.size());

	std::vector<const TrackCollection*>::iterator
	  theTrackColl = vectorOfTrackCollections.begin(),
	  endTrackColl = vectorOfTrackCollections.end();
    
	// Loop on container & create ROIs.
	int nFound(0);
	int nIDtracks(0);
	for ( ; theTrackColl != endTrackColl;  ++theTrackColl ) {//Tracks Collections Loop
      
	  TrackCollection::const_iterator
	    track     = (*theTrackColl)->begin(),
	    lasttrack = (*theTrackColl)->end();
      
	  for ( ; track != lasttrack; ++track ) {//Tracks Loop
	    //Select tracks
	    ++nIDtracks;
	    
	    if (!((*track)->perigeeParameters())) continue;   //Must have helix parameters
	    double pt_id    = (*track)->perigeeParameters()->momentum().perp()*0.001; //in GeV/c
	    double phi_id   = (*track)->perigeeParameters()->parameters()[Trk::phi0];
	    double theta    = (*track)->perigeeParameters()->parameters()[Trk::theta] ;
	    double eta_id   = -log(fabs(tan(theta*0.5)));
	    
	    // FIXME: MS what are the eta and phi resolutions? are these set correctly?
	    //        Use these for the roi sizes
	    // double deta_id = (*track)->perigeeParameters()->parameters()[Trk::eeta];
          
	    double zPos_id  = (*track)->perigeeParameters()->parameters()[Trk::z0];
	    double chi2_id  = (*track)->fitQuality()->chiSquared();
	    // int    npixh_id = (*track)->NPixelSpacePoints();
	    // int    nscth_id = (*track)->NSCT_SpacePoints();
	    //	    Eigen::Matrix<double, 5, 5, 0, 5, 5> covMat = *( (*track)->perigeeParameters()->covariance());
	    double dphi_id =  0.1;//Amg::error(covMat,Trk::phi0);
	    //	    double dtheta_id = Amg::error(covMat,Trk::theta);
	    double deta_id = 0.1;
	    //	    if(fabs(sin(theta))>0) deta_id = dtheta_id/sin(theta);
	    //	    ATH_MSG_DEBUG("dphi: "<<dphi_id<<" deta: "<<deta_id);
	    ATH_MSG_DEBUG( "Found track: "
			   << m_ID_algo_to_use
			   << "  with pt= " << pt_id
			   << ", eta=" << eta_id
			   << ", phi=" << phi_id
			   << ", Zid=" << zPos_id
			   << ", Chi2=" << chi2_id
			   // << ", NPix=" << npixh_id
			   // << ", NSCT=" << nscth_id
			   );
	    
	    // monitoring
	    m_ptTrkIn.push_back( std::abs(pt_id) );
	    m_etaTrkIn.push_back( eta_id );
	    m_phiTrkIn.push_back( phi_id );
	    
	    // track selection
	    if (std::abs(pt_id)   < m_PtMinTrk)       continue;
	    if (std::abs(eta_id)  > m_EtaMaxTrk)      continue;
	    if (std::abs(zPos_id) > m_ZMaxTrk)        continue;
	    if (chi2_id           > m_Chi2MaxTrk)     continue;
	    // if (npixh_id          < m_NPIXhitMinTrk)  continue;
	    // if (nscth_id          < m_NSCThitMinTrk)  continue;
	    
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
	    
        
	  }//Tracks loop
	}

    m_nTrksIn = nIDtracks;
    m_nRoiOut = nFound;
    if ( m_nTrksIn > 0 ) {
      m_nRoiToTrkRatio = (double)m_nRoiOut/(double)m_nTrksIn;
    } else {
      m_nRoiToTrkRatio = 0.0;
    }
  
    ATH_MSG_DEBUG("Made " << nFound << " ROIs from " << nIDtracks << m_ID_algo_to_use << " tracks.");

    if ( m_fullScanMode == kHybridScan ) {
      // add 2 eta regions outside of seeds range
      if ( m_EtaMaxTrk < m_fullEtaRange - 0.00001  ) {
        double etaCentral = 0.5 * std::abs(m_EtaMaxTrk + m_fullEtaRange);
        TrigRoiDescriptor* endcapPositive = new TrigRoiDescriptor( etaCentral, m_EtaMaxTrk, m_fullEtaRange, /* eta positive end-cap */
								   0., -M_PI+1e-6, M_PI-1e-6 /* full phi range */ ,0.,-255.,255.);
        TrigRoiDescriptor* endcapNegative = new TrigRoiDescriptor( -etaCentral, -m_fullEtaRange, -m_EtaMaxTrk, /* eta negative end-cap */
                                                                   0., -M_PI+1e-6, M_PI-1e-6 /* full phi range */,0.,-255.,255. );

	superRoI->push_back( endcapPositive );
	superRoI->push_back( endcapNegative );

	ATH_MSG_DEBUG("Added RoI from positive end-cap: " << *endcapPositive );
	ATH_MSG_DEBUG("Added RoI from negative end-cap: " << *endcapNegative );
      }
    } // if m_fullScanMode == kHybridScan
    
  } // if m_fullScanMode == kSeededScan || kHybridScan
  
  // add just one new outputTE with one TrigSuperRoI
  // outputTE = addRoI(0, superRoI);
  //  for(auto roi : superRoI)
  attachFeature(outputTE,superRoI,"forMSFS");
  outputTE->setActiveState(true);
  // fill monitor variables "by hand" because this is an allTE algo
  afterExecMonitors().ignore();
  return HLT::OK;  
}//hltExecute



TrigRoiDescriptor* InDetTrkRoiMaker::createSingleTrigRoiDescriptor( double eta, double phi, double etaWidth, double phiWidth, int roiNum ) {
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

  ATH_MSG_VERBOSE( "createSingleTrigRoiDescriptor: Creating Roi with center " << " (eta,phi) = (" << eta << "," << phi <<
                   ") and size (dEta,dPhi) = (" << etaWidth << "," << phiWidth << ")\n" <<
                   "  createSingleTrigRoiDescriptor: Roi num = " << roiNum << " etaRange = " << minEta << " -> " << maxEta <<
                   " phiRange = " << minphi << " -> " << maxphi );
 
  return trigRoI;
}

