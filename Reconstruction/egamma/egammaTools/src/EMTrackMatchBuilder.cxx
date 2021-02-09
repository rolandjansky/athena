/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:

#include "EMTrackMatchBuilder.h"

#include "egammaRecEvent/egammaRec.h"

#include "CaloUtils/CaloCellList.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODEgamma/EgammaxAODHelpers.h"

//extrapolation
#include "egammaInterfaces/IEMExtrapolationTools.h"
//
#include "CandidateMatchHelpers.h"
#include "FourMomUtils/P4Helpers.h"
#include "SGTools/CurrentEventStore.h"

#include <cmath>

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:
   
EMTrackMatchBuilder::EMTrackMatchBuilder(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent) : 
  egammaBaseTool(type, name, parent),
  m_extrapolationTool("EMExtrapolationTools"),
  m_isCosmics(false)
{

  // declare interface
  declareInterface<IEMTrackMatchBuilder>(this);

  // The following properties are specified at run-time
  // (declared in jobOptions file)

  // Name of the input track particle container
  declareProperty("TrackParticlesName", m_TrackParticlesName,
                  "Name of the input track particle container");
  
  // Name of the extrapolation tool
  declareProperty("ExtrapolationTool",
                  m_extrapolationTool,
                  "Name of the extrapolation tool");
  
  // Value of broad cut for delta eta
  declareProperty("broadDeltaEta",m_broadDeltaEta = 0.1,
                  "Value of broad cut for delta eta");

  // Value of broad cut for delta phi
  declareProperty("broadDeltaPhi",m_broadDeltaPhi = 0.1,
                  "Value of broad cut for delta phi");

  //Use rescale in the metric
  declareProperty("UseRescaleMetric", m_UseRescaleMetric =true,
		  "Use Rescale Metric");

  // Do second pass with rescale
  declareProperty("SecondPassRescale", m_SecondPassRescale =true,
		  "Do second pass with rescale");  

  // Value of narrow cuts
  declareProperty("narrowDeltaEta", m_narrowDeltaEta = 0.05,
		  "Value of narrow cut for delta eta");

  declareProperty("narrowDeltaPhi", m_narrowDeltaPhi =0.05,
		  "Value of the narrowd cut for delta phi ");

  declareProperty("narrowDeltaPhiBrem", m_narrowDeltaPhiBrem =0.1,
		  "Value of the narrow cut for delta phi  Brem");

  declareProperty("narrowDeltaPhiRescale",  m_narrowDeltaPhiRescale =0.05,
		  "Value of the narrow cut for delta phi Rescale");

  declareProperty("narrowDeltaPhiRescaleBrem", m_narrowDeltaPhiRescaleBrem =0.1,
		  "Value of the narrow cut for delta phi Rescale Brem");

  declareProperty("MaxDeltaPhiRescale", m_MaxDeltaPhiRescale=0.25,
		  "Maximum Value of the deltaPhi rescale");

  // Boolean to use candidate matching
  declareProperty("useCandidateMatch",
                  m_useCandidateMatch = true,
                  "Boolean to use candidate matching");

  //Boolean to favor tracks with Pixel hits
  declareProperty("useScoring",
                  m_useScoring= true,
                  "Boolean to favor tracks with Pixel hits");

  // Boolean for use of cosmics
  declareProperty("isCosmics",
                  m_isCosmics=false,
                  "Boolean for use of cosmics");

  // Boolean to use last measurement for track extrapolation
  // False means use perigee
  declareProperty("useLastMeasurement",
                  m_useLastMeasurement = false,
                  "Boolean to use last measurement for extrapolation, otherwise use perigee");

}

// ==============================================================
EMTrackMatchBuilder::~EMTrackMatchBuilder() 
{ 
  //
  // destructor
  //
}

