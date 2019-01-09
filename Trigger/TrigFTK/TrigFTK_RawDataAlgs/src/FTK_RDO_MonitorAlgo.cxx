/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FTK_RDO_MonitorAlgo.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"

#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "StoreGate/ReadCondHandle.h"

#include "TrigFTKSim/ftk_dcap.h"

#include "TrkSurfaces/Surface.h"

#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "TMath.h"

using namespace std;


/////////////////////////////////////////////////////////////////////////////
FTK_RDO_MonitorAlgo::FTK_RDO_MonitorAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_DataProviderSvc("IFTK_DataProviderSvc/IFTK_DataProviderSvc", name),
  m_pixelId(0),
  m_sctId(0),
  m_pixelManager(0),
  m_id_helper(0),
  m_minPt(1000.),
  m_maxa0(1.),
  m_maxz0(100.),
  m_mineta(-2.5),
  m_maxeta(2.5),
  m_minphi(-10.),
  m_maxphi(10.),
  m_minMatches(6),
  m_reverseIBLlocx(false),
  m_Nlayers(12),
  m_getHashFromTrack(true),
  m_getHashFromConstants(false),
  m_getRawTracks(true),
  m_getTracks(true),
  m_getRefitTracks(true),
  m_HashIDTool("IFTK_HashIDTool/TrigFTK_HashIDTool")

{
  declareProperty("HashIDTool",m_HashIDTool);
  declareProperty("RDO_CollectionName",m_ftk_raw_trackcollection_Name, "Collection name of RDO");
  declareProperty("offlineTracksName",m_offlineTracksName," offline tracks collection name");
  declareProperty("FTK_DataProvider", m_DataProviderSvc);
  declareProperty("minpt", m_minPt,"Minimum pT for offline track");
  declareProperty("maxa0", m_maxa0,"Maximum a0 for offline track");
  declareProperty("maxz0", m_maxz0,"Maximum z0 for offline track");
  declareProperty("mineta", m_mineta,"Minimum eta for offline track");
  declareProperty("maxeta", m_maxeta,"Maximum eta for offline track");
  declareProperty("minphi", m_minphi,"Minimum phi for offline track");
  declareProperty("maxphi", m_maxphi,"Maximum phi for offline track");
  declareProperty("minMatches", m_minMatches,"Minimum number of matched hits");
  declareProperty("reverseIBLlocx",m_reverseIBLlocx,"reverse direction of IBL locx from FTK");
  declareProperty("Nlayers",m_Nlayers,"No. FTK layers in the SectorsFile");
  declareProperty("GetHashFromTrack",m_getHashFromTrack," Get HashID From Track");
  declareProperty("GetHashFromConstants",m_getHashFromConstants," Get HashID From Constants");
  declareProperty("getRawTracks",m_getRawTracks,"get Raw FTK tracks");
  declareProperty("getTracks",m_getTracks,"get FTK tracks");
  declareProperty("getRefitTracks",m_getRefitTracks,"get Refit FTK tracks");


}

FTK_RDO_MonitorAlgo::~FTK_RDO_MonitorAlgo()
{ }


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode FTK_RDO_MonitorAlgo::initialize(){


  MsgStream athlog(msgSvc(), name());
  ATH_MSG_INFO("FTK_RDO_MonitorAlgo::initialize()" );


  //Set up the FTK Data Provider SVC //
  ATH_CHECK(m_DataProviderSvc.retrieve());
  /// Get Histogram Service ///
  ATH_CHECK(service("THistSvc", m_rootHistSvc));

  if (m_getHashFromConstants) {
    ATH_CHECK(m_HashIDTool.retrieve());
  }

  ///Tree
  std::string trackRootPath;

  std::vector<TH1D*> *histograms = new std::vector<TH1D*> ;

  //Book Histograms//
  Hist_Init(histograms);

  trackRootPath = "/TRACKS/";
  for( uint i = 0; i < histograms->size(); ++i){
    if ( m_rootHistSvc->regHist( trackRootPath + (*histograms)[i]->GetName(), (*histograms)[i] ).isFailure()) {
      std::string temp = "Failed to book Histogram: " + std::string((*histograms)[i]->GetName());
      ATH_MSG_WARNING(temp);
    } else {
      std::string temp = "Booked Histogram: " + std::string((*histograms)[i]->GetName());
      ATH_MSG_DEBUG(temp);
    }  }
  delete(histograms);

  m_h_missingHits = new TProfile("h_missingHits", " layer ",12,0.,12.,0.,1.);
  if (m_rootHistSvc->regHist( trackRootPath + m_h_missingHits->GetName(), m_h_missingHits).isFailure()) {
    std::string temp = "Failed to book Histogram: " + std::string(m_h_missingHits->GetName());
    ATH_MSG_WARNING(temp);
  }


  ATH_CHECK(detStore()->retrieve(m_pixelId, "PixelID"));
  ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_pixelManager));
  ATH_CHECK(detStore()->retrieve(m_id_helper, "AtlasID"));

  ATH_CHECK(m_pixelLorentzAngleTool.retrieve());
  ATH_CHECK(m_sctLorentzAngleTool.retrieve());

  // ReadCondHandleKey
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  ATH_MSG_INFO("RDO_CollectionName " << m_ftk_raw_trackcollection_Name);
  ATH_MSG_INFO("offlineTracksName "<<m_offlineTracksName);
  ATH_MSG_INFO("FTK_DataProvider "<< m_DataProviderSvc);
  ATH_MSG_INFO("minpt "<< m_minPt);
  ATH_MSG_INFO("maxa0 " << m_maxa0);
  ATH_MSG_INFO("maxz0 " << m_maxz0);
  ATH_MSG_INFO("mineta " << m_mineta);
  ATH_MSG_INFO("maxeta " <<  m_maxeta);
  ATH_MSG_INFO("minphi " << m_minphi);
  ATH_MSG_INFO("maxphi " << m_maxphi);
  ATH_MSG_INFO("minMatches " <<  m_minMatches);
  if ( m_reverseIBLlocx ) 
    ATH_MSG_INFO("reversing direction of IBL locx");

  ATH_MSG_INFO("Nlayers= "<<m_Nlayers);

  if (m_getHashFromTrack && m_getHashFromConstants) {
    ATH_MSG_INFO("getting HashID from Track and Constants and comparing the two");
  } else if (m_getHashFromTrack) {
    ATH_MSG_INFO("getting HashID from Track");
  } else if (m_getHashFromConstants) {
    ATH_MSG_INFO("getting HashID from Constants");
  } else {
    ATH_MSG_INFO("getting HashID from Track");
    m_getHashFromTrack=true;
  }
    
  return StatusCode::SUCCESS; 
}


