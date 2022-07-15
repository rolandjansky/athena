/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthService.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetTestPixelLayer/InDetTestPixelLayerTool.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "PixelReadoutGeometry/PixelFEUtils.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkTrack/Track.h"

#include "TrkSurfaces/CylinderSurface.h"

#include "TrkGeometry/Layer.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "TrkTrackSummary/TrackSummary.h"

namespace InDet {

InDetTestPixelLayerTool::InDetTestPixelLayerTool(const std::string& name,
                                                 const std::string& n,
                                                 const IInterface* p)
  : AthAlgTool(name, n, p)
  , m_idHelper(nullptr)
  , m_pixelId(nullptr)
  , m_configured(false)
{
  declareInterface<IInDetTestPixelLayerTool>(this);
  declareProperty("CheckActiveAreas", m_checkActiveAreas = false);
  declareProperty("CheckDeadRegions", m_checkDeadRegions = false);
  declareProperty("CheckDisabledFEs", m_checkDisabledFEs = false);
  declareProperty("PhiRegionSize", m_phiRegionSize = 3.);
  declareProperty("EtaRegionSize", m_etaRegionSize = 3.);
  declareProperty("GoodFracCut", m_goodFracCut = 0.5);
  declareProperty("OuterRadius" ,m_outerRadius = 230.);
}

StatusCode
InDetTestPixelLayerTool::initialize()
{

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure())
    return sc;

  // retrieve ID helpers:
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    msg(MSG::FATAL) << "Could not get AtlasDetectorID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get PixelID helper !" << endmsg;
    return StatusCode::FAILURE;
  }

  m_configured = true;
  if (m_extrapolator.empty()) {
    msg(MSG::INFO) << "Extrapolator not configured " << endmsg; // n
    m_configured = false;
  } else {
    if (m_extrapolator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator
                      << endmsg; // n
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg; // n
    }
  }

  ATH_CHECK(m_pixelCondSummaryTool.retrieve( DisableTool{!m_pixelDetElStatus.empty() && !VALIDATE_STATUS_ARRAY_ACTIVATED} ));
  ATH_CHECK(m_pixelDetElStatus.initialize( !m_pixelDetElStatus.empty()) );
  if (!m_pixelDetElStatus.empty()) {
     ATH_CHECK(m_pixelReadout.retrieve() );
  }

  if (!m_configured) {
    msg(MSG::INFO) << "you are using an unconfigured tool" << endmsg;
    msg(MSG::INFO) << "will not be able to extrapolate to the pixelLayer"
                   << endmsg;
    msg(MSG::INFO) << "the values from the track summary will be returned"
                   << endmsg;
  }

  msg(MSG::VERBOSE) << " Initialization of InDetTestPixelLayerTool succesfull"
                    << endmsg;
  return StatusCode::SUCCESS;
}

bool
InDet::InDetTestPixelLayerTool::expectHitInPixelLayer(const EventContext& ctx,
                                                      const Trk::Track* track,
                                                      int pixel_layer,
                                                      bool checkBarrelOnly) const
{
  ATH_MSG_DEBUG("computing expectHitInPixelLayer info");

  const Trk::Perigee* mp = track->perigeeParameters();

  if (!mp) {
    ATH_MSG_WARNING("Found Track with no perigee parameters: no b-layer info "
                    "will be provided ");
    return false;
  } else {
    ATH_MSG_DEBUG("Track perigee parameters");
    return this->expectHitInPixelLayer(ctx, mp, pixel_layer, checkBarrelOnly);
  }
}

bool
InDet::InDetTestPixelLayerTool::expectHitInPixelLayer(
  const EventContext& ctx,
  const Trk::TrackParticleBase* track,
  int pixel_layer) const
{

  // Need to compute if we do expect a B-layer hit.  If the previous
  // calculation already determined that we don't expect a hit, then we still
  // won't expect one.

  if (!track) {
    ATH_MSG_WARNING(
      "Not a valid TrackParticle: no b-layer info will be provided");
    return false;
  }

  ATH_MSG_DEBUG("computing expectHitInPixelLayer info");

  const Trk::Perigee* mp = track->perigee();

  if (!mp) {
    ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no "
                    "b-layer info will be provided");
    return false;
  } else {
    ATH_MSG_DEBUG("TrackParticle perigee parameters");
    //	  mp->dump(mLog);
    return (this->expectHitInPixelLayer(ctx, mp, pixel_layer));
  }
}

