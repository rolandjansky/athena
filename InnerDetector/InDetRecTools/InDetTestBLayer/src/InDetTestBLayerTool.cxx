/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthService.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetTestBLayer/InDetTestBLayerTool.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "Particle/TrackParticle.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkSurfaces/CylinderSurface.h"

#include "TrkGeometry/Layer.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"


#include <iostream>
#include <sstream>

using Amg::Transform3D;
// don't want to include TrackSummary in the header
// therefore anonymous "static" definition in the implementation file
//namespace {
  Trk::SummaryType s_layerSummaryTypeExpectHit[2] {
    Trk::expectInnermostPixelLayerHit,
    Trk::expectNextToInnermostPixelLayerHit
  };
//}


namespace InDet {
  const char *InDetTestBLayerTool::s_layerNames[2] {
    "innermost pixel layer",
    "next to innermost pixel layer"
  };

  InDetTestBLayerTool::InDetTestBLayerTool(const std::string& name,
					   const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p),
    m_extrapolator(""),
    m_pixelCondSummarySvc("",n),
    m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
    m_idHelper(nullptr),
    m_pixelId(nullptr),
    m_configured(false)
  {
    declareInterface<IInDetTestBLayerTool>(this);
    declareProperty("Extrapolator"   , m_extrapolator);
    declareProperty("PixelSummarySvc", m_pixelCondSummarySvc);
    declareProperty("ResidualPullCalculator",m_residualPullCalculator);
    declareProperty("CheckActiveAreas", m_checkActiveAreas = false);
    declareProperty("CheckDeadRegions", m_checkDeadRegions = false);
    declareProperty("CheckAtLeastNearestNeighbors", m_checkAtLeastNearestNeighbors = false);
    declareProperty("PhiRegionSize", m_phiRegionSize = 3.);
    declareProperty("EtaRegionSize", m_etaRegionSize = 3.);
    declareProperty("GoodFracCut", m_goodFracCut = 0.5);

  }

  InDetTestBLayerTool::~InDetTestBLayerTool() {}

  StatusCode InDetTestBLayerTool::initialize() {


    StatusCode sc = AthAlgTool::initialize();
    if (sc.isFailure()) return sc;

    // retrieve ID helpers:
    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
      ATH_MSG_FATAL( "Could not get AtlasDetectorID helper");
      return StatusCode::FAILURE;
    }