StatusCode FTK_RDO_MonitorAlgo::execute() {
  MsgStream athlog(msgSvc(), name());
  ATH_MSG_DEBUG("FTK_RDO_MonitorAlgo::execute() start");
  
  
  const EventInfo* eventInfo;
  StatusCode scEv = evtStore()->retrieve(eventInfo);
  if (scEv.isFailure()) {
    ATH_MSG_DEBUG( "Could not retrieve event info" );
  } else {
    int eventNumber = eventInfo->event_ID()->event_number();
    ATH_MSG_DEBUG(" Event " << eventNumber);
    
    const EventID* eventID( eventInfo->event_ID());
    ATH_MSG_DEBUG( "entered execution for run" << eventID->run_number()
		   << "event" << eventID->event_number());
  }

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* sctElements(*sctDetEleHandle);
  if (not sctDetEleHandle.isValid() or sctElements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " is not available.");
    return StatusCode::FAILURE;
  }
  
  // get FTK tracks
  const FTK_RawTrackContainer *rawTracks = m_DataProviderSvc->getRawTracks();
  ATH_MSG_DEBUG("rawTracks at " << std::hex << rawTracks << std::dec);

  if (rawTracks == nullptr) {
    ATH_MSG_DEBUG("No FTK tracks");
    return StatusCode::SUCCESS;
  }
  
  
  ATH_MSG_DEBUG(" Got FTK_RawTrackContainer with " << rawTracks->size() << " tracks");
  m_h_FTK_RawTrack_n->Fill(rawTracks->size());

  if (rawTracks->size()==0) {
    ATH_MSG_DEBUG("No FTK tracks");
    return StatusCode::SUCCESS;
  }
  
  // Trigger conversion of FTK tracks - needed later when we get the cached tracks
  if (m_getTracks) {
    TrackCollection* ftkTracks=m_DataProviderSvc->getTracks(false);
    ATH_MSG_DEBUG("Got " << ftkTracks->size() << " Converted Tracks");
    delete(ftkTracks);
  }
  if (m_getRefitTracks) {
    TrackCollection* ftkRefitTracks=m_DataProviderSvc->getTracks(true);
    ATH_MSG_DEBUG("Got " << ftkRefitTracks->size() << " Refitted Tracks");
    delete(ftkRefitTracks);
  }

  unsigned int pixMaxHash = m_pixelId->wafer_hash_max();
  unsigned int sctMaxHash = m_sctId->wafer_hash_max();
  

  std::vector<std::vector<unsigned int>*> pixList(pixMaxHash);
  std::vector<std::vector<unsigned int>*> sctList(sctMaxHash);

  (this)->fillMaps(rawTracks,pixList,sctList);

  const TrackCollection *offlineTracks = nullptr;
  if(evtStore()->retrieve(offlineTracks,m_offlineTracksName).isFailure()){
    ATH_MSG_DEBUG("Failed to retrieve Offline Tracks");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_VERBOSE( " Got offline collection with " << offlineTracks->size()<< " offline tracks" );
  
  m_h_offline_n->Fill(offlineTracks->size());

  auto track_it   = offlineTracks->begin();
  auto last_track = offlineTracks->end();
  
  int nAcc=0;
  int nMatched=0;

  for (int iTrack=0 ; track_it!= last_track; track_it++, iTrack++) {
  

    const Trk::TrackParameters* trackPars = (*track_it)->perigeeParameters();
    if(trackPars==nullptr) {
      continue;
    }


    float theta = trackPars->parameters()[Trk::theta]; 
    float qOverP = trackPars->parameters()[Trk::qOverP]; 
    if (fabs(qOverP)<1e-12) {
      //      ATH_MSG_DEBUG("q/p == 0, setting to 1e-12");
      qOverP = (qOverP>0?1e-12:-1e-12);
    }
    float pT=sin(theta)/qOverP;
    float a0 = trackPars->parameters()[Trk::d0]; 
    float z0 = trackPars->parameters()[Trk::z0]; 
    float phi0 = trackPars->parameters()[Trk::phi0]; 
    float eta = -log(tan(0.5*theta)); 

    // TODO: handle phi-wrap-arround for phi selection
    


    nAcc++;
    ATH_MSG_VERBOSE(" Accepted Offline Track " << iTrack << " pT " << pT << " eta " << eta << 
		    " phi0 " <<  phi0 << " d0 " << a0 << " z0 " << z0);
    
    
    
    if (eta> m_mineta && eta<m_maxeta && phi0> m_minphi && phi0<m_maxphi) {
      
      if (fabs(a0) < m_maxa0 && fabs(z0) < m_maxz0) m_h_offline_pt->Fill(fabs(pT));
      
      if (fabs(pT)>m_minPt && fabs(z0) < m_maxz0) m_h_offline_d0->Fill(a0);
      
      if (fabs(pT)>m_minPt && fabs(a0)< m_maxa0) m_h_offline_z0->Fill(z0);
      
    }
    if (fabs(pT)>m_minPt && fabs(a0) < m_maxa0 && fabs(z0) < m_maxz0) {
      if (phi0 > m_minphi && phi0 < m_maxphi) m_h_offline_eta->Fill(eta);
      if (eta> m_mineta && eta < m_maxeta) m_h_offline_phi->Fill(phi0);
    }    
    
    
    std::map<unsigned int,std::pair<double,double> > offlinetrackPixLocxLocy;
    std::map<unsigned int,double> offlinetrackSctLocx;
    bool uniqueMatch=false;
    
    const std::pair<unsigned int,unsigned int> ftkTrackMatch = (this)->matchTrack(*track_it,pixList, sctList, offlinetrackPixLocxLocy, offlinetrackSctLocx, uniqueMatch);
    
    m_h_FTK_nHitMatch->Fill((double)ftkTrackMatch.second);  
    if (ftkTrackMatch.second > 0) {
      ATH_MSG_VERBOSE(" matched to FTK track index " << ftkTrackMatch.first << " with " << ftkTrackMatch.second << " matches");
      if (ftkTrackMatch.second > m_minMatches) { 
	nMatched+=1;
	const FTK_RawTrack* ftkRawTrack = rawTracks->at(ftkTrackMatch.first);
	float ftkTrkTheta = std::atan2(1.0,(ftkRawTrack)->getCotTh());
	float ftkTrkEta = -std::log(std::tan(ftkTrkTheta/2.));
	float ftkTrkPt=1.e10;
	if (fabs((ftkRawTrack)->getInvPt()) >= 1e-10) ftkTrkPt=1./(ftkRawTrack)->getInvPt();
	ATH_MSG_VERBOSE(" FTK     Track " << ftkTrackMatch.first << " pT " << ftkTrkPt << " eta " << ftkTrkEta << 
		    " phi0 " << (ftkRawTrack)->getPhi() << " d0 " << (ftkRawTrack)->getD0() << " z0 " << (ftkRawTrack)->getZ0());
    
	//	ATH_MSG_VERBOSE( " nPix: " << (ftkRawTrack)->getPixelClusters().size() << " nSCT: "<< (ftkRawTrack)->getSCTClusters().size()<< " barCode: "<<(ftkRawTrack)->getBarcode()  );
	//    ATH_MSG_VERBOSE( "     SectorID " << (ftkRawTrack)->getSectorID()   <<   "  RoadID "  << (ftkRawTrack)->getRoadID() << " LayerMap " << (ftkRawTrack)->getLayerMap());
	
	if (eta> m_mineta && eta<m_maxeta && phi0> m_minphi && phi0<m_maxphi) {
	  
	  if (fabs(a0) < m_maxa0 && fabs(z0) < m_maxz0) m_h_FTK_ptEffNum->Fill(fabs(pT));
	  
	  if (fabs(pT)>m_minPt && fabs(z0) < m_maxz0) m_h_FTK_d0EffNum->Fill(a0);
	  
	  if (fabs(pT)>m_minPt && fabs(a0)< m_maxa0) m_h_FTK_z0EffNum->Fill(z0);
	  
	}

	if (fabs(pT)>m_minPt && fabs(a0) < m_maxa0 && fabs(z0) < m_maxz0) {
	  if (phi0 > m_minphi && phi0 < m_maxphi) m_h_FTK_etaEffNum->Fill(eta);
	  if (eta> m_mineta && eta < m_maxeta) m_h_FTK_phiEffNum->Fill(phi0);
	  
	  if (eta> m_mineta && eta < m_maxeta && phi0> m_minphi && phi0 < m_maxphi) {
	    if (uniqueMatch) { 
	      if (m_getRawTracks)(this)->compareTracks(ftkRawTrack, sctElements, offlinetrackPixLocxLocy, offlinetrackSctLocx);
	      if (m_getTracks) {
		const Trk::Track* ftktrack = m_DataProviderSvc->getCachedTrack(ftkTrackMatch.first,false);
		if (ftktrack) (this)->compareTracks(ftktrack, offlinetrackPixLocxLocy, offlinetrackSctLocx, false);
	      }
	      if (m_getRefitTracks) {
		const Trk::Track* ftkRefitTrack = m_DataProviderSvc->getCachedTrack(ftkTrackMatch.first,true);
		if (ftkRefitTrack) (this)->compareTracks(ftkRefitTrack, offlinetrackPixLocxLocy, offlinetrackSctLocx, true);
	      }
	      
	    }
	  }
	}
      } else {
	ATH_MSG_VERBOSE(" not enough matched FTK hits");
      }
    } else {
      ATH_MSG_VERBOSE(" no matched FTK track");
    }

  }
  ATH_MSG_DEBUG(" Number of offline tracks passing cuts "<<nAcc<<" number matched to FTK track "<<nMatched);
  
  m_h_offline_nAcc->Fill(nAcc);
  

  for ( auto& ip:pixList) if (ip) delete(ip);
  for ( auto& is:sctList) if (is) delete(is);

  
  ATH_MSG_DEBUG( "FTK_RDO_MonitorAlgo::execute() end" );
  
  return StatusCode::SUCCESS;
  
} 
 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_MonitorAlgo::finalize() {
  MsgStream athlog(msgSvc(), name());
  ATH_MSG_INFO("finalize()" );
  
  return StatusCode::SUCCESS;

}


