/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSummaryHelperTool/InDetTrackSummaryHelperTool.h"
// forward declares
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkToolInterfaces/IPixelToTPIDTool.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
// normal includes
#include "Identifier/Identifier.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetRecToolInterfaces/IInDetTestBLayerTool.h"

#include <cassert>

//==========================================================================
InDet::InDetTrackSummaryHelperTool::InDetTrackSummaryHelperTool(
   const std::string& t,
   const std::string& n,
   const IInterface*  p )
   :
   AthAlgTool(t,n,p),
   m_pixelId(0),
   m_sctId(0),
   m_trtId(0),
   m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels"), 
   m_pixeldedxtool(""),
   m_holeSearchTool("InDet::InDetTrackHoleSearchTool"),
   m_testBLayerTool(""),
   m_TRTStrawSummarySvc("InDetTRTStrawStatusSummarySvc",n),
   m_doSharedHits(false),
   m_doSharedHitsTRT(false), 
   m_doSplitPixelHits(true),
   m_runningTIDE_Ambi(false),
   m_ITkGeometry(false)
{
   declareInterface<ITrackSummaryHelperTool>(this);
   declareProperty("AssoTool",            m_assoTool);
   declareProperty("PixelToTPIDTool",     m_pixeldedxtool);
   declareProperty("HoleSearch",          m_holeSearchTool);
   declareProperty("TestBLayerTool",      m_testBLayerTool);
   declareProperty("TRTStrawSummarySvc",  m_TRTStrawSummarySvc);
   declareProperty("DoSharedHits",        m_doSharedHits);
   declareProperty("DoSharedHitsTRT",     m_doSharedHitsTRT);
   declareProperty("DoSplitHits",         m_doSplitPixelHits);
   declareProperty("RunningTIDE_Ambi",    m_runningTIDE_Ambi);
   declareProperty("usePixel",            m_usePixel = true);
   declareProperty("useSCT",              m_useSCT   = true);
   declareProperty("useTRT",              m_useTRT   = true);
   declareProperty("OverwriteIDSummary",  m_overwriteidsummary = false);
   declareProperty("ITkGeometry",         m_ITkGeometry);
}

//==========================================================================

InDet::InDetTrackSummaryHelperTool::~InDetTrackSummaryHelperTool()
{ }

//==========================================================================

