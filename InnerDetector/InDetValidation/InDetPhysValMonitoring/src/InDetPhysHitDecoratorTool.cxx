/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysHitDecoratorTool.cxx
 * @author shaun roe
 **/

#include "InDetPhysHitDecoratorTool.h"
#include "safeDecorator.h"
#include "xAODTracking/TrackParticle.h"
// #include "GeneratorUtils/PIDUtils.h"
#include "TrkParameters/TrackParameters.h" // Contains typedef to Trk::CurvilinearParameters
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkTrack/TrackCollection.h"
// for the identifiers
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetPrepRawData/SiCluster.h"
//
#include <tuple>
#include <limits>

// ref:
// ​https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/TrkParametersBase/trunk/TrkParametersBase/CurvilinearParametersT.h


InDetPhysHitDecoratorTool::InDetPhysHitDecoratorTool(const std::string& type, const std::string& name,
                                                     const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_holeSearchTool("InDet::InDetTrackHoleSearchTool"),
  m_updatorHandle("Trk::KalmanUpdator/TrkKalmanUpdator"),
  m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
  m_ptThreshold(0.8), m_isUnbiased(true), m_doUpgrade(false),
  m_idHelper(nullptr),
  m_pixelID(nullptr),
  m_sctID(nullptr),
  m_trtID(nullptr) {
  declareInterface<IInDetPhysValDecoratorTool>(this);
  declareProperty("InDetTrackHoleSearchTool", m_holeSearchTool);
  declareProperty("Updator", m_updatorHandle);
  declareProperty("ResidualPullCalculator", m_residualPullCalculator);
  // do I need to retrieve the 'Tracks' container?
}

InDetPhysHitDecoratorTool::~InDetPhysHitDecoratorTool () {
// nop
}

StatusCode
InDetPhysHitDecoratorTool::initialize() {
  ATH_CHECK(m_holeSearchTool.retrieve());
  if (not (m_updatorHandle.empty())) {
    ATH_CHECK(m_updatorHandle.retrieve());
  }
  if (not (m_holeSearchTool.empty())) {
    ATH_CHECK(m_holeSearchTool.retrieve());
  }

  // ID Helper
  m_idHelper = new AtlasDetectorID;

  // Get the dictionary manager from the detector store
  ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));
  ATH_CHECK(detStore()->retrieve(m_pixelID, "PixelID"));
  ATH_CHECK(detStore()->retrieve(m_sctID, "SCT_ID"));

  if (!m_doUpgrade) {
    ATH_CHECK(detStore()->retrieve(m_trtID, "TRT_ID"));
  }
  if (m_residualPullCalculator.empty()) {
    ATH_MSG_INFO("No residual/pull calculator for general hit residuals configured.");
    ATH_MSG_INFO("It is recommended to give R/P calculators to the det-specific tool handle lists then.");
  } else if (m_residualPullCalculator.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve " << m_residualPullCalculator << " (to calculate residuals and pulls) " <<
      endmsg;
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Generic hit residuals & pulls will be calculated in one or both available local coordinates");
  }
  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysHitDecoratorTool::finalize() {
  return StatusCode::SUCCESS;
}