void FTK_RDO_MonitorAlgo::Hist_Init(std::vector<TH1D*> *histograms){

  // FTK Raw Track Histograms //

  m_h_FTK_nHitMatch    = new TH1D("h_FTKMoni_nHitMatch",    "No. Hits on track matching Offline; No. Matched Hits; NTracks",      12,  0.,    12.);
  histograms->push_back(m_h_FTK_nHitMatch);

  m_h_FTK_nTrackMatch    = new TH1D("h_FTKMoni_nTrackMatch",    "No. FTK tracks matching offline;No. Matched FTK Tracks;",      12,  0.,    12.);
  histograms->push_back(m_h_FTK_nTrackMatch);

  m_h_FTK_RawTrack_n    = new TH1D("h_FTKMoni_RawTrack_n",    ";FTK Raw Track multiplicity;No. Tracks",      25,  0.,    25.);
  histograms->push_back(m_h_FTK_RawTrack_n);

  m_h_offline_n    = new TH1D("h_FTKMoni_offline_n",    ";Offline Track multiplicity;No. Tracks",      50,  0.,    200.);
  histograms->push_back(m_h_offline_n);

  m_h_offline_nAcc   = new TH1D("h_FTKMoni_offline_nAcc",    ";No. Offline Tracks accepted;No. Tracks",      50,  0.,    50.);
  histograms->push_back(m_h_offline_nAcc);


  m_h_FTK_pt    = new TH1D("h_FTKMoni_RawTrack_pt",    ";FTK track pt[MeV];No. Tracks",      100, 0., 100000.);
  histograms->push_back(m_h_FTK_pt);
  m_h_offline_pt    = new TH1D("h_FTKMoni_offline_pt",    ";offline track pt[MeV];No. Tracks",    100, 0., 100000.);
  histograms->push_back(m_h_offline_pt);
  m_h_FTK_ptEffNum    = new TH1D("h_FTKMoni_ptEffNum",    ";offline track pt[MeV];No. FTK Tracks",    100, 0., 100000.);
  histograms->push_back(m_h_FTK_ptEffNum);


  m_h_FTK_eta    = new TH1D("h_FTKMoni_RawTrack_eta",    ";FTK track eta;No. Tracks",      104,  -2.6, 2.6);
  histograms->push_back(m_h_FTK_eta);
  m_h_offline_eta    = new TH1D("h_FTKMoni_offline_eta",    ";offline track eta;No. Tracks",      104,  -2.6,    2.6);
  histograms->push_back(m_h_offline_eta);
  m_h_FTK_etaEffNum    = new TH1D("h_FTKMoni_etaEffNum",    ";offline track eta;No. FTK Tracks",      104,  -2.6, 2.6);
  histograms->push_back(m_h_FTK_etaEffNum);

  m_h_FTK_phi    = new TH1D("h_FTKMoni_RawTrack_phi",    ";FTK track phi[Rad];No. Tracks",      64,  -3.2,    3.2);
  histograms->push_back(m_h_FTK_phi);
  m_h_offline_phi    = new TH1D("h_FTKMoni_offline_phi",    ";offline track phi[Rad]; No. Tracks",      64,  -3.2,    3.2);
  histograms->push_back(m_h_offline_phi);
  m_h_FTK_phiEffNum    = new TH1D("h_FTKMoni_phiEffNum",    ";offline track phi[Rad]; No. FTK Tracks",      64,  -3.2,    3.2);
  histograms->push_back(m_h_FTK_phiEffNum);

  m_h_FTK_d0    = new TH1D("h_FTKMoni_RawTrack_d0",    ";FTK track d0[Rad];No. Tracks",      100, -5. , 5.);
  histograms->push_back(m_h_FTK_d0);
  m_h_offline_d0    = new TH1D("h_FTKMoni_offline_d0",    ";offline track d0[Rad]; No. Tracks",      100, -5. , 5.);
  histograms->push_back(m_h_offline_d0);
  m_h_FTK_d0EffNum    = new TH1D("h_FTKMoni_d0EffNum",    ";offline track d0[Rad]; No. FTK Tracks",      100, -5. , 5.);
  histograms->push_back(m_h_FTK_d0EffNum);

  m_h_FTK_z0    = new TH1D("h_FTKMoni_RawTrack_z0",    ";FTK track z0[Rad];No. Tracks",      60, -150., 150.);
  histograms->push_back(m_h_FTK_z0);
  m_h_offline_z0    = new TH1D("h_FTKMoni_offline_z0",    ";offline track z0[Rad]; No. Tracks",      60, -150., 150.);
  histograms->push_back(m_h_offline_z0);
  m_h_FTK_z0EffNum    = new TH1D("h_FTKMoni_z0EffNum",    ";offline track z0[Rad]; No. FTK Tracks",      60, -150., 150.);
  histograms->push_back(m_h_FTK_z0EffNum);


  m_h_IBL_dlocX_fullRange= new TH1D("h_IBL_dlocX_fullRange", "PIXB0: locx(offline)-locx(ftk)", 100,  -20.,    20.);
  histograms->push_back(m_h_IBL_dlocX_fullRange);
  
  m_h_IBL_dlocY_fullRange= new TH1D("h_IBL_dlocY_fullRange", "PIXB0: locy(offline)-locy(ftk)", 100,  -50.,    50.);
  histograms->push_back(m_h_IBL_dlocY_fullRange);
  
  m_h_IBL_locX= new TH1D("h_IBL_locX", "IBL: rawLocalPhiCoord%8)", 6,  -0.5,    5.5);
  histograms->push_back(m_h_IBL_locX);        
  m_h_IBL_locY= new TH1D("h_IBL_locY", "IBL: rawLocalEtaCoord%8)", 6,  -0.5,    5.5);
  histograms->push_back(m_h_IBL_locY);

  m_h_pixb_locX= new TH1D("h_pixb_locX", "PIXB: rawLocalPhiCoord%8)", 6,  -0.5,    5.5);
  histograms->push_back(m_h_pixb_locX);
  m_h_pixb_locY= new TH1D("h_pixb_locY", "PIXB: rawLocalEtaCoord%8)", 6,  -0.5,    5.5);
  histograms->push_back(m_h_pixb_locY);
  m_h_pixe_locX= new TH1D("h_pixe_locX", "PIXE: rawLocalPhiCoord%8)", 6,  -0.5,    5.5);
  histograms->push_back(m_h_pixe_locX);
  m_h_pixe_locY= new TH1D("h_pixe_locY", "PIXE: rawLocalEtaCoord%8)", 6,  -0.5,    5.5);
  histograms->push_back(m_h_pixe_locY);

  m_h_sctb_locX= new TH1D("h_sctb_locX", "SCTB: rawLocalPhiCoord%2)", 2,  -0.5,    1.5);
  histograms->push_back(m_h_sctb_locX);
  m_h_scte_locX= new TH1D("h_scte_locX", "SCTE: rawLocalPhiCoord%2)", 2,  -0.5,    1.5);
  histograms->push_back(m_h_scte_locX);
    
  TString histname,histtitle;
  
  Int_t ilay=0;
  if (m_Nlayers==8) ilay=1;
  
  for ( ; ilay<4; ilay++){
    
    
    // row coord 
    histname="h_FTKMoni_PIX";
    histname+=ilay;
    histname+="_row";
    
    histtitle="PIX";
    histtitle+=ilay;
    histtitle+=": row (phi) coord";
    
    TH1D* pix_row = new TH1D(histname, histtitle, 41,  0, 4100);
    m_h_pix_row.push_back(pix_row);
    histograms->push_back(pix_row);
    
    // row width 
    
    histname="h_FTKMoni_PIX";
    histname+=ilay;
    histname+="_rowW";
    
    histtitle="PIX";
    histtitle+=ilay;
    histtitle+=": row (phi) width";
    
    TH1D* pix_rowW = new TH1D(histname, histtitle, 8,  -0.5, 7.5);
    m_h_pix_rowW.push_back(pix_rowW);
    histograms->push_back(pix_rowW);
    
    // col coord
    
    histname="h_FTKMoni_PIX";
    histname+=ilay;
    histname+="_col";
    
    histtitle="PIX";
    histtitle+=ilay;
    histtitle+=": col (eta) coord";
    
    
    TH1D* pix_col = new TH1D(histname, histtitle, 41,  0,    4100);
    m_h_pix_col.push_back(pix_col);
    histograms->push_back(pix_col);
    
    // col width
    
    histname="h_FTKMoni_PIX";
    histname+=ilay;
    histname+="_colw";
    
    histtitle="PIX";
    histtitle+=ilay;
    histtitle+=": col (eta) width";
    
    
    TH1D* pix_colW = new TH1D(histname, histtitle, 8, -0.5, 7.5);
    m_h_pix_colW.push_back(pix_colW);
    histograms->push_back(pix_colW);
    
  }
  std::vector<unsigned int> sctplane;
  if (m_Nlayers==8) {
    sctplane={0,2,4,5,6};
  } else {
    sctplane={0,1,2,3,4,5,6,7};
  }
  
  for(auto const& ilay: sctplane) {
    // coord
    
    histname="h_FTKMoni_SCT";
    histname+=ilay;
    histname+="_coord";
    
    histtitle="SCT";
    histtitle+=ilay;
    histtitle+=": coord;coord;No. Hits";
    
    
    TH1D* sct_coord = new TH1D(histname, histtitle, 42,  0,    2100);
    m_h_sct_coord.push_back(sct_coord);
    histograms->push_back(sct_coord);


    // barrel side 0 width

    histname="h_FTKMoni_SCT";
    histname+=ilay;
    histname+="_width";
    
    histtitle="SCT";
    histtitle+=ilay;
    histtitle+=": width;width;No. Hits";
    

    TH1D* sct_width = new TH1D(histname, histtitle, 8,  -0.5,    7.5);
    m_h_sct_width.push_back(sct_width);
    histograms->push_back(sct_width);
  }

  TString trackName[7]={"Raw","Raw_wid1", "Raw_wid2","Raw_wid3","Raw_wid4","Cnv", "Rft"};

  m_h_pixb_dlocX.resize(7);
  m_h_pixb_dlocY.resize(7);
  m_h_pixe_dlocX.resize(7);
  m_h_pixe_dlocY.resize(7);
  m_h_sctb_0_dlocX.resize(7);
  m_h_sctb_1_dlocX.resize(7);
  m_h_scte_0_dlocX.resize(7);
  m_h_scte_1_dlocX.resize(7);
  
  for (unsigned int trackType=0; trackType<7; trackType++) {
    
    ATH_MSG_DEBUG(" Booking dlocX, dlocY hists for " << trackName[trackType]);
    for (Int_t ilay=0; ilay<4; ilay++){
      
      
      // PIXB dLocX
      
      histname="h_FTKMoni_PIXB";
      histname+=ilay;
      histname+="_dlocX_";
      histname+=trackName[trackType];
      
      histtitle="PIXB";
      histtitle+=ilay;
      histtitle+=": locx(offline)-locx(ftk"+trackName[trackType]+")";
      
      TH1D* pixb_dlocX = new TH1D(histname, histtitle, 500,  -0.5,    0.5);
      m_h_pixb_dlocX[trackType].push_back(pixb_dlocX);
      histograms->push_back(pixb_dlocX);
      
      // PIXB dLocY
      
      histname="h_FTKMoni_PIXB";
      histname+=ilay;
      histname+="_dlocY_";
      histname+=trackName[trackType];
      
      histtitle="PIXB";
      histtitle+=ilay;
      histtitle+=": locy(offline)-locy(ftk"+trackName[trackType]+")";
      
      
      TH1D* pixb_dlocY = new TH1D(histname, histtitle, 500,  -0.5,    0.5);
      m_h_pixb_dlocY[trackType].push_back(pixb_dlocY);
      histograms->push_back(pixb_dlocY);
      
      
      if (ilay < 3) {
	
	
	// PIXE dlocX
	
	histname="h_FTKMoni_PIXE";
	histname+=ilay;
	histname+="_dlocX_";
	histname+=trackName[trackType];

	histtitle="PIXE";
	histtitle+=ilay;
	histtitle+=": locx(offline)-locx(ftk"+trackName[trackType]+")";
	
	TH1D* pixe_dlocX = new TH1D(histname, histtitle, 500,  -0.5,    0.5);
	m_h_pixe_dlocX[trackType].push_back(pixe_dlocX);
	histograms->push_back(pixe_dlocX);
	
	// PIXE dlocY
	
	histname="h_FTKMoni_PIXE";
	histname+=ilay;
	histname+="_dlocY_";
	histname+=trackName[trackType];

	histtitle="PIXE";
	histtitle+=ilay;
	histtitle+=": locy(offline)-locy(ftk"+trackName[trackType]+")";
	
	TH1D* pixe_dlocY = new TH1D(histname, histtitle, 500,  -0.5,    0.5);
	m_h_pixe_dlocY[trackType].push_back(pixe_dlocY);
	histograms->push_back(pixe_dlocY);
      }      
      
      
      // SCT barrel side 0 dlocx
      
      histname="h_FTKMoni_SCTB";
      histname+=ilay;
      histname+="_0_dlocX_";
      histname+=trackName[trackType];

      histtitle="SCTB";
      histtitle+=ilay;
      histtitle+="(side 0): locx(offline)-locx(ftk"+trackName[trackType]+")";
      
      
      TH1D* sctb_0_dlocX = new TH1D(histname, histtitle, 500,  -0.5,    0.5);
      m_h_sctb_0_dlocX[trackType].push_back(sctb_0_dlocX);
      histograms->push_back(sctb_0_dlocX);
      
      
      
      // SCT barrel side 1 dlocx
      
      histname="h_FTKMoni_SCTB";
      histname+=ilay;
      histname+="_1_dlocX_";
      histname+=trackName[trackType];

      histtitle="SCTB";
      histtitle+=ilay;
      histtitle+="(side 1): locx(offline)-locx(ftk"+trackName[trackType]+")";
      
      
      TH1D* sctb_1_dlocX = new TH1D(histname, histtitle, 500,  -0.5,    0.5);
      m_h_sctb_1_dlocX[trackType].push_back(sctb_1_dlocX);
      histograms->push_back(sctb_1_dlocX);
      
      
      // SCT endcap side 0 dlocx
      
      histname="h_FTKMoni_SCTE";
      histname+=ilay;
      histname+="_0_dlocX_";
      histname+=trackName[trackType];
      
      histtitle="SCTE";
      histtitle+=ilay;
      histtitle+="(side 0): locx(offline)-locx(ftk"+trackName[trackType]+")";
      
      
      TH1D* scte_0_dlocX = new TH1D(histname, histtitle, 500,  -0.5,    0.5);
      m_h_scte_0_dlocX[trackType].push_back(scte_0_dlocX);
      histograms->push_back(scte_0_dlocX);
      
      
      // SCT endcap side 1 dlocx
      
      histname="h_FTKMoni_SCTE";
      histname+=ilay;
      histname+="_1_dlocX_";
      histname+=trackName[trackType];

      histtitle="SCTE";
      histtitle+=ilay;
      histtitle+="(side 1): locx(offline)-locx(ftk"+trackName[trackType]+")";
      
      
      TH1D* scte_1_dlocX = new TH1D(histname, histtitle, 500,  -0.5,    0.5);
      m_h_scte_1_dlocX[trackType].push_back(scte_1_dlocX);
      histograms->push_back(scte_1_dlocX);
    }
  }

  m_h_ftk_sct_clusWidth = new TH1D("h_FTKMoni_sct_clusWidth", "SCT cluster width", 8,  -0.5,    7.5);
  histograms->push_back(m_h_ftk_sct_clusWidth);

  m_h_ftk_pix_phiClusWidth = new TH1D("h_FTKMoni_pix_phiClusWidth", "Pixel cluster width in phi", 8,  -0.5,    7.5);
  histograms->push_back(m_h_ftk_pix_phiClusWidth);

  m_h_ftk_pix_etaClusWidth = new TH1D("h_FTKMoni_pix_etaClusWidth", "Pixel cluster width in eta", 8,  -0.5,    7.5);
  histograms->push_back(m_h_ftk_pix_etaClusWidth);
  
}
		    
		    