StatusCode InDet::InDetTrackSummaryHelperTool::initialize()
{
   if (m_usePixel) {
      if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) {
         msg(MSG::ERROR) << "Could not get PixelID helper !" << endreq;
         return StatusCode::FAILURE;
      }
   }

   if (m_useSCT) {
      if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) {
         msg(MSG::ERROR) << "Could not get SCT_ID helper !" << endreq;
         return StatusCode::FAILURE;
      }
   }

   if (m_useTRT) {
      if (detStore()->retrieve(m_trtId, "TRT_ID").isFailure()) {
         msg(MSG::ERROR) << "Could not get TRT_ID helper !" << endreq;
         return StatusCode::FAILURE;
      }
   }

   if (m_doSharedHits) {
      if ( m_assoTool.retrieve().isFailure() ) {
         msg(MSG::FATAL) << "Failed to retrieve tool " << m_assoTool << endreq;
         return StatusCode::FAILURE;
      } else {
         msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endreq;
      }
   }

   if ( !m_pixeldedxtool.empty() && m_pixeldedxtool.retrieve().isFailure() ) {
      ATH_MSG_ERROR ("Failed to retrieve pixel dEdx tool " << m_pixeldedxtool);
      ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
      return StatusCode::FAILURE;
   } else {
      if ( !m_pixeldedxtool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_pixeldedxtool << endreq;
   }

   if ( m_holeSearchTool.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_holeSearchTool << endreq;
      return StatusCode::FAILURE;
   } else {
      msg(MSG::INFO) << "Retrieved tool " << m_holeSearchTool << endreq;
   }

   if ( !m_testBLayerTool.empty() && m_testBLayerTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR ("Failed to retrieve Test B Layer tool " << m_pixeldedxtool);
      ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
      return StatusCode::FAILURE;
   } else {
      if ( !m_testBLayerTool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_pixeldedxtool << endreq;
   }


   if (m_useTRT && !m_TRTStrawSummarySvc.empty() && m_TRTStrawSummarySvc.retrieve().isFailure() ) {
      ATH_MSG_ERROR ("Failed to retrieve StrawStatus Summary " << m_TRTStrawSummarySvc);
      ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
      return StatusCode::FAILURE;
   } else {
      if ( !m_TRTStrawSummarySvc.empty()) msg(MSG::INFO) << "Retrieved tool " << m_TRTStrawSummarySvc << endreq;
   }



   msg(MSG::INFO) << "initialize() successful in " << name() << endreq;

   return StatusCode::SUCCESS;
}


//==========================================================================
void InDet::InDetTrackSummaryHelperTool::analyse(const Trk::Track& track, 
                                                 const Trk::RIO_OnTrack* rot, 
                                                 const Trk::TrackStateOnSurface* tsos,
                                                 std::vector<int>& information, 
                                                 std::bitset<Trk::numberOfDetectorTypes>& hitPattern,
                                                 Trk::DetailedHitInfo& detailedInfo ) const
{
   const Identifier& id = rot->identify();
   bool  isOutlier      = (tsos->type(Trk::TrackStateOnSurface::Outlier));
   bool  ispatterntrack =  (track.info().trackFitter()==Trk::TrackInfo::Unknown);


   //pointer to det El (possibly 0), from pointer to PRD (which can also be zero - at the moment at least)
   //const TrkDetElementBase* detEl = rot->detectorElement(); 

   if ( m_usePixel && m_pixelId->is_pixel(id) ) {

      if (isOutlier && !ispatterntrack ) { // ME: outliers on pattern tracks may be reintegrated by fitter, so count them as hits
         information[Trk::numberOfPixelOutliers]++;
         if (m_pixelId->is_blayer(id)){
            information[Trk::numberOfBLayerOutliers]++;
         }
	 if (m_pixelId->layer_disk(id)==0 && m_pixelId->is_barrel(id)){
	   information[Trk::numberOfInnermostPixelLayerOutliers]++;
	 }
	 if (m_pixelId->layer_disk(id)==1 && m_pixelId->is_barrel(id)){
	   information[Trk::numberOfNextToInnermostPixelLayerOutliers]++;
	 }
      }
      else {
         
         bool hitIsSplit(false);

         if (m_pixelId->is_dbm(id)) {
           int offset = static_cast<int> (Trk::DBM0); //get int value of first DBM layer
           offset    += m_pixelId->layer_disk(id);
           hitPattern.set(offset);
           information[Trk::numberOfDBMHits]++;
         }

	 else {
	   information[Trk::numberOfPixelHits]++;
	   if ( (m_pixelId->is_blayer(id) ) ) information[Trk::numberOfBLayerHits]++; // found b layer hit
	   if (m_pixelId->layer_disk(id)==0 && m_pixelId->is_barrel(id)) information[Trk::numberOfInnermostPixelLayerHits]++;
	   if (m_pixelId->layer_disk(id)==1 && m_pixelId->is_barrel(id)) information[Trk::numberOfNextToInnermostPixelLayerHits]++;  
	   // check to see if there's an ambiguity with the ganged cluster.
	   const PixelClusterOnTrack* pix = dynamic_cast<const PixelClusterOnTrack*>(rot);
	   if ( !pix ) {
	     if (msgLvl(MSG::ERROR)) msg(MSG::ERROR)<<"Could not cast pixel RoT to PixelClusterOnTrack!"<<endreq;
	   } else {
	     const InDet::PixelCluster* pixPrd = pix->prepRawData();
	     if ( pixPrd && pixPrd->isSplit() ){ information[Trk::numberOfPixelSplitHits]++; hitIsSplit=true; }
	     if ( pixPrd && m_pixelId->is_blayer(id) && pixPrd->isSplit() ) information[Trk::numberOfBLayerSplitHits]++;
	     if ( pixPrd && m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==0 && pixPrd->isSplit() ) information[Trk::numberOfInnermostLayerSplitHits]++;
	     if ( pixPrd && m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==1 && pixPrd->isSplit() ) information[Trk::numberOfNextToInnermostLayerSplitHits]++;
	     if ( pix->isBroadCluster() ) information[Trk::numberOfPixelSpoiltHits]++;
	     if ( pix->hasClusterAmbiguity() ) {
               information[Trk::numberOfGangedPixels]++;
               if (pix->isFake() ) 
		 information[Trk::numberOfGangedFlaggedFakes]++;
	     }
	   }

	   if ( ( m_pixelId->is_barrel(id) ) ) { 
	     int offset = m_pixelId->layer_disk(id);  
	     if (!hitPattern.test(offset)) information[Trk::numberOfContribPixelLayers]++; 
	     hitPattern.set(offset); // assumes numbered consecutively 
	   } 
	   else { 
	     int offset = static_cast<int> (Trk::pixelEndCap0); //get int value of first pixel endcap disc 
	     offset    += m_pixelId->layer_disk(id); 
	     if (!hitPattern.test(offset)) information[Trk::numberOfContribPixelLayers]++; 
	     hitPattern.set(offset); // assumes numbered consecutively 
	   }
	 } 
	 
	 // re-updating counters for ITk geometry
	 if (m_ITkGeometry) {
     Trk::DetectorRegion region;
     
     if (m_pixelId->is_barrel(id)) {
       const InDetDD::SiDetectorElement* detEl = dynamic_cast<const InDetDD::SiDetectorElement*>(rot->detectorElement()); 
       if (detEl->isBarrelRing()) region = Trk::pixelBarrelRing;
       else if (detEl->isInclined())  region = Trk::pixelBarrelInclined;
       else region = Trk::pixelBarrelFlat;
     } else region = Trk::pixelEndcap;

     detailedInfo.addHit(region, m_pixelId->layer_disk(id), m_pixelId->eta_module(id));
     
     information[Trk::numberOfContribPixelLayers] = detailedInfo.getPixelContributions();
     
     information[Trk::numberOfContribPixelBarrelFlatLayers]     = detailedInfo.getContributionFromRegion(Trk::pixelBarrelFlat    );
     information[Trk::numberOfContribPixelBarrelInclinedLayers] = detailedInfo.getContributionFromRegion(Trk::pixelBarrelInclined);
     information[Trk::numberOfContribPixelBarrelRings]          = detailedInfo.getContributionFromRegion(Trk::pixelBarrelRing    );    
     information[Trk::numberOfContribPixelEndcap]               = detailedInfo.getContributionFromRegion(Trk::pixelEndcap        );    
     
     information[Trk::numberOfPixelBarrelFlatHits]     = detailedInfo.getHitsFromRegion(Trk::pixelBarrelFlat    );        
     information[Trk::numberOfPixelBarrelInclinedHits] = detailedInfo.getHitsFromRegion(Trk::pixelBarrelInclined);       
     information[Trk::numberOfPixelBarrelRingHits]     = detailedInfo.getHitsFromRegion(Trk::pixelBarrelRing    );           
     information[Trk::numberOfPixelEndcapHits]         = detailedInfo.getHitsFromRegion(Trk::pixelEndcap        );             
   }   
   

         if (m_doSharedHits) {
           // If we are running the TIDE ambi don't count split hits as shared 
           if( !(m_runningTIDE_Ambi && hitIsSplit) ){
             // used in more than one track ?
             if ( m_assoTool->isShared(*(rot->prepRawData())) ) {
               if (msgLvl(MSG::DEBUG)) msg() << "shared Pixel hit found" << endreq;
               information[Trk::numberOfPixelSharedHits]++;
               if ( (m_pixelId->is_blayer(id) ) ) {
                 if (msgLvl(MSG::DEBUG)) msg() << "--> shared Pixel hit is in b-layer" << endreq;
                 information[Trk::numberOfBLayerSharedHits]++;        
               }
	       if ( (m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==0) ) {
                 if (msgLvl(MSG::DEBUG)) msg() << "--> shared Pixel hit is in innermost layer" << endreq;
                 information[Trk::numberOfInnermostPixelLayerSharedHits]++;        
               } 
	       if ( (m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==1) ) {
                 if (msgLvl(MSG::DEBUG)) msg() << "--> shared Pixel hit is in next to innermost layer" << endreq;
                 information[Trk::numberOfNextToInnermostPixelLayerSharedHits]++;        
               }
             }
           }
         }
      }

   } else if (m_useSCT && m_sctId->is_sct(id) ) {

      if (isOutlier && !ispatterntrack ) { // ME: outliers on pattern tracks may be reintegrated by fitter, so count them as hits    
         information[Trk::numberOfSCTOutliers]++;

      } else {


         information[Trk::numberOfSCTHits]++;
         const InDet::SCT_ClusterOnTrack *sctclus=dynamic_cast<const InDet::SCT_ClusterOnTrack *>(rot);
         if ( !sctclus ) {
            if (msgLvl(MSG::ERROR)) msg(MSG::ERROR)<<"Could not cast SCT RoT to SCT_ClusterOnTrack!"<<endreq;
         } else {
            if (sctclus->isBroadCluster()) information[Trk::numberOfSCTSpoiltHits]++;
         }

         if ( (m_sctId->is_barrel(id) ) ) {
            int offset = static_cast<int>(Trk::sctBarrel0);
            hitPattern.set( offset + m_sctId->layer_disk(id) ); // assumes numbered consecutively
         } else {
            int offset = static_cast<int>(Trk::sctEndCap0); //get int value of first sct endcap disc
            hitPattern.set( offset + m_sctId->layer_disk(id) ); // assumes numbered consecutively
         }

         if (m_doSharedHits) {
            // used in more than one track ?
            if ( m_assoTool->isShared(*(rot->prepRawData())) ) {
               if (msgLvl(MSG::DEBUG)) msg() << "shared SCT hit found" << endreq;
               information[Trk::numberOfSCTSharedHits]++;
            }
         }
      }
        
   } else if (m_useTRT && m_trtId->is_trt(id) ) {

     
     bool isArgonStraw   = false;
     bool isKryptonStraw = false;
     if (!m_TRTStrawSummarySvc.empty()) {
       int statusHT = m_TRTStrawSummarySvc->getStatusHT(id);
       if ( statusHT == TRTCond::StrawStatus::Argon ||
            statusHT == TRTCond::StrawStatus::Dead  ||
            statusHT == TRTCond::StrawStatus::EmulateArgon ) {
         isArgonStraw = true;
       }
       if ( statusHT == TRTCond::StrawStatus::Krypton ||
            statusHT == TRTCond::StrawStatus::EmulateKrypton ) {
         isKryptonStraw = true;
       }
     }
     if ( !isArgonStraw && !isKryptonStraw ) {
       information[Trk::numberOfTRTXenonHits]++;
     }

     
     if (isOutlier && !ispatterntrack ) { // ME: outliers on pattern tracks may be reintegrated by fitter, so count them as hits    
       information[Trk::numberOfTRTOutliers]++;

       const InDet::TRT_DriftCircleOnTrack* trtDriftCircle 
         = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(  rot  );
       if ( !trtDriftCircle ) {
         if (msgLvl(MSG::ERROR)) msg(MSG::ERROR)<<"Could not cast TRT RoT to TRT_DriftCircleOnTrack!"<<endreq;
       } else {
         if ( trtDriftCircle->highLevel()==true && !isArgonStraw && !isKryptonStraw ) information[Trk::numberOfTRTHighThresholdOutliers]++;
       }

     } else {

       information[Trk::numberOfTRTHits]++;
       double error2=rot->localCovariance()(0,0);
       if (error2>1) information[Trk::numberOfTRTTubeHits]++;

       const InDet::TRT_DriftCircleOnTrack* trtDriftCircle 
         = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(  rot  );
       if ( !trtDriftCircle ) {
         if (msgLvl(MSG::ERROR)) msg(MSG::ERROR)<<"Could not cast TRT RoT to TRT_DriftCircleOnTrack!"<<endreq;
       } else {
         if ( trtDriftCircle->highLevel()==true ) {
           if ( !isArgonStraw && !isKryptonStraw ) information[Trk::numberOfTRTHighThresholdHits]++;
           assert (Trk::numberOfTRTHighThresholdHitsTotal<information.size());
           information[Trk::numberOfTRTHighThresholdHitsTotal]++;
         }
       }
     }
   }

   if (m_doSharedHitsTRT) {
            // used in more than one track ?
     if ( m_assoTool->isShared(*(rot->prepRawData())) ) {
       if (msgLvl(MSG::DEBUG)) msg() << "shared TRT hit found" << endreq;
       information[Trk::numberOfTRTSharedHits]++;
     }
   }
   
   return;
}

void InDet::InDetTrackSummaryHelperTool::analyse(const Trk::Track& track,
                                                 const Trk::CompetingRIOsOnTrack* crot, 
                                                 const Trk::TrackStateOnSurface* tsos,
                                                 std::vector<int>& information, 
                                                 std::bitset<Trk::numberOfDetectorTypes>& hitPattern,                                                 
                                                 Trk::DetailedHitInfo& detailedInfo ) const
{
   // re-produce prior behaviour (i.e. just take most probable ROT)
   analyse(track, &crot->rioOnTrack(crot->indexOfMaxAssignProb() ), tsos, information, hitPattern, detailedInfo);
}

void InDet::InDetTrackSummaryHelperTool::searchForHoles(const Trk::Track& track, 
                                                        std::vector<int>& information,
                                                        const Trk::ParticleHypothesis partHyp         ) const 
{

   // ME: this is a temporary change for getting the blayer info into the summary 
   //if (msgLvl(MSG::WARNING))
   //{
   //  msg(MSG::WARNING) << "You are accessing the hole search through the InDetTrackSummaryHelperTool." << endreq;
   //  msg(MSG::WARNING) << "This will soon be disabled. Please access the HoleSearchTool directly!" << endreq;
   // }
 
   ATH_MSG_DEBUG("Do hole search within HELPER, PLEASE FIX THIS AFTER 16.0.X");
   m_holeSearchTool->countHoles(track,information,partHyp);
  

   // this is a hack, we need to run the TestBLayer Tool somewhere

   if (m_usePixel && !m_testBLayerTool.empty() ) {

      if ( information[Trk::numberOfContribPixelLayers] == 0 ) {
         ATH_MSG_DEBUG("No pxiels on track, so wo do not expect a B-Layer hit !");
         information[Trk::expectBLayerHit] = 0;
	 information[Trk::expectInnermostPixelLayerHit] = 0;
	 information[Trk::expectNextToInnermostPixelLayerHit] = 0;
      } else{


	///blayer block
	if ( information[Trk::numberOfBLayerHits] > 0) {
	  ATH_MSG_DEBUG("B-Layer hit on track, so we expect a B-Layer hit !");
	  information[Trk::expectBLayerHit] = 1;
	} else {
	  ATH_MSG_DEBUG("Testing B-Layer using tool..");
	  if (m_testBLayerTool->expectHitInBLayer(&track) ) {
            ATH_MSG_DEBUG("expect B-Layer hit !");
            information[Trk::expectBLayerHit] = 1;
	  } else {
            ATH_MSG_DEBUG("do not expect B-Layer hit !");
            information[Trk::expectBLayerHit] = 0;
	  }
	}

	//innermost layer block
	if (information[Trk::numberOfInnermostPixelLayerHits] > 0){
	  information[Trk::expectInnermostPixelLayerHit] = 1;
	} else {

	if (m_testBLayerTool->expectHitInInnermostPixelLayer(&track) ) {
            ATH_MSG_DEBUG("expect Pixel Layer 0 hit !");
            information[Trk::expectInnermostPixelLayerHit] = 1;
	  } else {
            ATH_MSG_DEBUG("do not expect Pixel Layer 0 hit !");
            information[Trk::expectInnermostPixelLayerHit] = 0; 
	}  
	
	}
	
	//next to innermost block
	if(information[Trk::numberOfNextToInnermostPixelLayerHits] > 0){
	  information[Trk::expectNextToInnermostPixelLayerHit] = 1; 
	} else {
	  
	  if (m_testBLayerTool->expectHitInNextToInnermostPixelLayer(&track) ) {
	    ATH_MSG_DEBUG("expect Pixel Layer 1 hit !");
            information[Trk::expectNextToInnermostPixelLayerHit] = 1;
	  } else {
	    ATH_MSG_DEBUG("do not expect Pixel Layer 1 hit !");
            information[Trk::expectNextToInnermostPixelLayerHit] = 0;
	  }
	  
	}
	
      }
   }

   return;
}


void InDet::InDetTrackSummaryHelperTool::updateSharedHitCount(const Trk::Track &track, Trk::TrackSummary& summary) const {
    
    // loop over track states on surface and take pixel / sct to update the shared hit count
    summary.m_information[Trk::numberOfPixelSharedHits]  = 0;
    summary.m_information[Trk::numberOfBLayerSharedHits] = 0;
    summary.m_information[Trk::numberOfInnermostPixelLayerSharedHits] = 0;
     summary.m_information[Trk::numberOfNextToInnermostPixelLayerSharedHits] = 0;
    summary.m_information[Trk::numberOfSCTSharedHits]    = 0;
     summary.m_information[Trk::numberOfTRTSharedHits]    = 0;
    if( m_runningTIDE_Ambi ) {
      summary.m_information[Trk::numberOfPixelSplitHits]   = 0;
      summary.m_information[Trk::numberOfBLayerSplitHits]   = 0;
      summary.m_information[Trk::numberOfInnermostLayerSplitHits]   = 0;
      summary.m_information[Trk::numberOfNextToInnermostLayerSplitHits]   = 0;
    }

    const DataVector<const Trk::MeasurementBase>* measurements = track.measurementsOnTrack();               
    if (measurements){
        for (auto& ms : *measurements){
            const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(ms);
            // check if it's a rot
            if (rot){
                const Identifier& id = rot->identify();
                if ( m_doSharedHits && m_usePixel && m_pixelId->is_pixel(id) ) {
                    // check if shared 
                   bool hitIsSplit(false);
                   if(m_runningTIDE_Ambi){
                      const PixelClusterOnTrack* pix = dynamic_cast<const PixelClusterOnTrack*>(rot);
                      if(pix){
                         const InDet::PixelCluster* pixPrd = pix->prepRawData();
                         if ( pixPrd && pixPrd->isSplit() ){ 
                            summary.m_information[Trk::numberOfPixelSplitHits]++; 
                            hitIsSplit=true;
			     if ( m_pixelId->is_blayer(id)) summary.m_information[Trk::numberOfBLayerSplitHits]++;
			    if ( m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==0) summary.m_information[Trk::numberOfInnermostLayerSplitHits]++;
			    if ( m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==1) summary.m_information[Trk::numberOfNextToInnermostLayerSplitHits]++;  
                         }
                      }
                   }

                   // If we are running the TIDE ambi don't count split hits as shared 
                  if( !(m_runningTIDE_Ambi && hitIsSplit) ){
                      if ( m_assoTool->isShared(*(rot->prepRawData())) ) {
                          ATH_MSG_DEBUG("shared Pixel hit found");
                          summary.m_information[Trk::numberOfPixelSharedHits]++;
			  if ( (m_pixelId->is_blayer(id) ) ) {
			    ATH_MSG_DEBUG("--> shared Pixel hit is in b-layer");
			    summary.m_information[Trk::numberOfBLayerSharedHits]++;       
			  }
                          if ( (m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==0) ) {
                             ATH_MSG_DEBUG("--> shared Pixel hit is in Innermost Pixel layer");
                             summary.m_information[Trk::numberOfInnermostPixelLayerSharedHits]++;       
                          }
			  else if ( (m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==1) ) {
                             ATH_MSG_DEBUG("--> shared Pixel hit is in Next To Innermost Pixel layer");
                             summary.m_information[Trk::numberOfNextToInnermostPixelLayerSharedHits]++;       
                          }

		      }
                    }
                } else if ( m_doSharedHits && m_useSCT && m_sctId->is_sct(id) ){
                    // used in more than one track ?
                    if ( m_assoTool->isShared(*(rot->prepRawData())) ) {
                       if (msgLvl(MSG::DEBUG)) msg() << "shared SCT hit found" << endreq;
                       summary.m_information[Trk::numberOfSCTSharedHits]++;
                    }
                }
            
		if (m_doSharedHitsTRT && m_useTRT && m_trtId->is_trt(id)) {
		  // used in more than one track ?
		  if ( m_assoTool->isShared(*(rot->prepRawData())) ) {
		    if (msgLvl(MSG::DEBUG)) msg() << "shared TRT hit found" << endreq;
		    summary.m_information[Trk::numberOfTRTSharedHits]++;
		  }
		}
		
	    }
	}
    }
    return;
}

void  InDet::InDetTrackSummaryHelperTool::updateExpectedHitInfo(const Trk::Track &track, Trk::TrackSummary& summary) const{

  if (m_usePixel && !m_testBLayerTool.empty() ) {
    
    if ( summary.m_information[Trk::numberOfContribPixelLayers] == 0 ) {
      ATH_MSG_DEBUG("No pxiels on track, so wo do not expect a B-Layer hit !");
      summary.m_information[Trk::expectBLayerHit] = 0;
      summary.m_information[Trk::expectInnermostPixelLayerHit] = 0;
      summary.m_information[Trk::expectNextToInnermostPixelLayerHit] = 0;
    } else{
      
      
      ///blayer block
      if ( summary.m_information[Trk::numberOfBLayerHits] > 0) {
	ATH_MSG_DEBUG("B-Layer hit on track, so we expect a B-Layer hit !");
	summary.m_information[Trk::expectBLayerHit] = 1;
      } else {
	ATH_MSG_DEBUG("Testing B-Layer using tool..");
	if (m_testBLayerTool->expectHitInBLayer(&track) ) {
	  ATH_MSG_DEBUG("expect B-Layer hit !");
	  summary.m_information[Trk::expectBLayerHit] = 1;
	} else {
	  ATH_MSG_DEBUG("do not expect B-Layer hit !");
	  summary.m_information[Trk::expectBLayerHit] = 0;
	}
      }
      
      //innermost layer block
      if (summary.m_information[Trk::numberOfInnermostPixelLayerHits] > 0){
	summary.m_information[Trk::expectInnermostPixelLayerHit] = 1;
      } else {
	
	if (m_testBLayerTool->expectHitInInnermostPixelLayer(&track) ) {
	  ATH_MSG_DEBUG("expect Pixel Layer 0 hit !");
	  summary.m_information[Trk::expectInnermostPixelLayerHit] = 1;
	} else {
	  ATH_MSG_DEBUG("do not expect Pixel Layer 0 hit !");
	  summary.m_information[Trk::expectInnermostPixelLayerHit] = 0;
	}  
	
      }
      
      //next to innermost block
      if(summary.m_information[Trk::numberOfNextToInnermostPixelLayerHits] > 0){
	summary.m_information[Trk::expectNextToInnermostPixelLayerHit] = 1;
      } else {
	
	if (m_testBLayerTool->expectHitInNextToInnermostPixelLayer(&track) ) {
	  ATH_MSG_DEBUG("expect Pixel Layer 1 hit !");
	  summary.m_information[Trk::expectNextToInnermostPixelLayerHit] = 1;
	} else {
	  ATH_MSG_DEBUG("do not expect Pixel Layer 1 hit !");
	  summary.m_information[Trk::expectNextToInnermostPixelLayerHit] = 0;
	}
	
      }
      
    }
  

}
  
  return;
}


void InDet::InDetTrackSummaryHelperTool::updateAdditionalInfo(Trk::TrackSummary& summary,std::vector<float>& eprob,float& dedx,int& nclus,int& noverflowclus) const {
  
  summary.m_eProbability = eprob;
  summary.m_dedx = dedx;
  summary.m_nhitsdedx =  nclus;
  summary.m_nhitsoverflowdedx = noverflowclus;

 return;
}

void  InDet::InDetTrackSummaryHelperTool::addDetailedTrackSummary(const Trk::Track &track, Trk::TrackSummary &summary) const {
   if ( summary.m_indetTrackSummary && !m_overwriteidsummary){
      ATH_MSG_DEBUG("TrackSummary already has detailed indet track summary, not adding a new one");
      return;
   }
   if ( !m_usePixel ) {
      ATH_MSG_DEBUG("Pixels are off, so no need for an detailed indet track summary");
      return;
   }
   ATH_MSG_DEBUG("Adding detailed indet track summary");
   delete summary.m_indetTrackSummary;
   Trk::InDetTrackSummary* indetTrackSummary = new Trk::InDetTrackSummary();
   Trk::InDetTrackSummary& trackSummary      = *indetTrackSummary;
   if (m_usePixel && !m_pixeldedxtool.empty() && (track.perigeeParameters() || !track.trackParameters()->empty())){
      const Trk::TrackParameters *par = track.perigeeParameters();
      if (!par) par = *track.trackParameters()->begin();
      double p      = (par->parameters()[Trk::qOverP]!=0) ? 1./par->parameters()[Trk::qOverP] : 1.e15;
      double dedx   = summary.getPixeldEdx();
      int ngoodhits = summary.numberOfUsedHitsdEdx();
      if (ngoodhits>0 && dedx>0 && track.info().trackFitter()!=Trk::TrackInfo::Unknown && p<1.e15){
         trackSummary.m_likelihoodspixeldedx = m_pixeldedxtool->getLikelihoods(dedx,p,ngoodhits);
         trackSummary.m_massdedx             = m_pixeldedxtool->getMass(dedx,p,ngoodhits);
      }
   }
   summary.m_indetTrackSummary = indetTrackSummary;
}

// ====================================================================================================================

StatusCode InDet::InDetTrackSummaryHelperTool::finalize() {
   return StatusCode::SUCCESS;
}

