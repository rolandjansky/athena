/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FatrasRecoTools/ISF_TrackSummaryHelperTool.h"
// forward declares
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
// normal includes
#include "Identifier/Identifier.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkParameters/TrackParameters.h"

//==========================================================================

iFatras::ISF_TrackSummaryHelperTool::ISF_TrackSummaryHelperTool(const std::string& t,
								const std::string& n,
								const IInterface*  p ):
  AthAlgTool(t,n,p),
  m_pixelId(0),
  m_sctId(0),
  m_assoTool("iFatras::ISF_PRD_AssociationTool"), 
  m_doSharedHits(false)
{
  declareInterface<ITrackSummaryHelperTool>(this);
  declareProperty("AssoTool",            m_assoTool);
  declareProperty("DoSharedHits",        m_doSharedHits);
  declareProperty("usePixel",            m_usePixel = true);
  declareProperty("useSCT",              m_useSCT   = true);
  declareProperty("OverwriteIDSummary",  m_overwriteidsummary = false);
} 

//==========================================================================

iFatras::ISF_TrackSummaryHelperTool::~ISF_TrackSummaryHelperTool() {}

//==========================================================================

StatusCode iFatras::ISF_TrackSummaryHelperTool::initialize()
{
  if (m_usePixel) {
    if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) {
      msg(MSG::ERROR) << "Could not get PixelID helper !" << endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::VERBOSE) << "PixelID helper retrieved successfully!" << endmsg;
  }
  
  if (m_useSCT) {
    if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) {
      msg(MSG::ERROR) << "Could not get SCT_ID helper !" << endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::VERBOSE) << "PixelID helper retrieved successfully!" << endmsg;
  }
  
  if (m_doSharedHits) {
    if ( m_assoTool.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_assoTool << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endmsg;
    }
  }

  msg(MSG::INFO) << "initialize() successful in " << name() << endmsg;

  return StatusCode::SUCCESS;
}

//==========================================================================

