/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms showing pixel hit level information
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
#include "InDetRawData/InDetTimeCollection.h"
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
#include "LWHists/TProfile_LW.h"
#include <sstream>

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
#include "PixelMonitoring/PixelMon2DMaps.h"
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
   
   sc = rdoShift.regHist(m_hits_per_lumi      = TProfile_LW::create("Hits_per_lumi",    ("Number of pixel hits per event per LB" + m_histTitleExt + ";lumi block;# hits/event").c_str(),2500,-0.5,2499.5));
   sc = rdoExpert.regHist(m_hits_per_lumi_ECA = TProfile_LW::create("Hits_per_lumi_ECA",("Number of pixel hits per event per LB, endcap A" + m_histTitleExt + ";lumi block;# hits/event").c_str(),2500,-0.5,2499.5));
   sc = rdoExpert.regHist(m_hits_per_lumi_ECC = TProfile_LW::create("Hits_per_lumi_ECC",("Number of pixel hits per event per LB, endcap C" + m_histTitleExt + ";lumi block;# hits/event").c_str(),2500,-0.5,2499.5));
   if(m_doIBL){sc = rdoExpert.regHist(m_hits_per_lumi_IBL  = TProfile_LW::create("Hits_per_lumi_IBL", ("Number of pixel hits per event per LB, IBL" + m_histTitleExt + ";lumi block;# hits/event").c_str(),2500,-0.5,2499.5));}
   sc = rdoExpert.regHist(m_hits_per_lumi_B0  = TProfile_LW::create("Hits_per_lumi_B0", ("Number of pixel hits per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# hits/event").c_str(),2500,-0.5,2499.5));
   sc = rdoExpert.regHist(m_hits_per_lumi_B1  = TProfile_LW::create("Hits_per_lumi_B1", ("Number of pixel hits per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# hits/event").c_str(),2500,-0.5,2499.5));
   sc = rdoExpert.regHist(m_hits_per_lumi_B2  = TProfile_LW::create("Hits_per_lumi_B2", ("Number of pixel hits per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# hits/event").c_str(),2500,-0.5,2499.5));
   
   sc = rdoExpert.regHist(m_hit_ToT_ECA    = TH1F_LW::create("Hit_ToT_ECA",("Hit Time over Threshold, endcap A" + m_histTitleExt + ";ToT;# hits").c_str(), 300,-0.5,299.5));
   sc = rdoExpert.regHist(m_hit_ToT_ECC    = TH1F_LW::create("Hit_ToT_ECC",("Hit Time over Threshold, endcap C" + m_histTitleExt + ";ToT;# hits").c_str(), 300,-0.5,299.5));
   if(m_doIBL){
     sc = rdoExpert.regHist(m_hit_ToT_IBL2D     = TH1F_LW::create("Hit_ToT_IBL2D", ("Hit Time over Threshold, IBL2D" + m_histTitleExt + ";ToT;# hits").c_str(), 16,-.5,15.5));
     sc = rdoExpert.regHist(m_hit_ToT_IBL3D     = TH1F_LW::create("Hit_ToT_IBL3D", ("Hit Time over Threshold, IBL3D" + m_histTitleExt + ";ToT;# hits").c_str(), 16,-.5,15.5));
   }
   sc = rdoExpert.regHist(m_hit_ToT_B0     = TH1F_LW::create("Hit_ToT_B0", ("Hit Time over Threshold, barrel layer 0" + m_histTitleExt + ";ToT;# hits").c_str(), 300,-0.5,299.5));
   sc = rdoExpert.regHist(m_hit_ToT_B1     = TH1F_LW::create("Hit_ToT_B1", ("Hit Time over Threshold, barrel layer 1" + m_histTitleExt + ";ToT;# hits").c_str(), 300,-0.5,299.5));
   sc = rdoExpert.regHist(m_hit_ToT_B2     = TH1F_LW::create("Hit_ToT_B2", ("Hit Time over Threshold, barrel layer 2" + m_histTitleExt + ";ToT;# hits").c_str(), 300,-0.5,299.5));
  
   if(m_doIBL && m_doModules && !m_doOnline){
     sc= rdoExpert.regHist(m_ToT_vs_eta_IBL      = TH2F_LW::create("ToT_vs_eta_IBL", ("Hit ToT vs Eta, IBL" + m_histTitleExt + "; Module eta;ToT").c_str(),20,-10.5,9.5,16,0,16));
     //m_ToT_vs_eta_IBL->SetOption("colz");m_ToT_vs_eta_IBL->SetMinimum(0.);m_ToT_vs_eta_IBL->SetStats(0.);
   }

   if(m_doModules && !m_doOnline){
     sc= rdoExpert.regHist(m_ToT_vs_eta_B0 = TH2F_LW::create("ToT_vs_eta_B0", ("Hit ToT vs Eta, B0" + m_histTitleExt + "; Module eta;ToT").c_str(),13,-6.5,6.5,300,0,300));
     sc= rdoExpert.regHist(m_ToT_vs_eta_B1 = TH2F_LW::create("ToT_vs_eta_B1", ("Hit ToT vs Eta, B1" + m_histTitleExt + "; Module eta;ToT").c_str(),13,-6.5,6.5,300,0,300));
     sc= rdoExpert.regHist(m_ToT_vs_eta_B2 = TH2F_LW::create("ToT_vs_eta_B2", ("Hit ToT vs Eta, B2" + m_histTitleExt + "; Module eta;ToT").c_str(),13,-6.5,6.5,300,0,300));
     //m_ToT_vs_eta_B0->SetOption("colz");m_ToT_vs_eta_B0->SetMinimum(0.);m_ToT_vs_eta_B0->SetStats(0.);
     //m_ToT_vs_eta_B1->SetOption("colz");m_ToT_vs_eta_B1->SetMinimum(0.);m_ToT_vs_eta_B1->SetStats(0.);
     //m_ToT_vs_eta_B2->SetOption("colz");m_ToT_vs_eta_B2->SetMinimum(0.);m_ToT_vs_eta_B2->SetStats(0.);
   }
      
   if(m_doTiming)
     {
      sc = timeExpert.regHist(m_Lvl1ID_diff_mod_ATLAS    = TH1I_LW::create("Lvl1ID_diff_ATLAS_mod",  ("ATLAS_{Level 1 ID} - Module_{Level 1 ID}" + m_histTitleExt + ";#Delta Level 1 ID; # hits").c_str(), 41, -20.5, 20.5));
      sc = timeShift.regHist(m_Lvl1A                    = TH1I_LW::create("Lvl1A",  ("Hit Level 1 Accept" + m_histTitleExt + ";Level 1 Accept; # hits").c_str(), 8, -1.5, 6.5));
      sc = timeExpert.regHist(m_Lvl1ID                  = TH1I_LW::create("LvlID",  ("Level 1 ID" + m_histTitleExt + ";level 1 ID;# hits").c_str(), 20,-0.5,19.5));
      sc = timeShift.regHist(m_BCID                     = TH1I_LW::create("Pixel_BCID",  ("BCID" + m_histTitleExt + ";Pixel BCID;# pixel hits").c_str(), 300,-0.5,299.5));//2808 bunches but have a few extra bins to check 
      sc = timeShift.regHist(m_Atlas_BCID               = TH1F_LW::create("Atlas_BCID",  ("BCID" + m_histTitleExt + ";ATLAS BCID;# pixel hits").c_str(), 3500,-0.5,3499.5));
      sc = timeShift.regHist(m_Atlas_BCID_hits          = TH2F_LW::create("Atlas_BCID_Hits",  ("BCID" + m_histTitleExt + ";BCID;# pixel hits").c_str(), 3500,-0.5,3499.5,100,0,5000));
      sc = timeExpert.regHist(m_BCID_Profile            = TProfile_LW::create("Atlas_BCID_Profile",  ("BCID_Profile" + m_histTitleExt + ";BCID;# pixel hits").c_str(), 3500,-0.5,3499.5));  
      sc = timeExpert.regHist(m_diff_ROD_vs_Module_BCID = TH1I_LW::create("ROD_Module_BCID",("Difference between BCID of RODs and Modules " + m_histTitleExt + "; BCID: ROD-Module (#bc) ; Number of Pixels").c_str(),101,-50.5,50.5));
      if(!(m_doOnTrack || m_doOnPixelTrack))sc = timeShift.regHist(m_diff_ROD_BCID = TH2I_LW::create("ROD_BCID",("Difference between ROD BCID's versus ROD Number " + m_histTitleExt + "; ROD Number ; BCID: current - previous ROD").c_str(),132,-0.5,131.5,201,-100.5,100.5));//this makes no sense on track, so skip it     
//      sc = timeShift.regHist(m_trigger           = TH1F_LW::create("trigger",     ("Trigger Item" + m_histTitleExt + ";trigger item; # pixel hits").c_str(),256,-0.5,255.5));
//      sc = timeShift.regHist(m_trigger_ECA       = TH1F_LW::create("trigger_ECA", ("Trigger Item, endcap A" + m_histTitleExt + ";trigger item; # pixel hits").c_str(),256,-0.5,255.5));
//      sc = timeShift.regHist(m_trigger_ECC       = TH1F_LW::create("trigger_ECC", ("Trigger Item, endcap C" + m_histTitleExt + ";trigger item; # pixel hits").c_str(),256,-0.5,255.5));
//      sc = timeShift.regHist(m_trigger_B0        = TH1F_LW::create("trigger_B0",  ("Trigger Item, barrel layer 0" + m_histTitleExt + ";trigger item; # pixel hits").c_str(),256,-0.5,255.5));
//      sc = timeShift.regHist(m_trigger_B1        = TH1F_LW::create("trigger_B1",  ("Trigger Item, barrel layer 1" + m_histTitleExt + ";trigger item; # pixel hits").c_str(),256,-0.5,255.5));
//     sc = timeShift.regHist(m_trigger_B2        = TH1F_LW::create("trigger_B2",  ("Trigger Item, barrel layer 2" + m_histTitleExt + ";trigger item; # pixel hits").c_str(),256,-0.5,255.5));
   }
   if(m_do2DMaps)
   {
      m_occupancy = new PixelMon2DMaps("Occupancy", ("hit occupancy" + m_histTitleExt).c_str());
      sc = m_occupancy->regHist(rdoShift);
      m_average_occupancy = new PixelMon2DMaps("Average_Occupancy", ("average hit occupancy" + m_histTitleExt).c_str());
      sc = m_average_occupancy->regHist(rdoShift);
      m_Lvl1ID_diff_mod_ATLAS_per_LB = new PixelMon2DLumiProfiles("Lvl1ID_diff_ATLAS_mod_per_LB", ("ATLAS_{Level 1 ID} - Module_{Level 1 ID} per LB" + m_histTitleExt).c_str(),"#Delta Level 1 ID");
      sc = m_Lvl1ID_diff_mod_ATLAS_per_LB->regHist(timeExpert);
   }
   if(m_doModules)
   {
     m_hit_num_mod = new PixelMonModules1D("Hit_num", ("Number of hits per event in module" + m_histTitleExt).c_str(), 15,-0.5,149.5,m_doIBL);
     sc = m_hit_num_mod->regHist(this,(path+"/Modules_NumberOfHits").c_str(),run, m_doIBL);
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
      sc = rdoShift.regHist(m_num_hits= TH1I_LW::create("num_hits",  ("Number of pixel hits per event" + m_histTitleExt + ";# pixel hits/event;# events").c_str(), 2500,0.,(int)max_hits));
      sc = rdoExpert.regHist(m_ecA_occupancy_summary     = TH1F_LW::create("A_occupancy_summary",  ("Average Endcap A hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 100,0.,(int)max_avhits));
      sc = rdoExpert.regHist(m_ecC_occupancy_summary     = TH1F_LW::create("C_occupancy_summary",  ("Average Endcap C hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 100,0.,(int)max_avhits));
      if(m_doIBL){sc = rdoExpert.regHist(m_bar_layI_occupancy_summary= TH1F_LW::create("IBL_occupancy_summary", ("Average IBL hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 100,0.,(int)max_avhits));}
      sc = rdoExpert.regHist(m_bar_lay0_occupancy_summary= TH1F_LW::create("B0_occupancy_summary", ("Average Barrel layer 0 hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 100,0.,(int)max_avhits));
      sc = rdoExpert.regHist(m_bar_lay1_occupancy_summary= TH1F_LW::create("B1_occupancy_summary", ("Average Barrel layer 1 hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 100,0.,(int)max_avhits));
      sc = rdoExpert.regHist(m_bar_lay2_occupancy_summary= TH1F_LW::create("B2_occupancy_summary", ("Average Barrel layer 2 hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 100,0.,(int)max_avhits));
   }
   if(m_doLowOccupancy)
   {
      sc = rdoShift.regHist(m_num_hits_low= TH1I_LW::create("num_hits_low_occupancy",  ("Number of pixel hits per event" + m_histTitleExt + ";# pixel hits/event;# events").c_str(), 200,-0.5,199.5));
      sc = rdoExpert.regHist(m_ecA_occupancy_summary_low     = TH1F_LW::create("A_occupancy_summary_low_occupancy",  ("Average Endcap A hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 50,0.,2.));
      sc = rdoExpert.regHist(m_ecC_occupancy_summary_low     = TH1F_LW::create("C_occupancy_summary_low_occupancy",  ("Average Endcap C hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 50,0.,2.));
      if(m_doIBL){sc = rdoExpert.regHist(m_bar_layI_occupancy_summary_low= TH1F_LW::create("IBL_occupancy_summary_low_occupancy", ("Average IBL hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 50,0.,2.));}
      sc = rdoExpert.regHist(m_bar_lay0_occupancy_summary_low= TH1F_LW::create("B0_occupancy_summary_low_occupancy", ("Average Barrel layer 0 hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 50,0.,2.));
      sc = rdoExpert.regHist(m_bar_lay1_occupancy_summary_low= TH1F_LW::create("B1_occupancy_summary_low_occupancy", ("Average Barrel layer 1 hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 50,0.,2.));
      sc = rdoExpert.regHist(m_bar_lay2_occupancy_summary_low= TH1F_LW::create("B2_occupancy_summary_low_occupancy", ("Average Barrel layer 2 hit occupancy" + m_histTitleExt + ";average # of hits per module per event;# modules").c_str(), 50,0.,2.));
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
   
   if(m_doHighOccupancy) {
     sc = lumiBlockHist.regHist(m_num_hits_LB = TH1I_LW::create("num_hits_LB", ("Number of pixel hits per event" + m_histTitleExt + ";# pixel hits/event;# events").c_str(), 200,0.,10000));
   }
   if(m_doLowOccupancy) {
     sc = lumiBlockHist.regHist(m_num_hits_low_LB = TH1I_LW::create("num_hits_low_occupancy_LB", ("Number of pixel hits per event" + m_histTitleExt + ";# pixel hits/event;# events").c_str(), 200,-0.5,199.5));
   }   
   if(m_doModules) {
     m_hit_num_mod_LB = new PixelMonModules1D("Hit_num_LB", ("Number of hits per event in module" + m_histTitleExt).c_str(), 20,-0.5,19.5,m_doIBL);
     sc = m_hit_num_mod_LB->regHist(this,(path+"/Modules_NumberOfHits").c_str(),lowStat,m_doIBL);
   }
   
   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;         
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::FillHitsMon(void) //Called once per event
{
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
	 
	 if ( ipix_bcid > Pixel_BCIDColl->begin() && m_diff_ROD_BCID) m_diff_ROD_BCID->Fill(n_pix_bcid_nrobs-1,prev_pix_bcid-pix_bcid); 
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
/*	if (m_doTiming) {
	  std::vector<unsigned int> level1TriggerInfo_t = thisEventInfo->trigger_info()->level1TriggerInfo();
	  for (unsigned int j=16; j<24 && j<level1TriggerInfo_t.size(); ++j) {
	    for (unsigned int i=0; i<32; ++i) {
	      if ( (level1TriggerInfo_t[j] >> i) & 0x1 ) trigid.push_back(i+(j%8)*32); // Found the ID
	    } // Loop over bits in the last word
	  } // Loop from 16-24 bits
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

   if (m_doModules)
     {
       if(m_doIBL){
	 for(int i=0;i<20;i++){
	   for(int j=0;j<14;j++) m_HitPerEventArray_lI[j][i]=0;
	 }
       }
       for(int i=0;i<13;i++)
	 {
	   for(int j=0;j<22;j++) m_HitPerEventArray_l0[j][i]=0;
	   for(int j=0;j<38;j++) m_HitPerEventArray_l1[j][i]=0;
	   for(int j=0;j<52;j++) m_HitPerEventArray_l2[j][i]=0;
	 }
       for(int j=0;j<3;j++)
	 {
	   for(int i=0;i<48;i++)
	     {
	       m_HitPerEventArray_disksA[i][j]=0;
	       m_HitPerEventArray_disksC[i][j]=0;
	     }
	 }
     }//end doModules

   int nhits=0;
   int nhits_ECA=0;
   int nhits_ECC=0;
   int nhits_IBL=0;
   int nhits_B0=0;
   int nhits_B1=0;
   int nhits_B2=0;
   Identifier rdoID;

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
  
   for (; colNext != lastCol; ++colNext) 
   {
      const InDetRawDataCollection<PixelRDORawData>* PixelCollection(*colNext);
      if (!PixelCollection) 
      {
         m_storegate_errors->Fill(1.,4.);  //first entry (1). is for RDO, second (4) is for data problem
         continue;
      }
      //int lasteta=-999;
      //int lastphi=-999;
      //int lastchip=-999;
      
      for(p_rdo=PixelCollection->begin(); p_rdo!=PixelCollection->end(); ++p_rdo) 
      {
         rdoID=(*p_rdo)->identify();

	 //if((*p_rdo)->getToT()>10){

         if(m_doOnTrack || m_doOnPixelTrack) if(!OnTrack(rdoID,false) )continue; //if we only want hits on track, and the hit is NOT on the track, skip filling
         /////////////Start main fill block here///////////////
         //be sure to check each histo exists before filling it

	 if(m_occupancy) m_occupancy->Fill(rdoID, m_pixelid, m_doIBL);
         if(m_average_occupancy) m_average_occupancy->Fill(rdoID, m_pixelid, m_doIBL);
	 
	 if(m_Lvl1A) m_Lvl1A->Fill((*p_rdo)->getLVL1A());                 
         if(m_Lvl1ID) m_Lvl1ID->Fill((*p_rdo)->getLVL1ID());

         const EventInfo* thisEventInfo;
         sc=evtStore()->retrieve(thisEventInfo);
	 //EventID *thisEvent = thisEventInfo->event_ID();
	 
         if(sc != StatusCode::SUCCESS)
         {
	   if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endreq;
         }else{         
	   if(m_Lvl1ID_diff_mod_ATLAS) m_Lvl1ID_diff_mod_ATLAS->Fill((int)(((thisEventInfo->trigger_info()->extendedLevel1ID())&0xf) - (*p_rdo)->getLVL1ID()));
	   if(m_Lvl1ID_diff_mod_ATLAS_per_LB) m_Lvl1ID_diff_mod_ATLAS_per_LB->Fill(m_lumiBlockNum,rdoID,m_pixelid,(int)(((thisEventInfo->trigger_info()->extendedLevel1ID())&0xf) - (*p_rdo)->getLVL1ID()));
         }
         
         if(m_BCID) m_BCID->Fill((*p_rdo)->getBCID());
	 if(m_Atlas_BCID) m_Atlas_BCID->Fill(prev_pix_bcid); //defined at the start of the method
	 
         if(m_BCID_Profile) m_BCID_Profile->Fill(double(prev_pix_bcid),double(nhits));          
         if(m_diff_ROD_vs_Module_BCID) m_diff_ROD_vs_Module_BCID->Fill((prev_pix_bcid&0x000000ff)-(*p_rdo)->getBCID());

	 if(m_RodSim_FrontEnd_minus_Lvl1ID) m_RodSim_FrontEnd_minus_Lvl1ID->Fill(m_pixelCableSvc->getFE(&rdoID,rdoID) -(*p_rdo)->getLVL1ID());
         if(m_RodSim_BCID_minus_ToT) m_RodSim_BCID_minus_ToT->Fill((*p_rdo)->getBCID() - (*p_rdo)->getToT() );

/*	 if (m_doTiming) {
	   for (unsigned int j = 0; j < trigid.size(); j++) {   
	     if (m_trigger) m_trigger->Fill(trigid[j]);
	     if (m_pixelid->barrel_ec(rdoID)==2 && m_trigger_ECA)       m_trigger_ECA->Fill(trigid[j]);
	     else if (m_pixelid->barrel_ec(rdoID)==-2 && m_trigger_ECC) m_trigger_ECC->Fill(trigid[j]);
	     else if (m_pixelid->barrel_ec(rdoID)==0) {
	       if (m_pixelid->layer_disk(rdoID)==0 && m_trigger_B0)      m_trigger_B0->Fill(trigid[j]);
	       else if (m_pixelid->layer_disk(rdoID)==1 && m_trigger_B1) m_trigger_B1->Fill(trigid[j]);
	       else if (m_pixelid->layer_disk(rdoID)==2 && m_trigger_B2) m_trigger_B2->Fill(trigid[j]);
	     }
	   }
	 }*/

         //int ieta=m_pixelid->eta_index(rdoID);
         //int iphi=m_pixelid->phi_index(rdoID);
	 //int fechip=m_pixelCableSvc->getFE(&rdoID,rdoID);

	 //Changing to fill with every hit:
	 //if (fechip!=lastchip&& ieta!=lasteta && iphi !=lastphi) { 
	 if(m_FE_chip_hit_summary) m_FE_chip_hit_summary->Fill(m_pixelCableSvc->getFE(&rdoID,rdoID),rdoID,m_pixelid,m_doIBL);
	 //lastchip=fechip;
	 //lasteta=ieta;
	 //lastphi=iphi;
         //}

	 if(m_pixelid->barrel_ec(rdoID)==2) m_hit_ToT_ECA->Fill((*p_rdo)->getToT());
	 if(m_pixelid->barrel_ec(rdoID)==-2) m_hit_ToT_ECC->Fill((*p_rdo)->getToT());
	 if (m_pixelid->barrel_ec(rdoID)==0) {
	   if(m_doIBL && m_hit_ToT_IBL2D && m_pixelid->layer_disk(rdoID)==0 && m_pixelid->eta_module(rdoID)<6 && m_pixelid->eta_module(rdoID)>-7) m_hit_ToT_IBL2D->Fill((*p_rdo)->getToT());
	   if(m_doIBL && m_hit_ToT_IBL3D && m_pixelid->layer_disk(rdoID)==0 && !(m_pixelid->eta_module(rdoID)<6 && m_pixelid->eta_module(rdoID)>-7)) m_hit_ToT_IBL3D->Fill((*p_rdo)->getToT());
	   if(m_hit_ToT_B0 && m_pixelid->layer_disk(rdoID)==0+m_doIBL) m_hit_ToT_B0->Fill((*p_rdo)->getToT());
	   if(m_hit_ToT_B1 && m_pixelid->layer_disk(rdoID)==1+m_doIBL) m_hit_ToT_B1->Fill((*p_rdo)->getToT());
	   if(m_hit_ToT_B2 && m_pixelid->layer_disk(rdoID)==2+m_doIBL ) m_hit_ToT_B2->Fill((*p_rdo)->getToT());
	   if(m_doIBL && m_ToT_vs_eta_IBL && m_pixelid->layer_disk(rdoID)==0) m_ToT_vs_eta_IBL->Fill(m_pixelid->eta_module(rdoID),(*p_rdo)->getToT());
	   if(m_ToT_vs_eta_B0 && m_pixelid->layer_disk(rdoID)==0+m_doIBL) m_ToT_vs_eta_B0->Fill(m_pixelid->eta_module(rdoID),(*p_rdo)->getToT());
	   if(m_ToT_vs_eta_B1 && m_pixelid->layer_disk(rdoID)==1+m_doIBL) m_ToT_vs_eta_B1->Fill(m_pixelid->eta_module(rdoID),(*p_rdo)->getToT());
	   if(m_ToT_vs_eta_B2 && m_pixelid->layer_disk(rdoID)==2+m_doIBL ) m_ToT_vs_eta_B2->Fill(m_pixelid->eta_module(rdoID),(*p_rdo)->getToT());
	 }
	 
	 //Not yet modified for IBL geometry:
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

	 if(m_pixelid->barrel_ec(rdoID)==2)  nhits_ECA++;
	 if(m_pixelid->barrel_ec(rdoID)==-2) nhits_ECC++;
	 if (m_pixelid->barrel_ec(rdoID)==0) {
	   if(m_doIBL && m_pixelid->layer_disk(rdoID)==0) nhits_IBL++;
	   if(m_pixelid->layer_disk(rdoID)==0+m_doIBL) nhits_B0++;
	   if(m_pixelid->layer_disk(rdoID)==1+m_doIBL) nhits_B1++;
	   if(m_pixelid->layer_disk(rdoID)==2+m_doIBL) nhits_B2++;
	 }	 	 
         nhits++;
         nhitsM1++;
         nhitsM2++;
         nhitsM3++;
         nhitsM4++;

	 if (m_doModules)//fill module hit arrays so we can calculate the number of hits/event/module 
         {
	   if(m_pixelid->barrel_ec(rdoID)==2 ) m_HitPerEventArray_disksA[m_pixelid->phi_module(rdoID)][m_pixelid->layer_disk(rdoID)]++;
	    if(m_pixelid->barrel_ec(rdoID)==2){
	    }

	    if(m_pixelid->barrel_ec(rdoID)==-2) m_HitPerEventArray_disksC[m_pixelid->phi_module(rdoID)][m_pixelid->layer_disk(rdoID)]++;
	    if(m_pixelid->barrel_ec(rdoID)==-2){
	    }

	    if(m_pixelid->barrel_ec(rdoID)==0 )
            {
 	       if(m_doIBL && m_pixelid->layer_disk(rdoID)==0) m_HitPerEventArray_lI[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+10]++;
               if(m_pixelid->layer_disk(rdoID)==0+m_doIBL) m_HitPerEventArray_l0[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+6]++;
               if(m_pixelid->layer_disk(rdoID)==1+m_doIBL) m_HitPerEventArray_l1[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+6]++;
               if(m_pixelid->layer_disk(rdoID)==2+m_doIBL) m_HitPerEventArray_l2[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+6]++;
            } 
         }

         ///////////End of main fill block////////////////////
	 }
      //      }
   }//end of event loop

   ////////////////////Fill after event block////////////////
   if(m_doOnline)//should we fill these mid run or only at the end?
     {
       FillSummaryHistos(m_occupancy,m_ecA_occupancy_summary,m_ecC_occupancy_summary,m_bar_layI_occupancy_summary,m_bar_lay0_occupancy_summary,m_bar_lay1_occupancy_summary,m_bar_lay2_occupancy_summary);
       FillSummaryHistos(m_occupancy,m_ecA_occupancy_summary_low,m_ecC_occupancy_summary_low,m_bar_layI_occupancy_summary_low,m_bar_lay0_occupancy_summary_low,m_bar_lay1_occupancy_summary_low,m_bar_lay2_occupancy_summary_low);
     }//end of doOnline loop processing                                  
   

   if(m_hits_per_lumi)    m_hits_per_lumi->Fill(m_lumiBlockNum,nhits);     
   if(m_hits_per_lumi_ECA)m_hits_per_lumi_ECA->Fill(m_lumiBlockNum,nhits_ECA);
   if(m_hits_per_lumi_ECC)m_hits_per_lumi_ECC->Fill(m_lumiBlockNum,nhits_ECC);
   if(m_hits_per_lumi_IBL) m_hits_per_lumi_IBL->Fill(m_lumiBlockNum,nhits_IBL);
   if(m_hits_per_lumi_B0) m_hits_per_lumi_B0->Fill(m_lumiBlockNum,nhits_B0);
   if(m_hits_per_lumi_B1) m_hits_per_lumi_B1->Fill(m_lumiBlockNum,nhits_B1);
   if(m_hits_per_lumi_B2) m_hits_per_lumi_B2->Fill(m_lumiBlockNum,nhits_B2);

   if(m_occupancy_time1&&m_occupancy_time2&&m_occupancy_time3) FillTimeHisto(double(nhits/(1744.0+224*m_doIBL)),m_occupancy_time1, m_occupancy_time2, m_occupancy_time3,10.,60.,360. );

   if(m_Atlas_BCID_hits) m_Atlas_BCID_hits->Fill(prev_pix_bcid,nhits);

   //Fill some histograms only if =< 50% of modules disabled
   if (!m_majorityDisabled) {

     if(m_Details_mod1_num_hits) m_Details_mod1_num_hits->Fill(nhitsM1);
     if(m_Details_mod2_num_hits) m_Details_mod2_num_hits->Fill(nhitsM2);
     if(m_Details_mod3_num_hits) m_Details_mod3_num_hits->Fill(nhitsM3);
     if(m_Details_mod4_num_hits) m_Details_mod4_num_hits->Fill(nhitsM4);
     
     if(m_num_hits) m_num_hits->Fill(nhits);
     if(m_num_hits_low) m_num_hits_low->Fill(nhits);
     if(m_doLumiBlock && m_num_hits_LB) m_num_hits_LB->Fill(nhits);
     if(m_doLumiBlock && m_num_hits_low_LB) m_num_hits_low_LB->Fill(nhits);

     if (m_doModules) 
       {
	 PixelID::const_id_iterator idIt       = m_pixelid->wafer_begin();
	 PixelID::const_id_iterator idItEnd    = m_pixelid->wafer_end();
	 for (; idIt != idItEnd; ++idIt) 
	   {
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
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Starting to fill pixel granularity histograms." << endreq;   
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "This is very CPU and memory intensive and should not normmally be turned on" << endreq;   
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Please be patient, it will take a while to fill these histograms" << endreq;   

      if(m_pixel_noise_map) m_pixel_noise_map->Reset();
      if(m_pixel_occupancy_spectrum) m_pixel_occupancy_spectrum->Reset();

      PixelID::const_id_iterator idIt       = m_pixelid->wafer_begin();
      PixelID::const_id_iterator idItEnd    = m_pixelid->wafer_end();
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

               if(m_pixel_noise_map) m_pixel_noise_map->Fill(etaIndex,phiIndex,WaferID, m_pixelid,occ_spec,m_doIBL);   //fill the right pixel, give it pointers to the right module, and a weight of 'occ_spec'
               if(m_pixel_occupancy_spectrum) m_pixel_occupancy_spectrum->Fill(occ_spec, WaferID, m_pixelid, m_doIBL);   //fill the pixel occupancy and give a pointer to the right module.  Will fill 80M times total in this loop...
            }
         }
      }
   }

   if(m_doOffline) //run these only for offline
   {
     FillSummaryHistos(m_occupancy,m_ecA_occupancy_summary,m_ecC_occupancy_summary,m_bar_layI_occupancy_summary,m_bar_lay0_occupancy_summary,m_bar_lay1_occupancy_summary,m_bar_lay2_occupancy_summary);
     FillSummaryHistos(m_occupancy,m_ecA_occupancy_summary_low,m_ecC_occupancy_summary_low,m_bar_layI_occupancy_summary_low,m_bar_lay0_occupancy_summary_low,m_bar_lay1_occupancy_summary_low,m_bar_lay2_occupancy_summary_low);
   }

   if(m_average_occupancy) {  //scale to get average occupancies
     m_average_occupancy->Scale(events);  
   }
  return StatusCode::SUCCESS;
}