bool
InDetPhysHitDecoratorTool::decorateTrack(const xAOD::TrackParticle& particle, const std::string& prefix) {
  static int trackNumber(0);

  typedef std::tuple<int, int, int, float, float, float, float, int, int, int> SingleResult_t;
  typedef std::vector<SingleResult_t> TrackResult_t;
  const float invalidFloat(-1);
  // const float invalidDouble(std::numeric_limits<double>::quiet_NaN());
  const float invalidRes(invalidFloat), invalidPull(invalidFloat);
  const int invalidDetector(-1);
  const int invalidRegion(-1);
  const int invalidLayer(-1);
  const int invalidWidth(-1);
  const int invalidMeasure(-1);
  const SingleResult_t invalidResult = std::make_tuple(invalidDetector, invalidRegion, invalidLayer, invalidRes,
                                                       invalidPull, invalidRes, invalidPull, invalidWidth, invalidWidth,
                                                       invalidMeasure);
  // get element link to the original track
  const ElementLink< TrackCollection >& trackLink = particle.trackLink();// using xAODTracking-00-03-09, interface has
                                                                         // changed later
  if (trackLink.isValid()) {
    ATH_MSG_VERBOSE("Track link found ");
    const double pt = particle.pt();
    if (pt > m_ptThreshold) {
      ATH_MSG_VERBOSE("pt is over threshold ");
      std::unique_ptr<const Trk::Track> trackWithHoles(m_holeSearchTool->getTrackWithHoles(**trackLink));
      const auto& allTrackStates = *(trackWithHoles->trackStateOnSurfaces());
      const int numberOfHits(allTrackStates.size());
      unsigned int trackParametersCounter(numberOfHits);
      TrackResult_t result;
      result.reserve(numberOfHits);
      // line 3595 original
      if (!m_updatorHandle.empty()) {
        m_isUnbiased = true;
      } else {
        ATH_MSG_WARNING("The updater handle is empty, now using biased estimators");
        m_isUnbiased = false;
      }
      ATH_MSG_DEBUG("Num. track states in track " << ++trackNumber << ": " << allTrackStates.size());

      for (const auto& thisTrackState: allTrackStates) {
        // Copy logic from InDetRttPerformance to get hits/outliers/holes
        // Variable specifying measurement type filled
        SingleResult_t thisResult(invalidResult);
        if (not thisTrackState) { // is this check needed?
          msg(MSG::ERROR) << "TSOS is NULL" << (thisTrackState) << endmsg;
          continue;
        }
        Identifier surfaceID;
        const Trk::MeasurementBase* mesb = (thisTrackState)->measurementOnTrack();
        const Trk::RIO_OnTrack* hit = mesb ? dynamic_cast<const Trk::RIO_OnTrack*>(mesb) : nullptr;
        if (mesb && !hit) {
          continue; // skip pseudomeasurements
        }
        // Get surfaceID, different for measuremnt hits & outliers, and holes
        if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
          surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
        } // Holes
        else {
          if (not (thisTrackState)->trackParameters()) {
            msg(MSG::INFO) << "TSOS surface is NULL" << endmsg;
            continue;
          }
          surfaceID = (thisTrackState)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
        }
        bool isMesb = (thisTrackState)->type(Trk::TrackStateOnSurface::Measurement);
        bool isOutl = (thisTrackState)->type(Trk::TrackStateOnSurface::Outlier);
        bool isHole = (thisTrackState)->type(Trk::TrackStateOnSurface::Hole);

        int measureType = -1;
        if (isMesb) {
          measureType = 0;
        }
        if (isOutl) {
          measureType = 1;
        }
        if (isHole) {
          measureType = 2;
        }

        bool anyHit = isMesb || isOutl || isHole;
        if (!anyHit) {
          continue;
        }
        Subdetector det(INVALID_DETECTOR);
        Region r(INVALID_REGION);
        int iLayer(invalidLayer);
        const bool successfulIdentification = decideDetectorRegion(surfaceID, det, r, iLayer);
        if (not successfulIdentification) {
          ATH_MSG_DEBUG("Could not identify surface");
          continue;
        }
        // Get residuals - old code, remains the same?
        // define residuals at -1 if no measurement (better way?)
        float residualLocY(invalidFloat), pullLocY(invalidFloat);// NaN by default
        float residualLocX = -1, pullLocX = -1; // what values?
        int phiWidth(-1);
        int etaWidth(-1);
        std::unique_ptr<const Trk::ResidualPull> residualPull(nullptr);
        const Trk::TrackParameters* biasedTrackParameters = thisTrackState->trackParameters();
        if (biasedTrackParameters) {
          ATH_MSG_VERBOSE("biased track parameters ok");
        }
        ATH_MSG_VERBOSE("checking mesb and track parameters");
        if (mesb && biasedTrackParameters) {
          ATH_MSG_DEBUG("mesb and biased track parameters are ok");
          // for outliers, the measurement is not part of the fit, so track parameters are already unbiased
          std::unique_ptr<const Trk::TrackParameters> cleanup_trackparam;
          const Trk::TrackParameters* trackParameters =
            (!thisTrackState->type(Trk::TrackStateOnSurface::Outlier)) ? getUnbiasedTrackParameters(
              biasedTrackParameters,
              mesb) :
            biasedTrackParameters;
          if (trackParameters != biasedTrackParameters) {
            cleanup_trackparam.reset(trackParameters);
          }
          if (not trackParameters) {
            ATH_MSG_DEBUG("unbiased track parameters pointer is NULL");
          }
          Trk::ResidualPull::ResidualType resType =
            (m_isUnbiased) ? (Trk::ResidualPull::Unbiased) : (Trk::ResidualPull::Biased);

          residualPull.reset(m_residualPullCalculator->residualPull(hit, trackParameters, resType));
          ATH_MSG_VERBOSE("checking residual pull");
          if (not residualPull) {
            ATH_MSG_DEBUG("residualPull is NULL");
            continue;
          }
          ATH_MSG_DEBUG("residualPull is OK");
          // around line 4058 in original code
          residualLocX = 1000. * residualPull->residual()[Trk::loc1]; // residuals in microns
          pullLocX = residualPull->pull()[Trk::loc1];
          if (residualPull->dimension() > 1) {
            residualLocY = 1000. * residualPull->residual()[Trk::loc2];
            pullLocY = residualPull->pull()[Trk::loc2];
          }
          // Unbiased residuals?!
          measureType = 4;

          // int width = 1; //check original code
          // copy-paste from original
          if (hit && m_isUnbiased) {
            // Cluster width determination
            if ((det == L0PIXBARR)or(det == PIXEL) or(det == SCT)) {
              const InDet::SiCluster* pCluster = dynamic_cast <const InDet::SiCluster*>(hit->prepRawData());
              if (pCluster) {
                InDet::SiWidth width = pCluster->width();
                phiWidth = int(width.colRow().x());
                etaWidth = int(width.colRow().y());
              }
            }
            ATH_MSG_VERBOSE("hit and isUnbiased ok");
          }
          // end copy-paste
          // must delete the pointers?
        } else {
          if (not mesb) {
            ATH_MSG_VERBOSE("mesb not ok");
          }
          if (not biasedTrackParameters) {
            ATH_MSG_VERBOSE("biasedTrackParameters were not found");
          }
          --trackParametersCounter;
        }
        thisResult = std::make_tuple(det, r, iLayer, residualLocX, pullLocX, residualLocY, pullLocY, phiWidth, etaWidth,
                                     measureType);
        result.push_back(thisResult);
      }// end of for loop*/
      ATH_MSG_DEBUG(
        "Out of " << numberOfHits << " hits, " << trackParametersCounter << " had track params, and " << result.size() <<
          " had residuals.");
      if (not result.empty()) {
//      particle.auxdecor<TrackResult_t>(prefix+"hitResiduals") = result; //!< no dictionary for tuple
        const unsigned int arraySize = result.size();
        std::vector<int> result_det;
        result_det.reserve(arraySize);
        std::vector<int> result_r;
        result_r.reserve(arraySize);
        std::vector<int> result_iLayer;
        result_iLayer.reserve(arraySize);
        std::vector<float> result_residualLocX;
        result_residualLocX.reserve(arraySize);
        std::vector<float> result_pullLocX;
        result_pullLocX.reserve(arraySize);
        std::vector<float> result_residualLocY;
        result_residualLocY.reserve(arraySize);
        std::vector<float> result_pullLocY;
        result_pullLocY.reserve(arraySize);
        std::vector<int> result_phiWidth;
        result_phiWidth.reserve(arraySize);
        std::vector<int> result_etaWidth;
        result_etaWidth.reserve(arraySize);
        std::vector<int> result_measureType;
        result_measureType.reserve(arraySize);
        for (const SingleResult_t& single_result : result) {
          result_det.push_back(std::get<0>(single_result));
          result_r.push_back(std::get<1>(single_result));
          result_iLayer.push_back(std::get<2>(single_result));
          result_residualLocX.push_back(std::get<3>(single_result));
          result_pullLocX.push_back(std::get<4>(single_result));
          result_residualLocY.push_back(std::get<5>(single_result));
          result_pullLocY.push_back(std::get<6>(single_result));
          result_phiWidth.push_back(std::get<7>(single_result));
          result_etaWidth.push_back(std::get<8>(single_result));
          result_measureType.push_back(std::get<9>(single_result));
        }
        particle.auxdecor<std::vector<int> >(prefix + "measurement_region") = result_r;
        particle.auxdecor<std::vector<int> >(prefix + "measurement_det") = result_det;
        particle.auxdecor<std::vector<int> >(prefix + "measurement_iLayer") = result_iLayer;
        particle.auxdecor<std::vector<float> >(prefix + "hitResiduals_residualLocX") = result_residualLocX;
        particle.auxdecor<std::vector<float> >(prefix + "hitResiduals_pullLocX") = result_pullLocX;
        particle.auxdecor<std::vector<float> >(prefix + "hitResiduals_residualLocY") = result_residualLocY;
        particle.auxdecor<std::vector<float> >(prefix + "hitResiduals_pullLocY") = result_pullLocY;
        particle.auxdecor<std::vector<int> >(prefix + "hitResiduals_phiWidth") = result_phiWidth;
        particle.auxdecor<std::vector<int> >(prefix + "hitResiduals_etaWidth") = result_etaWidth;
        particle.auxdecor<std::vector<int> >(prefix + "measurement_type") = result_measureType;
        return true;
      }
    }
  } else {
    ATH_MSG_DEBUG("No valid track link found ");
  }
  return false;
}