void iFatras::ISF_TrackSummaryHelperTool::analyse(const Trk::Track& track, 
						  const Trk::RIO_OnTrack* rot, 
						  const Trk::TrackStateOnSurface* tsos,
						  std::vector<int>& information, 
						  std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const
{
  const Identifier& id = rot->identify();
  bool  isOutlier      = (tsos->type(Trk::TrackStateOnSurface::Outlier));
  bool  ispatterntrack =  (track.info().trackFitter()==Trk::TrackInfo::Unknown);
  
  if (msgLvl(MSG::DEBUG)) msg() << "Starting analyse()" << endmsg;

  if ( m_usePixel && m_pixelId->is_pixel(id) ) {

    if (msgLvl(MSG::DEBUG)) msg() << "Pixel hit found" << endmsg;
    
    if (isOutlier && !ispatterntrack ) { // ME: outliers on pattern tracks may be reintegrated by fitter, so count them as hits
      
      if (msgLvl(MSG::DEBUG)) msg() << "Pixel outlier info storing" << endmsg;

      information[Trk::numberOfPixelOutliers]++;
      if (m_pixelId->layer_disk(id)==0 && m_pixelId->is_barrel(id)){
	information[Trk::numberOfInnermostPixelLayerOutliers]++;
      }
      if (m_pixelId->layer_disk(id)==1 && m_pixelId->is_barrel(id)){
	information[Trk::numberOfNextToInnermostPixelLayerOutliers]++;
      }
    } else {
      
      if (msgLvl(MSG::DEBUG)) msg() << "Pixel info storing" << endmsg;

      information[Trk::numberOfPixelHits]++;
      if (m_pixelId->layer_disk(id)==0 && m_pixelId->is_barrel(id)) information[Trk::numberOfInnermostPixelLayerHits]++;
      if (m_pixelId->layer_disk(id)==1 && m_pixelId->is_barrel(id)) information[Trk::numberOfNextToInnermostPixelLayerHits]++;  
      
      if ( ( m_pixelId->is_barrel(id) ) ) {
	if (msgLvl(MSG::DEBUG)) msg() << "Barrel hit" << endmsg;
	int offset = m_pixelId->layer_disk(id); 
	if (!hitPattern.test(offset)) information[Trk::numberOfContribPixelLayers]++;
	hitPattern.set(offset); // assumes numbered consecutively
      } else {
	if (msgLvl(MSG::DEBUG)) msg() << "Endcap hit" << endmsg;
	int offset = static_cast<int> (Trk::pixelEndCap0); //get int value of first pixel endcap disc
	offset    += m_pixelId->layer_disk(id);
	if (!hitPattern.test(offset)) information[Trk::numberOfContribPixelLayers]++;
	hitPattern.set(offset); // assumes numbered consecutively
      }

      if (m_doSharedHits) {
	// used in more than one track ?
	if ( m_assoTool->isShared(*(rot->prepRawData())) ) {
	  if (msgLvl(MSG::DEBUG)) msg() << "shared Pixel hit found" << endmsg;
	  information[Trk::numberOfPixelSharedHits]++;
	  if ( (m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==0) ) {
	    if (msgLvl(MSG::DEBUG)) msg() << "--> shared Pixel hit is in innermost layer" << endmsg;
	    information[Trk::numberOfInnermostPixelLayerSharedHits]++;        
	  } 
	  if ( (m_pixelId->is_barrel(id) && m_pixelId->layer_disk(id)==1) ) {
	    if (msgLvl(MSG::DEBUG)) msg() << "--> shared Pixel hit is in next to innermost layer" << endmsg;
	    information[Trk::numberOfNextToInnermostPixelLayerSharedHits]++;        
	  }
	} else ATH_MSG_DEBUG("shared Pixel hit NOT found");
      }
    }
  } else if (m_useSCT && m_sctId->is_sct(id) ) {
    
    if (msgLvl(MSG::DEBUG)) msg() << "SCT hit found" << endmsg;
    
    if (isOutlier && !ispatterntrack ) { // ME: outliers on pattern tracks may be reintegrated by fitter, so count them as hits    
      if (msgLvl(MSG::DEBUG)) msg() << "SCT outlier info storing" << endmsg;
      information[Trk::numberOfSCTOutliers]++;
    } else {
      if (msgLvl(MSG::DEBUG)) msg() << "SCT info storing" << endmsg;
      information[Trk::numberOfSCTHits]++;
      if ( (m_sctId->is_barrel(id) ) ) {
	if (msgLvl(MSG::DEBUG)) msg() << "Barrel hit" << endmsg;
	int offset = static_cast<int>(Trk::sctBarrel0);
	hitPattern.set( offset ) ;
	if (msgLvl(MSG::DEBUG)) msg() << "offset = " << offset << endmsg;
	if (msgLvl(MSG::DEBUG)) msg() << "m_sctId->layer_disk(id) = " << m_sctId->layer_disk(id) << endmsg;
	//hitPattern.set( offset + m_sctId->layer_disk(id) ); // assumes numbered consecutively
      } else {
	if (msgLvl(MSG::DEBUG)) msg() << "Endcap hit" << endmsg;
	int offset = static_cast<int>(Trk::sctEndCap0); //get int value of first sct endcap disc
	hitPattern.set( offset ) ;
	if (msgLvl(MSG::DEBUG)) msg() << "offset = " << offset << endmsg;
	if (msgLvl(MSG::DEBUG)) msg() << "m_sctId->layer_disk(id) = " << m_sctId->layer_disk(id) << endmsg;
	//hitPattern.set( offset + m_sctId->layer_disk(id) ); // assumes numbered consecutively
      }

      if (m_doSharedHits) {
	// used in more than one track ?
	if ( m_assoTool->isShared(*(rot->prepRawData())) ) {
	  if (msgLvl(MSG::DEBUG)) msg() << "shared SCT hit found" << endmsg;
	    information[Trk::numberOfSCTSharedHits]++;
	} else ATH_MSG_DEBUG("shared SCT hit NOT found");
      }
    }
  } else  ATH_MSG_WARNING("Nor Pixel or SCT used... Check!!");
  
  return;
}

void iFatras::ISF_TrackSummaryHelperTool::analyse(const Trk::Track&, 
						  const Trk::CompetingRIOsOnTrack*, 
						  const Trk::TrackStateOnSurface*,
						  std::vector<int>&, 
						  std::bitset<Trk::numberOfDetectorTypes>&) const
{
  ATH_MSG_DEBUG("analyse not implemented for Trk::CompetingRIOsOnTrack !!");
  return;
}

void iFatras::ISF_TrackSummaryHelperTool::searchForHoles(const Trk::Track&, 
							 std::vector<int>&,
							 const Trk::ParticleHypothesis) const {
  ATH_MSG_DEBUG("searchForHoles not implemented!!");
  return;
}

void iFatras::ISF_TrackSummaryHelperTool::addDetailedTrackSummary(const Trk::Track&, Trk::TrackSummary&) const {
  ATH_MSG_DEBUG("addDetailedTrackSummary not implemented!!");
  return;
}

void iFatras::ISF_TrackSummaryHelperTool::updateSharedHitCount(const Trk::Track &, Trk::TrackSummary&) const {
  ATH_MSG_DEBUG("updateSharedHitCount not implemented !!");
  return;
}

void iFatras::ISF_TrackSummaryHelperTool::updateExpectedHitInfo(const Trk::Track&, Trk::TrackSummary&) const {
  ATH_MSG_DEBUG("updateExpectedHitInfo not implemented !!");
  return;
}

void iFatras::ISF_TrackSummaryHelperTool::updateAdditionalInfo(Trk::TrackSummary&, std::vector<float>&, float&, int&, int&) const {
  ATH_MSG_DEBUG("updateAdditionalInfo not implemented !!");
  return;
}

// ====================================================================================================================

StatusCode iFatras::ISF_TrackSummaryHelperTool::finalize() {
   return StatusCode::SUCCESS;
}

