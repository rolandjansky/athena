/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ListItem.h"

#include "TrigL2TRTSegFinder/TrigTRTSegFinder.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "TrkSegment/SegmentCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "IRegionSelector/IRegSelSvc.h" 
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include <cmath>

namespace {
  template <class DEST,class SRC>
  inline
  DEST** trigTRTSegFinder_cast(SRC** ptr){
    return (DEST**)(ptr);
  }
}


TrigTRTSegFinder::TrigTRTSegFinder(const std::string& name,ISvcLocator* pSvcLocator) 
  : HLT::FexAlgo(name, pSvcLocator),
    m_robDataProvider("ROBDataProviderSvc", name),
    m_pRegionSelector("RegSelSvc", name),
    m_proptool("Trk::RungeKuttaPropagator"),
    m_magFieldProperties(0),
    m_magFieldSvc("AtlasFieldSvc",name)
{
  declareProperty("IROBDataProviderSvc", m_robDataProvider);
  declareProperty("IRegSelSvc", m_pRegionSelector);
  declareProperty("SegmentsMakerTool",m_segmentsmaker);
  declareProperty("TrackExtensionTool",m_trackextension);
  declareProperty("RoIhalfWidthDeltaPhi"    ,m_roiDeltaPhi                = 0.1 );
  declareProperty("RoIhalfWidthDeltaEta"    ,m_roiDeltaEta                = 0.1 );
  declareProperty("PropagatorTool"        ,m_proptool);
  declareProperty("TrigDriftCircleProvider" ,m_trtDCtool);
  declareProperty( "AthenaFieldService", m_magFieldSvc, "AtlasFieldSvc");
  declareProperty("pTmin" ,m_pTmin = 500.);

  // declare monitoring histograms

  typedef const DataVector<TrigInDetTrack> trigTRTSegFinderDV_t;

  declareMonitoredCollection("trk_nStrawsTRT", *trigTRTSegFinder_cast<trigTRTSegFinderDV_t>(&m_recoTracks), &TrigInDetTrack::NStrawHits);
  declareMonitoredCollection("trk_nTRsTRT",    *trigTRTSegFinder_cast<trigTRTSegFinderDV_t>(&m_recoTracks), &TrigInDetTrack::NTRHits);

  declareMonitoredCollection("trk_pt",      *trigTRTSegFinder_cast<trigTRTSegFinderDV_t>(&m_recoTracks), &monPt);
  declareMonitoredCollection("trk_a0",      *trigTRTSegFinder_cast<trigTRTSegFinderDV_t>(&m_recoTracks), &mona0);
  declareMonitoredCollection("trk_z0",      *trigTRTSegFinder_cast<trigTRTSegFinderDV_t>(&m_recoTracks), &monz0);
  declareMonitoredCollection("trk_phi",     *trigTRTSegFinder_cast<trigTRTSegFinderDV_t>(&m_recoTracks), &monPhi);
  declareMonitoredCollection("trk_eta",     *trigTRTSegFinder_cast<trigTRTSegFinderDV_t>(&m_recoTracks), &monEta);

  declareMonitoredVariable("roi_nTracks",m_ntracks);
  declareMonitoredStdContainer("dataErrors",m_dataErrors);
  declareMonitoredStdContainer("dataErrors_TRT",m_trtDataErrors);
		  
}