bool
InDet::InDetTestPixelLayerTool::expectHitInPixelLayer(
  const EventContext& ctx,
  const Trk::TrackParameters* trackpar,
  int pixel_layer,
  bool checkBarrelOnly) const
{

  if (!m_configured) {
    ATH_MSG_WARNING(
      "Unconfigured tool, unable to compute expectHitInPixelLayer");
    return false;
  }

  bool expect_hit = false; /// will be set to true if at least one good module is passed

  std::vector<std::unique_ptr<const Trk::TrackParameters>> pixelLayerParam;
  if (!this->getPixelLayerParameters(ctx, trackpar, pixelLayerParam)) {
    ATH_MSG_DEBUG("Failed to get pixel layer parameters!");
    return false;
  }

  SG::ReadHandle<InDet::SiDetectorElementStatus> pixelDetElStatus(getPixelDetElStatus(ctx));
  for (std::unique_ptr<const Trk::TrackParameters>& p : pixelLayerParam) {

    if (!(p->associatedSurface().associatedDetectorElement()))
      continue;
    Identifier id =
      p->associatedSurface().associatedDetectorElement()->identify();

    if (checkBarrelOnly && !m_pixelId->is_barrel(id)) continue;
    if (m_pixelId->layer_disk(id) != pixel_layer) continue;

    VALIDATE_STATUS_ARRAY(!m_pixelDetElStatus.empty(),pixelDetElStatus->isGood(p->associatedSurface().associatedDetectorElement()->identifyHash()), m_pixelCondSummaryTool->isGood(id, InDetConditions::PIXEL_MODULE));

    if ( (!m_pixelDetElStatus.empty() && pixelDetElStatus->isGood(p->associatedSurface().associatedDetectorElement()->identifyHash()))
         || (m_pixelDetElStatus.empty() && m_pixelCondSummaryTool->isGood(id, InDetConditions::PIXEL_MODULE))) {

      if (m_checkActiveAreas) {

        if (isActive(p.get())) {

          if (m_checkDeadRegions) {

            double fracGood =
               getFracGood(p.get(), m_phiRegionSize, m_etaRegionSize, !m_pixelDetElStatus.empty() ? pixelDetElStatus.cptr() : nullptr);
            if (fracGood > m_goodFracCut && fracGood >= 0) {
              ATH_MSG_DEBUG("Condition Summary: b-layer good");
              expect_hit =
                true; /// pass good module -> hit is expected on pixelLayer
            } else {
              ATH_MSG_DEBUG("b-layer in dead region: fracGood = " << fracGood);
            }

          } else { /// check dead regions
            ATH_MSG_DEBUG("Condition Summary: not checking dead regions");
            expect_hit =
              true; /// pass good module -> hit is expected on pixelLayer
          }

        } else {
          ATH_MSG_DEBUG(
            "Condition Summary: b-layer good but outside active area");
        }

      } /// check active area (check edges)
      else {
        ATH_MSG_DEBUG(
          "Condition Summary: b-layer good, active areas not checked");
        expect_hit = true; /// pass good module -> hit is expected on pixelLayer
      }

    } else {
      ATH_MSG_DEBUG(__LINE__ << "b-layer not good");
    }

  } /// pixelLayer param

  return expect_hit;
}