void FTK_RDO_MonitorAlgo::fillMaps(const FTK_RawTrackContainer* rawTracks, std::vector<std::vector<unsigned int>*> &pixList, std::vector<std::vector<unsigned int>*> &sctList) {

  
  //  ATH_MSG_DEBUG("fillMaps");

  unsigned int pixMaxHash = m_pixelId->wafer_hash_max();
  unsigned int sctMaxHash = m_sctId->wafer_hash_max();
  
  
  
  
  //    const FTK_RawTrack* highPtTrack = nullptr;


  std::vector<unsigned int>  missingPixLayer(4);
  std::vector<unsigned int> missingSctLayer(8);

  
  FTK_RawTrackContainer::const_iterator pTrack = rawTracks->begin();
  FTK_RawTrackContainer::const_iterator pLastTrack = rawTracks->end();
  for ( int itr=0; pTrack!= pLastTrack; pTrack++, itr++) {

    const FTK_RawTrack* ftkRawTrack = *pTrack;

    float ftkTrkTheta = std::atan2(1.0,(ftkRawTrack)->getCotTh());
    float ftkTrkEta = -std::log(std::tan(ftkTrkTheta/2.));
    float ftkTrkPt=1.e10;
    if (fabs((ftkRawTrack)->getInvPt()) >= 1e-10) ftkTrkPt=1./(ftkRawTrack)->getInvPt();
       
    
    m_h_FTK_pt->Fill(ftkTrkPt);
    m_h_FTK_eta->Fill(ftkTrkEta);
    m_h_FTK_phi->Fill((ftkRawTrack)->getPhi());
    m_h_FTK_d0->Fill((ftkRawTrack)->getD0());
    m_h_FTK_z0->Fill((ftkRawTrack)->getZ0());

    unsigned int tower = (*pTrack)->getTower();
    unsigned int sector = (*pTrack)->getSectorID();

    ATH_MSG_VERBOSE( itr << std::hex <<
		     ": Tower 0x" << tower << "     SectorID 0x" << (*pTrack)->getSectorID()   <<   
		     "  RoadID 0x"  << (*pTrack)->getRoadID() << " LayerMap 0x" << (*pTrack)->getLayerMap() << std::dec <<
		     " nPix: " << (*pTrack)->getPixelClusters().size() << " nSCT: "<< (*pTrack)->getSCTClusters().size());

    unsigned int iPlane=0,ihist=0;
    if (m_Nlayers==8) iPlane=1;

    for( ; iPlane < 4; iPlane++, ihist++){
      
      
      if ((*pTrack)->isMissingPixelLayer(iPlane))  {
	ATH_MSG_VERBOSE(" Missing Pixel Layer " <<  iPlane);
	if (iPlane<4)missingPixLayer[iPlane]++;
	continue;
      }
            
      
      IdentifierHash hash=0xffffffff;
      IdentifierHash hashfromTrack=0xffffffff;
      IdentifierHash hashfromConstants =0xffffffff;

      bool trackHashValid=false;
      bool constHashValid=false;

      if (m_getHashFromTrack) {
	hashfromTrack = (*pTrack)->getPixelClusters()[iPlane].getModuleID();
	if (hashfromTrack<pixMaxHash) {
	  trackHashValid=true;
	  ATH_MSG_VERBOSE(" pixel HashID from track 0x" << std::hex << hashfromTrack << std::dec << " for tower " << tower << " sector " << sector << " plane " << iPlane);
	} else {
	  ATH_MSG_WARNING(" invalid pixel HashID from track 0x" << std::hex << hashfromTrack << std::dec << " for tower " << tower << " sector " << sector << " plane " << iPlane);
	}
      }
      
      if (m_getHashFromConstants) {
	hashfromConstants = m_HashIDTool->getHash(tower, sector, iPlane);
	if (hashfromConstants < pixMaxHash) {
	  constHashValid=true;
	  ATH_MSG_VERBOSE(" pixel HashID from constants 0x" << std::hex << hashfromConstants << std::dec << " for tower " << tower << " sector " << sector << " plane " << iPlane);
	} else {
	  ATH_MSG_WARNING(" invalid pixel HashID from constants 0x" << std::hex << hashfromConstants << std::dec << " for tower " << tower << " sector " << sector << " plane " << iPlane);
	}
      }

      if (constHashValid && trackHashValid) {
	if (hashfromTrack != hashfromConstants) { 
	  
	  ATH_MSG_WARNING(" Pixel HashID missmatch: hash from Track 0x" << std::hex << hashfromTrack  << " hash from Constants 0x" << hashfromConstants << std::dec <<" tower " << tower << " sector " << sector << " plane " << iPlane);
	  
	} else {
	  ATH_MSG_VERBOSE(" Pixel HashID successful match: hash from Track 0x" << std::hex << hashfromTrack  << " hash from Constants 0x" << hashfromConstants << std::dec <<" tower " << tower << " sector " << sector << " plane " << iPlane);
	}
      }
      
      if (m_getHashFromConstants && constHashValid) {
	hash = hashfromConstants;
	ATH_MSG_VERBOSE("Using Pixel hash from constants");
      } else if (m_getHashFromTrack && trackHashValid) {
	hash = hashfromTrack;
	ATH_MSG_VERBOSE("Using Pixel hash from track");
      } else {
	ATH_MSG_VERBOSE("No valid Pixel hash, skipping this hit - hashfromTrack " << hashfromTrack << " hash from Constants " << hashfromConstants);
	continue;
      }

      
      m_h_pix_row[ihist]->Fill((*pTrack)->getPixelClusters()[iPlane].getRowCoord());
      m_h_pix_rowW[ihist]->Fill((*pTrack)->getPixelClusters()[iPlane].getRowWidth());
      m_h_pix_col[ihist]->Fill((*pTrack)->getPixelClusters()[iPlane].getColCoord());
      m_h_pix_colW[ihist]->Fill((*pTrack)->getPixelClusters()[iPlane].getColWidth());

      ATH_MSG_VERBOSE(" FTK_RawPixelCluster " << iPlane << std::hex <<
		      " Row Coord= 0x" << (unsigned int) (*pTrack)->getPixelClusters()[iPlane].getRowCoord() <<
		      " Row Width= 0x" << (*pTrack)->getPixelClusters()[iPlane].getRowWidth() <<
		      " Col Coord= 0x" << (unsigned int) (*pTrack)->getPixelClusters()[iPlane].getColCoord() <<
		      " Col Width= 0x" <<(*pTrack)->getPixelClusters()[iPlane].getColWidth() <<
		      " Hash ID= 0x" << hash << std::dec	      
		      );
      ATH_MSG_VERBOSE(m_id_helper->print_to_string(m_pixelId->wafer_id(hash)));
      if (hash<pixMaxHash) {
	ATH_MSG_DEBUG(" Adding to pixList[ "<<hash<<" ] track " << itr);
	if (!pixList[hash]) pixList[hash]= new std::vector<unsigned int>;
	pixList[hash]->push_back(itr);
      }
      
    }
    std::vector<unsigned int> sctplane;
    if (m_Nlayers==8) {
      sctplane={0,2,4,5,6};
    } else {
      sctplane={0,1,2,3,4,5,6,7};
    }

    ihist=0;
    for(auto isct:sctplane){
      iPlane = isct+4;
      if ((*pTrack)->isMissingSCTLayer(isct)) {
	ATH_MSG_VERBOSE(" Missing SCT Layer" << isct);
	if (isct<8)missingSctLayer[isct]++;
	continue;
      }
      

      IdentifierHash hash=0xffffffff;
      IdentifierHash hashfromTrack=0xffffffff;
      IdentifierHash hashfromConstants=0xffffffff;

      bool trackHashValid=false;
      bool constHashValid=false;

      if (m_getHashFromTrack) {
	hashfromTrack = (*pTrack)->getSCTClusters()[isct].getModuleID();
	if (hashfromTrack < sctMaxHash) {
	  trackHashValid=true;
	  ATH_MSG_VERBOSE(" SCT HashID from track 0x" << std::hex << hashfromTrack << std::dec << " for tower " << tower << " sector " << sector << " plane " << iPlane);
	} else {
	  ATH_MSG_WARNING(" invalid SCT HashID from track 0x" << std::hex << hashfromTrack << std::dec << " for tower " << tower << " sector " << sector << " plane " << iPlane);
      
	}
      }
      if (m_getHashFromConstants) {
        hashfromConstants = m_HashIDTool->getHash(tower, sector, iPlane);
	if (hashfromConstants < sctMaxHash) {
	  ATH_MSG_VERBOSE(" SCT HashID from constants 0x" << std::hex << hashfromConstants << std::dec << " for tower " << tower << " sector " << sector << " plane " << iPlane);
	  constHashValid=true;
	} else {
	  ATH_MSG_WARNING(" invalid SCT HashID from constants 0x" << std::hex << hashfromConstants << std::dec << " for tower " << tower << " sector " << sector << " plane " << iPlane);
	} 
      }
	  
      if ( trackHashValid && constHashValid) {
	if(hashfromTrack != hashfromConstants) {
	  
	  ATH_MSG_WARNING(" SCT HashID missmatch: hash from Track 0x" << std::hex << hashfromTrack << " hash from Constants 0x" << hashfromConstants << std::dec <<" tower " << tower << " sector " << sector << " plane " << iPlane);
	  
	} else {
	  ATH_MSG_VERBOSE(" SCT HashID successful match: hash from Track 0x" << std::hex << hashfromTrack << " hash from Constants 0x" << hashfromConstants << std::dec <<" tower " << tower << " sector " << sector << " plane " << iPlane);
	}
      }

      if (m_getHashFromConstants && constHashValid) {
	hash = hashfromConstants;
	ATH_MSG_VERBOSE("Using SCT hash from constants");
      } else if (m_getHashFromTrack && trackHashValid) {
	hash = hashfromTrack;
	ATH_MSG_VERBOSE("Using SCT hash from track");
      } else {
	ATH_MSG_VERBOSE("No valid SCT hash, skipping this hit - hashfromTrack " << hashfromTrack << " hash from Constants " << hashfromConstants );
	continue;
      }

      ATH_MSG_VERBOSE(" FTK_RawSCT_Cluster " << isct << " FTK plane 0x" << iPlane << std::hex <<
		      " Hit Coord= 0x" << (unsigned int) (*pTrack)->getSCTClusters()[isct].getHitCoord() <<
		      " Hit Width= 0x" << (*pTrack)->getSCTClusters()[isct].getHitWidth() <<
		      " Hash ID = 0x" << hash << std::dec);
      ATH_MSG_VERBOSE(m_id_helper->print_to_string(m_sctId->wafer_id(hash)));

      m_h_sct_coord[ihist]->Fill((*pTrack)->getSCTClusters()[isct].getHitCoord());
      m_h_sct_width[ihist]->Fill((*pTrack)->getSCTClusters()[isct].getHitWidth());

      if (hash<sctMaxHash) {
	ATH_MSG_DEBUG(" Adding to sctList[ "<<hash<<" ] track " << itr);
	
	if (!sctList[hash]) sctList[hash]= new std::vector<unsigned int>;
	  sctList[hash]->push_back(itr);
      }
    
      ihist++;
    }
  } // end loop over tracks
  

  unsigned int ntracks = rawTracks->size();
  if (ntracks > 0) {
    unsigned int ilay=0;
    for (auto& ipix:missingPixLayer) {
      double frac = (double) ipix/ (double) ntracks;
      ATH_MSG_DEBUG(" Fraction of Tracks Missing Hits from Layer " << ilay << " = "<< frac); 
      m_h_missingHits->Fill(ilay++,frac);
    }
    for (auto& isct:missingSctLayer) {
      double frac = (double) isct/ (double) ntracks;
      ATH_MSG_DEBUG(" Fraction of Tracks Missing Hits from Layer " << ilay << " = "<< frac); 
      double x = (double) ilay++;
      m_h_missingHits->Fill(x,frac);
    }
  }
}





