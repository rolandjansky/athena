/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms for tracks (combined ID and pixel only tracks)
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"         
//#include "TrkParameters/MeasuredAtaPlane.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "TH1F.h"   
#include "TH1I.h"   
#include "TH2F.h"
#include "TH2I.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include <sstream>
#include <algorithm>

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
#include "TrkTrack/TrackCollection.h"            
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StatusCode PixelMainMon::BookTrackMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Tracks" << endmsg;  

   std::string path = "Pixel/Track";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/TrackOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/TrackOnPixelTrack");
   MonGroup trackHistos( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms

   std::string modlabel[9];
   modlabel[0]="ECA"; modlabel[1]="ECC";
   modlabel[2]="B0";  modlabel[3]="B1";    modlabel[4]="B2"; 
   modlabel[5]="IBL"; modlabel[6]="IBL2D"; modlabel[7]="IBL3D"; 
   std::string modlabel2[PixLayerIBL2D3DDBM::COUNT];
   modlabel2[0]="ECA0"; modlabel2[1]="ECA1"; modlabel2[2]="ECA2"; 
   modlabel2[3]="ECC0"; modlabel2[4]="ECC1"; modlabel2[5]="ECC2"; 
   modlabel2[6]="B0";  modlabel2[7]="B1";  modlabel2[8]="B2"; 
   modlabel2[9]="DBMA";modlabel2[10]="DBMC";
   modlabel2[11]="IBL"; modlabel2[12]="IBL2D"; modlabel2[13]="IBL3D"; 
   std::string hname;
   std::string htitles;

   sc=trackHistos.regHist(m_track_res_phi  = TH1F_LW::create("Track_res_phi",  ("Pixel Residual LocX(phi)" + m_histTitleExt).c_str(),100,-0.1,0.1));
   sc=trackHistos.regHist(m_track_res_eta  = TH1F_LW::create("Track_res_eta",  ("Pixel Residual LocY(eta)" + m_histTitleExt).c_str(),100,-0.3,0.3));
   sc=trackHistos.regHist(m_track_pull_phi = TH1F_LW::create("Track_pull_phi", ("Pixel pull LocX(phi)"     + m_histTitleExt).c_str(),100,-1.2,1.2));      
   sc=trackHistos.regHist(m_track_pull_eta = TH1F_LW::create("Track_pull_eta", ("Pixel pull LocY(eta)"     + m_histTitleExt).c_str(),100,-2.0,2.0));   
   sc=trackHistos.regHist(m_track_chi2     = TH1F_LW::create("Track_chi2",     ("chi2 of rec. track"       + m_histTitleExt + ";#chi^{2}/DoF;#tracks").c_str(), 50,-0.,10.));

   sc=trackHistos.regHist(m_tracksPerEvt_per_lumi = TProfile_LW::create("tracksPerEvt_per_lumi", ("Number of tracks per event per LB" + m_histTitleExt + ";lumi block;tracks/event").c_str(), m_lbRange, -0.5,-0.5+(float)m_lbRange));
   //sc=trackHistos.regHist(m_tracksPerEvtPerMu_per_lumi = TProfile_LW::create("tracksPerEvtPerMu_per_lumi", ("Number of tracks per event per mu per LB (pixel tracks)" + m_histTitleExt + ";lumi block;tracks/event").c_str(), m_lbRange, -0.5, -0.5+(float)m_lbRange));
   
   //sc=trackHistos.regHist(m_degFactorMap = TProfile2D_LW::create("degFactorMap", ("degradation factor map for IP resolution" + m_histTitleExt + ";track #eta;track #phi").c_str(), 60, -3.0, 3.0, 80, -4.0, 4.0));
   //m_degFactorMap->SetOption("colz");
   //sc=trackHistos.regHist(m_degFactorMap_per_lumi = TProfile_LW::create("degFactorMap_per_lumi", ("overall degradation factor for IP resolution per lumi" + m_histTitleExt + ";lumi block;overall avg deg. factor").c_str(), m_lbRange, -0.5, -0.5+(float)m_lbRange));
   //sc=trackHistos.regHist(m_degFactorMap_eta_per_lumi = TProfile2D_LW::create("degFactorMap_eta_per_lumi", ("degradation factor (eta) for IP resolution per lumi" + m_histTitleExt + ";lumi block;track #eta").c_str(),  m_lbRange, -0.5, -0.5+(float)m_lbRange, 60, -3.0, 3.0 ));
   //m_degFactorMap_eta_per_lumi->SetOption("colz");
   //sc=trackHistos.regHist(m_degFactorMap_phi_per_lumi = TProfile2D_LW::create("degFactorMap_phi_per_lumi", ("degradation factor (phi) for IP resolution per lumi" + m_histTitleExt + ";lumi block;track #phi").c_str(),  2500, -0.5, 2499.5, 80, -4.0, 4.0 ));
   //m_degFactorMap_phi_per_lumi->SetOption("colz");

   if(m_do2DMaps && !m_doOnline){
      m_tsos_hitmap = new PixelMon2DMapsLW("TSOS_Measurement", ("TSOS of type Measurement" + m_histTitleExt), m_doIBL, false);
      sc = m_tsos_hitmap->regHist(trackHistos, m_doIBL, false);
      m_tsos_holemap = new PixelMon2DMapsLW("TSOS_Hole", ("TSOS of type Hole" + m_histTitleExt), m_doIBL, false);
      sc = m_tsos_holemap->regHist(trackHistos, m_doIBL, false);
      m_tsos_outliermap = new PixelMon2DMapsLW("TSOS_Outlier", ("TSOS of type Outlier" + m_histTitleExt), m_doIBL, false);
      sc = m_tsos_outliermap->regHist(trackHistos, m_doIBL, false);

      //m_tsos_measratio = new PixelMon2DProfilesLW("TSOS_MeasRatio", ("TSOS of type Meas per track" + m_histTitleExt), m_doIBL, false);
      //sc = m_tsos_measratio->regHist(trackHistos, m_doIBL, false);
      //m_tsos_holeratio = new PixelMon2DProfilesLW("TSOS_HoleRatio", ("TSOS of type Hole per track" + m_histTitleExt), m_doIBL, false);
      //sc = m_tsos_holeratio->regHist(trackHistos, m_doIBL, false);
      m_misshits_ratio = new PixelMon2DProfilesLW("MissHitsRatioOnTrack", ("Hole+Outlier per track" + m_histTitleExt), m_doIBL, false);
      sc = m_misshits_ratio->regHist(trackHistos, m_doIBL, false);
   }
   if(m_doOnline){
      m_tsos_holeratio_tmp = new PixelMon2DProfilesLW("HoleRatio_tmp", ("TSOS of type Hole per track tmp" + m_histTitleExt), m_doIBL, false);
      sc = m_tsos_holeratio_tmp->regHist(trackHistos, m_doIBL, false);
      m_tsos_holeratio_mon = new PixelMon2DProfilesLW("HoleRatio_mon", ("TSOS of type Hole per track for monitoring" + m_histTitleExt), m_doIBL, false);
      sc = m_tsos_holeratio_mon->regHist(trackHistos, m_doIBL, false);
      m_misshits_ratio_tmp = new PixelMon2DProfilesLW("MissHitsRatioOnTrack_tmp", ("Hole+Outlier per track" + m_histTitleExt), m_doIBL, false);
      sc = m_misshits_ratio_tmp->regHist(trackHistos, m_doIBL, false);
      m_misshits_ratio_mon = new PixelMon2DProfilesLW("MissHitsRatioOnTrack_mon", ("Hole+Outlier per track for monitoring" + m_histTitleExt), m_doIBL, false);
      sc = m_misshits_ratio_mon->regHist(trackHistos, m_doIBL, false);
   }

   if(m_doModules){
      m_tsos_hiteff_vs_lumi = new PixelMonModulesProf("TSOS_HitEfficiency",("TSOS-based hit efficiency in module" + m_histTitleExt).c_str(),2500,-0.5,2499.5,m_doIBL);
      sc = m_tsos_hiteff_vs_lumi->regHist(this,(path+"/Modules_TSOSHitEff").c_str(),run, m_doIBL);
      if(!m_doOnline){
         sc=trackHistos.regHist(m_clustot_lowpt = TH1F_LW::create("m_clustot_lowpt",("Cluster ToT vs Track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
         sc=trackHistos.regHist(m_clustot_highpt = TH1F_LW::create("m_clustot_highpt",("Cluster ToT vs Track pT (pT>=10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
         sc=trackHistos.regHist(m_1hitclustot_lowpt = TH1F_LW::create("m_1hitclustot_lowpt",("1-Hit cluster ToT vs track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
         sc=trackHistos.regHist(m_1hitclustot_highpt = TH1F_LW::create("m_1hitclustot_highpt",("1-Hit cluster ToT vs track pT (pT>=10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
         sc=trackHistos.regHist(m_2hitclustot_lowpt = TH1F_LW::create("m_2hitclustot_lowpt",("2-Hit cluster ToT vs track pT (pT<10GeV)" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),250,0,250));
         sc=trackHistos.regHist(m_2hitclustot_highpt = TH1F_LW::create("m_2hitclustot_highpt",("2-Hit cluster ToT vs track pT (pT>=10GeV)" + m_histTitleExt + " Track pT; Cluster ToT (on track)").c_str(),250,0,250));
         sc=trackHistos.regHist(m_clustot_vs_pt = TH2F_LW::create("m_clustot_vs_pt",("Cluster ToT vs Track pT" + m_histTitleExt + "; Track pT; Cluster ToT (on track)").c_str(),10,0,50,250,0,250));
         sc=trackHistos.regHist(m_track_chi2_bcl1 = TH1F_LW::create("m_Pixel_track_chi2_bcl1", ("track chi2 with 1 1-hit, low-ToT cluster" + m_histTitleExt + "; #chi^{ 2}/DoF;").c_str(), 50,-0.,10.));
         sc=trackHistos.regHist(m_track_chi2_bcl0 = TH1F_LW::create("m_Pixel_track_chi2_bcl0", ("track chi2 with 0 1-hit, low-ToT clusters" + m_histTitleExt + "; #chi^ {2}/DoF;").c_str(), 50,-0.,10.));
         sc=trackHistos.regHist(m_track_chi2_bclgt1 = TH1F_LW::create("m_Pixel_track_chi2_bclgt1", ("track chi2 with >1 1-hit, low-ToT cluster" + m_histTitleExt + "; # chi^{2}/DoF;").c_str(), 50,-0.,10.));
         sc=trackHistos.regHist(m_track_chi2_bcl1_highpt = TH1F_LW::create("m_Pixel_track_chi2_bcl1_highpt", ("track chi2 with 1 1-hit, low-ToT cluster (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
         sc=trackHistos.regHist(m_track_chi2_bcl0_highpt = TH1F_LW::create("m_Pixel_track_chi2_bcl0_highpt", ("track chi2 with 0 1-hit, low-ToT clusters (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
         sc=trackHistos.regHist(m_track_chi2_bclgt1_highpt = TH1F_LW::create("m_Pixel_track_chi2_bclgt1_highpt", ("track chi2 with >1 1-hit, low-ToT cluster (pT>=10GeV)" + m_histTitleExt + "; #chi^{2}/DoF;").c_str(), 50,-0.,10.));
      }
   }

   for(int i=0; i<PixLayerDBM::COUNT; i++){
      hname = makeHistname(("HitEff_all_"+modlabel2[i]), false);
      htitles = makeHisttitle(("hit efficiency, "+modlabel2[i]), ";lumi block;hit efficiency", false);
      sc = trackHistos.regHist(m_hiteff_incl_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), m_lbRange, -0.5, -0.5+(float)m_lbRange));

      hname = makeHistname(("HitEff_actv_"+modlabel2[i]), false);
      htitles = makeHisttitle(("hit efficiency for active modules, "+modlabel2[i]), ";lumi block;hit efficiency", false);
      sc = trackHistos.regHist(m_hiteff_actv_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), m_lbRange, -0.5, -0.5+(float)m_lbRange));
      
      /// clusters
      //hname = makeHistname(("ClusterSize_Measurement_"+modlabel[i]), false);
      //htitles = makeHisttitle(("cluster size  for clusters on tracks, "+modlabel[i]), ";cluster size;#clusters", false);
      //sc = trackHistos.regHist(m_clusize_measurement_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5+300.0));
      //hname = makeHistname(("ClusterSize_Outlier_"+modlabel[i]), false);
      //htitles = makeHisttitle(("cluster size  for outliers , "+modlabel[i]), ";cluster size;#clusters", false);
      //sc = trackHistos.regHist(m_clusize_outlier_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5+300.0));
   }


   hname = makeHistname("LorentzAngle_IBL", false);
   htitles = makeHisttitle("Lorentz angle IBL", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
   sc = trackHistos.regHist(m_LorentzAngle_IBL = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 14, -0.5, 13.5));
   hname = makeHistname("LorentzAngle_IBL2D", false);
   htitles = makeHisttitle("Lorentz angle IBL2D", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
   sc = trackHistos.regHist(m_LorentzAngle_IBL2D = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 14, -0.5, 13.5));
   hname = makeHistname("LorentzAngle_IBL3D", false);
   htitles = makeHisttitle("Lorentz angle IBL3D", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
   sc = trackHistos.regHist(m_LorentzAngle_IBL3D = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 14, -0.5, 13.5));
   hname = makeHistname("LorentzAngle_B0", false);
   htitles = makeHisttitle("Lorentz angle B0", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
   sc = trackHistos.regHist(m_LorentzAngle_B0 = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 22, -0.5, 21.5));
   hname = makeHistname("LorentzAngle_B1", false);
   htitles = makeHisttitle("Lorentz angle B1", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
   sc = trackHistos.regHist(m_LorentzAngle_B1 = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 38, -0.5, 37.5));
   hname = makeHistname("LorentzAngle_B2", false);
   htitles = makeHisttitle("Lorentz angle B2", ";#phi of track incidence [rad];phi module index;cluster row (phi) width [pixels]", false);
   sc = trackHistos.regHist(m_LorentzAngle_B2 = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 100, -0.4, 0.6, 52, -0.5, 51.5));

   if(m_LorentzAngle_IBL)   m_LorentzAngle_IBL->SetOption("colz");
   if(m_LorentzAngle_IBL2D) m_LorentzAngle_IBL2D->SetOption("colz");
   if(m_LorentzAngle_IBL3D) m_LorentzAngle_IBL3D->SetOption("colz");
   if(m_LorentzAngle_B0)    m_LorentzAngle_B0->SetOption("colz");
   if(m_LorentzAngle_B1)    m_LorentzAngle_B1->SetOption("colz");
   if(m_LorentzAngle_B2)    m_LorentzAngle_B2->SetOption("colz");

   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endmsg;   
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::FillTrackMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "MainFillTrackMon" << endmsg;  

   ///
   /// Retrieve and Check Store Gate Error
   ///
   StatusCode sc;
   sc = evtStore()->retrieve(m_tracks, m_TracksName);
   if (sc.isFailure())
   {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  <<"No tracks in StoreGate found"<< endmsg;
      if(m_storegate_errors) m_storegate_errors->Fill(4.,3.);  
      return StatusCode::SUCCESS;
   } else{
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  <<"Tracks in StoreGate found" <<endmsg;
   }

   m_ntracksPerEvent = 0;
   //int nTracks=0;
   int nPixelHits=0;
   
   ///
   /// Clear
   ///
   if(m_doOnTrack || m_doOnPixelTrack)m_RDOIDs.clear();//reset these so you can fill them with the new id's
   if(m_doOnTrack || m_doOnPixelTrack)m_ClusterIDs.clear();

   
   ///
   /// Track Loop
   ///
   for (int i=0; i<(int)m_tracks->size(); i++) 
   {
      const Trk::Track *track0=(*m_tracks)[i];
      if (track0 == 0) {
         if (msgLvl(MSG::ERROR) ) msg(MSG::ERROR) << "no pointer to track!!!" << endmsg; break;
      }


      //double mass = 0;
      //double dedx = 0;
      //float degradationFactor = 1.0;
      unsigned int hitslayer = 0;
      //bool isEndcaps = false;
      int nholes=-1;
      int nbadclus=0;
      int ngoodclus=0;
      bool passQualityCut = false;
      bool passTightCut = false;
      
      const Trk::TrackParameters *trkParameters = 0;
      const Trk::TrackSummary* summary = track0->trackSummary();
      const Trk::Perigee *measPerigee = dynamic_cast< const Trk::Perigee *>(track0->perigeeParameters());

      /// Check the track summary if it exists to see if there are pixel hits on this track.
      /// if no hits, skip ahead
      if(summary){
      	if (summary->get(Trk::numberOfPixelHits)==0) continue;
      } else {
      	if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "No Track Summary Found" << endmsg; continue;
      }

      const Trk::Track* track = track0;
      
      /// get the track state on surfaces (a vector, on element per surface) and loop over it
	   nholes = summary->get(Trk::numberOfPixelHoles);
      if(m_doHoleSearch && !m_doOnline && nholes>0){
	      track = m_holeSearchTool->getTrackWithHoles(*track0);
      }

      ///
      /// Track Quality Cut
      ///

      if( measPerigee->pT()/1000.0 > 5.0 && fabs(measPerigee->eta()) < 2.5) passQualityCut = true;

      if( ((fabs(measPerigee->eta()) <= 1.65 && summary->get(Trk::numberOfPixelHits)+summary->get(Trk::numberOfSCTHits) >= 9) ||
          (fabs(measPerigee->eta()) >  1.65 && summary->get(Trk::numberOfPixelHits)+summary->get(Trk::numberOfSCTHits) >= 11) ) &&
          (summary->get(Trk::numberOfNextToInnermostPixelLayerHits)+summary->get(Trk::numberOfInnermostPixelLayerHits ) > 0 ) &&
          (summary->get(Trk::numberOfPixelHoles) == 0 ) && 
          (fabs(measPerigee->parameters()[Trk::d0]) < 2.0) && 
          (fabs(measPerigee->parameters()[Trk::z0]) < 150.0) ){
         passTightCut = true;
      }

      ///
      /// TSOS Loop
      ///

      const DataVector< const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();
      for(DataVector< const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurfaceIterator=trackStates->begin(); trackStateOnSurfaceIterator!=trackStates->end(); trackStateOnSurfaceIterator++)
      {
         /// Change the track state on 1 surface into the cluster it represents
         Identifier clusID;
         Identifier surfaceID;
         IdentifierHash id_hash;
         const InDet::SiClusterOnTrack *clus=0;
         const InDetDD::SiDetectorElement *side = 0;
         const Trk::MeasurementBase* mesb=(*trackStateOnSurfaceIterator)->measurementOnTrack();
	 
         const Trk::RIO_OnTrack* hit = mesb ? dynamic_cast<const Trk::RIO_OnTrack*>(mesb) : 0;
        
         //float nMeasurement = 0.;
         float nOutlier = 0.;
         float nHole = 0.;
         float npixHitsInCluster = 0;
         //float colWidthOfCluster = 0;
         float rowWidthOfCluster = 0;
         float totalToTOfCluster = 0;
         bool  passClusterSelection = false;

         ///
         /// Requiements
         ///
         if(mesb && !hit) continue;  // skip pseudomeasurements etc.                                         
         if(mesb && mesb->associatedSurface().associatedDetectorElement()) {
            surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
            side = dynamic_cast<const InDetDD::SiDetectorElement *>( mesb->associatedSurface().associatedDetectorElement() );
         }else{ // holes, perigee                                                                              
            if(not (*trackStateOnSurfaceIterator)->trackParameters() ) {
               msg(MSG::INFO) << "pointer of TSOS to track parameters or associated surface is null" << endmsg;
               continue;
            }
            surfaceID = (*trackStateOnSurfaceIterator)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();//check ptr
         }

         if( !m_idHelper->is_pixel(surfaceID)) continue;

         int pixlayer = GetPixLayerID(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID), m_doIBL);
         int pixlayerdbm = GetPixLayerIDDBM(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID), m_doIBL);
         int pixlayeribl2d3d = pixlayer;
         if( pixlayeribl2d3d == PixLayer::kIBL ){
            pixlayeribl2d3d = GetPixLayerIDIBL2D3D(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID), m_pixelid->eta_module(surfaceID), m_doIBL);
         }
         if( pixlayer == 99) continue;
         
         id_hash = m_pixelid->wafer_hash(surfaceID);
         //if( pixlayer == PixLayer::kECA || pixlayer == PixLayer::kECC ) isEndcaps = true;
         bool active = m_pixelCondSummarySvc->isActive(id_hash);


         if((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)){
           clus = dynamic_cast< const InDet::SiClusterOnTrack*>(mesb);
           if(clus) clusID = clus->identify();
           //nMeasurement = 1.0;

           if( m_tsos_hitmap ) m_tsos_hitmap->Fill(surfaceID,m_pixelid,m_doIBL,false);
           if( m_tsos_hiteff_vs_lumi ) m_tsos_hiteff_vs_lumi->Fill(m_manager->lumiBlockNumber(),1.,surfaceID,m_pixelid,m_doIBL);
           if( m_hiteff_incl_mod[pixlayerdbm] && passQualityCut ) m_hiteff_incl_mod[pixlayerdbm]->Fill( m_manager->lumiBlockNumber(), 1.0 );
           if( m_hiteff_actv_mod[pixlayerdbm] && passQualityCut && active) m_hiteff_actv_mod[pixlayerdbm]->Fill( m_manager->lumiBlockNumber(), 1.0 ); 
           /////
           ///// Study for Quick Status
           /////
           //int fephi=0; int feeta=0;
           //if( pixlayer == PixLayer::kB0 && GetFEID( pixlayer, m_pixelid->phi_index(surfaceID), m_pixelid->eta_index(surfaceID), fephi, feeta) ){
           //   if(passQualityCut &&
           //      m_pixelid->phi_module(surfaceID) == 0 &&
           //      m_pixelid->eta_module(surfaceID) == -6){
           //      if(m_hiteff_incl_L0_B11_S2_C6) m_hiteff_incl_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 1.0 );
           //      if(m_hiteff_isgood_L0_B11_S2_C6 && m_pixelCondSummarySvc->isGood(id_hash) == true ) m_hiteff_isgood_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 1.0 );
           //      if(m_holeRatio_incl_L0_B11_S2_C6) m_holeRatio_incl_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 0.0 );
           //      if(m_holeRatio_isgood_L0_B11_S2_C6 && m_pixelCondSummarySvc->isGood(id_hash) == true) m_holeRatio_isgood_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 0.0 );
           //   }
           //}
         }
         
         if((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Outlier)){
           clus = dynamic_cast< const InDet::SiClusterOnTrack*>((*trackStateOnSurfaceIterator)->measurementOnTrack());
           if(clus) clusID = clus->identify();
           nOutlier = 1.0;

           if( m_tsos_holemap ) m_tsos_holemap->Fill(surfaceID,m_pixelid,m_doIBL,false);
           if( m_tsos_hiteff_vs_lumi ) m_tsos_hiteff_vs_lumi->Fill(m_manager->lumiBlockNumber(),0.,surfaceID,m_pixelid,m_doIBL);
           if( m_hiteff_incl_mod[pixlayerdbm] && passQualityCut ) m_hiteff_incl_mod[pixlayerdbm]->Fill( m_manager->lumiBlockNumber(), 0.0 );
           if( m_hiteff_actv_mod[pixlayerdbm] && passQualityCut && active) m_hiteff_actv_mod[pixlayerdbm]->Fill( m_manager->lumiBlockNumber(), 0.0 );
           /////
           ///// Study for Quick Status
           /////
           //int fephi=0; int feeta=0;
           //if( pixlayer == PixLayer::kB0 && GetFEID( pixlayer, m_pixelid->phi_index(surfaceID), m_pixelid->eta_index(surfaceID), fephi, feeta) ){
           //   if(passQualityCut &&
           //      m_pixelid->phi_module(surfaceID) == 0 &&
           //      m_pixelid->eta_module(surfaceID) == -6){
           //      if(m_hiteff_incl_L0_B11_S2_C6) m_hiteff_incl_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID))+(1.0*feeta), 0.0 );
           //      if(m_hiteff_isgood_L0_B11_S2_C6 && m_pixelCondSummarySvc->isGood(id_hash) == true ) m_hiteff_isgood_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 0.0 );
           //      if(m_holeRatio_incl_L0_B11_S2_C6) m_holeRatio_incl_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 0.0 );
           //      if(m_holeRatio_isgood_L0_B11_S2_C6 && m_pixelCondSummarySvc->isGood(id_hash) == true) m_holeRatio_isgood_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 0.0 );
           //   }
           //}
         }
          
         if((*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Hole)){
           clus = dynamic_cast< const InDet::SiClusterOnTrack*>((*trackStateOnSurfaceIterator)->measurementOnTrack());
           if(clus) clusID = clus->identify();
           nHole = 1.0;

           if( m_tsos_outliermap)m_tsos_outliermap->Fill(surfaceID,m_pixelid,m_doIBL,false);
           if( m_tsos_hiteff_vs_lumi) m_tsos_hiteff_vs_lumi->Fill(m_manager->lumiBlockNumber(),0.,surfaceID,m_pixelid,m_doIBL);
           if( m_hiteff_incl_mod[pixlayerdbm] && passQualityCut ) m_hiteff_incl_mod[pixlayerdbm]->Fill( m_manager->lumiBlockNumber(), 0.0 );
           if( m_hiteff_actv_mod[pixlayerdbm] && passQualityCut && active) m_hiteff_actv_mod[pixlayerdbm]->Fill( m_manager->lumiBlockNumber(), 0.0 );
           /////
           ///// Study for Quick Status
           /////
           //int fephi=0; int feeta=0;
           //if( pixlayer == PixLayer::kB0 && GetFEID( pixlayer, m_pixelid->phi_index(surfaceID), m_pixelid->eta_index(surfaceID), fephi, feeta) ){
           //   if(passQualityCut &&
           //      m_pixelid->phi_module(surfaceID) == 0 &&
           //      m_pixelid->eta_module(surfaceID) == -6){
           //      if(m_hiteff_incl_L0_B11_S2_C6) m_hiteff_incl_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID))+(1.0*feeta), 0.0 );
           //      if(m_hiteff_isgood_L0_B11_S2_C6 && m_pixelCondSummarySvc->isGood(id_hash) == true ) m_hiteff_isgood_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 0.0 );
           //      if(m_holeRatio_incl_L0_B11_S2_C6) m_holeRatio_incl_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 1.0 );
           //      if(m_holeRatio_isgood_L0_B11_S2_C6 && m_pixelCondSummarySvc->isGood(id_hash) == true) m_holeRatio_isgood_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (8.0*m_pixelid->eta_module(surfaceID)+(1.0*feeta)), 1.0 );
           //   }
           //}
         }


         //if(m_tsos_measratio && passQualityCut) m_tsos_measratio->Fill(surfaceID,m_pixelid,m_doIBL,false,nMeasurement);
         //if(m_tsos_holeratio && passQualityCut) m_tsos_holeratio->Fill(surfaceID,m_pixelid,m_doIBL,false,nHole);
         if(m_doOnline && m_tsos_holeratio_tmp && passQualityCut) m_tsos_holeratio_tmp->Fill(surfaceID,m_pixelid,m_doIBL,false,nHole);
         if(passQualityCut){
           if(nOutlier + nHole > 0.){
             if(m_misshits_ratio) m_misshits_ratio->Fill(surfaceID,m_pixelid,m_doIBL,false,1.0);
             if(m_doOnline && m_misshits_ratio_tmp) m_misshits_ratio_tmp->Fill(surfaceID,m_pixelid,m_doIBL,false,1.0);
           }else{
             if(m_misshits_ratio) m_misshits_ratio->Fill(surfaceID,m_pixelid,m_doIBL,false,0.0);
             if(m_doOnline && m_misshits_ratio_tmp) m_misshits_ratio_tmp->Fill(surfaceID,m_pixelid,m_doIBL,false,0.0);
           }
         }

         ///
         /// PixelClusters are valid
         ///
         if(!(*trackStateOnSurfaceIterator)->type(Trk::TrackStateOnSurface::Measurement)){continue;}
         if(!clus)continue;
         const InDet::SiCluster *RawDataClus = dynamic_cast< const InDet::SiCluster*>(clus->prepRawData());
         if(!RawDataClus)continue;
         if(!RawDataClus->detectorElement()->isPixel()) continue;
 
	      nPixelHits++;//add another pixel hit 
	      for(unsigned int loopSize=0;loopSize < RawDataClus->rdoList().size(); loopSize++) {
	        if(m_doOnTrack || m_doOnPixelTrack) m_RDOIDs.push_back(RawDataClus->rdoList().at(loopSize));
	      }
         if(m_doOnTrack || m_doOnPixelTrack) m_ClusterIDs.push_back( clus->identify());

         const InDet::PixelCluster* pixelCluster=dynamic_cast<const InDet::PixelCluster*>(RawDataClus);
         
         if(pixelCluster){
            ///
            /// Pixel Cluster Selection
            ///
            if( !RawDataClus->gangedPixel() && /// not include ganged-pixel
                !pixelCluster->isFake() &&     /// not fake
                ( (pixlayer == PixLayer::kIBL && fabs(clus->localParameters()[Trk::locX])<8.3)
                  || (pixlayer != PixLayer::kIBL && fabs(clus->localParameters()[Trk::locX])<8.1) ) &&
                (    (pixlayeribl2d3d == PixLayerIBL2D3D::kIBL2D && fabs(clus->localParameters()[Trk::locY])<19.7)
                  || (pixlayeribl2d3d == PixLayerIBL2D3D::kIBL3D && fabs(clus->localParameters()[Trk::locY])<9.5) 
                  || (pixlayer        != PixLayer::kIBL          && fabs(clus->localParameters()[Trk::locY])<28.7) )
                ){
              passClusterSelection = true;
            }
            ///
            /// Cluster Variables
            ///
            npixHitsInCluster = pixelCluster->rdoList().size();
            //colWidthOfCluster = pixelCluster->width().colRow().y();
            rowWidthOfCluster = pixelCluster->width().colRow().x();
            totalToTOfCluster = pixelCluster->totalToT();
            
            if( npixHitsInCluster == 1 && totalToTOfCluster < 8) { nbadclus++; }
            else{ ngoodclus++; }
      	    
            if(m_doModules && !m_doOnline){
	           if(measPerigee){
	             float pt = measPerigee->pT()/1000.0;
	             if(m_clustot_vs_pt) m_clustot_vs_pt->Fill(pt, totalToTOfCluster);
	             if(pt<10){
	               if(m_clustot_lowpt) m_clustot_lowpt->Fill(totalToTOfCluster);
	               if(npixHitsInCluster==1){ if(m_1hitclustot_lowpt) m_1hitclustot_lowpt->Fill(totalToTOfCluster);}
	               if(npixHitsInCluster==2){ if(m_2hitclustot_lowpt) m_2hitclustot_lowpt->Fill(totalToTOfCluster);}
	             }else{
	               if(m_clustot_highpt) m_clustot_highpt->Fill(totalToTOfCluster);
	               if(npixHitsInCluster==1){ if(m_1hitclustot_highpt) m_1hitclustot_highpt->Fill(totalToTOfCluster);}
	               if(npixHitsInCluster==2){ if(m_2hitclustot_highpt) m_2hitclustot_highpt->Fill(totalToTOfCluster);}
	             }
	           }
            }
            ///
            /// Hole module map
            ///

            ///
            /// Categorize tracks for IP resolution degradation
            ///
            if(pixlayer >= PixLayer::kECC){
               unsigned int tmplayer = 0x1<<pixlayer;
               if( hitslayer != tmplayer ) hitslayer += 0x1<<pixlayer;
            }
	      }

	    
	      ///
         /// Get track parameters for current surface (with AtaPlane)
         ///
         trkParameters = (*trackStateOnSurfaceIterator)->trackParameters();
         const Trk::AtaPlane *trackAtPlane = dynamic_cast<const Trk::AtaPlane*>(trkParameters);
         if(trackAtPlane)
         {
            const Amg::Vector2D localpos = trackAtPlane->localPosition();

            // Get local error matrix for hit and track and calc pull
            const AmgSymMatrix(5) trackErrMat = (*trackAtPlane->covariance());
            const Amg::MatrixX clusErrMat = clus->localCovariance();

            //pulls and residuals Phi
            double error_sum = sqrt(pow(Amg::error(trackErrMat,Trk::locX),2) + pow(Amg::error(clusErrMat,Trk::locX), 2));
            double res = clus->localParameters()[Trk::locX]-localpos[0];
            if(m_track_res_phi) m_track_res_phi->Fill(res);
            double pull = 0;
            if(error_sum != 0) pull = (res)/error_sum;
            if(m_track_pull_phi) m_track_pull_phi->Fill(pull);

            //pulls and residuals Eta
            error_sum = sqrt(pow(Amg::error(trackErrMat,Trk::locY),2) + pow(Amg::error(clusErrMat,Trk::locY), 2));
            res = clus->localParameters()[Trk::locY]-localpos[1];
            if(m_track_res_eta) m_track_res_eta->Fill(res);
            if(error_sum != 0) pull = (res)/error_sum;
            if(m_track_pull_eta) m_track_pull_eta->Fill(pull);

            /// LorentzAngle
            Amg::Vector3D mynormal = side->normal();
            Amg::Vector3D myphiax = side->phiAxis();
            Amg::Vector3D mytrack = trackAtPlane->momentum();
            double trkphicomp = mytrack.dot(myphiax);
            double trknormcomp = mytrack.dot(mynormal); 
            double phiIncident =  atan2(trkphicomp,trknormcomp);
            if(npixHitsInCluster > 0 && passTightCut && passClusterSelection){
               if(pixlayer == PixLayer::kIBL && m_LorentzAngle_IBL) m_LorentzAngle_IBL->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0*rowWidthOfCluster);
               if(pixlayeribl2d3d == PixLayerIBL2D3D::kIBL2D && m_LorentzAngle_IBL2D) m_LorentzAngle_IBL2D->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0*rowWidthOfCluster);
               if(pixlayeribl2d3d == PixLayerIBL2D3D::kIBL3D && m_LorentzAngle_IBL3D) m_LorentzAngle_IBL3D->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0*rowWidthOfCluster);
               if(pixlayer == PixLayer::kB0 && m_LorentzAngle_B0) m_LorentzAngle_B0->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0*rowWidthOfCluster);
               if(pixlayer == PixLayer::kB1 && m_LorentzAngle_B1) m_LorentzAngle_B1->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0*rowWidthOfCluster);
               if(pixlayer == PixLayer::kB2 && m_LorentzAngle_B2) m_LorentzAngle_B2->Fill(phiIncident, m_pixelid->phi_module(surfaceID), 1.0*rowWidthOfCluster);
            }
         }
      } // end of TSOS loop
    
      if(!m_doOnline && m_doModules){
	      float pt = measPerigee->pT()/1000.0;
	      if(nbadclus==1){
	         if(m_track_chi2_bcl1 && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bcl1->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      }
	      if(nbadclus==0){
	         if(m_track_chi2_bcl0 && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bcl0->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      } 
	      if(nbadclus>1){
	         if(m_track_chi2_bclgt1 && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bclgt1->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      } 
	      if(pt>=10){
	         if(nbadclus==1){
	            if(m_track_chi2_bcl1_highpt && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bcl1_highpt->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	         }
	         if(nbadclus==0){
	            if(m_track_chi2_bcl1_highpt && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bcl0_highpt->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	         } 
	         if(nbadclus>1){
	            if(m_track_chi2_bclgt1_highpt && track0->fitQuality()->numberDoF() != 0) m_track_chi2_bclgt1_highpt->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	         } 
	      }
      }
            
      if(nPixelHits>0)//track properties for tracks through the pixel detector
      {
         //const  Trk::Perigee *measPerigee = dynamic_cast< const Trk::Perigee *>(track0->perigeeParameters());
         //if (measPerigee != NULL){
         //   //m_track_qOverP->Fill((measPerigee->parameters()[Trk::qOverP]*1000.0));
	      //   //if (measPerigee->parameters()[Trk::qOverP] != 0) { 
	      //     //m_track_qOverP_inv->Fill((1./(measPerigee->parameters()[Trk::qOverP]*1000.0))); 
	      //     //m_track_dedx->Fill(1./(measPerigee->parameters()[Trk::qOverP]),dedx);
	      //   //}
         //   //if (std::abs(mass-139.57)>.00001&&mass>0&&dedx>1.8) m_track_mass_dedx->Fill(mass);
         //   //m_track_pt->Fill((measPerigee->pT()/1000.0));
         //   //m_track_d0->Fill(measPerigee->parameters()[Trk::d0]);
         //   //m_track_z0->Fill(measPerigee->parameters()[Trk::z0]);
         //   //m_track_phi0->Fill(measPerigee->parameters()[Trk::phi0]);
         //   //m_track_theta->Fill(measPerigee->parameters()[Trk::theta]);
         //   //m_track_eta->Fill(measPerigee->eta());
         //   if (track0->fitQuality()->numberDoF() > 0){ 
	      //      if(m_track_chi2) m_track_chi2->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      //      if(m_track_chi2_LB) m_track_chi2_LB->Fill(track0->fitQuality()->chiSquared()/track0->fitQuality()->numberDoF());
	      //   }else{ 
	      //      if(m_track_chi2) m_track_chi2->Fill(-1);
	      //      if(m_track_chi2_LB) m_track_chi2_LB->Fill(-1);
	      //   }	 
	      //   if(m_tracks_per_lumi) m_tracks_per_lumi->Fill(m_manager->lumiBlockNumber());
	         //nTracks++;
            m_ntracksPerEvent++;
	      //}
      }

      /// IP degradation factor map
      if( m_doDegFactorMap ){
         if (measPerigee){

            //double eta = measPerigee->eta();
            //double phi = measPerigee->parameters()[Trk::phi0];
            //double pt = measPerigee->pT();

            //if(isEndcaps){
            //   degradationFactor = 1.0;
            //}else{
            //   unsigned int alllayers =  0x1<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
            //   unsigned int IBL       =  0x0<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
            //   unsigned int B0        =  0x1<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
            //   unsigned int B1        =  0x1<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x0<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
            //   unsigned int B2        =  0x1<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
            //   unsigned int IBLB0     =  0x0<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
            //   unsigned int IBLB2     =  0x0<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
            //   unsigned int B0B2      =  0x1<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x1<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
            //   unsigned int B1B2      =  0x1<<PixLayer::kIBL | 0x1<<PixLayer::kB0 | 0x0<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
            //   unsigned int IBLB0B1   =  0x0<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x0<<PixLayer::kB1 | 0x1<<PixLayer::kB2;
            //   unsigned int IBLB0B1B2 =  0x0<<PixLayer::kIBL | 0x0<<PixLayer::kB0 | 0x0<<PixLayer::kB1 | 0x0<<PixLayer::kB2;
            //   if( hitslayer == alllayers ) degradationFactor = 1.0; // All hits
            //   else{
            //      if( hitslayer == IBL ) degradationFactor = 1.8; // IBL
            //      else if( hitslayer == B0 ) degradationFactor = 1.16; // B0
            //      else if( hitslayer == B1 ) degradationFactor = 1.0; // B1
            //      else if( hitslayer == B2 ) degradationFactor = 1.0; // B2
            //      else if( hitslayer == IBLB0 ) degradationFactor = 3.5; // IBL & B0
            //      else if( hitslayer == IBLB2 ) degradationFactor = 1.8; // IBL & B2
            //      else if( hitslayer == B0B2 ) degradationFactor = 1.17; // B & B2
            //      else if( hitslayer == B1B2 ) degradationFactor = 1.0;  // B1 & B2
            //      else if( hitslayer == IBLB0B1 ) degradationFactor = 6.0;  // IBL & B0 & B1
            //      else if( hitslayer == IBLB0B1B2 ) degradationFactor = 6.0;  // IBL & B0 & B1
            //      else degradationFactor = 1.0;
            //   }
            //   //std::cout << hitslayer << " " << tests << " " << degradationFactor << std::endl;
            //}

            //if( fabs(eta) < 2.5 && pt > 400.0 ){ // requirement of tracks
            //   if(m_degFactorMap) m_degFactorMap->Fill( eta, phi, degradationFactor);
            //   if(m_degFactorMap_per_lumi) m_degFactorMap_per_lumi->Fill(m_manager->lumiBlockNumber(), degradationFactor);
            //   if(m_degFactorMap_eta_per_lumi) m_degFactorMap_eta_per_lumi->Fill(m_manager->lumiBlockNumber(), eta, degradationFactor);
            //   //if(m_degFactorMap_phi_per_lumi) m_degFactorMap_phi_per_lumi->Fill(m_manager->lumiBlockNumber(), measPerigee->parameters()[Trk::phi0], degradationFactor);
            //}
         }
      }

      if(m_doHoleSearch && !m_doOnline && nholes>0){delete (track);}
   } // end of track loop

   
   ///
   /// Fill histograms per event
   ///
    
   if(m_tracksPerEvt_per_lumi) m_tracksPerEvt_per_lumi->Fill(m_manager->lumiBlockNumber(), m_ntracksPerEvent);
   //if(m_tracksPerEvtPerMu_per_lumi && m_lumiTool->lbAverageInteractionsPerCrossing() > 0 ){
   //   m_tracksPerEvtPerMu_per_lumi->Fill(m_manager->lumiBlockNumber(),nTracks/m_lumiTool->lbAverageInteractionsPerCrossing());
   //}
   if(m_doOnTrack || m_doOnPixelTrack)sort (m_RDOIDs.begin(), m_RDOIDs.end());
   if(m_doOnTrack || m_doOnPixelTrack)sort (m_ClusterIDs.begin(), m_ClusterIDs.end());

   if(m_doOnline){
      if(m_doRefresh5min) {
         if(m_tsos_holeratio_mon && m_tsos_holeratio_tmp) m_tsos_holeratio_mon->Fill2DMon(m_tsos_holeratio_tmp);
         if(m_misshits_ratio_mon && m_misshits_ratio_tmp) m_misshits_ratio_mon->Fill2DMon(m_misshits_ratio_tmp);
      }
   }//end of doOnline loop processing


   return StatusCode::SUCCESS;
}      

StatusCode PixelMainMon::ProcTrackMon(void)
{
  double lengthLB = 0;
  lengthLB = m_LBendTime - m_LBstartTime;
  if (lengthLB <= 0) {
    if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Luminosity block has length <= 0 sec, cannot calculate track rate." << endmsg;  
    return StatusCode::SUCCESS; //if LB length is zero, the rest is pointless and would divide by 0
  }
 
  //if(m_trackRate_per_lumi) {
  //  if(m_tracks_per_lumi) double ntrkLB = m_tracks_per_lumi->GetBinContent(m_manager->lumiBlockNumber()+1); //value LB number corresponds to bin nbr (m_lumiBlockNum+1)
  //  double trackRateLB = (double) ntrkLB/lengthLB;
  //  m_trackRate_per_lumi->Fill(m_manager->lumiBlockNumber(),trackRateLB);
  //}
  //for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
  for(int i=0; i<PixLayerDBM::COUNT; i++){
     if(m_hiteff_incl_mod[i]) m_hiteff_incl_mod[i]->SetMinimum(0.8);
     if(m_hiteff_incl_mod[i]) m_hiteff_incl_mod[i]->SetMaximum(1.01);
     if(m_hiteff_actv_mod[i]) m_hiteff_actv_mod[i]->SetMinimum(0.8);
     if(m_hiteff_actv_mod[i]) m_hiteff_actv_mod[i]->SetMaximum(1.01);
  }
  return StatusCode::SUCCESS;
  
}