HLT::ErrorCode TrigTRTSegFinder::hltInitialize()
{
  if (msgLvl() <= MSG::DEBUG){
    msg() << MSG::DEBUG << " in Initialize" << endmsg;
  }
  m_ntracks = 0;

  // get Tools service
  StatusCode sc;
  
  // m_segmentsmaker   = 0;
  

  sc = m_segmentsmaker.retrieve();
  if( sc.isFailure() ) {
    msg()<<MSG::FATAL<<"Could not get TRT_TrackSegmentsMaker tool "<<m_segmentsmaker<<endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  sc = m_trackextension.retrieve();
  if( sc.isFailure() ) {
    msg()<<MSG::FATAL<<"Could not get TRT_TrackExtension tool "<<m_trackextension<<endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  // Get output print level
  if ( msgLvl() <= MSG::DEBUG ) {
    m_nprint = 0; 
    msg() << MSG::DEBUG << (*this) << endmsg;
  }
  
  // Get the region selector Service
  //sc = service("RegSelSvc", m_pRegionSelector);

  if(m_pRegionSelector.retrieve().isFailure()){                                 
    msg() << MSG::FATAL << "Unable to retrieve RegionSelector Service" << endmsg; 
    return HLT::OK;
  }

  //---- Propagator (for conversion segment->track->InDetTrack :(  
  // Get propagator tool
  //
  
  sc = m_proptool.retrieve();
  
  if(sc.isFailure() || !m_proptool) {
    msg() << MSG::FATAL << "Could not get Propagator Tool " << m_proptool << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  // Get tool for magnetic field service
  
  if (m_magFieldSvc.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve Tool " << m_magFieldSvc << ". Exiting."<<endmsg;
        return StatusCode::FAILURE;
  }
  // Build MagneticFieldProperties 
  m_magFieldProperties = new Trk::MagneticFieldProperties();
  if(!m_magFieldProperties) {
    msg() << MSG::FATAL << "Could not build Trk::MagneticFieldProperties" << endmsg; 
    return HLT::BAD_JOB_SETUP; 
  }

  if (m_trtDCtool.retrieve().isFailure()){
    msg() << MSG::FATAL << "Could not get TrigTRT_DriftCircleProviderTool tool" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if ( timerSvc() ) {
    m_totalTimer   = addTimer("Total",  "EtaRoi");
    m_relSelTimer  = addTimer("RegSel", "nIds");
    m_segFindTimer = addTimer("SegFind");
    m_trkConvTimer = addTimer("TrkCnv");
    m_DCTimer      = addTimer("DC");
  }

  // Get ROBDataProvider
  if (m_robDataProvider.retrieve().isFailure()){
    //if ( StatusCode::SUCCESS != service("ROBDataProviderSvc", m_robDataProvider ) ) {
    msg() << MSG::ERROR << " Can't get ROBDataProviderSvc " << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  return HLT::OK;
}


HLT::ErrorCode TrigTRTSegFinder::hltExecute(const HLT::TriggerElement* inputTE, 
					    HLT::TriggerElement* outputTE)
{
  if( timerSvc() ) m_totalTimer->start();
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " inputTE->getId(): " <<  inputTE->getId() << endmsg;
    msg() << MSG::DEBUG << "outputTE->getId(): " << outputTE->getId() << endmsg;
  }
  
  m_recoTracks = NULL;
  m_dataErrors.clear();
  m_trtDataErrors.clear();
  const TrigRoiDescriptor* pEmTauRoI = 0;
  HLT::ErrorCode stat = getFeature( inputTE, pEmTauRoI, "");
  if (stat != HLT::OK) pEmTauRoI = 0;

  if ( pEmTauRoI ) {
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << " REGTEST RoI id " << pEmTauRoI->roiId()
	    << " located at phi = " <<  pEmTauRoI->phi()
	    << ", eta = " << pEmTauRoI->eta() << endmsg;
  } else {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " <<endmsg;
    if( timerSvc() ) m_totalTimer->stop();
    return stat;
  }
  
  if ( timerSvc() ) m_relSelTimer->start();

 
  std::vector<IdentifierHash> listTrtIds;
  
  m_pRegionSelector->DetHashIDList(TRT,*pEmTauRoI,listTrtIds);
  
  if( timerSvc() ) { 
    m_relSelTimer->propVal(listTrtIds.size());
    m_relSelTimer->stop();
  }  
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << listTrtIds.size() << " REGTEST: TRT Dets in region" << endmsg;

    if (msgLvl() <= MSG::VERBOSE) {
      msg() << MSG::VERBOSE <<"  List of TRT Layers Ids: "<<endmsg;
      for (unsigned int i=0; i<listTrtIds.size(); i++ ) {
	msg() << MSG::VERBOSE <<" "<<listTrtIds[i];                    }
      msg() << MSG::VERBOSE <<endmsg;
    }  
  }
  
  if( timerSvc() ) m_DCTimer->start();
  std::vector<unsigned int> uIntListOfTRTRobs;
  m_pRegionSelector->DetROBIDListUint(TRT,*pEmTauRoI,uIntListOfTRTRobs);
  std::vector<const ROBF*> V;
  m_robDataProvider->addROBData(uIntListOfTRTRobs);
  m_robDataProvider->getROBData(uIntListOfTRTRobs,V);

  StatusCode sc = m_trtDCtool->fillCollections(listTrtIds,V);

  if (sc.isRecoverable() ){
    if (msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << "Errors filling DrftCircle Collections: Recovering "<<endmsg;
    }
    m_dataErrors.push_back(1);//"recoverable" bin
    const std::vector<int>* errVect = m_trtDCtool->fillTRT_DataErrors();
    std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_trtDataErrors));
  }
  else if ( sc.isFailure() ) {
    msg() << MSG::WARNING << "Stopped filling DriftCircle Collections: Failing " << endmsg;
    m_dataErrors.push_back(0);//"Failure" bin
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::CORRUPTED_ROD);
  }
    
  if( timerSvc() ) {
    m_DCTimer->stop();
    m_segFindTimer->start();
  }

  m_segmentsmaker->newRegion(listTrtIds);
  m_segmentsmaker->find(); 

  Trk::Segment* segment = 0;
  m_nsegments           = 0;
  
  // Loop through all segments and reconsrtucted segments collection preparation
  
  Trk::SegmentCollection* foundSegments  = new Trk::SegmentCollection;
  
  while((segment = m_segmentsmaker->next())) {
    ++m_nsegments; foundSegments->push_back(segment);
  }
  
  m_segmentsmaker->endEvent();

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST: Found " << m_nsegments << " TRT segments" << endmsg;
  }

  if( timerSvc() ) {
    m_segFindTimer->stop();
    m_trkConvTimer->start();
  }
 
  // if(m_nsegments >0 ) ???

  m_recoTracks = new TrigInDetTrackCollection;

  ConvertTRT( *foundSegments);
  //if (m_nsegments >0) for (unsigned int i=0; i<foundSegments->size(); i++) { delete (*foundSegments)[i]; } //
  delete foundSegments;

  if( timerSvc() ) m_trkConvTimer->stop();
 
  //status = writeHistoryOfTE(outputTE, uses, foundSegments);
  //std::string key;
  //HLT::ErrorCode status = reAttachFeature(outputTE, tracks, key, "TRTSegmentFinder");

  HLT::ErrorCode status =   attachFeature(outputTE, m_recoTracks, "TRTSegmentFinder");


  if ( status != HLT::OK ){
    msg() << MSG::ERROR << "Could not write history of TE!!!" << endmsg;
    return status;
  }
 
  m_ntracks = m_recoTracks->size();

  // Print common event information
  //
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST: Stored " << m_recoTracks->size() << " tracks in SG." << endmsg;
    //msg() << MSG::DEBUG << "Track collection " << key << endmsg; 
    m_nprint = 1;
  }

  // delete m_recoTracks to prevent memory leak!
  //delete m_recoTracks;<--- do not delete it otherwise monitoring will crash !

  if ( timerSvc() ) m_totalTimer->stop();
  return HLT::OK;    
}