const std::pair<unsigned int, unsigned int> FTK_RDO_MonitorAlgo::matchTrack(const Trk::Track* track, 
									    std::vector<std::vector<unsigned int>*> &pixList, // list FTK tracks containing pix hashID 
									    std::vector<std::vector<unsigned int>*> &sctList, // list FTK tracks containing SCT hashID
									    std::map<unsigned int,std::pair<double,double>>& offlinetrackPixLocxLocy,
									    std::map<unsigned int,double>& offlinetrackSctLocx,
									    bool& uniqueMatch
									    ) {

  //  ATH_MSG_DEBUG("matchTrack");
  std::pair<unsigned int, unsigned int> index_freq(0,0);
  
  const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();   
  if(!trackStates) {
    //    ATH_MSG_DEBUG(" No trackStates");
    return index_freq;
  }
  
  
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end=trackStates->end();
  if (!(*it)) {
    //    ATH_MSG_WARNING("TrackStateOnSurface == Null");
    return index_freq;
  }

  unsigned int pixMaxHash = m_pixelId->wafer_hash_max();
  unsigned int sctMaxHash = m_sctId->wafer_hash_max();
  
  
  std::vector<unsigned int> ftkTrackIndexList;
  
  
  unsigned int iHit=0;
  for (unsigned int iSurf=0; it!=it_end; it++, iSurf++) {
    //    ATH_MSG_DEBUG(" Surface " << iSurf);
    
    const Trk::TrackStateOnSurface* tsos=(*it);	
    if (tsos == 0) {
      //      ATH_MSG_DEBUG(" No  TrackStateOnSurface ");
      continue;
    }
    if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ){
      //      ATH_MSG_DEBUG(" Measurment Surface");
      const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
      if (!measurement) {
	//	ATH_MSG_DEBUG(" No MeasurmentOnTrack");
	continue;
      }
      
      const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(measurement);
      if (!hit) {
	//	ATH_MSG_DEBUG(" No Hit");
	continue;
      }
      //      ATH_MSG_DEBUG(" got hit - getting ID");
      const Identifier & hitId = hit->identify();
      
      const IdentifierHash hash = hit->idDE();
      
      
      //      ATH_MSG_DEBUG(" getting  locX locY");
      const float locX = (float)measurement->localParameters()[Trk::locX];
      
      if (m_id_helper->is_pixel(hitId)) {

	const float locY = (float)measurement->localParameters()[Trk::locY];

	if (hash < pixMaxHash) {
	  const std::pair<double, double>  locXlocY(locX,locY);
	  offlinetrackPixLocxLocy.insert({hash,locXlocY});
	  if (pixList[hash]) {
	    ATH_MSG_VERBOSE("Pixel Hit on Offline track: hash 0x"<<std::hex<<hash<<": "<<  m_id_helper->print_to_string(m_pixelId->wafer_id(hash))<< std::dec);
	    ATH_MSG_VERBOSE(" is on the following FTK tracks:");
	    
	    for (auto& ipixtr:*pixList[hash]) {
	      ATH_MSG_VERBOSE(ipixtr);
	      ftkTrackIndexList.push_back(ipixtr);
	    }
	  }
	}
	
      } else {

	  
	offlinetrackSctLocx.insert({hash,locX});

	
	if (hash < sctMaxHash) {
	  if (sctList[hash]) {
	    ATH_MSG_VERBOSE("SCT Hit on Offline track: hash 0x"<<std::hex<<hash<<": "<<  m_id_helper->print_to_string(m_sctId->wafer_id(hash))<< std::dec);
	    ATH_MSG_VERBOSE(" is on the following FTK tracks:");
	    
	    for (auto& iscttr:*sctList[hash]) {
	      ATH_MSG_VERBOSE(iscttr);
	      ftkTrackIndexList.push_back(iscttr);
	    }
	  }
	}
	
      }
      iHit++;
      
    }
  }

  if (ftkTrackIndexList.size()==0) return index_freq;

  std::vector<unsigned int> ftktrack;
  std::vector<unsigned int> freq;

  for ( auto& i:ftkTrackIndexList) {
    bool found=false;
    if (ftktrack.size()>0){
      auto pTrack = std::find(ftktrack.begin(), ftktrack.end(), i);
      if ( pTrack!= ftktrack.end() ) {
	unsigned int pos = std::distance(ftktrack.begin(), pTrack);
	freq[pos]++;
	found = true;
      }
    }
    if (!found) {
      ftktrack.push_back(i);
      freq.push_back(1);
    }
  }
  auto pMax = std::max_element(freq.begin(), freq.end());

  if (pMax == freq.end()) return index_freq;

  unsigned int nmatch = std::count(freq.begin(), freq.end(), *pMax);
  
  if (nmatch ==1 ) {
    uniqueMatch=true;
  } else {
    ATH_MSG_DEBUG(" found " << nmatch << " matching FTK tracks" );
  }


  
  unsigned int n = *pMax;

  if (n>m_minMatches) m_h_FTK_nTrackMatch->Fill((double)nmatch);  

  unsigned int index = ftktrack[std::distance(freq.begin(), pMax)];
  
  return std::pair<unsigned int, unsigned int> (index,n);
  
}