bool
InDet::InDetTestPixelLayerTool::expectHit(
   const Trk::TrackParameters* trackpar) const
{
  bool expect_hit =
    false; /// will be set to true if at least one good module is passed

  Identifier id =
    trackpar->associatedSurface().associatedDetectorElement()->identify();

  SG::ReadHandle<InDet::SiDetectorElementStatus> pixelDetElStatus(getPixelDetElStatus(Gaudi::Hive::currentContext()));

  VALIDATE_STATUS_ARRAY(!m_pixelDetElStatus.empty(),pixelDetElStatus->isGood(trackpar->associatedSurface().associatedDetectorElement()->identifyHash()), m_pixelCondSummaryTool->isGood(id, InDetConditions::PIXEL_MODULE));
    if ( (!m_pixelDetElStatus.empty() && pixelDetElStatus->isGood(trackpar->associatedSurface().associatedDetectorElement()->identifyHash()))
       || (m_pixelDetElStatus.empty() && m_pixelCondSummaryTool->isGood(id, InDetConditions::PIXEL_MODULE))) {

    if (m_checkDeadRegions) {

      double fracGood = getFracGood(trackpar, m_phiRegionSize, m_etaRegionSize, !m_pixelDetElStatus.empty() ? pixelDetElStatus.cptr() : nullptr);
      if (fracGood > m_goodFracCut && fracGood >= 0) {
        ATH_MSG_DEBUG("Condition Summary: b-layer good");
        expect_hit = true; /// pass good module -> hit is expected on pixelLayer
      } else {
        ATH_MSG_DEBUG("b-layer in dead region: fracGood = " << fracGood);
      }

    } else { /// check dead regions
      ATH_MSG_DEBUG("Condition Summary: not checking dead regions");
      expect_hit = true; /// pass good module -> hit is expected on pixelLayer
    }

  } else {
    ATH_MSG_DEBUG(__LINE__ << "b-layer not good");
  }

  return expect_hit;
}

double
InDet::InDetTestPixelLayerTool::getFracGood(const Trk::TrackParticleBase* track,
                                            int pixel_layer) const
{
  const Trk::Perigee* mp = track->perigee();

  if (!mp) {
    ATH_MSG_WARNING("Found Track with no perigee parameters: no b-layer info "
                    "will be provided ");
    return -4.;
  } else {
    ATH_MSG_DEBUG("Track perigee parameters");
    return this->getFracGood(mp, pixel_layer);
  }
}

double
InDet::InDetTestPixelLayerTool::getFracGood(
  const Trk::TrackParameters* trackpar,
  int pixel_layer) const
{

  if (!m_configured) {
    ATH_MSG_WARNING(
      "Unconfigured tool, unable to compute expectHitInPixelLayer");
    return -6.;
  }

  std::vector<std::unique_ptr<const Trk::TrackParameters>> pixelLayerParam;
  if (!this->getPixelLayerParameters(trackpar, pixelLayerParam))
    return -5.;

  // now, figure out which layer we're supposed to be checking.
  bool found_layer = false;
  for (std::unique_ptr<const Trk::TrackParameters>& p : pixelLayerParam) {
    if (!(p->associatedSurface().associatedDetectorElement()))
      continue;
    Identifier id =
      p->associatedSurface().associatedDetectorElement()->identify();

    if (m_pixelId->layer_disk(id) == pixel_layer){
      found_layer = true;
      break;
    }
  }

  // if we're asking for a layer that doesn't exist in the extrapolation, then
  // return.
  if (!found_layer)
    return -7.;

  SG::ReadHandle<InDet::SiDetectorElementStatus> pixelDetElStatus(getPixelDetElStatus(Gaudi::Hive::currentContext()));

  for (std::unique_ptr<const Trk::TrackParameters>& p : pixelLayerParam) {

    if (!(p->associatedSurface().associatedDetectorElement()))
      continue;
    Identifier id =
      p->associatedSurface().associatedDetectorElement()->identify();

    if (m_pixelId->layer_disk(id) != pixel_layer)
      continue;

    VALIDATE_STATUS_ARRAY(!m_pixelDetElStatus.empty(),pixelDetElStatus->isGood(trackpar->associatedSurface().associatedDetectorElement()->identifyHash()), m_pixelCondSummaryTool->isGood(id, InDetConditions::PIXEL_MODULE));
      if (  (!m_pixelDetElStatus.empty() && pixelDetElStatus->isGood(p->associatedSurface().associatedDetectorElement()->identifyHash()))
        || (m_pixelDetElStatus.empty() && m_pixelCondSummaryTool->isGood(id, InDetConditions::PIXEL_MODULE))) {

      if (isActive(p.get())) {

        return getFracGood(p.get(), m_phiRegionSize, m_etaRegionSize, !m_pixelDetElStatus.empty() ? pixelDetElStatus.cptr() : nullptr);

      } else {
        ATH_MSG_DEBUG(
          "Condition Summary: b-layer good but outside active area");
      }

    } else {
      ATH_MSG_DEBUG(__LINE__ << "b-layer not good");
    }

  } /// pixelLayer param

  return -3.;
}

