/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysHitDecoratorTool.cxx
 * @author shaun roe
**/

#include "InDetPhysHitDecoratorTool.h"
#include "xAODTracking/TrackParticle.h"
//#include "GeneratorUtils/PIDUtils.h"
#include "TrkParameters/TrackParameters.h" //Contains typedef to Trk::CurvilinearParameters
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkTrack/TrackCollection.h"
//for the identifiers
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
//
#include <tuple>
#include <limits>

//ref: ​https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/TrkParametersBase/trunk/TrkParametersBase/CurvilinearParametersT.h


InDetPhysHitDecoratorTool::InDetPhysHitDecoratorTool(const std::string& type,const std::string& name,const IInterface* parent):
AthAlgTool(type,name,parent), 
m_holeSearchTool("InDet::InDetTrackHoleSearchTool"),
m_updatorHandle("Trk::KalmanUpdator/TrkKalmanUpdator"),
m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
m_ptThreshold(0.8), m_isUnbiased(false), m_doUpgrade(false){
declareInterface<IInDetPhysValDecoratorTool>(this);
//do I need to retrieve the 'Tracks' container?
}

InDetPhysHitDecoratorTool::~InDetPhysHitDecoratorTool (){
	//delete m_idHelper;m_idHelper=0; doesnt even compile if you do this
}

StatusCode 
InDetPhysHitDecoratorTool::initialize(){
	StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;
  if ( m_holeSearchTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_holeSearchTool << endreq;
    return StatusCode::FAILURE;
  }
  if ( not (m_updatorHandle.empty()) and  m_updatorHandle.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve measurement updator tool: "<< m_updatorHandle << endreq;
    return StatusCode::FAILURE;
  }
  
  if ( not (m_holeSearchTool.empty()) and  m_holeSearchTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve m_holeSearchTool: "<< m_holeSearchTool << endreq;
    return StatusCode::FAILURE;
  }
  
   //ID Helper
  m_idHelper = new AtlasDetectorID;

  // Get the dictionary manager from the detector store
  sc = detStore()->retrieve(m_idHelper, "AtlasID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get ID helper !" << endreq;
    return sc;
  }
  
  sc = detStore()->retrieve(m_pixelID, "PixelID");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel Detector ID helper!" << endreq;
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_sctID, "SCT_ID");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not get SCT Detector ID helper!" << endreq;
    return StatusCode::FAILURE;
  }
  if (!m_doUpgrade) {
    sc = detStore()->retrieve(m_trtID, "TRT_ID");
    if(sc.isFailure()) {
      msg(MSG::FATAL) << "Could not get TRT Detector ID helper!" << endreq;
      return StatusCode::FAILURE;
    }
  }
  if (m_residualPullCalculator.empty()) {
    ATH_MSG_INFO("No residual/pull calculator for general hit residuals configured.");
    ATH_MSG_INFO("It is recommended to give R/P calculators to the det-specific tool handle lists then.");
  } else if (m_residualPullCalculator.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve "<< m_residualPullCalculator<<" (to calculate residuals and pulls) "<< endreq;
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Generic hit residuals&pulls will be calculated in one or both available local coordinates");
  }
	return sc;
}

StatusCode 
InDetPhysHitDecoratorTool::finalize  (){
	StatusCode sc(StatusCode::SUCCESS);
	return sc;
}

