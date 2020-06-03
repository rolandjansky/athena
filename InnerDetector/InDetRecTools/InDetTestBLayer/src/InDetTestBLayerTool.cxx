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

namespace InDet {
  
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
      msg(MSG::FATAL) << "Could not get AtlasDetectorID helper" << endreq;
      return StatusCode::FAILURE;
    }
    
    sc = detStore()->retrieve(m_pixelId, "PixelID");
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not get PixelID helper !" << endreq;
      return StatusCode::FAILURE;
    }
    
    m_configured=true;
    if( m_extrapolator.empty() ){
      msg(MSG::INFO) << "Extrapolator not configured " << endreq; //n
      m_configured=false;
    }
    else{
      if ( m_extrapolator.retrieve().isFailure() )
	{
	  msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq; //n
	  return StatusCode::FAILURE;
	}  else { 
	msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endreq; //n
      }
    }
    
    // Get PixelConditionsSummarySvc
    if( m_pixelCondSummarySvc.empty() ){
      msg(MSG::INFO) << "PixelConditionsSummarySvc not configured " << endreq; //n
      m_configured=false;
    }
    else{
      if ( m_pixelCondSummarySvc.retrieve().isFailure() ) {
	msg(MSG::FATAL) << "Failed to retrieve tool " << m_pixelCondSummarySvc << endreq; //n
	return StatusCode::FAILURE;
      } else {
	msg(MSG::INFO) << "Retrieved tool " << m_pixelCondSummarySvc << endreq; //n
      }
    }

    if(!m_configured){
      msg(MSG::INFO) << "you are using an unconfigured tool" << endreq; 
      msg(MSG::INFO) << "will not be able to extrapolate to the blayer" << endreq;
      msg(MSG::INFO) << "the values from the track summary will be returned" << endreq; 
    }
   
    if (m_residualPullCalculator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_residualPullCalculator << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_residualPullCalculator << endreq;
    }


    msg(MSG::VERBOSE) << " Initialization of InDetTestBLayerTool succesfull" << endreq;
    return StatusCode::SUCCESS;

  }
  
  StatusCode InDetTestBLayerTool::finalize() {
    ATH_MSG_DEBUG("Finalization of InDetTestBLayerTool succesfull");
    return StatusCode::SUCCESS;
  }

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::bLayerHitResidual(const Trk::TrackParticleBase* trackparticle) const
  {
    const Trk::Track* track = trackparticle->originalTrack();
    
    if (!track) {
      msg(MSG::DEBUG) << "No original track, residual calculation can not be performed" << endreq;
      return 0;
    }

    return(this->bLayerHitResidual(track));

  }

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::innermostPixelLayerHitResidual(const Trk::TrackParticleBase* trackparticle) const
  {
  
    const Trk::Track* track = trackparticle->originalTrack();
    
    if (!track) {
      msg(MSG::DEBUG) << "No original track, residual calculation can not be performed" << endreq;
      return 0;
    }

    return(this->innermostPixelLayerHitResidual(track));

  }

   const Trk::ResidualPull* InDet::InDetTestBLayerTool::nextToInnermostPixelLayerHitResidual(const Trk::TrackParticleBase* trackparticle) const
  {
  
    const Trk::Track* track = trackparticle->originalTrack();
    
    if (!track) {
      msg(MSG::DEBUG) << "No original track, residual calculation can not be performed" << endreq;
      return 0;
    }

    return(this->nextToInnermostPixelLayerHitResidual(track));

  }

 
  const Trk::ResidualPull* InDet::InDetTestBLayerTool::bLayerHitResidual(const Trk::Track* track) const
  {
   
    return (this->innermostPixelLayerHitResidual(track));
 
  }  

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::innermostPixelLayerHitResidual(const Trk::Track* track) const
  {
    //    const Trk::ResidualPull* residualPull=0;
    const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();    
    
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
	 it!=trackStates->end();
	 it++) {
      if (!(*it)) {
	msg(MSG::WARNING) << "TrackStateOnSurface == Null" << endreq;
	continue;
      }

      if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ){

	ATH_MSG_VERBOSE ("try to get measurement for track state");
	// Get pointer to measurement on track
	const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();


	if(  (*it)->trackParameters() !=0 &&
	     &((*it)->trackParameters()->associatedSurface()) !=0 &&  
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
		    if(m_pixelId->layer_disk(id) == 0) 
		      {
			if (msgLvl(MSG::DEBUG)) 
			  {
			    msg(MSG::DEBUG) << "Found Innermost Pixel Layer  " << id.get_compact() << endreq;	    
			  }
			
			return m_residualPullCalculator->residualPull(measurement,(*it)->trackParameters(),Trk::ResidualPull::Biased);
			
		      }
		  }
	      }
	  }
      }
    }
  
    return 0;
    
  }  

  const Trk::ResidualPull* InDet::InDetTestBLayerTool::nextToInnermostPixelLayerHitResidual(const Trk::Track* track) const
  {
    //    const Trk::ResidualPull* residualPull=0;
    const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();    
    
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
	 it!=trackStates->end();
	 it++) {
      if (!(*it)) {
	msg(MSG::WARNING) << "TrackStateOnSurface == Null" << endreq;
	continue;
      }

      if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ){

	ATH_MSG_VERBOSE ("try to get measurement for track state");
	// Get pointer to measurement on track
	const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();


	if(  (*it)->trackParameters() !=0 &&
	     &((*it)->trackParameters()->associatedSurface()) !=0 &&  
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
		    if(m_pixelId->layer_disk(id) == 1) 
		      {
			if (msgLvl(MSG::DEBUG)) 
			  {
			    msg(MSG::DEBUG) << "Found Next To Innermost Pixel Layer  " << id.get_compact() << endreq;	    
			  }
			
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
    if(!recompute){
      const Trk::TrackSummary* ts =  track->trackSummary();
      if(ts){
	int ehbl = ts->get(Trk::expectBLayerHit); 
	if(0==ehbl || 1==ehbl ){
	  ATH_MSG_DEBUG("Found expectHitInBLayer info in TrackSummary: return cached value" );
	  return ehbl;
	}
      }
    }
    else{
      ATH_MSG_DEBUG("Forced to recompute expectHitInBLayer info" );
    }

    ATH_MSG_DEBUG("computing expectHitInBLayer info" );

    const Trk::Perigee* mp = track->perigeeParameters();
    
    if (!mp)
      {
	ATH_MSG_WARNING("Found Track with no perigee parameters: no b-layer info will be provided " );
	return false;
      } 
    else 
      {
	ATH_MSG_DEBUG("Track perigee parameters");
	return this->expectHitInBLayer(mp);
      }
  }

   bool InDet::InDetTestBLayerTool::expectHitInInnermostPixelLayer(const Trk::Track* track, bool recompute) const 
  {
    if(!recompute){
      const Trk::TrackSummary* ts =  track->trackSummary();
      if(ts){
	int ehbl = ts->get(Trk::expectInnermostPixelLayerHit); 
	if(0==ehbl || 1==ehbl ){
	  ATH_MSG_DEBUG("Found expectHitInInnermostPixelLayer info in TrackSummary: return cached value" );
	  return ehbl;
	}
      }
    }
    else{
      ATH_MSG_DEBUG("Forced to recompute expectHitInInnermostPixelLayer info" );
    }

    ATH_MSG_DEBUG("computing expectHitInInnermostPixelLayer info" );

    const Trk::Perigee* mp = track->perigeeParameters();
    
    if (!mp)
      {
	ATH_MSG_WARNING("Found Track with no perigee parameters: no Innermost Pixel Layer info will be provided " );
	return false;
      } 
    else 
      {
	ATH_MSG_DEBUG("Track perigee parameters");
	return this->expectHitInInnermostPixelLayer(mp);
      }
  }


  bool InDet::InDetTestBLayerTool::expectHitInNextToInnermostPixelLayer(const Trk::Track* track, bool recompute) const 
  {
    if(!recompute){
      const Trk::TrackSummary* ts =  track->trackSummary();
      if(ts){
	int ehbl = ts->get(Trk::expectNextToInnermostPixelLayerHit); 
	if(0==ehbl || 1==ehbl ){
	  ATH_MSG_DEBUG("Found expectHitInNextToInnermostPixelLayer info in TrackSummary: return cached value" );
	  return ehbl;
	}
      }
    }
    else{
      ATH_MSG_DEBUG("Forced to recompute expectHitInNextToInnermostPixelLayer info" );
    }

    ATH_MSG_DEBUG("computing expectHitInNextToInnermostPixelLayer info" );

    const Trk::Perigee* mp = track->perigeeParameters();
    
    if (!mp)
      {
	ATH_MSG_WARNING("Found Track with no perigee parameters: no Next To Innermost Pixel Layer info will be provided " );
	return false;
      } 
    else 
      {
	ATH_MSG_DEBUG("Track perigee parameters");
	return this->expectHitInNextToInnermostPixelLayer(mp);
      }
  }


  
  bool InDet::InDetTestBLayerTool::expectHitInBLayer(const Trk::TrackParticleBase* track, bool recompute) const
  {
    if(!recompute){
      const Trk::TrackSummary* ts =  track->trackSummary();
      if(ts){
	int ehbl = ts->get(Trk::expectBLayerHit); 
	if(0==ehbl || 1==ehbl ){
	  ATH_MSG_DEBUG("Found expectHitInBLayer info in TrackSummary: return cached value" );
	  return ehbl;
	}
      }
    }
    else{
      ATH_MSG_DEBUG("Forced to recompute expectHitInBLayer info" );
    }

    ATH_MSG_DEBUG("computing expectHitInBLayer info" );

    const Trk::Perigee* mp = track->perigee();

    if(!mp)
      {
	ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no b-layer info will be provided");
	return false;
      } 
    else
      {
	ATH_MSG_DEBUG("TrackParticle perigee parameters");
	//	  mp->dump(mLog);
	return (this->expectHitInBLayer(mp));
      }
  }

  bool InDet::InDetTestBLayerTool::expectHitInInnermostPixelLayer(const Trk::TrackParticleBase* track, bool recompute) const
  {
    
    if(!recompute){
      const Trk::TrackSummary* ts =  track->trackSummary();
      if(ts){
	int ehbl = ts->get(Trk::expectInnermostPixelLayerHit); 
	if(0==ehbl || 1==ehbl ){
	  ATH_MSG_DEBUG("Found expectHitInInnerMostPixelLayer info in TrackSummary: return cached value" );
	  return ehbl;
	}
      }
    }
    else{
      ATH_MSG_DEBUG("Forced to recompute expectHitInInnermostPixelLayer info" );
    }
    
    ATH_MSG_DEBUG("computing expectHitInInnermostPxielLayer info" );
    
    const Trk::Perigee* mp = track->perigee();
    
    if(!mp)
      {
	ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no Innermost Pixel info will be provided");
	return false;
      } 
    else
      {
	ATH_MSG_DEBUG("TrackParticle perigee parameters");
	//	  mp->dump(mLog);
	return (this->expectHitInInnermostPixelLayer(mp));
      }
  }


   bool InDet::InDetTestBLayerTool::expectHitInNextToInnermostPixelLayer(const Trk::TrackParticleBase* track, bool recompute) const
  {
    
    if(!recompute){
      const Trk::TrackSummary* ts =  track->trackSummary();
      if(ts){
	int ehbl = ts->get(Trk::expectNextToInnermostPixelLayerHit); 
	if(0==ehbl || 1==ehbl ){
	  ATH_MSG_DEBUG("Found expectHitInNextToInnermostPxielLayer info in TrackSummary: return cached value" );
	  return ehbl;
	}
      }
    }
    else{
      ATH_MSG_DEBUG("Forced to recompute expectHitInNextToInnermostLayer info" );
    }
    
    ATH_MSG_DEBUG("computing expectHitInNexttoInnermostPixelLayer info" );
    
    const Trk::Perigee* mp = track->perigee();
    
    if(!mp)
      {
	ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no NextToInnermost info will be provided");
	return false;
      } 
    else
      {
	ATH_MSG_DEBUG("TrackParticle perigee parameters");
	//	  mp->dump(mLog);
	return (this->expectHitInNextToInnermostPixelLayer(mp));
      }
  }
  


  bool InDet::InDetTestBLayerTool::expectHitInBLayer(const Trk::TrackParameters* trackpar) const
  {
    if(!m_configured){
      ATH_MSG_WARNING("Unconfigured tool, unable to compute expectHitInBLayer");
      return false;
    }


    bool expect_hit = false; /// will be set to true if at least on good module is passed

    //// Cylinder bigger than the b-layer ////
    

    std::vector<const Trk::TrackParameters*> blayerParam;
    if(!this->getPixelLayerParameters(trackpar, blayerParam, 0)) return false;	

    std::vector<const Trk::TrackParameters*>::const_iterator it = blayerParam.begin();

    for(; it !=blayerParam.end(); ++it){

      Identifier id = (*it)->associatedSurface().associatedDetectorElement()->identify();

      if( m_pixelCondSummarySvc->isGood(id,InDetConditions::PIXEL_MODULE) ){

	if( m_checkActiveAreas ){

	  if( isActive(*it) ){

	    if(m_checkDeadRegions){

	      double fracGood = getFracGood(*it, m_phiRegionSize, m_etaRegionSize);
	      if(fracGood>m_goodFracCut){
		ATH_MSG_DEBUG("Condition Summary: b-layer good");
		expect_hit=true;  /// pass good module -> hit is expected on blayer
	      }
	      else{
		ATH_MSG_DEBUG("b-layer in dead region");
	      }

	    }
	    else{ /// check dead regios
	      ATH_MSG_DEBUG("Condition Summary: b-layer good");
	      expect_hit=true;  /// pass good module -> hit is expected on blayer
	    }

	  }
	  else{
	    ATH_MSG_DEBUG("Condition Summary: b-layer good but outside active area");
	  }

	} /// check active area (check edges)
	else{
	  ATH_MSG_DEBUG("Condition Summary: b-layer good, active areas not checked");
	  expect_hit=true; /// pass good module -> hit is expected on blayer
	}

      }
      else{
	ATH_MSG_DEBUG("b-layer not good");
      }

      delete *it;

    } /// blayer param


    return expect_hit;
    
  }

  bool InDet::InDetTestBLayerTool::expectHitInInnermostPixelLayer(const Trk::TrackParameters* trackpar) const
  {
    
    if(!m_configured){
      ATH_MSG_WARNING("Unconfigured tool, unable to compute expectHitInInnermostPixelLayer");
      return false;
    }


    bool expect_hit = false; /// will be set to true if at least on good module is passed

    //// Cylinder bigger than the b-layer ////
    

    std::vector<const Trk::TrackParameters*> blayerParam;
    if(!this->getPixelLayerParameters(trackpar, blayerParam,0)) return false;	

    std::vector<const Trk::TrackParameters*>::const_iterator it = blayerParam.begin();

    for(; it !=blayerParam.end(); ++it){

      Identifier id = (*it)->associatedSurface().associatedDetectorElement()->identify();

      if( m_pixelCondSummarySvc->isGood(id,InDetConditions::PIXEL_MODULE) ){

	if( m_checkActiveAreas ){

	  if( isActive(*it) ){

	    if(m_checkDeadRegions){

	      double fracGood = getFracGood(*it, m_phiRegionSize, m_etaRegionSize);
	      if(fracGood>m_goodFracCut){
		ATH_MSG_DEBUG("Condition Summary: Innermost Layer good");
		expect_hit=true;  /// pass good module -> hit is expected on blayer
	      }
	      else{
		ATH_MSG_DEBUG("b-layer in dead region");
	      }

	    }
	    else{ /// check dead regios
	      ATH_MSG_DEBUG("Condition Summary: Innermost Layer good");
	      expect_hit=true;  /// pass good module -> hit is expected on blayer
	    }

	  }
	  else{
	    ATH_MSG_DEBUG("Condition Summary: Innermost Layer good but outside active area");
	  }

	} /// check active area (check edges)
	else{
	  ATH_MSG_DEBUG("Condition Summary: Innermost Layer good, active areas not checked");
	  expect_hit=true; /// pass good module -> hit is expected on blayer
	}

      }
      else{
	ATH_MSG_DEBUG("Innermost Layer not good");
      }

      delete *it;

    } /// blayer param


    return expect_hit;
    
  }
		    

  bool InDet::InDetTestBLayerTool::expectHitInNextToInnermostPixelLayer(const Trk::TrackParameters* trackpar) const
  {
    
    if(!m_configured){
      ATH_MSG_WARNING("Unconfigured tool, unable to compute expectHitInNextToInnermostPixelLayer");
      return false;
    }


    bool expect_hit = false; /// will be set to true if at least on good module is passed

    //// Cylinder bigger than the b-layer ////
    

    std::vector<const Trk::TrackParameters*> blayerParam;
    if(!this->getPixelLayerParameters(trackpar, blayerParam,1)) return false;	

    std::vector<const Trk::TrackParameters*>::const_iterator it = blayerParam.begin();

    for(; it !=blayerParam.end(); ++it){

      Identifier id = (*it)->associatedSurface().associatedDetectorElement()->identify();

      if( m_pixelCondSummarySvc->isGood(id,InDetConditions::PIXEL_MODULE) ){

	if( m_checkActiveAreas ){

	  if( isActive(*it) ){

	    if(m_checkDeadRegions){

	      double fracGood = getFracGood(*it, m_phiRegionSize, m_etaRegionSize);
	      if(fracGood>m_goodFracCut){
		ATH_MSG_DEBUG("Condition Summary: NExt To Innermost Layer good");
		expect_hit=true;  /// pass good module -> hit is expected on blayer
	      }
	      else{
		ATH_MSG_DEBUG("b-layer in dead region");
	      }

	    }
	    else{ /// check dead regios
	      ATH_MSG_DEBUG("Condition Summary: Next To Innermost Layer good");
	      expect_hit=true;  /// pass good module -> hit is expected on blayer
	    }

	  }
	  else{
	    ATH_MSG_DEBUG("Condition Summary: Next To Innermost Layer good but outside active area");
	  }

	} /// check active area (check edges)
	else{
	  ATH_MSG_DEBUG("Condition Summary: Next To Innermost Layer good, active areas not checked");
	  expect_hit=true; /// pass good module -> hit is expected on blayer
	}

      }
      else{
	ATH_MSG_DEBUG(" Next To Innermost Layer not good");
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
	 if (msgLvl(MSG::DEBUG)) 
	   {
	     msg(MSG::DEBUG) << "---> extrapolation on bond gap within " << etatol << ", return" << endreq;
	   }
       } else if (!siIn.in()) {
	 if (msgLvl(MSG::DEBUG)) 
	   { 
	     msg(MSG::DEBUG) << "---> extrapolation not inside (active?) detector within "<< phitol << " " << 
				   etatol << ", return" << endreq;
	   }
       } else {
	 return true;
       }
     }
    
    return false;

  }



  bool InDet::InDetTestBLayerTool::getTrackStateOnBlayerInfo(const Trk::Track* track, std::vector<TrackStateOnBLayerInfo>& infoList)  const{
  
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
      ATH_MSG_WARNING("Found Track with no perigee parameters: no b-layer info will be provided" );
      return false;
    }

     bool succeed = getTrackStateOnBlayerInfo(startParameters, infoList);
     delete startParameters;
     return succeed;
  }


   bool InDet::InDetTestBLayerTool::getTrackStateOnInnermostPixelLayerInfo(const Trk::Track* track, std::vector<TrackStateOnBLayerInfo>& infoList)  const{
   
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
      ATH_MSG_WARNING("Found Track with no perigee parameters: no Innermost Pixel layer info will be provided" );
      return false;
    }

    bool succeed = getTrackStateOnInnermostPixelLayerInfo(startParameters, infoList);
     delete startParameters;
     return succeed;
  }


   bool InDet::InDetTestBLayerTool::getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::Track* track, std::vector<TrackStateOnBLayerInfo>& infoList)  const{
   
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
      ATH_MSG_WARNING("Found Track with no perigee parameters: no Next to Innermost Pixel layer info will be provided" );
      return false;
    }

    bool succeed = getTrackStateOnNextToInnermostPixelLayerInfo(startParameters, infoList);
     delete startParameters;
     return succeed;
  }


  bool InDet::InDetTestBLayerTool::getTrackStateOnBlayerInfo(const Trk::TrackParticleBase* track, std::vector<TrackStateOnBLayerInfo>& infoList)  const{
  
   const Trk::Perigee* startParameters = track->perigee();

    if(!startParameters){
      ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no b-layer info will be provided");
      return false;
    } 
 
    return getTrackStateOnBlayerInfo(startParameters, infoList);
    
  }

  bool InDet::InDetTestBLayerTool::getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParticleBase* track, std::vector<TrackStateOnBLayerInfo>& infoList)  const{
    
   const Trk::Perigee* startParameters = track->perigee();

    if(!startParameters){
      ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no Innermost Pixel layer info will be provided");
      return false;
    } 
 
    return getTrackStateOnInnermostPixelLayerInfo(startParameters, infoList);
    
  }


   bool InDet::InDetTestBLayerTool::getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::TrackParticleBase* track, std::vector<TrackStateOnBLayerInfo>& infoList)  const{
    
   const Trk::Perigee* startParameters = track->perigee();

    if(!startParameters){
      ATH_MSG_WARNING("Found TrackParticle with no perigee parameters: no Next To Innermost Pixel layer info will be provided");
      return false;
    } 
 
    return getTrackStateOnNextToInnermostPixelLayerInfo(startParameters, infoList);
    
  }



  bool InDet::InDetTestBLayerTool::getTrackStateOnBlayerInfo(const Trk::TrackParameters* trackpar, std::vector<TrackStateOnBLayerInfo>& infoList)  const{
 
    infoList.clear();


    if(!m_configured){
      ATH_MSG_WARNING("Unconfigured tool, unable to compute BLayer info");
      return false;
    }

    std::vector<const Trk::TrackParameters*> blayerParam;
    if(!getPixelLayerParameters(trackpar, blayerParam, 0)) return false;

    std::vector<const Trk::TrackParameters*>::const_iterator it = blayerParam.begin();
    for(; it !=blayerParam.end(); ++it){

      const Trk::TrackParameters* trkParam = *it;           
      TrackStateOnBLayerInfo blayerInfo;
        
      double fracGood = getFracGood(trkParam, m_phiRegionSize, m_etaRegionSize);
      blayerInfo.goodFraction(fracGood);

      Identifier id = trkParam->associatedSurface().associatedDetectorElement()->identify();
      blayerInfo.moduleId(id);

      const InDetDD::SiDetectorElement* sielem = dynamic_cast<const InDetDD::SiDetectorElement*>(trkParam->associatedSurface().associatedDetectorElement());
//     const Trk::Surface *aS = trkParam->associatedSurface();
//     const Trk::LocalPosition locPos = *(aS->globalToLocal(trkParam->position()));
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

      if(trkParam){
        error_locx = sqrt((*trkParam->covariance())(Trk::locX,Trk::locX));
        error_locy = sqrt((*trkParam->covariance())(Trk::locY,Trk::locY));
      }
      else{
	ATH_MSG_DEBUG ( "could not MeasuredTrackParameters for hole  " <<   m_idHelper->show_to_string(id)  );
      }

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

  bool InDet::InDetTestBLayerTool::getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, std::vector<TrackStateOnBLayerInfo>& infoList)  const{
   
    infoList.clear();


    if(!m_configured){
      ATH_MSG_WARNING("Unconfigured tool, unable to compute Innermost Pixel Layer info");
      return false;
    }

    std::vector<const Trk::TrackParameters*> blayerParam;
    if(!getPixelLayerParameters(trackpar, blayerParam, 0)) return false;

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

  bool InDet::InDetTestBLayerTool::getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, std::vector<TrackStateOnBLayerInfo>& infoList)  const{
   
    infoList.clear();


    if(!m_configured){
      ATH_MSG_WARNING("Unconfigured tool, unable to compute Next To Innermost Pixel Layer info");
      return false;
    }

    std::vector<const Trk::TrackParameters*> blayerParam;
    if(!getPixelLayerParameters(trackpar, blayerParam, 1)) return false;

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
//     const Trk::Surface *aS = trkParam->associatedSurface();
//     const Trk::LocalPosition locPos = *(aS->globalToLocal(trkParam->position()));
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

