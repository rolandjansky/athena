/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysHitDecoratorAlg.cxx
 * @author shaun roe
 **/

#include "InDetPhysHitDecoratorAlg.h"
#include "safeDecorator.h"
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


InDetPhysHitDecoratorAlg::InDetPhysHitDecoratorAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name,pSvcLocator),
  m_holeSearchTool("InDet::InDetTrackHoleSearchTool"),
  m_updatorHandle("Trk::KalmanUpdator/TrkKalmanUpdator"),
  m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
  m_ptThreshold(0.8), m_doUpgrade(false),
  m_idHelper(nullptr),
  m_pixelID(nullptr),
  m_sctID(nullptr),
  m_trtID(nullptr) {
  declareProperty("InDetTrackHoleSearchTool", m_holeSearchTool);
  declareProperty("Updator", m_updatorHandle);
  declareProperty("ResidualPullCalculator", m_residualPullCalculator);
  // do I need to retrieve the 'Tracks' container?
}

InDetPhysHitDecoratorAlg::~InDetPhysHitDecoratorAlg () {
// nop
}

StatusCode
InDetPhysHitDecoratorAlg::initialize() {
  ATH_CHECK(m_holeSearchTool.retrieve());
  if (not (m_updatorHandle.empty())) {
    ATH_CHECK(m_updatorHandle.retrieve());
  }
  if (not (m_holeSearchTool.empty())) {
    ATH_CHECK(m_holeSearchTool.retrieve());
  }

  std::vector<std::string> float_decor_names(kNFloatDecorators);
  std::vector<std::string> int_decor_names(kNIntDecorators);
  int_decor_names[kDecorRegion]="measurement_region";
  int_decor_names[kDecorDet]="measurement_det";
  int_decor_names[kDecorILayer]="measurement_iLayer";
  int_decor_names[kDecorType]="measurement_type";

  float_decor_names[kDecorResidualLocX]="hitResiduals_residualLocX";
  float_decor_names[kDecorPullLocX]="hitResiduals_pullLocX";
  float_decor_names[kDecorResidualLocY]="hitResiduals_residualLocY";
  float_decor_names[kDecorPullLocY]="hitResiduals_pullLocY";

  int_decor_names[kDecorPhiWidth]="hitResiduals_phiWidth";
  int_decor_names[kDecorEtaWidth]="hitResiduals_etaWidth";

  ATH_CHECK( m_trkParticleName.initialize() );
  IDPVM::createDecoratorKeys(*this,m_trkParticleName,m_prefix,float_decor_names,m_floatDecor);
  
  IDPVM::createDecoratorKeys(*this,m_trkParticleName,m_prefix, int_decor_names, m_intDecor);
  assert( m_intDecor.size() == kNIntDecorators);
  assert( m_floatDecor.size() == kNFloatDecorators);

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
    ATH_MSG_FATAL( "Could not retrieve " << m_residualPullCalculator << " (to calculate residuals and pulls) " );
  } else {
    ATH_MSG_INFO("Generic hit residuals & pulls will be calculated in one or both available local coordinates");
  }
  return StatusCode::SUCCESS;
}

StatusCode
InDetPhysHitDecoratorAlg::finalize() {
  return StatusCode::SUCCESS;
}