HLT::ErrorCode TrigTRTSegFinder::hltFinalize() 
{

  return HLT::OK;
}


///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& operator    << 
  (MsgStream& sl,const TrigTRTSegFinder& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& operator << 
  (std::ostream& sl,const TrigTRTSegFinder& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& TrigTRTSegFinder::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out);
  return dumptools(out);
}
///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& TrigTRTSegFinder::dumptools( MsgStream& out ) const
{
  int n = 65-m_segmentsmaker.name().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  out<<"| Tool for TRT track segments finding             | "<<m_segmentsmaker<<s1
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& TrigTRTSegFinder::dumpevent( MsgStream& out ) const
{
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  out<<"| TRT track segments found                        |"
     <<std::setw(7)<<m_nsegments
     <<"                                       |"
     <<std::endl;
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& TrigTRTSegFinder::dump( std::ostream& out ) const
{
  return out;
}
 
//
// convert TRT segments into TrigInDetTracks
//
void TrigTRTSegFinder::ConvertTRT(const Trk::SegmentCollection& bColl) 
{
  int nTrk = 0;
  double theta;
  
  for(Trk::SegmentCollection::const_iterator trkIter = bColl.begin(); 
      trkIter != bColl.end(); ++trkIter) { 

    // Have Segment. Want Track...
    const Trk::StraightLineSurface* surf = 
      dynamic_cast<const Trk::StraightLineSurface*>(&(*trkIter)->associatedSurface());

    Trk::AtaStraightLine aSL((*trkIter)->localParameters()[Trk::loc1],
			     (*trkIter)->localParameters()[Trk::loc2],
			     (*trkIter)->localParameters()[Trk::phi],
			     (*trkIter)->localParameters()[Trk::theta],
			     (*trkIter)->localParameters()[Trk::qOverP],
			     *surf );

    const Amg::Vector3D globalPos( 0, 0, 0 );

    Trk::PerigeeSurface periSurf(globalPos);

    const Trk::TrackParameters* trackPars = 
      m_proptool->propagateParameters(aSL, periSurf, Trk::oppositeMomentum, false, *m_magFieldProperties, Trk::pion);
    if (!trackPars)  
      trackPars = m_proptool->propagateParameters(aSL, periSurf, Trk::alongMomentum, false, *m_magFieldProperties, Trk::pion);
     
    if (!trackPars)  
      trackPars = m_proptool->propagateParameters(aSL, periSurf, Trk::anyDirection, false, *m_magFieldProperties, Trk::pion);

    double a0Vert =0.0;
    double zVert = 0.0;
    double phiVert = 0.0;
    double cotThVert = 0.0;
    double pTVert = 0.0;
    int nHits = 0;
    int nTR = 0,nTime=0,nStraw=0;
    bool validDT=false;
    if(trackPars){
     
      a0Vert    = trackPars->parameters()[0];
      zVert     = trackPars->parameters()[1];
      phiVert   = trackPars->parameters()[2];
      cotThVert = 1.0/tan(trackPars->parameters()[Trk::theta]);
      pTVert    = trackPars->pT();

      theta  = atan ( fabs(1/cotThVert) ) ;
      theta = tan(0.5*theta) ;
      theta = -log(theta) ; if( cotThVert < 0 ) theta= -theta;
      nHits = (*trkIter)->numberOfMeasurementBases();
      nTR=0, nTime=0, nStraw=0;
      validDT=false;

    }
    else {
      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "trackPars is not valid!!!"<<endmsg;
      }
    }
  
    //Finished with trackPars and have to prevent memory leak by deleting objects pointed to by trackPars
    delete trackPars;

    if( fabs(pTVert) < m_pTmin ) 
      {
	if (msgLvl() <= MSG::DEBUG) 
	  msg() << MSG::DEBUG << "pT= " << pTVert << " is too low - skipping TrigInDetTrack formation " << endmsg; 
	continue;//do not create track if pT is too low
      }
   
    std::vector<const Trk::MeasurementBase*>::const_iterator 
      s  = (*trkIter)->containedMeasurements().begin(),
      se = (*trkIter)->containedMeasurements().end();
    std::vector<const InDet::TRT_DriftCircle*>* pDC=new std::vector<const InDet::TRT_DriftCircle*>;
    
    for(++s; s!=se; ++s) {
      if (!(dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(*s))) continue;
//      const InDet::TRT_DriftCircle* dc = ((const InDet::TRT_DriftCircleOnTrack*)(*s))->prepRawData();
      const InDet::TRT_DriftCircle* dc = (dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(*s))->prepRawData();
      if(dc!=0){
	pDC->push_back(dc);
	if(dc->highLevel()) nTR++;
	dc->driftTime(validDT);
	if(validDT) nTime++;
      }
    }
    ++nTrk;
    
    const TrigInDetTrackFitPar* fitPar = 
      new TrigInDetTrackFitPar( a0Vert, phiVert, zVert, theta, pTVert, NULL);

    TrigInDetTrack*  track = new TrigInDetTrack(fitPar, 0, 0);
    
    //delete fitPar;//dont' need fitPar anymore <--- TrigInDetTrack owns fitPar and will take care of its deletion

    //track->algorithmId(TrigInDetTrack::TRTXKID);
    track->algorithmId(TrigInDetTrack::TRTLUTID);
    track->StrawHits(nHits); // ??? is it correct ??
    track->StrawTime(nTime);
    track->TRHits(nTR);

    if(pDC->size() > 0) track->trtDriftCircles(pDC); 
    else delete pDC;// <--- the same issue - TrigInDetTrack owns DC collection
     
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST / TRT track: pT= " << pTVert << " phi0= " << phiVert 
	    << " z0= " << zVert <<" a0= "<<a0Vert<< " eta= " << theta <<  endmsg;

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST / nHits= " << nHits << " nStraw= " << nStraw
	    << " nTR= " << nTR << " nTime= " << nTime << endmsg;
    
    m_recoTracks->push_back(track);
    
    //delete (*trkIter);
  }
  //delete (*bColl);
}

