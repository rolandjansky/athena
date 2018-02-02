/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrigTrackResidualMonitor/TrigTrackResidualMonitor.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkTrack/Track.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/ITHistSvc.h"
#include <TH1F.h>
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkValTools/TrackDiff.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkTrack/Track.h"
#include "TrkSurfaces/Surface.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/SiliconID.h"
#include "InDetIdentifier/PixelID.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

namespace InDet
{

  TrigTrackResidualMonitor::TrigTrackResidualMonitor(const std::string &name, 
					   ISvcLocator *pSvcLocator)
    : HLT::FexAlgo (name, pSvcLocator),
    
      m_allTracksFromStoreGate(0),
      m_monitoring(0),
      m_path("/EXPERT"),  

    
      m_trackSelectorTool("InDet::InDetDetailedTrackSelectorTool"),
      m_trkSummaryTool("Trk::TrackSummaryTool/InDetTrackSummaryTool"),
      m_updator("Trk::Updator"),
      
      m_propagator("Trk::Propagator"),
      //m_ParticleHypothesis(Trk::nonInteracting),
      m_ParticleHypothesis(Trk::pion),
    
      m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator")
       
  {
   
    declareProperty("doMonitoring",                   m_monitoring);
    declareProperty("histoPath",                      m_path);
    declareProperty("ResidualPullCalculator",         m_residualPullCalculator, "Tool to calculate residuals and pulls");
    declareProperty("TrackSelectorTool",              m_trackSelectorTool,"Tool to select the tracks");
    declareProperty("SummaryTool",                    m_trkSummaryTool);
    
   
    /////biased plots for residuals and pull measurements
    
    
    ///////////// Barrel Region //////////
    
    declareMonitoredStdContainer("ResidualPixellocXBiasedBarrel" , m_resPixellocXBiasedBarrel );
    declareMonitoredStdContainer("ResidualPixellocYBiasedBarrel" , m_resPixellocYBiasedBarrel );
    declareMonitoredStdContainer("PullPixellocXBiasedBarrel" ,     m_pullPixellocXBiasedBarrel );
    declareMonitoredStdContainer("PullPixellocYBiasedBarrel" ,     m_pullPixellocYBiasedBarrel );
    declareMonitoredStdContainer("ResidualSCTBiasedBarrel" ,       m_resSCTBiasedBarrel );
    declareMonitoredStdContainer("PullSCTBiasedBarrel" ,           m_pullSCTBiasedBarrel );
    
    
    /////////// EndCap A //////////
    
    declareMonitoredStdContainer("ResidualPixellocXBiasedEndCapA" , m_resPixellocXBiasedEndCapA );
    declareMonitoredStdContainer("ResidualPixellocYBiasedEndCapA" , m_resPixellocYBiasedEndCapA );
    declareMonitoredStdContainer("PullPixellocXBiasedEndCapA" ,     m_pullPixellocXBiasedEndCapA );
    declareMonitoredStdContainer("PullPixellocYBiasedEndCapA" ,     m_pullPixellocYBiasedEndCapA );
    declareMonitoredStdContainer("ResidualSCTBiasedEndCapA" ,       m_resSCTBiasedEndCapA );
    declareMonitoredStdContainer("PullSCTBiasedEndCapA" ,           m_pullSCTBiasedEndCapA );
    
    
    ///////////// EndCap C //////////

    declareMonitoredStdContainer("ResidualPixellocXBiasedEndCapC" , m_resPixellocXBiasedEndCapC );
    declareMonitoredStdContainer("ResidualPixellocYBiasedEndCapC" , m_resPixellocYBiasedEndCapC );
    declareMonitoredStdContainer("PullPixellocXBiasedEndCapC" ,     m_pullPixellocXBiasedEndCapC );
    declareMonitoredStdContainer("PullPixellocYBiasedEndCapC" ,     m_pullPixellocYBiasedEndCapC );
    declareMonitoredStdContainer("ResidualSCTBiasedEndCapC" ,       m_resSCTBiasedEndCapC );
    declareMonitoredStdContainer("PullSCTBiasedEndCapC" ,           m_pullSCTBiasedEndCapC );
    
    

    
   
    /////Unbiased plots for residuals and pull measurements
    
    
    /////////////Barrel Region//////////
    
   declareMonitoredStdContainer("ResidualPixellocXUnbiasedBarrel" , m_resPixellocXUnbiasedBarrel );
   declareMonitoredStdContainer("ResidualPixellocYUnbiasedBarrel" , m_resPixellocYUnbiasedBarrel );
   declareMonitoredStdContainer("PullPixellocXUnbiasedBarrel" ,     m_pullPixellocXUnbiasedBarrel );
   declareMonitoredStdContainer("PullPixellocYUnbiasedBarrel" ,     m_pullPixellocYUnbiasedBarrel );
   declareMonitoredStdContainer("ResidualSCTUnbiasedBarrel" ,       m_resSCTUnbiasedBarrel );
   declareMonitoredStdContainer("PullSCTUnbiasedBarrel" ,           m_pullSCTUnbiasedBarrel );
   

   ///////////// EndCap A  //////////
   
   
   declareMonitoredStdContainer("ResidualPixellocXUnbiasedEndCapA" , m_resPixellocXUnbiasedEndCapA );
   declareMonitoredStdContainer("ResidualPixellocYUnbiasedEndCapA" , m_resPixellocYUnbiasedEndCapA );
   declareMonitoredStdContainer("PullPixellocXUnbiasedEndCapA" ,     m_pullPixellocXUnbiasedEndCapA );
   declareMonitoredStdContainer("PullPixellocYUnbiasedEndCapA" ,     m_pullPixellocYUnbiasedEndCapA );
   declareMonitoredStdContainer("ResidualSCTUnbiasedEndCapA" ,       m_resSCTUnbiasedEndCapA );
   declareMonitoredStdContainer("PullSCTUnbiasedEndCapA" ,           m_pullSCTUnbiasedEndCapA );
   
   //////////// EndCap C //////////

   
   declareMonitoredStdContainer("ResidualPixellocXUnbiasedEndCapC" , m_resPixellocXUnbiasedEndCapC );
   declareMonitoredStdContainer("ResidualPixellocYUnbiasedEndCapC" , m_resPixellocYUnbiasedEndCapC );
   declareMonitoredStdContainer("PullPixellocXUnbiasedEndCapC" ,     m_pullPixellocXUnbiasedEndCapC );
   declareMonitoredStdContainer("PullPixellocYUnbiasedEndCapC" ,     m_pullPixellocYUnbiasedEndCapC );
   declareMonitoredStdContainer("ResidualSCTUnbiasedEndCapC" ,       m_resSCTUnbiasedEndCapC );
   declareMonitoredStdContainer("PullSCTUnbiasedEndCapC" ,           m_pullSCTUnbiasedEndCapC );
   
   


   
   ///Propagator and Updator
   
   declareProperty("UpdatorTool" ,      m_updator);
   declareProperty("propagator" ,       m_propagator);
   
   
   //// Track parameters
   
   declareMonitoredStdContainer("TrackPt" ,  m_TrackPt);
   declareMonitoredStdContainer("npix" ,     m_npix);
   declareMonitoredStdContainer("nsct" ,     m_nsct);
   declareMonitoredStdContainer("npixh" ,    m_npixh);
   declareMonitoredStdContainer("nscth" ,    m_nscth);
   

}
  