void FTK_RDO_MonitorAlgo::compareTracks(const FTK_RawTrack* ftkTrack, 
                                        const InDetDD::SiDetectorElementCollection* sctElements,
					std::map<unsigned int,std::pair<double,double> >& offlinetrackPixLocxLocy,
					std::map<unsigned int,double>& offlinetrackSctLocx
					) {

  unsigned int iPlane = 0;
  unsigned int pixMaxHash = m_pixelId->wafer_hash_max();
  unsigned int sctMaxHash = m_sctId->wafer_hash_max();
  
  for( unsigned int i = 0; i < 4; ++i,iPlane++){
    
	
    if ((ftkTrack)->isMissingPixelLayer(i))  {
      continue;
    }
    /* ATH_MSG_VERBOSE(" FTK_RawPixelCluster " << i <<
		    " Row Coord= " << (ftkTrack)->getPixelClusters()[i].getRowCoord() <<
		    " Row Width= " << (ftkTrack)->getPixelClusters()[i].getRowWidth() <<
		    " Col Coord= " << (ftkTrack)->getPixelClusters()[i].getColCoord() <<
		    " Col Width= " << (ftkTrack)->getPixelClusters()[i].getColWidth() <<
		    " Hash ID= 0x" << std::hex << (ftkTrack)->getPixelClusters()[i].getModuleID() << std::dec <<
		    " Barcode= "   << (ftkTrack)->getPixelClusters()[i].getBarcode() );*/
    
    unsigned int phiWidth =  ftkTrack->getPixelClusters()[i].getRowWidth();
    unsigned int etaWidth =  ftkTrack->getPixelClusters()[i].getColWidth();

    m_h_ftk_pix_phiClusWidth->Fill((double) phiWidth);
    m_h_ftk_pix_etaClusWidth->Fill((double) etaWidth);
  
    const IdentifierHash hash = (ftkTrack)->getPixelClusters()[i].getModuleID();

    if (hash > pixMaxHash) continue;

    Identifier wafer_id = m_pixelId->wafer_id(hash); // Need to set up this tool
    unsigned int layer = m_pixelId->layer_disk(wafer_id);
    bool isBarrel = (m_pixelId->barrel_ec(wafer_id)==0);

    const float rawLocalPhiCoord = ( m_reverseIBLlocx && isBarrel && layer==0)? 
      (2680. -  (ftkTrack)->getPixelClusters()[i].getRowCoord()) : (ftkTrack)->getPixelClusters()[i].getRowCoord();
    
    const float rawLocalEtaCoord = (ftkTrack)->getPixelClusters()[i].getColCoord();
    
    std::string layername;
    if (isBarrel){
      if (layer==0){
	layername = "IBL ";
      } else {
	layername="PIXB";
      }
    } else {
      layername="PIXE";
    }


    const std::pair<double,double> ftkLocxLocy =(this)->getPixLocXlocY(hash,rawLocalPhiCoord, rawLocalEtaCoord);
    auto p = offlinetrackPixLocxLocy.find(hash);
    if (p!=offlinetrackPixLocxLocy.end()) {
      std::pair<double, double> offlineLocxLoxy= p->second;
      ATH_MSG_DEBUG(layername << layer<< " Raw locX: " << rawLocalPhiCoord << " locY: " << rawLocalEtaCoord <<  " locX*6.25e-3 " << rawLocalPhiCoord*6.25e-3 << "locY*25.0e-3: " << rawLocalEtaCoord*25.0e-3); 
      ATH_MSG_DEBUG(layername << layer<< " locX: offline " << offlineLocxLoxy.first << " FTK " << ftkLocxLocy.first << " off-FTK " <<  offlineLocxLoxy.first - ftkLocxLocy.first);
      ATH_MSG_DEBUG(layername << layer<< " locY: offline " << offlineLocxLoxy.second << " FTK " << ftkLocxLocy.second << " off-FTK " <<  offlineLocxLoxy.second - ftkLocxLocy.second);
      if (isBarrel) {
	
	if (layer==0) {
	  m_h_IBL_locX->Fill((int)rawLocalPhiCoord%8);
	  m_h_IBL_locY->Fill((int)rawLocalEtaCoord%8);
	  m_h_IBL_dlocX_fullRange->Fill(offlineLocxLoxy.first - ftkLocxLocy.first);
	  m_h_IBL_dlocY_fullRange->Fill(offlineLocxLoxy.second - ftkLocxLocy.second);
	} else {
	  m_h_pixb_locX->Fill((int)rawLocalPhiCoord%8);
	  m_h_pixb_locY->Fill((int)rawLocalEtaCoord%8);
	}

	m_h_pixb_dlocX[FTK_MonHistType::Raw][layer]->Fill(offlineLocxLoxy.first - ftkLocxLocy.first);
	m_h_pixb_dlocY[FTK_MonHistType::Raw][layer]->Fill(offlineLocxLoxy.second - ftkLocxLocy.second);
	for (unsigned int iwid=1; iwid < 5; iwid++) {
	  if (phiWidth==iwid) m_h_pixb_dlocX[iwid][layer]->Fill(offlineLocxLoxy.first - ftkLocxLocy.first);
	  if (etaWidth==iwid) m_h_pixb_dlocY[iwid][layer]->Fill(offlineLocxLoxy.second - ftkLocxLocy.second);
	}
      } else {
	m_h_pixe_locX->Fill((int)rawLocalPhiCoord%8);
	m_h_pixe_locY->Fill((int)rawLocalEtaCoord%8);
	m_h_pixe_dlocX[FTK_MonHistType::Raw][layer]->Fill(offlineLocxLoxy.first - ftkLocxLocy.first);
	m_h_pixe_dlocY[FTK_MonHistType::Raw][layer]->Fill(offlineLocxLoxy.second - ftkLocxLocy.second);
	for (unsigned int iwid=1; iwid < 5; iwid++) {
	  if (phiWidth==iwid) m_h_pixe_dlocX[iwid][layer]->Fill(offlineLocxLoxy.first - ftkLocxLocy.first);
	  if (etaWidth==iwid) m_h_pixe_dlocY[iwid][layer]->Fill(offlineLocxLoxy.second - ftkLocxLocy.second);
	}
      }

    } else {
      ATH_MSG_DEBUG("didn't find pixel hash " << hash << " on matched offline track");
    }
  
  }
  for( unsigned int isct = 0; isct < 8; ++isct){
    
    if ((ftkTrack)->isMissingSCTLayer(isct)) {
      //ATH_MSG_VERBOSE(" Missing SCT Layer" << isct);
      continue;
    }
    
    /* ATH_MSG_VERBOSE(" FTK_RawSCT_Cluster " << isct <<
       " Hit Coord= " << (ftkTrack)->getSCTClusters()[isct].getHitCoord() <<
       " Hit Width= " << (ftkTrack)->getSCTClusters()[isct].getHitWidth() <<
       " Module ID= " << (ftkTrack)->getSCTClusters()[isct].getModuleID() <<
       " Barcode= "   << (ftkTrack)->getSCTClusters()[isct].getBarcode() ); */
    



    float rawStripCoord = (ftkTrack)->getSCTClusters()[isct].getHitCoord();
    float stripCoord = (ftkTrack)->getSCTClusters()[isct].getHitCoord()/2.; // rawStribCoord is in units of half a strip
    
    int strip = (int) stripCoord;
    IdentifierHash scthash = (ftkTrack)->getSCTClusters()[isct].getModuleID();

    if (scthash > sctMaxHash) continue;

    unsigned int clusterWidth=(ftkTrack)->getSCTClusters()[isct].getHitWidth();


    if (clusterWidth == 0) { // fix for case that width is not set
      if ((int)rawStripCoord%2==1) { 
	clusterWidth=2; // cluster position is odd => between strips => width 2
      } else {
	clusterWidth=1; //  // cluster position is even => on strip => width 1
      } 
    }
    m_h_ftk_sct_clusWidth->Fill(clusterWidth);

    Identifier sctwafer_id = m_sctId->wafer_id(scthash); 
    
    unsigned int layer = m_sctId->layer_disk(sctwafer_id);
    bool isBarrel = (m_sctId->barrel_ec(sctwafer_id)==0);
    unsigned int side = m_sctId->side(sctwafer_id);

    std::string layername;
    if (isBarrel){
      layername="SCTB";
    } else {
      layername="SCTE";
    }
    
    const InDetDD::SiDetectorElement* element = sctElements->getDetectorElement(scthash);
    double ftkLocX  = (this)->getSctLocX(element,rawStripCoord,clusterWidth);
    
    auto p = offlinetrackSctLocx.find(scthash);
    if (p!=offlinetrackSctLocx.end()) {
      double offlineLocX = p->second;
      ATH_MSG_DEBUG(layername << layer<< ":"<<side<<" Raw locX: " << rawStripCoord << " locX/2: " << strip << " clusterWidth " << clusterWidth);
      ATH_MSG_DEBUG(layername << layer<< ":"<<side<< " locX: offline " << offlineLocX << " FTK(mode 2) " << ftkLocX << " off-FTK " <<  offlineLocX  - ftkLocX);
      
      if (isBarrel) {
	m_h_sctb_locX->Fill((int)(ftkTrack)->getSCTClusters()[isct].getHitCoord()%2);
	if (side==0) {
 	  m_h_sctb_0_dlocX[FTK_MonHistType::Raw][layer]->Fill(offlineLocX  - ftkLocX);
 	  if (clusterWidth==0) m_h_sctb_0_dlocX[FTK_MonHistType::Raw_wid1][layer]->Fill(offlineLocX  - ftkLocX);
	  for (unsigned int iwid=1; iwid < 5; iwid++) {
	    if (clusterWidth==(iwid)) m_h_sctb_0_dlocX[iwid][layer]->Fill(offlineLocX  - ftkLocX);
	  }
	} else {
 	  m_h_sctb_1_dlocX[FTK_MonHistType::Raw][layer]->Fill(offlineLocX  - ftkLocX);
	  for (unsigned int iwid=1; iwid < 5; iwid++) {
	    if (clusterWidth==(iwid)) m_h_sctb_1_dlocX[iwid][layer]->Fill(offlineLocX  - ftkLocX);
	  }
	}
      } else {
	unsigned int ring = m_sctId->eta_module(sctwafer_id);

	m_h_scte_locX->Fill((int)(ftkTrack)->getSCTClusters()[isct].getHitCoord()%2);

	if (side==0) {
 	  m_h_scte_0_dlocX[FTK_MonHistType::Raw][ring]->Fill(offlineLocX  - ftkLocX);
	  for (unsigned int iwid=1; iwid < 5; iwid++) {
	    if (clusterWidth==(iwid)) m_h_scte_0_dlocX[iwid][ring]->Fill(offlineLocX  - ftkLocX);
	  }
	} else {
 	  m_h_scte_1_dlocX[FTK_MonHistType::Raw][ring]->Fill(offlineLocX  - ftkLocX);
	  for (unsigned int iwid=1; iwid < 5; iwid++) {
	    if (clusterWidth==(iwid)) m_h_scte_1_dlocX[iwid][ring]->Fill(offlineLocX  - ftkLocX);
	  }
	}
      }

    } else {
            ATH_MSG_DEBUG("did not find SCT hash " << scthash << " on matched offline track");
    }
    
  }
   
} 


