/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDAlignMonNtuple.cxx
// AUTHORS: Ben Cooper
// **********************************************************************

#include "IDAlignMonNtuple.h"

#include <sstream>
#include <math.h>
#include "TMath.h"

#include "GaudiKernel/IJobOptionsSvc.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
//#include "IdDictDetDescr/IdDictManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"


#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruth.h"
#include "TrkTruthData/TrackTruthCollection.h"
//#include "TrkTruthToTrack/TruthToTrack.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"

#include "VxVertex/VxContainer.h"

#include "TrkToolInterfaces/IUpdator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

static const int s_n_maxTracks = 1500;
static const int s_n_maxHits      = 3000;
static const int s_n_maxEventHits = 50000;
static const int s_n_ERRORVALUE = 99999;


// *********************************************************************
// Public Methods
// *********************************************************************

IDAlignMonNtuple::IDAlignMonNtuple( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_doPulls(false),
   m_idHelper(0),
   m_SCT_Mgr(0),
   m_pixelID(0),
   m_sctID(0),
   m_ntupleSvc(0),
   m_ntuple(0)
{
  m_iUpdator = ToolHandle<Trk::IUpdator>("Trk::KalmanUpdator");
  m_propagator = ToolHandle<Trk::IPropagator>("Trk::RungeKuttaPropagator");
  m_truthToTrack = ToolHandle<Trk::ITruthToTrack>("Trk::TruthToTrack/InDetTruthToTrack");
  m_residualPullCalculator = ToolHandle<Trk::IResidualPullCalculator>("Trk::ResidualPullCalculator/ResidualPullCalculator");

  declareProperty("tracksName",m_tracksName="ExtendedTracks");
  declareProperty("tracksTruthName",m_tracksTruthName="ExtendedTracksTruthCollection");
  declareProperty("CheckRate",m_checkrate=1000);
  declareProperty("TruthToTrackTool",m_truthToTrack);
  declareProperty("TrulyUnBiasedSCT",m_unbiasedSCT=true);
  declareProperty("UsePrepRawData",m_usePRD=false);
  declareProperty("ResidualPullCalculatorTool", m_residualPullCalculator);
  declareProperty("VxPrimContainerName"  , m_VxPrimContainerName);

}

//---------------------------------------------------------------------------------------

IDAlignMonNtuple::~IDAlignMonNtuple() { }

//---------------------------------------------------------------------------------------


StatusCode IDAlignMonNtuple::initialize()
{
  
  //initialize tools and services
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Calling initialize() to setup tools/services" << endmsg;
  StatusCode sc = setupTools();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to initialize tools/services!" << endmsg;
    return StatusCode::FAILURE;
  } 
  else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Successfully initialized tools/services" << endmsg;

  if(m_unbiasedSCT) msg(MSG::INFO) << "Using Truly unbiased SCT residuals" << endmsg;
  if(m_usePRD) msg(MSG::INFO) << "Using PrepRawData for hits information" << endmsg;   
     
  return StatusCode::SUCCESS;
}