bool
InDetPhysHitDecoratorTool::decideDetectorRegion(const Identifier& id, Subdetector& det, Region& r, int& layer) {
  bool success(false);
  const int normalBarrel(0);
  const int upgradedBarrel(1);
  const int normalTrtBarrel(1);
  const int dbm(4);

  det = INVALID_DETECTOR;// default
  r = INVALID_REGION;
  int bec(-100);

  // following the logic in the original code, should be reviewed!
  if (m_idHelper->is_pixel(id)) {
    bec = abs(m_pixelID->barrel_ec(id));
    if (bec == dbm) {
      det = DBM;
    } else {
      det = PIXEL;
    }
  }
  if (m_idHelper->is_sct(id)) {
    det = SCT;
  }
  if (not m_doUpgrade and m_idHelper->is_trt(id)) {
    det = TRT;
  }
  //
  // check this specifically
  if (det == PIXEL) {
    bec = abs(m_pixelID->barrel_ec(id));
    r = (bec == normalBarrel) ? (BARREL) : (ENDCAP);
    layer = m_pixelID->layer_disk(id);
    if (BARREL == r and layer == 0) {
      det = L0PIXBARR;
    }
  }
  if (det == DBM) {
    r = (bec < 0) ? (BARREL) : (ENDCAP);
  }
  if (det == SCT) {
    bec = abs(m_sctID->barrel_ec(id));
    if (not m_doUpgrade) {
      r = (bec == normalBarrel) ? (BARREL) : (ENDCAP);
    } else {
      r = (bec == upgradedBarrel) ? (BARREL) : (ENDCAP);
    }
    layer = m_sctID->layer_disk(id);
  }
  if (det == TRT) {
    bec = abs(m_trtID->barrel_ec(id));
    r = (bec == normalTrtBarrel) ? (BARREL) : (ENDCAP);
    layer = m_trtID->layer_or_wheel(id);
  }
  success = (det != INVALID_DETECTOR)and(r != INVALID_REGION);

  return success;
}