void FTK_RDO_MonitorAlgo::compareTracks(const Trk::Track* ftkTrack, 
					std::map<unsigned int,std::pair<double,double> >& offlinetrackPixLocxLocy,
					std::map<unsigned int,double>& offlinetrackSctLocx,
					bool refitTrack) {

  ATH_MSG_DEBUG("compare FTK Trk::Track with offline TrkTrack");

  unsigned int histIndex=FTK_MonHistType::Cnv;
  if (refitTrack) histIndex=FTK_MonHistType::Rft;

  const DataVector<const Trk::TrackStateOnSurface>* trackStates=ftkTrack->trackStateOnSurfaces();   
  if(!trackStates) {
    //    ATH_MSG_DEBUG(" No trackStates");
    return;
  }
  
  
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end=trackStates->end();
  if (!(*it)) {
    ATH_MSG_WARNING("TrackStateOnSurface == Null");
    return;
  }

  for (unsigned int iSurf=0; it!=it_end; it++, iSurf++) {
    ATH_MSG_DEBUG(" Surface " << iSurf);
    
    const Trk::TrackStateOnSurface* tsos=(*it);	
    if (tsos == 0) {
      ATH_MSG_DEBUG(" No  TrackStateOnSurface ");
      continue;
    }
    if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ){
      ATH_MSG_DEBUG(" Measurment Surface");
      const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
      if (!measurement) {
	ATH_MSG_DEBUG(" No MeasurmentOnTrack");
	continue;
      }
      
      const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(measurement);
      if (!hit) {
	ATH_MSG_DEBUG(" No Hit");
	continue;
      }
      ATH_MSG_DEBUG(" got hit - getting ID");
      const Identifier & hitId = hit->identify();
      
      const IdentifierHash hash = hit->idDE();
      
      
      //      ATH_MSG_DEBUG(" getting  locX locY");
      const float locX = (float)measurement->localParameters()[Trk::locX];

      if (m_id_helper->is_pixel(hitId)) {
	const float locY = (float)measurement->localParameters()[Trk::locY];

	Identifier wafer_id = m_pixelId->wafer_id(hash); 
	unsigned int layer = m_pixelId->layer_disk(wafer_id);
	bool isBarrel = (m_pixelId->barrel_ec(wafer_id)==0);

	auto p = offlinetrackPixLocxLocy.find(hash);
	if (p!=offlinetrackPixLocxLocy.end()) {
	  std::pair<double, double> offlineLocxLoxy= p->second;
	  std::string layername;
	  if (isBarrel){
	    if (layer==0){
	      layername = "IBL ";
	    } else {
	      layername="PIXB";
	    }
	  } else {
	    layername="PIXE";
	  }
	  ATH_MSG_DEBUG(layername << layer<< " locX: offline " << offlineLocxLoxy.first << " FTK " << locX << " off-FTK " <<  offlineLocxLoxy.first - locX);
	  ATH_MSG_DEBUG(layername << layer<< " locY: offline " << offlineLocxLoxy.second << " FTK " << locY << " off-FTK " <<  offlineLocxLoxy.second - locY);
	  if (isBarrel) {
	    m_h_pixb_dlocX[histIndex][layer]->Fill(offlineLocxLoxy.first - locX);
	    m_h_pixb_dlocY[histIndex][layer]->Fill(offlineLocxLoxy.second - locY);
	  } else {
	    m_h_pixe_dlocX[histIndex][layer]->Fill(offlineLocxLoxy.first - locX);
	    m_h_pixe_dlocY[histIndex][layer]->Fill(offlineLocxLoxy.second - locY);
	  }

	} else {
	  ATH_MSG_DEBUG("didn't find pixel hash " << hash << " on matched offline track");
	}
      } else { // SCT hit

	Identifier sctwafer_id = m_sctId->wafer_id(hash); 
	
	unsigned int layer = m_sctId->layer_disk(sctwafer_id);
	bool isBarrel = (m_sctId->barrel_ec(sctwafer_id)==0);
	unsigned int side = m_sctId->side(sctwafer_id);

	auto p = offlinetrackSctLocx.find(hash);
	if (p!=offlinetrackSctLocx.end()) {
	  double offlineLocX = p->second;
	  
	  std::string layername;
	  if (isBarrel){
	    layername="SCTB";
	  } else {
	    layername="SCTE";
	  }
	  ATH_MSG_DEBUG(layername << layer<< ":"<<side<< " locX: offline " << offlineLocX << " FTK " << locX << " off-FTK " <<  offlineLocX  - locX);
	  
	  if (isBarrel) {
	    if (side==0) {
	      m_h_sctb_0_dlocX[histIndex][layer]->Fill(offlineLocX  - locX);
	    } else {
	      m_h_sctb_1_dlocX[histIndex][layer]->Fill(offlineLocX  - locX);
	    }
	  } else {
	    unsigned int ring = m_sctId->eta_module(sctwafer_id);
	    
	    if (side==0) {
	      m_h_scte_0_dlocX[histIndex][ring]->Fill(offlineLocX  - locX);
	    } else {
	      m_h_scte_1_dlocX[histIndex][ring]->Fill(offlineLocX  - locX);
	    }
	  }
	  
	} else {
	  ATH_MSG_DEBUG("did not find SCT hash " << hash << " on matched offline track");
	  
	}
      }
    }
  }
   
} 