bool
InDetPhysHitDecoratorTool::decorateTrack(const xAOD::TrackParticle & particle, const std::string& prefix){
	typedef std::tuple<int, float, float, float, float, int> SingleResult_t;
	typedef std::vector<SingleResult_t> TrackResult_t;
	const float invalidFloat(std::numeric_limits<float>::quiet_NaN());
	//const float invalidDouble(std::numeric_limits<double>::quiet_NaN());
	const float invalidRes(invalidFloat), invalidPull(invalidFloat);
	const int invalidLayer(-1);
	const int invalidWidth(-1);
	const SingleResult_t invalidResult=std::make_tuple(invalidLayer, invalidRes, invalidPull,invalidRes,invalidPull,invalidWidth);
  //get element link to the original track
  const ElementLink< TrackCollection >& trackLink = particle.trackLink();//using xAODTracking-00-03-09, interface has changed later
  unsigned int count(0);
  if (trackLink.isValid()){
    ATH_MSG_VERBOSE ("Track link found " );
    const double pt = particle.pt(); 
    if (pt > m_ptThreshold){
    ATH_MSG_VERBOSE ("pt is over threshold " );
			std::auto_ptr<const Trk::Track> trackWithHoles(m_holeSearchTool->getTrackWithHoles(**trackLink));
			const int numberOfHits((trackWithHoles->trackStateOnSurfaces())->size());
			ATH_MSG_VERBOSE ("number of Hits "<<numberOfHits );
			TrackResult_t result; result.reserve(numberOfHits);
			for (auto thisTrackState: *(trackWithHoles->trackStateOnSurfaces())){
				SingleResult_t thisResult(invalidResult);
				const Trk::MeasurementBase* mesb=thisTrackState->measurementOnTrack();
				if (not mesb) {
				  ATH_MSG_INFO ("intermediate mesb is NULL");
				} else {
				  ATH_MSG_INFO ("intermediate mesb is ok");
				}
				const Trk::RIO_OnTrack* hit = mesb ? dynamic_cast<const Trk::RIO_OnTrack*>(mesb) : 0;
				const Trk::TrackParameters* biasedTrackParameters = thisTrackState->trackParameters();
				if (biasedTrackParameters) ATH_MSG_INFO ("biased track parameters ok");
				Identifier surfaceID;
				if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
          surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
          ATH_MSG_VERBOSE("Surface ID found");
        } else { // holes, perigee
          if (not biasedTrackParameters ) {
            msg(MSG::DEBUG) << "The track parameters are not valid for this track state (the pointer is null)" << endreq;
            continue;
          }
          surfaceID = biasedTrackParameters->associatedSurface().associatedDetectorElementIdentifier();
          ATH_MSG_VERBOSE("Surface ID found for holes etc");

        }
				
				if (mesb && biasedTrackParameters) {
					ATH_MSG_VERBOSE("mesb and track parameters are ok");
					const Trk::TrackParameters *trackParameters = (! thisTrackState->type(Trk::TrackStateOnSurface::Outlier)) ? getUnbiasedTrackParameters(biasedTrackParameters,mesb) : biasedTrackParameters;
				  Trk::ResidualPull::ResidualType resType = (m_isUnbiased) ? (Trk::ResidualPull::Unbiased):(Trk::ResidualPull::Biased);
				  const std::auto_ptr<const Trk::ResidualPull> residualPull(m_residualPullCalculator->residualPull(hit,trackParameters,resType));
				  float residualLocX = 1000.*residualPull->residual()[Trk::loc1]; // residuals in microns
				  float pullLocX = residualPull->pull()[Trk::loc1];
				  float residualLocY,  pullLocY;//should make NaN in initialization
				  if (residualPull->dimension() > 1){
				  	residualLocY = 1000.*residualPull->residual()[Trk::loc2];
				  	pullLocY = residualPull->pull()[Trk::loc2];
				  } else {
				  	residualLocY= invalidFloat;
				  	pullLocY = invalidFloat;
				  }
				  Subdetector det(INVALID_DETECTOR);
				  Region r(INVALID_REGION);
				  int iLayer(invalidLayer);
				  const bool successfulIdentification = decideDetectorRegion(surfaceID, det, r, iLayer);
				  if (not successfulIdentification) {
				    ATH_MSG_DEBUG("Could not identify surface");
				  	continue;
				  }
				  int width = 1;
				  thisResult=std::make_tuple(iLayer, residualLocX, pullLocX, residualLocY, pullLocY, width);
				  ATH_MSG_DEBUG ("result "<<iLayer<<", "<<residualLocX<<", "<<pullLocX<<", "<<residualLocY<<", "<<pullLocY<<", "<<width );
				  result.push_back(thisResult);
				  ++count;
				  //must delete the pointers?
				} else {
					if (not mesb) ATH_MSG_INFO("mesb not ok");
					if (not biasedTrackParameters) ATH_MSG_INFO("biasedTrackParameters were not found");
				}
			}//end of for loop
			particle.auxdecor<TrackResult_t>(prefix+"hitResiduals") = result;
			return true;
		} 
	} else {
	  ATH_MSG_DEBUG ("No valid track link found " );
	}
	return false;
}