bool
InDet::InDetTestPixelLayerTool::isActive(
  const Trk::TrackParameters* trackpar) const
{

  const InDetDD::SiDetectorElement* siElement =
    dynamic_cast<const InDetDD::SiDetectorElement*>(
      trackpar->associatedSurface().associatedDetectorElement());
  if (siElement == nullptr) {
    // -------  in dubio pro reo --> return false (is assumed insensitive)
    ATH_MSG_DEBUG("TrackParameters do not belong to a Si Element");
    /// checking active material
  } else {
    double phitol = 2.5;
    double etatol = 5.;
    if (trackpar->covariance()) {
      phitol = 3. * sqrt((*trackpar->covariance())(Trk::locX, Trk::locX));
      etatol = 3. * sqrt((*trackpar->covariance())(Trk::locY, Trk::locY));
    }
    InDetDD::SiIntersect siIn =
      siElement->inDetector(trackpar->localPosition(), phitol, etatol);
    if (siElement->nearBondGap(trackpar->localPosition(), etatol)) {
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "---> extrapolation on bond gap within " << etatol
                        << ", return" << endmsg;
      }
    } else if (!siIn.in()) {
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG)
          << "---> extrapolation not inside (active?) detector within "
          << phitol << " " << etatol << ", return" << endmsg;
      }
    } else {
      return true;
    }
  }

  return false;
}

bool
InDet::InDetTestPixelLayerTool::getTrackStateOnPixelLayerInfo(
  const Trk::Track* track,
  std::vector<TrackStateOnPixelLayerInfo>& infoList) const
{

  std::unique_ptr<const Trk::TrackParameters> startParameters = nullptr;

  if (track->perigeeParameters()) {
    startParameters = track->perigeeParameters()->uniqueClone();
  } else if (track->trackParameters()->front()) {
    startParameters =
      m_extrapolator->extrapolate(
        Gaudi::Hive::currentContext(),
        *(track->trackParameters()->front()),
        Trk::PerigeeSurface(),
        Trk::anyDirection,
        false);
  }

  if (!startParameters) {
    ATH_MSG_WARNING("Found Track with no perigee parameters: no b-layer info "
                    "will be provided");
    return false;
  }

  bool succeed = getTrackStateOnPixelLayerInfo(startParameters.get(), infoList);
  return succeed;
}

bool
InDet::InDetTestPixelLayerTool::getTrackStateOnPixelLayerInfo(
  const Trk::TrackParticleBase* track,
  std::vector<TrackStateOnPixelLayerInfo>& infoList) const
{

  const Trk::Perigee* startParameters = track->perigee();

  if (!startParameters) {
    ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no "
                    "b-layer info will be provided");
    return false;
  }

  return getTrackStateOnPixelLayerInfo(startParameters, infoList);
}