const std::pair<double,double> FTK_RDO_MonitorAlgo::getPixLocXlocY(const IdentifierHash hash, const float rawLocalPhiCoord, const float rawLocalEtaCoord) {
	
  const InDetDD::SiDetectorElement* pixelDetectorElement = m_pixelManager->getDetectorElement(hash);
  const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&pixelDetectorElement->design());
  
  const InDetDD::SiCellId cornerCell(0, 0);
  const InDetDD::SiLocalPosition localPositionOfCornerCell = design->localPositionOfCell(cornerCell);


  double shift = m_pixelLorentzAngleTool->getLorentzShift(hash);

  const double phi0 = localPositionOfCornerCell.xPhi();
  const double eta0 = localPositionOfCornerCell.xEta();
  
  //  ATH_MSG_VERBOSE( " local position of pixel at (0,0) is "<<  phi0 << ",  " << eta0);
  

  // zero is center of the row coordinates, so to find the cell we can use it, units of 6.25 microns
  // zero is edge of the column coordinates, so to find the cell we add 0.5, units of 25 microns
  double phiPos = rawLocalPhiCoord * 6.25e-3 + phi0 + shift; // rawLocalPhiCoord=0 is the centre of the zeroth pixel
  
  Identifier wafer_id = m_pixelId->wafer_id(hash);
  unsigned int layer = m_pixelId->layer_disk(wafer_id);
  bool isBarrel = (m_pixelId->barrel_ec(wafer_id)==0);
  
  if (!isBarrel) phiPos-=0.025; // correction for the fact that rawLocalPhiCoord=0 seems to be the edge of the endcap pixel, not the centre!

  double etaPos = 0.;
  
  if (isBarrel && layer==0) {
     etaPos =  rawLocalEtaCoord * 25.0e-3 + eta0 - 0.25; // rawLocalEtaCoord=0 is the edge (-0.25mm) of the zeroth IBL pixel.
  } else {
     etaPos =  rawLocalEtaCoord * 25.0e-3 + eta0 - 0.3; // rawLocalEtaCoord=0 is the edge (-0.3mm) of the zeroth pixel.
  }    
  
  //ATH_MSG_VERBOSE( "Cluster position phiPos, etaPos "<<  phiPos << ", " << etaPos);
  
  if ( etaPos >  design->length()/2.) {
    ATH_MSG_DEBUG( "Illegal position: Setting Cluster eta position to module length/2.");
    etaPos = design->length()/2.;
  } else if (  etaPos <  -design->length()/2. ) {
    etaPos = -design->length()/2.;
    ATH_MSG_DEBUG( "llegal position: Setting Cluster eta position to -(module length/2.)");
  }
  if ( phiPos >  design->width()/2.) {
    phiPos = design->width()/2.;
    ATH_MSG_DEBUG( "llegal position: Setting Cluster phi position to module width/2.");
  } else if (  phiPos <  -design->width()/2. ) {
    phiPos = -design->width()/2.;
    ATH_MSG_DEBUG( "llegal position: Setting Cluster phi position to -(module width/2.)");
  }
  
  return std::pair<double,double>(phiPos,etaPos);
}
  

double FTK_RDO_MonitorAlgo::getSctLocX(const InDetDD::SiDetectorElement* pDE, const float rawstripCoord, const int clusterWidth) {

  const IdentifierHash hash = pDE->identifyHash();

  double xloc=0.;

  float stripCoord = rawstripCoord/2.; // rawStribCoord is in units of half a strip
	
  const InDetDD::SCT_ModuleSideDesign* design;

  double shift = m_sctLorentzAngleTool->getLorentzShift(hash);

  Identifier wafer_id = m_sctId->wafer_id(hash);

  if (m_sctId->is_barrel(wafer_id)){
    design = (static_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(&pDE->design()));
  } else{
    design = (static_cast<const InDetDD::SCT_ForwardModuleSideDesign*>(&pDE->design()));
  }
	
    
  int firstStrip = (int)(stripCoord-0.5*(clusterWidth-1));
  int lastStrip  = (int)(stripCoord+0.5*(clusterWidth-1));
  ATH_MSG_VERBOSE("rawstripCoord " << rawstripCoord << " stripCoord " << " firstStrip " << firstStrip<< " lastStrip " << lastStrip);

  if (firstStrip < 0) {
    firstStrip = 0;
    //    ATH_MSG_VERBOSE( " firstStrip was " << firstStrip << " setting to 0");
  }
  if (lastStrip >= design->cells()) {
    lastStrip = design->cells()-1;
    //    ATH_MSG_VERBOSE( " lastStrip was " << lastStrip << " setting to " << design->cells()-1);
  }
  
  const InDetDD::SiCellId cell1(firstStrip - 1);
  const InDetDD::SiCellId cell2(lastStrip + 1);
  
  const InDetDD::SiLocalPosition firstStripPos(pDE->rawLocalPositionOfCell(cell1));
  const InDetDD::SiLocalPosition lastStripPos(pDE->rawLocalPositionOfCell(cell2));
  
  const InDetDD::SiLocalPosition centre((firstStripPos+lastStripPos)/2.0);
  
  xloc = centre.xPhi()+shift;
  ATH_MSG_VERBOSE("xloc " << xloc << " stripcoord " << stripCoord << " firstStrip " <<   firstStrip << " lastStrip " << lastStrip << " centre.xPhi() " <<  centre.xPhi() << " shift " << shift);
  

  return xloc;
}
