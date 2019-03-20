/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

// INCLUDE HEADER FILES:

#include "EMTrackMatchBuilder.h"

#include "egammaRecEvent/egammaRec.h"

#include "CaloUtils/CaloCellList.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "egammaUtils/CandidateMatchHelpers.h"
#include "FourMomUtils/P4Helpers.h"
#include "SGTools/CurrentEventStore.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/EventContext.h"

#include <cmath>

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:

EMTrackMatchBuilder::EMTrackMatchBuilder(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent) : 
  AthAlgTool(type, name, parent)
{
  // declare interface
  declareInterface<IEMTrackMatchBuilder>(this);
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


  ATH_CHECK(m_TrackParticlesKey.initialize());

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
StatusCode EMTrackMatchBuilder::executeRec(const EventContext& ctx, egammaRec* eg) const
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
  SG::ReadHandle<xAOD::TrackParticleContainer> trackPC(m_TrackParticlesKey, ctx);

  // check is only used for serial running; remove when MT scheduler used
  if( !trackPC.isValid() ) {
    ATH_MSG_ERROR("Couldn't retrieve TrackParticle container with key: " << m_TrackParticlesKey.key());
    return StatusCode::FAILURE;
  }

  // call the execute method
  CHECK( trackExecute(ctx, eg, trackPC.cptr()) );
  return StatusCode::SUCCESS;
}

// ===============================================================
StatusCode EMTrackMatchBuilder::trackExecute(const EventContext& ctx, egammaRec* eg, const xAOD::TrackParticleContainer*    trackPC) const
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
          inBroadWindow(ctx, trkMatches, cluster, trackNumber, isTRT, (*trkIt), Trk::alongMomentum)) && m_isCosmics)
    {
      // Second chance for cosmics, flip eta and phi
      if (isCandidateMatch(cluster, isTRT, (*trkIt), true))
        inBroadWindow(ctx, trkMatches, cluster,  trackNumber, isTRT,  (*trkIt), Trk::oppositeMomentum);
    }
  }

  if(trkMatches.size()>0)
  {
    //sort the track matches
    std::sort(trkMatches.begin(), trkMatches.end(), TrackMatchSorter);


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
EMTrackMatchBuilder::inBroadWindow(const EventContext& ctx,
                                   std::vector<TrackMatch>& trackMatches,
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
  
  /* 
   * If it is TRT we can determine immediately if it failed
   * or not (we use matches at calo)
   * If it is silicon we always do both Rescale 
   * and Standard and then we decide
   */
  if(trkTRT){//TRTSA
    if(!m_extrapolationTool->matchesAtCalo (ctx,
                                            cluster, 
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
    if (m_extrapolationTool->getMatchAtCalo (ctx,
                                             cluster, 
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
    if (m_extrapolationTool->getMatchAtCalo (ctx, 
                                             cluster, 
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

  /*In case of extrapolation from 
   * perigee keep the dPhi from the last measurement
   * This means for success we do another extrapolation
   * from last
   */
  if (!m_useLastMeasurement ) {  
    IEMExtrapolationTools::TrkExtrapDef extrapFrom1 = IEMExtrapolationTools::fromLastMeasurement;
    std::vector<double>  eta1(4, -999.0);
    std::vector<double>  phi1(4, -999.0);
    std::vector<double>  deltaEta1(4, -999.0);
    std::vector<double>  deltaPhi1(4, -999.0);
    if (m_extrapolationTool->getMatchAtCalo (ctx,
                                             cluster, 
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
    double z_first=candidatePerigee.position().z();
    //===========================================================//     
    double clusterEta=cluster->etaBE(2);
    bool isEndCap= cluster->inEndcap();
    double Et= cluster->e()/cosh(trkEta);
    if(trkTRT){
      Et = cluster->et();
    }
    //===========================================================//     
    double etaclus_corrected = CandidateMatchHelpers::CorrectedEta(clusterEta,z_first,isEndCap);
    double phiRot = CandidateMatchHelpers::PhiROT(Et,trkEta, track->charge(),r_first ,isEndCap)  ;
    double phiRotTrack = CandidateMatchHelpers::PhiROT(track->pt(),trkEta, track->charge(),r_first ,isEndCap)  ;
    //===========================================================//     
    double deltaPhiStd = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhi);
    double trkPhiCorr = P4Helpers::deltaPhi(trkPhi, phiRot);
    double deltaPhi2 = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhiCorr);
    double trkPhiCorrTrack = P4Helpers::deltaPhi(trkPhi, phiRotTrack);
    double deltaPhi2Track = P4Helpers::deltaPhi(cluster->phiBE(2), trkPhiCorrTrack);
    //===========================================================//     
    //check eta match . Both metrics need to fail in order to disgard the track
    if ( (!trkTRT) && (fabs(cluster->etaBE(2) - trkEta) > 2.*m_broadDeltaEta) && (fabs( etaclus_corrected- trkEta) > 2.*m_broadDeltaEta)){
      ATH_MSG_DEBUG(" Fails broad window eta match (track eta, cluster eta, cluster eta corrected): ( " << trkEta << ", " << cluster->etaBE(2) <<", "<<etaclus_corrected<<")" );
      return false;
    }
    //It has to fail all metrics in order to be disgarded
    else if ( (fabs(deltaPhi2) > 2.*m_broadDeltaPhi) && (fabs(deltaPhi2Track) > 2.*m_broadDeltaPhi) && (fabs(deltaPhiStd) > 2.*m_broadDeltaPhi) ){
      ATH_MSG_DEBUG(" Fails broad window eta match (track eta, cluster eta, cluster eta corrected): ( " << trkEta << ", " << cluster->etaBE(2) <<", "<<etaclus_corrected<<")" );
      return false;
    }
    //if not false returned we end up here
    return true;
  }//use candidate match   
  return true; 
}

bool EMTrackMatchBuilder::TrackMatchSorter(const EMTrackMatchBuilder::TrackMatch& match1,
					   const EMTrackMatchBuilder::TrackMatch& match2)
{

  if(match1.score!= match2.score) {//Higher score
    return match1.score>match2.score;
  }
  //sqrt(0.025**2)*sqrt(2)/sqrt(12) ~ 0.01
  if(fabs(match1.dR-match2.dR)<1e-02) {

    if(fabs(match1.seconddR-match2.seconddR)>1e-02 ){ //Can the second distance separate them?
      return match1.seconddR < match2.seconddR	;
    }

    if((match1.hitsScore!= match2.hitsScore)){ //use the one with more pixel
      return match1.hitsScore>match2.hitsScore;
    }

  }

  //closest DR
  return match1.dR < match2.dR	;
}


