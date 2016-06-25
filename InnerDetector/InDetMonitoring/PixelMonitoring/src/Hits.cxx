/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms showing pixel hit level information
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
//#include "TrkSpacePoint/SpacePointContainer.h"
//#include "InDetPrepRawData/PixelClusterContainer.h"         
//#include "TrkParameters/MeasuredAtaPlane.h"
//#include "TrkParameters/MeasuredPerigee.h"
//#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
//#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "TH1F.h"   
#include "TH1I.h"   
#include "TH2F.h"
#include "TH2I.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "TProfile.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include <sstream>

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/DBMMon2DMaps.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "PixelMonitoring/PixelMon2DLumiProfiles.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
#include "PixelCabling/IPixelCablingSvc.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::BookHitsMon(void)
{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Hits" << endreq;  
 
   std::string path = "Pixel/Hits";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/HitsOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/HitsOnPixelTrack");
   MonGroup rdoShift(   this, path.c_str(),  run, ATTRIB_MANAGED ); //declare a group of histograms
   MonGroup rdoExpert ( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms

   std::string pathT = "Pixel/Timing";
   if(m_doOnTrack) pathT.replace(pathT.begin(), pathT.end(), "Pixel/TimingOnTrack");
   if(m_doOnPixelTrack) pathT.replace(pathT.begin(), pathT.end(), "Pixel/TimingOnPixelTrack");
   MonGroup timeShift(  this, pathT.c_str(),  run, ATTRIB_MANAGED ); //declare a group of histograms
   MonGroup timeExpert( this, pathT.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms
   
   std::string modlabel[9];
   modlabel[0]="ECA"; modlabel[1]="ECC";
   modlabel[2]="B0";  modlabel[3]="B1";  modlabel[4]="B2"; 
   modlabel[5]="IBL"; modlabel[6]="IBL2D"; modlabel[7]="IBL3D"; 
   std::string modlabel2[10];
   modlabel2[0]="ECA"; modlabel2[1]="ECC";
   modlabel2[2]="B0";  modlabel2[3]="B1";  modlabel2[4]="B2"; 
   modlabel2[5]="DBMA";modlabel2[6]="DBMC";
   modlabel2[7]="IBL"; modlabel2[8]="IBL2D"; modlabel2[9]="IBL3D"; 
   std::string tmp;
   std::string tmp2;
   std::string atitles; 
   std::string hname;
   std::string htitles;
   
   int nbins_LB = m_lbRange;     double min_LB  = -0.5;   double max_LB = min_LB + (1.0*nbins_LB);        
   int nbins_BCID = m_bcidRange; double min_BCID  = -0.5; double max_BCID = min_BCID + (1.0*nbins_BCID);        
   int nbins_tot3 = 300; double min_tot3 = -0.5; double max_tot3 = min_tot3 + (1.0*nbins_tot3);
   int nbins_tot4 = 20;  double min_tot4 = -0.5; double max_tot4 = min_tot4 + (1.0*nbins_tot4);

   static constexpr int nmod_phi[PixLayer::COUNT] = {48, 48, 22, 38, 52, 14};
   static constexpr int nmod_eta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 20};

   std::string atext_LB = ";lumi block"; 
   std::string atext_BCID = ";BCID"; 
   std::string atext_nevt = ";# events"; 
   std::string atext_nhit = ";# hits"; 
   std::string atext_hit = ";# hits/event"; 
   std::string atext_occ = ";# hits/pixel/event"; 
   std::string atext_tot = ";ToT [BC]"; 
   std::string atext_lv1 = ";Level 1 Accept"; 
  
   hname = makeHistname("Interactions_vs_lumi", false);
   htitles = makeHisttitle("<Interactions> vs LB", (atext_LB+";<#Interactions/event>"), false);
   sc = rdoExpert.regHist(m_mu_vs_lumi = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("Events_per_lumi", false);
   htitles = makeHisttitle("Number of events in a LB", (atext_LB+atext_nevt), false);
   sc = rdoShift.regHist(m_events_per_lumi = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("Hits_per_lumi", false);
   htitles = makeHisttitle("Average number of pixel hits per event", (atext_LB+atext_hit), false);
   sc = rdoShift.regHist(m_hits_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("AvgOcc_per_lumi", false);
   htitles = makeHisttitle("Average pixel occupancy per event over all layers", (atext_LB+atext_occ), false);
   sc = rdoShift.regHist(m_avgocc_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("AvgOcc_RatioIBLB0_per_lumi", false);
   htitles = makeHisttitle("Average pixel occupancy ratio of IBL/B0 per event per LB", (atext_LB+";ratio"), false);
   sc = rdoShift.regHist(m_avgocc_ratioIBLB0_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){ // not include IBL2D and IBL3D
      hname = makeHistname(("Hits_per_lumi_"+modlabel[i]), false);
      htitles = makeHisttitle(("Average number of pixel hits per event, "+modlabel[i]), (atext_LB+atext_hit), false);
      sc = rdoExpert.regHist(m_hits_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      hname = makeHistname(("AvgOcc_per_lumi_"+modlabel[i]), false);
      htitles = makeHisttitle(("Average pixel occupancy per event, "+modlabel[i]), (atext_LB+atext_occ), false);
      sc = rdoExpert.regHist(m_avgocc_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      hname = makeHistname(("MaxOcc_per_lumi_"+modlabel[i]), false);
      htitles = makeHisttitle(("Max. pixel occupancy per event, "+modlabel[i]), (atext_LB+atext_occ), false);
      sc = rdoShift.regHist(m_maxocc_per_lumi_mod[i] = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 500, 0.0001, 0.01));

      hname = makeHistname(("nHits_per_module_per_event_"+modlabel[i]), false);
      htitles = makeHisttitle(("Number of hits in a module in an event, "+modlabel[i]), ";#hits in a module in an event;#events #times #modules", false);
      sc = rdoShift.regHist(m_nhits_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 1000, -0.5, -0.5+1000.0));

      hname = makeHistname(("nLargeEvent_per_lumi_"+modlabel[i]), false);
      htitles = makeHisttitle(("Number of large events (hitocc > 0.7#times 10^{-3}), "+modlabel[i]), (atext_LB+atext_nevt), false);
      sc = rdoShift.regHist(m_nlargeevt_per_lumi_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      hname = makeHistname(("AvgOcc_per_BCID_"+modlabel[i]), false);
      htitles = makeHisttitle(("Average pixel occupancy per BCID, "+modlabel[i]), (atext_BCID+atext_occ), false);
      sc = rdoExpert.regHist(m_avgocc_per_bcid_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      hname = makeHistname(("MaxOcc_per_BCID_"+modlabel[i]), false);
      htitles = makeHisttitle(("Max. pixel occupancy per BCID, "+modlabel[i]), (atext_BCID+atext_occ), false);
      sc = rdoShift.regHist(m_maxocc_per_bcid_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      hname = makeHistname(("ToatlHits_per_BCID_"+modlabel[i]), false);
      htitles = makeHisttitle(("Total Number of hits per BCID, "+modlabel[i]), (atext_BCID+";#hits"), false);
      sc = rdoExpert.regHist(m_totalhits_per_bcid_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      hname = makeHistname(("AvgOcc_LBvsBCID_"+modlabel[i]), false);
      htitles = makeHisttitle(("Average pixel occupancy per BCID, "+modlabel[i]), (atext_BCID+atext_LB+atext_occ), false);
      sc = rdoExpert.regHist(m_avgocc_LBvsBCID_mod[i] = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID, nbins_LB/50, min_LB, max_LB));

      hname = makeHistname(("AvgOcc_wSyncMod_per_lumi_"+modlabel[i]), false);
      htitles = makeHisttitle(("Average pixel occupancy for active and (good or sync) mod per event, "+modlabel[i]), (atext_LB+atext_occ), false);
      sc = rdoExpert.regHist(m_avgocc_wSyncMod_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));
   }

   for(int i=0; i<PixLayerIBL2D3DDBM::COUNT; i++){
      hname = makeHistname(("Hit_ToT_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Hit ToT, "+modlabel2[i]), (atext_tot+atext_nhit), false);
      if(i<PixLayerIBL2D3DDBM::kDBMA){
         sc = rdoExpert.regHist(m_hit_ToT[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot3, min_tot3, max_tot3));
      }else if(m_doIBL){
         sc = rdoExpert.regHist(m_hit_ToT[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot4, min_tot4, max_tot4));
      }
   }
   
   if(m_doOnline)
   {
      m_hitmap_mon = new PixelMon2DMaps("HitMap_Mon", ("Hit map for monitoring" + m_histTitleExt).c_str()); sc = m_hitmap_mon->regHist(rdoShift);
      m_hitmap_tmp = new PixelMon2DMaps("HitMap_tmp", ("Hit map for monitoring" + m_histTitleExt).c_str()); sc = m_hitmap_tmp->regHist(rdoShift);
      
      for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         hname = makeHistname(("Hit_ToT_Mon_"+modlabel[i]), false);
         htitles = makeHisttitle(("Hit ToT Monitoring, "+modlabel[i]), (atext_tot+atext_nhit), false);
         if( i<PixLayer::kIBL ){
            sc = rdoExpert.regHist(m_hit_ToT_Mon_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot3, min_tot3, max_tot3));
         }else{
            sc = rdoExpert.regHist(m_hit_ToT_Mon_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot4, min_tot4, max_tot4));
         }

         hname = makeHistname(("Hit_ToT_tmp_"+modlabel[i]), false);
         htitles = makeHisttitle(("Hit ToT tmp, "+modlabel[i]), (atext_tot+atext_nhit), false);
         if( i<PixLayer::kIBL ){
            sc = rdoExpert.regHist(m_hit_ToT_tmp_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot3, min_tot3, max_tot3));
         }else{
            sc = rdoExpert.regHist(m_hit_ToT_tmp_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot4, min_tot4, max_tot4));
         }
      }
   }

   if(m_doModules && !m_doOnline){
      for( int i=0; i<PixLayer::COUNT; i++){
         tmp = "ToT_etaphi"; tmp2 = "Average Hit ToT Map"; atitles = ";Module eta;Module phi;Avg. ToT";
         float xmin = -0.5; float xmax = xmin + (1.0 * nmod_eta[i]);
         float ymin = -0.5; float ymax = ymin + (1.0 * nmod_phi[i]);
         sc= rdoExpert.regHist(m_ToT_etaphi_mod[i] = TProfile2D_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(), nmod_eta[i], xmin, xmax, nmod_phi[i], ymin, ymax));
      }
   }
      
   if(m_doTiming)
   {
      sc = timeExpert.regHist(m_Lvl1ID_PIX     = TH1I_LW::create("LvlID_PIX",  ("Level 1 ID (PIX)" + m_histTitleExt + ";level 1 ID" + atext_nhit).c_str(), 20,-0.5,19.5));
      sc = timeExpert.regHist(m_Lvl1ID_IBL     = TH1I_LW::create("LvlID_IBL",  ("Level 1 ID (IBL)" + m_histTitleExt + ";level 1 ID" + atext_nhit).c_str(), 40,-0.5,39.5));
      sc = timeShift.regHist(m_BCID            = TH1I_LW::create("Pixel_BCID", ("BCID" + m_histTitleExt + ";Pixel BCID" + atext_nhit).c_str(), 300,-0.5,299.5));//2808 bunches but have a few extra bins to check 
      sc = timeShift.regHist(m_Atlas_BCID      = TH1F_LW::create("Atlas_BCID", ("BCID" + m_histTitleExt + ";ATLAS BCID" + atext_nhit).c_str(), 3500,-0.5,3499.5));
      sc = timeShift.regHist(m_Atlas_BCID_hits = TH2F_LW::create("Atlas_BCID_Hits",  ("BCID" + m_histTitleExt + ";BCID" + atext_nhit).c_str(), 3500,-0.5,3499.5,100,0,25000));
      sc = timeExpert.regHist(m_BCID_Profile   = TProfile_LW::create("Atlas_BCID_Profile",  ("BCID_Profile" + m_histTitleExt + ";BCID" + atext_nhit).c_str(), 3500,-0.5,3499.5));  
      sc = timeShift.regHist(m_Lvl1A = TH1F_LW::create("Lvl1A", ("Hit Level 1 Accept" + m_histTitleExt + ";Level 1 Accept" + atext_nhit).c_str(), 14, -1.5, 12.5));

      for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         tmp = "ROD_Module_BCID"; tmp2 = "Difference between BCID of RODs and Modules"; atitles = "; BCID: ROD-Module (#bc) ; Number of Pixels";
         sc = timeExpert.regHist(m_diff_ROD_vs_Module_BCID_mod[i] = TH1I_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(),101,-50.5,50.5));
         tmp = "Lvl1ID_diff_ATLAS_mod"; tmp2 = "ATLAS_{Level 1 ID} - Module_{Level 1 ID}"; atitles = ";#Delta Level 1 ID; # hits";
         sc = timeExpert.regHist(m_Lvl1ID_diff_mod_ATLAS_mod[i] = TH1I_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(), 201, -200.5, 200.5));
         //tmp = "Lvl1A"; tmp2 = "Hit Level 1 Accept"; atitles = ";Level 1 Accept; # hits";
         //sc = timeShift.regHist(m_Lvl1A_mod[i] = TH1F_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(), 14, -1.5, 12.5));
      }

      for(int i=0; i<PixLayerDBM::COUNT-1+(int)(m_doIBL); i++){
         hname = makeHistname(("Lvl1A_"+modlabel2[i]), false);
         htitles = makeHisttitle(("Hit Level 1 Accept, "+modlabel2[i]), (atext_lv1+atext_nhit), false);
         sc = timeShift.regHist(m_Lvl1A_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 14, -1.5, 12.5));
      }
   }

   if(m_do2DMaps)
   {
      m_occupancy = new PixelMon2DMaps("Occupancy", ("hit map"+ m_histTitleExt).c_str());
      sc = m_occupancy->regHist(rdoShift);

      m_average_pixocc = new PixelMon2DMapsLW("Occupancy_per_pixel", ("#hits / pixel" + m_histTitleExt).c_str(), m_doIBL, false);
      sc = m_average_pixocc->regHist(rdoShift, m_doIBL, false);
     
      m_Lvl1ID_diff_mod_ATLAS_per_LB = new PixelMon2DLumiProfiles("Lvl1ID_diff_ATLAS_mod_per_LB", ("ATLAS_{Level 1 ID} - Module_{Level 1 ID} per LB" + m_histTitleExt).c_str(),"#Delta Level 1 ID",m_doIBL,false);
      sc = m_Lvl1ID_diff_mod_ATLAS_per_LB->regHist(timeExpert,m_doIBL,false);

      m_Lvl1ID_absdiff_mod_ATLAS_per_LB = new PixelMon2DLumiProfiles("Lvl1ID_absdiff_ATLAS_mod_per_LB", ("ATLAS_{Level 1 ID} - Module_{Level 1 ID} per LB" + m_histTitleExt).c_str(),"#Delta Level 1 ID",m_doIBL,false);
      sc = m_Lvl1ID_absdiff_mod_ATLAS_per_LB->regHist(timeExpert,m_doIBL,false);
   }

   if(m_doModules)
   {
      m_hit_num_mod = new PixelMonModules1D("Hit_num", ("Number of hits in a module in an event" + m_histTitleExt).c_str(), 15,-0.5,149.5,m_doIBL);
      sc = m_hit_num_mod->regHist(this,(path+"/Modules_NumberOfHits").c_str(),run, m_doIBL);
      m_hiteff_mod = new PixelMonModulesProf("Hit_track_eff", ("Proportion of hits on track" + m_histTitleExt).c_str(), 2500,-0.5,2499.5,m_doIBL);
      sc = m_hiteff_mod->regHist(this,(path+"/Modules_HitEff").c_str(),run, m_doIBL);
   }
   if(!m_doOnline && m_doOnTrack){
      //m_hiteff_mod = new PixelMonModulesProf("HitEff_incl", ("Proportion of hits on track" + m_histTitleExt).c_str(), 2500,-0.5,2499.5,m_doIBL);
      //sc = m_hiteff_mod->regHist(this,(path+"/Modules_HitEff").c_str(),run, m_doIBL);
      //m_hiteff_goodmod = new PixelMonModulesProf("HitEff_active", ("Proportion of hits on track for active modules" + m_histTitleExt).c_str(), 2500,-0.5,2499.5,m_doIBL);
      //sc = m_hiteff_mod->regHist(this,(path+"/Modules_HitEff").c_str(),run, m_doIBL);
   }
   if(m_doModules || m_doFEChipSummary)
   {
      m_FE_chip_hit_summary = new PixelMonModules1D("FE_Chip_Summary", ("FE Chip Summary" + m_histTitleExt).c_str(), 16,-0.5,15.5,m_doIBL);
      sc = m_FE_chip_hit_summary->regHist(this,(path+"/Modules_FEChipSummary").c_str(),run, m_doIBL);
   }
   if(m_doOffline)
   {
      if(m_doLowOccupancy){ }
      if(m_doHighOccupancy){ }
   }
   if(m_doOnline)
   {
      sc = rdoShift.regHist(m_occupancy_time1= new TProfile("occupancy_time_10min", ("Module hit occupancy as function of time over 10 minutes. 6 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));
      sc = rdoShift.regHist(m_occupancy_time2= new TProfile("occupancy_time_1hr",   ("Module hit occupancy as function of time over 1 hour.  36 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));
      sc = rdoShift.regHist(m_occupancy_time3= new TProfile("occupancy_time_6hr",   ("Module hit occupancy as function of time over 6 hours.  3.6 min/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));
      if(m_doLowOccupancy){ }
      if(m_doHighOccupancy){ }
   }
   if(m_doHighOccupancy)
   {
      int max_hits = 80000;
      int max_avhits = 100;
      if (m_doHeavyIonMon) { max_hits = 350000; max_avhits = 2500; }
      sc = rdoShift.regHist(m_num_hits= TH1I_LW::create("num_hits",  ("Number of pixel hits in an event" + m_histTitleExt + ";# pixel hits/event;# events").c_str(), 2500,0.,(int)max_hits));

      for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         hname = makeHistname(("Occupancy_Summary_"+modlabel[i]), false);
         htitles = makeHisttitle(("Average hit occupancy, "+modlabel[i]), ";average # hits in a module in an event;# modules", false);
         sc = rdoExpert.regHist(m_occupancy_summary_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 100,0.,(int)max_avhits));
      }
   }
   if(m_doLowOccupancy)
   {
      sc = rdoShift.regHist(m_num_hits_low= TH1I_LW::create("num_hits_low_occupancy",  ("Number of pixel hits per event" + m_histTitleExt + ";# pixel hits/event;# events").c_str(), 200,-0.5,199.5));
      
      for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         hname = makeHistname(("Occupancy_Summary_Low_"+modlabel[i]), false);
         htitles = makeHisttitle(("Average hit occupancy(low), "+modlabel[i]), ";average # hits in a module in an event;# modules", false);
         sc = rdoExpert.regHist(m_occupancy_summary_low_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 50, 0., 2.));
      }
   }

   if(m_doPixelOccupancy)
   {
     m_pixel_occupancy = new PixelMonModules2D("Pixel_Occupancy", ("Pixel Occupancy" + m_histTitleExt).c_str(), 160, -0.,160.,336,0.,336.,m_doIBL);
     sc = m_pixel_occupancy->regHist(this,(path+"/PixelOccupancy").c_str(),run, m_doIBL);
     // if(m_doNoiseMap)
     //  {
     //    m_pixel_noise_map = new PixelMonModules2D("Pixel_Noise_Map", ("Pixel Noise Map" + m_histTitleExt).c_str(), 160, -0.,160.,336,0.,336.,m_doIBL);
     //    sc = m_pixel_noise_map->regHist(this,(path+"/PixelNoiseMap").c_str(),run, m_doIBL);
     //  }
     //  if(m_doSpectrum)
     //  {
     //     double edge_ini = 1e-9;
     //     double edge[8];
     //     for(int i=0; i<8; i++)
     //     {
     //        edge[i]= edge_ini;
     //        edge_ini=edge_ini*10;
     //     }
     // 	 m_pixel_occupancy_spectrum = new PixelMonModules1D("Pixel_Occupancy_spectrum", ("Pixel Noise Map" + m_histTitleExt).c_str(), 7, edge,m_doIBL);
     //     sc = m_pixel_occupancy_spectrum->regHist(this,(path+"/PixelOccupancySpectrum").c_str(),run, m_doIBL);
     //  }
   }
   if(m_doRodSim)
   {
      sc = rdoExpert.regHist(m_RodSim_BCID_minus_ToT= TH1F_LW::create("RodSim_BCID_minus_ToT",  ("BCID - ToT" + m_histTitleExt).c_str(), 300,-0.5,299.5));
      sc = rdoExpert.regHist(m_RodSim_FrontEnd_minus_Lvl1ID= TH1F_LW::create("RodSim_FrontEnd_minus_Lvl1ID",  ("Front End Chip - Lvl1ID" + m_histTitleExt).c_str(), 33,-16.5,16.5));
   }
   if(m_doDetails)
   {
      sc = rdoExpert.regHist(m_Details_mod1_num_hits = TH1F_LW::create(("Details_num_hits_"+m_DetailsMod1).c_str(),  ("number of pixel hits per event for mod1" + m_histTitleExt).c_str(), 100,-0.,100)); 
      sc = rdoExpert.regHist(m_Details_mod2_num_hits = TH1F_LW::create(("Details_num_hits_"+m_DetailsMod2).c_str(),  ("number of pixel hits per event for mod2" + m_histTitleExt).c_str(), 100,-0.,100));  
      sc = rdoExpert.regHist(m_Details_mod3_num_hits = TH1F_LW::create(("Details_num_hits_"+m_DetailsMod3).c_str(),  ("number of pixel hits per event for mod3" + m_histTitleExt).c_str(), 100,-0.,100));  
      sc = rdoExpert.regHist(m_Details_mod4_num_hits = TH1F_LW::create(("Details_num_hits_"+m_DetailsMod4).c_str(),  ("number of pixel hits per event for mod4" + m_histTitleExt).c_str(), 100,-0.,100));
      //New ranges for 2 FEI4Bs:
      sc = rdoExpert.regHist(m_Details_mod1_occupancy= TH2F_LW::create(("Details_occupancy_"+m_DetailsMod1).c_str(),   ("column vs row hitmap for mod1" + m_histTitleExt).c_str(), 160,-0.5,155.5,336,-0.5,335.5));  
      sc = rdoExpert.regHist(m_Details_mod2_occupancy= TH2F_LW::create(("Details_occupancy_"+m_DetailsMod2).c_str(),   ("column vs row hitmap for mod2" + m_histTitleExt).c_str(), 160,-0.5,155.5,336,-0.5,335.5));   
      sc = rdoExpert.regHist(m_Details_mod3_occupancy= TH2F_LW::create(("Details_occupancy_"+m_DetailsMod3).c_str(),   ("column vs row hitmap for mod3" + m_histTitleExt).c_str(), 160,-0.5,155.5,336,-0.5,335.5));   
      sc = rdoExpert.regHist(m_Details_mod4_occupancy= TH2F_LW::create(("Details_occupancy_"+m_DetailsMod4).c_str(),   ("column vs row hitmap for mod4" + m_histTitleExt).c_str(), 160,-0.5,155.5,336,-0.5,335.5));   
      sc = rdoExpert.regHist(m_Details_mod1_ToT      = TH1F_LW::create(("Details_ToT_"+m_DetailsMod1).c_str(),  ("ToT mod1" + m_histTitleExt).c_str(), 300,-0.5,299.5));  
      sc = rdoExpert.regHist(m_Details_mod2_ToT      = TH1F_LW::create(("Details_ToT_"+m_DetailsMod2).c_str(),  ("ToT mod2" + m_histTitleExt).c_str(), 300,-0.5,299.5));  
      sc = rdoExpert.regHist(m_Details_mod3_ToT      = TH1F_LW::create(("Details_ToT_"+m_DetailsMod3).c_str(),  ("ToT mod3" + m_histTitleExt).c_str(), 300,-0.5,299.5));  
      sc = rdoExpert.regHist(m_Details_mod4_ToT      = TH1F_LW::create(("Details_ToT_"+m_DetailsMod4).c_str(),  ("ToT mod4" + m_histTitleExt).c_str(), 300,-0.5,299.5));  
   }
   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;         
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::BookHitsLumiBlockMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Hits for lowStat" << endreq;  
   
   std::string path = "Pixel/LumiBlock";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnPixelTrack");
   MonGroup lumiBlockHist(   this, path.c_str(), lowStat, ATTRIB_MANAGED); //declare a group of histograms

   std::string modlabel[6];
   modlabel[0]="ECA"; modlabel[1]="ECC";
   modlabel[2]="B0";  modlabel[3]="B1";  modlabel[4]="B2"; modlabel[5]="IBL";

   std::string hname;
   std::string htitles;
   std::string atext_LB = ";lumi block"; 
   std::string atext_nevt = ";# events"; 
   std::string atext_nhit = ";# hits"; 
   std::string atext_hit = ";# hits/event"; 
   std::string atext_occ = ";# hits/pixel/event"; 
   std::string atext_tot = ";ToT [BC]"; 
   std::string atext_lv1 = ";Level 1 Accept"; 

   if(m_doHighOccupancy) {
      hname = makeHistname("num_hits_LB", false);
      htitles = makeHisttitle("Number of pixel hits in an event", (atext_hit+atext_nevt), false);
      sc = lumiBlockHist.regHist(m_num_hits_LB = TH1I_LW::create(hname.c_str(), htitles.c_str(), 100, -0.5, 24999.5));
   }
   if(m_doLowOccupancy) {
      hname = makeHistname("num_hits_low_LB", false);
      htitles = makeHisttitle("Number of pixel hits in an event", (atext_hit+atext_nevt), false);
      sc = lumiBlockHist.regHist(m_num_hits_low_LB = TH1I_LW::create(hname.c_str(), htitles.c_str(), 200, -0.5, 199.5));
   }   
   if(m_doModules) {
      hname = makeHistname("num_Hits_mod_LB", false);
      htitles = makeHisttitle("Number of pixel hits in a module in an event", (atext_hit+atext_nevt), false);
      m_hit_num_mod_LB = new PixelMonModules1D(hname.c_str(), htitles.c_str(), 20, -0.5, 19.5, m_doIBL);
      sc = m_hit_num_mod_LB->regHist(this, (path+"/Modules_NumberOfHits").c_str(), lowStat, m_doIBL);
   }
   for( int i=0; i<PixLayer::COUNT; i++){
      hname = makeHistname(("Hit_ToT_LB_"+modlabel[i]), false);
      htitles = makeHisttitle(("Hit ToT, "+modlabel[i]), (atext_tot+atext_nhit), false);
      if(i != PixLayer::kIBL){
         sc = lumiBlockHist.regHist(m_hit_ToT_LB_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, 299.5));   
      }else{
         sc = lumiBlockHist.regHist(m_hit_ToT_LB_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 20, -0.5, 19.5));   
      }

      hname = makeHistname(("Lvl1A_10min_"+modlabel[i]), false);
      htitles = makeHisttitle(("Hit Level 1 Accept, "+modlabel[i]), (atext_lv1+atext_nhit), false);
      sc = lumiBlockHist.regHist(m_Lvl1A_10min_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 14, -1.5, 12.5));
   }

   m_occupancy_10min = new PixelMon2DMaps("Occupancy_10min", ("hit occupancy" + m_histTitleExt).c_str());
   sc = m_occupancy_10min->regHist(lumiBlockHist);
   
   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;         
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::FillHitsMon(void) //Called once per event
{
   if(newLumiBlock && m_mu_vs_lumi){
      if(m_lumiTool){
         float mu = m_lumiTool->lbAverageInteractionsPerCrossing();
         m_mu_vs_lumi->Fill(m_manager->lumiBlockNumber(),mu);
      }else{
         msg(MSG::ERROR)  << "No lumitool found in FillHitsMon!"<<endreq;
      }
   }

   //  m_mu_LB->Fill(m_manager->lumiBlockNumber(),managed->lbAverageInteractionsPerCrossing();

   // get ROD BCID
   const InDetTimeCollection *Pixel_BCIDColl = 0;  
   if ( evtStore()->contains<InDetTimeCollection>("PixelBCID") )  sc = evtStore()->retrieve(Pixel_BCIDColl, "PixelBCID"); 
   int n_pix_bcid_nrobs   = 0;   //pixel BCID robs counter
   int prev_pix_bcid = 0;
   
   if ( !sc.isFailure() && Pixel_BCIDColl!=0 ) 
   {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Found Pixel BCID collection"<<endreq;

      for ( InDetTimeCollection::const_iterator ipix_bcid = Pixel_BCIDColl->begin(); ipix_bcid != Pixel_BCIDColl->end(); ++ipix_bcid ) 
      {
         if (!(*ipix_bcid)) continue;
         //Current bcid
         const unsigned int pix_bcid = (*ipix_bcid)->second;
	 
	      ///if ( ipix_bcid > Pixel_BCIDColl->begin() && m_diff_ROD_BCID){
         ///   m_diff_ROD_BCID->Fill(n_pix_bcid_nrobs-1,prev_pix_bcid-pix_bcid); 
         ///}
	      prev_pix_bcid = pix_bcid;
         n_pix_bcid_nrobs++;
      } // End for loop
   }
   if (sc.isFailure()) if(msgLvl(MSG::INFO)) {msg(MSG::INFO)  << "Could not find the data object PixelBCID" << " !" << endreq;}

   //needed for the rodSim histos and timing/trigger histogram
   //long int extLvl1ID = -1;
   //   std::vector<int> trigid; trigid.clear(); // Trigger ID
   if(m_doRodSim || m_doTiming)
   {
      const EventInfo* thisEventInfo;
      sc=evtStore()->retrieve(thisEventInfo);
      if(sc != StatusCode::SUCCESS) 
      {
	      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endreq;
      }else{
	      if (m_doRodSim|| m_doTiming) {
	      //extLvl1ID = thisEventInfo->trigger_info()->extendedLevel1ID();
	      }
       /*if (m_doTiming) {
        * std::vector<unsigned int> level1TriggerInfo_t = thisEventInfo->trigger_info()->level1TriggerInfo();
        * for (unsigned int j=16; j<24 && j<level1TriggerInfo_t.size(); ++j) {
        *   for (unsigned int i=0; i<32; ++i) {
        *      if ( (level1TriggerInfo_t[j] >> i) & 0x1 ) trigid.push_back(i+(j%8)*32); // Found the ID
        *   } // Loop over bits in the last word
        *} // Loop from 16-24 bits
	      }*/
      }
   }

   int DetailsMod1 = 0;
   int DetailsMod2 = 0;
   int DetailsMod3 = 0;
   int DetailsMod4 = 0;
   int nhitsM1=0;
   int nhitsM2=0;
   int nhitsM3=0;
   int nhitsM4=0;
   if(m_doDetails)
   {
      DetailsMod1 = ParseDetailsString(m_DetailsMod1); 
      DetailsMod2 = ParseDetailsString(m_DetailsMod2); 
      DetailsMod3 = ParseDetailsString(m_DetailsMod3); 
      DetailsMod4 = ParseDetailsString(m_DetailsMod4); 
   }

   static constexpr int nmod_phi[PixLayer::COUNT] = {48, 48, 22, 38, 52, 14};
   static constexpr int nmod_eta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 20};

   int m_HitPerEventArray_disksA[ nmod_phi[PixLayer::kECA] ][ nmod_eta[PixLayer::kECA] ];
   int m_HitPerEventArray_disksC[ nmod_phi[PixLayer::kECC] ][ nmod_eta[PixLayer::kECC] ];
   int m_HitPerEventArray_l0[ nmod_phi[PixLayer::kB0] ][ nmod_eta[PixLayer::kB0] ];
   int m_HitPerEventArray_l1[ nmod_phi[PixLayer::kB1] ][ nmod_eta[PixLayer::kB1] ];
   int m_HitPerEventArray_l2[ nmod_phi[PixLayer::kB2] ][ nmod_eta[PixLayer::kB2] ];
   int m_HitPerEventArray_lI[ nmod_phi[PixLayer::kIBL] ][ nmod_eta[PixLayer::kIBL] ];

   ///if (m_doModules)
   ///{
      for(int i=0; i<PixLayer::COUNT; i++){
         for( int phi=0; phi<nmod_phi[i]; phi++){
            for(int eta=0; eta<nmod_eta[i]; eta++){
               if(i==PixLayer::kECA) m_HitPerEventArray_disksA[phi][eta]=0;
               if(i==PixLayer::kECC) m_HitPerEventArray_disksC[phi][eta]=0;
               if(i==PixLayer::kB0)  m_HitPerEventArray_l0[phi][eta]=0;
               if(i==PixLayer::kB1)  m_HitPerEventArray_l1[phi][eta]=0;
               if(i==PixLayer::kB2)  m_HitPerEventArray_l2[phi][eta]=0;
               if(i==PixLayer::kIBL) m_HitPerEventArray_lI[phi][eta]=0;
            }
         }
      }
   ///}//end doModules

   double nhits=0;
   double nhits_mod[PixLayer::COUNT]={0};

   Identifier rdoID;

   int nGood_mod[PixLayerIBL2D3D::COUNT] = {m_nGood_ECA, m_nGood_ECC, m_nGood_B0, m_nGood_B1, m_nGood_B2, (int)((2* m_nGood_IBL2D)+m_nGood_IBL3D), m_nGood_IBL2D, m_nGood_IBL3D};
   int nchannels[PixLayerIBL2D3D::COUNT] = {46080, 46080, 46080, 46080, 46080, 26880, 53760, 26880};
   double nactivechannels = 0.;
   double nactivechannels_mod[PixLayerIBL2D3D::COUNT];
   double nactivechannels_wSync_mod[PixLayerIBL2D3D::COUNT];
   for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      nactivechannels_mod[i] = 1.0*nchannels[i]*nGood_mod[i];
      nactivechannels_wSync_mod[i] = 1.0 * nchannels[i] * m_nActivAndSync_mod[i];
      nactivechannels =+ nactivechannels_mod[i];
   }


   // retrieve Pixel RDO container from storegate
   sc=evtStore()->retrieve(m_rdocontainer,m_Pixel_RDOName);
   if (sc.isFailure() || !m_rdocontainer) 
   {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not find the data object " << m_Pixel_RDOName << " !" << endreq;
      m_storegate_errors->Fill(1.,2.);  //first entry (1). is for RDO, second (2) is for retrieve problem
      return StatusCode::SUCCESS;  //fail gracefully and keep going in the next tool
   } else {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Data object " << m_Pixel_RDOName << " found" << endreq;
   }

   PixelRDO_Container::const_iterator colNext   = m_rdocontainer->begin();
   PixelRDO_Container::const_iterator lastCol   = m_rdocontainer->end();
   DataVector<PixelRDORawData>::const_iterator p_rdo;
  
   for (; colNext != lastCol; ++colNext) // Pixel ROD Loop
   {
      const InDetRawDataCollection<PixelRDORawData>* PixelCollection(*colNext);
      if (!PixelCollection) 
      {
         m_storegate_errors->Fill(1.,4.);  //first entry (1). is for RDO, second (4) is for data problem
         continue;
      }
      
      for(p_rdo=PixelCollection->begin(); p_rdo!=PixelCollection->end(); ++p_rdo) {


	      rdoID=(*p_rdo)->identify();
         int pixlayer = GetPixLayerID(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_doIBL);
         int pixlayerdbm = GetPixLayerIDDBM(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_doIBL);
         int pixlayeribl2d3d = pixlayer;
         if( pixlayeribl2d3d == PixLayer::kIBL ){
            pixlayeribl2d3d = GetPixLayerIDIBL2D3D(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_pixelid->eta_module(rdoID), m_doIBL);
         }
         int pixlayeribl2d3ddbm = pixlayerdbm;
         if( pixlayeribl2d3ddbm == PixLayerDBM::kIBL ){
            pixlayeribl2d3ddbm = GetPixLayerIDIBL2D3DDBM(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_pixelid->eta_module(rdoID), m_doIBL);
         }


	      bool isIBL = false;
	      if (m_pixelid->barrel_ec(rdoID)==0 && m_doIBL && m_Lvl1A_mod[PixLayerDBM::kIBL] && m_pixelid->layer_disk(rdoID)==0) isIBL = true;

         if(m_doOnTrack || m_doOnPixelTrack) if(!OnTrack(rdoID,false) ) continue; //if we only want hits on track, and the hit is NOT on the track, skip filling
         
         /////////////Start main fill block here///////////////
         //be sure to check each histo exists before filling it

	      /// Fill Occupancy
         if(m_occupancy) m_occupancy->Fill(rdoID, m_pixelid, m_doIBL);
         if(m_occupancy_10min && m_doLumiBlock) m_occupancy_10min->Fill(rdoID, m_pixelid, m_doIBL);
         if(m_average_pixocc && nchannels[pixlayeribl2d3d] > 0) m_average_pixocc->WeightingFill(rdoID, m_pixelid, m_doIBL, 1.0/( 1.0*nchannels[pixlayeribl2d3d]) );
	      if(m_doOnline && m_hitmap_tmp) m_hitmap_tmp->Fill(rdoID, m_pixelid, m_doIBL);
         
         /// Fill Lvl1A
	      if(m_Lvl1A){
	         m_Lvl1A->Fill((*p_rdo)->getLVL1A());
            if(pixlayerdbm != 99 && m_Lvl1A_mod[pixlayerdbm]) m_Lvl1A_mod[pixlayerdbm]->Fill( (*p_rdo)->getLVL1A());
	      }
         if(pixlayer != 99 && m_Lvl1A_10min_mod[pixlayer] && m_doLumiBlock ) m_Lvl1A_10min_mod[pixlayer]->Fill( (*p_rdo)->getLVL1A() );
	      if(isIBL && m_Lvl1ID_IBL) m_Lvl1ID_IBL->Fill((*p_rdo)->getLVL1ID()%32);
	      if(!isIBL && m_Lvl1ID_PIX) m_Lvl1ID_PIX->Fill((*p_rdo)->getLVL1ID());

         
         const EventInfo* thisEventInfo;
         sc=evtStore()->retrieve(thisEventInfo);
	      
         
         /// Fill difference of Lvl1
         if(sc != StatusCode::SUCCESS)
         {
	         if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endreq;
         }else{
	         int lvl1idMOD = (int)(*p_rdo)->getLVL1ID();
	         int lvl1idATLAS = (int)((thisEventInfo->trigger_info()->extendedLevel1ID())&0xf);
	         int difflvl1id = lvl1idATLAS - lvl1idMOD%32;
	         
            if(pixlayer != 99 && m_Lvl1ID_diff_mod_ATLAS_mod[pixlayer]) m_Lvl1ID_diff_mod_ATLAS_mod[pixlayer]->Fill(difflvl1id);
	   
	         //	   int lvl1id = (thisEventInfo->trigger_info()->extendedLevel1ID())&0xf;
	         if(m_Lvl1ID_diff_mod_ATLAS_per_LB) m_Lvl1ID_diff_mod_ATLAS_per_LB->Fill(m_manager->lumiBlockNumber(),rdoID,m_pixelid,difflvl1id,m_doIBL,false);
	         if(m_Lvl1ID_absdiff_mod_ATLAS_per_LB) m_Lvl1ID_absdiff_mod_ATLAS_per_LB->Fill(m_manager->lumiBlockNumber(),rdoID,m_pixelid,fabs(difflvl1id),m_doIBL,false);
         }
         
         /// Fill BCID
         if(m_BCID) m_BCID->Fill((*p_rdo)->getBCID());
	      if(m_Atlas_BCID) m_Atlas_BCID->Fill(prev_pix_bcid); //defined at the start of the method
         if(m_BCID_Profile) m_BCID_Profile->Fill(double(prev_pix_bcid),double(nhits));          
         if(pixlayer != 99 && m_diff_ROD_vs_Module_BCID_mod[pixlayer]) m_diff_ROD_vs_Module_BCID_mod[pixlayer]->Fill( (prev_pix_bcid&0x000000ff)-(*p_rdo)->getBCID() ); 
	      if(m_RodSim_FrontEnd_minus_Lvl1ID) m_RodSim_FrontEnd_minus_Lvl1ID->Fill(m_pixelCableSvc->getFE(&rdoID,rdoID) -(*p_rdo)->getLVL1ID());
         if(m_RodSim_BCID_minus_ToT) m_RodSim_BCID_minus_ToT->Fill((*p_rdo)->getBCID() - (*p_rdo)->getToT() );


	      if(m_FE_chip_hit_summary) m_FE_chip_hit_summary->Fill(m_pixelCableSvc->getFE(&rdoID,rdoID),rdoID,m_pixelid,m_doIBL);

	      if(m_hiteff_mod){
	         if(OnTrack(rdoID,false)){
	            m_hiteff_mod->Fill(m_manager->lumiBlockNumber(),1.,rdoID,m_pixelid,m_doIBL);
	         }else{
	            m_hiteff_mod->Fill(m_manager->lumiBlockNumber(),0.,rdoID,m_pixelid,m_doIBL);
	         }
	      }
	    

         /// Fill ToT
	      if(pixlayerdbm != 99 && m_hit_ToT[pixlayerdbm]) m_hit_ToT[pixlayerdbm]->Fill((*p_rdo)->getToT());
         if(pixlayerdbm == PixLayerDBM::kIBL){
            int ibl2d3d = GetPixLayerIDIBL2D3DDBM(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_pixelid->eta_module(rdoID), m_doIBL);
            if(m_hit_ToT[ibl2d3d]) m_hit_ToT[ibl2d3d]->Fill((*p_rdo)->getToT());
         }
         if(m_doLumiBlock && pixlayer != 99){
            if(m_hit_ToT_LB_mod[pixlayer]) m_hit_ToT_LB_mod[pixlayer]->Fill((*p_rdo)->getToT());
         }
         if(pixlayer != 99 && m_ToT_etaphi_mod[pixlayer] ) m_ToT_etaphi_mod[pixlayer]->Fill(m_pixelid->eta_module(rdoID), m_pixelid->phi_module(rdoID), (*p_rdo)->getToT());


         /// Monitoring!!
         if(m_doOnline)
         {
            if(pixlayer != 99 && m_hit_ToT_tmp_mod[pixlayer]) m_hit_ToT_tmp_mod[pixlayer]->Fill((*p_rdo)->getToT());
         }
	 
	      /// Not yet modified for IBL geometry:
         if(m_doDetails){
            int currentID = 1000000 + (-m_pixelid->barrel_ec(rdoID) + 2)*100000 + (m_pixelid->layer_disk(rdoID))*10000 + (m_pixelid->phi_module(rdoID))*100 + (m_pixelid->eta_module(rdoID) + 6); 
	         if(m_Details_mod1_occupancy && currentID==DetailsMod1) m_Details_mod1_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID));
            if(m_Details_mod2_occupancy && currentID==DetailsMod2) m_Details_mod2_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID));
            if(m_Details_mod3_occupancy && currentID==DetailsMod3) m_Details_mod3_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID));
            if(m_Details_mod4_occupancy && currentID==DetailsMod4) m_Details_mod4_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID));
            if(m_Details_mod1_ToT && currentID==DetailsMod1) m_Details_mod1_ToT->Fill((*p_rdo)->getToT());     
            if(m_Details_mod2_ToT && currentID==DetailsMod2) m_Details_mod2_ToT->Fill((*p_rdo)->getToT());     
            if(m_Details_mod3_ToT && currentID==DetailsMod3) m_Details_mod3_ToT->Fill((*p_rdo)->getToT());     
            if(m_Details_mod4_ToT && currentID==DetailsMod4) m_Details_mod4_ToT->Fill((*p_rdo)->getToT());  
         }
         if(m_pixel_occupancy) m_pixel_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID),rdoID, m_pixelid,m_doIBL);

         if(pixlayer != 99) nhits_mod[pixlayer]++;
         nhits++;
         nhitsM1++;
         nhitsM2++;
         nhitsM3++;
         nhitsM4++;

	      ///if (m_doModules)//fill module hit arrays so we can calculate the number of hits/event/module 
         ///{
	         if(m_pixelid->barrel_ec(rdoID)==2 ) m_HitPerEventArray_disksA[m_pixelid->phi_module(rdoID)][m_pixelid->layer_disk(rdoID)]++;

	         if(m_pixelid->barrel_ec(rdoID)==-2) m_HitPerEventArray_disksC[m_pixelid->phi_module(rdoID)][m_pixelid->layer_disk(rdoID)]++;

	         if(m_pixelid->barrel_ec(rdoID)==0 )
            {
 	            if(m_doIBL && m_pixelid->layer_disk(rdoID)==0) m_HitPerEventArray_lI[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+10]++;
               if(m_pixelid->layer_disk(rdoID)==0+m_doIBL) m_HitPerEventArray_l0[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+6]++;
               if(m_pixelid->layer_disk(rdoID)==1+m_doIBL) m_HitPerEventArray_l1[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+6]++;
               if(m_pixelid->layer_disk(rdoID)==2+m_doIBL) m_HitPerEventArray_l2[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+6]++;
            } 
         ///}

         ///////////End of main fill block////////////////////
      }
   } //end of ROD loop
   
   

   ////////////////////Fill after event block////////////////
   if(m_doOnline)//should we fill these mid run or only at the end?
   {
      FillSummaryHistos(m_occupancy, m_occupancy_summary_mod[PixLayer::kECA], m_occupancy_summary_mod[PixLayer::kECC], 
                                     m_occupancy_summary_mod[PixLayer::kIBL], m_occupancy_summary_mod[PixLayer::kB0], 
                                     m_occupancy_summary_mod[PixLayer::kB1],  m_occupancy_summary_mod[PixLayer::kB2]);
      FillSummaryHistos(m_occupancy, m_occupancy_summary_low_mod[PixLayer::kECA], m_occupancy_summary_low_mod[PixLayer::kECC], 
                                     m_occupancy_summary_low_mod[PixLayer::kIBL], m_occupancy_summary_low_mod[PixLayer::kB0], 
                                     m_occupancy_summary_low_mod[PixLayer::kB1],  m_occupancy_summary_low_mod[PixLayer::kB2]);
      if(m_doRefresh) {
         for(int i=0; i<PixLayer::COUNT; i++){
            if( m_hit_ToT_Mon_mod[i] && m_hit_ToT_tmp_mod[i]) TH1FFillMonitoring(m_hit_ToT_Mon_mod[i], m_hit_ToT_tmp_mod[i]);
         }
         if(m_hitmap_mon) m_hitmap_mon->Fill2DMon(m_hitmap_tmp);
      }
   }//end of doOnline loop processing                                  


   /// Fill number of hits per lLB
   if(m_events_per_lumi) m_events_per_lumi->Fill(m_manager->lumiBlockNumber());
   if(m_hits_per_lumi) m_hits_per_lumi->Fill(m_manager->lumiBlockNumber(), nhits);     
   for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
      if(m_hits_per_lumi_mod[i]) m_hits_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), nhits_mod[i]);
   }

   /// Fill average occupancy
   double avgocc = 0;
   double avgocc_mod[PixLayer::COUNT] = {0};
   double avgocc_wSync_mod[PixLayer::COUNT] = {0};
   if(nactivechannels>0) avgocc = nhits/nactivechannels;
   if(m_avgocc_per_lumi) m_avgocc_per_lumi->Fill(m_manager->lumiBlockNumber(), avgocc);

   for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
      if(nactivechannels_mod[i] > 0){
         avgocc_mod[i] = nhits_mod[i]/nactivechannels_mod[i];
         avgocc_wSync_mod[i] = nhits_mod[i]/nactivechannels_wSync_mod[i];
      }
      if(m_avgocc_per_lumi_mod[i]) m_avgocc_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(),avgocc_mod[i]);
      if(m_avgocc_wSyncMod_per_lumi_mod[i]) m_avgocc_wSyncMod_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(),avgocc_wSync_mod[i]);
      if(m_avgocc_per_bcid_mod[i]) m_avgocc_per_bcid_mod[i]->Fill(prev_pix_bcid, avgocc_mod[i]);
      if(avgocc_mod[i] > 0.0007 && m_nlargeevt_per_lumi_mod[i]) m_nlargeevt_per_lumi_mod[i]->Fill( m_lumiBlockNum );
      if(m_maxocc_per_lumi_mod[i]) m_maxocc_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), avgocc_mod[i]);
      if(m_maxocc_per_bcid_mod[i]){
         int bin = m_maxocc_per_bcid_mod[i]->GetXaxis()->FindBin( 1.0*prev_pix_bcid );
         double content = m_maxocc_per_bcid_mod[i]->GetBinContent( bin );
         if( avgocc_mod[i] > content ) m_maxocc_per_bcid_mod[i]->SetBinContent(bin, avgocc_mod[i]);
      }
      if(m_totalhits_per_bcid_mod[i]) m_totalhits_per_bcid_mod[i]->Fill(1.0*prev_pix_bcid, nhits_mod[i]);
      if(m_avgocc_LBvsBCID_mod[i]) m_avgocc_LBvsBCID_mod[i]->Fill(1.0*prev_pix_bcid, m_manager->lumiBlockNumber(), avgocc_mod[i] );
   }

   if(avgocc_mod[PixLayer::kB0] > 0 && m_avgocc_ratioIBLB0_per_lumi) 
      m_avgocc_ratioIBLB0_per_lumi->Fill(m_manager->lumiBlockNumber(), avgocc_mod[PixLayer::kIBL]/avgocc_mod[PixLayer::kB0]);

   if(m_occupancy_time1&&m_occupancy_time2&&m_occupancy_time3) FillTimeHisto(double(nhits/(1744.0+280*m_doIBL)),m_occupancy_time1, m_occupancy_time2, m_occupancy_time3,10.,60.,360. );

   if(m_Atlas_BCID_hits) m_Atlas_BCID_hits->Fill(prev_pix_bcid,nhits);

   /// Fill the #hit per module per event
   //if(!m_doOnline){
   for(int i=0; i<PixLayer::COUNT; i++){
      for(int phi=0; phi<nmod_phi[i]; phi++){
         for(int eta=0; eta<nmod_eta[i]; eta++){
	   if(i == PixLayer::kECA && m_nhits_mod[i]) m_nhits_mod[i]->Fill( m_HitPerEventArray_disksA[phi][eta] );
	   if(i == PixLayer::kECC && m_nhits_mod[i]) m_nhits_mod[i]->Fill( m_HitPerEventArray_disksC[phi][eta] );
	   if(i == PixLayer::kB0  && m_nhits_mod[i]) m_nhits_mod[i]->Fill( m_HitPerEventArray_l0[phi][eta]);
      if(i == PixLayer::kB1  && m_nhits_mod[i]) m_nhits_mod[i]->Fill( m_HitPerEventArray_l1[phi][eta]);
      if(i == PixLayer::kB2  && m_nhits_mod[i]) m_nhits_mod[i]->Fill( m_HitPerEventArray_l2[phi][eta]);
	   if(i == PixLayer::kIBL && m_nhits_mod[i]) m_nhits_mod[i]->Fill( m_HitPerEventArray_lI[phi][eta]);
         }
      }
   }

   /// Put the #hits per event for each layer
   if( m_event == 0){
      for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         m_hitocc_stock[i].push_back( avgocc_mod[i] );
      }
   }else if( !newLumiBlock ){
      for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         m_hitocc_stock[i].push_back( avgocc_mod[i] );
      }
   }
   /// Fill some histograms only if =< 50% of modules disabled
   if(!m_majorityDisabled) {

      if(m_Details_mod1_num_hits) m_Details_mod1_num_hits->Fill(nhitsM1);
      if(m_Details_mod2_num_hits) m_Details_mod2_num_hits->Fill(nhitsM2);
      if(m_Details_mod3_num_hits) m_Details_mod3_num_hits->Fill(nhitsM3);
      if(m_Details_mod4_num_hits) m_Details_mod4_num_hits->Fill(nhitsM4);
     
      if(m_num_hits) m_num_hits->Fill(nhits);
      if(m_num_hits_low) m_num_hits_low->Fill(nhits);
      if(m_doLumiBlock){ 
         if(m_num_hits_LB) m_num_hits_LB->Fill(nhits);
         if(m_num_hits_low_LB) m_num_hits_low_LB->Fill(nhits);
      }

      if (m_doModules) 
      {
	      PixelID::const_id_iterator idIt    = m_pixelid->wafer_begin();
	      PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();
	      for (; idIt != idItEnd; ++idIt) {
	         Identifier WaferID = *idIt;
	         if(m_pixelid->barrel_ec(WaferID)==2 ){
	            m_hit_num_mod->Fill( m_HitPerEventArray_disksA[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	         }
	         if(m_pixelid->barrel_ec(WaferID)==-2) m_hit_num_mod->Fill( m_HitPerEventArray_disksC[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	         if(m_pixelid->barrel_ec(WaferID)==0 )
	         {
		         if(m_doIBL && m_pixelid->layer_disk(WaferID)==0) {
		            m_hit_num_mod->Fill( m_HitPerEventArray_lI[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+10], WaferID, m_pixelid ,m_doIBL);
		         }
		         if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) {
		            m_hit_num_mod->Fill( m_HitPerEventArray_l0[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		         }
		         if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) m_hit_num_mod->Fill( m_HitPerEventArray_l1[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		         if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) m_hit_num_mod->Fill( m_HitPerEventArray_l2[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
	         }  
	     
	         if (m_doLumiBlock) {
	            if(m_pixelid->barrel_ec(WaferID)==2 ) m_hit_num_mod_LB->Fill( m_HitPerEventArray_disksA[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	            if(m_pixelid->barrel_ec(WaferID)==-2) m_hit_num_mod_LB->Fill( m_HitPerEventArray_disksC[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	            if(m_pixelid->barrel_ec(WaferID)==0 )                                                                                                                                       
		         {
		            if(m_doIBL && m_pixelid->layer_disk(WaferID)==0) m_hit_num_mod_LB->Fill( m_HitPerEventArray_lI[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+10], WaferID, m_pixelid ,m_doIBL);
		            if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) m_hit_num_mod_LB->Fill( m_HitPerEventArray_l0[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		            if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) m_hit_num_mod_LB->Fill( m_HitPerEventArray_l1[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		            if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) m_hit_num_mod_LB->Fill( m_HitPerEventArray_l2[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		         }  
	         }
	      }
      }
   }

   ////////////////////End fill after event block///////////
   if(nhits==0) m_storegate_errors->Fill(1.,3.);//first entry for RDO, second for size = 0
      
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::ProcHitsMon(void)
{
   
   double events = m_event;
   if(events==0) return StatusCode::SUCCESS; //if no events, the rest of the test is pointless and would divide by 0

   if(m_doPixelOccupancy && m_pixel_occupancy && (m_doNoiseMap||m_doSpectrum) )//flags need to be set, and the right histograms need to exist
   {
      if(msgLvl(MSG::WARNING)){
         msg(MSG::WARNING)  << "Starting to fill pixel granularity histograms." << endreq;   
         msg(MSG::WARNING)  << "This is very CPU and memory intensive and should not normmally be turned on" << endreq;   
         msg(MSG::WARNING)  << "Please be patient, it will take a while to fill these histograms" << endreq;   
      }

      //if(m_pixel_noise_map) m_pixel_noise_map->Reset();
      //if(m_pixel_occupancy_spectrum) m_pixel_occupancy_spectrum->Reset();

      PixelID::const_id_iterator idIt    = m_pixelid->wafer_begin();
      PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();
      for (; idIt != idItEnd; ++idIt) 
      {
         Identifier WaferID = *idIt;
         for(int phiIndex=0; phiIndex < m_pixelid->phi_index_max(WaferID); phiIndex++)
         {
            for(int etaIndex=0; etaIndex < m_pixelid->eta_index_max(WaferID); etaIndex++)
            {
               double occ_spec=0;
               if(m_pixelid->barrel_ec(WaferID)==2 ) occ_spec = m_pixel_occupancy->A[m_pixelid->layer_disk(WaferID)][m_pixelid->phi_module(WaferID)]->GetBinContent(etaIndex,phiIndex);
               if(m_pixelid->barrel_ec(WaferID)==-2) occ_spec = m_pixel_occupancy->C[m_pixelid->layer_disk(WaferID)][m_pixelid->phi_module(WaferID)]->GetBinContent(etaIndex,phiIndex);
               if(m_pixelid->barrel_ec(WaferID)==0 )                                                                                                                                       
               {
		            if(m_pixelid->layer_disk(WaferID)==0 && m_doIBL) occ_spec = m_pixel_occupancy->IBL[m_pixelid->eta_module(WaferID)+10][m_pixelid->phi_module(WaferID)]->GetBinContent(etaIndex,phiIndex);
		            if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) occ_spec = m_pixel_occupancy->B0[m_pixelid->eta_module(WaferID)+6][m_pixelid->phi_module(WaferID)]->GetBinContent(etaIndex,phiIndex);
                  if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) occ_spec = m_pixel_occupancy->B1[m_pixelid->eta_module(WaferID)+6][m_pixelid->phi_module(WaferID)]->GetBinContent(etaIndex,phiIndex);
                  if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) occ_spec = m_pixel_occupancy->B2[m_pixelid->eta_module(WaferID)+6][m_pixelid->phi_module(WaferID)]->GetBinContent(etaIndex,phiIndex);
               }  
               occ_spec = occ_spec/events;

               //if(m_pixel_noise_map) m_pixel_noise_map->Fill(etaIndex,phiIndex,WaferID, m_pixelid,occ_spec,m_doIBL);   //fill the right pixel, give it pointers to the right module, and a weight of 'occ_spec'
               //if(m_pixel_occupancy_spectrum) m_pixel_occupancy_spectrum->Fill(occ_spec, WaferID, m_pixelid, m_doIBL);   //fill the pixel occupancy and give a pointer to the right module.  Will fill 80M times total in this loop...
            }
         }
      }
   }
   if(m_doOffline) //run these only for offline
   {
      FillSummaryHistos(m_occupancy, m_occupancy_summary_mod[PixLayer::kECA], m_occupancy_summary_mod[PixLayer::kECC], 
                                     m_occupancy_summary_mod[PixLayer::kIBL], m_occupancy_summary_mod[PixLayer::kB0], 
                                     m_occupancy_summary_mod[PixLayer::kB1],  m_occupancy_summary_mod[PixLayer::kB2]);
      FillSummaryHistos(m_occupancy, m_occupancy_summary_low_mod[PixLayer::kECA], m_occupancy_summary_low_mod[PixLayer::kECC], 
                                     m_occupancy_summary_low_mod[PixLayer::kIBL], m_occupancy_summary_low_mod[PixLayer::kB0], 
                                     m_occupancy_summary_low_mod[PixLayer::kB1],  m_occupancy_summary_low_mod[PixLayer::kB2]);
   }

   if(m_average_pixocc) {  
      if( m_num_hits->GetEntries() == m_event ){
         m_event2 = m_num_hits->GetEntries();
      }else{
      }

   }


   //for( int i=0; i<PixLayer::COUNT; i++){
   //   double max = 0;
   //   for( unsigned int j=0; j<m_hitocc_stock[i].size(); j++){
   //      if(max < m_hitocc_stock[i].at(j)) max = m_hitocc_stock[i].at(j);
   //   }
   //   if(m_maxocc_per_lumi_mod[i] && max != 0) m_maxocc_per_lumi_mod[i]->Fill(m_lumiBlockNum, max);
   //}

  return StatusCode::SUCCESS;
}