  TrigTrackResidualMonitor::~TrigTrackResidualMonitor()
  {}

  //----------------------------------
  //          beginRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigTrackResidualMonitor::hltBeginRun() {
    msg() << MSG::INFO << "TrigTrackResidualMonitor::beginRun()" << endmsg;

    return HLT::OK;
  }
  //----------------------------------------------------------------------------

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigTrackResidualMonitor::hltInitialize() {
    
    msg() << MSG::DEBUG << "initialize() success" << endmsg;
    
    //Track Selector Tool
    if ( m_trackSelectorTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve track selector tool " << m_trackSelectorTool << endmsg;
      return HLT::BAD_ALGO_CONFIG;
    }
    else{
      msg() << MSG::INFO << "Retrieved track selector tool " << m_trackSelectorTool << endmsg;
    }
    
    //Track Summary Tool
    if ( m_trkSummaryTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve track summary tool " <<  m_trkSummaryTool << endmsg;
      return HLT::BAD_ALGO_CONFIG;
    }
    else{
      msg() << MSG::INFO << "Retrieved track summary tool " << m_trkSummaryTool << endmsg;
    }
    
    
    //Residual Pool Calculator Tool 
    if ( m_residualPullCalculator.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve ResidualPull Calculator " << m_residualPullCalculator << endmsg;
      return HLT::BAD_ALGO_CONFIG;
    }
    else{
      msg() << MSG::INFO << "Retrieved ResidualPull Calculator " << m_residualPullCalculator << endmsg;
    }
    
    //detStore 
    StoreGateSvc* detStore(0);
    StatusCode sc = service("DetectorStore", detStore);
    if (sc.isFailure()){
      msg() << MSG::FATAL << "Detector service not found !" << endmsg;
      return HLT::BAD_ALGO_CONFIG;
    } 
    
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Detector service found" << endmsg;
    
    
    //idHelper ATLAS ID 
    if (detStore->retrieve(m_idHelper, "AtlasID").isFailure()) {
      msg() << MSG::FATAL << "Could not get Atlas ID helper" << endmsg;
      return HLT::BAD_ALGO_CONFIG;
    }
    else{
      msg() << MSG::DEBUG << "Atlas id Helper found !" << endmsg;
    }
    
    
    //Pixel ID 
    const PixelID * IdHelperPixel(0);
    if (detStore->retrieve(IdHelperPixel, "PixelID").isFailure()) {
      msg() << MSG::FATAL << "Could not get Pixel ID helper" << endmsg;
      return HLT::BAD_ALGO_CONFIG;
    }
    else{
      msg() << MSG::DEBUG << "Pixel Id Helper found !" << endmsg;
    }
    
    m_idHelperPixel = IdHelperPixel;
    

    //SCT ID 
    const SCT_ID * IdHelperSCT(0);
    if (detStore->retrieve(IdHelperSCT, "SCT_ID").isFailure()) {
      msg() << MSG::FATAL << "Could not get SCT ID helper" << endmsg;
      return HLT::BAD_ALGO_CONFIG;
    }
    else{
      msg() << MSG::DEBUG << "SCT Id Helper found !" << endmsg;
    }
    
    m_idHelperSCT = IdHelperSCT;
    

    ///SCT Manager
    if(detStore->retrieve(m_SCT_Manager, "SCT").isFailure()){
      msg() << MSG::FATAL   << "Could not get SCT_Manager !" << endmsg;
    }
    else{
      msg() << MSG::DEBUG << "SCT manager found !" << endmsg;
    }
    
    
    // Pixel Manager
    if(detStore->retrieve(m_Pixel_Manager, "Pixel").isFailure()){
      msg() << MSG::FATAL   << "Could not get Pixel_Manager !" << endmsg;
    }
    else{
      msg() << MSG::DEBUG << "Pixel manager found !" << endmsg;
    }
    
    
    //Get Updator
    if (m_updator.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Can not retrieve Updator of type  " << m_updator.typeAndName() << endmsg;
      return HLT::BAD_ALGO_CONFIG;
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " <<  m_updator.typeAndName() << endmsg;
    }
    
    
    
    // Get Propagator
    if (m_propagator.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Can not retrieve Updator of type  " << m_propagator.typeAndName() << endmsg;
      return HLT::BAD_ALGO_CONFIG;
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " <<  m_propagator.typeAndName() << endmsg;
    }
    
    return HLT::OK;
    
  }
  
  
  
  


  ///////////////////////////////////////////////////////////////////
  // Execute HLT Algorithm
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigTrackResidualMonitor::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
 
    int outputLevel = msgLvl();
    
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "execHLTAlgorithm()" << endmsg;
    
    //////  histogram variables /////////
    
    /////////////////////// Biased ////////////////////////////////
    
    
    //Barrel Region 
    m_resPixellocXBiasedBarrel.clear(); 
    m_resPixellocYBiasedBarrel.clear(); 
    m_pullPixellocXBiasedBarrel.clear();
    m_pullPixellocYBiasedBarrel.clear();
    m_resSCTBiasedBarrel.clear(); 
    m_pullSCTBiasedBarrel.clear();


    //EndCap A
    m_resPixellocXBiasedEndCapA.clear(); 
    m_resPixellocYBiasedEndCapA.clear(); 
    m_pullPixellocXBiasedEndCapA.clear();
    m_pullPixellocYBiasedEndCapA.clear();
    m_resSCTBiasedEndCapA.clear(); 
    m_pullSCTBiasedEndCapA.clear();


    //EndCap C
    m_resPixellocXBiasedEndCapC.clear();
    m_resPixellocYBiasedEndCapC.clear();
    m_pullPixellocXBiasedEndCapC.clear();
    m_pullPixellocYBiasedEndCapC.clear();
    m_resSCTBiasedEndCapC.clear(); 
    m_pullSCTBiasedEndCapC.clear();



    /////////////////////// UnBiased ////////////////////////////////

    //Barrel Region
    m_resPixellocXUnbiasedBarrel.clear();
    m_resPixellocYUnbiasedBarrel.clear();
    m_pullPixellocXUnbiasedBarrel.clear(); 
    m_pullPixellocYUnbiasedBarrel.clear(); 
    m_resSCTUnbiasedBarrel.clear(); 
    m_pullSCTUnbiasedBarrel.clear();


    //EndCap A
    m_resPixellocXUnbiasedEndCapA.clear(); 
    m_resPixellocYUnbiasedEndCapA.clear(); 
    m_pullPixellocXUnbiasedEndCapA.clear();
    m_pullPixellocYUnbiasedEndCapA.clear();
    m_resSCTUnbiasedEndCapA.clear(); 
    m_pullSCTUnbiasedEndCapA.clear();


    //EndCap C
    m_resPixellocXUnbiasedEndCapC.clear(); 
    m_resPixellocYUnbiasedEndCapC.clear(); 
    m_pullPixellocXUnbiasedEndCapC.clear();
    m_pullPixellocYUnbiasedEndCapC.clear();
    m_resSCTUnbiasedEndCapC.clear(); 
    m_pullSCTUnbiasedEndCapC.clear();


    m_TrackPt.clear(); 
    m_npix.clear(); 
    m_nsct.clear(); 
    m_npixh.clear();
    m_nscth.clear();

    //----------------------------------------------------------------------
    //  Navigate throw the trigger element to retrieve the last TrackCollection
    //----------------------------------------------------------------------
    
    //initialize monitored objects
    m_allTracksFromStoreGate = 0;
    if ( HLT::OK != getFeature(outputTE, m_allTracksFromStoreGate) ) {
      msg() << MSG::ERROR << " Input track collection could not be found " << endmsg;
      
      return HLT::NAV_ERROR;
    }
    if(!m_allTracksFromStoreGate){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input track collection was not attached. Algorithm not executed!" << endmsg;
      
      return HLT::OK; 
    } else {
      if(outputLevel <= MSG::VERBOSE)
	msg() << MSG::VERBOSE << " Input track collection has size " << m_allTracksFromStoreGate->size() << endmsg;
      if ( m_allTracksFromStoreGate->size() == 0 ) {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << " Input track collection has 0 size. Algorithm not executed!" << endmsg;
	return HLT::OK; 
      }
    }

    for (TrackCollection::const_iterator itResTrk = m_allTracksFromStoreGate->begin();
	 
	 itResTrk != m_allTracksFromStoreGate->end()   ; ++itResTrk) {
      
      DataVector<const Trk::TrackStateOnSurface>::const_iterator it= (*itResTrk)->trackStateOnSurfaces()->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator itEnd= (*itResTrk)->trackStateOnSurfaces()->end();
      
      const Trk::Track* track = *itResTrk;
      const Trk::TrackStateOnSurface* tsos;
      
      const Trk::TrackSummary *trkSum = track->trackSummary();
      
      if(!trkSum){
	Trk::Track* nonConstTrack = const_cast<Trk::Track*>(track);
	m_trkSummaryTool->updateTrack(*nonConstTrack);
	trkSum = nonConstTrack->trackSummary();
      }
      
      
      int npix, nsct, npixh, nscth, npixds,nsctds;
      npix = nsct = npixh = nscth = npixds = nsctds = -1;
      if(trkSum){
	nsct = trkSum->get(Trk::numberOfSCTHits);
	npix = trkSum->get(Trk::numberOfPixelHits);
	nscth = trkSum->get(Trk::numberOfSCTHoles);
	npixh = trkSum->get(Trk::numberOfPixelHoles);
	nsctds = trkSum->get(Trk::numberOfSCTDeadSensors);
	npixds = trkSum->get(Trk::numberOfPixelDeadSensors);
	
	m_npix.push_back(npix);
	m_nsct.push_back(nsct);
	m_npixh.push_back(npixh);
	m_nscth.push_back(nscth);
      }
      
      
      if (trkSum && npix >= 3 && nsct >= 6){
	const Trk::Perigee *trkPerig = track->perigeeParameters();
	double TrackPt = -1;
	if(trkPerig){
	  TrackPt = trkPerig->pT();
	  m_TrackPt.push_back(TrackPt);
	}
	if (trkPerig->pT() > 5000){
	  for( ; it!=itEnd; ++it ){
	    tsos = *it;
	    if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ){
	      const Trk::MeasurementBase* mesb=
		dynamic_cast <const Trk::MeasurementBase*>((*it)->measurementOnTrack());
	      const Trk::TrackParameters* trackPars = (*it)->trackParameters();
	      
	      const Trk::ResidualPull* resPullPixelBiased;// = 0;
	      const Trk::ResidualPull* resPullSCTBiased; //= 0;
	      
	      const Trk::ResidualPull* resPullPixelUnbiased;// = 0;
	      const Trk::ResidualPull* resPullSCTUnbiased; //= 0;
	      
	      int detType = 99;
	      
	      const Trk::MeasurementBase* mesh = (*it)->measurementOnTrack();
	      const Trk::RIO_OnTrack* hitbec = dynamic_cast <const Trk::RIO_OnTrack*>(mesh);
	      const Identifier & hitIdbec = hitbec->identify();
	      
	      
	      if (m_idHelper->is_pixel(hitIdbec)){
		detType = 0;
	      }
	      if (m_idHelper->is_sct(hitIdbec)) {
		detType = 1;
	      }
	      
	      if((mesb) != 0 && (trackPars) !=0){
		const Trk::TrackParameters* TrackParams(0);
		const Trk::TrackParameters* UnbiasedTrackParams(0);
		const Trk::TrackParameters* PropagatedTrackParams(0);
		const Trk::TrackParameters* OtherSideUnbiasedTrackParams(0);
		const Trk::TrackParameters* PropagatedPixelUnbiasedTrackParams(0);
		const Trk::TrackParameters* PixelUnbiasedTrackParams(0);
		
		const Trk::Surface* Surf = &(mesb->associatedSurface());
		const Identifier id =  Surf->associatedDetectorElementIdentifier();
		
		if ( id.is_valid()) {
		  int barrelECSCTUB = 99;
		  int barrelECSCTB = 99;		  
		  int barrelECPUB = 99;
		  int barrelECPB = 99;
		  if (detType == 1 ) {
		    //	if (m_idHelperSCT->is_sct(id)) {  //there's no TrueUnbiased for non-SCT (pixel) hits)  
		    
		    // check if other module side was also hit and try to remove other hit as well
		    const Identifier& idbECSCTUB = m_idHelperSCT->wafer_id(hitIdbec);
		    barrelECSCTUB = m_idHelperSCT->barrel_ec(idbECSCTUB);
		    const Trk::TrackStateOnSurface* OtherModuleSideHit(0);
		    const Identifier& OtherModuleSideID = m_SCT_Manager->getDetectorElement(id)->otherSide()->identify();
		    //const Trk::RIO_OnTrack* hit(0);
		    
		    for (const Trk::TrackStateOnSurface* TempTsos : *(*itResTrk)->trackStateOnSurfaces()) {
		      const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(TempTsos->measurementOnTrack());
		      //hit = hitOnTrack;
		      if (hitOnTrack != 0) {
			const Identifier& trkID = hitOnTrack->identify();
			if (m_idHelperSCT->wafer_id(trkID) == OtherModuleSideID) {
			  OtherModuleSideHit = TempTsos;
			}
		      }
		    }
		    
		    if (OtherModuleSideHit) {
		      if (OtherModuleSideHit->trackParameters()) {
			OtherSideUnbiasedTrackParams = m_updator->removeFromState(*(OtherModuleSideHit->trackParameters()),
										  OtherModuleSideHit->measurementOnTrack()->localParameters(),
										  OtherModuleSideHit->measurementOnTrack()->localCovariance());
			
			if (OtherSideUnbiasedTrackParams) {
			  const Trk::Surface* TempSurface = &(OtherModuleSideHit->measurementOnTrack()->associatedSurface());
			  const Trk::MagneticFieldProperties* TempField = 0;
			  if (TempSurface){
			    if (TempSurface->associatedLayer()){
			      if (TempSurface->associatedLayer()->enclosingTrackingVolume()){
				TempField = dynamic_cast <const Trk::MagneticFieldProperties*>(TempSurface->associatedLayer()->enclosingTrackingVolume());
			      }
			    }
			  } 
			  
			  PropagatedTrackParams = m_propagator->propagate(*OtherSideUnbiasedTrackParams,
									  tsos->measurementOnTrack()->associatedSurface(),
									  Trk::anyDirection, false,
									  *TempField,
									  m_ParticleHypothesis);
			  
			  delete OtherSideUnbiasedTrackParams;
			  
			  UnbiasedTrackParams = m_updator->removeFromState(*PropagatedTrackParams, 
									   tsos->measurementOnTrack()->localParameters(), 
									   tsos->measurementOnTrack()->localCovariance());
			  
			  delete PropagatedTrackParams;
			  if (UnbiasedTrackParams) {
			    TrackParams = UnbiasedTrackParams->clone(); 
			  }
			  
			  resPullSCTUnbiased =   m_residualPullCalculator->residualPull(mesb,TrackParams,Trk::ResidualPull::Unbiased);
			  if(barrelECSCTUB == 0) {  // Barrel region
			    m_resSCTUnbiasedBarrel.push_back(resPullSCTUnbiased->residual()[Trk::loc1]);
			    m_pullSCTUnbiasedBarrel.push_back(resPullSCTUnbiased->pull()[Trk::loc1]);
			  }
			  else if (barrelECSCTUB == 2){ // EndCap A
			    m_resSCTUnbiasedEndCapA.push_back(resPullSCTUnbiased->residual()[Trk::loc1]);
			    m_pullSCTUnbiasedEndCapA.push_back(resPullSCTUnbiased->pull()[Trk::loc1]);
			  }
			  else if (barrelECSCTUB == -2 ) { // EndCap A
			    m_resSCTUnbiasedEndCapC.push_back(resPullSCTUnbiased->residual()[Trk::loc1]);
			    m_pullSCTUnbiasedEndCapC.push_back(resPullSCTUnbiased->pull()[Trk::loc1]);
			  }
			  delete UnbiasedTrackParams;  	      
			} 
		      } 
		    } 
		  } // end of m_True Unbiased Loop
		  
		  
		  else if ( detType == 0 ) {
		    // else  if (m_idHelperPixel->is_pixel(id) ) {  
		    const Identifier& idbECPUB = m_idHelperPixel->wafer_id(hitIdbec);
		    barrelECPUB = m_idHelperPixel->barrel_ec(idbECPUB);
		    const Trk::TrackStateOnSurface* PixelSideHit(0);
		    PixelSideHit = *it;
		    
		    PropagatedPixelUnbiasedTrackParams = m_updator->removeFromState(*tsos->trackParameters(), 
										    tsos->measurementOnTrack()->localParameters(), 
										    tsos->measurementOnTrack()->localCovariance());
		    //  const Identifier& PixelID = m_Pixel_Manager->getDetectorElement(id)->identify();
		    const Trk::Surface* TempSurfacePixel = &(PixelSideHit->measurementOnTrack()->associatedSurface());
		    const Trk::MagneticFieldProperties* TempFieldPixel = 0;
		    
		    if (TempSurfacePixel){
		      if (TempSurfacePixel->associatedLayer()){
			if (TempSurfacePixel->associatedLayer()->enclosingTrackingVolume()){
			  TempFieldPixel = dynamic_cast <const Trk::MagneticFieldProperties*>(TempSurfacePixel->associatedLayer()->enclosingTrackingVolume());
			}
		      }
		    } 
		    
		    PixelUnbiasedTrackParams = m_propagator->propagate(*PropagatedPixelUnbiasedTrackParams,
								       tsos->measurementOnTrack()->associatedSurface(),
								       Trk::anyDirection, false,
								       *TempFieldPixel,
								       m_ParticleHypothesis);
		    delete PropagatedPixelUnbiasedTrackParams;		
		    
		    if (PixelUnbiasedTrackParams) {
		      TrackParams = PixelUnbiasedTrackParams->clone(); 
		    }
		    
		    resPullPixelUnbiased = m_residualPullCalculator->residualPull(mesb,TrackParams,Trk::ResidualPull::Unbiased);
		    if (barrelECPUB == 0){
		      m_resPixellocXUnbiasedBarrel.push_back(resPullPixelUnbiased->residual()[Trk::locX]);
		      m_resPixellocYUnbiasedBarrel.push_back(resPullPixelUnbiased->residual()[Trk::locY]);
		      m_pullPixellocXUnbiasedBarrel.push_back(resPullPixelUnbiased->pull()[Trk::locX]);
		      m_pullPixellocYUnbiasedBarrel.push_back(resPullPixelUnbiased->pull()[Trk::locY]);
		    }
		    else if (barrelECPUB == 2 ){
		      m_resPixellocXUnbiasedEndCapA.push_back(resPullPixelUnbiased->residual()[Trk::locX]);
		      m_resPixellocYUnbiasedEndCapA.push_back(resPullPixelUnbiased->residual()[Trk::locY]);
		      m_pullPixellocXUnbiasedEndCapA.push_back(resPullPixelUnbiased->pull()[Trk::locX]);
		      m_pullPixellocYUnbiasedEndCapA.push_back(resPullPixelUnbiased->pull()[Trk::locY]);
		    }
		    else if (barrelECPUB == -2 ) {
		      m_resPixellocXUnbiasedEndCapC.push_back(resPullPixelUnbiased->residual()[Trk::locX]);
		      m_resPixellocYUnbiasedEndCapC.push_back(resPullPixelUnbiased->residual()[Trk::locY]);
		      m_pullPixellocXUnbiasedEndCapC.push_back(resPullPixelUnbiased->pull()[Trk::locX]);
		      m_pullPixellocYUnbiasedEndCapC.push_back(resPullPixelUnbiased->pull()[Trk::locY]);
		    }
		  }
		  
		  
		  if ( detType == 0 ){
		    // if (m_idHelperPixel->is_pixel(id) ) {
		    const Identifier& idbECPB = m_idHelperPixel->wafer_id(hitIdbec);
		    barrelECPB = m_idHelperPixel->barrel_ec(idbECPB);
		    
		    resPullPixelBiased = m_residualPullCalculator->residualPull(mesb,trackPars,Trk::ResidualPull::Biased);
		    
		    if(barrelECPB == 0){
		      m_resPixellocXBiasedBarrel.push_back(resPullPixelBiased->residual()[Trk::locX]);
		      m_resPixellocYBiasedBarrel.push_back(resPullPixelBiased->residual()[Trk::locY]);
		      m_pullPixellocXBiasedBarrel.push_back(resPullPixelBiased->pull()[Trk::locX]);
		      m_pullPixellocYBiasedBarrel.push_back(resPullPixelBiased->pull()[Trk::locY]);
		    }
		    else if (barrelECPB == 2 ){
		      m_resPixellocXBiasedEndCapA.push_back(resPullPixelBiased->residual()[Trk::locX]);
		      m_resPixellocYBiasedEndCapA.push_back(resPullPixelBiased->residual()[Trk::locY]);
		      m_pullPixellocXBiasedEndCapA.push_back(resPullPixelBiased->pull()[Trk::locX]);
		      m_pullPixellocYBiasedEndCapA.push_back(resPullPixelBiased->pull()[Trk::locY]);
		    }
		    else if (barrelECPB == -2 ) {
		      m_resPixellocXBiasedEndCapC.push_back(resPullPixelBiased->residual()[Trk::locX]);
		      m_resPixellocYBiasedEndCapC.push_back(resPullPixelBiased->residual()[Trk::locY]);
		      m_pullPixellocXBiasedEndCapC.push_back(resPullPixelBiased->pull()[Trk::locX]);
		      m_pullPixellocYBiasedEndCapC.push_back(resPullPixelBiased->pull()[Trk::locY]);
		    }
		    
		    
		  } else if ( detType == 1 ) {
		    // } else if (m_idHelperSCT->is_sct(id)) {
		    const Identifier& idbECSCTB = m_idHelperSCT->wafer_id(hitIdbec);
		    barrelECSCTB = m_idHelperSCT->barrel_ec(idbECSCTB);
		    
		    resPullSCTBiased =   m_residualPullCalculator->residualPull(mesb,trackPars,Trk::ResidualPull::Biased);
		    
		    if(barrelECSCTB == 0){
		      m_resSCTBiasedBarrel.push_back(resPullSCTBiased->residual()[Trk::locX]);
		      m_pullSCTBiasedBarrel.push_back(resPullSCTBiased->pull()[Trk::locX]);
		    }
		    else if (barrelECSCTB == 2) {
		      m_resSCTBiasedEndCapA.push_back(resPullSCTBiased->residual()[Trk::locX]);
		      m_pullSCTBiasedEndCapA.push_back(resPullSCTBiased->pull()[Trk::locX]);
		    }
		    else if (barrelECSCTB == -2 ) {
		      m_resSCTBiasedEndCapC.push_back(resPullSCTBiased->residual()[Trk::locX]);
		      m_pullSCTBiasedEndCapC.push_back(resPullSCTBiased->pull()[Trk::locX]);
		    }
		  }
		} // is_valid, for detector types
	      } // if loop, track parameters ! = 0 or trackPars !=0
	    } // If loop   Trk::measurement
	  } // end of for loop over trackStateOnSurfaces
	} // end of the if loop for trkPerig->pT()
      }// end of trkSum and npix and nsct
    } //  end of loop over all tracks
    
    
    return HLT::OK;
  }
  
  


  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////
  
  HLT::ErrorCode TrigTrackResidualMonitor::hltFinalize() {

    msg() << MSG::DEBUG << "finalize() success" << endmsg;
    return HLT::OK;
  }

  //----------------------------------
  //          endRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigTrackResidualMonitor::hltEndRun() {
   
    msg() << MSG::INFO << "TrigTrackResidualMonitor::endRun()" << endmsg;
   
    return HLT::OK;
  }
  //---------------------------------------------------------------------------
} // end namespace