bool
InDet::InDetTestPixelLayerTool::getTrackStateOnPixelLayerInfo(
  const Trk::TrackParameters* trackpar,
  std::vector<TrackStateOnPixelLayerInfo>& infoList,
  int pixel_layer,
  bool checkBarrelOnly) const
{

  infoList.clear();

  if (!m_configured) {
    ATH_MSG_WARNING("Unconfigured tool, unable to compute PixelLayer info");
    return false;
  }

  std::vector<std::unique_ptr<const Trk::TrackParameters>> pixelLayerParam;
  if (!getPixelLayerParameters(trackpar, pixelLayerParam))
    return false;

  SG::ReadHandle<InDet::SiDetectorElementStatus> pixelDetElStatus(getPixelDetElStatus(Gaudi::Hive::currentContext()));

  for (std::unique_ptr<const Trk::TrackParameters>& trkParam :
       pixelLayerParam) {
    TrackStateOnPixelLayerInfo pixelLayerInfo;

    double fracGood =
      getFracGood(trkParam.get(), m_phiRegionSize, m_etaRegionSize, !m_pixelDetElStatus.empty() ? pixelDetElStatus.cptr() : nullptr);
    pixelLayerInfo.goodFraction(fracGood);

    Identifier id =
      trkParam->associatedSurface().associatedDetectorElement()->identify();
    pixelLayerInfo.moduleId(id);

    if (checkBarrelOnly && !m_pixelId->is_barrel(id)) continue;
    if (m_pixelId->layer_disk(id) != pixel_layer) continue;

    const InDetDD::SiDetectorElement* sielem =
      dynamic_cast<const InDetDD::SiDetectorElement*>(
        trkParam->associatedSurface().associatedDetectorElement());

    if (!sielem) {
      ATH_MSG_WARNING("Failed to cast TrackParameters associated surface to "
                      "SiDetectorElement");
      return false;
    }

    const Amg::Vector2D& locPos = trkParam->localPosition();
    Identifier holeId_c = sielem->identifierOfPosition(locPos);
    pixelLayerInfo.pixelId(holeId_c);

    double etaDist = -9999;
    double phiDist = -9999;
    const InDetDD::PixelModuleDesign* design =
      dynamic_cast<const InDetDD::PixelModuleDesign*>(&sielem->design());
    if (design) {
      design->distanceToDetectorEdge(
        locPos,
        etaDist,
        phiDist); //// implicite cast from Amg::Vector2D to SiLocalPosition
    } else {
      ATH_MSG_WARNING("could not get pixel module design for  "
                      << m_idHelper->show_to_string(id)
                      << ", returning false for getTrackStateOnPixelLayerInfo");
      return false;
    }
    pixelLayerInfo.distToModuleEdgePhi(phiDist);
    pixelLayerInfo.distToModuleEdgeEta(etaDist);

    pixelLayerInfo.globalPosition(trkParam->position());

    pixelLayerInfo.localX(locPos[Trk::locX]);
    pixelLayerInfo.localY(locPos[Trk::locY]);

    pixelLayerInfo.theta(trkParam->parameters()[Trk::theta]);
    pixelLayerInfo.phi(trkParam->parameters()[Trk::phi0]);

    float error_locx = -9999;
    float error_locy = -9999;

    if (trkParam->covariance()) {
      error_locx = sqrt((*trkParam->covariance())(Trk::locX, Trk::locX));
      error_locy = sqrt((*trkParam->covariance())(Trk::locY, Trk::locY));
    } else {
      ATH_MSG_DEBUG("could not TrackParameters for hole  "
                    << m_idHelper->show_to_string(id));
    }

    pixelLayerInfo.errLocalX(error_locx);
    pixelLayerInfo.errLocalY(error_locy);

    VALIDATE_STATUS_ARRAY(!m_pixelDetElStatus.empty(),pixelDetElStatus->isGood(trkParam->associatedSurface().associatedDetectorElement()->identifyHash()), m_pixelCondSummaryTool->isGood(id, InDetConditions::PIXEL_MODULE));
    bool isgood =
      (  (!m_pixelDetElStatus.empty() && pixelDetElStatus->isGood(trkParam->associatedSurface().associatedDetectorElement()->identifyHash()))
       || (m_pixelDetElStatus.empty() && m_pixelCondSummaryTool->isGood(id, InDetConditions::PIXEL_MODULE)));

    double phitol = 2.5;
    double etatol = 5.;

    if (trkParam->covariance()) {
      phitol = 3 * sqrt((*trkParam->covariance())(Trk::locX, Trk::locX));
      etatol = 3 * sqrt((*trkParam->covariance())(Trk::locY, Trk::locY));
    }

    InDetDD::SiIntersect siIn = sielem->inDetector(locPos, phitol, etatol);
    bool isIn = siIn.in();

    if (isgood) {
      if (isIn)
        pixelLayerInfo.type(insideGoodModule);
      else
        pixelLayerInfo.type(nearGoodModuleEdge);
    } else {
      if (isIn)
        pixelLayerInfo.type(insideBadModule);
      else
        pixelLayerInfo.type(nearBadModuleEdge);
    }

    infoList.push_back(pixelLayerInfo);
  } /// pixelLayer param

  return true;
}