const Trk::TrackParameters*
InDetPhysHitDecoratorTool::getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters,
                                                      const Trk::MeasurementBase* measurement) {
  static bool alreadyWarned(false);
  const Trk::TrackParameters* unbiasedTrkParameters(trkParameters);

  if (!m_updatorHandle.empty() && (m_isUnbiased)) {
    if (trkParameters->covariance()) {
      // Get unbiased state
      unbiasedTrkParameters = m_updatorHandle->removeFromState(*trkParameters,
                                                               measurement->localParameters(),
                                                               measurement->localCovariance());
      if (!unbiasedTrkParameters) {
        msg(MSG::INFO) << "Could not get unbiased track parameters, use normal parameters" << endmsg;
        m_isUnbiased = false;
      }
    } else if (not alreadyWarned) {
      // warn only once!
      msg(MSG::WARNING) <<
        "TrackParameters contain no covariance, unbiased track states can not be calculated (ie. pulls and residuals will be too small)"
                        << endmsg;
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
   const Trk::TrackParameters *trackParameters = (!(*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier))
      ?getUnbiasedTrackParameters(biasedTrackParameters,mesb) : biasedTrackParameters;

   Trk::ResidualPull::ResidualType resType = (m_isUnbiased) ? (Trk::ResidualPull::Unbiased):(Trk::ResidualPull::Biased);
   const auto_ptr<const Trk::ResidualPull>
      residualPull(m_residualPullCalculator->residualPull(hit,trackParameters,resType));
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
        <<"use normal parameters" << endmsg;
    m_isUnbiased = 0;
      }
    } else if (!m_UpdatorWarning) {
      // warn only once!
      msg(MSG::WARNING) << "TrackParameters contain no covariance: "
      <<"Unbiased track states can not be calculated "
      <<"(ie. pulls and residuals will be too small)" << endmsg;
      m_UpdatorWarning = true;
      m_isUnbiased = 0;
    } else {
      m_isUnbiased = 0;
    }// end if no measured track parameter
   }
   return unbiasedTrkParameters;
   }
 **/