bool
InDetPhysHitDecoratorTool::decideDetectorRegion(const Identifier & id, Subdetector & det, Region & r, int & /*layer*/){
	bool success(false);
	const int normalBarrel(0);
	const int upgradedBarrel(1);
	const int normalTrtBarrel(1);
	det = INVALID_DETECTOR;//default
	r = INVALID_REGION;
	int bec(-100);
	
	//following the logic in the original code, should be reviewed!
	if (m_idHelper->is_pixel(id)) det=PIXEL;
	if (m_idHelper->is_sct(id)) det=SCT;
	if (not m_doUpgrade and m_idHelper->is_trt(id)) det=TRT;
	//
	if (det==PIXEL) {
		bec = abs(m_pixelID->barrel_ec(id));
		r= (bec==normalBarrel)?(BARREL):(ENDCAP);
		if (m_pixelID->layer_disk(id) == 0) det=BLAYER;
	}
	if (det==SCT) {
		bec = abs(m_sctID->barrel_ec(id));
		if (not m_doUpgrade){
		  r = (bec==normalBarrel)?(BARREL):(ENDCAP);
		} else {
			r = (bec==upgradedBarrel)?(BARREL):(ENDCAP);
		}
	}
	if (det==TRT) {
		bec = abs(m_trtID->barrel_ec(id));
		r = (bec==normalTrtBarrel)?(BARREL):(ENDCAP);
	}
	success = (det != INVALID_DETECTOR) and (r != INVALID_REGION);
	
	return success;
}

const Trk::TrackParameters*
InDetPhysHitDecoratorTool::getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters, const Trk::MeasurementBase* measurement ) {
	static bool alreadyWarned(false);
	const Trk::TrackParameters* unbiasedTrkParameters(0);
	if (!m_updatorHandle.empty() && (m_isUnbiased) ) {
    if ( trkParameters->covariance() ) {
      // Get unbiased state
      unbiasedTrkParameters =m_updatorHandle->removeFromState( *trkParameters,measurement->localParameters(),measurement->localCovariance());
      if (!unbiasedTrkParameters) {
    		msg(MSG::WARNING) << "Could not get unbiased track parameters, use normal parameters" << endreq;
    		m_isUnbiased = false;
      }
    } else if (not alreadyWarned) {
      // warn only once!
      msg(MSG::WARNING) << "TrackParameters contain no covariance, unbiased track states can not be calculated (ie. pulls and residuals will be too small)" << endreq;
      alreadyWarned = true;
      m_isUnbiased = false;
    } else {
      m_isUnbiased = false;
    }// end if no measured track parameter
  }
  return unbiasedTrkParameters;                                 
}

  /**
for (; TSOSItr != trackWithHoles->trackStateOnSurfaces()->end(); ++TSOSItr) {
	const Trk::MeasurementBase* mesb=(*TSOSItr)->measurementOnTrack();
        const Trk::RIO_OnTrack* hit = mesb ? dynamic_cast<const Trk::RIO_OnTrack*>(mesb) : 0;
	const Trk::TrackParameters* biasedTrackParameters = (*TSOSItr)->trackParameters();
                
	if (mesb && biasedTrackParameters) {
		const Trk::TrackParameters *trackParameters = (!(*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier)) ?getUnbiasedTrackParameters(biasedTrackParameters,mesb) : biasedTrackParameters;
	
		Trk::ResidualPull::ResidualType resType = (m_isUnbiased) ? (Trk::ResidualPull::Unbiased):(Trk::ResidualPull::Biased);
		const auto_ptr<const Trk::ResidualPull> residualPull(m_residualPullCalculator->residualPull(hit,trackParameters,resType));
		residualLocX = 1000*residualPull->residual()[Trk::loc1]; // residuals in microns
		m_residualx_pixel_barrel->Fill(residualLocX);
	}
}
}      
        **/
    
  





/**
const Trk::TrackParameters*
IDStandardPerformance::getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters
                                                , const Trk::MeasurementBase* measurement ) {
  const Trk::TrackParameters *unbiasedTrkParameters = 0;

  // ------------------------------------
  // try to get measured track parameters
  // ------------------------------------

  //const Trk::MeasuredTrackParameters *measuredTrkParameters =
  //dynamic_cast<const Trk::MeasuredTrackParameters*>(trkParameters);

  if (!m_updatorHandle.empty() && (m_isUnbiased==1) ) {
    if ( trkParameters->covariance() ) {
      // Get unbiased state
      unbiasedTrkParameters =
    m_updatorHandle->removeFromState( *trkParameters,
                    measurement->localParameters(),
                    measurement->localCovariance());

      if (!unbiasedTrkParameters) {
    msg(MSG::WARNING) << "Could not get unbiased track parameters, "
        <<"use normal parameters" << endreq;
    m_isUnbiased = 0;
      }
    } else if (!m_UpdatorWarning) {
      // warn only once!
      msg(MSG::WARNING) << "TrackParameters contain no covariance: "
      <<"Unbiased track states can not be calculated "
      <<"(ie. pulls and residuals will be too small)" << endreq;
      m_UpdatorWarning = true;
      m_isUnbiased = 0;
    } else {
      m_isUnbiased = 0;
    }// end if no measured track parameter
  }
  return unbiasedTrkParameters;
}
**/