    sc = detStore()->retrieve(m_pixelId, "PixelID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not get PixelID helper !");
      return StatusCode::FAILURE;
    }

    m_configured=true;
    if( m_extrapolator.empty() ){
      ATH_MSG_INFO( "Extrapolator not configured " );
      m_configured=false;
    }
    else{
      if ( m_extrapolator.retrieve().isFailure() )
	{
	  ATH_MSG_FATAL( "Failed to retrieve tool " << m_extrapolator );
	  return StatusCode::FAILURE;
	}  else {
	ATH_MSG_INFO( "Retrieved tool " << m_extrapolator );
      }
    }

    // Get PixelConditionsSummarySvc
    if( m_pixelCondSummarySvc.empty() ){
      ATH_MSG_INFO( "PixelConditionsSummarySvc not configured " );
      m_configured=false;
    }
    else{
      if ( m_pixelCondSummarySvc.retrieve().isFailure() ) {
	ATH_MSG_FATAL("Failed to retrieve tool " << m_pixelCondSummarySvc);
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO("Retrieved tool " << m_pixelCondSummarySvc);
      }
    }

    if(!m_configured){
      ATH_MSG_INFO("you are using an unconfigured tool");
      ATH_MSG_INFO("will not be able to extrapolate to the blayer");
      ATH_MSG_INFO("the values from the track summary will be returned");
    }

    if (m_residualPullCalculator.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_residualPullCalculator);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Retrieved tool " << m_residualPullCalculator);
    }


    ATH_MSG_VERBOSE(" Initialization of InDetTestBLayerTool succesfull");
    return StatusCode::SUCCESS;

  }

  StatusCode InDetTestBLayerTool::finalize() {
    ATH_MSG_DEBUG("Finalization of InDetTestBLayerTool succesfull");
    return StatusCode::SUCCESS;
  }

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::bLayerHitResidual(const Trk::TrackParticleBase* trackparticle) const
  {
    return pixelLayerHitResidual(trackparticle,0);
  }

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::innermostPixelLayerHitResidual(const Trk::TrackParticleBase* trackparticle) const
  {
    return pixelLayerHitResidual(trackparticle,0);
  }

   const Trk::ResidualPull* InDet::InDetTestBLayerTool::nextToInnermostPixelLayerHitResidual(const Trk::TrackParticleBase* trackparticle) const
  {
    return pixelLayerHitResidual(trackparticle,1);
  }

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::pixelLayerHitResidual(const Trk::TrackParticleBase* trackparticle, int layer) const
  {
    assert(layer>=0 && layer<=1);
    const Trk::Track* track = trackparticle->originalTrack();

    if (!track) {
      ATH_MSG_DEBUG( "No original track, residual calculation for " << s_layerNames[layer] << " can not be performed" );
      return 0;
    }

    return(this->pixelLayerHitResidual(track,layer));
  }


  const Trk::ResidualPull* InDet::InDetTestBLayerTool::bLayerHitResidual(const Trk::Track* track) const
  {
    return pixelLayerHitResidual(track,0);
  }

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::innermostPixelLayerHitResidual(const Trk::Track* track) const
  {
    return pixelLayerHitResidual(track,0);
  }

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::nextToInnermostPixelLayerHitResidual(const Trk::Track* track) const
  {
    return pixelLayerHitResidual(track,1);
  }

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::pixelLayerHitResidual(const Trk::Track* track, int layer) const
  {
    assert(layer>=0 && layer<=1);

    //    const Trk::ResidualPull* residualPull=0;
    const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();

    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
	 it!=trackStates->end();
	 it++) {
      if (!(*it)) {
	ATH_MSG_WARNING("TrackStateOnSurface == Null");
	continue;
      }

      if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ){

	ATH_MSG_VERBOSE ("try to get measurement for track state");
	// Get pointer to measurement on track
	const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();


	if(  (*it)->trackParameters() !=0 &&
             /*	     &((*it)->trackParameters()->associatedSurface()) !=0 && */
	     (*it)->trackParameters()->associatedSurface().associatedDetectorElement() !=0 &&
	     (*it)->trackParameters()->associatedSurface().associatedDetectorElement()->identify() !=0 )
	  {

	    Identifier id;
	    id = (*it)->trackParameters()->associatedSurface().associatedDetectorElement()->identify();
	    if (m_idHelper->is_pixel(id))
	      {
		ATH_MSG_DEBUG("Found pixel module : Associated track parameter");
		if(m_pixelId->is_barrel(id))
		{
		    ATH_MSG_DEBUG("Found pixel barrel");
		    if(m_pixelId->layer_disk(id) == layer)
		      {
                        ATH_MSG_DEBUG("Found " << s_layerNames[layer] << " " << id.get_compact() );
			return m_residualPullCalculator->residualPull(measurement,(*it)->trackParameters(),Trk::ResidualPull::Biased);
		      }
		  }
	      }
	  }
      }
    }

    return 0;

  }

  bool InDet::InDetTestBLayerTool::expectHitInBLayer(const Trk::Track* track, bool recompute) const
  {
    return expectHitInPixelLayer(track,0, recompute);
  }

   bool InDet::InDetTestBLayerTool::expectHitInInnermostPixelLayer(const Trk::Track* track, bool recompute) const
  {
    return expectHitInPixelLayer(track,0, recompute);
  }

  bool InDet::InDetTestBLayerTool::expectHitInNextToInnermostPixelLayer(const Trk::Track* track, bool recompute) const
  {
    return expectHitInPixelLayer(track,1, recompute);
  }

  bool InDet::InDetTestBLayerTool::expectHitInPixelLayer(const Trk::Track *track, int layer, bool recompute) const {
    assert( layer>=0 && layer<=1);
    if(!recompute){
      const Trk::TrackSummary* ts =  track->trackSummary();
      if(ts){
	int ehbl = ts->get(s_layerSummaryTypeExpectHit[layer]);
	if(0==ehbl || 1==ehbl ){
	  ATH_MSG_DEBUG("Found the expected hit in the " << s_layerNames[layer] << " info in TrackSummary: return cached value" );
	  return ehbl;
	}
      }
    }
    else{
      ATH_MSG_DEBUG("Forced to recompute whether a hit is expected in the " << s_layerNames[layer] << " or not." );
    }

    ATH_MSG_DEBUG("Computing whether a hit is expected in the " << s_layerNames[layer] << " or not." );

    const Trk::Perigee* mp = track->perigeeParameters();

    if (!mp)
      {
        ATH_MSG_WARNING("Found Track with no perigee parameters: no information whether a hit is expected in the " << s_layerNames[layer] << " will be provided." );
	return false;
      }
    else
      {
	ATH_MSG_DEBUG("Track perigee parameters");
	return this->expectHitInPixelLayer(mp,layer);
      }
  }


  bool InDet::InDetTestBLayerTool::expectHitInBLayer(const Trk::TrackParticleBase* track, bool recompute) const
  {
    return expectHitInPixelLayer(track,0,recompute);
  }

  bool InDet::InDetTestBLayerTool::expectHitInInnermostPixelLayer(const Trk::TrackParticleBase* track, bool recompute) const
  {
    return expectHitInPixelLayer(track,0,recompute);
  }


   bool InDet::InDetTestBLayerTool::expectHitInNextToInnermostPixelLayer(const Trk::TrackParticleBase* track, bool recompute) const
  {
    return expectHitInPixelLayer(track,1,recompute);
  }

  bool InDet::InDetTestBLayerTool::expectHitInPixelLayer(const Trk::TrackParticleBase *track, int layer, bool recompute) const {
    assert( layer>=0 && layer<=1);
    if(!recompute){
      const Trk::TrackSummary* ts =  track->trackSummary();
      if(ts){
	int ehbl = ts->get(s_layerSummaryTypeExpectHit[layer]);
	if(0==ehbl || 1==ehbl ){
	  ATH_MSG_DEBUG("Found the expected hit in the " << s_layerNames[layer] << " info in TrackSummary: return cached value" );
	  return ehbl;
	}
      }
    }
    else{
      ATH_MSG_DEBUG("Forced to recompute whether a hit is expected in the " << s_layerNames[layer] << " or not." );
    }

    ATH_MSG_DEBUG("Computing whether a hit is expected in the " << s_layerNames[layer] << " or not." );

    const Trk::Perigee* mp = track->perigee();

    if(!mp)
      {
        ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no information whether a hit is expected in the " << s_layerNames[layer] << " will be provided." );
	return false;
      }
    else
      {
	ATH_MSG_DEBUG("TrackParticle perigee parameters");
	//	  mp->dump(mLog);
	return (this->expectHitInPixelLayer(mp,layer));
      }
  }


  bool InDet::InDetTestBLayerTool::expectHitInBLayer(const Trk::TrackParameters* trackpar) const
  {
    return expectHitInPixelLayer(trackpar, 0);
  }

  bool InDet::InDetTestBLayerTool::expectHitInInnermostPixelLayer(const Trk::TrackParameters* trackpar) const
  {
    return expectHitInPixelLayer(trackpar, 0);
  }

  bool InDet::InDetTestBLayerTool::expectHitInNextToInnermostPixelLayer(const Trk::TrackParameters* trackpar) const
  {
    return expectHitInPixelLayer(trackpar, 1);
  }

  bool InDet::InDetTestBLayerTool::expectHitInPixelLayer(const Trk::TrackParameters* trackpar,int layer) const
  {
    assert( layer >=0 && layer<=1);
    const char *layer_name=s_layerNames[layer];

    if(!m_configured){
      ATH_MSG_WARNING("Unconfigured tool, unable to compute expected hit in the " << layer_name << ".");
      return false;
    }

    bool expect_hit = false; /// will be set to true if at least on good module is passed

    //// Cylinder bigger than the given layer ? ////
    std::vector<const Trk::TrackParameters*> blayerParam;
    if(!this->getPixelLayerParameters(trackpar, blayerParam,layer)) return false;

    std::vector<const Trk::TrackParameters*>::const_iterator it = blayerParam.begin();

    for(; it !=blayerParam.end(); ++it){

      Identifier id = (*it)->associatedSurface().associatedDetectorElement()->identify();

      if( m_pixelCondSummarySvc->isGood(id,InDetConditions::PIXEL_MODULE) ){

	if( m_checkActiveAreas ){

	  if( isActive(*it) ){

	    if(m_checkDeadRegions){

	      double fracGood = getFracGood(*it, m_phiRegionSize, m_etaRegionSize);
	      if(fracGood>m_goodFracCut){
		ATH_MSG_DEBUG("Condition Summary: " << layer_name << " good");
		expect_hit=true;  /// pass good module -> hit is expected on blayer
	      }
	      else{
		ATH_MSG_DEBUG( layer_name << " in dead region");
	      }

	    }
	    else{ /// check dead regios
	      ATH_MSG_DEBUG("Condition Summary: " << layer_name << " good");
	      expect_hit=true;  /// pass good module -> hit is expected on blayer
	    }

	  }
	  else{
	    ATH_MSG_DEBUG("Condition Summary: " << layer_name << " good but outside active area");
	  }

	} /// check active area (check edges)
	else{
	  ATH_MSG_DEBUG("Condition Summary: " << layer_name << " good, active areas not checked");
	  expect_hit=true; /// pass good module -> hit is expected on blayer
	}

      }
      else{
	ATH_MSG_DEBUG( layer_name << " not good");
      }

      delete *it;

    } /// blayer param


    return expect_hit;

  }



  bool InDet::InDetTestBLayerTool::isActive(const Trk::TrackParameters* trackpar) const
  {

    const InDetDD::SiDetectorElement* siElement =
      dynamic_cast<const InDetDD::SiDetectorElement*> (trackpar->associatedSurface().associatedDetectorElement());
    if (siElement == 0 ) {
      // -------  in dubio pro reo --> return false (is assumed insensitive)
      ATH_MSG_DEBUG("TrackParameters do not belong to a Si Element" );
      /// checking active material
     } else {
       double phitol = 2.5;
       double etatol = 5.;

       phitol = 3.* sqrt((*trackpar->covariance())(Trk::locX,Trk::locX));
       etatol = 3.* sqrt((*trackpar->covariance())(Trk::locY,Trk::locY));

       InDetDD::SiIntersect siIn = siElement->inDetector(trackpar->localPosition(), phitol, etatol);
       if( siElement->nearBondGap(trackpar->localPosition(), etatol) ) {
         ATH_MSG_DEBUG( "---> extrapolation on bond gap within " << etatol << ", return" );
       } else if (!siIn.in()) {
         ATH_MSG_DEBUG( "---> extrapolation not inside (active?) detector within "<< phitol << " " << etatol << ", return" );
       } else {
	 return true;
       }
     }

    return false;

  }

  bool InDet::InDetTestBLayerTool::getTrackStateOnBlayerInfo(const Trk::Track* track, std::vector<TrackStateOnBLayerInfo>& infoList)  const {
    return getTrackStateOnPixelLayerInfo(track,infoList,0);
  }


  bool InDet::InDetTestBLayerTool::getTrackStateOnInnermostPixelLayerInfo(const Trk::Track* track, std::vector<TrackStateOnBLayerInfo>& infoList)  const {
     return getTrackStateOnPixelLayerInfo(track,infoList,0);
  }


  bool InDet::InDetTestBLayerTool::getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::Track* track, std::vector<TrackStateOnBLayerInfo>& infoList)  const {
     return getTrackStateOnPixelLayerInfo(track,infoList,1);
  }


  bool InDet::InDetTestBLayerTool::getTrackStateOnPixelLayerInfo(const Trk::Track* track, std::vector<TrackStateOnBLayerInfo>& infoList, int layer)  const
  {
    assert( layer>=0 && layer<=1);

    const Trk::TrackParameters* startParameters = 0;

    if (track->perigeeParameters()){
      startParameters = track->perigeeParameters()->clone();
    }
    else if (track->trackParameters()->front()) {
      startParameters = m_extrapolator->extrapolate(*(track->trackParameters()->front()),
						    Trk::PerigeeSurface(),
						    Trk::anyDirection,
						    false);
    }

    if(!startParameters){
      ATH_MSG_WARNING("Found Track with no perigee parameters: no " << s_layerNames[layer] << " info will be provided" );
      return false;
    }

    bool succeed = getTrackStateOnPixelLayerInfo(startParameters, infoList,layer);
     delete startParameters;
     return succeed;
  }


  bool InDet::InDetTestBLayerTool::getTrackStateOnBlayerInfo(const Trk::TrackParticleBase* trackparticle,
                                                             std::vector<TrackStateOnBLayerInfo>& infoList)  const {
    return getTrackStateOnPixelLayerInfo(trackparticle, infoList,0);
  }

  bool InDet::InDetTestBLayerTool::getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParticleBase* trackparticle,
                                                                          std::vector<TrackStateOnBLayerInfo>& infoList)  const {
    return getTrackStateOnPixelLayerInfo(trackparticle, infoList,0);
  }


  bool InDet::InDetTestBLayerTool::getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::TrackParticleBase* trackparticle,
                                                                                std::vector<TrackStateOnBLayerInfo>& infoList) const {
    return getTrackStateOnPixelLayerInfo(trackparticle, infoList,1);
  }


  bool InDet::InDetTestBLayerTool::getTrackStateOnPixelLayerInfo(const Trk::TrackParticleBase* track,
                                                                 std::vector<TrackStateOnBLayerInfo>& infoList,
                                                                 int layer) const
  {
    assert( layer>=0 && layer<=1);

    const Trk::Perigee* startParameters = track->perigee();

    if(!startParameters){
      ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no " << s_layerNames[layer] << " info will be provided");
      return false;
    }

    return getTrackStateOnPixelLayerInfo(startParameters, infoList, layer);

  }



  bool InDet::InDetTestBLayerTool::getTrackStateOnBlayerInfo(const Trk::TrackParameters* trackpar, std::vector<TrackStateOnBLayerInfo>& infoList)  const {
    return getTrackStateOnPixelLayerInfo(trackpar,infoList,0);
  }

  bool InDet::InDetTestBLayerTool::getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, std::vector<TrackStateOnBLayerInfo>& infoList)  const {
    return getTrackStateOnPixelLayerInfo(trackpar,infoList,0);
  }

  bool InDet::InDetTestBLayerTool::getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, std::vector<TrackStateOnBLayerInfo>& infoList)  const {
    return getTrackStateOnPixelLayerInfo(trackpar,infoList,1);
  }

  bool InDet::InDetTestBLayerTool::getTrackStateOnPixelLayerInfo(const Trk::TrackParameters* trackpar,
                                                                 std::vector<TrackStateOnBLayerInfo>& infoList,
                                                                 int layer)  const
  {
    assert( layer>=0 && layer<=1);

    infoList.clear();

    if(!m_configured){
      ATH_MSG_WARNING("Unconfigured tool, unable to compute " << s_layerNames[layer] << " info");
      return false;
    }

    std::vector<const Trk::TrackParameters*> blayerParam;
    if(!getPixelLayerParameters(trackpar, blayerParam, layer)) return false;

    std::vector<const Trk::TrackParameters*>::const_iterator it = blayerParam.begin();
    for(; it !=blayerParam.end(); ++it){

      const Trk::TrackParameters* trkParam = *it;
      TrackStateOnBLayerInfo blayerInfo;

      double fracGood = getFracGood(trkParam, m_phiRegionSize, m_etaRegionSize);
      blayerInfo.goodFraction(fracGood);

      Identifier id;

      if(trkParam){
	id = trkParam->associatedSurface().associatedDetectorElement()->identify();
	blayerInfo.moduleId(id);
      }
      else{
	ATH_MSG_WARNING("Invalid TrackParameters - could not get Identifier");
	return false;
      }

      const InDetDD::SiDetectorElement* sielem = dynamic_cast<const InDetDD::SiDetectorElement*>(trkParam->associatedSurface().associatedDetectorElement());
      const Amg::Vector2D& locPos = trkParam->localPosition();

      if(sielem){
	Identifier holeId_c =sielem->identifierOfPosition(locPos);
	blayerInfo.pixelId(holeId_c);

	double etaDist = -9999;
	double phiDist = -9999;
	const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&sielem->design());
	if(design){
	  design->distanceToDetectorEdge(locPos, etaDist, phiDist); //// implicite cast from Trk::LocalPosition to SiLocalPosition
	}
	else{
	  ATH_MSG_WARNING (  "could not get pixel module design for  " <<   m_idHelper->show_to_string(id)  );
	}
	blayerInfo.distToModuleEdgePhi(phiDist);
	blayerInfo.distToModuleEdgeEta(etaDist);
      }
      else{
	ATH_MSG_WARNING ( " SiDetectorElement not found in TrackParameters" );
      }

      blayerInfo.globalPosition(trkParam->position());


      blayerInfo.localX(locPos[Trk::locX]);
      blayerInfo.localY(locPos[Trk::locY]);

      blayerInfo.theta(trkParam->parameters()[Trk::theta]);
      blayerInfo.phi(trkParam->parameters()[Trk::phi0]);

      float error_locx = -9999;
      float error_locy = -9999;

      error_locx = sqrt((*trkParam->covariance())(Trk::locX,Trk::locX));
      error_locy = sqrt((*trkParam->covariance())(Trk::locY,Trk::locY));


      blayerInfo.errLocalX(error_locx);
      blayerInfo.errLocalY(error_locy);

      bool isgood =  m_pixelCondSummarySvc->isGood(id,InDetConditions::PIXEL_MODULE);

      double phitol = 2.5;
      double etatol = 5.;
      if (trkParam) {
	phitol = 3.* sqrt((*trkParam->covariance())(Trk::locX,Trk::locX));
	etatol = 3.* sqrt((*trkParam->covariance())(Trk::locY,Trk::locY));
      }

      bool isIn=true;
      if(sielem){
	InDetDD::SiIntersect siIn = sielem->inDetector(locPos, phitol, etatol);
	isIn = siIn.in();
      }

      if(isgood){
	if(isIn)blayerInfo.type(insideGoodModule);
	else blayerInfo.type(nearGoodModuleEdge);
      }
      else{
	if(isIn)blayerInfo.type(insideBadModule);
	else blayerInfo.type(nearBadModuleEdge);
      }

      infoList.push_back(blayerInfo);

      delete trkParam;

    } /// blayer param

    return true;

  }

  bool InDet::InDetTestBLayerTool::getPixelLayerParameters(const Trk::TrackParameters* trackpar, std::vector<const Trk::TrackParameters*>& blayerParam, int layer) const{

    //// Cylinder bigger than the b-layer ////
    ATH_MSG_DEBUG("Trying to extrapolate to Pixel layer " << layer);

    Trk::CylinderSurface BiggerThanBLayerSurface (new Transform3D(Transform3D::Identity()),
						  100.0,
						  10000.0);

    // extrapolate stepwise to this parameter (be careful, sorting might be wrong)
    const std::vector<const Trk::TrackParameters*>* paramList =
      m_extrapolator->extrapolateStepwise(*trackpar,
					  BiggerThanBLayerSurface,
					  Trk::alongMomentum,
					  false);




   if(!paramList){
     ATH_MSG_DEBUG("No parameter returned by propagator ");
     ATH_MSG_VERBOSE("dumping track parameters " <<*trackpar);
     return false;
   }

   ATH_MSG_DEBUG(" Number of generated parameters by propagator: " << paramList->size() );


   int s_int = 0;
   for (std::vector<const Trk::TrackParameters*>::const_iterator it = paramList->begin();
	it != paramList->end(); ++it){

     ATH_MSG_DEBUG( s_int++ << "th surface : ");

     Identifier id;
     if( !( (*it)->associatedSurface().associatedDetectorElement() !=0 &&
	 (*it)->associatedSurface().associatedDetectorElement()->identify() !=0 ) ){
       delete *it;
       continue;
     }

     id = (*it)->associatedSurface().associatedDetectorElement()->identify();
     if (!m_idHelper->is_pixel(id)){
       delete *it;
       continue;
     }

     ATH_MSG_DEBUG("Found pixel module : Associated track parameter");
     if(!m_pixelId->is_barrel(id)){
       delete *it;
       continue;
     }
     ATH_MSG_DEBUG("Found pixel barrel");
     if(m_pixelId->layer_disk(id)!=layer) {
       delete *it;
       continue;
     }

     ATH_MSG_DEBUG( "Found layer  " <<  layer << " ID: " << id.get_compact() );
     blayerParam.push_back((*it));

   } /// all params

   delete paramList;
   return true;

  }


  double InDet::InDetTestBLayerTool::getFracGood(const Trk::TrackParameters* trkParam,
						 double phiRegionSize, double etaRegionSize) const{

    Identifier moduleid;

    if(trkParam){
      moduleid = trkParam->associatedSurface().associatedDetectorElement()->identify();
    }
    else {
      ATH_MSG_WARNING (  "Invalid TrackParameters - could not get Identifier"  );
      return 0;
    }

    IdentifierHash id_hash = m_pixelId->wafer_hash(moduleid);

    if( !m_pixelCondSummarySvc->isGood(id_hash) ){
      ATH_MSG_WARNING (  "Invalid Hash"  );
      return 0.;
    }

    const Amg::Vector2D& locPos = trkParam->localPosition();
    double locx = locPos[Trk::locX];
    double locy = locPos[Trk::locY];
    double error_locx = -9999;
    double error_locy = -9999;

    error_locx = sqrt((*trkParam->covariance())(Trk::locX,Trk::locX));
    error_locy = sqrt((*trkParam->covariance())(Trk::locY,Trk::locY));

    double phitol = 2.5;
    double etatol = 5.;

    phitol = phiRegionSize*error_locx;
    etatol = etaRegionSize*error_locy;

    const InDetDD::SiDetectorElement* sielem =
      dynamic_cast<const InDetDD::SiDetectorElement*>(trkParam->associatedSurface().associatedDetectorElement());

    if(sielem){
      const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&sielem->design());

      if (m_checkAtLeastNearestNeighbors){
	if(design){
	  phitol = std::max(phitol, design->phiPitch()+1e-6);
	  etatol = std::max(etatol, design->etaPitch()+1e-6);
	}
	else{
	  ATH_MSG_WARNING (  "could not get pixel module design "  );
	}
      }


      double startLocX = locx - phitol;
      double startLocY = locy - etatol;

      double endLocX = locx + phitol;
      double endLocY = locy + etatol;

      Amg::Vector2D startLocPostmp(startLocX,startLocY);
      Amg::Vector2D endLocPostmp(endLocX,endLocY);

      double etaDist = -9999;
      double phiDist = -9999;
      if(design){
	design->distanceToDetectorEdge(startLocPostmp, etaDist, phiDist);
	if(phiDist<0)startLocX += (fabs(phiDist)+1e-6);/// not exactly on the edge
	if(etaDist<0)startLocY += (fabs(etaDist)+1e-6);
	design->distanceToDetectorEdge(endLocPostmp, etaDist, phiDist);
	if(phiDist<0)endLocX-=(fabs(phiDist)+1e-6);
	if(etaDist<0)endLocY-=(fabs(etaDist)+1e-6);
      }
      else{
	ATH_MSG_WARNING (  "could not get pixel module design "  );
      }

      Amg::Vector2D startLocPos(startLocX,startLocY);
      Amg::Vector2D endLocPos(endLocX,endLocY);

      Identifier startId = sielem->identifierOfPosition(startLocPos);
      Identifier endId = sielem->identifierOfPosition(endLocPos);

      if(!startId.is_valid() || !endId.is_valid()){
	ATH_MSG_WARNING ( "unvalid identifier: locPos outside module! " );
	return 0.;
      }
      double frac = m_pixelCondSummarySvc->goodFraction(id_hash,startId,endId);
      return frac;
    }
    else{
      ATH_MSG_WARNING ( " SiDetectorElement not found in TrackParameters" );
      return 0;
    }

  }


} //end namespace