bool
InDet::InDetTestPixelLayerTool::getPixelLayerParameters(
  const EventContext& ctx,
  const Trk::TrackParameters* trackpar,
  std::vector<std::unique_ptr<const Trk::TrackParameters>>& pixelLayerParam)
  const
{
  //// Cylinder bigger than the b-layer ////
  ATH_MSG_DEBUG("Trying to extrapolate to pixelLayer");
  Amg::Transform3D surfTrans;
  surfTrans.setIdentity();
  Trk::CylinderSurface BiggerThanPixelLayerSurface(surfTrans, m_outerRadius, 10000.0);

  // extrapolate stepwise to this parameter (be careful, sorting might be wrong)
  std::vector<std::unique_ptr<Trk::TrackParameters>> paramList =
    m_extrapolator->extrapolateStepwise(
      ctx, *trackpar, BiggerThanPixelLayerSurface, Trk::alongMomentum, false);

  if (paramList.empty()) {
    ATH_MSG_DEBUG("No parameter returned by propagator ");
    ATH_MSG_VERBOSE("dumping track parameters " << *trackpar);
    return false;
  }

  ATH_MSG_DEBUG(
    " Number of generated parameters by propagator: " << paramList.size());

  int s_int = 0;
  for (std::unique_ptr<Trk::TrackParameters>& p : paramList) {
    ATH_MSG_DEBUG(s_int++ << "th surface : ");

    Identifier id;
    if (!(p->associatedSurface().associatedDetectorElement() != nullptr &&
          p->associatedSurface().associatedDetectorElement()->identify() !=
            0)) {
      continue;
    }

    id = p->associatedSurface().associatedDetectorElement()->identify();
    if (!m_idHelper->is_pixel(id)) {
      continue;
    }

    ATH_MSG_DEBUG("Found pixel module : " << id.get_compact());

    pixelLayerParam.push_back(std::move(p));
  } /// all params

  return true;
}