// to migrate to AthReentrantAlgorithm later
StatusCode
InDetPhysHitDecoratorAlg::execute(const EventContext &ctx) const {
  SG::ReadHandle<xAOD::TrackParticleContainer> ptracks(m_trkParticleName, ctx);
  if ((not ptracks.isValid())) {
    ATH_MSG_ERROR("Cannot get ReadHandle " << m_trkParticleName);
    return StatusCode::FAILURE;
  }

  std::vector< SG::WriteDecorHandle<xAOD::TrackParticleContainer,std::vector<float> > >
    float_decor( IDPVM::createDecorators<xAOD::TrackParticleContainer, std::vector<float> >(m_floatDecor, ctx) );
  std::vector< SG::WriteDecorHandle<xAOD::TrackParticleContainer,std::vector<int> > >
    int_decor( IDPVM::createDecorators<xAOD::TrackParticleContainer,std::vector<int> >(m_intDecor, ctx) );

  for (const xAOD::TrackParticle *trk_particle : *ptracks) {
    if (not decorateTrack(*trk_particle, float_decor, int_decor) ) {
      ATH_MSG_ERROR("Could not decorate track");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

bool
InDetPhysHitDecoratorAlg::decorateTrack(const xAOD::TrackParticle &particle,
                                         std::vector< SG::WriteDecorHandle<xAOD::TrackParticleContainer,std::vector<float> > > &float_decor,
                                         std::vector< SG::WriteDecorHandle<xAOD::TrackParticleContainer,std::vector<int> > > &int_decor) const
{
  int trackNumber(0);

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
  bool isUnbiased(true);
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
        isUnbiased = true;
      } else {
        ATH_MSG_WARNING("The updater handle is empty, now using biased estimators");
        isUnbiased = false;
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
        float residualLocY(invalidFloat), pullLocY(invalidFloat);// -1 by default
        float residualLocX = invalidFloat, pullLocX = invalidFloat; // what values?
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
              mesb, isUnbiased) :
            biasedTrackParameters;
          if (trackParameters != biasedTrackParameters) {
            cleanup_trackparam.reset(trackParameters);
          }
          if (not trackParameters) {
            ATH_MSG_DEBUG("unbiased track parameters pointer is NULL");
          }
          Trk::ResidualPull::ResidualType resType =
            (isUnbiased) ? (Trk::ResidualPull::Unbiased) : (Trk::ResidualPull::Biased);

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
          if (hit && isUnbiased) {
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
        int_decor[kDecorRegion](particle)=result_r;
        int_decor[kDecorDet](particle) = result_det;
        int_decor[kDecorILayer](particle) = result_iLayer;
        //IDPVM::decorateOrRejectQuietly(particle,float_decor[kDecorResidualLocX],result_residualLocX);
        float_decor[kDecorResidualLocX](particle) = result_residualLocX;
        float_decor[kDecorResidualLocY](particle) = result_residualLocY;
        float_decor[kDecorPullLocX](particle) = result_pullLocX;
        float_decor[kDecorPullLocY](particle) = result_pullLocY;
        int_decor[kDecorPhiWidth](particle) = result_phiWidth;
        int_decor[kDecorEtaWidth](particle) = result_etaWidth;
        int_decor[kDecorType](particle) = result_measureType;
        return true;
      }
    }
    else {
       // particle below pt threshold for decoration. Since this is not an error now "true" is returned.
       // If "false" is returned the job would be aborted.
       return true;
    }
  } else {
    ATH_MSG_ERROR("No valid track link found ");
  }
  return false;
}

bool
InDetPhysHitDecoratorAlg::decideDetectorRegion(const Identifier& id, Subdetector& det, Region& r, int& layer) const {
  bool success(false);
  const int normalBarrel(0);
  const int upgradedBarrel(1);
  const int normalTrtBarrel(1);

  det = INVALID_DETECTOR;// default
  r = INVALID_REGION;
  int bec(-100);

  // following the logic in the original code, should be reviewed!
  if (m_idHelper->is_pixel(id)) {
    bec = abs(m_pixelID->barrel_ec(id));
    if(fabs(bec) < 4) det = PIXEL;
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
InDetPhysHitDecoratorAlg::getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters,
                                                      const Trk::MeasurementBase* measurement,
                                                      bool &isUnbiased) const {
  const Trk::TrackParameters* unbiasedTrkParameters(trkParameters);

  if (!m_updatorHandle.empty() && (isUnbiased)) {
    if (trkParameters->covariance()) {
      // Get unbiased state
      unbiasedTrkParameters = m_updatorHandle->removeFromState(*trkParameters,
                                                               measurement->localParameters(),
                                                               measurement->localCovariance());
      if (!unbiasedTrkParameters) {
        ATH_MSG_INFO(  "Could not get unbiased track parameters, use normal parameters" );
        isUnbiased = false;
      }
    } else if (not m_alreadyWarned) {
      // warn only once!
      ATH_MSG_WARNING("TrackParameters contain no covariance, unbiased track states can not be calculated "
                      "(ie. pulls and residuals will be too small)" );
      m_alreadyWarned = true;
      isUnbiased = false;
    } else {
      isUnbiased = false;
    }// end if no measured track parameter
  }
  return unbiasedTrkParameters;
}

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