StatusCode IDAlignMonNtuple::bookHistograms()
{
  StatusCode sc;

  //if ( newLowStatFlag() ) {    }
  //if ( newLumiBlockFlag() ) {  }
  if ( newRunFlag() ) {  }
  
  std::string directoryStructure = "/NTUPLES/ALIGNMONITOR";
  //std::string fullNtuplePath = "/NTUPLES/ALIGNMONITOR/Alignment/tree";
  std::string fullNtuplePath = "/NTUPLES/ALIGNMONITOR/" + m_tracksName + "/tree";
  //NTupleFilePtr file( m_ntupleSvc, directoryStructure );
  NTuplePtr nt(m_ntupleSvc, fullNtuplePath );
  
  //booking m_ntuple
  if (!nt)    {    // Check if already booked
    
    nt = m_ntupleSvc->book(fullNtuplePath, CLID_ColumnWiseTuple,"tree" );
    if(nt) {
      
      m_ntuple=nt;
      msg(MSG::INFO) << "Alignment monitoring m_ntuple booked." << endmsg;
      
      //information per event
      sc = m_ntuple->addItem("event_ntracks",m_nt_ntrks,0,s_n_maxTracks);		
      sc = m_ntuple->addItem("event_nhits",m_nt_nhits,0,s_n_maxEventHits);
      sc = m_ntuple->addItem("event_nvtx",m_nt_nvtx,0,1000);	
      sc = m_ntuple->addItem("event_goodvtxfound",m_nt_goodvtx,0,1000);	
      sc = m_ntuple->addItem("event_vtxntrks",m_nt_vtxntrks,0,s_n_maxTracks);	
      sc = m_ntuple->addItem("event_vtxX",m_nt_vtxX,-1000,1000);	
      sc = m_ntuple->addItem("event_vtxY",m_nt_vtxY,-1000,1000);	
      sc = m_ntuple->addItem("event_vtxZ",m_nt_vtxZ,-1000,1000);	
	
      //information per track
      sc = m_ntuple->addIndexedItem("track_nhits",m_nt_ntrks,m_nt_trknhits);
      sc = m_ntuple->addIndexedItem("track_qoverpt",m_nt_ntrks,m_nt_trkqoverpt);
      sc = m_ntuple->addIndexedItem("track_eta",m_nt_ntrks,m_nt_trketa);
      sc = m_ntuple->addIndexedItem("track_theta",m_nt_ntrks,m_nt_trktheta);
      sc = m_ntuple->addIndexedItem("track_phi",m_nt_ntrks,m_nt_trkphi);
      sc = m_ntuple->addIndexedItem("track_d0",m_nt_ntrks,m_nt_trkd0);
      sc = m_ntuple->addIndexedItem("track_z0",m_nt_ntrks,m_nt_trkz0);
      sc = m_ntuple->addIndexedItem("track_charge",m_nt_ntrks,m_nt_trkcharge);
      sc = m_ntuple->addIndexedItem("track_chi2",m_nt_ntrks,m_nt_trkchi2);
      sc = m_ntuple->addIndexedItem("track_dof",m_nt_ntrks,m_nt_trkdof);
      sc = m_ntuple->addIndexedItem("track_vtxd0",m_nt_ntrks,m_nt_trkvtxd0);

      //truth information per track
      sc = m_ntuple->addIndexedItem("track_istruth",m_nt_ntrks,m_nt_trkistruth);
      sc = m_ntuple->addIndexedItem("track_truthprob",m_nt_ntrks,m_nt_trktruthprob);
      sc = m_ntuple->addIndexedItem("track_truthpt",m_nt_ntrks,m_nt_trktruthpt);
      sc = m_ntuple->addIndexedItem("track_truthphi",m_nt_ntrks,m_nt_trktruthphi);
      sc = m_ntuple->addIndexedItem("track_trutheta",m_nt_ntrks,m_nt_trktrutheta);
      sc = m_ntuple->addIndexedItem("track_truthpdg",m_nt_ntrks,m_nt_trktruthpdg);
      
      sc = m_ntuple->addIndexedItem("track_truthphi0",m_nt_ntrks,m_nt_trktruthphi0);
      sc = m_ntuple->addIndexedItem("track_truthd0",m_nt_ntrks,m_nt_trktruthd0);
      sc = m_ntuple->addIndexedItem("track_truthz0",m_nt_ntrks,m_nt_trktruthz0);
      sc = m_ntuple->addIndexedItem("track_trutheta0",m_nt_ntrks,m_nt_trktrutheta0);
      sc = m_ntuple->addIndexedItem("track_truthqoverpt",m_nt_ntrks,m_nt_trktruthqoverpt);
      sc = m_ntuple->addIndexedItem("track_truthpt0",m_nt_ntrks,m_nt_trktruthpt0);
      sc = m_ntuple->addIndexedItem("track_truthcharge",m_nt_ntrks,m_nt_trktruthcharge);
      sc = m_ntuple->addIndexedItem("track_truthvtxX",m_nt_ntrks,m_nt_trktruthvtxX);
      sc = m_ntuple->addIndexedItem("track_truthvtxY",m_nt_ntrks,m_nt_trktruthvtxY);
      sc = m_ntuple->addIndexedItem("track_truthvtxZ",m_nt_ntrks,m_nt_trktruthvtxZ);

      //int max_hits = 5000;//do not make this smaller!

      //information per hit per track
      sc = m_ntuple->addItem("hit_dettype",m_nt_ntrks,m_nt_dettype,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_isbarrel",m_nt_ntrks,m_nt_isbarrel,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_layer",m_nt_ntrks,m_nt_layer,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_modphi",m_nt_ntrks,m_nt_hitmodphi,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_modeta",m_nt_ntrks,m_nt_hitmodeta,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_hitx",m_nt_ntrks,m_nt_hitx,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_hity",m_nt_ntrks,m_nt_hity,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_residualx",m_nt_ntrks,m_nt_residualx,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_residualy",m_nt_ntrks,m_nt_residualy,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_biasedresidualx",m_nt_ntrks,m_nt_biasedresidualx,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_biasedresidualy",m_nt_ntrks,m_nt_biasedresidualy,s_n_maxHits);	
      sc = m_ntuple->addItem("hit_hittype",m_nt_ntrks,m_nt_hittype,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_errorx",m_nt_ntrks,m_nt_errorx,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_errory",m_nt_ntrks,m_nt_errory,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_hitxwidth",m_nt_ntrks,m_nt_hitxwidth,s_n_maxHits);		
      sc = m_ntuple->addItem("hit_hitywidth",m_nt_ntrks,m_nt_hitywidth,s_n_maxHits);	
      sc = m_ntuple->addItem("hit_hitolegwidth",m_nt_ntrks,m_nt_hitolegwidth,s_n_maxHits);
      sc = m_ntuple->addItem("hit_incidangle",m_nt_ntrks,m_nt_hitincidangle,s_n_maxHits);

    } else { 
      msg(MSG::ERROR) << "Failed to book Alignment monitoring m_ntuple." << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------

StatusCode IDAlignMonNtuple::fillHistograms()
{
  
  //-------------------------------------------------------------
  //looking at vertex reconstruction


  float xv=0.0;
  float yv=0.0;
  float zv=0.0;
  const VxContainer* vertices;
  int ntrkMax=0;
  int nVtx = 0;
  
  if (evtStore()->contains<VxContainer>(m_VxPrimContainerName)) {
    StatusCode scv = evtStore()->retrieve (vertices,m_VxPrimContainerName);
    if (scv.isFailure()) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No Collection with name  "<<m_VxPrimContainerName<<" found in StoreGate" << endmsg;
      return StatusCode::SUCCESS;
    } else {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Collection with name  "<<m_VxPrimContainerName<< " with size " << vertices->size() <<" found  in StoreGate" << endmsg;
  
      VxContainer::const_iterator vxItr  = vertices->begin();
      VxContainer::const_iterator vxItrE = vertices->end();    
      nVtx = vertices->size();
      for (; vxItr != vxItrE; ++vxItr) {
	int numTracksPerVertex = (*vxItr)->vxTrackAtVertex()->size();
	if (numTracksPerVertex>ntrkMax) {
	  ntrkMax=numTracksPerVertex;
	  xv=(*vxItr)->recVertex().position()[0];
	  yv=(*vxItr)->recVertex().position()[1];
	  zv=(*vxItr)->recVertex().position()[2];
	}
      }
    }
  } else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "StoreGate does not contain VxPrimaryCandidate Container" << endmsg;
  

  if (xv==-999 || yv==-999 || zv==-999) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No vertex found => setting it to 0"<<endmsg;
    xv=0;yv=0;zv=0;
  }

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Best vertex (" << ntrkMax << " Tracks) x, y, z = " << xv << ", " <<  yv << ", " << zv << endmsg;

  m_nt_nvtx = nVtx;
  m_nt_vtxntrks = ntrkMax;
  //these quantities are only 0,0,0 if something has gone wrong with vtx finding
  //so we don't want these in the m_ntuple
  if(!(xv==0.0 && yv==0.0 && zv==0.0)){
    m_nt_vtxX = xv;   m_nt_vtxY = yv;   m_nt_vtxZ = zv;
    m_nt_goodvtx = 1;
  }
  else {
    m_nt_vtxX = 999;   m_nt_vtxY = 999;   m_nt_vtxZ = 999;
    m_nt_goodvtx = 0;
  }

  //-------------------------------------------------------------

  if (m_tracksName.empty()) msg(MSG::ERROR) << " no track collection given"<<endmsg;

  //const Rec::TrackParticleContainer* tracks        = new Rec::TrackParticleContainer;
  const TrackCollection* tracks;//        = new TrackCollection;
  
  StatusCode sc = evtStore()->retrieve(tracks,m_tracksName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "No TrackCollection with name "<<m_tracksName<<" found in StoreGate" << endmsg;
    return sc;
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<m_tracksName<<" found in StoreGate" << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< tracks->size() <<" reconstructed tracks from StoreGate" << endmsg;
  }

  const TrackTruthCollection  * truthMap  = NULL;

  sc = evtStore()->retrieve(truthMap, m_tracksTruthName);
  if (sc.isFailure()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackTruthCollection with name "<< m_tracksTruthName <<" found in StoreGate" << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Truth information will not be filled in the AlignMonNtuple" << endmsg;
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Collection with name "<< m_tracksTruthName <<" found in StoreGate" << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< truthMap->size() <<" truth tracks from StoreGate" << endmsg;
  }
    

  int nTracks = 0;
  int nHitsEvent = 0;

  TrackCollection::const_iterator trackItr  = tracks->begin();
  TrackCollection::const_iterator trackItrE = tracks->end();
  //Rec::TrackParticleContainer::const_iterator trackItr  = tracks->begin();
  //Rec::TrackParticleContainer::const_iterator trackItrE = tracks->end();
  for (; trackItr != trackItrE && nTracks < s_n_maxTracks; ++trackItr) { //looping over tracks

    
    //need to get the Trk::Track object from which the TrackParticle object was created
    //this has the hit information
    
    //const Trk::Track* track = (*trackItr)->originalTrack();
    const Trk::Track* track = *trackItr;
    if(track == NULL){
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No associated Trk::Track object found for track "<< nTracks << endmsg;
      continue;
    }

    //sets all track m_ntuple variables to error values
    //variables will be overwritten later if can be defined
    setTrackErrorValues(nTracks);

    //trackStateOnSurfaces is a vector of Trk::TrackStateOnSurface objects which contain information 
    //on track at each (inner)detector surface it crosses eg hit used to fit track
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Track = " << nTracks << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found " << track->trackStateOnSurfaces()->size() << " TrkSurfaces " << endmsg;

    int nHits = 0;

    //looping over the hits
    for (const Trk::TrackStateOnSurface* tsos : *track->trackStateOnSurfaces()) {

      float residualX = s_n_ERRORVALUE; float residualY = s_n_ERRORVALUE;
      float biasedResidualX = s_n_ERRORVALUE; float biasedResidualY = s_n_ERRORVALUE;
      float errorX = s_n_ERRORVALUE; float errorY = s_n_ERRORVALUE;
      float hitX = s_n_ERRORVALUE; float hitY = s_n_ERRORVALUE; 
      int detType = s_n_ERRORVALUE; int barrelEC = s_n_ERRORVALUE;
      int layerDisk = s_n_ERRORVALUE; int modEta = s_n_ERRORVALUE;
      int modPhi = s_n_ERRORVALUE; int hitType = s_n_ERRORVALUE;
      int phiWidth = s_n_ERRORVALUE; int zWidth = s_n_ERRORVALUE;
      int olegWidth = s_n_ERRORVALUE; float trkIncidAngle = s_n_ERRORVALUE;

      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TSOS (hit) = " << nHits << endmsg;       

      if (tsos == nullptr) continue;

      //determining type of TSOS object - can represent many different track elements
      if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trk::TrackStateOnSurface::Measurement" << endmsg;
	hitType = 0;
      }
      else if (tsos->type(Trk::TrackStateOnSurface::InertMaterial)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trk::TrackStateOnSurface::InertMaterial" << endmsg;
	hitType = 1;
      }
      else if (tsos->type(Trk::TrackStateOnSurface::BremPoint)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trk::TrackStateOnSurface::BremPoint" << endmsg;
	hitType = 2;
      }
      else if (tsos->type(Trk::TrackStateOnSurface::Scatterer)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trk::TrackStateOnSurface::Scatterer" << endmsg;
	hitType = 3;
      }
      else if (tsos->type(Trk::TrackStateOnSurface::Perigee)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trk::TrackStateOnSurface::Perigee" << endmsg;
	hitType = 4;
      }
      else if (tsos->type(Trk::TrackStateOnSurface::Outlier)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trk::TrackStateOnSurface::Outlier" << endmsg;
	hitType = 5;
      }
      else if (tsos->type(Trk::TrackStateOnSurface::Hole)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trk::TrackStateOnSurface::Hole" << endmsg;
	hitType = 6;
      }
      else if (tsos->type(Trk::TrackStateOnSurface::Unknown)) {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Trk::TrackStateOnSurface::Unknown" << endmsg;
	hitType = 7;
      }
      else if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unrecognised Trk::TrackStateOnSurface Type" << endmsg;


      const Trk::MeasurementBase* mesh =tsos->measurementOnTrack();
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Defined  hit MeasurementBase " << endmsg;
         
      //Trk::RIO_OnTrack object contains information on the hit used to fit the track at this surface
      const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(mesh);

      if (hit== NULL) { 
	//for some reason the first tsos has no associated hit - maybe because this contains the defining parameters?
	if (nHits > 0) if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No hit associated with TrkSurface - probably a hole"<< nHits << endmsg; 
	continue;
      }
      
      //if desired we can use PrepRawData hits information i.e. before insitu calibration of hits
      const InDet::SiCluster* hitPRD;
      if(m_usePRD){
	hitPRD = dynamic_cast <const InDet::SiCluster*>(hit->prepRawData());
      } else {hitPRD = NULL;}
 
      const Identifier & hitId = hit->identify();
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Defined  hit Identifier " << endmsg;
      if (m_idHelper->is_pixel(hitId)) {
	detType = 0;
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "This is a Pixel Hit" << endmsg;
      }
      if (m_idHelper->is_sct(hitId)) {
	detType = 1;
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "This is an SCT Hit" << endmsg;
      }
      if (m_idHelper->is_trt(hitId)) {
	detType = 2;
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "This is a TRT Hit" << endmsg;
      }
      
      //finding local error on hit
      if(m_usePRD && hitPRD != NULL){
	errorX = Amg::error(hitPRD->localCovariance(),Trk::locX);
	errorY = Amg::error(hitPRD->localCovariance(),Trk::locY);
      }
      else {
	errorX = Amg::error(hit->localCovariance(),Trk::locX);
	errorY = Amg::error(hit->localCovariance(),Trk::locY);
      }
      
      if (detType==0) {//getting pixel hit information

	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " This is a PIXEL hit " << hitId << endmsg;
	const Identifier& id = m_pixelID->wafer_id(hitId);

	barrelEC = m_pixelID->barrel_ec(id);
	layerDisk = m_pixelID->layer_disk(id);
	modEta = m_pixelID->eta_module(id);
	modPhi = m_pixelID->phi_module(id);

	const InDet::SiCluster* pCluster = dynamic_cast <const InDet::SiCluster*>(hit->prepRawData());
	if ( pCluster != NULL ){
	  InDet::SiWidth width = pCluster->width();
	  phiWidth = int(width.colRow().x());
	  zWidth = int(width.colRow().y());
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pixel hit phi cluster width = " << phiWidth << endmsg;
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pixel hit z cluster width = " << zWidth << endmsg;
	}
      }
      
      
      if (detType==1) {//getting SCT hit information 

	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " This is an SCT hit " << hitId << endmsg;
	const Identifier& id = m_sctID->wafer_id(hitId);

	barrelEC = m_sctID->barrel_ec(id);
	layerDisk = m_sctID->layer_disk(id);
	modEta = m_sctID->eta_module(id);
	modPhi = m_sctID->phi_module(id);

	const InDet::SiCluster* pCluster = dynamic_cast <const InDet::SiCluster*>(hit->prepRawData());
	if ( pCluster != NULL ){
	  InDet::SiWidth width = pCluster->width();
	  phiWidth = int(width.colRow().x());
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SCT hit phi cluster width = " << phiWidth << endmsg;
	}
      }
      
      if (detType==0 || detType==1) {//have identified pixel or SCT hit
	
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Hit is pixel or SCT, finding residuals... " << endmsg;

	const Trk::TrackParameters* trackParameter = tsos->trackParameters();

	//alternative method used by Oleg in his HitQualitySelection tool to get cluster width
	const std::vector<Identifier> idVec = (hit->prepRawData())->rdoList() ;
	olegWidth = idVec.size() ;
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Oleg Si clusterSize = " << olegWidth << endmsg ;

	//finding residuals
	if(trackParameter){

	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found Trk::TrackParameters" << endmsg;	 

	  //finding track incidence angle (taken from InDetAlignTools/InDetAlignHitQualSelectTool)
	  const InDetDD::SiDetectorElement *detEle = dynamic_cast<const InDetDD::SiDetectorElement*>( hit->detectorElement() ) ;
	  
	  if ( detEle != NULL ){
	    Amg::Vector3D trkDir       = trackParameter->momentum() ;
	    Amg::Vector3D detElePhi    = detEle->phiAxis() ; //!< local x axis in global frame
	    Amg::Vector3D detEleNormal = detEle->normal() ; //!< local z axis in global frame
	    double trkDotPhi = trkDir.dot( detElePhi ) ; //!< scalar product
	    double trkDotNormal = trkDir.dot( detEleNormal ) ;
	    trkIncidAngle = (float)atan( trkDotPhi/trkDotNormal ) ;
	  }
	  if(m_usePRD && hitPRD != NULL){
	    hitX = hitPRD->localPosition().x();
	    hitY = hitPRD->localPosition().y();
	  }
	  else{
	    hitX = hit->localParameters()[Trk::locX];
	    hitY = hit->localParameters()[Trk::locY];
	  }

	  double unbiasedResXY[4] = {9999.0,9999.0,9999.0,9999.0};
	  double biasedResXY[4] = {9999.0,9999.0,9999.0,9999.0};
	  
	  //finding unbiased single residuals
	  StatusCode sc;
	  sc = getSiResiduals(track,tsos,true,unbiasedResXY);
	  if (sc.isFailure()) {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Problem in determining unbiased residuals! Hit is skipped." << endmsg;
	    //return StatusCode::SUCCESS;
	    continue;
	  }
	  else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "unbiased residuals found ok" << endmsg;
	  residualX = (float)unbiasedResXY[0];
	  residualY = (float)unbiasedResXY[1];
// 	  float pullX = (float)unbiasedResXY[2];
// 	  float pullY = (float)unbiasedResXY[3];

	  
	  //finding biased single residuals (for interest)
	  sc = getSiResiduals(track,tsos,false,biasedResXY);
	  if (sc.isFailure()) {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Problem in determining biased residuals! Hit is skipped." << endmsg;
	    //return StatusCode::SUCCESS;
	    continue;
	  }
	  else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "biased residuals found ok" << endmsg;
	  biasedResidualX = biasedResXY[0];
	  biasedResidualY = biasedResXY[1];
	  	  
	}
	else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackParameters associated with TrkSurface "<< nHits << ", hit type = " << hitType << endmsg; 
      }

      //filling m_ntuple
      m_nt_hittype[nTracks][nHits] = hitType;
      m_nt_dettype[nTracks][nHits] = detType;
      m_nt_isbarrel[nTracks][nHits] = barrelEC;
      m_nt_layer[nTracks][nHits] = layerDisk;
      m_nt_hitmodphi[nTracks][nHits] = modPhi;
      m_nt_hitmodeta[nTracks][nHits] = modEta;
      m_nt_hitx[nTracks][nHits] = hitX;
      m_nt_hity[nTracks][nHits] = hitY;
      m_nt_residualx[nTracks][nHits] = residualX;
      m_nt_residualy[nTracks][nHits] = residualY;
      m_nt_biasedresidualx[nTracks][nHits] = biasedResidualX;
      m_nt_biasedresidualy[nTracks][nHits] = biasedResidualY;
      m_nt_errorx[nTracks][nHits] = errorX;
      m_nt_errory[nTracks][nHits] = errorY;
      m_nt_hitxwidth[nTracks][nHits] = phiWidth;
      m_nt_hitywidth[nTracks][nHits] = zWidth;
      m_nt_hitolegwidth[nTracks][nHits] = olegWidth;
      m_nt_hitincidangle[nTracks][nHits] = trkIncidAngle;

      nHits++; 
      nHitsEvent++;
      
    }//end of loop on track surfaces
    
    //bounds checking
    if (nHits >= s_n_maxHits) {
      msg(MSG::ERROR) << "WATCH OUT: There are more HITS in this events than fit in the ntuples hits matrix!" << endmsg;
      msg(MSG::ERROR) << "Set max hits per track = " <<  s_n_maxHits << ", current track has " << nHits << " or more hits!" << endmsg;
      return StatusCode::FAILURE;
    }
    
    m_nt_trknhits[nTracks] = nHits; 
    
    //filling m_ntuple with some track parameters
    const Trk::Perigee* startPerigee = track->perigeeParameters();
    float theta = startPerigee->parameters()[Trk::theta];  
    float d0 = startPerigee->parameters()[Trk::d0];
    float phi0 = startPerigee->parameters()[Trk::phi0];
    m_nt_trktheta[nTracks] = theta;
    m_nt_trkqoverpt[nTracks] = startPerigee->parameters()[Trk::qOverP]/sin(theta);
    m_nt_trketa[nTracks] = startPerigee->eta();
    m_nt_trkphi[nTracks] = phi0;
    m_nt_trkd0[nTracks] = d0;
    m_nt_trkz0[nTracks] = startPerigee->parameters()[Trk::z0];
    m_nt_trkcharge[nTracks] = startPerigee->charge();
    
    //finding d0 wrt the primary vertex if one is well-defined
    if(!(xv==0.0 && yv==0.0 && zv==0.0)){
      //if we found a decent vertex
      float d0wrtVtx = d0 -(yv*cos(phi0) - xv*sin(phi0));
      m_nt_trkvtxd0[nTracks] = d0wrtVtx;
    }
    else m_nt_trkvtxd0[nTracks] = 999;

    const Trk::FitQuality* fit = track->fitQuality();
    float chiSquared = (fit) ? fit->chiSquared() : -1.;
    int DoF = (fit) ? fit->numberDoF() : -1;
    m_nt_trkchi2[nTracks] = chiSquared;
    m_nt_trkdof[nTracks] = DoF;
            

    //tracktruth stuff (put in separate method)
    if (truthMap) {
	  
      //the key for the truth std::map is an ElementLink<TrackCollection> object
      //comprises a pointer to the track and reconstructed track collection
      ElementLink<TrackCollection> trackLink;
      trackLink.setElement(const_cast<Trk::Track*>(track));
      trackLink.setStorableObject(*tracks);
      const ElementLink<TrackCollection> trackLink2=trackLink;
      
      //trying to find the std::map entry for this reconstructed track
      TrackTruthCollection::const_iterator found = truthMap->find(trackLink2);
      
      if (found != truthMap->end()) {

      	TrackTruth trkTruth = found->second;//getting the TrackTruth object - the map element
      	float trkTruthProb =  trkTruth.probability();//probability of the reco<->truth match
      	HepMcParticleLink HMPL = trkTruth.particleLink();

	if ( HMPL.isValid()) {
	  const HepMC::GenParticle *genParticle = HMPL.cptr(); 
	  
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Particle with PDG "<< genParticle->pdg_id() << " Status "<< genParticle->status()<<" mass "<< genParticle->momentum().m() <<" pt "<<genParticle->momentum().perp()<<" eta "<<genParticle->momentum().eta()<<" phi "<<genParticle->momentum().phi()<<endmsg;

	  m_nt_trkistruth[nTracks] = 1;
	  m_nt_trktruthpdg[nTracks] = genParticle->pdg_id();
	  m_nt_trktruthprob[nTracks] = trkTruthProb;
	  float pX = genParticle->momentum().px();  float pY = genParticle->momentum().py();
	  float genParticlePt = sqrt((pX*pX)+(pY*pY));
	  m_nt_trktruthpt[nTracks] = genParticlePt;
	  m_nt_trktrutheta[nTracks] = genParticle->momentum().eta();
	  m_nt_trktruthphi[nTracks] = genParticle->momentum().phi();


	  if(genParticle->pdg_id()==0) {if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Particle with PDG 0!" << endmsg;}
	  else if(!genParticle->production_vertex()) {if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No GenVertex (generator level) production vertex found!" << endmsg;}
	  else{
	    //currently cannot configure the TruthToTrack tool properly
	    
	    const Trk::TrackParameters* generatedTrackPerigee = m_truthToTrack->makePerigeeParameters(genParticle);

	    if (!generatedTrackPerigee)   if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<  "Unable to extrapolate genParticle to perigee!" << endmsg;
	    
	    if ( generatedTrackPerigee) {
	    
	      float phi0 = generatedTrackPerigee->parameters()[Trk::phi0];
	      float d0 = generatedTrackPerigee->parameters()[Trk::d0];
	      float z0 = generatedTrackPerigee->parameters()[Trk::z0];
	      float theta = generatedTrackPerigee->parameters()[Trk::theta];
	      float eta = generatedTrackPerigee->eta();
	      float charge = generatedTrackPerigee->charge();
	      float qoverpt = generatedTrackPerigee->parameters()[Trk::qOverP]/(sin(theta));
	      float pt = (1/qoverpt)*(charge);
	    
	      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Extrapolated genParticle perigee parameters: q/Pt = " << qoverpt << " d0 = " << d0 << " z0 = " << z0 << " phi0 = " << phi0 << " pt = " << pt << endmsg;

	      m_nt_trktruthphi0[nTracks] = phi0;
	      m_nt_trktruthd0[nTracks] = d0;
	      m_nt_trktruthz0[nTracks] = z0;
	      m_nt_trktrutheta0[nTracks] = eta;
	      m_nt_trktruthqoverpt[nTracks] = qoverpt;
	      m_nt_trktruthpt0[nTracks] = pt;
	      m_nt_trktruthcharge[nTracks] = charge;
	      m_nt_trktruthvtxX[nTracks] = genParticle->production_vertex()->position().x();
	      m_nt_trktruthvtxY[nTracks] = genParticle->production_vertex()->position().y();
	      m_nt_trktruthvtxZ[nTracks] = genParticle->production_vertex()->position().z();
	      
	      delete  generatedTrackPerigee; 
	    }
	  }
	}
      }
    }

    nTracks++;
    

  } // end of loop on tracks

  //bounds checking
  if (nTracks >= s_n_maxTracks) {
    msg(MSG::ERROR) << "WATCH OUT: There are more TRACKS in this events than fit in the m_ntuple tracks matrix!" << endmsg;
    msg(MSG::ERROR) << "Set max tracks per event = " <<  s_n_maxTracks << ", current event has " << nTracks << " or more tracks!" << endmsg;
    return StatusCode::FAILURE;
  }
  else m_nt_ntrks = nTracks;
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of tracks : "<< nTracks << endmsg;
  
  //bounds checking
  if (nHitsEvent >= s_n_maxEventHits) {
    msg(MSG::ERROR) << "WATCH OUT: There are more EVENTHITS in this event than fit in the ntuples eventhits!" << endmsg;
    msg(MSG::ERROR) << "Set max hits per event = " <<  s_n_maxEventHits << ", current event has " << nHitsEvent << " or more hits!" << endmsg;
    return StatusCode::FAILURE;
  }
  else m_nt_nhits = nHitsEvent;
  
  //write the m_ntuple record 
  if (!(m_ntupleSvc->writeRecord(m_ntuple)).isSuccess()) {
    msg(MSG::ERROR) << "problems writing m_ntuple record" << endmsg;
  }


  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------

StatusCode IDAlignMonNtuple::procHistograms()
{

  //if( endOfLowStatFlag() ) {  }
  //if( endOfLumiBlockFlag() ) {  }
  //if( endOfRunFlag() ) {}
  
  return StatusCode::SUCCESS;
}

//__________________________________________________________________________
StatusCode  IDAlignMonNtuple::getSiResiduals(const Trk::Track* track, const Trk::TrackStateOnSurface* tsos, bool unBias, double* results)
{
  if (!m_doPulls) return StatusCode::FAILURE;

  StatusCode sc = StatusCode::SUCCESS;

  double residualX = -9999.0;
  double residualY = -9999.0;
  double pullX = -9999.0;
  double pullY = -9999.0;
  
  //extract the hit object from the tsos
  const Trk::MeasurementBase* mesh =tsos->measurementOnTrack();
  const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(mesh);
  
  //get the unbiased track parameters (can fail if no MeasuredTrackParameters exists)
  const Trk::TrackParameters* trackParameterUnbiased = NULL;
  if(unBias) trackParameterUnbiased = getUnbiasedTrackParameters(track,tsos);
   
  //updator can fail in defining unbiased parameters, in which case we use biased
  const Trk::TrackParameters* trackParameterForResiduals = NULL;
  if(trackParameterUnbiased) trackParameterForResiduals = trackParameterUnbiased;
  else {
    //use the original biased track parameters
    trackParameterForResiduals = tsos->trackParameters();
  }

  if (!m_residualPullCalculator.empty() && !m_residualPullCalculator.retrieve().isFailure()) {
    
    if (hit && trackParameterForResiduals) {
      
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" got hit and track parameters " << endmsg;

      //const Trk::ResidualPull* residualPull = m_residualPullCalculator->residualPull(hit, trackParameterForResiduals, unBias);
      const Trk::ResidualPull* residualPull = NULL;
      if(unBias) residualPull = m_residualPullCalculator->residualPull(mesh, trackParameterForResiduals, Trk::ResidualPull::Unbiased);
      else residualPull = m_residualPullCalculator->residualPull(mesh, trackParameterForResiduals, Trk::ResidualPull::Biased);

      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" got hit and track parameters...done " << endmsg;
      if (residualPull) {

	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " got residual pull " << endmsg;
	residualX = residualPull->residual()[Trk::loc1];
	if(residualPull->isPullValid()) pullX = residualPull->pull()[Trk::loc1];
	else {
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ResidualPullCalculator finds invalid X Pull!!!" << endmsg;
	  sc = StatusCode::FAILURE;
	}

	if (residualPull->dimension() >= 2){

	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " residualPull dim >= 2" << endmsg;
	  residualY = residualPull->residual()[Trk::loc2];

	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " residual Y = " << residualY << endmsg; 
	  if(residualPull->isPullValid()) pullY = residualPull->pull()[Trk::loc2];
	  else {
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ResidualPullCalculator finds invalid Y Pull!!!" << endmsg;
	    sc = StatusCode::FAILURE;
	  }
	}
	
	delete residualPull;

      }
      else {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ResidualPullCalculator failed!" << endmsg;
	sc = StatusCode::FAILURE;
      }
    }
  }

  // for SCT modules the residual pull calculator only finds the (rotated) Rphi residual
  // for each of the SCT sides; residualPull->dimension()==1 always.

  //std::pair <double, double> result(residualX, residualY);
  results[0] = residualX; 
  results[1] = residualY;
  results[2] = pullX;
  results[3] = pullY;

  if(pullX!=pullX || pullY!=pullY){
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ResidualPullCalculator finds Pull=NAN!!!" << endmsg;
    sc = StatusCode::FAILURE;
  }
  
  //delete these TrackParameters which are newly created in the getUnbiasedTrackParameters(track,tsos) method
  delete trackParameterUnbiased;

  return sc;
  
}


//__________________________________________________________________________
const Trk::TrackParameters* IDAlignMonNtuple::getUnbiasedTrackParameters(const Trk::Track* trkPnt, const Trk::TrackStateOnSurface* tsos)
{
 
  const Trk::TrackParameters* TrackParams;
  const Trk::TrackParameters* UnbiasedTrackParams(0);
  const Trk::TrackParameters* PropagatedTrackParams(0);
  const Trk::TrackParameters* OtherSideUnbiasedTrackParams(0);

  //controls if the SCT residuals will be 'truly' unbiased - removing also the opposite side hit.
  bool trueUnbiased = true;

  Identifier surfaceID;
  

  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "original track parameters: " << *(tsos->trackParameters()) <<endmsg;

  
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Trying to unbias track parameters." << endmsg;

  const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(tsos->measurementOnTrack());
  
  if (hitOnTrack == NULL)
    return NULL;


  surfaceID = hitOnTrack->identify();
  // if SCT Hit and TrueUnbiased then remove other side hit first
  if (trueUnbiased && m_idHelper->is_sct(surfaceID)) {  //there's no TrueUnbiased for non-SCT (pixel) hits)
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Entering True Unbiased loop." << endmsg;
    // check if other module side was also hit and try to remove other hit as well
    const Trk::TrackStateOnSurface* OtherModuleSideHit(0);
    const Identifier& OtherModuleSideID = m_SCT_Mgr->getDetectorElement(surfaceID)->otherSide()->identify();
      
    for (const Trk::TrackStateOnSurface* TempTsos : *trkPnt->trackStateOnSurfaces()) {
        
      const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(TempTsos->measurementOnTrack());
      if (hitOnTrack != 0) {
	const Identifier& trkID = hitOnTrack->identify();
	if (m_sctID->wafer_id(trkID) == OtherModuleSideID) {
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "True unbiased residual. Removing OtherModuleSide Hit " << m_idHelper->show_to_string(OtherModuleSideID,0,'/') << endmsg;
	  OtherModuleSideHit = TempTsos;
	}
      }
    }

    if (OtherModuleSideHit) {

      
      const Trk::TrackParameters* OMSHmeasuredTrackParameter = OtherModuleSideHit->trackParameters();
      
      // check that the hit on the other module side has measuredtrackparameters, otherwise it cannot be removed from the track
      const AmgSymMatrix(5)* covariance = OMSHmeasuredTrackParameter->covariance();
      if (covariance) {
      

	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "OtherSideTrackParameters: " << *(OtherModuleSideHit->trackParameters()) << endmsg;
	OtherSideUnbiasedTrackParams = m_iUpdator->removeFromState(*(OtherModuleSideHit->trackParameters()),
								   OtherModuleSideHit->measurementOnTrack()->localParameters(),
								   OtherModuleSideHit->measurementOnTrack()->localCovariance());

	if (OtherSideUnbiasedTrackParams) {
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Unbiased OtherSideTrackParameters: " << *OtherSideUnbiasedTrackParams << endmsg;


	  const Trk::Surface& TempSurface = OtherModuleSideHit->measurementOnTrack()->associatedSurface();

	  const Trk::MagneticFieldProperties* TempField = 0;
	  
          
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "After OtherSide surface call. Surface exists" << endmsg;
	  if (TempSurface.associatedLayer())
	    {
	      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TempSurface->associatedLayer() exists" << endmsg;
	      if(TempSurface.associatedLayer()->enclosingTrackingVolume())
		{
		  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TempSurface->associatedLayer()->enclosingTrackingVolume exists" << endmsg;
		  
		  TempField = dynamic_cast <const Trk::MagneticFieldProperties*>(TempSurface.associatedLayer()->enclosingTrackingVolume());
		  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "After MagneticFieldProperties cast" << endmsg;
		  
		} else {
		if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TempSurface->associatedLayer()->enclosingTrackingVolume does not exist" << endmsg;
	      }
	    } else {
	    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TempSurface->associatedLayer() does not exist" << endmsg;
	  }
	  
	  
	  
	  
	  
	  
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Before other side unbiased propagation" << endmsg;
	  if (TempSurface.associatedLayer() && TempField) PropagatedTrackParams = m_propagator->propagate(*OtherSideUnbiasedTrackParams,
													  tsos->measurementOnTrack()->associatedSurface(),
													  Trk::anyDirection, false,
													  *TempField,
													  Trk::nonInteracting);
	  
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "After other side unbiased propagation" << endmsg;
	  delete OtherSideUnbiasedTrackParams;
	  if (PropagatedTrackParams) {
	    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Propagated Track Parameters: " << *PropagatedTrackParams << endmsg;
	  } else {
	    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Propagation of unbiased OtherSideParameters failed" << endmsg;
	  }
	} else {
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "RemoveFromState did not work for OtherSideParameters" << endmsg;
	}
      } else {
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "No OtherModuleSideHit Measured Track Parameters found. Covariance Null" << endmsg;
      }
    } else {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "No OtherModuleSideHit found" << endmsg;
    }
  }

  // if propagation failed or no TrueUnbiased or no SCT then use original TrackParams
  if (!PropagatedTrackParams) {
    PropagatedTrackParams = tsos->trackParameters()->clone();
  }

    
  UnbiasedTrackParams = m_iUpdator->removeFromState(*PropagatedTrackParams, tsos->measurementOnTrack()->localParameters(), tsos->measurementOnTrack()->localCovariance());

  delete PropagatedTrackParams;

  if (UnbiasedTrackParams) {
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Unbiased residual. Removing original Hit " << m_idHelper->show_to_string(surfaceID,0,'/') << endmsg;
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Unbiased Trackparameters: " << *UnbiasedTrackParams << endmsg;

    TrackParams = UnbiasedTrackParams->clone();

  } else { // Unbiasing went awry.
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "RemoveFromState did not work, using original TrackParameters" << endmsg;
    TrackParams = tsos->trackParameters()->clone();
  }
  

  delete UnbiasedTrackParams;  
  return TrackParams;
  
}
 
 
//---------------------------------------------------------------------------------------

StatusCode IDAlignMonNtuple::setupTools()
{
  //initializing tools

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In initialize()" << endmsg;

  StatusCode sc;

  sc = service( "NTupleSvc", m_ntupleSvc );
  if ( sc.isFailure() ){
    msg(MSG::FATAL) << "Cannot retrieve the NTuple service... Exiting" << endmsg;
    return StatusCode::FAILURE;
  }  

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Defined detector service" << endmsg;

  sc = detStore()->retrieve(m_pixelID, "PixelID");
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get Pixel ID helper !" << endmsg;
    return StatusCode::FAILURE;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized PixelIDHelper" << endmsg;

  sc = detStore()->retrieve(m_sctID, "SCT_ID");
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get SCT ID helper !" << endmsg;
    return StatusCode::FAILURE;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized SCTIDHelper" << endmsg;

  // Get the SCT manager from the detector store
  sc = detStore()->retrieve(m_SCT_Mgr, "SCT");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get SCT_Manager !" << endmsg;
    return sc;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized SCTManager" << endmsg;

  //ID Helper
  sc = detStore()->retrieve(m_idHelper, "AtlasID" );
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get AtlasDetectorID !" << endmsg;
    return StatusCode::SUCCESS;
  }else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found AtlasDetectorID" << endmsg;
  }
  

  if (m_iUpdator.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_iUpdator << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_iUpdator << endmsg;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved IUpdator" << endmsg;


  if (m_propagator.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Can not retrieve Propagator tool of type "
	<< m_propagator.typeAndName() << endmsg;
    return StatusCode::FAILURE;
  } else msg(MSG::INFO) << "Retrieved tool " << m_propagator.typeAndName() << endmsg;

  if (m_truthToTrack.retrieve().isFailure() ) {
     msg(MSG::FATAL) << "Failed to retrieve tool " << m_truthToTrack << endmsg;
     return StatusCode::FAILURE;
   } else {
     msg(MSG::INFO) << "Retrieved tool " << m_truthToTrack << endmsg;
   }

  if (m_residualPullCalculator.empty()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << 
      "No residual/pull calculator for general hit residuals configured."
	<< endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << 
      "It is recommended to give R/P calculators to the det-specific tool"
	<< " handle lists then." << endmsg;
    m_doPulls = false;
  } else if (m_residualPullCalculator.retrieve().isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve "<< m_residualPullCalculator 
	<<" (to calculate residuals and pulls) "<< endmsg;
    m_doPulls = false;
    
   } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) 
	<< "Generic hit residuals&pulls will be calculated in one or both "
	<< "available local coordinates" << endmsg;
    m_doPulls = true;
  }


  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------

void IDAlignMonNtuple::setTrackErrorValues(int nTracks)
{

  //for each track ensures that the variable is set to 
  //a defined errorvalue in case it can't be defined

  m_nt_trknhits[nTracks] = s_n_ERRORVALUE;
  
  m_nt_trktheta[nTracks] = s_n_ERRORVALUE;
  m_nt_trkqoverpt[nTracks] = s_n_ERRORVALUE;
  m_nt_trketa[nTracks] = s_n_ERRORVALUE;
  m_nt_trkphi[nTracks] = s_n_ERRORVALUE;
  m_nt_trkd0[nTracks] = s_n_ERRORVALUE;
  m_nt_trkz0[nTracks] = s_n_ERRORVALUE;
  m_nt_trkcharge[nTracks] = s_n_ERRORVALUE;
  m_nt_trkvtxd0[nTracks] = s_n_ERRORVALUE;
  m_nt_trkchi2[nTracks] = s_n_ERRORVALUE;
  m_nt_trkdof[nTracks] = s_n_ERRORVALUE;

  m_nt_trkistruth[nTracks] = 0;//will be set to one if find a truth
  m_nt_trktruthpdg[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthprob[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthpt[nTracks] = s_n_ERRORVALUE;
  m_nt_trktrutheta[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthphi[nTracks] = s_n_ERRORVALUE;
  
  m_nt_trktruthphi0[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthd0[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthz0[nTracks] = s_n_ERRORVALUE;
  m_nt_trktrutheta0[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthqoverpt[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthpt0[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthcharge[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthvtxX[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthvtxY[nTracks] = s_n_ERRORVALUE;
  m_nt_trktruthvtxZ[nTracks] = s_n_ERRORVALUE;

}