double
InDet::InDetTestPixelLayerTool::getFracGood(
  const Trk::TrackParameters* trkParam,
  double phiRegionSize,
  double etaRegionSize,
  const InDet::SiDetectorElementStatus *pixelDetElStatus) const
{

  Identifier moduleid =
    trkParam->associatedSurface().associatedDetectorElement()->identify();
  IdentifierHash id_hash = m_pixelId->wafer_hash(moduleid);

  VALIDATE_STATUS_ARRAY(pixelDetElStatus,pixelDetElStatus->isGood(id_hash), m_pixelCondSummaryTool->isGood(id_hash));
  bool is_good (  ( pixelDetElStatus && pixelDetElStatus->isGood(id_hash))
                || (!pixelDetElStatus && m_pixelCondSummaryTool->isGood(id_hash)));
  if (!is_good)
    return 0.;

  const Amg::Vector2D& locPos = trkParam->localPosition();
  double locx = locPos[Trk::locX];
  double locy = locPos[Trk::locY];
  double error_locx = -9999;
  double error_locy = -9999;

  if (trkParam->covariance()) {
    error_locx = sqrt((*trkParam->covariance())(Trk::locX, Trk::locX));
    error_locy = sqrt((*trkParam->covariance())(Trk::locY, Trk::locY));
  } else {
    ATH_MSG_DEBUG("could not get MeasuredTrackParameters ");
  }

  double phitol = 2.5;
  double etatol = 5.;
  if (trkParam->covariance()) {
    phitol = phiRegionSize * error_locx;
    etatol = etaRegionSize * error_locy;
  }

  // now, check to see that phitol and etatol extend at least beyond the current
  // pixel
  const InDetDD::SiDetectorElement* sielem =
    dynamic_cast<const InDetDD::SiDetectorElement*>(
      trkParam->associatedSurface().associatedDetectorElement());

  if (!sielem) {
    ATH_MSG_WARNING(
      "Failed to cast TrackParameters associated surface to SiDetectorElement");
    return false;
  }

  const InDetDD::PixelModuleDesign* design =
    dynamic_cast<const InDetDD::PixelModuleDesign*>(&sielem->design());
  if (design) {
    phitol = std::max(phitol, design->phiPitch() + 1e-6);
    etatol = std::max(etatol, design->etaPitch() + 1e-6);
  } else {
    ATH_MSG_WARNING(
      "could not get pixel module design, returning 0 for getFracGood");
    return 0.;
  }

  Amg::Vector2D LocPos(locx, locy);

  double startLocX = locx - phitol;
  double startLocY = locy - etatol;

  double endLocX = locx + phitol;
  double endLocY = locy + etatol;

  Amg::Vector2D startLocPostmp(startLocX, startLocY);
  Amg::Vector2D endLocPostmp(endLocX, endLocY);

  double etaDist = -9999;
  double phiDist = -9999;

  if (design) {
    design->distanceToDetectorEdge(LocPos, etaDist, phiDist);
    if (phiDist < 0)
      locx += (fabs(phiDist) + 1e-6); /// not exactly on the edge
    if (etaDist < 0)
      locy += (fabs(etaDist) + 1e-6);
    design->distanceToDetectorEdge(startLocPostmp, etaDist, phiDist);
    if (phiDist < 0)
      startLocX += (fabs(phiDist) + 1e-6); /// not exactly on the edge
    if (etaDist < 0)
      startLocY += (fabs(etaDist) + 1e-6);
    design->distanceToDetectorEdge(endLocPostmp, etaDist, phiDist);
    if (phiDist < 0)
      endLocX -= (fabs(phiDist) + 1e-6);
    if (etaDist < 0)
      endLocY -= (fabs(etaDist) + 1e-6);
  }

  LocPos = Amg::Vector2D(locx, locy);

  if (m_checkDisabledFEs) {
    const InDetConditions::Hierarchy context = InDetConditions::PIXEL_CHIP;
    Identifier centreId = sielem->identifierOfPosition(LocPos);
    if (centreId.is_valid()) {
      Identifier moduleID = m_pixelId->wafer_id(centreId);
      IdentifierHash id_hash = m_pixelId->wafer_hash(moduleID);

      VALIDATE_STATUS_ARRAY(!m_pixelDetElStatus.empty(),pixelDetElStatus->isChipGood(id_hash,m_pixelReadout->getFE(centreId, moduleID)), m_pixelCondSummaryTool->isGood(centreId, context));
      bool is_chip_good (  (!m_pixelDetElStatus.empty() && pixelDetElStatus->isChipGood(id_hash,m_pixelReadout->getFE(centreId, moduleID)))
                      || (m_pixelDetElStatus.empty() && m_pixelCondSummaryTool->isGood(centreId, context)));
      if (!is_chip_good)
        return 0.;
    }

    else
      ATH_MSG_WARNING("Invalid Identifier, skipping check of FE...");
  }

  Amg::Vector2D startLocPos(startLocX, startLocY);
  Amg::Vector2D endLocPos(endLocX, endLocY);
  Identifier startId = sielem->identifierOfPosition(startLocPos);
  Identifier endId = sielem->identifierOfPosition(endLocPos);

  if (!startId.is_valid() || !endId.is_valid()) {
    ATH_MSG_WARNING("unvalid identifier: locPos outside module! ");
    return 0.;
  }

  double frac =0.;

  if (pixelDetElStatus) {
     frac = Pixel::getGoodFraction(*pixelDetElStatus, *m_pixelReadout, *m_pixelId, moduleid, id_hash, startId, endId);
     VALIDATE_STATUS_ARRAY(!m_pixelDetElStatus.empty(),frac, m_pixelCondSummaryTool->goodFraction(id_hash, startId, endId));

  }
  else {
      frac = m_pixelCondSummaryTool->goodFraction(id_hash, startId, endId);
  }

  return frac;
}

} // end namespace

