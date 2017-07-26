/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms showing pixel hit level information
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include "PixelMonitoring/PixelMainMon.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TH3F.h"
#include "TProfile.h"

#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "PixelMonitoring/Components.h"
#include "PixelMonitoring/PixelMon2DLumiProfiles.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/PixelMonModules.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "PixelCabling/IPixelCablingSvc.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::bookHitsMon(void) {
  ATH_MSG_DEBUG("Start booking Hit histogtams..");
  std::string path = "Pixel/Hits";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/HitsOnTrack");
  MonGroup rdoShift(this, path.c_str(), run, ATTRIB_MANAGED);   // declare a group of histograms
  MonGroup rdoExpert(this, path.c_str(), run, ATTRIB_MANAGED);  // declare a group of histograms

  std::string pathT = "Pixel/Timing";
  if (m_doOnTrack) pathT.replace(pathT.begin(), pathT.end(), "Pixel/TimingOnTrack");
  MonGroup timeShift(this, pathT.c_str(), run, ATTRIB_MANAGED);   // declare a group of histograms
  MonGroup timeExpert(this, pathT.c_str(), run, ATTRIB_MANAGED);  // declare a group of histograms

  std::string tmp;
  std::string tmp2;
  std::string atitles;
  std::string hname;
  std::string htitles;

  int nbins_LB = m_lbRange;
  double min_LB = -0.5;
  double max_LB = min_LB + (1.0 * nbins_LB);
  int nbins_BCID = m_bcidRange;
  double min_BCID = -0.5;
  double max_BCID = min_BCID + (1.0 * nbins_BCID);
  int nbins_tot3 = 300;
  double min_tot3 = -0.5;
  double max_tot3 = min_tot3 + (1.0 * nbins_tot3);
  int nbins_tot4 = 20;
  double min_tot4 = -0.5;
  double max_tot4 = min_tot4 + (1.0 * nbins_tot4);

  static constexpr int nmod_eta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 20};

  std::string atext_LB = ";lumi block";
  std::string atext_BCID = ";BCID";
  std::string atext_nevt = ";# events";
  std::string atext_nhit = ";# hits";
  std::string atext_hit = ";# hits/event";
  std::string atext_occ = ";# hits/pixel/event";
  std::string atext_occ_mod = ";# hits/pixel";
  std::string atext_tot = ";ToT [BC]";
  std::string atext_lv1 = ";Level 1 Accept";

  StatusCode sc;

  hname = makeHistname("Hits_per_lumi", false);
  htitles = makeHisttitle("Average number of pixel hits per event", (atext_LB + atext_hit), false);
  sc = rdoShift.regHist(m_hits_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

  hname = makeHistname("AvgOcc_per_lumi", false);
  htitles = makeHisttitle("Average pixel occupancy per event over all layers", (atext_LB + atext_occ), false);
  sc = rdoShift.regHist(m_avgocc_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

  hname = makeHistname("AvgOcc_RatioIBLB0_per_lumi", false);
  htitles = makeHisttitle("Average pixel occupancy ratio of IBL/B0 per event per LB", (atext_LB + ";ratio"), false);
  sc = rdoShift.regHist(m_avgocc_ratioIBLB0_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

  for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {  // not include IBL2D and IBL3D
    hname = makeHistname(("Hits_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Average number of pixel hits per event, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_LB + atext_hit), false);
    sc = rdoExpert.regHist(m_hits_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

    hname = makeHistname(("nHits_per_module_per_event_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Number of hits in a module in an event, " + m_modLabel_PixLayerIBL2D3D[i]), ";#hits in a module in an event;#events #times #modules", false);
    sc = rdoShift.regHist(m_nhits_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 1000, -0.5, -0.5 + 1000.0));

    hname = makeHistname(("Hit_ToTMean_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Hit ToT Mean, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_LB + ";Average Hit ToT"), false);
    sc = rdoExpert.regHist(m_hit_ToTMean_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

    if (!m_doOnline) {
      hname = makeHistname(("ModOcc_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Module occupancy per event, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_LB + atext_occ_mod), false);
      sc = rdoShift.regHist(m_modocc_per_lumi[i] = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 100, 0.0001, 0.0101));
    }
  }

  for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) {
    if (!m_doIBL && i >= PixLayerIBL2D3D::kIBL) continue;

    hname = makeHistname(("AvgOcc_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Average pixel occupancy per event, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_LB + atext_occ), false);
    sc = rdoExpert.regHist(m_avgocc_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

    hname = makeHistname(("AvgOcc_per_BCID_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Average pixel occupancy per BCID, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_BCID + atext_occ), false);
    sc = rdoExpert.regHist(m_avgocc_per_bcid_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

    hname = makeHistname(("AvgOcc_active_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Average pixel occupancy for active per event, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_LB + atext_occ), false);
    sc = rdoExpert.regHist(m_avgocc_active_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

    hname = makeHistname(("MaxOcc_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Max. pixel occupancy per event, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_LB + atext_occ), false);
    sc = rdoShift.regHist(m_maxocc_per_lumi_mod[i] = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 500, 0.0001, 0.01));

    hname = makeHistname(("MaxOcc_per_BCID_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Max. pixel occupancy per BCID, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_BCID + atext_occ), false);
    sc = rdoShift.regHist(m_maxocc_per_bcid_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

    hname = makeHistname(("ToatlHits_per_BCID_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Total Number of hits per BCID, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_BCID + ";#hits"), false);
    sc = rdoExpert.regHist(m_totalhits_per_bcid_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

    hname = makeHistname(("nLargeEvent_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Number of large events (hitocc > 0.7#times 10^{-3}), " + m_modLabel_PixLayerIBL2D3D[i]), (atext_LB + atext_nevt), false);
    sc = rdoShift.regHist(m_nlargeevt_per_lumi_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));
  }

  for (int i = 0; i < PixLayerIBL2D3DDBM::COUNT; i++) {
    hname = makeHistname(("Hit_ToT_" + m_modLabel_PixLayerIBL2D3DDBM[i]), false);
    htitles = makeHisttitle(("Hit ToT, " + m_modLabel_PixLayerIBL2D3DDBM[i]), (atext_tot + atext_nhit), false);
    if (i < PixLayerIBL2D3DDBM::kDBMA) {
      sc = rdoExpert.regHist(m_hit_ToT[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot3, min_tot3, max_tot3));
    } else if (m_doIBL) {
      sc = rdoExpert.regHist(m_hit_ToT[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot4, min_tot4, max_tot4));
    }
  }

  m_hitmap_tmp = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("HitMap_tmp", ("Hit map for monitoring" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D));
  sc = m_hitmap_tmp->regHist(rdoShift);

  for (int i = 0; i < PixLayer::COUNT; i++) {
    hname = makeHistname(("nFEswithHits_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Number of FEs with hits, " + m_modLabel_PixLayerIBL2D3D[i]), ";lumi block;eta index of module;# FEs with hits in a module in an event;# event #times # modules", false);
    sc = rdoExpert.regHist(m_nFEswithHits_mod[i] = new TH3F(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, nmod_eta[i], -0.5, -0.5 + nmod_eta[i], 18, -0.5, 17.5));
  }

  if (m_doOnline) {
    sc = rdoShift.regHist(m_occupancy_time1 = new TProfile("occupancy_time_10min", ("Module hit occupancy as function of time over 10 minutes. 6 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99, 0., 1., "i"));
    sc = rdoShift.regHist(m_occupancy_time2 = new TProfile("occupancy_time_1hr", ("Module hit occupancy as function of time over 1 hour.  36 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99, 0., 1., "i"));
    sc = rdoShift.regHist(m_occupancy_time3 = new TProfile("occupancy_time_6hr", ("Module hit occupancy as function of time over 6 hours.  3.6 min/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99, 0., 1., "i"));

    m_hitmap_mon = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("HitMap_Mon", ("Hit map for monitoring" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D));
    sc = m_hitmap_mon->regHist(rdoShift);

    for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
      hname = makeHistname(("Hit_ToT_Mon_" + m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Hit ToT Monitoring, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_tot + atext_nhit), false);
      if (i < PixLayer::kIBL) {
        sc = rdoExpert.regHist(m_hit_ToT_Mon_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot3, min_tot3, max_tot3));
      } else {
        sc = rdoExpert.regHist(m_hit_ToT_Mon_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot4, min_tot4, max_tot4));
      }

      hname = makeHistname(("Hit_ToT_tmp_" + m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Hit ToT tmp, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_tot + atext_nhit), false);
      if (i < PixLayer::kIBL) {
        sc = rdoExpert.regHist(m_hit_ToT_tmp_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot3, min_tot3, max_tot3));
      } else {
        sc = rdoExpert.regHist(m_hit_ToT_tmp_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot4, min_tot4, max_tot4));
      }
    }
  }

  if (m_doTiming) {
    sc = timeExpert.regHist(m_Lvl1ID_PIX = TH1I_LW::create("LvlID_PIX", ("Level 1 ID (PIX)" + m_histTitleExt + ";level 1 ID" + atext_nhit).c_str(), 20, -0.5, 19.5));
    sc = timeExpert.regHist(m_Lvl1ID_IBL = TH1I_LW::create("LvlID_IBL", ("Level 1 ID (IBL)" + m_histTitleExt + ";level 1 ID" + atext_nhit).c_str(), 40, -0.5, 39.5));
    sc = timeShift.regHist(m_BCID = TH1I_LW::create("Pixel_BCID", ("BCID" + m_histTitleExt + ";Pixel BCID" + atext_nhit).c_str(), 300, -0.5, 299.5));
    sc = timeShift.regHist(m_Atlas_BCID = TH1F_LW::create("Atlas_BCID", ("BCID" + m_histTitleExt + ";ATLAS BCID" + atext_nhit).c_str(), 3500, -0.5, 3499.5));
    sc = timeShift.regHist(m_Atlas_BCID_hits = TH2F_LW::create("Atlas_BCID_Hits", ("BCID" + m_histTitleExt + ";BCID" + atext_nhit).c_str(), 3500, -0.5, 3499.5, 100, 0, 25000));
    sc = timeExpert.regHist(m_BCID_Profile = TProfile_LW::create("Atlas_BCID_Profile", ("BCID_Profile" + m_histTitleExt + ";BCID" + atext_nhit).c_str(), 3500, -0.5, 3499.5));
    sc = timeShift.regHist(m_Lvl1A = TH1F_LW::create("Lvl1A", ("Hit Level 1 Accept" + m_histTitleExt + ";Level 1 Accept" + atext_nhit).c_str(), 14, -1.5, 12.5));

    for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
      tmp = "ROD_Module_BCID_" + m_modLabel_PixLayerIBL2D3D[i];
      tmp2 = "Difference between BCID of RODs and Modules, " + m_modLabel_PixLayerIBL2D3D[i] + m_histTitleExt + ";BCID:ROD-Module (#bc) ;Number of Pixels";
      sc = timeExpert.regHist(m_diff_ROD_vs_Module_BCID_mod[i] = TH1I_LW::create(tmp.c_str(), tmp2.c_str(), 101, -50.5, 50.5));

      tmp = "Lvl1ID_diff_ATLAS_mod_" + m_modLabel_PixLayerIBL2D3D[i];
      tmp2 = "ATLAS_{Level 1 ID} - Module_{Level 1 ID}, " + m_modLabel_PixLayerIBL2D3D[i] + m_histTitleExt + ";#Delta Level 1 ID; # hits";
      sc = timeExpert.regHist(m_Lvl1ID_diff_mod_ATLAS_mod[i] = TH1I_LW::create(tmp.c_str(), tmp2.c_str(), 201, -200.5, 200.5));
    }

    for (int i = 0; i < PixLayerDBM::COUNT - 1 + (int)(m_doIBL); i++) {
      hname = makeHistname(("Lvl1A_" + m_modLabel_PixLayerDBM[i]), false);
      htitles = makeHisttitle(("Hit Level 1 Accept, " + m_modLabel_PixLayerDBM[i]), (atext_lv1 + atext_nhit), false);
      sc = timeShift.regHist(m_Lvl1A_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 14, -1.5, 12.5));
    }
  }

  if (m_do2DMaps) {
    m_occupancy = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("Occupancy", ("hit map" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D));
    sc = m_occupancy->regHist(rdoShift);

    m_average_pixocc = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("Occupancy_per_pixel", ("#hits / pixel" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D, false));
    sc = m_average_pixocc->regHist(rdoShift);

    m_occupancy_pix_evt = std::make_unique<PixelMon2DProfilesLW>(PixelMon2DProfilesLW("Occupancy_per_pixel_event", ("#hits / pixel / event" + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D));
    sc = m_occupancy_pix_evt->regHist(rdoShift);

    m_Lvl1ID_diff_mod_ATLAS_per_LB = std::make_unique<PixelMon2DLumiProfiles>(PixelMon2DLumiProfiles("Lvl1ID_diff_ATLAS_mod_per_LB", ("ATLAS_{Level 1 ID} - Module_{Level 1 ID} per LB" + m_histTitleExt).c_str(), "#Delta Level 1 ID", PixMon::HistConf::kPixIBL));
    sc = m_Lvl1ID_diff_mod_ATLAS_per_LB->regHist(timeExpert);

    m_Lvl1ID_absdiff_mod_ATLAS_per_LB = std::make_unique<PixelMon2DLumiProfiles>(PixelMon2DLumiProfiles("Lvl1ID_absdiff_ATLAS_mod_per_LB", ("ATLAS_{Level 1 ID} - Module_{Level 1 ID} per LB" + m_histTitleExt).c_str(), "#Delta Level 1 ID", PixMon::HistConf::kPixIBL));
    sc = m_Lvl1ID_absdiff_mod_ATLAS_per_LB->regHist(timeExpert);
  }

  if (m_doModules) {
    if (m_doOnTrack) {
      m_hiteff_mod = std::make_unique<PixelMonModulesProf>(PixelMonModulesProf("Hit_track_eff", ("Proportion of hits on track" + m_histTitleExt).c_str(), 2500, -0.5, 2499.5));
      sc = m_hiteff_mod->regHist(this, (path + "/Modules_HitEff").c_str(), run);
    }
    m_FE_chip_hit_summary = std::make_unique<PixelMonModules1D>(PixelMonModules1D("FE_Chip_Summary", ("FE Chip Summary" + m_histTitleExt).c_str(), 16, -0.5, 15.5));
    sc = m_FE_chip_hit_summary->regHist(this, (path + "/Modules_FEChipSummary").c_str(), run);
  }

  if (m_doLowOccupancy || m_doHighOccupancy) {
    int nbins_hits = 2000;
    float max_hits = 80000.0;
    int nbins_avhits = 100;
    float max_avhits = 100.0;

    if (m_doLowOccupancy) {
      nbins_hits = 200;
      max_hits = 200.0;
      nbins_avhits = 50;
      max_avhits = 2.0;
    }

    if (m_doHeavyIonMon) {
      max_hits = 350000;
      max_avhits = 2500;
    }

    sc = rdoShift.regHist(m_num_hits = TH1I_LW::create("num_hits", ("Number of pixel hits in an event" + m_histTitleExt + ";# pixel hits/event;# events").c_str(), nbins_hits, 0., max_hits));

    for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
      hname = makeHistname(("Occupancy_Summary_" + m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Average hit occupancy, " + m_modLabel_PixLayerIBL2D3D[i]), ";average # hits in a module in an event;# modules", false);
      sc = rdoExpert.regHist(m_occupancy_summary_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_avhits, 0., max_avhits));
    }
  }
  if (m_doPixelOccupancy) {
    m_pixel_occupancy = std::make_unique<PixelMonModules2D>(PixelMonModules2D("Pixel_Occupancy", ("Pixel Occupancy" + m_histTitleExt).c_str(), 160, -0., 160., 336, 0., 336.));
    sc = m_pixel_occupancy->regHist(this, (path + "/PixelOccupancy").c_str(), run);
  }

  if (m_doDetails) {
    // evaluate for specific modules
    sc = rdoExpert.regHist(m_Details_mod1_num_hits = TH1F_LW::create(("Details_num_hits_" + m_DetailsMod1).c_str(), ("number of pixel hits per event for mod1" + m_histTitleExt).c_str(), 100, -0., 100));
    sc = rdoExpert.regHist(m_Details_mod2_num_hits = TH1F_LW::create(("Details_num_hits_" + m_DetailsMod2).c_str(), ("number of pixel hits per event for mod2" + m_histTitleExt).c_str(), 100, -0., 100));
    sc = rdoExpert.regHist(m_Details_mod3_num_hits = TH1F_LW::create(("Details_num_hits_" + m_DetailsMod3).c_str(), ("number of pixel hits per event for mod3" + m_histTitleExt).c_str(), 100, -0., 100));
    sc = rdoExpert.regHist(m_Details_mod4_num_hits = TH1F_LW::create(("Details_num_hits_" + m_DetailsMod4).c_str(), ("number of pixel hits per event for mod4" + m_histTitleExt).c_str(), 100, -0., 100));
    // New ranges for 2 FEI4Bs:
    sc = rdoExpert.regHist(m_Details_mod1_occupancy = TH2F_LW::create(("Details_occupancy_" + m_DetailsMod1).c_str(), ("column vs row hitmap for mod1" + m_histTitleExt).c_str(), 160, -0.5, 155.5, 336, -0.5, 335.5));
    sc = rdoExpert.regHist(m_Details_mod2_occupancy = TH2F_LW::create(("Details_occupancy_" + m_DetailsMod2).c_str(), ("column vs row hitmap for mod2" + m_histTitleExt).c_str(), 160, -0.5, 155.5, 336, -0.5, 335.5));
    sc = rdoExpert.regHist(m_Details_mod3_occupancy = TH2F_LW::create(("Details_occupancy_" + m_DetailsMod3).c_str(), ("column vs row hitmap for mod3" + m_histTitleExt).c_str(), 160, -0.5, 155.5, 336, -0.5, 335.5));
    sc = rdoExpert.regHist(m_Details_mod4_occupancy = TH2F_LW::create(("Details_occupancy_" + m_DetailsMod4).c_str(), ("column vs row hitmap for mod4" + m_histTitleExt).c_str(), 160, -0.5, 155.5, 336, -0.5, 335.5));
    sc = rdoExpert.regHist(m_Details_mod1_ToT = TH1F_LW::create(("Details_ToT_" + m_DetailsMod1).c_str(), ("ToT mod1" + m_histTitleExt).c_str(), 300, -0.5, 299.5));
    sc = rdoExpert.regHist(m_Details_mod2_ToT = TH1F_LW::create(("Details_ToT_" + m_DetailsMod2).c_str(), ("ToT mod2" + m_histTitleExt).c_str(), 300, -0.5, 299.5));
    sc = rdoExpert.regHist(m_Details_mod3_ToT = TH1F_LW::create(("Details_ToT_" + m_DetailsMod3).c_str(), ("ToT mod3" + m_histTitleExt).c_str(), 300, -0.5, 299.5));
    sc = rdoExpert.regHist(m_Details_mod4_ToT = TH1F_LW::create(("Details_ToT_" + m_DetailsMod4).c_str(), ("ToT mod4" + m_histTitleExt).c_str(), 300, -0.5, 299.5));
  }

  if (sc.isFailure()) ATH_MSG_WARNING("Problems with booking Hit histograms");
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::bookHitsLumiBlockMon(void) {
  ATH_MSG_DEBUG("Start booking Hits histograms per LB (low stat)");

  std::string path = "Pixel/LumiBlock";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
  MonGroup lumiBlockHist(this, path.c_str(), lowStat, ATTRIB_MANAGED);  // declare a group of histograms

  std::string hname;
  std::string htitles;
  std::string atext_LB = ";lumi block";
  std::string atext_nevt = ";# events";
  std::string atext_nhit = ";# hits";
  std::string atext_hit = ";# hits/event";
  std::string atext_occ = ";# hits/pixel/event";
  std::string atext_tot = ";ToT [BC]";
  std::string atext_lv1 = ";Level 1 Accept";

  StatusCode sc;

  if (m_doLowOccupancy || m_doHighOccupancy) {
    int nbins_hits = 100;
    double min_hits = -0.5;
    double max_hits = min_hits + 25000.0;
    if (m_doLowOccupancy) {
      nbins_hits = 200;
      max_hits = min_hits + 200.0;
    }
    hname = makeHistname("num_hits_LB", false);
    htitles = makeHisttitle("Number of pixel hits in an event", (atext_hit + atext_nevt), false);
    sc = lumiBlockHist.regHist(m_num_hits_LB = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_hits, min_hits, max_hits));
  }
  if (m_doModules) {
    hname = makeHistname("num_Hits_mod_LB", false);
    htitles = makeHisttitle("Number of pixel hits in a module in an event", (atext_hit + atext_nevt), false);
    m_hit_num_mod_LB = std::make_unique<PixelMonModules1D>(PixelMonModules1D(hname.c_str(), htitles.c_str(), 20, -0.5, 19.5));
    sc = m_hit_num_mod_LB->regHist(this, (path + "/Modules_NumberOfHits").c_str(), lowStat);
  }
  for (int i = 0; i < PixLayer::COUNT; i++) {
    hname = makeHistname(("Hit_ToT_LB_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Hit ToT, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_tot + atext_nhit), false);
    if (i != PixLayer::kIBL) {
      sc = lumiBlockHist.regHist(m_hit_ToT_LB_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, 299.5));
    } else {
      sc = lumiBlockHist.regHist(m_hit_ToT_LB_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 20, -0.5, 19.5));
    }

    hname = makeHistname(("Lvl1A_10min_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Hit Level 1 Accept, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_lv1 + atext_nhit), false);
    sc = lumiBlockHist.regHist(m_Lvl1A_10min_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 14, -1.5, 12.5));
  }

  m_occupancy_10min = std::make_unique<PixelMon2DMapsLW>(PixelMon2DMapsLW("Occupancy_10min", ("hit occupancy" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D));
  sc = m_occupancy_10min->regHist(lumiBlockHist);

  if (sc.isFailure()) ATH_MSG_WARNING("Problems with booking Hit histograms per LB (low stat)");
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::fillHitsMon(void)  // Called once per event
{
  int DetailsMod1 = 0;
  int DetailsMod2 = 0;
  int DetailsMod3 = 0;
  int DetailsMod4 = 0;
  int nhitsM1 = 0;
  int nhitsM2 = 0;
  int nhitsM3 = 0;
  int nhitsM4 = 0;
  if (m_doDetails) {
    DetailsMod1 = parseDetailsString(m_DetailsMod1);
    DetailsMod2 = parseDetailsString(m_DetailsMod2);
    DetailsMod3 = parseDetailsString(m_DetailsMod3);
    DetailsMod4 = parseDetailsString(m_DetailsMod4);
  }

  static constexpr int nmod_phi[PixLayer::COUNT] = {48, 48, 22, 38, 52, 14};
  static constexpr int nmod_eta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 20};

  // Reset the centrally saved hit arrays back to zero.
  memset(m_HitPerEventArray_disksA, 0, sizeof(m_HitPerEventArray_disksA[0][0]) * nmod_phi[PixLayer::kECA] * nmod_eta[PixLayer::kECA]);
  memset(m_HitPerEventArray_disksC, 0, sizeof(m_HitPerEventArray_disksC[0][0]) * nmod_phi[PixLayer::kECC] * nmod_eta[PixLayer::kECC]);
  memset(m_HitPerEventArray_l0, 0, sizeof(m_HitPerEventArray_l0[0][0]) * nmod_phi[PixLayer::kB0] * nmod_eta[PixLayer::kB0]);
  memset(m_HitPerEventArray_l1, 0, sizeof(m_HitPerEventArray_l1[0][0]) * nmod_phi[PixLayer::kB1] * nmod_eta[PixLayer::kB1]);
  memset(m_HitPerEventArray_l2, 0, sizeof(m_HitPerEventArray_l2[0][0]) * nmod_phi[PixLayer::kB2] * nmod_eta[PixLayer::kB2]);
  memset(m_HitPerEventArray_lI, 0, sizeof(m_HitPerEventArray_lI[0][0]) * nmod_phi[PixLayer::kIBL] * nmod_eta[PixLayer::kIBL]);

  double nhits = 0;
  double nhits_mod[PixLayerIBL2D3D::COUNT] = {0};

  int fewithHits_EA[nmod_phi[PixLayer::kECA]][nmod_eta[PixLayer::kECA]][16];
  int fewithHits_EC[nmod_phi[PixLayer::kECC]][nmod_eta[PixLayer::kECC]][16];
  int fewithHits_B0[nmod_phi[PixLayer::kB0]][nmod_eta[PixLayer::kB0]][16];
  int fewithHits_B1[nmod_phi[PixLayer::kB1]][nmod_eta[PixLayer::kB1]][16];
  int fewithHits_B2[nmod_phi[PixLayer::kB2]][nmod_eta[PixLayer::kB2]][16];
  for (int i = 0; i < PixLayer::COUNT; i++) {
    for (int phi = 0; phi < nmod_phi[i]; phi++) {
      for (int eta = 0; eta < nmod_eta[i]; eta++) {
        for (int j = 0; j < 16; j++) {
          if (i == PixLayer::kECA) fewithHits_EA[phi][eta][j] = 0;
          if (i == PixLayer::kECC) fewithHits_EC[phi][eta][j] = 0;
          if (i == PixLayer::kB0) fewithHits_B0[phi][eta][j] = 0;
          if (i == PixLayer::kB1) fewithHits_B1[phi][eta][j] = 0;
          if (i == PixLayer::kB2) fewithHits_B2[phi][eta][j] = 0;
        }
      }
    }
  }

  Identifier rdoID;

  int nChannels_mod[PixLayerIBL2D3D::COUNT] = {46080, 46080, 46080, 46080, 46080, 26880, 53760, 26880};
  double inv_nChannels_mod[PixLayerIBL2D3D::COUNT];
  double nGoodChannels_total = 0.;
  double nGoodChannels_layer[PixLayerIBL2D3D::COUNT];
  double nActiveChannels_layer[PixLayerIBL2D3D::COUNT];
  for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) {
    inv_nChannels_mod[i] = 1.0 / (1.0 * nChannels_mod[i]);
    nGoodChannels_layer[i] = 1.0 * nChannels_mod[i] * m_nGood_mod[getPixLayerIDWithDBM(i)];
    nActiveChannels_layer[i] = 1.0 * nChannels_mod[i] * m_nActive_mod[getPixLayerIDWithDBM(i)];
    nGoodChannels_total = +nGoodChannels_layer[i];
  }

  StatusCode sc;

  const InDetTimeCollection* Pixel_BCIDColl = 0;
  if (evtStore()->contains<InDetTimeCollection>("PixelBCID")) sc = evtStore()->retrieve(Pixel_BCIDColl, "PixelBCID");
  int pix_rod_bcid = 0;
  if (!sc.isFailure() && Pixel_BCIDColl != 0) {
    ATH_MSG_DEBUG("Found Pixel BCID collection");
    for (InDetTimeCollection::const_iterator ipix_bcid = Pixel_BCIDColl->begin(); ipix_bcid != Pixel_BCIDColl->end(); ++ipix_bcid) {
      if (!(*ipix_bcid)) continue;
      const unsigned int pix_bcid = (*ipix_bcid)->second;
      pix_rod_bcid = pix_bcid;
    }
  }
  if (sc.isFailure()) ATH_MSG_WARNING("Could not find the data object PixelBCID !");

  int lvl1idATLAS(-1);
  const EventInfo* thisEventInfo;
  sc = evtStore()->retrieve(thisEventInfo);
  if (sc != StatusCode::SUCCESS) {
    ATH_MSG_WARNING("No EventInfo object found");
  } else {
    lvl1idATLAS = (int)((thisEventInfo->trigger_info()->extendedLevel1ID()) & 0xf);
  }

  // retrieve Pixel RDO container from storegate
  sc = evtStore()->retrieve(m_rdocontainer, m_Pixel_RDOName);
  if (sc.isFailure() || !m_rdocontainer) {
    ATH_MSG_WARNING("Could not retrieve Pixel RDO container !");
    if (m_storegate_errors) m_storegate_errors->Fill(1., 3.);  // first entry is for RDO, second is for retrieve problem
    return StatusCode::SUCCESS;                                // fail gracefully and keep going in the next tool
  } else {
    ATH_MSG_DEBUG("Pixel RDO container " << m_Pixel_RDOName << " found");
  }

  PixelRDO_Container::const_iterator colNext = m_rdocontainer->begin();
  PixelRDO_Container::const_iterator lastCol = m_rdocontainer->end();
  DataVector<PixelRDORawData>::const_iterator p_rdo;

  for (; colNext != lastCol; ++colNext) {
    const InDetRawDataCollection<PixelRDORawData>* PixelCollection(*colNext);
    if (!PixelCollection) {
      if (m_storegate_errors) m_storegate_errors->Fill(1., 5.);  // first entry is for RDO, second is for data problem
      continue;
    }

    for (p_rdo = PixelCollection->begin(); p_rdo != PixelCollection->end(); ++p_rdo) {
      rdoID = (*p_rdo)->identify();

      if (m_hiteff_mod) {
        if (isOnTrack(rdoID, false)) {
          m_hiteff_mod->fill(m_manager->lumiBlockNumber(), 1., rdoID, m_pixelid);
        } else {
          m_hiteff_mod->fill(m_manager->lumiBlockNumber(), 0., rdoID, m_pixelid);
        }
      }

      if (m_doOnTrack && !isOnTrack(rdoID, false)) {
        // if we only want hits on track, and the hit is NOT on the track, skip filling
        continue;
      }

      int pixlayer = getPixLayerID(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_doIBL);
      int pixlayerdbm = getPixLayerIDDBM(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_doIBL);
      int pixlayeribl2d3d = pixlayer;
      if (pixlayeribl2d3d == PixLayerIBL2D3D::kIBL) {
        pixlayeribl2d3d = getPixLayerIDIBL2D3D(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_pixelid->eta_module(rdoID), m_doIBL);
      }
      int pixlayeribl2d3ddbm = pixlayerdbm;
      if (pixlayeribl2d3ddbm == PixLayerDBM::kIBL) {
        pixlayeribl2d3ddbm = getPixLayerIDIBL2D3DDBM(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_pixelid->eta_module(rdoID), m_doIBL);
      }
      bool isIBL = false;
      if (m_pixelid->barrel_ec(rdoID) == 0 && m_doIBL && m_Lvl1A_mod[PixLayerDBM::kIBL] && m_pixelid->layer_disk(rdoID) == 0) isIBL = true;

      // Fill Occupancy
      if (m_occupancy) m_occupancy->fill(rdoID, m_pixelid);
      if (m_occupancy_10min && m_doLumiBlock) m_occupancy_10min->fill(rdoID, m_pixelid);
      if (m_hitmap_tmp) m_hitmap_tmp->fill(rdoID, m_pixelid);
      if (m_average_pixocc && pixlayeribl2d3d < PixLayerIBL2D3D::COUNT && nChannels_mod[pixlayeribl2d3d] > 0) {
        m_average_pixocc->fill(rdoID, m_pixelid, 1.0 / (1.0 * nChannels_mod[pixlayeribl2d3d]));
      }

      // Fill Lvl1A
      if (m_Lvl1A) {
        m_Lvl1A->Fill((*p_rdo)->getLVL1A());
        if (pixlayerdbm != 99 && m_Lvl1A_mod[pixlayerdbm]) m_Lvl1A_mod[pixlayerdbm]->Fill((*p_rdo)->getLVL1A());
      }
      if (pixlayer != 99 && m_Lvl1A_10min_mod[pixlayer] && m_doLumiBlock) m_Lvl1A_10min_mod[pixlayer]->Fill((*p_rdo)->getLVL1A());
      if (isIBL && m_Lvl1ID_IBL) m_Lvl1ID_IBL->Fill((*p_rdo)->getLVL1ID() % 32);
      if (!isIBL && m_Lvl1ID_PIX) m_Lvl1ID_PIX->Fill((*p_rdo)->getLVL1ID());

      // Fill difference of Lvl1
      int lvl1idMOD = (int)(*p_rdo)->getLVL1ID();
      int difflvl1id = lvl1idATLAS - lvl1idMOD % 32;
      if (pixlayer != 99 && m_Lvl1ID_diff_mod_ATLAS_mod[pixlayer]) m_Lvl1ID_diff_mod_ATLAS_mod[pixlayer]->Fill(difflvl1id);

      if (m_Lvl1ID_diff_mod_ATLAS_per_LB) m_Lvl1ID_diff_mod_ATLAS_per_LB->fill(m_manager->lumiBlockNumber(), rdoID, m_pixelid, difflvl1id);
      if (m_Lvl1ID_absdiff_mod_ATLAS_per_LB) m_Lvl1ID_absdiff_mod_ATLAS_per_LB->fill(m_manager->lumiBlockNumber(), rdoID, m_pixelid, fabs(difflvl1id));

      // Fill BCID
      if (m_BCID) m_BCID->Fill((*p_rdo)->getBCID());
      if (m_Atlas_BCID) m_Atlas_BCID->Fill(pix_rod_bcid);
      if (m_BCID_Profile) m_BCID_Profile->Fill(double(pix_rod_bcid), double(nhits));
      if (pixlayer != 99 && m_diff_ROD_vs_Module_BCID_mod[pixlayer]) m_diff_ROD_vs_Module_BCID_mod[pixlayer]->Fill((pix_rod_bcid & 0x000000ff) - (*p_rdo)->getBCID());

      if (m_FE_chip_hit_summary) m_FE_chip_hit_summary->fill(m_pixelCableSvc->getFE(&rdoID, rdoID), rdoID, m_pixelid);

      // Fill ToT
      if (pixlayeribl2d3ddbm != 99 && m_hit_ToT[pixlayeribl2d3ddbm]) m_hit_ToT[pixlayeribl2d3ddbm]->Fill((*p_rdo)->getToT());
      if (pixlayerdbm == PixLayerDBM::kIBL && m_hit_ToT[pixlayerdbm]) m_hit_ToT[pixlayerdbm]->Fill((*p_rdo)->getToT());

      if (pixlayer != 99 && m_hit_ToTMean_mod[pixlayer]) m_hit_ToTMean_mod[pixlayer]->Fill(m_manager->lumiBlockNumber(), (*p_rdo)->getToT());
      if (m_doLumiBlock && pixlayer != 99 && m_hit_ToT_LB_mod[pixlayer]) {
        m_hit_ToT_LB_mod[pixlayer]->Fill((*p_rdo)->getToT());
      }

      if (m_doOnline && pixlayer != 99 && m_hit_ToT_tmp_mod[pixlayer]) {
        m_hit_ToT_tmp_mod[pixlayer]->Fill((*p_rdo)->getToT());
      }

      // Not yet modified for IBL geometry:
      if (m_doDetails) {
        int currentID = 1000000 + (-m_pixelid->barrel_ec(rdoID) + 2) * 100000 + (m_pixelid->layer_disk(rdoID)) * 10000 + (m_pixelid->phi_module(rdoID)) * 100 + (m_pixelid->eta_module(rdoID) + 6);
        if (m_Details_mod1_occupancy && currentID == DetailsMod1) m_Details_mod1_occupancy->Fill(m_pixelid->eta_index(rdoID), m_pixelid->phi_index(rdoID));
        if (m_Details_mod2_occupancy && currentID == DetailsMod2) m_Details_mod2_occupancy->Fill(m_pixelid->eta_index(rdoID), m_pixelid->phi_index(rdoID));
        if (m_Details_mod3_occupancy && currentID == DetailsMod3) m_Details_mod3_occupancy->Fill(m_pixelid->eta_index(rdoID), m_pixelid->phi_index(rdoID));
        if (m_Details_mod4_occupancy && currentID == DetailsMod4) m_Details_mod4_occupancy->Fill(m_pixelid->eta_index(rdoID), m_pixelid->phi_index(rdoID));
        if (m_Details_mod1_ToT && currentID == DetailsMod1) m_Details_mod1_ToT->Fill((*p_rdo)->getToT());
        if (m_Details_mod2_ToT && currentID == DetailsMod2) m_Details_mod2_ToT->Fill((*p_rdo)->getToT());
        if (m_Details_mod3_ToT && currentID == DetailsMod3) m_Details_mod3_ToT->Fill((*p_rdo)->getToT());
        if (m_Details_mod4_ToT && currentID == DetailsMod4) m_Details_mod4_ToT->Fill((*p_rdo)->getToT());
      }
      if (m_pixel_occupancy) m_pixel_occupancy->fill(m_pixelid->eta_index(rdoID), m_pixelid->phi_index(rdoID), rdoID, m_pixelid);

      if (pixlayer != 99) nhits_mod[pixlayer]++;
      nhits++;
      nhitsM1++;
      nhitsM2++;
      nhitsM3++;
      nhitsM4++;

      if (m_pixelid->barrel_ec(rdoID) == 2) m_HitPerEventArray_disksA[m_pixelid->phi_module(rdoID)][m_pixelid->layer_disk(rdoID)]++;
      if (m_pixelid->barrel_ec(rdoID) == -2) m_HitPerEventArray_disksC[m_pixelid->phi_module(rdoID)][m_pixelid->layer_disk(rdoID)]++;
      if (m_pixelid->barrel_ec(rdoID) == 0) {
        if (m_doIBL && m_pixelid->layer_disk(rdoID) == 0) m_HitPerEventArray_lI[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID) + 10]++;
        if (m_pixelid->layer_disk(rdoID) == 0 + m_doIBL) m_HitPerEventArray_l0[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID) + 6]++;
        if (m_pixelid->layer_disk(rdoID) == 1 + m_doIBL) m_HitPerEventArray_l1[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID) + 6]++;
        if (m_pixelid->layer_disk(rdoID) == 2 + m_doIBL) m_HitPerEventArray_l2[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID) + 6]++;
      }

      // Quick Status
      int fephi = 0;
      int feeta = 0;
      if (pixlayer == PixLayer::kB0 && getFEID(pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta)) {
        fewithHits_B0[m_pixelid->phi_module(rdoID)][(int)(fabs(6 + m_pixelid->eta_module(rdoID)))][(int)((8 * fephi) + feeta)] = 1;
      }
      if (pixlayer == PixLayer::kB1 && getFEID(pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta)) {
        fewithHits_B1[m_pixelid->phi_module(rdoID)][(int)(fabs(6 + m_pixelid->eta_module(rdoID)))][(int)((8 * fephi) + feeta)] = 1;
      }
      if (pixlayer == PixLayer::kB2 && getFEID(pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta)) {
        fewithHits_B2[m_pixelid->phi_module(rdoID)][(int)(fabs(6 + m_pixelid->eta_module(rdoID)))][(int)((8 * fephi) + feeta)] = 1;
      }
      if (pixlayer == PixLayer::kECA && getFEID(pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta)) {
        fewithHits_EA[m_pixelid->phi_module(rdoID)][(int)m_pixelid->layer_disk(rdoID)][(int)((8 * fephi) + feeta)] = 1;
      }
      if (pixlayer == PixLayer::kECC && getFEID(pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta)) {
        fewithHits_EC[m_pixelid->phi_module(rdoID)][(int)m_pixelid->layer_disk(rdoID)][(int)((8 * fephi) + feeta)] = 1;
      }
    }
  }  // end of RDO (hit) loop

  if (m_doOnline) {
    fillSummaryHistos(m_occupancy.get(),
                      m_occupancy_summary_mod[PixLayer::kECA],
                      m_occupancy_summary_mod[PixLayer::kECC],
                      m_occupancy_summary_mod[PixLayer::kIBL],
                      m_occupancy_summary_mod[PixLayer::kB0],
                      m_occupancy_summary_mod[PixLayer::kB1],
                      m_occupancy_summary_mod[PixLayer::kB2]);
    if (m_occupancy_time1 && m_occupancy_time2 && m_occupancy_time3) {
      fillTimeHisto(double(nhits / (1744.0 + 280 * m_doIBL)), m_occupancy_time1, m_occupancy_time2, m_occupancy_time3, 10., 60., 360.);
    }
    if (m_doRefresh) {
      for (int i = 0; i < PixLayer::COUNT; i++) {
        if (m_hit_ToT_Mon_mod[i] && m_hit_ToT_tmp_mod[i]) th1FillMonitoring(m_hit_ToT_Mon_mod[i], m_hit_ToT_tmp_mod[i]);
      }
      if (m_hitmap_tmp && m_hitmap_mon) {
        if (m_occupancy_pix_evt) m_occupancy_pix_evt->fillFromMap(m_hitmap_tmp.get(), false);
        m_hitmap_mon->fill2DMon(m_hitmap_tmp.get());
      }
    }
  }

  if (m_hits_per_lumi) m_hits_per_lumi->Fill(m_manager->lumiBlockNumber(), nhits);
  for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
    if (m_hits_per_lumi_mod[i]) m_hits_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), nhits_mod[i]);
  }

  if (!m_doOnline && m_occupancy_pix_evt && m_hitmap_tmp) m_occupancy_pix_evt->fillFromMap(m_hitmap_tmp.get(), true);

  double avgocc = 0;
  double avgocc_mod[PixLayerIBL2D3D::COUNT] = {0};
  double avgocc_active_mod[PixLayerIBL2D3D::COUNT] = {0};
  if (nGoodChannels_total > 0) avgocc = nhits / nGoodChannels_total;
  if (m_avgocc_per_lumi) m_avgocc_per_lumi->Fill(m_manager->lumiBlockNumber(), avgocc);

  for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) {
    if (nGoodChannels_layer[i] > 0) avgocc_mod[i] = nhits_mod[i] / nGoodChannels_layer[i];
    if (nActiveChannels_layer[i] > 0) avgocc_active_mod[i] = nhits_mod[i] / nActiveChannels_layer[i];

    if (m_avgocc_per_lumi_mod[i]) m_avgocc_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), avgocc_mod[i]);
    if (m_avgocc_per_bcid_mod[i]) m_avgocc_per_bcid_mod[i]->Fill(pix_rod_bcid, avgocc_mod[i]);
    if (m_avgocc_active_per_lumi_mod[i]) m_avgocc_active_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), avgocc_active_mod[i]);

    if (m_maxocc_per_lumi_mod[i]) m_maxocc_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), avgocc_active_mod[i]);
    if (m_maxocc_per_bcid_mod[i]) {
      int bin = m_maxocc_per_bcid_mod[i]->GetXaxis()->FindBin(1.0 * pix_rod_bcid);
      double content = m_maxocc_per_bcid_mod[i]->GetBinContent(bin);
      if (avgocc_mod[i] > content) m_maxocc_per_bcid_mod[i]->SetBinContent(bin, avgocc_mod[i]);
    }
    if (m_totalhits_per_bcid_mod[i]) m_totalhits_per_bcid_mod[i]->Fill(1.0 * pix_rod_bcid, nhits_mod[i]);
    if (avgocc_mod[i] > 0.0007 && m_nlargeevt_per_lumi_mod[i]) m_nlargeevt_per_lumi_mod[i]->Fill(m_lumiBlockNum);
  }

  if (avgocc_mod[PixLayer::kB0] > 0 && m_avgocc_ratioIBLB0_per_lumi) m_avgocc_ratioIBLB0_per_lumi->Fill(m_manager->lumiBlockNumber(), avgocc_mod[PixLayer::kIBL] / avgocc_mod[PixLayer::kB0]);

  if (m_Atlas_BCID_hits) m_Atlas_BCID_hits->Fill(pix_rod_bcid, nhits);

  // Fill the #hit per module per event
  for (int i = 0; i < PixLayer::COUNT; i++) {
    if (m_nhits_mod[i]) {
      for (int phi = 0; phi < nmod_phi[i]; phi++) {
        for (int eta = 0; eta < nmod_eta[i]; eta++) {
          if (i == PixLayer::kECA) m_nhits_mod[i]->Fill(m_HitPerEventArray_disksA[phi][eta]);
          if (i == PixLayer::kECC) m_nhits_mod[i]->Fill(m_HitPerEventArray_disksC[phi][eta]);
          if (i == PixLayer::kB0) m_nhits_mod[i]->Fill(m_HitPerEventArray_l0[phi][eta]);
          if (i == PixLayer::kB1) m_nhits_mod[i]->Fill(m_HitPerEventArray_l1[phi][eta]);
          if (i == PixLayer::kB2) m_nhits_mod[i]->Fill(m_HitPerEventArray_l2[phi][eta]);
          if (i == PixLayer::kIBL) m_nhits_mod[i]->Fill(m_HitPerEventArray_lI[phi][eta]);
        }
      }
    }
  }

  if (!m_doOnline) {
    for (int i = 0; i < PixLayer::COUNT; i++) {
      if (m_modocc_per_lumi[i]) {
        for (int phi = 0; phi < nmod_phi[i]; phi++) {
          for (int eta = 0; eta < nmod_eta[i]; eta++) {
            if (i == PixLayer::kECA) m_modocc_per_lumi[i]->Fill(m_manager->lumiBlockNumber(), m_HitPerEventArray_disksA[phi][eta] * inv_nChannels_mod[i]);
            if (i == PixLayer::kECC) m_modocc_per_lumi[i]->Fill(m_manager->lumiBlockNumber(), m_HitPerEventArray_disksC[phi][eta] * inv_nChannels_mod[i]);
            if (i == PixLayer::kB0) m_modocc_per_lumi[i]->Fill(m_manager->lumiBlockNumber(), m_HitPerEventArray_l0[phi][eta] * inv_nChannels_mod[i]);
            if (i == PixLayer::kB1) m_modocc_per_lumi[i]->Fill(m_manager->lumiBlockNumber(), m_HitPerEventArray_l1[phi][eta] * inv_nChannels_mod[i]);
            if (i == PixLayer::kB2) m_modocc_per_lumi[i]->Fill(m_manager->lumiBlockNumber(), m_HitPerEventArray_l2[phi][eta] * inv_nChannels_mod[i]);
            if (i == PixLayer::kIBL) {
              if (eta < 4 || eta > 15) {
                m_modocc_per_lumi[i]->Fill(m_manager->lumiBlockNumber(), m_HitPerEventArray_lI[phi][eta] * inv_nChannels_mod[i]);
              } else {
                m_modocc_per_lumi[i]->Fill(m_manager->lumiBlockNumber(), m_HitPerEventArray_lI[phi][eta] * inv_nChannels_mod[i + 1]);
              }
            }
          }
        }
      }
    }
  }

  // Fill these histograms only if =< 50% of modules disabled
  if (!m_majorityDisabled) {
    if (m_doDetails) {
      if (m_Details_mod1_num_hits) m_Details_mod1_num_hits->Fill(nhitsM1);
      if (m_Details_mod2_num_hits) m_Details_mod2_num_hits->Fill(nhitsM2);
      if (m_Details_mod3_num_hits) m_Details_mod3_num_hits->Fill(nhitsM3);
      if (m_Details_mod4_num_hits) m_Details_mod4_num_hits->Fill(nhitsM4);
    }
    if (m_num_hits) m_num_hits->Fill(nhits);
    if (m_doLumiBlock && m_num_hits_LB) {
      m_num_hits_LB->Fill(nhits);
    }

    if (m_doModules) {
      PixelID::const_id_iterator idIt = m_pixelid->wafer_begin();
      PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();
      for (; idIt != idItEnd; ++idIt) {
        Identifier WaferID = *idIt;
        if (m_doLumiBlock) {
          if (m_pixelid->barrel_ec(WaferID) == 2) m_hit_num_mod_LB->fill(m_HitPerEventArray_disksA[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid);
          if (m_pixelid->barrel_ec(WaferID) == -2) m_hit_num_mod_LB->fill(m_HitPerEventArray_disksC[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid);
          if (m_pixelid->barrel_ec(WaferID) == 0) {
            if (m_doIBL && m_pixelid->layer_disk(WaferID) == 0) m_hit_num_mod_LB->fill(m_HitPerEventArray_lI[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 10], WaferID, m_pixelid);
            if (m_pixelid->layer_disk(WaferID) == 0 + m_doIBL) m_hit_num_mod_LB->fill(m_HitPerEventArray_l0[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 6], WaferID, m_pixelid);
            if (m_pixelid->layer_disk(WaferID) == 1 + m_doIBL) m_hit_num_mod_LB->fill(m_HitPerEventArray_l1[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 6], WaferID, m_pixelid);
            if (m_pixelid->layer_disk(WaferID) == 2 + m_doIBL) m_hit_num_mod_LB->fill(m_HitPerEventArray_l2[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 6], WaferID, m_pixelid);
          }
        }
      }
    }  // if (m_doModules)
  }    // if (!m_majorityDisabled)

  for (int i = 0; i < PixLayer::COUNT; i++) {
    for (int phi = 0; phi < nmod_phi[i]; phi++) {
      for (int eta = 0; eta < nmod_eta[i]; eta++) {
        int nfes = 0;
        for (int j = 0; j < 16; j++) {
          if (i == PixLayer::kECA) nfes += fewithHits_EA[phi][eta][j];
          if (i == PixLayer::kECC) nfes += fewithHits_EC[phi][eta][j];
          if (i == PixLayer::kB0) nfes += fewithHits_B0[phi][eta][j];
          if (i == PixLayer::kB1) nfes += fewithHits_B1[phi][eta][j];
          if (i == PixLayer::kB2) nfes += fewithHits_B2[phi][eta][j];
        }
        if (m_nFEswithHits_mod[i]) m_nFEswithHits_mod[i]->Fill(m_manager->lumiBlockNumber(), eta, nfes);
      }
    }
  }

  if (nhits == 0 && m_storegate_errors) m_storegate_errors->Fill(1., 4.);  // first entry for RDO, second for size = 0

  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::procHitsMon(void) {

  if (m_doOffline) {
    fillSummaryHistos(m_occupancy.get(),
                      m_occupancy_summary_mod[PixLayer::kECA],
                      m_occupancy_summary_mod[PixLayer::kECC],
                      m_occupancy_summary_mod[PixLayer::kIBL],
                      m_occupancy_summary_mod[PixLayer::kB0],
                      m_occupancy_summary_mod[PixLayer::kB1],
                      m_occupancy_summary_mod[PixLayer::kB2]);
  }

  return StatusCode::SUCCESS;
}