// ==============================================================
StatusCode EMTrackMatchBuilder::initialize()
{
  //
  // initialize
  //

  ATH_MSG_DEBUG("Initializing EMTrackMatchBuilder");


  // the extrapolation tool
  if(m_extrapolationTool.retrieve().isFailure()){
    ATH_MSG_ERROR("initialize: Cannot retrieve extrapolationTool " << m_extrapolationTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("initialize: Retrieved extrapolationTool " << m_extrapolationTool);
  }


  // Boolean to use candidate matching
  if (m_useCandidateMatch) { 
    ATH_MSG_INFO("initialize: useCandidateMatch is true");
  }
  else ATH_MSG_INFO("initialize: useCandidateMatch is turned off");

  return StatusCode::SUCCESS;
}

// ==============================================================
StatusCode EMTrackMatchBuilder::executeRec(egammaRec* eg)
{
  //
  // standard egamma execute method
  // egamma* is an input
  //

  ATH_MSG_DEBUG("Executing EMTrackMatchBuilder");
 
  // protection against bad pointers
  if (eg==0) return StatusCode::SUCCESS;

  // retrieve the cluster
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if (cluster == 0) return StatusCode::SUCCESS;

  // check for 0 energy clusters
  if (cluster->e() == 0.0) {
    ATH_MSG_WARNING("execute: cluster energy is 0.0! Ignoring cluster.");
    return StatusCode::SUCCESS;
  }
        
  // retrieve the trackparticle container
  const xAOD::TrackParticleContainer* trackPC = evtStore()->retrieve< const xAOD::TrackParticleContainer >(m_TrackParticlesName );
  if( ! trackPC ) {
    REPORT_MESSAGE( MSG::WARNING )
      << "Couldn't retrieve TrackParticle container with key: "
      << m_TrackParticlesName;
    return StatusCode::SUCCESS;
  }
  // Check that the auxiliary store association was made successfully:
  if( ! trackPC->hasStore() ) {
    REPORT_MESSAGE( MSG::WARNING )
      << "No auxiliary store got associated to the TrackParticle container "
      << "with key: " << m_TrackParticlesName;
    return StatusCode::SUCCESS;
  }
   
  // call the execute method
  CHECK( trackExecute(eg, trackPC) );
  return StatusCode::SUCCESS;
}

// ===============================================================
StatusCode EMTrackMatchBuilder::trackExecute(egammaRec* eg, const xAOD::TrackParticleContainer*    trackPC)
{
  if (!eg || !trackPC)
  {
    ATH_MSG_WARNING("trackExecute: NULL pointer to egammaRec or TrackParticleContainer");
    return StatusCode::SUCCESS;
  }
  
  // retrieve corresponding cluster
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  
  // check for 0 energy clusters
  if (cluster && cluster->e() == 0.0) {
    ATH_MSG_WARNING("trackExecute: cluster energy is 0.0! Ignoring cluster.");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("in trackExecute");
  
  // Loop over tracks and fill TrackMatch vector
  std::vector<TrackMatch> trkMatches; 
  xAOD::TrackParticleContainer::const_iterator trkIt = trackPC->begin();
  for (unsigned int trackNumber = 0; trkIt != trackPC->end(); ++trkIt, ++trackNumber) 
  {
    bool isTRT = (xAOD::EgammaHelpers::numberOfSiHits(*trkIt) < 4);
    
    if (!(isCandidateMatch(cluster, isTRT, (*trkIt), false ) &&
        inBroadWindow(trkMatches, cluster, trackNumber, isTRT, (*trkIt), Trk::alongMomentum)) && m_isCosmics)
    {
      // Second chance for cosmics, flip eta and phi
      if (isCandidateMatch(cluster, isTRT, (*trkIt), true))
        inBroadWindow(trkMatches, cluster,  trackNumber, isTRT,  (*trkIt), Trk::oppositeMomentum);
    }
  }

  if(trkMatches.size()>0)
  {
    //sort the track matches
    std::sort(trkMatches.begin(), trkMatches.end(), TrackMatchSorter());
  
  
    //set the matching values
    TrackMatch bestTrkMatch=trkMatches.at(0);
    for(int i=0; i<4 ;++i){
      eg->setDeltaEta (i, bestTrkMatch.deltaEta[i]); 
      eg->setDeltaPhi (i, bestTrkMatch.deltaPhi[i]); 
      eg->setDeltaPhiRescaled (i, bestTrkMatch.deltaPhiRescaled[i]); 
    }
    eg->setDeltaPhiLast(bestTrkMatch.deltaPhiLast);
    
    //set the element Links
    ATH_MSG_DEBUG("============================================");
    typedef ElementLink<xAOD::TrackParticleContainer> EL;
    std::vector<EL> trackParticleLinks;
    trackParticleLinks.reserve (trkMatches.size());
    std::string key = EL(*trackPC, 0).dataID();
    IProxyDict* sg = SG::CurrentEventStore::store();
    for (const TrackMatch& m : trkMatches) {
      ATH_MSG_DEBUG("Match  dR: "<< m.dR
		    <<" second  dR: "<< m.seconddR
		    <<" score: "<< m.score
		    <<" hitsScore: " << m.hitsScore 
		    <<" isTRT : "<< m.isTRT);
      if (key.empty())
        trackParticleLinks.emplace_back (*trackPC, m.trackNumber, sg);
      else
        trackParticleLinks.emplace_back (key, m.trackNumber, sg);
    }
    eg->setTrackParticles(trackParticleLinks);
  }
  ATH_MSG_DEBUG("================================================");  

  return StatusCode::SUCCESS;
}

// =================================================================
bool
EMTrackMatchBuilder::inBroadWindow(std::vector<TrackMatch>& trackMatches,
                                   const xAOD::CaloCluster*   cluster, 
                                   int                  trackNumber,
                                   bool                 trkTRT,
                                   const xAOD::TrackParticle* trkPB,
                                   const Trk::PropDirection dir) const
{
  // make position match between cluster and TrackParameters at LAr calorimeter 
  // 
  
  // protection against bad pointers
  if (cluster==0 || trkPB==0) return false;

  IEMExtrapolationTools::TrkExtrapDef extrapFrom;
  if (m_useLastMeasurement) {
    extrapFrom   = IEMExtrapolationTools::fromLastMeasurement;
  }
  else {
    extrapFrom   = IEMExtrapolationTools::fromPerigee;
  }
  
  ATH_MSG_DEBUG("inBroadWindow: extrapolation method From (0 Last, 1 perigee , 2 Rescale) " << extrapFrom);
  
  // Now get the delta eta/phi and eta correction at the calorimeter
  std::vector<double>  eta(4, -999.0);
  std::vector<double>  phi(4, -999.0);
  // final arrays that we will write
  // Save the value of deltaPhiRescale. If we do not use rescaled
  // perigee, we recalculate deltaPhi using rescaled momentum. This
  // will be saved in EMTrackMatch
  double deltaPhiRescale  = -999.;
  double deltaPhiLast     = -999.;
  // temporary arrays
  std::vector<double>  deltaEta(4, -999.0);
  std::vector<double>  deltaPhi(4, -999.0);    
  std::vector<double>  deltaEtaRes(4, -999.0);
  std::vector<double>  deltaPhiRes(4, -999.0); 
  ATH_MSG_DEBUG("### Matching #### ");  
  if(trkTRT){//TRTSA
    if(!m_extrapolationTool->matchesAtCalo (cluster, 
					    trkPB, 
					    trkTRT,
					    dir, 
					    eta,
					    phi,
					    deltaEta, 
					    deltaPhi, 
					    extrapFrom)) {
      ATH_MSG_DEBUG("Normal matched Failed deltaPhi/deltaEta " << deltaPhi[2] <<" / "<< deltaEta[2]<<", No Rescale attempted, isTRTSA = " <<trkTRT);
      return false;
    }
  } //end if is TRTSA
  else{//Silicon tracks
    if (m_extrapolationTool->getMatchAtCalo (cluster, 
					     trkPB, 
					     trkTRT,
					     dir, 
					     eta,
					     phi,
					     deltaEta, 
					     deltaPhi, 
					     extrapFrom).isFailure()) 
		{
		  if (m_useLastMeasurement) ATH_MSG_DEBUG("Extrapolation from last measurement failed");
		  else ATH_MSG_DEBUG("Extrapolation from perigee failed");
		  return false;
		}
    //=======================================================================================///
    //Calculate both always std and rescale
    IEMExtrapolationTools::TrkExtrapDef extrapFrom1 = IEMExtrapolationTools::fromPerigeeRescaled;
    std::vector<double>  eta1(4, -999.0);
    std::vector<double>  phi1(4, -999.0);
    if (m_extrapolationTool->getMatchAtCalo (cluster, 
					     trkPB, 
					     trkTRT,
					     dir, 
					     eta1,
					     phi1,
					     deltaEtaRes, 
					     deltaPhiRes, 
					     extrapFrom1).isFailure())
		{
		  ATH_MSG_DEBUG("Extrapolation from rescaled perigee failed");
		  return false;
		}
    
    ATH_MSG_DEBUG("Done with extrapolations");
    
    deltaPhiRescale = deltaPhiRes[2];
    //Try to match : First std , then rescale, else failure 
    if(fabs(deltaEta[2]) < m_narrowDeltaEta &&deltaPhi[2] < m_narrowDeltaPhi && deltaPhi[2] > -m_narrowDeltaPhiBrem){
      ATH_MSG_DEBUG("Matched with Perigee") ;
    }
    else if(m_SecondPassRescale && fabs(deltaEtaRes[2]) < m_narrowDeltaEta &&deltaPhiRes[2] < m_narrowDeltaPhiRescale && deltaPhiRes[2] > -m_narrowDeltaPhiRescaleBrem){
      ATH_MSG_DEBUG("Not Perigee but matched with Rescale") ;
    }
    else{
      ATH_MSG_DEBUG("Normal matched Failed deltaPhi/deltaEta " << deltaPhi[2] <<" / "<< deltaEta[2]<<", No Rescale attempted, isTRTSA = " <<trkTRT);
      ATH_MSG_DEBUG("Rescaled matched Failed deltaPhi/deltaEta " << deltaPhiRes[2] <<" / "<< deltaEtaRes[2] );
      return false;
    }
    if(fabs(deltaPhiRes[2]) > m_MaxDeltaPhiRescale)
    {
      ATH_MSG_DEBUG("DeltaPhiRescaled above maximum: " << deltaPhiRes[2] << 
                    " (max: " << m_MaxDeltaPhiRescale << ")" );
      return false;
    }
  } //end if is silicon
  
  //In case of extrapolation from perigee keep the dPhi from the last measurement
  if (!m_useLastMeasurement ) {  
    IEMExtrapolationTools::TrkExtrapDef extrapFrom1 = IEMExtrapolationTools::fromLastMeasurement;
    std::vector<double>  eta1(4, -999.0);
    std::vector<double>  phi1(4, -999.0);
    std::vector<double>  deltaEta1(4, -999.0);
    std::vector<double>  deltaPhi1(4, -999.0);
    if (m_extrapolationTool->getMatchAtCalo (cluster, 
                                             trkPB, 
					     trkTRT,
					     dir, 
                                             eta1,
                                             phi1,
                                             deltaEta1, 
                                             deltaPhi1, 
                                             extrapFrom1).isFailure())
    {
      ATH_MSG_DEBUG("Extrapolation from last measurement failed");
      return false;
    }
    
    //Always the deltaPhiLast will be from the last measurement
    deltaPhiLast = deltaPhi1[2];
  }
  else{
    //No need to overwrite deltaPhi2 , the deltaPhi2 should be always what used for the match!!!!
    deltaPhiLast = deltaPhi[2];
  }
  ATH_MSG_DEBUG("Rescale dPhi " << deltaPhiRescale);
  ATH_MSG_DEBUG("dPhi Last measurement " << deltaPhiLast);     
  
  //Create Track Match struct here
  TrackMatch trkmatch;
  
  //Add the matching variable to the TrackMAtch
  for(int i=0; i<4 ;++i){
    trkmatch.deltaEta[i]=deltaEta.at(i); 
    trkmatch.deltaPhi[i]=deltaPhi.at(i); 
    trkmatch.deltaPhiRescaled[i]=deltaPhiRes.at(i); 
  }  
  trkmatch.deltaPhiLast=deltaPhiLast; 
  
  //Variables used for the sorting. Note both dPhi will be used.
  trkmatch.isTRT=trkTRT;
  trkmatch.trackNumber=trackNumber;
  if (!trkTRT) {
    if(m_UseRescaleMetric){
      trkmatch.dR = sqrt(deltaEta[2]*deltaEta[2] + deltaPhiRescale*deltaPhiRescale);
      trkmatch.seconddR = sqrt(deltaEta[2]*deltaEta[2] + deltaPhi[2]*deltaPhi[2]);
    }
    else{
      trkmatch.dR = sqrt(deltaEta[2]*deltaEta[2] + deltaPhi[2]*deltaPhi[2]);
      trkmatch.seconddR = sqrt(deltaEta[2]*deltaEta[2] + deltaPhiRescale*deltaPhiRescale);
    }
    ATH_MSG_DEBUG("TRTSA = " << trkTRT << " DR " << trkmatch.dR <<" deltaPhi " << deltaPhi[2] <<" deltaEta "<< deltaEta[2]);   
  }
  else if (trkTRT){
    trkmatch.dR = fabs(deltaPhi[2]);
    trkmatch.seconddR = -999;
    ATH_MSG_DEBUG("TRTSA = " << trkTRT << " DPhi " << trkmatch.dR <<" deltaPhi " << deltaPhi[2]);   
  }

  //Primary Score. The first thing to check in finding the best track match
  trkmatch.score=0; 
  /*Seconday score based on hits to be used for track that are very close 
    to each other at the calo, pick the innermost possible one*/
  trkmatch.hitsScore=0;

  if(!trkTRT){  //10 primary points if it is not TRT ==> TRT-only are the weakest candidates ==> score =0

    trkmatch.score+=10 ;   
    if(m_useScoring){

      int nPixel=0;
      uint8_t uint8_value=0;

      //Check number of pixel hits
      if (trkPB->summaryValue(uint8_value,  xAOD::numberOfPixelDeadSensors)){
	nPixel+=uint8_value;
      }
      if (trkPB->summaryValue(uint8_value,  xAOD::numberOfPixelHits)){
	nPixel+=uint8_value;
      }

      //20 primary score points if it also have pixel hits
      if (nPixel > 0)  {
	trkmatch.score+=20;
      }

      //Check the 2 innermost layers
      int nInnerMost =0;
      if (trkPB->summaryValue(uint8_value,  xAOD::numberOfInnermostPixelLayerHits)){
	nInnerMost+=uint8_value;
      }
      int expectInnermostPixelLayerHit = 0; 
      if (trkPB->summaryValue(uint8_value,  xAOD::expectInnermostPixelLayerHit)){
	expectInnermostPixelLayerHit+=uint8_value;
      }
      int nNextToInnerMost =0;
      if (trkPB->summaryValue(uint8_value,  xAOD::numberOfNextToInnermostPixelLayerHits)){
	nNextToInnerMost+=uint8_value;
      }
      int expectNextToInnermostPixelLayerHit = 0; 
      if (trkPB->summaryValue(uint8_value,  xAOD::expectNextToInnermostPixelLayerHit)){
	expectNextToInnermostPixelLayerHit+=uint8_value;
      }
      
      //Secondary score , find the longest track possible, 
      //i.e the one with the most inner hists  in the pixel 
      //npixel*5 
      trkmatch.hitsScore+=(nPixel*5);
      //Extra points for NextToInnermost
      if(!expectNextToInnermostPixelLayerHit ||  nNextToInnerMost>0){
	trkmatch.hitsScore+=5;
      }
      //Extra points for Innermost
      if(!expectInnermostPixelLayerHit ||  nInnerMost>0){
	trkmatch.hitsScore+=10;
      }

      ATH_MSG_DEBUG("Pixel hits : " <<nPixel 
		    <<" InnerMost : " << nInnerMost
		    <<" Expected InnerMost : " << expectInnermostPixelLayerHit
		    <<" NextToInnerMost : " << nNextToInnerMost
		    <<" Expected NextToInnerMost : " << expectNextToInnermostPixelLayerHit);   
    
    }  
  }
  ATH_MSG_DEBUG("Score : " <<trkmatch.score <<" hitsScore : " <<trkmatch.hitsScore);   

  trackMatches.push_back(trkmatch);
  return true;
}

// =================================================================
bool
EMTrackMatchBuilder::isCandidateMatch(const xAOD::CaloCluster*        cluster,
                                      bool                      trkTRT,
                                      const xAOD::TrackParticle* track,
                                      bool                      flip) const
{
  // loose cluster-track matching
  if ( m_useCandidateMatch ) {
    ATH_MSG_DEBUG("EMTrackMatch builder , broad window");
    if (cluster == 0 || track == 0) return false;
    //////
    const Trk::Perigee candidatePerigee =track->perigeeParameters();
    double trkPhi = candidatePerigee.parameters()[Trk::phi];
    double trkEta = candidatePerigee.eta();
    //Decide whether to try the opposite direction (cosmics)
    if(flip) {trkPhi = -trkPhi; trkEta = -trkEta;}
    double r_first=candidatePerigee.position().perp();
    const Amg::Vector3D PerigeeXYZPosition = candidatePerigee.position();

    //===========================================================//     
    double clusterEta=cluster->etaBE(2);
    bool isEndCap= cluster->inEndcap();
    double Et= cluster->e()/cosh(trkEta);
    if(trkTRT){
      Et = cluster->et();
    }
    //Avoid clusters with |eta| > 10 or Et less than 10 MeV
    if(fabs(clusterEta)>10.0 || Et <10){
      return false;
    }
 
    //Calculate the eta/phi of the cluster as would be seen from the perigee position of the Track
    const Amg::Vector3D XYZClusterWrtTrackPerigee = CandidateMatchHelpers::approxXYZwrtPoint(*cluster,
                                                                                           PerigeeXYZPosition,
                                                                                           isEndCap);
     
    
    const double clusterEtaCorrected=XYZClusterWrtTrackPerigee.eta(); 
    //check eta match . Both metrics need to fail in order to disgard the track
    if ( (!trkTRT) && (fabs(clusterEta - trkEta) > 2.*m_broadDeltaEta) && 
       (fabs(clusterEtaCorrected- trkEta) > 2.*m_broadDeltaEta)){
      ATH_MSG_DEBUG(" Fails broad window eta match (track eta, cluster eta, cluster eta corrected): ( " 
                  << trkEta << ", " << clusterEta <<", "<<clusterEtaCorrected<<")" );
      return false;
    }
    


    
    //===========================================================//     
    double phiRotRescaled = CandidateMatchHelpers::PhiROT(Et,trkEta, track->charge(),r_first ,isEndCap)  ;
    double phiRotTrack = CandidateMatchHelpers::PhiROT(track->pt(),trkEta, track->charge(),r_first ,isEndCap)  ;
    
    const double clusterPhiCorrected=XYZClusterWrtTrackPerigee.phi();   

    //===========================================================//         
    //deltaPhi between the track and the cluster 
    const double deltaPhiStd = P4Helpers::deltaPhi(clusterPhiCorrected, trkPhi);
    //deltaPhi between the track and the cluster accounting for rotation assuming cluster Et is a better estimator
    const double trkPhiRescaled= P4Helpers::deltaPhi(trkPhi, phiRotRescaled);
    const double deltaPhiRescaled = P4Helpers::deltaPhi(clusterPhiCorrected, trkPhiRescaled);
    //deltaPhi between the track and the cluster accounting for rotation
    const double trkPhiCorrTrack = P4Helpers::deltaPhi(trkPhi, phiRotTrack);
    const double deltaPhiTrack = P4Helpers::deltaPhi(clusterPhiCorrected, trkPhiCorrTrack);
 
    //It has to fail all phi metrics in order to be disgarded
    if ( (fabs(deltaPhiRescaled) > 2.*m_broadDeltaPhi) && 
       (fabs(deltaPhiTrack) > 2.*m_broadDeltaPhi) && 
       (fabs(deltaPhiStd) > 2.*m_broadDeltaPhi) ){
      ATH_MSG_DEBUG("FAILS broad window phi match (track phi, phirotCluster , phiRotTrack , "
                  <<"cluster phi corrected, cluster phi): ( " 
                  << trkPhi << ", " << phiRotRescaled<< ", "<<phiRotTrack<< ", " 
                  <<clusterPhiCorrected<< ")" );
      return false;
  }


    //if not false returned we end up here
    return true;
  }//use candidate match   
  return true; 
}



